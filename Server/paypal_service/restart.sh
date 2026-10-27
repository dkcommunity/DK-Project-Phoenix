#!/usr/bin/env bash
# PayPal FastAPI servisini yeniden yükler (.env / kod değişince).
# Kullanım (VPS veya LAN, phoenix kullanıcısı):
#   cd /home/phoenix/phoenix/ServerSRC/paypal_service
#   ./restart.sh
#
# sudo şifresi gerekmez: unit User=phoenix + Restart=always ise süreç kill
# yeter. sudo -n varsa systemctl restart kullanılır.

set -euo pipefail

UNIT="paypal"
HEALTH_URL="http://127.0.0.1:8000/health"

wait_healthy() {
    local i pid
    for i in $(seq 1 20); do
        if curl -fs -o /dev/null "$HEALTH_URL" 2>/dev/null; then
            pid="$(systemctl show -p MainPID --value "$UNIT")"
            echo "paypal: active pid=${pid} health=200"
            return 0
        fi
        sleep 1
    done
    echo "paypal: health check failed after restart" >&2
    systemctl is-active "$UNIT" || true
    return 1
}

if sudo -n systemctl restart "$UNIT" 2>/dev/null; then
    echo "paypal: systemctl restart"
    wait_healthy
    exit 0
fi

PID="$(systemctl show -p MainPID --value "$UNIT")"
if [[ -z "${PID}" || "${PID}" == "0" ]]; then
    echo "paypal: not running (MainPID=0). Start with: sudo systemctl start ${UNIT}" >&2
    exit 1
fi

echo "paypal: no passwordless sudo — killing pid=${PID} (Restart=always)"
kill "${PID}"
for i in $(seq 1 20); do
    if ! kill -0 "${PID}" 2>/dev/null; then
        break
    fi
    sleep 0.5
done
if kill -0 "${PID}" 2>/dev/null; then
    echo "paypal: old pid ${PID} still running after kill" >&2
    exit 1
fi
wait_healthy
