# CashPackages AmountCents -> AmountDollar (5 = $5). Secret yazılmaz.
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from dotenv import load_dotenv

load_dotenv(".env")
load_dotenv("/etc/paypal_service/paypal.env")

import pymysql
from pymysql.cursors import DictCursor

from app.config import config


def _columns(cur):
    cur.execute("SHOW COLUMNS FROM CashPackages")
    return [r["Field"] for r in cur.fetchall()]


def migrate() -> None:
    conn = pymysql.connect(
        host=config.db_host,
        port=config.db_port,
        user=config.db_user,
        password=config.db_pass,
        database=config.db_name,
        charset="utf8mb4",
        cursorclass=DictCursor,
        autocommit=True,
    )
    try:
        with conn.cursor() as cur:
            cols = _columns(cur)
            if "AmountDollar" in cols and "AmountCents" not in cols:
                print("already migrated: AmountDollar present, AmountCents gone")
                cur.execute(
                    "SELECT PackageID, Name, AmountDollar, CashAmount FROM CashPackages ORDER BY PackageID"
                )
                for row in cur.fetchall():
                    print(dict(row))
                return
            if "AmountCents" not in cols:
                raise RuntimeError(f"unexpected CashPackages columns: {cols}")
            if "AmountDollar" not in cols:
                cur.execute(
                    "ALTER TABLE CashPackages ADD COLUMN AmountDollar INT UNSIGNED NULL AFTER Name"
                )
            cur.execute(
                "UPDATE CashPackages SET AmountDollar = AmountCents / 100 WHERE AmountDollar IS NULL"
            )
            cur.execute(
                "SELECT PackageID, AmountCents, AmountDollar FROM CashPackages ORDER BY PackageID"
            )
            rows = cur.fetchall()
            for row in rows:
                cents = int(row["AmountCents"])
                dollars = int(row["AmountDollar"])
                if cents % 100 != 0 or dollars != cents // 100 or dollars < 1:
                    raise RuntimeError(f"unsafe convert {row}")
                print(f"ok PackageID={row['PackageID']} {cents} cents -> {dollars} dollar")
            cur.execute("ALTER TABLE CashPackages MODIFY AmountDollar INT UNSIGNED NOT NULL")
            cur.execute("ALTER TABLE CashPackages DROP COLUMN AmountCents")
            print("migrated columns:", _columns(cur))
    finally:
        conn.close()


if __name__ == "__main__":
    migrate()
