# Project Phoenix Update Endpoint

This directory contains the tools for setting up the static update endpoint required by the new `Updater.exe` and publishing client releases.

## Architecture

- The client `Updater.exe` reads these URLs:
  - `https://<host>/updates/manifest.json`
  - `https://<host>/updates/manifest.sig`
  - Or the `ManifestUrl` / `PhoenixManifestUrl` values in the client's `data/info/updateclient.inf`.
- The web server only serves static files.
- The game `LoginServer` still compares the client version against the database value `ClientVersion.Version`.
- After publishing a release, `ClientVersion.Version` must match the manifest's `version` value.
- `manifest.sig` is an ECDSA P-256 / SHA-256 signature of the raw bytes of `manifest.json`.

## Installation

On the server:

```bash
cd /home/phoenix/phoenix/ServerSRC/tools/phoenix_updates
chmod +x install_update_endpoint.sh publish_update_release.sh
sudo ./install_update_endpoint.sh --server-name 89.252.164.244 --web-root /var/www/phoenix-updates
```

If an HTTPS certificate is ready:

```bash
sudo ./install_update_endpoint.sh \
  --server-name patch.project-phoenix.example \
  --web-root /var/www/phoenix-updates \
  --ssl-cert /etc/letsencrypt/live/patch.project-phoenix.example/fullchain.pem \
  --ssl-key /etc/letsencrypt/live/patch.project-phoenix.example/privkey.pem
```

This command:

- Creates `/var/www/phoenix-updates`.
- Writes an Nginx `location` configuration for `/updates/`.
- Serves the same endpoint over HTTPS if a certificate and key are provided.
- Rejects methods other than `GET` and `HEAD`.
- Disables access to dotfiles.
- Tests the Nginx configuration and reloads Nginx.

If `certbot --nginx --redirect` has been used on the same VPS as PayPal, the HTTP block may return `404` for the IP host. In that case, the Python GUI (`http://VPS_IP/updates`) and `Updater.exe` (`HttpUrl` in `updateclient.inf`) will receive a 404. The canonical setup is `phoenix-updates.conf.example` (DuckDNS HTTP-to-HTTPS, IP HTTP `/updates/` GET/HEAD, and both `/updates/` and PayPal over HTTPS). Without `--force`, `install_update_endpoint.sh` will not overwrite a site marked as managed by PayPal/Certbot.

Live manifest URL (GUI default): `https://dklunox.duckdns.org/updates`. IP endpoint: `http://169.58.161.68/updates`.

The HTTPS landing page at `https://dklunox.duckdns.org/` is served from `/var/www/dklunox-home/` (source: `ServerSRC/tools/dklunox_home/index.html`). `location = /` and `location /` serve static files; `/updates/`, `/pay/`, `/health`, and `/paypal/` are not shadowed. `sites-enabled/default` remains enabled.

## Publishing

Generate the manifest on Windows:

```powershell
python "F:\Project Phoenix\tools\phoenix_updater\generate_manifest.py" `
  --root "F:\Project Phoenix\GameData" `
  --out "F:\Project Phoenix\update_publish" `
  --base-url "https://89.252.164.244/updates" `
  --version 370 `
  --private-key "F:\Project Phoenix\tools\phoenix_updater\phoenix_manifest_private_key.pem"
```

Keep the private key local. The public key is embedded in `Updater.exe`.

Upload the `update_publish` directory to the server:

```bash
rsync -avz "F:/Project Phoenix/update_publish/" root@89.252.164.244:/home/phoenix/update_publish/
```

Publish the release on the server:

```bash
cd /home/phoenix/phoenix/ServerSRC/tools/phoenix_updates
sudo ./publish_update_release.sh
```

Defaults:

- Source: `/home/phoenix/update_publish`
- Web root: `/var/www/phoenix-updates`
- Version: the `version` value in `/home/phoenix/update_publish/manifest.json`

Publishing is cumulative:

- `/var/www/phoenix-updates` is the active patch pool.
- Old patch files are not deleted or moved into a `.previous` directory.
- The script first copies the active web root into a staging directory.
- It then overlays the new release files from `/home/phoenix/update_publish`.
- It verifies that every `files/...` entry in `manifest.json` exists in staging and has the expected size and hash.
- If validation succeeds, staging becomes the active `/var/www/phoenix-updates`.

Therefore, the `manifest.json` generated on Windows must include all current patch files managed by the updater, not only the files selected today. The Patch Manifest GUI does this by reading the current signed live manifest and merging the newly selected files into it. The output package's `files/` directory contains only newly selected or changed files; older files are preserved from the active patch pool on the VPS.

## Empty release / full-client ZIP distribution

Sometimes no patch will be published, and players will instead receive a full client ZIP manually. In this case, the old cumulative patch pool can be dangerous: when a player launches the new full client through `Updater.exe`, files remaining in the old manifest may be downloaded again and overwrite the new files.

Generate a signed empty manifest:

```powershell
python "F:\Project Phoenix\tools\phoenix_updater\generate_manifest.py" `
  --empty `
  --out "F:\Project Phoenix\update_publish" `
  --base-url "https://89.252.164.244/updates" `
  --version 371 `
  --private-key "F:\Project Phoenix\tools\phoenix_updater\phoenix_manifest_private_key.pem"
```

Upload the `update_publish` directory to the server, then publish in empty-release mode:

```bash
cd /home/phoenix/phoenix/ServerSRC/tools/phoenix_updates
sudo ./publish_update_release.sh --empty-release
```

The `--empty-release` mode is not cumulative:

- It replaces the active `/var/www/phoenix-updates` patch pool without preserving old files.
- The `files` list in `manifest.json` must be empty.
- Publishing fails if there are patch files under `files/` in the source.
- The updater verifies the manifest, finds no files to download, and starts the client.

Players who have not launched the game for a long time will not be updated through the patch chain when this mode is used; they must receive the new full-client ZIP.

To override the source directory or version:

```bash
sudo ./publish_update_release.sh \
  --source /home/phoenix/update_publish \
  --web-root /var/www/phoenix-updates \
  --version 370
```

To update the database version in the same operation:

```bash
export MYSQL_PWD='DB_PASSWORD'
sudo -E ./publish_update_release.sh \
  --set-db-version \
  --db-name DARKEDEN \
  --db-user elcastle \
  --db-host 127.0.0.1
unset MYSQL_PWD
```

## Verification

```bash
curl -I http://127.0.0.1/updates/manifest.json
curl -I http://127.0.0.1/updates/manifest.sig
curl http://127.0.0.1/updates/manifest.json | head
mysql -u elcastle -p DARKEDEN -e "SELECT Version FROM ClientVersion;"
```

## Security notes

- This endpoint does not accept uploads; it only serves static files.
- `manifest.json`, `manifest.sig`, and the patch files are generated locally before publishing.
- Using a domain with HTTPS is recommended for production.
- By default, the updater requires a valid manifest signature. `--optional-signature` or `RequireManifestSignature : 0` may be used for temporary testing, but not in production.
