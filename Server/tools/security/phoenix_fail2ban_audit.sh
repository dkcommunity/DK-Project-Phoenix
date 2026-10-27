#!/usr/bin/env bash

# Phoenix fail2ban audit action.
# The firewall ban is handled by fail2ban's banaction; this script records
# the action to text/DB and optionally queues a safe in-game notice.

set -u

ACTION="${1:-}"
REMOTE_IP="${2:-}"
JAIL_NAME="${3:-phoenix-login-abuse}"
SERVICE="${4:-LoginServer}"
REASON="${5:-packet_abuse}"
BAN_SECONDS="${6:-0}"

LOG_DIR="${PHOENIX_SECURITY_LOG_DIR:-/home/phoenix/phoenix/ServerSRC/bin/log}"
CONF_FILE="${PHOENIX_SECURITY_DB_CONF:-/etc/phoenix/security-db.conf}"
TEXT_LOG="${LOG_DIR}/phoenix_security_ban.log"

DB_HOST=""
DB_NAME=""
DB_USER=""
DB_PASS=""
NOTICE_QUEUE="0"
NOTICE_COOLDOWN_SECONDS="300"
NOTICE_MESSAGE="Security: suspicious login traffic was blocked."

if [ -r "$CONF_FILE" ]; then
	# shellcheck source=/dev/null
	. "$CONF_FILE"
fi

mkdir -p "$LOG_DIR" 2>/dev/null || true

now_local() {
	date '+%Y-%m-%d %H:%M:%S'
}

sql_escape() {
	printf '%s' "$1" | sed "s/'/''/g"
}

is_safe_ip() {
	case "$1" in
		*[!0-9a-fA-F:.]*|'') return 1 ;;
		*) return 0 ;;
	esac
}

write_text_log() {
	printf '%s action=%s ip=%s jail=%s service=%s reason=%s bantime=%s\n' \
		"$(now_local)" "$ACTION" "$REMOTE_IP" "$JAIL_NAME" "$SERVICE" "$REASON" "$BAN_SECONDS" >> "$TEXT_LOG" 2>/dev/null || true
}

mysql_exec() {
	sql="$1"

	if [ -z "${DB_HOST:-}" ] || [ -z "${DB_NAME:-}" ] || [ -z "${DB_USER:-}" ]; then
		return 0
	fi

	if ! command -v mysql >/dev/null 2>&1; then
		printf '%s mysql client not found; db audit skipped\n' "$(now_local)" >> "$TEXT_LOG" 2>/dev/null || true
		return 0
	fi

	if [ -n "${DB_PASS:-}" ]; then
		MYSQL_PWD="$DB_PASS" mysql --protocol=tcp -h "$DB_HOST" -u "$DB_USER" "$DB_NAME" --batch --silent -e "$sql" >> "$TEXT_LOG" 2>&1 || true
	else
		mysql --protocol=tcp -h "$DB_HOST" -u "$DB_USER" "$DB_NAME" --batch --silent -e "$sql" >> "$TEXT_LOG" 2>&1 || true
	fi
}

if ! is_safe_ip "$REMOTE_IP"; then
	write_text_log
	exit 0
fi

case "$BAN_SECONDS" in
	*[!0-9]*|'') BAN_SECONDS=0 ;;
esac

write_text_log

escaped_ip="$(sql_escape "$REMOTE_IP")"
escaped_jail="$(sql_escape "$JAIL_NAME")"
escaped_service="$(sql_escape "$SERVICE")"
escaped_reason="$(sql_escape "$REASON")"
escaped_message="$(sql_escape "$NOTICE_MESSAGE")"
escaped_detail="$(sql_escape "Blocked ${REMOTE_IP} for ${REASON}.")"

case "$ACTION" in
	ban)
		mysql_exec "INSERT INTO SecurityBanLog (RemoteIP, Service, Reason, JailName, BanSeconds, BannedAt, CreatedAt) VALUES ('$escaped_ip', '$escaped_service', '$escaped_reason', '$escaped_jail', ${BAN_SECONDS:-0}, NOW(), NOW());"

		if [ "${NOTICE_QUEUE:-0}" = "1" ]; then
			case "${NOTICE_COOLDOWN_SECONDS:-300}" in
				*[!0-9]*|'') NOTICE_COOLDOWN_SECONDS=300 ;;
			esac

			mysql_exec "INSERT INTO SecurityNoticeQueue (Message, DetailMessage, Audience, Source, CreatedAt) SELECT '$escaped_message', '$escaped_detail', 0, 'fail2ban', NOW() FROM DUAL WHERE NOT EXISTS (SELECT ID FROM SecurityNoticeQueue WHERE Source='fail2ban' AND CreatedAt >= DATE_SUB(NOW(), INTERVAL ${NOTICE_COOLDOWN_SECONDS} SECOND) LIMIT 1);"
		fi
		;;

	unban)
		mysql_exec "UPDATE SecurityBanLog SET UnbannedAt=NOW() WHERE RemoteIP='$escaped_ip' AND UnbannedAt IS NULL ORDER BY ID DESC LIMIT 1;"
		;;
esac

exit 0
