# Phoenix PayPal Cash servisi — FastAPI uygulaması.
#
# Akış:
#   GET  /pay/?token=...  -> OneTimeKey doğrulama + paket seçim sayfası
#   POST /pay/create      -> PayPal order yaratma (fiyat her zaman DB'den) + approval redirect
#   GET  /pay/return      -> capture + ATOMİK kredi (PENDING->COMPLETED guard)
#   GET  /pay/cancel      -> PENDING->CANCELLED (tek yönlü)
#   POST /paypal/webhook  -> imza doğrulama + idempotent kredi
#
# Güvenlik: imza doğrulanmadan ASLA kredi yazılmaz; amount/currency mismatch'te
# kredi YOK + FAILED + log; return ve webhook akışları status guard'ı sayesinde
# çift kredi yazamaz.

import hmac
import json
import logging
import re
from datetime import datetime, timezone
from decimal import Decimal
from pathlib import Path
from typing import Optional

from fastapi import FastAPI, Form, Query, Request
from fastapi.responses import HTMLResponse, JSONResponse, RedirectResponse
from fastapi.templating import Jinja2Templates

from . import db, paypal_api
from .config import config
from .paypal_api import PayPalAPIError, to_cents

# --- Logging: dosya + stdout. Log dosyası açılamazsa stdout'a düşülür (uyarıyla). ---
_log_handlers = [logging.StreamHandler()]
try:
    _log_handlers.append(logging.FileHandler(config.log_file, encoding="utf-8"))
except OSError as exc:
    print(f"WARNING: log file '{config.log_file}' cannot be opened ({exc}); stdout only")

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s %(levelname)s %(name)s %(message)s",
    handlers=_log_handlers,
)
logger = logging.getLogger("paypal_service")

# docs/redoc public payment endpoint'inde kapalı tutulur (API şeması ifşa edilmez)
app = FastAPI(title="Phoenix PayPal Cash Service", docs_url=None, redoc_url=None)
templates = Jinja2Templates(directory=str(Path(__file__).resolve().parent / "templates"))

_TOKEN_RE = re.compile(r"^[A-Za-z0-9]{1,64}$")
_WEBHOOK_MAX_BODY = 64 * 1024        # tasarım §5-6: webhook body limiti
_TRANSMISSION_MAX_SKEW_SEC = 300     # tasarım §5-10: replay penceresi ±5 dk


# ---------------------------------------------------------------------------
# Yardımcılar
# ---------------------------------------------------------------------------

def _error_page(request: Request, title: str, message: str,
                status_code: int = 400) -> HTMLResponse:
    return templates.TemplateResponse(
        request, "result.html",
        {"state": "error", "title": title, "message": message},
        status_code=status_code,
    )


def _info_page(request: Request, title: str, message: str,
               state: str = "info") -> HTMLResponse:
    return templates.TemplateResponse(
        request, "result.html",
        {"state": state, "title": title, "message": message},
    )


def _validate_token(token: str) -> Optional[dict]:
    """OneTimeKey'i doğrular: var, PENDING, süresi geçmemiş.

    Geçersizse None döner; her ret sebebi loglanır (sessiz ret yok).
    """
    if not token or not _TOKEN_RE.match(token):
        logger.warning("token rejected: malformed")
        return None
    txn = db.fetch_transaction_by_token(token)
    if txn is None:
        logger.warning("token rejected: not found")
        return None
    # DB eşleşmesine ek olarak timing-safe karşılaştırma (derinlemesine savunma)
    if not hmac.compare_digest(str(txn["OneTimeKey"]), token):
        logger.warning("token rejected: digest mismatch txn=%s", txn["ID"])
        return None
    if txn["Status"] != "PENDING":
        logger.info("token rejected: status=%s txn=%s", txn["Status"], txn["ID"])
        return None
    # KeyExpiresAt MySQL sunucu saatiyle yazılır; servis aynı hostta çalışır (VPS).
    if txn["KeyExpiresAt"] is not None and txn["KeyExpiresAt"] <= datetime.now():
        logger.info("token rejected: expired txn=%s", txn["ID"])
        return None
    return txn


