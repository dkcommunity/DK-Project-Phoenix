/*-----------------------------------------------------------------------------

	PhoenixResolutionLive.h

	2026-09-02 O3 — Reborn Option portu: canli cozunurluk degisimi.

	Zincir (Apply):
	  1. Whitelist dogrulama (Client.cpp kPhoenixResolutionModes paritesi)
	  2. iResolution_x/y guncelle
	  3. Render backend Resize (swapchain + composition + dx11gpu katmani)
	     — basarisizsa globals + backend eski boyuta geri alinir (rollback)
	  4. CDirectDraw statik ekran state senkronu
	  5. HWND yeniden boyutlandir + ekrana ortala
	  6. resolution.inf kaydet

	UI tarafi (Base offscreen re-init + StartTitle + Option menusu yeniden
	acma) UIMessageManager::Execute_UI_APPLY_RESOLUTION icinde yapilir —
	pencere iterasyonu disinda, kuyruklu UI mesajiyla guvenli.

	Oyun icinde canli uygulanmaz: SavePending resolution.inf'e yazar,
	degisiklik yeniden baslatmada gecerli olur (in-game UI agaci
	construction-time koordinat sakladigi icin canli relayout guvensiz).

-----------------------------------------------------------------------------*/

#pragma once

// Desteklenen cozunurluk mu (whitelist)?
bool Phoenix_ResolutionLive_IsSupported(int w, int h);

// Canli uygulama zinciri. Basarida true; basarisizlikta rollback yapip false.
bool Phoenix_ResolutionLive_Apply(int w, int h);

// Oyun ici yol: yalniz resolution.inf'e yazar (globals'a dokunmaz),
// yeniden baslatmada InitResolutionConfig okur.
// fullscreen: -1 = mevcut g_bFullScreen korunur, 0/1 = acikca yaz.
bool Phoenix_ResolutionLive_SavePending(int w, int h, int fullscreen = -1);

// 2026-09-02 fullscreen fix — canli pencere modu (Reborn ApplyWindowMode
// paritesi). Backend destekliyorsa (D3D11: borderless CDS_FULLSCREEN)
// aninda uygular + g_bFullScreen gunceller; desteklemiyorsa false doner,
// caller restart-pending yoluna duser. Cozunurluk DEGISMEZ — UI relayout
// gerekmez, oyun icinde de guvenlidir.
bool Phoenix_WindowMode_Apply(bool bFullscreen);
