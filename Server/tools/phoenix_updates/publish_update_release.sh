#!/usr/bin/env bash
set -euo pipefail

DEFAULT_SOURCE_DIR="/home/phoenix/update_publish"
DEFAULT_WEB_ROOT="/var/www/phoenix-updates"

SOURCE_DIR="${PHOENIX_UPDATE_SOURCE:-${DEFAULT_SOURCE_DIR}}"
WEB_ROOT="${PHOENIX_UPDATE_WEB_ROOT:-${DEFAULT_WEB_ROOT}}"
VERSION=""
SET_DB_VERSION=0
DB_HOST="127.0.0.1"
DB_USER="elcastle"
DB_NAME="DARKEDEN"
EMPTY_RELEASE=0

copy_tree_contents() {
  local source="$1"
  local target="$2"

  install -d -m 0755 "${target}"
  if command -v rsync >/dev/null 2>&1; then
    rsync -a "${source}/" "${target}/"
  else
    cp -a "${source}/." "${target}/"
  fi
}

read_manifest_version() {
  local manifest_path="$1"

  if ! command -v python3 >/dev/null 2>&1; then
    echo "python3 is required to read manifest.json version." >&2
    return 1
  fi

  MANIFEST_PATH="${manifest_path}" python3 - <<'PY'
import json
import os
import sys

path = os.environ["MANIFEST_PATH"]
try:
    with open(path, "r", encoding="utf-8") as handle:
        manifest = json.load(handle)
except Exception as exc:
    print(f"failed to read manifest.json: {exc}", file=sys.stderr)
    sys.exit(1)

version = manifest.get("version")
if isinstance(version, int) and version >= 0:
    print(version)
elif isinstance(version, str) and version.isdigit():
    print(version)
else:
    print("manifest.json does not contain a numeric version field.", file=sys.stderr)
    sys.exit(1)
PY
}

validate_staged_manifest() {
  local staged_root="$1"
  local empty_release="${2:-0}"

  STAGED_ROOT="${staged_root}" EMPTY_RELEASE="${empty_release}" python3 - <<'PY'
import hashlib
import json
import os
import sys
from pathlib import Path

root = Path(os.environ["STAGED_ROOT"]).resolve()
empty_release = os.environ.get("EMPTY_RELEASE") == "1"
manifest_path = root / "manifest.json"

try:
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
except Exception as exc:
    print(f"failed to read staged manifest: {exc}", file=sys.stderr)
    sys.exit(1)

files = manifest.get("files")
if not isinstance(files, list):
    print("staged manifest does not contain a files array.", file=sys.stderr)
    sys.exit(1)

if empty_release and files:
    print("--empty-release requires manifest files array to be empty.", file=sys.stderr)
    sys.exit(1)

if empty_release:
    files_root = root / "files"
    if files_root.exists():
        unexpected = [path for path in files_root.rglob("*") if path.is_file()]
        if unexpected:
            first = unexpected[0].relative_to(root).as_posix()
            print(f"--empty-release source must not contain patch files; found {first}", file=sys.stderr)
            sys.exit(1)

def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()

def fail(message: str) -> None:
    print(message, file=sys.stderr)
    sys.exit(1)

checked = 0
for index, entry in enumerate(files, start=1):
    if not isinstance(entry, dict):
        fail(f"manifest file entry #{index} is not an object.")
    rel_path = str(entry.get("path", "")).replace("\\", "/").strip()
    url = str(entry.get("url", "")).replace("\\", "/").strip()
    if not rel_path or rel_path.startswith("/") or any(part in ("", ".", "..") for part in rel_path.split("/")):
        fail(f"unsafe manifest path: {rel_path!r}")
    if not url:
        url = f"files/{rel_path}"
    if url.startswith(("http://", "https://")) or url.startswith("/"):
        fail(f"absolute manifest URLs are not allowed for cumulative publish: {rel_path}")
    expected_url = f"files/{rel_path}"
    if url != expected_url:
        fail(f"manifest url/path mismatch for {rel_path}: {url!r} != {expected_url!r}")

    target = (root / url).resolve()
    try:
        target.relative_to(root)
    except ValueError:
        fail(f"manifest target escapes web root: {rel_path}")
    if not target.is_file():
        fail(f"manifest target missing after cumulative publish: {url}")

    expected_size = entry.get("size")
    if isinstance(expected_size, int) and expected_size >= 0 and target.stat().st_size != expected_size:
        fail(f"size mismatch for {url}: {target.stat().st_size} != {expected_size}")

    expected_hash = str(entry.get("sha256", "")).strip().lower()
    if expected_hash and sha256_file(target) != expected_hash:
        fail(f"sha256 mismatch for {url}")
    checked += 1

print(f"staged manifest validation OK: {checked} file(s)")
PY
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --source)
      SOURCE_DIR="$2"
      shift 2
      ;;
    --web-root)
      WEB_ROOT="$2"
      shift 2
      ;;
    --version)
      VERSION="$2"
      shift 2
      ;;
    --set-db-version)
      SET_DB_VERSION=1
      shift
      ;;
    --empty-release)
      EMPTY_RELEASE=1
      shift
      ;;
    --db-host)
      DB_HOST="$2"
      shift 2
      ;;
    --db-user)
      DB_USER="$2"
      shift 2
      ;;
    --db-name)
      DB_NAME="$2"
      shift 2
      ;;
    -h|--help)
      cat <<EOF
Usage: ./publish_update_release.sh [options]

Options:
  --source DIR          Default: /home/phoenix/update_publish
  --web-root PATH       Default: /var/www/phoenix-updates
  --version VERSION     Default: manifest.json version from --source
  --set-db-version      Also sync DB ClientVersion.Version
  --empty-release       Publish a signed empty manifest and replace the active
                        web root instead of preserving old patch files.
                        Use this when a full client zip is distributed manually.
  --db-host HOST        Default: 127.0.0.1
  --db-user USER        Default: elcastle
  --db-name DB          Default: DARKEDEN