def _find_txn(order_id: Optional[str], custom_id: str) -> Optional[dict]:
    """Webhook/return eşleştirmesi: önce OrderID, olmazsa custom_id (=TxnID)."""
    if order_id:
        txn = db.fetch_transaction_by_order_id(order_id)
        if txn is not None:
            return txn
    if custom_id and custom_id.isdigit():
        return db.fetch_transaction_by_id(int(custom_id))
    return None


# ---------------------------------------------------------------------------
# Oyuncuya dönük sayfalar
# ---------------------------------------------------------------------------

@app.get("/pay/", response_class=HTMLResponse)
async def pay_landing(request: Request, token: str = Query(default="")):
    txn = _validate_token(token)
    if txn is None:
        return _error_page(
            request,
            "Invalid or expired link",
            "This payment link is invalid, already used, or expired. "
            "Please open the marketplace in game and request a new one.",
        )
    packages = db.fetch_active_packages()
    return templates.TemplateResponse(
        request, "packages.html",
        {"token": token, "packages": packages, "currency": config.currency},
    )


@app.post("/pay/create")
async def pay_create(request: Request, token: str = Form(...), package_id: int = Form(...)):
    txn = _validate_token(token)
    if txn is None:
        return _error_page(
            request,
            "Invalid or expired link",
            "This payment link is invalid, already used, or expired. "
            "Please open the marketplace in game and request a new one.",
        )

    # Paket ve fiyat HER ZAMAN DB'den — client'tan gelen tek şey PackageID'dir.
    package = db.fetch_package(package_id)
    if package is None:
        logger.warning("package not found or inactive: id=%s txn=%s", package_id, txn["ID"])
        return _error_page(
            request,
            "Package unavailable",
            "The selected package is no longer available. Please go back and choose another one.",
        )

    # Refresh güvenliği / token başına tek order: bu transaction için order zaten
    # yaratıldıysa YENİ order YARATILMAZ; mevcut approval URL'sine (order_id'den
    # türetilir) yönlendirilir.
    if txn["OrderID"]:
        approve_url = f"{config.paypal_web_base}/checkoutnow?token={txn['OrderID']}"
        logger.info("txn=%s already has order %s — redirecting to existing approval",
                    txn["ID"], txn["OrderID"])
        return RedirectResponse(approve_url, status_code=303)

    currency = (package.get("Currency") or config.currency).upper()
    try:
        order_id, approve_url = await paypal_api.create_order(
            amount_dollar=int(package["AmountDollar"]),
            currency=currency,
            custom_id=str(txn["ID"]),
            return_url=f"{config.base_url}/pay/return",
            cancel_url=f"{config.base_url}/pay/cancel",
        )
    except PayPalAPIError as exc:
        logger.error("order create failed txn=%s: %s", txn["ID"], exc)
        return _error_page(
            request,
            "Payment provider error",
            "The payment could not be initiated. Please try again later.",
            status_code=502,
        )

    if not db.attach_order(
        txn_id=txn["ID"],
        package_id=package["PackageID"],
        # PayPalCashPackages.AmountDollar tam dolar (5 = $5); HowMuchPay USD DECIMAL snapshot
        how_much_pay=Decimal(int(package["AmountDollar"])),
        currency=currency,
        cash_amount=int(package["CashAmount"]),
        order_id=order_id,
    ):
        # Eşzamanlı istek durumu değiştirmiş (örn. iptal). Yetim order capture
        # edilmedikçe ücretlendirme yapmaz; oyuncu akışı yeniden başlatır.
        logger.error("attach_order lost race txn=%s order=%s", txn["ID"], order_id)
        return _error_page(
            request,
            "Session state changed",
            "This payment session has changed state. Please restart the payment from the game.",
            status_code=409,
        )

    logger.info("order created txn=%s order=%s package=%s amount=%s %s",
                txn["ID"], order_id, package["PackageID"],
                package["AmountDollar"], currency)
    return RedirectResponse(approve_url, status_code=303)


