-- ============================================================================
-- 2026-08-20_gdr_lair_summon_wave.sql
-- GDR Lair (zone 1412) SUMMON_MONSTER wave listesi — SQL-driven config
-- Tasarim: Dev_Log/Server/gdr_lair_sql_summon_wave_2026-08-20.md
--
-- UYARI: Bu dosya 2026-08-20 itibariyla VPS / LAN 192.168.1.5'e
-- UYGULANMADI. Gameserver da derlenmedi. Combined GS build
-- (InventorySlot EventGiftBox assertion fix ile birlikte) sonrasi.
--
-- Uygulama (VPS + LAN 192.168.1.5, kural 12) — henuz yapma:
--   mysql DARKEDEN < 2026-08-20_gdr_lair_summon_wave.sql
--
-- Nasil kullanilir:
--  * Wave sayisi = DISTINCT WaveIndex (1,2,3...; bosluk atlanir, sira ORDER BY).
--  * Bir wave'e monster eklemek: ayni WaveIndex + yeni Slot satiri.
--  * Wave silmek: o WaveIndex satirlarini DELETE.
--  * Spawn noktasi satir bazli (varsayilan 78,89 — mevcut hardcoded nokta).
--  * Gameserver her GDR_LAIR_SUMMON_MONSTER start'inda tabloyu yeniden okur
--    (raid arasi SQL edit, GS restart gerektirmez). Tablo yoksa GS init
--    built-in seed fallback kullanir (DBError.log + GDRLair.log).
--
-- Seed = rallser 10-wave listesi (applyDefaultSummonWaves + bu INSERT).
-- Mekanik degismez; veri kaynagi SQL'e tasinir.
-- ============================================================================

CREATE TABLE IF NOT EXISTS GDRLairSummonWave (
  WaveIndex    TINYINT UNSIGNED NOT NULL,           -- 1-based wave numarası
  Slot         TINYINT UNSIGNED NOT NULL,           -- wave içi sıra (0..)
  MonsterType  INT UNSIGNED     NOT NULL,           -- MonsterInfo.MonsterType
  MonsterNum   INT UNSIGNED     NOT NULL,           -- aynı noktaya basılacak adet
  X            SMALLINT         NOT NULL DEFAULT 78,
  Y            SMALLINT         NOT NULL DEFAULT 89,
  Note         VARCHAR(64)      NOT NULL DEFAULT '',
  PRIMARY KEY (WaveIndex, Slot),
  KEY IDX_GDRLairSummonWave_Type (MonsterType)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Idempotent seed: tablo boşsa mevcut 10-wave listeyi yaz.
INSERT INTO GDRLairSummonWave (WaveIndex, Slot, MonsterType, MonsterNum, X, Y, Note)
SELECT * FROM (
  SELECT 1 AS WaveIndex, 0 AS Slot, 1377 AS MonsterType, 20 AS MonsterNum, 78 AS X, 89 AS Y, 'Maze Guard' AS Note
  UNION ALL SELECT 1, 1, 1386, 10, 78, 89, 'Outrage Specieman'
  UNION ALL SELECT 1, 2, 1398, 10, 78, 89, 'Tragedy Gardener'
  UNION ALL SELECT 2, 0, 1410, 20, 78, 89, 'Cruel Rat'
  UNION ALL SELECT 2, 1, 1422, 10, 78, 89, 'Cerebral Touch'
  UNION ALL SELECT 2, 2, 1432, 20, 78, 89, 'Topf'
  UNION ALL SELECT 3, 0, 888, 20, 78, 89, 'Hell Rados'
  UNION ALL SELECT 3, 1, 889, 20, 78, 89, ''
  UNION ALL SELECT 3, 2, 890, 2, 78, 89, ''
  UNION ALL SELECT 4, 0, 1459, 10, 78, 89, ''
  UNION ALL SELECT 4, 1, 1460, 20, 78, 89, ''
  UNION ALL SELECT 4, 2, 1461, 10, 78, 89, ''
  UNION ALL SELECT 5, 0, 1462, 10, 78, 89, ''
  UNION ALL SELECT 5, 1, 1463, 5, 78, 89, ''
  UNION ALL SELECT 5, 2, 1464, 20, 78, 89, ''
  UNION ALL SELECT 6, 0, 1465, 20, 78, 89, ''
  UNION ALL SELECT 6, 1, 1579, 10, 78, 89, ''
  UNION ALL SELECT 6, 2, 1490, 20, 78, 89, ''
  UNION ALL SELECT 7, 0, 1653, 10, 78, 89, ''
  UNION ALL SELECT 7, 1, 1578, 20, 78, 89, ''
  UNION ALL SELECT 7, 2, 1653, 5, 78, 89, '1653 ikinci slot (mevcut kodla aynı)'
  UNION ALL SELECT 8, 0, 1655, 10, 78, 89, ''
  UNION ALL SELECT 8, 1, 1700, 10, 78, 89, ''
  UNION ALL SELECT 8, 2, 1697, 10, 78, 89, ''
  UNION ALL SELECT 9, 0, 1699, 4, 78, 89, ''
  UNION ALL SELECT 9, 1, 1500, 10, 78, 89, ''
  UNION ALL SELECT 10, 0, 1466, 1, 78, 89, 'Wandering Camilla'
  UNION ALL SELECT 10, 1, 1577, 3, 78, 89, ''
) AS seed
WHERE NOT EXISTS (SELECT 1 FROM GDRLairSummonWave LIMIT 1);
