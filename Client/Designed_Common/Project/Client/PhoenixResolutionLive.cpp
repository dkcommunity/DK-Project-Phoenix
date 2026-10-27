/*-----------------------------------------------------------------------------

	PhoenixResolutionLive.cpp

	2026-09-02 O3 — canli cozunurluk degisimi. Detay: PhoenixResolutionLive.h

-----------------------------------------------------------------------------*/

#include "Client_PCH.h"
#include "PhoenixResolutionLive.h"
#include "crash_trace_tool.h"

#include "Client.h"				// g_hWnd, g_bFullScreen
#include "UserInformation.h"	// g_pUserInformation->iResolution_x/y
#include "Packet/Properties.h"	// g_pFileDef (resolution.inf yolu)
#include "DXLib.h"				// CDirectDraw + IRenderBackend zinciri
#include "DXLib/IRenderBackend.h"
#include "MTopView.h"			// g_pTopView->InitChanges() (adim 3.6)

#include <fstream>

// Client.cpp'de tanimli (dosya-lokal prototip orada; burada extern kullaniyoruz)
void SaveResolutionConfig();

// GameMain.cpp globalleri — ekran boyutuna bagli "Yol 1" sprite wrapper'lari
// (composition singleton'ina bagli). Cozunurluk degisiminde dims/clip'leri
// yeni boyutla tazelenmeli.
extern CSpriteSurface* g_pBack;
extern CSpriteSurface* g_pLast;

namespace
{
	// Client.cpp kPhoenixResolutionModes ile ayni tutulmali (whitelist tek
	// kaynak orada; anonim namespace icinde oldugu icin burada kopya).
	struct ResMode { int x, y; };
	const ResMode kModes[] = {
		{ 1280,  960 },
		{ 1440,  900 },
		{ 1920, 1080 },
		{ 1280, 1024 },
	};

	// Pencereyi calisma alanina ortala + client'i tam cozunurluge kilitle.
	void ResizeAndCenterWindow(HWND hWnd, int w, int h, bool bFullscreen)
	{
		if (hWnd == NULL)
			return;

		if (bFullscreen)
		{
			// Exclusive FS: backend Resize display mode'u degistirdi; pencere
			// yalnizca yeni boyuta cekilir.
			SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, w, h,
			             SWP_SHOWWINDOW | SWP_NOCOPYBITS);
			return;
		}

		const DWORD style   = (DWORD)GetWindowLongPtr(hWnd, GWL_STYLE);
		const DWORD exStyle = (DWORD)GetWindowLongPtr(hWnd, GWL_EXSTYLE);

		RECT rc = { 0, 0, w, h };
		AdjustWindowRectEx(&rc, style, FALSE, exStyle);
		const int winW = rc.right - rc.left;
		const int winH = rc.bottom - rc.top;

		RECT rcWork = { 0, 0, w, h };
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);
		const int x = rcWork.left + ((rcWork.right - rcWork.left) - winW) / 2;
		const int y = rcWork.top  + ((rcWork.bottom - rcWork.top) - winH) / 2;

		SetWindowPos(hWnd, NULL, x, y, winW, winH,
		             SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	}
}

bool Phoenix_ResolutionLive_IsSupported(int w, int h)
{
	for (int i = 0; i < (int)(sizeof(kModes) / sizeof(kModes[0])); ++i)
	{
		if (kModes[i].x == w && kModes[i].y == h)
			return true;
	}
	return false;
}