@app.get("/pay/return", response_class=HTMLResponse)
async def pay_return(request: Request, token: str = Query(default="")):
    # PayPal return URL'e order id'yi 'token' query parametresi olarak ekler.
    order_id = token.strip()
    if not order_id:
        return _error_page(request, "Missing order reference",
                           "The payment reference is missing. If you were charged, contact support.")

    txn = db.fetch_transaction_by_order_id(order_id)
    if txn is None:
        logger.error("return: unknown order %s", order_id)
        return _error_page(request, "Unknown payment",
                           "This payment could not be matched. If you were charged, contact support.")

    if txn["Status"] == "COMPLETED":
        # Webhook çoktan kredi yazmış olabilir — idempotent başarı sayfası.
        return _info_page(request, "Payment completed",
                          "Your payment has been completed and the Cash has been credited. "
                          "Reopen the marketplace in game to see your new balance.",
                          state="success")
    if txn["Status"] != "PENDING":
        logger.info("return: order %s in status %s", order_id, txn["Status"])
        return _info_page(request, "Payment not active",
                          "This payment is no longer active. You can close this page and return to the game.")

    try:
        capture = await paypal_api.capture_order(order_id)
    except PayPalAPIError as exc:
        logger.error("capture failed order=%s txn=%s: %s", order_id, txn["ID"], exc)
        return _error_page(request, "Could not confirm payment",
                           "The payment could not be confirmed. If you were charged, contact support.",
                           status_code=502)

    if capture["status"] != "COMPLETED":
        # Örn. echeck ile PENDING: kredi YOK; ödeme clearing'den sonra webhook tamamlar.
        logger.info("capture not completed order=%s status=%s reason=%s",
                    order_id, capture["status"], capture.get("pending_reason") or "-")
        return _info_page(request, "Payment is being processed",
                          "Your payment is being processed by PayPal. The Cash will be credited "
                          "automatically once the payment clears.")

    # Tutar/para birimi/custom_id eşleşmesi — mismatch => kredi YOK + FAILED + log
    # Snapshot HowMuchPay (USD DECIMAL) cent'e çevrilip PayPal capture tutarıyla
    # karşılaştırılır; HowMuchPay NULL ise to_cents None döner -> mismatch (fail-closed)
    if (capture["amount_cents"] is None
            or capture["amount_cents"] != to_cents(txn["HowMuchPay"])
            or capture["currency"] != txn["Currency"].upper()
            or capture["custom_id"] != str(txn["ID"])):
        logger.critical(
            "amount mismatch on return order=%s txn=%s paypal=%s/%s custom=%s expected=%s/%s txn_id=%s",
            order_id, txn["ID"], capture["amount_cents"], capture["currency"],
            capture["custom_id"], txn["HowMuchPay"], txn["Currency"], txn["ID"],
        )
        db.mark_failed(txn["ID"], "amount/currency/custom_id mismatch on return")
        return _error_page(request, "Payment verification failed",
                           "The payment details did not match the order. No Cash was credited. "
                           "Please contact support.", status_code=409)

    result = db.complete_and_credit(
        txn_id=txn["ID"], account=txn["Account"],
        cash_amount=int(txn["CashAmount"]),
        # Fiilen capture edilen USD tutarı (yukarıda snapshot ile eşleştiği doğrulandı)
        how_much_pay=Decimal(capture["amount_cents"]) / Decimal(100),
        capture_id=capture["capture_id"], payer_email=capture["payer_email"],
    )
    if result == db.CREDIT_FAILED:
        return _error_page(request, "Credit failed",
                           "Your payment was received but the Cash could not be credited "
                           "automatically. Please contact support.", status_code=500)

    # CREDITED veya ALREADY_PROCESSED (webhook önce davranmış) — ikisi de başarı.
    return _info_page(request, "Payment completed",
                      "Your payment has been completed and the Cash has been credited. "
                      "Reopen the marketplace in game to see your new balance.",
                      state="success")


@app.get("/pay/cancel", response_class=HTMLResponse)
async def pay_cancel(request: Request, token: str = Query(default="")):
    order_id = token.strip()
    if order_id and db.mark_cancelled_by_order(order_id):
        logger.info("order cancelled by user: %s", order_id)
    return _info_page(request, "Payment cancelled",
                      "The payment was cancelled. No charge was made. "
                      "You can close this page and return to the game.")


