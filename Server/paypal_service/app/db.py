# DARKEDEN veritabanı erişim katmanı (pymysql).
#
# Kurallar:
# - Tüm sorgular parametrelidir (%s placeholder); string-format SQL YOKTUR.
# - Kredi ekleme ATOMİK koşullu UPDATE ile yapılır:
#     PayPalTransactions: PENDING->COMPLETED geçişi affected-rows==1 şartıyla (race koruması)
#     userinfo:           havecount = havecount + N (read-modify-write DEĞİL — online oyuncu
#                         ile eşzamanlı market alışverişinde yarış olmaz, tasarım §5-4)
# - Hiçbir hata sessizce yutulmaz; bağlamıyla loglanır ve caller'a anlamlı sonuç döner.

import json
import logging
from contextlib import contextmanager
from decimal import Decimal
from typing import Optional

import pymysql
from pymysql.cursors import DictCursor

from .config import config

logger = logging.getLogger("paypal_service.db")

# complete_and_credit() dönüş değerleri
CREDIT_CREDITED = "credited"
CREDIT_ALREADY_PROCESSED = "already_processed"
CREDIT_FAILED = "credit_failed"

# Kolon adları 2026-08-20_paypal_cash.sql ile birebir aynı tutulur (kullanıcı şartı):
# PlayerID (Player.PlayerID == userinfo.username, string), HowMuchPay (DECIMAL USD),
# PayDate (COMPLETED anı).
_TXN_COLUMNS = (
    "ID, Account, PlayerID, CharName, PackageID, HowMuchPay, Currency, CashAmount,"
    " Status, OrderID, CaptureID, OneTimeKey, KeyExpiresAt, WebhookEventID,"
    " PayerEmail, CreatedAt, PayDate"
)


@contextmanager
def _connect():
    """İşlem başına kısa ömürlü bağlantı.

    Ödeme trafiği düşük hacimlidir; pool yerine bağlantı-bazlı yaklaşım
    thread-safety ve hata izolasyonu açısından daha güvenlidir.
    """
    conn = pymysql.connect(
        host=config.db_host,
        port=config.db_port,
        user=config.db_user,
        password=config.db_pass,
        database=config.db_name,
        charset="utf8mb4",
        cursorclass=DictCursor,
        autocommit=False,
    )
    try:
        yield conn
    finally:
        conn.close()


# ---------------------------------------------------------------------------
# Transaction okuma
# ---------------------------------------------------------------------------

def fetch_transaction_by_token(token: str) -> Optional[dict]:
    with _connect() as conn:
        with conn.cursor() as cur:
            cur.execute(
                f"SELECT {_TXN_COLUMNS} FROM PayPalTransactions WHERE OneTimeKey=%s",
                (token,),
            )
            return cur.fetchone()


def fetch_transaction_by_order_id(order_id: str) -> Optional[dict]:
    with _connect() as conn:
        with conn.cursor() as cur:
            cur.execute(
                f"SELECT {_TXN_COLUMNS} FROM PayPalTransactions WHERE OrderID=%s",
                (order_id,),
            )
            return cur.fetchone()


def fetch_transaction_by_id(txn_id: int) -> Optional[dict]:
    """custom_id (PayPal order'a gömülen PayPalTransactions.ID) üzerinden bulma —
    webhook'ta order_id eşleşmezse yedek yol."""
    with _connect() as conn:
        with conn.cursor() as cur:
            cur.execute(
                f"SELECT {_TXN_COLUMNS} FROM PayPalTransactions WHERE ID=%s",
                (txn_id,),
            )
            return cur.fetchone()


# ---------------------------------------------------------------------------
# Paket okuma (fiyat tek doğruluk kaynağı: PayPalCashPackages tablosu)
# ---------------------------------------------------------------------------

def fetch_active_packages() -> list:
    with _connect() as conn:
        with conn.cursor() as cur:
            cur.execute(
                "SELECT PackageID, Name, AmountDollar, Currency, CashAmount"
                " FROM PayPalCashPackages WHERE Active=1 ORDER BY AmountDollar ASC"
            )
            return cur.fetchall()


