# Phoenix Client Update Service

This directory contains the Nginx endpoint installer and the release publishing
script for the Phoenix client updater. The service publishes signed manifests
and static patch files; it does not accept uploads from clients.

## Paths and request flow

Keep these three directories separate:

| Purpose | Server path |
|---|---|
| Installer and publisher scripts | `/home/phoenix/phoenix_updates/` |
| Incoming signed release package | `/home/phoenix/update_publish/` |
| Active Nginx document root | `/var/www/phoenix-updates/` |

Nginx maps the public `/updates/` URL prefix to the active document root:

```text
https://updates.example.com/updates/manifest.json
https://updates.example.com/updates/manifest.sig
https://updates.example.com/updates/files/<patch-path>
```

Do **not** put the scripts, signing key, or incoming package in
`/var/www/phoenix-updates/`. That directory is the public, read-only download
area. The default document root is `/var/www/phoenix-updates`; the installer
and publisher both accept an explicit `--web-root` override, but production
deployments should use the default path unless Nginx is deliberately configured
for another location.

The client updater reads `data/info/updateclient.inf` next to `Updater.exe`.
It selects the manifest URL in this order:

1. `ManifestUrl`
2. `PhoenixManifestUrl`
3. Legacy `HttpUrl`, then `HttpUrl2`, with `/updates/manifest.json` appended

The signature URL defaults to `manifest.sig` beside the selected manifest;
`ManifestSignatureUrl` can override it. Manifest signature verification is
enabled by default.

**Check the client configuration before distributing an updater.** The current
repository snapshot at `GameData/data/info/updateclient.inf` has legacy
`HttpUrl` and `HttpUrl2` values for `169.58.161.68` and does not define
`ManifestUrl`. With that file, the updater will use the legacy host rather than
the update endpoint currently configured on the VPS. Set an explicit endpoint
in the client configuration, for example:

```text
ManifestUrl : https://updates.example.com/updates/manifest.json
ManifestSignatureUrl : https://updates.example.com/updates/manifest.sig
RequireManifestSignature : 1
```

Replace the example hostname with the production hostname and ensure its DNS
and TLS certificate are configured. Do not disable signature verification in
production. The updater's embedded public key must match the private key used
to sign releases; keep that private key off the server and out of Git.

## Requirements

- A supported Linux host with `systemd` (or an Nginx installation that can be
  reloaded with `nginx -s reload`).
- Nginx installed and running.
- Python 3 on the host for release manifest validation.
- `rsync` on the upload workstation and host for the example transfer command.
  The publisher itself falls back to `cp` if host-side `rsync` is unavailable.
- DNS pointed at the host and inbound TCP access to ports 80 and 443 as used.
- A trusted release-generation process that creates a signed
  `manifest.json`, its matching `manifest.sig`, and the referenced `files/`
  payload. The manifest-generation tool and signing private key are not
  included in this server-tools directory.

## Install or configure the Nginx endpoint

The installer creates the web root, writes an Nginx site configuration,
enables it, tests the configuration, and reloads Nginx. It does not install
Nginx or obtain a TLS certificate.

First inspect the active Nginx configuration. If an existing virtual host
already handles the desired hostname and port, add the `/updates/` location to
that server block rather than creating a competing catch-all server:

```nginx
location /updates/ {
    alias /var/www/phoenix-updates/;
    autoindex off;
    try_files $uri =404;

    limit_except GET HEAD {
        deny all;
    }

    add_header X-Content-Type-Options "nosniff" always;
    add_header Cache-Control "public, max-age=60" always;
}

location ~ /\. {
    deny all;
}
```

For a new, dedicated Nginx site, run the installer as root. The script is
invoked with `bash` because the checked-in server copy may not have its
executable bit set:

```bash
sudo apt update
sudo apt install -y nginx python3

sudo bash /home/phoenix/phoenix_updates/install_update_endpoint.sh \
  --server-name updates.example.com \
  --web-root /var/www/phoenix-updates
```

For HTTPS, provision a valid certificate first, then provide both certificate
paths:

```bash
sudo bash /home/phoenix/phoenix_updates/install_update_endpoint.sh \
  --server-name updates.example.com \
  --web-root /var/www/phoenix-updates \
  --ssl-cert /etc/letsencrypt/live/updates.example.com/fullchain.pem \
  --ssl-key /etc/letsencrypt/live/updates.example.com/privkey.pem
```