@app.get("/health")
async def health():
    return {"status": "ok"}


# ---------------------------------------------------------------------------
# PayPal webhook
# ---------------------------------------------------------------------------

def _log_webhook(event_id, event_type, order_id, verified, verify_detail,
                 raw: bytes, headers, remote_ip: str) -> None:
    try:
        db.insert_webhook_log(
            event_id=event_id, event_type=event_type, order_id=order_id,
            verified=verified, verify_detail=verify_detail,
            raw_body=raw, headers=headers, remote_ip=remote_ip,
        )
    except Exception:
        # Log yazılamaması işlem kararını değiştirmez ama asla sessiz geçilmez.
        logger.exception("webhook log insert failed (event=%s)", event_id)


def _handle_capture_completed(event_id: Optional[str], resource: dict,
                              order_id: Optional[str]) -> None:
    custom_id = resource.get("custom_id") or resource.get("custom") or ""
    txn = _find_txn(order_id, custom_id)
    if txn is None:
        # Ödeme alınmış ama eşleşen transaction yok — manuel inceleme şart.
        logger.critical(
            "webhook capture for UNKNOWN order=%s custom=%s event=%s — manual review required",
            order_id, custom_id, event_id,
        )
        return

    amount = resource.get("amount") or {}
    cents = to_cents(amount.get("value"))
    currency = (amount.get("currency_code") or "").upper()
    if (cents is None
            or cents != to_cents(txn["HowMuchPay"])
            or currency != txn["Currency"].upper()
            or (custom_id and custom_id != str(txn["ID"]))):
        # Amount/currency mismatch: kredi YOK + FAILED + log (tasarım §5-3)
        logger.critical(
            "webhook amount mismatch event=%s txn=%s paypal=%s/%s expected=%s/%s",
            event_id, txn["ID"], cents, currency, txn["HowMuchPay"], txn["Currency"],
        )
        db.mark_failed(txn["ID"], f"webhook amount/currency mismatch (event {event_id})")
        return

    payer_email = (resource.get("payer") or {}).get("email_address")
    result = db.complete_and_credit(
        txn_id=txn["ID"], account=txn["Account"],
        cash_amount=int(txn["CashAmount"]),
        # Fiilen capture edilen USD tutarı (yukarıda snapshot ile eşleştiği doğrulandı)
        how_much_pay=Decimal(cents) / Decimal(100),
        capture_id=resource.get("id"), payer_email=payer_email,
        webhook_event_id=event_id,
    )
    if result == db.CREDIT_CREDITED:
        logger.info("webhook credited txn=%s event=%s", txn["ID"], event_id)
    elif result == db.CREDIT_ALREADY_PROCESSED:
        # Return akışı (veya önceki webhook teslimi) krediyi çoktan yazmış — çift kredi YOK.
        logger.info("webhook txn=%s already processed — no double credit", txn["ID"])
    else:
        logger.critical("webhook credit FAILED txn=%s — manual review required", txn["ID"])


def _handle_capture_denied(event_id: Optional[str], resource: dict,
                           order_id: Optional[str]) -> None:
    custom_id = resource.get("custom_id") or resource.get("custom") or ""
    txn = _find_txn(order_id, custom_id)
    if txn is None:
        logger.warning("webhook DENIED for unknown order=%s custom=%s event=%s",
                       order_id, custom_id, event_id)
        return
    if db.mark_failed(txn["ID"], f"capture denied by PayPal (event {event_id})"):
        logger.warning("txn=%s marked FAILED (capture denied, event=%s)", txn["ID"], event_id)