def fetch_package(package_id: int) -> Optional[dict]:
    with _connect() as conn:
        with conn.cursor() as cur:
            cur.execute(
                "SELECT PackageID, Name, AmountDollar, Currency, CashAmount"
                " FROM PayPalCashPackages WHERE PackageID=%s AND Active=1",
                (package_id,),
            )
            return cur.fetchone()


# ---------------------------------------------------------------------------
# Transaction yazma
# ---------------------------------------------------------------------------

def attach_order(*, txn_id: int, package_id: int, how_much_pay: Decimal,
                 currency: str, cash_amount: int, order_id: str) -> bool:
    """PayPal order id + paket snapshot'ını transaction'a yazar.

    Fiyat/cash her zaman PayPalCashPackages'tan okunan değerlerdir (client'tan ASLA
    gelmez); sonradan fiyat değişse bile açık işlem korunur (tasarım §7).
    how_much_pay: USD cinsinden Decimal (örn. Decimal('4.99')) — HowMuchPay
    kolonuna yazılır; COMPLETED'a geçişte capture tutarıyla teyit edilir.
    Koşullu UPDATE: yalnızca hâlâ PENDING ise. affected!=1 -> False (eşzamanlı
    istek durumu değiştirmiş demektir).
    """
    with _connect() as conn:
        with conn.cursor() as cur:
            cur.execute(
                "UPDATE PayPalTransactions"
                " SET PackageID=%s, HowMuchPay=%s, Currency=%s, CashAmount=%s, OrderID=%s"
                " WHERE ID=%s AND Status='PENDING'",
                (package_id, how_much_pay, currency, cash_amount, order_id, txn_id),
            )
            if cur.rowcount != 1:
                conn.rollback()
                return False
        conn.commit()
        return True


def mark_cancelled_by_order(order_id: str) -> bool:
    """PENDING -> CANCELLED (tek yönlü). Zaten işlenmişse False döner."""
    with _connect() as conn:
        with conn.cursor() as cur:
            cur.execute(
                "UPDATE PayPalTransactions SET Status='CANCELLED'"
                " WHERE OrderID=%s AND Status='PENDING'",
                (order_id,),
            )
            if cur.rowcount != 1:
                conn.rollback()
                return False
        conn.commit()
        return True


def mark_failed(txn_id: int, reason: str) -> bool:
    """PENDING -> FAILED (tek yönlü). Sebep loglanır; tabloda reason kolonu yoktur."""
    logger.error("marking txn=%s FAILED: %s", txn_id, reason)
    with _connect() as conn:
        with conn.cursor() as cur:
            cur.execute(
                "UPDATE PayPalTransactions SET Status='FAILED'"
                " WHERE ID=%s AND Status='PENDING'",
                (txn_id,),
            )
            if cur.rowcount != 1:
                conn.rollback()
                return False
        conn.commit()
        return True


