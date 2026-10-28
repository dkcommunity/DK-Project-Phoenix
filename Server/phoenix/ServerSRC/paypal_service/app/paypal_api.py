# PayPal REST API istemcisi (httpx async).
#
# - OAuth access token'ları bellekte cache'lenir (expires_in - 60 sn güvenlik payı).
# - Secret'lar yalnızca config'ten okunur; hiçbir log'a yazılmaz.
# - API hataları PayPalAPIError ile yukarı taşınır; hiçbir hata sessizce yutulmaz.

import logging
import time
from decimal import Decimal, InvalidOperation, ROUND_HALF_UP
from typing import Optional

import httpx

from .config import config

logger = logging.getLogger("paypal_service.paypal")


class PayPalAPIError(Exception):
    """PayPal API çağrısı başarısız; mesaj teşhis bağlamı (endpoint, HTTP kodu) içerir."""


def to_cents(value) -> Optional[int]:
    """'4.99' -> 499. Ayrıştırılamazsa None döner; caller bunu mismatch olarak işler
    (sessizce 0 kabul edilmez)."""
    try:
        return int((Decimal(str(value)) * 100).quantize(Decimal("1"), rounding=ROUND_HALF_UP))
    except (InvalidOperation, TypeError, ValueError):
        return None


_token_cache = {"access_token": None, "expires_at": 0.0}


async def _get_access_token() -> str:
    now = time.time()
    if _token_cache["access_token"] and now < _token_cache["expires_at"]:
        return _token_cache["access_token"]

    async with httpx.AsyncClient(base_url=config.paypal_api_base, timeout=15.0) as client:
        resp = await client.post(
            "/v1/oauth2/token",
            auth=(config.paypal_client_id, config.paypal_client_secret),
            data={"grant_type": "client_credentials"},
        )
    if resp.status_code != 200:
        # Yanıt gövdesi secret içermez; teşhis için ilk 300 karakter yeterli.
        raise PayPalAPIError(
            f"OAuth token request failed: HTTP {resp.status_code}: {resp.text[:300]}"
        )
    payload = resp.json()
    token = payload.get("access_token")
    if not token:
        raise PayPalAPIError(f"OAuth response missing access_token: {resp.text[:300]}")
    expires_in = int(payload.get("expires_in", 300))
    _token_cache["access_token"] = token
    _token_cache["expires_at"] = now + max(expires_in - 60, 30)
    return token


async def create_order(*, amount_dollar: int, currency: str, custom_id: str,
                       return_url: str, cancel_url: str) -> tuple:
    """Checkout order yaratır; (order_id, approval_url) döner.

    Tutar PayPalCashPackages.AmountDollar (tam dolar, 5 = $5.00). custom_id olarak
    PayPalTransactions.ID gömülür (webhook eşleştirmesi için).
    """
    if amount_dollar < 1:
        raise PayPalAPIError(f"invalid AmountDollar={amount_dollar}")
    token = await _get_access_token()
    body = {
        "intent": "CAPTURE",
        "purchase_units": [{
            "amount": {
                "currency_code": currency,
                "value": f"{amount_dollar:.2f}",
            },
            "custom_id": custom_id,
        }],
        "application_context": {
            "return_url": return_url,
            "cancel_url": cancel_url,
            "user_action": "PAY_NOW",
            "shipping_preference": "NO_SHIPPING",  # dijital bakiye — adres istenmez
        },
    }
    async with httpx.AsyncClient(base_url=config.paypal_api_base, timeout=20.0) as client:
        resp = await client.post(
            "/v2/checkout/orders",
            json=body,
            headers={"Authorization": f"Bearer {token}"},
        )
    if resp.status_code not in (200, 201):
        raise PayPalAPIError(
            f"order create failed: HTTP {resp.status_code}: {resp.text[:300]}"
        )
    payload = resp.json()
    order_id = payload.get("id")
    approve_url = next(
        (link["href"] for link in payload.get("links", []) if link.get("rel") == "approve"),
        None,
    )
    if not order_id or not approve_url:
        raise PayPalAPIError(f"order create response malformed: {resp.text[:300]}")
    return order_id, approve_url


def _normalize_capture(payload: dict, order_id: str, raw_text: str) -> dict:
    """Capture JSON'undan kredi için gerekli alanları çıkarır."""
    try:
        capture = payload["purchase_units"][0]["payments"]["captures"][0]
    except (KeyError, IndexError, TypeError) as exc:
        raise PayPalAPIError(
            f"capture response malformed order={order_id}: {exc}: {raw_text[:300]}"
        )
    amount = capture.get("amount") or {}
    unit = (payload.get("purchase_units") or [{}])[0] or {}
    details = capture.get("status_details") or {}
    return {
        "status": capture.get("status", ""),
        "capture_id": capture.get("id", ""),
        "amount_cents": to_cents(amount.get("value")),
        "currency": (amount.get("currency_code") or "").upper(),
        "custom_id": (capture.get("custom_id") or capture.get("custom")
                      or unit.get("custom_id") or ""),
        "payer_email": (payload.get("payer") or {}).get("email_address"),
        "pending_reason": details.get("reason") or "",
    }


