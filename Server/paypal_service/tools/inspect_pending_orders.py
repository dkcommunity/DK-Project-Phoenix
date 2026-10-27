# Son PayPal siparişlerinin capture status / status_details (secret yazılmaz).
# Kullanım (VPS, paypal_service dizininde):
#   ./venv/bin/python tools/inspect_pending_orders.py

import asyncio
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from dotenv import load_dotenv

load_dotenv(".env")
load_dotenv("/etc/paypal_service/paypal.env")

import httpx
import pymysql
from pymysql.cursors import DictCursor

from app.config import config
from app.paypal_api import _get_access_token


async def inspect() -> None:
    conn = pymysql.connect(
        host=config.db_host,
        port=config.db_port,
        user=config.db_user,
        password=config.db_pass,
        database=config.db_name,
        charset="utf8mb4",
        cursorclass=DictCursor,
    )
    try:
        with conn.cursor() as cur:
            cur.execute(
                "SELECT ID, Status, OrderID, CaptureID"
                " FROM PayPalTransactions ORDER BY ID DESC LIMIT 8"
            )
            rows = cur.fetchall()
    finally:
        conn.close()

    token = await _get_access_token()
    async with httpx.AsyncClient(base_url=config.paypal_api_base, timeout=20.0) as client:
        for row in rows:
            oid = row.get("OrderID")
            print(
                f"txn={row['ID']} db={row['Status']} capture_id={row.get('CaptureID')!r} order={oid!r}"
            )
            if not oid:
                continue
            resp = await client.get(
                f"/v2/checkout/orders/{oid}",
                headers={"Authorization": f"Bearer {token}"},
            )
            if resp.status_code != 200:
                print(f"  GET HTTP {resp.status_code} {resp.text[:180]}")
                continue
            payload = resp.json()
            print(f"  order.status={payload.get('status')}")
            units = payload.get("purchase_units") or []
            payments = (units[0].get("payments") or {}) if units else {}
            captures = payments.get("captures") or []
            if not captures:
                print("  captures=[]")
                continue
            cap = captures[0]
            details = cap.get("status_details") or {}
            print(
                f"  capture.status={cap.get('status')} reason={details.get('reason')!r}"
                f" details={details}"
            )


if __name__ == "__main__":
    asyncio.run(inspect())