@app.post("/paypal/webhook")
async def paypal_webhook(request: Request):
    remote_ip = request.client.host if request.client else "-"
    if not config.paypal_webhook_id:
        logger.warning(
            "webhook rejected: PAYPAL_WEBHOOK_ID empty (sandbox HTTP test; "
            "credit only via /pay/return) ip=%s",
            remote_ip,
        )
        return JSONResponse({"detail": "webhook not configured"}, status_code=503)
    raw = await request.body()
    if len(raw) > _WEBHOOK_MAX_BODY:
        logger.warning("webhook rejected: body %d bytes from %s", len(raw), remote_ip)
        return JSONResponse({"detail": "body too large"}, status_code=413)

    try:
        event = json.loads(raw.decode("utf-8", errors="replace"))
    except json.JSONDecodeError:
        logger.warning("webhook rejected: invalid JSON from %s", remote_ip)
        return JSONResponse({"detail": "invalid json"}, status_code=400)

    headers = request.headers
    transmission_id = headers.get("paypal-transmission-id", "")
    transmission_time = headers.get("paypal-transmission-time", "")
    cert_url = headers.get("paypal-cert-url", "")
    transmission_sig = headers.get("paypal-transmission-sig", "")
    auth_algo = headers.get("paypal-auth-algo", "")
    if not all((transmission_id, transmission_time, cert_url, transmission_sig, auth_algo)):
        logger.warning("webhook rejected: missing signature headers from %s", remote_ip)
        return JSONResponse({"detail": "missing signature headers"}, status_code=400)

    # Replay koruması: transmission_time ±5 dk (tasarım §5-10)
    try:
        ts = datetime.fromisoformat(transmission_time.replace("Z", "+00:00"))
        skew = abs((datetime.now(timezone.utc) - ts).total_seconds())
    except ValueError:
        logger.warning("webhook rejected: bad transmission_time '%s' from %s",
                       transmission_time, remote_ip)
        return JSONResponse({"detail": "bad transmission_time"}, status_code=400)
    if skew > _TRANSMISSION_MAX_SKEW_SEC:
        logger.warning("webhook rejected: stale transmission (skew %.0fs) from %s",
                       skew, remote_ip)
        return JSONResponse({"detail": "stale transmission"}, status_code=400)

    event_id = event.get("id")
    event_type = event.get("event_type") or "UNKNOWN"
    resource = event.get("resource") or {}
    order_id = ((resource.get("supplementary_data") or {}).get("related_ids") or {}).get("order_id")

    # İmza doğrulama — bu adım geçilmeden ASLA kredi yazılmaz (tasarım §5-1)
    try:
        verified = await paypal_api.verify_webhook_signature(
            transmission_id=transmission_id,
            transmission_time=transmission_time,
            cert_url=cert_url,
            transmission_sig=transmission_sig,
            auth_algo=auth_algo,
            webhook_event=event,
        )
    except PayPalAPIError as exc:
        # Geçici PayPal API sorunu: 500 dön ki PayPal teslimatı yeniden denesin.
        logger.error("webhook verify API error (event=%s): %s", event_id, exc)
        return JSONResponse({"detail": "verification temporarily unavailable"},
                            status_code=500)

    if not verified:
        _log_webhook(event_id, event_type, order_id, False,
                     "verification_status != SUCCESS", raw, headers, remote_ip)
        logger.warning("webhook signature verification FAILED (event=%s type=%s ip=%s)",
                       event_id, event_type, remote_ip)
        # tasarım §5-1: logla, kredi YOK, 200 dön
        return JSONResponse({"status": "ignored"})

    # İdempotency: aynı event_id bir kez işlenir (tasarım §5-2)
    if event_id and db.webhook_event_seen(event_id):
        logger.info("webhook duplicate event %s — already logged, skipped", event_id)
        return JSONResponse({"status": "duplicate"})

    _log_webhook(event_id, event_type, order_id, True, "SUCCESS",
                 raw, headers, remote_ip)

    try:
        if event_type == "PAYMENT.CAPTURE.COMPLETED":
            _handle_capture_completed(event_id, resource, order_id)
        elif event_type == "PAYMENT.CAPTURE.DENIED":
            _handle_capture_denied(event_id, resource, order_id)
        else:
            logger.info("webhook event %s type %s: logged, no action", event_id, event_type)
    except Exception:
        # DB hatası vb.: logla + 500 dön ki PayPal yeniden denesin; status guard'ı
        # yeniden denemede çift krediyi engeller.
        logger.exception("webhook processing error (event=%s type=%s)", event_id, event_type)
        return JSONResponse({"detail": "processing error"}, status_code=500)

    return JSONResponse({"status": "ok"})
