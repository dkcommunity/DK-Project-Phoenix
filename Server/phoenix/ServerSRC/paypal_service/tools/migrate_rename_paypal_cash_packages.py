# CashPackages -> PayPalCashPackages. Secret yazılmaz.
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


def _tables(cur) -> list:
    cur.execute("SHOW TABLES")
    return [next(iter(row.values())) for row in cur.fetchall()]


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
            tables = _tables(cur)
            has_old = "CashPackages" in tables
            has_new = "PayPalCashPackages" in tables
            if has_new and not has_old:
                print("already renamed: PayPalCashPackages")
            elif has_old and has_new:
                raise RuntimeError("both CashPackages and PayPalCashPackages exist")
            elif has_old:
                cur.execute("RENAME TABLE CashPackages TO PayPalCashPackages")
                print("renamed CashPackages -> PayPalCashPackages")
            else:
                raise RuntimeError(f"neither table found; tables sample={tables[:20]}")
            try:
                cur.execute(
                    "GRANT SELECT ON DARKEDEN.PayPalCashPackages TO 'phoenix_paypal'@'localhost'"
                )
                cur.execute("FLUSH PRIVILEGES")
                print("GRANT SELECT phoenix_paypal ok")
            except pymysql.err.OperationalError as exc:
                print(f"GRANT skipped ({exc.args[0]}): {exc.args[1]}")
            cur.execute(
                "SELECT PackageID, Name, AmountDollar, CashAmount, Active"
                " FROM PayPalCashPackages ORDER BY PackageID"
            )
            for row in cur.fetchall():
                print(dict(row))
    finally:
        conn.close()


if __name__ == "__main__":
    migrate()