DB password:
  Export MYSQL_PWD before running this script. Example:
  export MYSQL_PWD='password'
EOF
      exit 0
      ;;
    *)
      echo "Unknown option: $1" >&2
      exit 2
      ;;
  esac
done

if [[ -z "${SOURCE_DIR}" ]]; then
  echo "source directory cannot be empty." >&2
  exit 2
fi

if [[ -z "${WEB_ROOT}" || "${WEB_ROOT}" == "/" ]]; then
  echo "web root cannot be empty or /." >&2
  exit 2
fi

if [[ ! -d "${SOURCE_DIR}" ]]; then
  echo "source directory not found: ${SOURCE_DIR}" >&2
  echo "Upload the prepared update release to ${DEFAULT_SOURCE_DIR}, or pass --source DIR." >&2
  exit 1
fi

SOURCE_REAL="$(readlink -f "${SOURCE_DIR}")"
WEB_REAL="$(readlink -f "${WEB_ROOT}" 2>/dev/null || true)"
if [[ -n "${WEB_REAL}" && "${SOURCE_REAL}" == "${WEB_REAL}" ]]; then
  echo "source directory must not be the active web root: ${WEB_ROOT}" >&2
  exit 2
fi
if [[ -n "${WEB_REAL}" && "${SOURCE_REAL}/" == "${WEB_REAL}/"* ]]; then
  echo "source directory must not be inside the active web root: ${WEB_ROOT}" >&2
  exit 2
fi

if [[ ! -f "${SOURCE_DIR}/manifest.json" ]]; then
  echo "manifest.json not found in source: ${SOURCE_DIR}" >&2
  exit 1
fi

MANIFEST_VERSION="$(read_manifest_version "${SOURCE_DIR}/manifest.json")"

if [[ -z "${VERSION}" ]]; then
  VERSION="${MANIFEST_VERSION}"
elif [[ "${VERSION}" != "${MANIFEST_VERSION}" ]]; then
  echo "--version (${VERSION}) does not match manifest.json version (${MANIFEST_VERSION})." >&2
  exit 2
fi

if ! [[ "${VERSION}" =~ ^[0-9]+$ ]]; then
  echo "--version must be numeric." >&2
  exit 2
fi

if [[ ! -f "${SOURCE_DIR}/manifest.sig" ]]; then
  echo "manifest.sig not found in source: ${SOURCE_DIR}" >&2
  echo "Generate a signed release with tools/phoenix_updater/generate_manifest.py --private-key ..." >&2
  exit 1
fi

if [[ "${EMPTY_RELEASE}" -eq 0 && ! -d "${SOURCE_DIR}/files" ]]; then
  echo "files directory not found in source: ${SOURCE_DIR}/files" >&2
  exit 1
fi

install -d -m 0755 "${WEB_ROOT}"
STAGING="${WEB_ROOT}.staging.$$"
rm -rf "${STAGING}"
install -d -m 0755 "${STAGING}"

if [[ "${EMPTY_RELEASE}" -eq 0 ]]; then
  copy_tree_contents "${WEB_ROOT}" "${STAGING}"
fi
copy_tree_contents "${SOURCE_DIR}" "${STAGING}"

test -f "${STAGING}/manifest.json"
test -f "${STAGING}/manifest.sig"
validate_staged_manifest "${STAGING}" "${EMPTY_RELEASE}"

OLD_WEB_ROOT="${WEB_ROOT}.replacing.$$"
cleanup_publish_swap() {
  local status=$?
  if [[ "${status}" -ne 0 ]]; then
    if [[ -d "${OLD_WEB_ROOT}" && ! -d "${WEB_ROOT}" ]]; then
      mv "${OLD_WEB_ROOT}" "${WEB_ROOT}" || true
    fi
    rm -rf "${STAGING}" || true
  fi
  exit "${status}"
}
trap cleanup_publish_swap EXIT

mv "${WEB_ROOT}" "${OLD_WEB_ROOT}"
mv "${STAGING}" "${WEB_ROOT}"
rm -rf "${OLD_WEB_ROOT}"
trap - EXIT

if [[ "${EUID}" -eq 0 ]] && id www-data >/dev/null 2>&1; then
  chown -R www-data:www-data "${WEB_ROOT}"
fi
find "${WEB_ROOT}" -type d -exec chmod 0755 {} \;
find "${WEB_ROOT}" -type f -exec chmod 0644 {} \;

if [[ "${SET_DB_VERSION}" -eq 1 ]]; then
  if ! command -v mysql >/dev/null 2>&1; then
    echo "mysql client not found; DB version was not updated." >&2
    exit 1
  fi

  COUNT=$(mysql -h "${DB_HOST}" -u "${DB_USER}" "${DB_NAME}" -N -B -e "SELECT COUNT(*) FROM ClientVersion;")
  if [[ "${COUNT}" == "0" ]]; then
    mysql -h "${DB_HOST}" -u "${DB_USER}" "${DB_NAME}" -e "INSERT INTO ClientVersion (Version) VALUES (${VERSION});"
  else
    mysql -h "${DB_HOST}" -u "${DB_USER}" "${DB_NAME}" -e "UPDATE ClientVersion SET Version = ${VERSION};"
  fi
fi

echo "Phoenix update release published."
echo "Web root : ${WEB_ROOT}"
echo "Version  : ${VERSION}"
echo "Manifest : ${WEB_ROOT}/manifest.json"
if [[ "${EMPTY_RELEASE}" -eq 1 ]]; then
  echo "Mode     : empty-release"
else
  echo "Mode     : cumulative"
fi
