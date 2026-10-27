#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FAIL2BAN_DIR="/etc/fail2ban"
SECURITY_CONF_DIR="/etc/phoenix"
AUDIT_SCRIPT="/usr/local/sbin/phoenix_fail2ban_audit.sh"
LOG_DIR="${PHOENIX_SECURITY_LOG_DIR:-/home/phoenix/phoenix/ServerSRC/bin/log}"
BANACTION="${PHOENIX_FAIL2BAN_BANACTION:-nftables-multiport}"
SERVER_USER="${PHOENIX_SERVER_USER:-phoenix}"

if [ "$(id -u)" -ne 0 ]; then
	echo "Run as root."
	exit 1
fi

install -d "$FAIL2BAN_DIR/filter.d" "$FAIL2BAN_DIR/jail.d" "$FAIL2BAN_DIR/action.d"
install -d "$SECURITY_CONF_DIR"
install -d "$LOG_DIR"
touch "$LOG_DIR/phoenix_packet_abuse.log"
chown "$SERVER_USER:$SERVER_USER" "$LOG_DIR/phoenix_packet_abuse.log" 2>/dev/null || true
chmod 0644 "$LOG_DIR/phoenix_packet_abuse.log"

install -m 0644 "$SCRIPT_DIR/fail2ban/filter.d/phoenix-login-abuse.conf" "$FAIL2BAN_DIR/filter.d/phoenix-login-abuse.conf"
install -m 0644 "$SCRIPT_DIR/fail2ban/action.d/phoenix-db-audit.conf" "$FAIL2BAN_DIR/action.d/phoenix-db-audit.conf"
install -m 0755 "$SCRIPT_DIR/phoenix_fail2ban_audit.sh" "$AUDIT_SCRIPT"

tmp_jail="$(mktemp)"
cp "$SCRIPT_DIR/fail2ban/jail.d/phoenix-login-abuse.local" "$tmp_jail"
sed -i "s/^banaction = .*/banaction = ${BANACTION}/" "$tmp_jail"

if [ -n "${PHOENIX_FAIL2BAN_IGNOREIP:-}" ]; then
	sed -i "s|^ignoreip = .*|ignoreip = 127.0.0.1/8 ::1 10.0.0.0/8 172.16.0.0/12 192.168.0.0/16 ${PHOENIX_FAIL2BAN_IGNOREIP}|" "$tmp_jail"
fi

install -m 0644 "$tmp_jail" "$FAIL2BAN_DIR/jail.d/phoenix-login-abuse.local"
rm -f "$tmp_jail"

if [ ! -f "$SECURITY_CONF_DIR/security-db.conf" ]; then
	install -m 0600 "$SCRIPT_DIR/security-db.conf.example" "$SECURITY_CONF_DIR/security-db.conf"
fi

if [ -n "${PHOENIX_SECURITY_DB_PASS:-}" ]; then
	case "$PHOENIX_SECURITY_DB_PASS" in
		*"'"*|*"\\"*)
			echo "PHOENIX_SECURITY_DB_PASS must not contain single quote or backslash for this installer."
			exit 1
			;;
	esac

	tmp_sql="$(mktemp)"
	escaped_db_pass="$(printf '%s' "$PHOENIX_SECURITY_DB_PASS" | sed 's/[\/&]/\\&/g')"
	sed "s/CHANGE_ME/${escaped_db_pass}/g" "$SCRIPT_DIR/sql/security_ban_tables.sql" > "$tmp_sql"
	mysql DARKEDEN < "$tmp_sql"
	rm -f "$tmp_sql"

	sed -i "s/^DB_PASS=.*/DB_PASS=${PHOENIX_SECURITY_DB_PASS}/" "$SECURITY_CONF_DIR/security-db.conf"
	sed -i "s/^DB_USER=.*/DB_USER=phoenix_security/" "$SECURITY_CONF_DIR/security-db.conf"
	sed -i "s/^DB_NAME=.*/DB_NAME=DARKEDEN/" "$SECURITY_CONF_DIR/security-db.conf"
	sed -i "s/^NOTICE_QUEUE=.*/NOTICE_QUEUE=1/" "$SECURITY_CONF_DIR/security-db.conf"
	chmod 600 "$SECURITY_CONF_DIR/security-db.conf"
fi

if command -v fail2ban-regex >/dev/null 2>&1; then
	fail2ban-regex "$LOG_DIR/phoenix_packet_abuse.log" "$FAIL2BAN_DIR/filter.d/phoenix-login-abuse.conf" || true
fi

systemctl enable fail2ban >/dev/null 2>&1 || true
systemctl restart fail2ban
fail2ban-client status phoenix-login-abuse || true

echo "Phoenix packet-abuse fail2ban jail installed."
