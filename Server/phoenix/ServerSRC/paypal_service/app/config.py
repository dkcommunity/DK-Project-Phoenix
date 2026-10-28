# PayPal Cash servisi — ortam tabanlı yapılandırma.
# Tüm secret'lar (PayPal client secret, DB şifresi) YALNIZCA bu servisin çalıştığı
# ortamda tutulur (systemd EnvironmentFile veya .env); repo'ya ve gameserver
# binary'sine ASLA girmez. Eksik zorunlu değişken servis açılışında net hata verir
# (sessiz varsayılan yok).

import os
from dataclasses import dataclass

from dotenv import load_dotenv

# systemd EnvironmentFile kullanıldığında .env bulunamaz — bu sorun değildir,
# değişkenler zaten process ortamında yüklüdür.
load_dotenv()


def _require(name: str) -> str:
    value = os.environ.get(name, "").strip()
    if not value:
        raise RuntimeError(f"Missing required environment variable: {name}")
    return value


def _optional(name: str) -> str:
    return os.environ.get(name, "").strip()


@dataclass(frozen=True)
class Config:
    paypal_client_id: str
    paypal_client_secret: str
    paypal_mode: str        # "sandbox" | "live"
    paypal_webhook_id: str
    base_url: str           # public base URL (nginx TLS sonlandırma), sonda '/' olmadan
    currency: str           # varsayılan para birimi (paket satırında Currency yoksa)
    db_host: str
    db_port: int
    db_user: str
    db_pass: str
    db_name: str
    log_file: str

    @property
    def paypal_api_base(self) -> str:
        # Sandbox: api-m.sandbox.paypal.com / Live: api-m.paypal.com
        if self.paypal_mode == "live":
            return "https://api-m.paypal.com"
        return "https://api-m.sandbox.paypal.com"

    @property
    def paypal_web_base(self) -> str:
        # Mevcut order için approval URL'si türetirken kullanılan web tabanı
        # (refresh güvenliği: checkoutnow?token=<order_id>)
        if self.paypal_mode == "live":
            return "https://www.paypal.com"
        return "https://www.sandbox.paypal.com"


def load_config() -> Config:
    mode = os.environ.get("PAYPAL_MODE", "sandbox").strip().lower()
    if mode not in ("sandbox", "live"):
        raise RuntimeError(f"PAYPAL_MODE must be 'sandbox' or 'live', got: {mode!r}")
    webhook_id = _optional("PAYPAL_WEBHOOK_ID")
    # Live webhook imzası zorunlu. Sandbox HTTP/IP testinde return-capture
    # krediyi yazar; PayPal public webhook LAN IP'ye ulaşamaz.
    if mode == "live" and not webhook_id:
        raise RuntimeError("Missing required environment variable: PAYPAL_WEBHOOK_ID")
    return Config(
        paypal_client_id=_require("PAYPAL_CLIENT_ID"),
        paypal_client_secret=_require("PAYPAL_CLIENT_SECRET"),
        paypal_mode=mode,
        paypal_webhook_id=webhook_id,
        base_url=_require("BASE_URL").rstrip("/"),
        currency=os.environ.get("CURRENCY", "USD").strip().upper(),
        db_host=_require("DB_HOST"),
        db_port=int(os.environ.get("DB_PORT", "3306")),
        db_user=_require("DB_USER"),
        db_pass=_require("DB_PASS"),
        db_name=os.environ.get("DB_NAME", "DARKEDEN"),
        log_file=os.environ.get("LOG_FILE", "paypal_service.log"),
    )


config = load_config()
