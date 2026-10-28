# Canlı PayPalCashPackages satırlarını basar (secret yok).
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

conn = pymysql.connect(
    host=config.db_host,
    port=config.db_port,
    user=config.db_user,
    password=config.db_pass,
    database=config.db_name,
    charset="utf8mb4",
    cursorclass=DictCursor,
)
with conn.cursor() as cur:
    cur.execute("SHOW COLUMNS FROM PayPalCashPackages")
    print("COLUMNS:", [r["Field"] for r in cur.fetchall()])
    cur.execute("SELECT * FROM PayPalCashPackages ORDER BY PackageID")
    for row in cur.fetchall():
        print(dict(row))
conn.close()