The installer defaults to HTTP port 80, HTTPS port 443, and Nginx site name
`phoenix-updates`. It grants `www-data` ownership of the web root when that
account exists. Review the generated site and `sudo nginx -T` after installation.

## Publish a signed release

Prepare a release package on a trusted workstation. The package must contain:

```text
update_publish/
├── manifest.json
├── manifest.sig
└── files/                 # payload paths referenced by the manifest
```

The signature must be generated over the exact bytes of `manifest.json`, using
the signing key that matches the public key embedded in the updater. Keep the
private key offline or in an approved secret store. Never upload it with the
release.

Upload the prepared package to the server's incoming directory. The example
uses `--delete` only against that dedicated incoming directory so stale files
from an earlier package do not leak into a later release. Review the dry-run
output before running the actual transfer:

```bash
rsync -avn --delete --rsync-path="sudo rsync" \
  ./update_publish/ phoenix@updates.example.com:/home/phoenix/update_publish/

rsync -av --delete --rsync-path="sudo rsync" \
  ./update_publish/ phoenix@updates.example.com:/home/phoenix/update_publish/
```

Then publish. The paths shown are the defaults, so the explicit options are
included to make the destination clear:

```bash
sudo /home/phoenix/phoenix_updates/publish_update_release.sh \
  --source /home/phoenix/update_publish \
  --web-root /var/www/phoenix-updates
```

The publisher reads the numeric `version` from `manifest.json` unless
`--version` is supplied; an override must match the manifest. For a normal
cumulative release, it copies the active web root into a staging directory,
overlays the incoming package, validates every manifest file path, size, and
SHA-256 value, then swaps the validated staging directory into place. Keep the
manifest's `files` list complete for the patch set that clients are expected to
have; unchanged payload files may remain in the active web root.

The publisher checks that `manifest.sig` exists, but it does not verify the
cryptographic signature itself. The client updater performs that verification.
Always publish the matching manifest and signature together.

### Empty release after distributing a full client

Use `--empty-release` only when a full client is being distributed separately
and the online patch set must be cleared. The incoming manifest must have an
empty `files` array, and the incoming `files/` directory must contain no patch
files:

```bash
sudo /home/phoenix/phoenix_updates/publish_update_release.sh \
  --source /home/phoenix/update_publish \
  --web-root /var/www/phoenix-updates \
  --empty-release
```

Unlike a cumulative publish, this replaces the active web root without
preserving old patch files. Players with older clients will need the full
client package; they cannot obtain removed patches from the updater.

### Optional database version synchronization

The login server's `ClientVersion.Version` should match the release manifest's
`version` when the game uses that version gate. The publisher can synchronize
it with `--set-db-version`, but it publishes the files **before** attempting
the database update; a database failure does not roll back a successful file
publish. Review and test database access before using this option.

Supply the database password through the environment rather than putting it in
the command line or this README:

```bash
read -r -s -p "Database password: " MYSQL_PWD
printf '\n'
export MYSQL_PWD
sudo --preserve-env=MYSQL_PWD \
  /home/phoenix/phoenix_updates/publish_update_release.sh \
  --set-db-version \
  --db-host 127.0.0.1 \
  --db-user elcastle \
  --db-name DARKEDEN
status=$?
unset MYSQL_PWD
test "$status" -eq 0
```

## Verify the endpoint

Run these checks after initial installation and every release:

```bash
sudo nginx -t
curl -fsSI https://updates.example.com/updates/manifest.json
curl -fsSI https://updates.example.com/updates/manifest.sig
curl -fsS https://updates.example.com/updates/manifest.json
```

For each `files/...` entry in the manifest, verify that the corresponding URL
returns the expected payload. A `404` usually means the manifest path, URL
prefix, Nginx alias, or published payload does not match.

## Current VPS check

On the VPS inspected for this documentation, Nginx is active, its configuration
passes `nginx -t`, `/updates/` is already aliased to
`/var/www/phoenix-updates/`, and a local request for `manifest.json` returns
HTTP 200. The current manifest has an empty `files` array and `manifest.sig` is
present; the endpoint is responding, but it does not currently advertise patch
files. No endpoint installation or release publish is performed by this README.
