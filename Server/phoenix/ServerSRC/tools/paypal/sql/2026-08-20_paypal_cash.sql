-- ============================================================================
-- 2026-08-20_paypal_cash.sql
-- PayPal Cash (DianJuan) yükleme sistemi — DB şeması
-- Tasarım: Dev_Log/Server/paypal_cash_charge_system_design_2026-08-20.md
--
-- Örüntü: tools/security/sql/security_ban_tables.sql (InnoDB, utf8mb4,
--         CREATE TABLE IF NOT EXISTS, ayrı least-privilege kullanıcı)
--
-- Notlar:
--  * userinfo tablosuna DOKUNULMUYOR — cash kredisi mevcut havecount kolonuna
--    web endpoint'i tarafından ATOMIC UPDATE ile yazılır
--    (UPDATE userinfo SET havecount = havecount + N WHERE username=...).
--  * PayPalTransactions.PackageID/HowMuchPay/CashAmount PENDING anında NULL'dır;
--    paket web sayfasında seçilir, checkout sırasında web endpoint snapshot yazar.
--  * Gameserver sadece PENDING kayıt + tek-kullanımlık token üretir
--    (CGCashChargeRequestHandler); fiyat/paket bilgisi gameserver'da TUTULMAZ.
--  * Uygulama: mysql DARKEDEN < 2026-08-20_paypal_cash.sql  (VPS + LAN 192.168.1.5)
--
-- Revizyon 2026-08-20 (kullanıcı şartları — canlı DB 192.168.1.5'te doğrulandı):
--  * PlayerID kolonu ZORUNLU: Player.PlayerID varchar(10) utf8mb4_unicode_ci ile
--    birebir aynı tip+collation. Canlı DB'de Player.PlayerID == userinfo.username
--    (18/18 satır birebir eşleşti) — yani PlayerID STRING hesap id'sidir, numeric
--    DEĞİL. Account kolonuyla aynı değeri taşır; Account servis katmanı sürekliliği
--    için korunur, PlayerID kullanıcının kanonik isimlendirmesidir. İkisi de
--    gameserver PENDING insert'inde aynı değerle yazılır.
--    (Player tablosu MyISAM — InnoDB FK kurulamaz; ilişki INDEX + uygulama
--    katmanıyla sağlanır.)
--  * PayDate ZORUNLU: ödeme tamamlanma anı. COMPLETED'a geçişte servis
--    (complete_and_credit) NOW() yazar; PENDING/FAILED/CANCELLED'da NULL kalır.
--    (Taslağın CompletedAt kolonunun kullanıcı isimlendirmesiyle yeniden adı —
--    tablo hiçbir ortamda henüz yok, rename temizdir.)
--  * HowMuchPay ZORUNLU: oyuncunun fiilen ödediği USD miktarı (örn. 50.00),
--    DECIMAL(10,2). Taslaktaki AmountCents INT'in yerine geçer (kullanıcı
--    isimlendirmesi tercih edildi). Checkout'ta CashPackages liste fiyatından
--    snapshot'lanır; COMPLETED'a geçişte capture edilen gerçek tutarla teyit
--    edilerek yazılır. CashPackages.AmountCents (cent cinsinden liste fiyatı)
--    AYNEN KALIR — PayPal API doğrulaması cent bazlı çalışır; servis kodu
--    snapshot'ı cent'e çevirerek karşılaştırır (paypal_api.to_cents).
-- ============================================================================

