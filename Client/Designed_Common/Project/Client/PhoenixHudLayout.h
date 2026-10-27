/*-----------------------------------------------------------------------------

	PhoenixHudLayout.h

	2026-09-03 plan F3 — per-resolution HUD konum defteri.

	Kullanici istegi: HUD panelleri (simpleInformation, minimap+skill grubu)
	her cozunurlukte AYRI konum hatirlamali; cozunurluk degisiminde o
	cozunurluk icin kayitli konuma gitmeli, kayit yoksa ctor default
	formulune re-anchor edilmeli.

	Kalicilik: GameData\UserSet\PhoenixHudLayout.set (text, satir formati
	"WxH.windowId=x,y"). UserOption.set binary formatina BILINCLI olarak
	dokunulmaz — legacy slotlar (C_VS_UI_WINDOW_MANAGER) aynen calismaya
	devam eder, bu defter cozunurluk-bazli katman olarak ustune biner.

	windowId = C_VS_UI_WINDOW_MANAGER::WINDOW_ID enum degeri.

-----------------------------------------------------------------------------*/

#ifndef __PHOENIX_HUD_LAYOUT_H__
#define __PHOENIX_HUD_LAYOUT_H__

// Defterde resW x resH + windowId icin kayit varsa outX/outY doldurur, true
// doner. Kayit yoksa false (caller default formulune duser).
bool Phoenix_HudLayout_Get(int resW, int resH, int windowId, int* outX, int* outY);

// Kaydi gunceller ve dosyaya hemen yazar (kucuk dosya, atomik davranis
// onemli degil; her cagri tam rewrite).
void Phoenix_HudLayout_Set(int resW, int resH, int windowId, int posX, int posY);

#endif // __PHOENIX_HUD_LAYOUT_H__
