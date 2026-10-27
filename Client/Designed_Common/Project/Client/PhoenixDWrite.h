//-----------------------------------------------------------------------------
// PhoenixDWrite.h
//
// FULL DX11 gecis Faz T1 (2026-09-01) — DirectWrite text motoru.
//
// FL2'nin 3 GDI boğazının (TextOut / DrawText / GetTextExtentPoint32) DWrite
// muadili. Cagiran taraf HDC state'ini (secili HFONT, text color, bk mode,
// bk color, text align) aynen kurar; bu fonksiyonlar state'i HDC'den okur —
// PrintInfo akisi ve FL2 API imzalari DEGISMEZ.
//
// Mekanizma (Reborn D2D HwndRenderTarget'tan farkli — composition sirasi
// korunmali, dunya ici isimler UI altinda kalmali):
//   1. String → IDWriteFactory::CreateGdiCompatibleTextLayout (GDI metrik
//      paritesi) → IDWriteBitmapRenderTarget'a grayscale-AA rasterize
//   2. Coverage (alpha mask, 8bpp) string+font anahtarli cache'e alinir —
//      ayni string farkli renklerle tekrar cizilirken (outline=5x, shadow=2x)
//      rasterize TEKRARLANMAZ
//   3. Cizim: mask × renk premultiplied scratch DIB'e doldurulur,
//      GdiAlphaBlend ile hedef HDC'ye bindirilir (DIB memory DC / D3D9
//      GetDC — her iki backend'de ayni yol; DC clip region'a uyar)
//
// Secim: text_engine.cfg (Client.exe yaninda, ilk satir "dwrite") veya
// PHOENIX_TEXT_ENGINE env. Varsayilan GDI (sifir risk). DWrite init/rasterize
// herhangi bir adimda basarisiz olursa fonksiyonlar FALSE doner — caller
// ayni cagri icinde GDI yoluna duser (gorsel kayip yok).
//-----------------------------------------------------------------------------

#ifndef __PHOENIX_DWRITE_H__
#define __PHOENIX_DWRITE_H__

#include <windows.h>

// DWrite text motoru secili ve init edilebilir durumda mi?
// Ilk cagrida config okunur + lazy init denenir; sonuc cache'lenir.
bool PhoenixDWrite_IsEnabled();

// TextOut muadili. HDC'den font/renk/align/bkmode okur, (x,y)'ye cizer.
// FALSE = motor kapali veya rasterize fail — caller GDI TextOut'a dusmeli.
BOOL PhoenixDWrite_TextOut(HDC hdc, int x, int y, LPCSTR str, int len);

// DrawText muadili (FL2 g_DrawText kullanim profili: '\n' ile onceden
// sarilmis metin, format & 3 yatay hizalama, rect'e clip).
// -1 = motor kapali/fail — caller GDI DrawText'e dusmeli. >=0 = cizilen yukseklik.
int PhoenixDWrite_DrawText(HDC hdc, LPCSTR str, int len, LPRECT rect, UINT format);

// GetTextExtentPoint32 muadili (GDI-compatible layout metrikleri).
// FALSE = motor kapali/fail — caller GDI olcumune dusmeli.
BOOL PhoenixDWrite_GetTextExtent(HDC hdc, LPCSTR str, int len, LPSIZE size);

// Cache + COM kaynaklarini birak (process cikisinda opsiyonel).
void PhoenixDWrite_Shutdown();

// Faz T3 pattern'i — tek satirlik kullanim: DWrite dene, olmazsa GDI TextOut.
// FL2 disindaki direkt TextOut call-site'lari (GDI_Text, whisper, U_edit,
// leaderboard) icin. HDC state'i caller kurmus olmali (mevcut davranis).
inline BOOL Phoenix_TextOutDW(HDC hdc, int x, int y, LPCSTR str, int len)
{
    if (PhoenixDWrite_IsEnabled() && PhoenixDWrite_TextOut(hdc, x, y, str, len))
        return TRUE;
    return TextOut(hdc, x, y, str, len);
}

#endif // __PHOENIX_DWRITE_H__