-- Fiyat paketleri (server-side tek doğruluk kaynağı)
CREATE TABLE IF NOT EXISTS CashPackages (
  PackageID    INT UNSIGNED NOT NULL,
  Name         VARCHAR(64)  NOT NULL,               -- ornek: "Starter 500 Cash"
  AmountCents  INT UNSIGNED NOT NULL,               -- ornek 499 = $4.99
  Currency     CHAR(3)      NOT NULL DEFAULT 'USD',
  CashAmount   INT UNSIGNED NOT NULL,               -- verilecek DianJuan (havecount)
  Active       TINYINT      NOT NULL DEFAULT 1,
  CreatedAt    DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (PackageID)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Ana transaction tablosu
CREATE TABLE IF NOT EXISTS PayPalTransactions (
  ID               BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  Account          VARCHAR(32)  NOT NULL,           -- userinfo.username (oyuncu HESAP id)
  PlayerID         VARCHAR(10)  COLLATE utf8mb4_unicode_ci NOT NULL, -- Player.PlayerID ile birebir ayni tip+collation; Account ile ayni deger (canli DB dogrulamasi: Player.PlayerID == userinfo.username)
  CharName         VARCHAR(32)  NULL,               -- bilgi amaçlı
  PackageID        INT UNSIGNED NULL,               -- PENDING'de NULL — paket web sayfasında seçilir
  HowMuchPay       DECIMAL(10,2) NULL,              -- ödenen USD (örn. 50.00); checkout'ta CashPackages fiyatından snapshot, COMPLETED'da capture tutarıyla teyit edilerek yazılır (fiyat değişikliğine karşı koruma)
  Currency         CHAR(3)      NOT NULL DEFAULT 'USD',
  CashAmount       INT UNSIGNED NULL,               -- checkout'ta snapshot
  Status           ENUM('PENDING','COMPLETED','FAILED','CANCELLED','REFUNDED') NOT NULL DEFAULT 'PENDING',
  OrderID          VARCHAR(32)  NULL,               -- PayPal order id (web endpoint yazar)
  CaptureID        VARCHAR(32)  NULL,               -- PayPal capture id
  OneTimeKey       VARCHAR(64)  NOT NULL,           -- checkout oturum anahtarı (WebMarketKey örüntüsü, tek kullanımlık)
  KeyExpiresAt     DATETIME     NOT NULL,           -- gameserver 15 dakika verir
  WebhookEventID   VARCHAR(64)  NULL,               -- idempotency: PAYMENT.CAPTURE.COMPLETED event id
  PayerEmail       VARCHAR(128) NULL,
  CreatedAt        DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
  UpdatedAt        DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PayDate          DATETIME     NULL DEFAULT NULL,  -- ödeme tamamlanma anı; COMPLETED'a geçişte NOW() set edilir, aksi halde NULL
  PRIMARY KEY (ID),
  UNIQUE KEY UQ_PayPalTransactions_OrderID (OrderID),
  UNIQUE KEY UQ_PayPalTransactions_OneTimeKey (OneTimeKey),
  UNIQUE KEY UQ_PayPalTransactions_WebhookEventID (WebhookEventID),
  KEY IDX_PayPalTransactions_Account (Account, Status),
  KEY IDX_PayPalTransactions_PlayerID (PlayerID, Status),
  KEY IDX_PayPalTransactions_Status (Status, CreatedAt)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Ham webhook log (denetim / dispute kanıtı)
CREATE TABLE IF NOT EXISTS PayPalWebhookLog (
  ID           BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  EventID      VARCHAR(64) NULL,
  EventType    VARCHAR(64) NOT NULL,
  OrderID      VARCHAR(32) NULL,
  Verified     TINYINT     NOT NULL DEFAULT 0,
  VerifyDetail VARCHAR(255) NULL,
  RawBody      MEDIUMTEXT  NULL,
  Headers      TEXT        NULL,
  RemoteIP     VARCHAR(45) NULL,
  CreatedAt    DATETIME    NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (ID),
  KEY IDX_PayPalWebhookLog_OrderID (OrderID),
  KEY IDX_PayPalWebhookLog_EventID (EventID)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Örnek paketler (USD) — rakamlar placeholder, kullanıcı düzenleyecek.
-- 5/10/25/50 USD -> 500/1100/3000/7000 Cash (bonus oranları yaklaşık %0/%10/%20/%40)
-- INSERT IGNORE: tekrar çalıştırmada mevcut (muhtemelen düzenlenmiş) paketler ezilmez.
INSERT IGNORE INTO CashPackages (PackageID, Name, AmountCents, Currency, CashAmount, Active) VALUES
  (1, 'Starter 500 Cash',    500, 'USD',  500, 1),
  (2, 'Value 1100 Cash',    1000, 'USD', 1100, 1),
  (3, 'Pro 3000 Cash',      2500, 'USD', 3000, 1),
  (4, 'Ultimate 7000 Cash', 5000, 'USD', 7000, 1);

-- Web endpoint için least-privilege kullanıcı (security_ban_tables.sql örüntüsü).
-- Gameserver kendi ana DB kullanıcısıyla INSERT/SELECT yapar; bu kullanıcı
-- yalnızca gelecekteki web endpoint'i (checkout + webhook) içindir.
-- userinfo'da kolon-seviyesi GRANT: sadece havecount kredisi için gerekli alanlar.
CREATE USER IF NOT EXISTS 'phoenix_paypal'@'localhost' IDENTIFIED BY 'CHANGE_ME';
ALTER USER 'phoenix_paypal'@'localhost' IDENTIFIED BY 'CHANGE_ME';
GRANT SELECT ON DARKEDEN.CashPackages TO 'phoenix_paypal'@'localhost';
GRANT SELECT, INSERT, UPDATE ON DARKEDEN.PayPalTransactions TO 'phoenix_paypal'@'localhost';
GRANT INSERT ON DARKEDEN.PayPalWebhookLog TO 'phoenix_paypal'@'localhost';
GRANT SELECT (username, havecount), UPDATE (havecount) ON DARKEDEN.userinfo TO 'phoenix_paypal'@'localhost';
FLUSH PRIVILEGES;
