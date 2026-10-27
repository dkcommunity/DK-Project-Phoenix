#!/usr/bin/env bash
set -euo pipefail

WEB_ROOT="/var/www/phoenix-updates"
SERVER_NAME="_"
LISTEN_PORT="80"
HTTPS_PORT="443"
SITE_NAME="phoenix-updates"
SSL_CERT=""
SSL_KEY=""
FORCE_OVERWRITE="0"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --web-root)
      WEB_ROOT="$2"
      shift 2
      ;;
    --server-name)
      SERVER_NAME="$2"
      shift 2
      ;;
    --listen-port)
      LISTEN_PORT="$2"
      shift 2
      ;;
    --https-port)
      HTTPS_PORT="$2"
      shift 2
      ;;
    --site-name)
      SITE_NAME="$2"
      shift 2
      ;;
    --ssl-cert)
      SSL_CERT="$2"
      shift 2
      ;;
    --ssl-key)
      SSL_KEY="$2"
      shift 2
      ;;
    --force)
      FORCE_OVERWRITE="1"
      shift
      ;;
    -h|--help)
      cat <<EOF
Usage: sudo ./install_update_endpoint.sh [options]

Options:
  --web-root PATH       Default: /var/www/phoenix-updates
  --server-name NAME    Default: _
  --listen-port PORT    Default: 80
  --https-port PORT     Default: 443
  --site-name NAME      Default: phoenix-updates
  --ssl-cert PATH       Optional TLS certificate path
  --ssl-key PATH        Optional TLS private key path
  --force               Overwrite a site that already has PayPal or Certbot markers
EOF
      exit 0
      ;;
    *)
      echo "Unknown option: $1" >&2
      exit 2
      ;;
  esac
done

if [[ "${EUID}" -ne 0 ]]; then
  echo "This script must run as root because it writes nginx config." >&2
  exit 1
fi

if ! command -v nginx >/dev/null 2>&1; then
  echo "nginx is not installed. Install it first: apt update && apt install nginx" >&2
  exit 1
fi

install -d -m 0755 "${WEB_ROOT}"
install -d -m 0755 /etc/nginx/sites-available /etc/nginx/sites-enabled

SITE_AVAILABLE="/etc/nginx/sites-available/${SITE_NAME}.conf"
SITE_ENABLED="/etc/nginx/sites-enabled/${SITE_NAME}.conf"

if [[ -f "${SITE_AVAILABLE}" ]] && grep -qE 'paypal|managed by Certbot' "${SITE_AVAILABLE}"; then
  if [[ "${FORCE_OVERWRITE}" != "1" ]]; then
    echo "Refusing to overwrite ${SITE_AVAILABLE}: PayPal locations or Certbot markers present." >&2
    echo "Canonical combined layout is phoenix-updates.conf.example (HTTP IP /updates/ + HTTPS PayPal)." >&2
    echo "Pass --force only if you intend to replace that site." >&2
    exit 1
  fi
fi

TLS_BLOCK=""
if [[ -n "${SSL_CERT}" || -n "${SSL_KEY}" ]]; then
  if [[ -z "${SSL_CERT}" || -z "${SSL_KEY}" ]]; then
    echo "--ssl-cert and --ssl-key must be used together." >&2
    exit 2
  fi
  if [[ ! -f "${SSL_CERT}" ]]; then
    echo "TLS certificate not found: ${SSL_CERT}" >&2
    exit 1
  fi
  if [[ ! -f "${SSL_KEY}" ]]; then
    echo "TLS private key not found: ${SSL_KEY}" >&2
    exit 1
  fi
  TLS_BLOCK="    listen ${HTTPS_PORT} ssl;
    ssl_certificate ${SSL_CERT};
    ssl_certificate_key ${SSL_KEY};
    ssl_protocols TLSv1.2 TLSv1.3;
    ssl_prefer_server_ciphers off;"
fi

cat > "${SITE_AVAILABLE}" <<EOF
server {
    listen ${LISTEN_PORT};
${TLS_BLOCK}
    server_name ${SERVER_NAME};

    access_log /var/log/nginx/${SITE_NAME}.access.log;
    error_log  /var/log/nginx/${SITE_NAME}.error.log warn;

    location /updates/ {
        alias ${WEB_ROOT}/;
        autoindex off;
        try_files \$uri =404;

        limit_except GET HEAD {
            deny all;
        }

        add_header X-Content-Type-Options "nosniff" always;
        add_header Cache-Control "public, max-age=60" always;
    }

    location ~ /\\. {
        deny all;
    }
}
EOF

ln -sfn "${SITE_AVAILABLE}" "${SITE_ENABLED}"

if id www-data >/dev/null 2>&1; then
  chown -R www-data:www-data "${WEB_ROOT}"
fi

nginx -t
if command -v systemctl >/dev/null 2>&1; then
  systemctl reload nginx
else
  nginx -s reload
fi

echo "Phoenix update endpoint installed."
echo "Web root : ${WEB_ROOT}"
echo "URL      : http://${SERVER_NAME}/updates/manifest.json"
if [[ -n "${SSL_CERT}" ]]; then
  echo "HTTPS    : https://${SERVER_NAME}/updates/manifest.json"
fi
