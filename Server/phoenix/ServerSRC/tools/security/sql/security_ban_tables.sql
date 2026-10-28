CREATE TABLE IF NOT EXISTS SecurityBanLog (
  ID BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  RemoteIP VARCHAR(45) NOT NULL,
  RemotePort INT NULL,
  Service VARCHAR(32) NOT NULL,
  Reason VARCHAR(128) NOT NULL,
  JailName VARCHAR(64) NULL,
  PacketID INT NULL,
  PacketName VARCHAR(64) NULL,
  PacketSize BIGINT UNSIGNED NULL,
  PlayerID VARCHAR(32) NULL,
  MatchCount INT NOT NULL DEFAULT 1,
  BanSeconds INT NOT NULL DEFAULT 0,
  RawLine TEXT NULL,
  CreatedAt DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  BannedAt DATETIME NULL,
  UnbannedAt DATETIME NULL,
  PRIMARY KEY (ID),
  KEY IDX_SecurityBanLog_RemoteIP (RemoteIP),
  KEY IDX_SecurityBanLog_CreatedAt (CreatedAt),
  KEY IDX_SecurityBanLog_BannedAt (BannedAt)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS SecurityNoticeQueue (
  ID BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  Message VARCHAR(255) NOT NULL,
  DetailMessage VARCHAR(255) NULL,
  Audience TINYINT NOT NULL DEFAULT 0 COMMENT '0=all players, 1=admin only reserved',
  Source VARCHAR(64) NOT NULL DEFAULT 'security',
  CreatedAt DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  Processed TINYINT NOT NULL DEFAULT 0,
  ProcessedAt DATETIME NULL,
  PRIMARY KEY (ID),
  KEY IDX_SecurityNoticeQueue_Processed (Processed, ID),
  KEY IDX_SecurityNoticeQueue_SourceCreated (Source, CreatedAt)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE USER IF NOT EXISTS 'phoenix_security'@'localhost' IDENTIFIED BY 'CHANGE_ME';
ALTER USER 'phoenix_security'@'localhost' IDENTIFIED BY 'CHANGE_ME';
GRANT INSERT, UPDATE, SELECT ON DARKEDEN.SecurityBanLog TO 'phoenix_security'@'localhost';
GRANT INSERT, UPDATE, SELECT ON DARKEDEN.SecurityNoticeQueue TO 'phoenix_security'@'localhost';
FLUSH PRIVILEGES;
