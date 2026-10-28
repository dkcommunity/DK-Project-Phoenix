# Project Phoenix Update Endpoint

Bu klasor, yeni `Updater.exe` icin server tarafinda gereken statik update endpoint'ini kurmak ve release publish etmek icin kullanilir.

## Mimari

- Client `Updater.exe` su URL'leri okur:
  - `https://<host>/updates/manifest.json`
  - `https://<host>/updates/manifest.sig`
  - veya client `data/info/updateclient.inf` icindeki `ManifestUrl` / `PhoenixManifestUrl`.
- Web server sadece statik dosya sunar.
- Oyun `LoginServer` hala DB `ClientVersion.Version` ile client surumunu karsilastirir.
- Release publish edildikten sonra `ClientVersion.Version` manifest `version` degeriyle ayni olmalidir.
- `manifest.sig`, `manifest.json` ham byte iceriginin ECDSA P-256 / SHA-256 imzasidir.

## Kurulum

Sunucuda:

```bash
cd /home/phoenix/phoenix/ServerSRC/tools/phoenix_updates
chmod +x install_update_endpoint.sh publish_update_release.sh
sudo ./install_update_endpoint.sh --server-name 89.252.164.244 --web-root /var/www/phoenix-updates
```

HTTPS sertifikasi hazirsa:

```bash
sudo ./install_update_endpoint.sh \
  --server-name patch.project-phoenix.example \
  --web-root /var/www/phoenix-updates \
  --ssl-cert /etc/letsencrypt/live/patch.project-phoenix.example/fullchain.pem \
  --ssl-key /etc/letsencrypt/live/patch.project-phoenix.example/privkey.pem
```

Bu islem:

- `/var/www/phoenix-updates` olusturur.
- Nginx icin `/updates/` location config'i yazar.
- Cert/key verilirse ayni endpoint'i HTTPS uzerinden de sunar.
- `GET` ve `HEAD` disindaki methodlari reddeder.
- Dotfile erisimini kapatir.
- Nginx config test edip reload eder.

## Publish

Windows tarafinda manifest uret:

```powershell
python "F:\Project Phoenix\tools\phoenix_updater\generate_manifest.py" `
  --root "F:\Project Phoenix\GameData" `
  --out "F:\Project Phoenix\update_publish" `
  --base-url "https://89.252.164.244/updates" `
  --version 370 `
  --private-key "F:\Project Phoenix\tools\phoenix_updater\phoenix_manifest_private_key.pem"
```

Private key lokal kalmalidir. Public key `Updater.exe` icine gomuludur.

`update_publish` klasorunu sunucuya gonder:

```bash
rsync -avz "F:/Project Phoenix/update_publish/" root@89.252.164.244:/home/phoenix/update_publish/
```

Sunucuda publish et:

```bash
cd /home/phoenix/phoenix/ServerSRC/tools/phoenix_updates
sudo ./publish_update_release.sh
```

Varsayilanlar:

- Source: `/home/phoenix/update_publish`
- Web root: `/var/www/phoenix-updates`
- Version: `/home/phoenix/update_publish/manifest.json` icindeki `version`

Publish cumulative calisir:

- `/var/www/phoenix-updates` aktif patch havuzudur.
- Eski patch dosyalari silinmez ve `.previous` klasorune tasinmaz.
- Script once aktif web root'u staging klasorune kopyalar.
- Sonra `/home/phoenix/update_publish` icindeki yeni release dosyalarini bunun ustune yazar.
- `manifest.json` icindeki her `files/...` kaydi staging icinde mevcut mu, size/hash dogru mu diye kontrol edilir.
- Kontrol basariliysa staging aktif `/var/www/phoenix-updates` olur.

Bu yuzden Windows tarafinda uretilen `manifest.json`, sadece bugun secilen dosyalari degil updater ile yonetilen tum guncel patch dosyalarini icermelidir. Patch Manifest GUI bunu mevcut signed live manifest'i okuyup yeni secilen dosyalari onun ustune merge ederek yapar. Output paketindeki `files/` klasoru sadece yeni secilen/degisken dosyalari tasir; eski dosyalar VPS'teki aktif patch havuzundan korunur.

## Bos release / full client zip dagitimi

Bazen patch yayinlanmayacak, bunun yerine oyunculara manuel full client zip verilecek. Bu durumda eski cumulative patch havuzu risklidir: oyuncu yeni full client'i `Updater.exe` uzerinden acinca eski manifestte kalan patch dosyalarini tekrar indirip yeni dosyalarin ustune yazabilir.

Bu durumda imzali bos manifest uret:

```powershell
python "F:\Project Phoenix\tools\phoenix_updater\generate_manifest.py" `
  --empty `
  --out "F:\Project Phoenix\update_publish" `
  --base-url "https://89.252.164.244/updates" `
  --version 371 `
  --private-key "F:\Project Phoenix\tools\phoenix_updater\phoenix_manifest_private_key.pem"
```

`update_publish` klasorunu sunucuya gonder ve sunucuda bos release moduyla publish et:

```bash
cd /home/phoenix/phoenix/ServerSRC/tools/phoenix_updates
sudo ./publish_update_release.sh --empty-release
```

`--empty-release` modu cumulative calismaz:

- Aktif `/var/www/phoenix-updates` patch havuzunu eski dosyalari korumadan degistirir.
- `manifest.json` icindeki `files` listesi bos olmak zorundadir.
- Source icinde `files/` altinda patch dosyasi varsa publish fail olur.
- Updater manifesti dogrular, indirilecek dosya bulmaz ve client'i baslatir.

Bu mod kullanildiginda uzun sure oyuna girmeyen oyuncular patch zinciriyle guncellenmez; onlara yeni full client zip verilmesi gerekir.

Farkli klasor veya version override gerekiyorsa:

```bash
sudo ./publish_update_release.sh \
  --source /home/phoenix/update_publish \
  --web-root /var/www/phoenix-updates \
  --version 370
```

DB version degerini ayni islemde guncellemek icin:

```bash
export MYSQL_PWD='DB_PASSWORD'
sudo -E ./publish_update_release.sh \
  --set-db-version \
  --db-name DARKEDEN \
  --db-user elcastle \
  --db-host 127.0.0.1
unset MYSQL_PWD
```

## Kontrol

```bash
curl -I http://127.0.0.1/updates/manifest.json
curl -I http://127.0.0.1/updates/manifest.sig
curl http://127.0.0.1/updates/manifest.json | head
mysql -u elcastle -p DARKEDEN -e "SELECT Version FROM ClientVersion;"
```

## Guvenlik Notlari

- Bu endpoint upload kabul etmez; sadece statik dosya sunar.
- `manifest.json`, `manifest.sig` ve dosyalar publish oncesi local olarak uretilir.
- Production icin domain + HTTPS kullanilmasi onerilir.
- Updater default olarak manifest imzasini zorunlu tutar. Gecici test icin `--optional-signature` veya `RequireManifestSignature : 0` kullanilabilir, production icin kullanilmaz.
