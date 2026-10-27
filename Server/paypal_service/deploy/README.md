# PayPal Cash Servisi — Deploy Rehberi

FastAPI + uvicorn ile çalışan ödeme köprüsü. systemd servisi olarak `127.0.0.1:8000`
üzerinde dinler; nginx TLS sonlandırma + reverse proxy yapar. PayPal secret'ları
**yalnızca bu servisin env'indedir** — gameserver binary'sinde ve repo'da YOKTUR.

## Mimari

```
Oyuncu (oyun-içi browser / sistem browser'i)
   |  https://<domain>/pay/?token=...
   v
nginx (443, TLS)  --->  uvicorn 127.0.0.1:8000  --->  MySQL DARKEDEN
   ^                                               --->  PayPal REST (api-m[.sandbox].paypal.com)
   |                                                       ^
PayPal webhook --->  POST /paypal/webhook (imza doğrulamalı)
```

## Ön koşullar

1. **DB migration uygulanmış olmalı:** `PayPalCashPackages`, `PayPalTransactions`,
   `PayPalWebhookLog` tabloları (tasarım §3 — `ServerSRC/tools/paypal/sql/paypal_tables.sql`).
   Bu servis şema **yaratmaz**; tablolar yoksa açılışta değil ilk istekte hata alırsın.
   `PayPalCashPackages`'a en az bir `Active=1` paket girilmiş olmalı.
2. **Domain şart:** PayPal webhook public HTTPS ister. Domain yoksa DuckDNS
   (duckdns.org) ile ücretsiz subdomain alıp VPS IP'sine (`89.252.164.244`)
   yönlendir.
3. VPS'te Python 3.10+ ve nginx kurulu olmalı.

## Kurulum adımları (VPS)

```bash
# 1) Dosyalar
sudo mkdir -p /opt/paypal_service
sudo cp -r app requirements.txt /opt/paypal_service/

# 2) Ayrıcalıksız kullanıcı
sudo useradd -r -s /usr/sbin/nologin paypal
sudo chown -R paypal:paypal /opt/paypal_service

# 3) venv + bağımlılıklar
cd /opt/paypal_service
sudo -u paypal python3 -m venv venv
sudo -u paypal venv/bin/pip install -r requirements.txt

# 4) Ortam değişkenleri (secret'lar burada; repo'ya GİRMEZ)
sudo mkdir -p /etc/paypal_service
sudo cp .env.example /etc/paypal_service/paypal.env
sudo nano /etc/paypal_service/paypal.env      # değerleri doldur
sudo chmod 600 /etc/paypal_service/paypal.env

# 5) systemd
sudo cp deploy/paypal.service.example /etc/systemd/system/paypal.service
sudo systemctl daemon-reload
sudo systemctl enable --now paypal
systemctl status paypal                       # active (running) görmelisin
curl -s http://127.0.0.1:8000/health          # {"status":"ok"}
# .env veya app/ değişince: /home/phoenix/phoenix/ServerSRC/paypal_service/restart.sh

# 6) nginx
sudo cp deploy/nginx.conf.example /etc/nginx/sites-available/paypal_service
sudo nano /etc/nginx/sites-available/paypal_service   # server_name'i değiştir
# limit_req_zone satırını http {} bloğuna taşı (dosya başındaki nota bak):
echo 'limit_req_zone $binary_remote_addr zone=paypal_webhook:10m rate=10r/s;' \
  | sudo tee /etc/nginx/conf.d/paypal_ratelimit.conf
sudo ln -s /etc/nginx/sites-available/paypal_service /etc/nginx/sites-enabled/
sudo nginx -t && sudo systemctl reload nginx

# 7) TLS (Let's Encrypt)
sudo certbot --nginx -d pay.example.duckdns.org
```

## PayPal Developer Console ayarları

1. https://developer.paypal.com → **Apps & Credentials** → **Sandbox** (başlangıç
   kesinlikle sandbox) → **Create App** (tip: Merchant) → `Client ID` + `Secret`
   değerlerini `paypal.env` içine yaz.
2. Aynı app sayfasında **Webhooks** → **Add Webhook**:
   - URL: `https://<domain>/paypal/webhook`
   - Event types: **`PAYMENT.CAPTURE.COMPLETED` (minimum, zorunlu)** +
     `PAYMENT.CAPTURE.DENIED` (önerilen — reddedilen ödemeler FAILED işaretlenir)
3. Webhook detay sayfasındaki **Webhook ID** değerini `PAYPAL_WEBHOOK_ID`'ye yaz.
4. `sudo systemctl restart paypal`

## Test (sandbox)

1. Gameserver tarafı PENDING transaction + OneTimeKey ürettikten sonra
   `https://<domain>/pay/?token=<key>` sayfasını aç.
2. Paket seç → PayPal sandbox **buyer** hesabıyla öde.
3. Dönüş sayfasında "Payment completed" görmelisin; DB'de
   `PayPalTransactions.Status='COMPLETED'` ve `userinfo.havecount` artmış olmalı.
4. Loglar: `journalctl -u paypal -f` ve `$LOG_FILE`.
5. Webhook teslimatını Developer Console → Webhooks → **Webhook Events** altından
   izle; `PayPalWebhookLog` tablosunda `Verified=1` satırı görmelisin.

## Live'a geçiş

1. Developer Console'da **Live** sekmesinde ayrı app + ayrı webhook yarat
   (live webhook ID farklıdır).
2. `paypal.env`: `PAYPAL_MODE=live` + live `PAYPAL_CLIENT_ID/SECRET/WEBHOOK_ID`.
3. `sudo systemctl restart paypal` → sandbox akışının aynısını live'da küçük
   tutarla tekrar test et.

## VPS + LAN parity (kural 12)

Aynı kurulum LAN test sunucusu `192.168.1.5` üzerinde de yapılır (kendi domain/env
değerleriyle). Deploy sonrası iki tarafta `systemctl status paypal`, `/health`
ve kritik log satırları doğrulanır.

## Sorun giderme

| Belirti | Kontrol |
|---|---|
| Sayfa açılmıyor | `systemctl status paypal`, `journalctl -u paypal -n 100` |
| "Invalid or expired link" | `PayPalTransactions` satırı var mı, `Status='PENDING'` mi, `KeyExpiresAt` geçmiş mi |
| Webhook işlenmiyor | `PayPalWebhookLog.Verified=0` ise imza/WEBHOOK_ID yanlış; PayPal console'da event durumu |
| Kredi yazılmadı | Log'da `credit FAILED` / `amount mismatch` ara; `userinfo` satırı var mı |
| 502 provider error | `PAYPAL_MODE` ile credential çifti uyumlu mu (sandbox cred + live mode = hata) |