def _existing_capture(payload: dict, order_id: str, raw_text: str) -> Optional[dict]:
    """Order GET gövdesinde capture varsa normalize eder; yoksa None."""
    try:
        units = payload.get("purchase_units") or []
        if not units:
            return None
        payments = units[0].get("payments") or {}
        captures = payments.get("captures") or []
        if not captures:
            return None
    except (AttributeError, TypeError):
        return None
    return _normalize_capture(payload, order_id, raw_text)


async def _get_order_payload(order_id: str) -> tuple:
    token = await _get_access_token()
    async with httpx.AsyncClient(base_url=config.paypal_api_base, timeout=20.0) as client:
        resp = await client.get(
            f"/v2/checkout/orders/{order_id}",
            headers={"Authorization": f"Bearer {token}"},
        )
    if resp.status_code != 200:
        raise PayPalAPIError(
            f"order get failed order={order_id}: HTTP {resp.status_code}: {resp.text[:300]}"
        )
    return resp.json(), resp.text


async def get_order_capture(order_id: str) -> dict:
    """GET /v2/checkout/orders/{id} — yenilemede ikinci capture atılmaz."""
    payload, raw = await _get_order_payload(order_id)
    existing = _existing_capture(payload, order_id, raw)
    if existing is None:
        raise PayPalAPIError(f"order has no capture yet order={order_id}")
    return existing


async def capture_order(order_id: str) -> dict:
    """Mevcut capture varsa yalnız GET; yoksa bir kez POST /capture.

    Yenileme ve paralel return istekleri PayPal 422
    (ORDER_ALREADY_CAPTURED / MAX_NUMBER_OF_PAYMENT_ATTEMPTS_EXCEEDED)
    üretmesin diye önce GET bakılır.
    """
    payload, raw = await _get_order_payload(order_id)
    existing = _existing_capture(payload, order_id, raw)
    if existing is not None:
        logger.info("order=%s already has capture status=%s — skip POST",
                    order_id, existing["status"])
        return existing

    token = await _get_access_token()
    async with httpx.AsyncClient(base_url=config.paypal_api_base, timeout=20.0) as client:
        resp = await client.post(
            f"/v2/checkout/orders/{order_id}/capture",
            # PayPal capture çağrısı gövdesizdir; Content-Type header'ı şart.
            headers={"Authorization": f"Bearer {token}",
                     "Content-Type": "application/json"},
        )
    if resp.status_code in (200, 201):
        return _normalize_capture(resp.json(), order_id, resp.text)
    if resp.status_code == 422:
        logger.info("order=%s capture POST HTTP 422 — fetching existing capture: %s",
                    order_id, resp.text[:200])
        return await get_order_capture(order_id)
    raise PayPalAPIError(
        f"capture failed order={order_id}: HTTP {resp.status_code}: {resp.text[:300]}"
    )


async def verify_webhook_signature(*, transmission_id: str, transmission_time: str,
                                   cert_url: str, transmission_sig: str,
                                   auth_algo: str, webhook_event: dict) -> bool:
    """Webhook imzasını PayPal'a doğrulatır (verify-webhook-signature).

    True yalnızca verification_status == 'SUCCESS' ise döner. Bu doğrulama
    geçilmeden ASLA kredi yazılmaz (tasarım §5-1).
    """
    token = await _get_access_token()
    body = {
        "auth_algo": auth_algo,
        "cert_url": cert_url,
        "transmission_id": transmission_id,
        "transmission_sig": transmission_sig,
        "transmission_time": transmission_time,
        "webhook_id": config.paypal_webhook_id,
        "webhook_event": webhook_event,
    }
    async with httpx.AsyncClient(base_url=config.paypal_api_base, timeout=20.0) as client:
        resp = await client.post(
            "/v1/notifications/verify-webhook-signature",
            json=body,
            headers={"Authorization": f"Bearer {token}"},
        )
    if resp.status_code != 200:
        raise PayPalAPIError(
            f"verify-webhook-signature failed: HTTP {resp.status_code}: {resp.text[:300]}"
        )
    status = resp.json().get("verification_status")
    if status != "SUCCESS":
        logger.warning("webhook verification_status=%s transmission_id=%s",
                       status, transmission_id)
        return False
    return True