def complete_and_credit(*, txn_id: int, account: str, cash_amount: int,
                        how_much_pay: Decimal,
                        capture_id: Optional[str], payer_email: Optional[str],
                        webhook_event_id: Optional[str] = None) -> str:
    """PENDING->COMPLETED geçişi + havecount artışı TEK DB transaction'ında.

    Çift kredi koruması (return akışı + webhook aynı anda gelse bile):
      1) Koşullu UPDATE ... WHERE ID=%s AND Status='PENDING' — yalnızca ilk gelen
         akış affected-rows==1 alır; ikincisi ALREADY_PROCESSED döner.
      2) WebhookEventID UNIQUE — aynı PayPal event'i ikinci kez yazılamaz.
      3) userinfo güncellemesi atomik increment'tir (havecount = havecount + N).

    COMPLETED'a geçişte PayDate=NOW() ve HowMuchPay (capture edilen fiili USD
    tutarı; caller bunu checkout snapshot'ıyla eşleştirdikten SONRA çağırır)
    birlikte yazılır — kullanıcı şartı (2026-08-20 revizyonu).

    userinfo eşleşmesi username üzerinden yapılır: tabloda id kolonu yoktur,
    gameserver da tüm market işlemlerini username ile yapar (tasarım §5-4).
    """
    conn = pymysql.connect(
        host=config.db_host, port=config.db_port,
        user=config.db_user, password=config.db_pass,
        database=config.db_name, charset="utf8mb4",
        cursorclass=DictCursor, autocommit=False,
    )
    try:
        with conn.cursor() as cur:
            cur.execute(
                "UPDATE PayPalTransactions"
                " SET Status='COMPLETED', CaptureID=%s, PayerEmail=%s,"
                "     WebhookEventID=%s, HowMuchPay=%s, PayDate=NOW()"
                " WHERE ID=%s AND Status='PENDING'",
                (capture_id, payer_email, webhook_event_id, how_much_pay, txn_id),
            )
            if cur.rowcount != 1:
                conn.rollback()
                # Başka bir akış (return/webhook) bu işlemi çoktan tamamlamış.
                logger.info("txn=%s already processed (status guard) — credit skipped", txn_id)
                return CREDIT_ALREADY_PROCESSED

            cur.execute(
                "UPDATE userinfo SET havecount = havecount + %s WHERE username=%s",
                (cash_amount, account),
            )
            if cur.rowcount != 1:
                conn.rollback()
                # userinfo satırı yoksa kredi YAZILMAZ; işlem PENDING kalır. Alarm:
                logger.critical(
                    "credit FAILED: userinfo row missing account=%s txn=%s cash=%s"
                    " — manual review required",
                    account, txn_id, cash_amount,
                )
                return CREDIT_FAILED

        conn.commit()
        logger.info("credited account=%s txn=%s cash=+%s capture=%s",
                    account, txn_id, cash_amount, capture_id)
        return CREDIT_CREDITED
    except pymysql.err.IntegrityError as exc:
        conn.rollback()
        # WebhookEventID UNIQUE ihlali: aynı event ikinci kez teslim edilmiş.
        logger.warning("credit integrity error txn=%s (duplicate event?): %s", txn_id, exc)
        return CREDIT_ALREADY_PROCESSED
    except Exception:
        conn.rollback()
        logger.exception("credit DB error txn=%s account=%s", txn_id, account)
        return CREDIT_FAILED
    finally:
        conn.close()


# ---------------------------------------------------------------------------
# Webhook log (denetim / dispute kanıtı + idempotency)
# ---------------------------------------------------------------------------

def webhook_event_seen(event_id: str) -> bool:
    """Aynı PayPal event_id daha önce loglandıysa True (idempotency ilk filtresi;
    nihai koruma complete_and_credit'teki status guard'ıdır)."""
    with _connect() as conn:
        with conn.cursor() as cur:
            cur.execute(
                "SELECT ID FROM PayPalWebhookLog WHERE EventID=%s LIMIT 1",
                (event_id,),
            )
            return cur.fetchone() is not None


def insert_webhook_log(*, event_id: Optional[str], event_type: str,
                       order_id: Optional[str], verified: bool,
                       verify_detail: Optional[str], raw_body: bytes,
                       headers, remote_ip: str) -> None:
    with _connect() as conn:
        with conn.cursor() as cur:
            cur.execute(
                "INSERT INTO PayPalWebhookLog"
                " (EventID, EventType, OrderID, Verified, VerifyDetail, RawBody, Headers, RemoteIP)"
                " VALUES (%s, %s, %s, %s, %s, %s, %s, %s)",
                (
                    event_id,
                    event_type,
                    order_id,
                    1 if verified else 0,
                    verify_detail,
                    raw_body.decode("utf-8", errors="replace"),
                    # Header dict'i JSON olarak saklanır (imza doğrulama kanıtı dahil)
                    json.dumps(dict(headers), ensure_ascii=False),
                    remote_ip,
                ),
            )
        conn.commit()