bool Phoenix_ResolutionLive_Apply(int w, int h)
{
	if (g_pUserInformation == NULL)
		return false;
	if (!Phoenix_ResolutionLive_IsSupported(w, h))
	{
		CT_Done("ResolutionLive: %dx%d whitelist disi — reddedildi", w, h);
		return false;
	}

	const int oldW = g_pUserInformation->iResolution_x;
	const int oldH = g_pUserInformation->iResolution_y;
	if (oldW == w && oldH == h)
		return true;

	CT_Done("ResolutionLive: %dx%d -> %dx%d APPLY", oldW, oldH, w, h);

	// 1. Backend resize — en riskli adim once (basarisizsa hicbir sey degismez)
	phoenix::render::IRenderBackend* pBackend = phoenix::render::GetRenderBackend();
	if (pBackend == NULL || !pBackend->Resize((WORD)w, (WORD)h))
	{
		CT_Done("ResolutionLive: backend Resize FAILED — rollback (state degismedi)");
		// Backend kendi icinde eski boyuta toparlanmayi dener; globals
		// henuz degismedigi icin ek rollback gerekmez.
		return false;
	}

	// 2. Globals — WM_GETMINMAXINFO ve tum UI tuketicileri yeni degeri gorur
	g_pUserInformation->iResolution_x = w;
	g_pUserInformation->iResolution_y = h;
	g_pUserInformation->IsResolution1024 = true;

	// 3. CDirectDraw statik ekran state (GetScreenWidth/Height + rect'ler)
	CDirectDraw::OnResolutionChanged((WORD)w, (WORD)h);

	// 3.5 Ekran boyutuna bagli sprite wrapper'lari (2026-09-02 crash fix
	// tamamlayicisi): backend Resize composition'i YERINDE yeniden kurdugu
	// icin delegate pointer'lari gecerli, ama wrapper'larin kendi
	// m_Width/m_Height/clip degerleri eski cozunurlukte kalir. Yeniden
	// baglama dims/clip'i tazeler (InitBacksurface/InitOffsurface Yol 1
	// binding'i owned=false oldugu icin singleton'i silmez).
	if (g_pBack != NULL)
		g_pBack->InitBacksurface();
	if (g_pLast != NULL)
	{
		g_pLast->InitOffsurface(w, h, DDSCAPS_SYSTEMMEMORY);
		g_pLast->SetTransparency(0);				// GameInit paritesi
		g_pLast->FillSurface(CDirectDraw::Color(0, 0, 0));
	}

	// 3.6 MTopView cozunurluge bagli kaynaklari (2026-09-02 crash fix #2):
	// MTopView::Init startup yuklemesinde ESKI cozunurlukle calisir —
	// m_LightBufferFilter SCREENLIGHT_WIDTH/HEIGHT (SURFACE_* tureviyle)
	// boyutlanir. Canli degisim sonrasi oyuna girilirse
	// ClearLightBufferFilter3D'nin memset'i yeni (buyuk) SCREENLIGHT_WIDTH
	// ile eski (kucuk) buffer satirlarina yazar → heap overflow AV
	// (dump v2.0-20260902-234112, memset ← ClearLightBufferFilter3D).
	// Cozum legacy'nin kendi mekanizmasi: MODE_CHANGE_OPTION'in display
	// degisikliginde cagirdigi InitChanges() — InitSurfaces/InitColors/
	// InitSprites/InitFilters/InitFonts'u YENI SURFACE_* ile yeniden kurar
	// (InitFilters re-entrant: eski buffer/texture'lari delete eder).
	// m_bInit degilse InitChanges kendi icinde no-op doner.
	if (g_pTopView != NULL)
	{
		if (!g_pTopView->InitChanges())
			CT_Done("ResolutionLive: MTopView InitChanges atlandi/basarisiz (m_bInit=0 olabilir)");
	}

	// 3.7 Tam dunya redraw (2026-09-03 plan F2a — "tile'lar kayboluyor" fix):
	// Apply zinciri surface'lari yeniden kuruyor ama zone tile/objelerin
	// yeniden cizimini ZORLAMIYORDU — ekran alt-tab'a (WM_ACTIVATEAPP
	// regain, GameMain.cpp CHKACT REGAIN) kadar bos/eksik kaliyordu cunku
	// o path RestoreSurface + SetFirstDraw cagiriyor. Ayni cifti burada da
	// cagir: SetFirstDraw m_bFirstTileDraw'i set eder → bir sonraki frame
	// DrawTileSurface/DetermineImageObject full redraw yapar.
	if (g_pTopView != NULL && g_pTopView->IsInit())
	{
		g_pTopView->RestoreSurface();
		g_pTopView->SetFirstDraw();
		CT_Done("ResolutionLive: TopView RestoreSurface+SetFirstDraw OK");
	}

	// 4. HWND
	ResizeAndCenterWindow(g_hWnd, w, h, g_bFullScreen);
	CDirectDraw::OnMove();	// m_rcWindow senkronu

	// 5. Kalicilik
	SaveResolutionConfig();

	CT_Done("ResolutionLive: %dx%d APPLY OK", w, h);
	return true;
}

bool Phoenix_ResolutionLive_SavePending(int w, int h, int fullscreen)
{
	if (g_pFileDef == NULL || !Phoenix_ResolutionLive_IsSupported(w, h))
		return false;

	const bool bFS = (fullscreen < 0) ? g_bFullScreen : (fullscreen != 0);

	// SaveResolutionConfig globals'tan yazar; oyun icinde globals'a
	// dokunmadan dosyaya dogrudan yaziyoruz (ayni format).
	std::string str = g_pFileDef->getProperty("FILE_INFO_RESOLUTION");
	std::ofstream file(str.c_str());
	if (!file.is_open())
	{
		CT_Done("ResolutionLive: SavePending '%s' ACILAMADI", str.c_str());
		return false;
	}
	file << "ResolutionX: " << w << std::endl;
	file << "ResolutionY: " << h << std::endl;
	file << "FullScreen: " << (bFS ? "1" : "0") << '\n';
	CT_Done("ResolutionLive: SavePending %dx%d fs=%d (restart'ta gecerli)", w, h, bFS ? 1 : 0);
	return true;
}

bool Phoenix_WindowMode_Apply(bool bFullscreen)
{
	phoenix::render::IRenderBackend* pBackend = phoenix::render::GetRenderBackend();
	if (pBackend == NULL || !pBackend->SetFullscreenMode(bFullscreen))
	{
		CT_Done("WindowMode: canli %s desteklenmedi — restart-pending",
		        bFullscreen ? "fullscreen" : "windowed");
		return false;
	}

	g_bFullScreen = bFullscreen;
	CDirectDraw::OnMove();	// m_rcWindow yeni pencere konumuyla senkron
	CT_Done("WindowMode: canli %s OK", bFullscreen ? "fullscreen" : "windowed");
	return true;
}
