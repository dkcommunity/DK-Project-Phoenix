//----------------------------------------------------------------------
// CDirectDraw.cpp
//----------------------------------------------------------------------

#include "CDirectDraw.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include "../crash_trace_tool.h"

// D3D9Backend tek aktif render yoludur. PHOENIX_RENDER_BACKEND_D3D9
// RenderTypes.h tarafinda guvenli default 1, CMake tarafinda global 1.
#include "D3D9Backend.h"

// dgVoodoo runtime config/include/link hattı render yolundan kaldirildi.

/*-----------------------------------------------------------------------------
  GLOBALS
-----------------------------------------------------------------------------*/
//----------------------------------------------------------------------
//
// init Static members
//
//----------------------------------------------------------------------
LPDIRECTDRAW7					CDirectDraw::m_pDD					= NULL;
LPDIRECTDRAWSURFACE7			CDirectDraw::m_pDDSPrimary			= NULL;
LPDIRECTDRAWSURFACE7			CDirectDraw::m_pDDSBack				= NULL;
DDSURFACEDESC2					CDirectDraw::m_ddsd;

LPDIRECTDRAWGAMMACONTROL	CDirectDraw::m_pDDGammaControl	= NULL;

HWND								CDirectDraw::m_hWnd					= NULL;
//bool							CDirectDraw::m_bActive				= false;

bool								CDirectDraw::m_bFullscreen			= true;
WORD								CDirectDraw::m_ScreenWidth			= 0;
WORD								CDirectDraw::m_ScreenHeight			= 0;
bool								CDirectDraw::m_b565					= true;
bool								CDirectDraw::m_b3D					= true;
bool								CDirectDraw::m_bMMX					= false;
bool								CDirectDraw::m_bGammaControl		= false;
DDGAMMARAMP							CDirectDraw::m_DDGammaRamp;
WORD								CDirectDraw::m_GammaStep = 100;
WORD								CDirectDraw::m_AddGammaStep[3] = {0, 0, 0};

RECT								CDirectDraw::m_rcWindow;
RECT								CDirectDraw::m_rcScreen;
RECT								CDirectDraw::m_rcViewport;

// Mask
WORD								CDirectDraw::s_wMASK_SHIFT[5];
DWORD								CDirectDraw::s_dwMASK_SHIFT[5];
QWORD								CDirectDraw::s_qwMASK_SHIFT[5];

WORD								CDirectDraw::s_wMASK_RGB[6];
DWORD								CDirectDraw::s_dwMASK_RGB[6];
QWORD								CDirectDraw::s_qwMASK_RGB[6];

QWORD								CDirectDraw::s_qwMASK_ALPHA0;
QWORD								CDirectDraw::s_qwMASK_ALPHA1;
DWORD								CDirectDraw::s_dwMASK_ALPHA0;
DWORD								CDirectDraw::s_dwMASK_ALPHA1;
WORD								CDirectDraw::s_wMASK_ALPHA0;
WORD								CDirectDraw::s_wMASK_ALPHA1;

BYTE								CDirectDraw::s_bSHIFT_R; 
BYTE								CDirectDraw::s_bSHIFT_G;
BYTE								CDirectDraw::s_bSHIFT_B;

// 1 Byte ShiftCount for 4444
BYTE								CDirectDraw::s_bSHIFT4_R; 
BYTE								CDirectDraw::s_bSHIFT4_G;
BYTE								CDirectDraw::s_bSHIFT4_B;

// 기본 ����
WORD								CDirectDraw::RED	= 0xF800;
WORD								CDirectDraw::GREEN	= 0x07D0;
WORD								CDirectDraw::BLUE	= 0x001F;
WORD								CDirectDraw::WHITE	= 0xFFFF;

bool								CDirectDraw::s_bUseIMEHandle = false;


//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------
CDirectDraw::CDirectDraw()
{
	// ~empty~
}

CDirectDraw::~CDirectDraw()
{
	//ReleaseAll(); // ��붗��� ��줘야 ����.
}

/*-----------------------------------------------------------------------------
- Init
- DirectDraw� 초기������.

  <���� ���>
  HWND hWnd									= window handle
  WORD wWidth, WORD wHeight			= surface size
  enum CDirectDraw::SCREENMODE mode = screen mode option
-----------------------------------------------------------------------------*/
// -----------------------------------------------------------------------------
// CDirectDraw::ConfigureDgVoodoo
// -----------------------------------------------------------------------------
// D3D9 native render backend'te dgVoodoo wrapper/config yolu yoktur.
// Fonksiyon eski public API uyumlulugu icin no-op kalir.
// -----------------------------------------------------------------------------
void CDirectDraw::ConfigureDgVoodoo()
{
	CT_Done("DIAG: ConfigureDgVoodoo no-op (D3D9 native backend).");
}

HRESULT CDirectDraw::QueryAvailableVideoMemory(DWORD dwCaps, DWORD* pTotal, DWORD* pFree)
{
	if (pTotal == NULL || pFree == NULL)
	{
		return E_INVALIDARG;
	}

#if PHOENIX_RENDER_BACKEND_D3D9
	(void)dwCaps;
	const DWORD fallbackTotal = 64 * 1024 * 1024;
	const DWORD fallbackFree  = 32 * 1024 * 1024;

	IDirect3DDevice9Ex* pDevice = phoenix::render::D3D9Backend::Instance().GetD3D9Device();
	if (pDevice == NULL)
	{
		*pTotal = fallbackTotal;
		*pFree  = fallbackFree;
		CT_Done("DIAG: QueryAvailableVideoMemory D3D9 device NULL; fallback %lu/%lu",
			*pFree, *pTotal);
		return DD_OK;
	}

	DWORD available = pDevice->GetAvailableTextureMem();
	if (available < 16 * 1024 * 1024)
	{
		available = fallbackFree;
	}

	*pFree  = available;
	*pTotal = (available > fallbackTotal) ? available : fallbackTotal;
	return DD_OK;
#else
	if (m_pDD == NULL)
	{
		return DDERR_NOTINITIALIZED;
	}

	DDSCAPS2 ddsCaps2;
	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));
	ddsCaps2.dwCaps = dwCaps;
	return m_pDD->GetAvailableVidMem(&ddsCaps2, pTotal, pFree);
#endif
}

HRESULT CDirectDraw::QueryDeviceCaps(DDCAPS* pDriverCaps)
{
	if (pDriverCaps == NULL)
	{
		return E_INVALIDARG;
	}

#if PHOENIX_RENDER_BACKEND_D3D9
	DWORD dwTotal = 0;
	DWORD dwFree  = 0;
	HRESULT hr = QueryAvailableVideoMemory(DDSCAPS_VIDEOMEMORY, &dwTotal, &dwFree);
	if (hr != DD_OK)
	{
		return hr;
	}

	ZeroMemory(pDriverCaps, sizeof(DDCAPS));
	pDriverCaps->dwSize = sizeof(DDCAPS);
	pDriverCaps->dwVidMemTotal = dwTotal;
	pDriverCaps->dwVidMemFree  = dwFree;
	pDriverCaps->dwCaps2 = DDCAPS2_NONLOCALVIDMEMCAPS;
	return DD_OK;
#else
	if (m_pDD == NULL)
	{
		return DDERR_NOTINITIALIZED;
	}
	return m_pDD->GetCaps(pDriverCaps, NULL);
#endif
}

HRESULT CDirectDraw::SetDisplayModeCompat(WORD wWidth, WORD wHeight, DWORD dwBPP)
{
#if PHOENIX_RENDER_BACKEND_D3D9
	(void)wWidth;
	(void)wHeight;
	(void)dwBPP;
	return DD_OK;
#else
	if (m_pDD == NULL)
	{
		return DDERR_NOTINITIALIZED;
	}
	return m_pDD->SetDisplayMode(wWidth, wHeight, dwBPP, 0, 0);
#endif
}

HRESULT CDirectDraw::RestoreDisplayModeCompat()
{
#if PHOENIX_RENDER_BACKEND_D3D9
	return DD_OK;
#else
	if (m_pDD == NULL)
	{
		return DDERR_NOTINITIALIZED;
	}
	return m_pDD->RestoreDisplayMode();
#endif
}

bool CDirectDraw::Init(HWND hWnd, WORD wWidth, WORD wHeight, enum CDirectDraw::SCREENMODE mode, bool b3D, bool bUseIMEWindow)
{
#if PHOENIX_RENDER_BACKEND_D3D9
	//------------------------------------------------------------------
	// Phase 4 — D3D9Backend init yolu.
	//
	// DDraw7 lifecycle (SetCooperativeLevel + SetDisplayMode + CreateSurface)
	// tamamen SKIP. Phoenix-static
	// state (m_pDD/m_pDDSPrimary/m_pDDSBack) NULL kalir; backend
	// IDirect3D9Ex/IDirect3DDevice9Ex + composition buffer'i kendi
	// yonetir. CDirectDrawSurface (Phase 5) D3D9Surface delegate'i
	// uzerinden Lock/Unlock kontratini surdurur.
	//
	// dgVoodoo runtime config D3D9 backend'inde anlamsizdir; wrapper yolu
	// build/runtime hattindan kaldirildi.
	//------------------------------------------------------------------
	CT_Done("DIAG: CDirectDraw::Init: PHOENIX_RENDER_BACKEND_D3D9=1 — D3D9Backend moduna geciliyor (DDraw7 lifecycle SKIP).");

	m_hWnd          = hWnd;
	m_b3D           = b3D;
	s_bUseIMEHandle = bUseIMEWindow;

	// Backend init — ScreenMode enum CDirectDraw::SCREENMODE'a paralel
	// (FULLSCREEN=0/Windowed=1).
	// 2026-08-31 DX11 P0: concrete D3D9Backend yerine factory dispatch —
	// render_backend.cfg "dx11" ise D3D11Backend, degilse D3D9Backend.
	phoenix::render::IRenderBackend& backend = *phoenix::render::GetRenderBackend();
	phoenix::render::ScreenMode bmode = (mode == FULLSCREEN)
		? phoenix::render::ScreenMode::Fullscreen
		: phoenix::render::ScreenMode::Windowed;

	if (!backend.Init(hWnd, wWidth, wHeight, bmode, b3D, bUseIMEWindow))
	{
		DirectDrawFailed("RenderBackend::Init FAILED");
		return false;
	}

	//------------------------------------------------------------------
	// DDraw7 compat objesi artik yaratilmiyor. Query/control ihtiyaci
	// QueryAvailableVideoMemory/QueryDeviceCaps/*DisplayModeCompat helper'lari
	// uzerinden D3D9-safe davranisa cevrildi.
	//------------------------------------------------------------------
	m_pDD = NULL;

	//------------------------------------------------------------------
	// Phoenix-static state senkronu — eski CDirectDraw contract'i ile ayni
	// kalir; downstream GetScreenWidth/Height/IsFullscreen/Is565/IsMMX
	// inline'lari bu degerlere baglidir.
	// CreateDeviceEx backbuffer boyutunu degistirdiyse backend'den oku.
	//------------------------------------------------------------------
	m_ScreenWidth  = backend.GetScreenWidth();
	m_ScreenHeight = backend.GetScreenHeight();
	if (m_ScreenWidth == 0)  m_ScreenWidth  = wWidth;
	if (m_ScreenHeight == 0) m_ScreenHeight = wHeight;
	m_bFullscreen  = (mode == FULLSCREEN);

	// MMX: modern x86 baseline'da daima true (CheckMMX da boyle dondurur).
	m_bMMX = true;

	// 16-bit composition buffer her zaman R5G6B5 (Phase 1 D3D9Backend pattern):
	// Check565() flag ON'da sabit TRUE doner (asagidaki refactor'a bak).
	m_b565 = true;

	// Gamma: D3D9'da fullscreen'de hardware ramp var (D3D9Backend::IsGammaSupported).
	// Windowed'da silent ignore — Phoenix UI hala "supports gamma" iddiasinda
	// olabilir, sorun yok cunku SetGammaRamp NULL-guard'lı.
	m_bGammaControl = backend.IsGammaSupported();

	//------------------------------------------------------------------
	// Pencere RECT'leri — legacy path GetWindowRect/
	// GetClientRect/ClientToScreen ile populated. D3D9'da composition
	// buffer fullscreen quad olarak back buffer'i kapliyor; yine de
	// CDirectDraw client'lari (OnMove vs.) bu RECT'leri okuyor, init
	// edelim.
	//------------------------------------------------------------------
	GetWindowRect(hWnd, &m_rcWindow);
	SetRect(&m_rcScreen,   0, 0, m_ScreenWidth, m_ScreenHeight);
	SetRect(&m_rcViewport, 0, 0, m_ScreenWidth, m_ScreenHeight);

	// Maskeleri R5G6B5 kalibrasyonu ile init.
	InitMask(m_b565);

	CT_Done("DIAG: CDirectDraw::Init: D3D9Backend hazir — %dx%d %s, 565=%d, gamma=%d.",
		wWidth, wHeight, m_bFullscreen ? "fullscreen" : "windowed",
		(int)m_b565, (int)m_bGammaControl);

	return true;
#else
	//------------------------------------------------------------------
	// Legacy DDraw7 + dgVoodoo path (default — Phoenix regression 0).
	//------------------------------------------------------------------
	m_hWnd	= hWnd;

	// Programmatically configure dgVoodoo if it's being used
	ConfigureDgVoodoo();

	// Log ddraw.dll path to verify wrapper loading
	HMODULE hDDraw = GetModuleHandle("ddraw.dll");
	if (hDDraw) {
		char szPath[MAX_PATH];
		GetModuleFileNameA(hDDraw, szPath, MAX_PATH);
		CT_Done("DIAG: ddraw.dll loaded from: %s", szPath);
	} else {
		CT_Done("DIAG: ddraw.dll NOT LOADED via GetModuleHandle!");
	}

	// DX Object Create
	if (DirectDrawCreateEx(NULL, (VOID**)&m_pDD, IID_IDirectDraw7, NULL) != DD_OK)
	{
		DirectDrawFailed("DirectDrawCreate Error!");
		return false;
	}

	// 3D Primary Surface?
	m_b3D = b3D;

	s_bUseIMEHandle = bUseIMEWindow;

	bool bOK;
	//-------------------------------------
	//
	// Full Screen Mode
	//
	//-------------------------------------
	if (mode == FULLSCREEN)
	{
		bOK = InitFullscreen(wWidth, wHeight);
	}

	//-------------------------------------
	//
	// Window Mode
	//
	//-------------------------------------
	else
	{
		bOK = InitWindowMode(wWidth, wHeight);
	}

	if (bOK)
	{
		//-------------------------------------
		// check for Support MMX?
		//-------------------------------------
		m_bMMX			= CheckMMX();

		//-------------------------------------
		// check for Support Gamma Control?
		//-------------------------------------
		m_bGammaControl = CheckGammaControl();

		//-------------------------------------
		// check for VideoCard 5:6:5  or 5:5:5?
		//-------------------------------------
		m_b565			= Check565();

		// 5:6:5  or 5:5:5�� 맞는 mask� setting����.
		InitMask(m_b565);
	}

	return bOK;
#endif // PHOENIX_RENDER_BACKEND_D3D9
}

//----------------------------------------------------------------------
// DirectDraw Initialization
//----------------------------------------------------------------------
bool CDirectDraw::InitFullscreen(WORD wWidth, WORD wHeight)
{
	DDSURFACEDESC2	ddsd;
	DDSCAPS2		ddscaps;	

	//------------------------------------------------------
	// Get exclusive mode
	//------------------------------------------------------
	if (m_pDD->SetCooperativeLevel(m_hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN) != DD_OK)
	{
		DirectDrawFailed("SetCooperativeLevel FAILED");
		return false;	
	}

	//------------------------------------------------------
	// Set the video mode to  ( dwWidth x dwHeight x bpp )
	// 2026-05-14: 16-bit -> 32-bit primary surface (DDrawCompat/dxwrapper
	// migration icin modern Windows DWM zorunlu). DDraw7 spec'i source 16-bit
	// sprite asset'leri 32-bit primary'ye blit ederken otomatik conversion
	// yapar (driver-level), colorkey value source format'ta korunur.
	// Sprite blit'lerinde manuel format manipulasyonu yoksa bu fix yeterli.
	//------------------------------------------------------
	if (m_pDD->SetDisplayMode(wWidth, wHeight, 32, 0, 0) != DD_OK)
	{
		DirectDrawFailed("SetDisplayMode FAILED");
		return false;
	}

	//------------------------------------------------------
	// Create the primary surface with 1 back buffer
	//------------------------------------------------------
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
						   DDSCAPS_FLIP | //DDSCAPS_VIDEOMEMORY |
						   DDSCAPS_COMPLEX;

	//------------------------------------------------------
	// D3D� ���������..
	//------------------------------------------------------
	if (m_b3D)
	{
		ddsd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE;
		ddsd.dwBackBufferCount = 2;
	}
	else
	{
	    ddsd.dwBackBufferCount = 1;
	}

	if (m_pDD->CreateSurface(&ddsd, &m_pDDSPrimary, NULL) != DD_OK )
	{
		DirectDrawFailed("CreateSurface FAILED");
		return false;
	}

	CT_Done("DIAG: Fullscreen Primary Surface Created. Caps: 0x%08X", ddsd.ddsCaps.dwCaps);

	//------------------------------------------------------
	// Get a pointer to the back buffer	
	//------------------------------------------------------
	ZeroMemory(&ddscaps, sizeof(ddscaps));
	ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
	if (m_pDDSPrimary->GetAttachedSurface(&ddscaps, &m_pDDSBack)!=DD_OK)
	{
		DirectDrawFailed("GetAttachedSurface FAILED");
		return false;	
	}

	if(s_bUseIMEHandle)
	{
		m_pDDSBack->AddRef();
		
		DDCAPS ddcaps;
		ZeroMemory(&ddcaps,sizeof(DDCAPS));
		m_pDD->GetCaps(&ddcaps,NULL);
		/*if( (ddcaps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED) == 0)
		{
			DirectDrawFailed("This display card can not render GDI.");
			return false;
		}*/
		//------------------------------------------------------
		// Create the window clipper
		//------------------------------------------------------
		LPDIRECTDRAWCLIPPER		pDDClipper;	// DirectDraw Clipper
		if ( m_pDD->CreateClipper(0, &pDDClipper, NULL) != DD_OK )
		{
			DirectDrawFailed("Clipper Create Error!");
			return false;
		}
		
		if ( pDDClipper->SetHWnd(0, m_hWnd) != DD_OK )
		{
			DirectDrawFailed("Clipper Setting Error! - Window -");
			return false;
		}
		
		if ( m_pDDSPrimary->SetClipper(pDDClipper) != DD_OK )
		{
			DirectDrawFailed("Clipper Setting Error! - Primary Surface -");
			return false;
		}
		
		pDDClipper->Release();
		pDDClipper = NULL;
	}	

	//------------------------------------------------------
	// member init..
	//------------------------------------------------------
	m_bFullscreen	= true;

	m_ScreenWidth			= wWidth;
	m_ScreenHeight			= wHeight;

   return true;
}

//----------------------------------------------------------------------
// WindowMode DirectDraw Initialization
//----------------------------------------------------------------------
bool CDirectDraw::InitWindowMode(WORD wWidth, WORD wHeight)
{
	// Get the dimensions of the viewport and screen bounds
	// Store the rectangle which contains the renderer
	//SetRect(&m_rcViewport, 0, 0, wWidth, wHeight );
	//memcpy(&m_rcScreen, &m_rcViewport, sizeof(m_rcScreen) );	

	// Save the window size/pos for switching modes
	GetWindowRect(m_hWnd, &m_rcWindow);

	DDSURFACEDESC2	ddsd;
	LPDIRECTDRAWCLIPPER		pDDClipper;	// DirectDraw Clipper

	if (m_pDD->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL) != DD_OK)
	{
		DirectDrawFailed("CooperativeLevel mode Error!");
		return false;
	}

	//------------------------------------------------------
	// Get the dimensions of the viewport and screen bounds
	//------------------------------------------------------
	// Force window to (0,0) to recover from off-screen spawn issues
	SetWindowPos(m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);

   	GetClientRect(m_hWnd, &m_rcViewport);
   	GetClientRect(m_hWnd, &m_rcScreen);
   	ClientToScreen(m_hWnd, (POINT*)&m_rcScreen.left);
   	ClientToScreen(m_hWnd, (POINT*)&m_rcScreen.right);

	CT_Done("DIAG: Windowed Mode Rects (FORCED SYNC) - Viewport: (%d,%d)-(%d,%d), Screen: (%d,%d)-(%d,%d)",
		m_rcViewport.left, m_rcViewport.top, m_rcViewport.right, m_rcViewport.bottom,
		m_rcScreen.left, m_rcScreen.top, m_rcScreen.right, m_rcScreen.bottom);


	//------------------------------------------------------
	// Create Primary Surface
	//------------------------------------------------------
	::ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize			= sizeof(ddsd);
	ddsd.dwFlags		= DDSD_CAPS;
	ddsd.ddsCaps.dwCaps	= DDSCAPS_PRIMARYSURFACE;	

	if (m_pDD->CreateSurface(&ddsd, &m_pDDSPrimary, NULL) != DD_OK )
	{
		DirectDrawFailed("Create PrimarySurface Error!");
		return false;
	}

	//------------------------------------------------------
	// Create Back Surface
	//------------------------------------------------------
   ::ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize			= sizeof(ddsd);
	ddsd.dwFlags		= DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps	= DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY;
	ddsd.dwWidth		= wWidth;
	ddsd.dwHeight		= wHeight;

	//------------------------------------------------------
	// D3D� ���������..
	//------------------------------------------------------
	if (m_b3D)
	{		
		ddsd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE;		
	}

	HRESULT hr1 = m_pDD->CreateSurface(&ddsd, &m_pDDSBack, NULL);
	if (hr1 != DD_OK)
	{
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		HRESULT hr2 = m_pDD->CreateSurface(&ddsd, &m_pDDSBack, NULL);
		if (hr2 != DD_OK)
		{
			// Try with System Memory
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
			HRESULT hr3 = m_pDD->CreateSurface(&ddsd, &m_pDDSBack, NULL);
			if (hr3 != DD_OK)
			{
				char debugMsg[256];
				wsprintfA(debugMsg, "Create BackSurface Error! Res: %dx%d HRESULT: 0x%08X", wWidth, wHeight, hr3);
				DirectDrawFailed(debugMsg);
				return false;
			}
			CT_Done("DIAG: Windowed Back Surface Created in SYSTEM MEMORY. Caps: 0x%08X", ddsd.ddsCaps.dwCaps);
		}
		else
		{
			CT_Done("DIAG: Windowed Back Surface Created (OFFSCREENPLAIN). Caps: 0x%08X", ddsd.ddsCaps.dwCaps);
		}
	}
	else
	{
		CT_Done("DIAG: Windowed Back Surface Created (DEFAULT). Caps: 0x%08X", ddsd.ddsCaps.dwCaps);
	}

	//------------------------------------------------------
	// Create the window clipper
	//------------------------------------------------------
	if ( m_pDD->CreateClipper(0, &pDDClipper, NULL) != DD_OK )
	{
		DirectDrawFailed("Clipper Create Error!");
		return false;
	}

	if ( pDDClipper->SetHWnd(0, m_hWnd) != DD_OK )
	{
		DirectDrawFailed("Clipper Setting Error! - Window -");
		return false;
	}

	if ( m_pDDSPrimary->SetClipper(pDDClipper) != DD_OK )
	{
		DirectDrawFailed("Clipper Setting Error! - Primary Surface -");
		return false;
	}

	pDDClipper->Release();
	pDDClipper = NULL;

	//------------------------------------------------------
	// data member init...
	//------------------------------------------------------
	m_bFullscreen	= false;

	m_ScreenWidth			= wWidth;
	m_ScreenHeight			= wHeight;

	return true;
}

//----------------------------------------------------------------------
// Release Primary & Back Surface
//----------------------------------------------------------------------
void CDirectDraw::ReleaseSurface()
{
    if (m_pDD != NULL)
    {
		m_pDD->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL);

		if (m_pDDSBack != NULL)
        {
            m_pDDSBack->Release();
            m_pDDSBack = NULL;
        }

        if (m_pDDSPrimary != NULL)
        {
            m_pDDSPrimary->Release();
            m_pDDSPrimary = NULL;
        }
    }
}

//----------------------------------------------------------------------
// Release All
//----------------------------------------------------------------------
void CDirectDraw::ReleaseAll()
{
#if PHOENIX_RENDER_BACKEND_D3D9
	//------------------------------------------------------------------
	// Phase 4 — D3D9 backend shutdown.
	//
	// D3D9Backend::Shutdown() composition buffer + texture + device +
	// IDirect3D9Ex'i sirayla release eder. m_pDDSPrimary/m_pDDSBack/m_pDD
	// D3D9 native modda NULL kalir.
	//------------------------------------------------------------------
	CT_Done("DIAG: CDirectDraw::ReleaseAll: render backend Shutdown cagiriliyor.");
	phoenix::render::GetRenderBackend()->Shutdown();
	if (m_pDD != NULL)
	{
		m_pDD->Release();
		m_pDD = NULL;
	}
#else
	if (m_pDDGammaControl!=NULL)
	{
		m_pDDGammaControl->SetGammaRamp(0, &m_DDGammaRamp);
		m_pDDGammaControl->Release();
		m_pDDGammaControl = NULL;
	}

	// Release Primary & Back Surface
	ReleaseSurface();

	if (m_pDD != NULL)
	{
		m_pDD->Release();
		m_pDD = NULL;
	}
#endif // PHOENIX_RENDER_BACKEND_D3D9
}

//----------------------------------------------------------------------
// 2026-09-02 O3 — canli cozunurluk: statik ekran state senkronu.
// PhoenixResolutionLive backend Resize'i basardiktan sonra cagirir;
// GetScreenWidth/Height inline'lari + rcScreen/rcViewport tuketicileri
// yeni boyutu gorur. Pencere RECT'i caller SetWindowPos sonrasi OnMove
// ile guncellenir.
//----------------------------------------------------------------------
void CDirectDraw::OnResolutionChanged(WORD wWidth, WORD wHeight)
{
	m_ScreenWidth  = wWidth;
	m_ScreenHeight = wHeight;
	SetRect(&m_rcScreen,   0, 0, wWidth, wHeight);
	SetRect(&m_rcViewport, 0, 0, wWidth, wHeight);
	if (m_hWnd != NULL)
		GetWindowRect(m_hWnd, &m_rcWindow);
	CT_Done("CDirectDraw::OnResolutionChanged %dx%d", (int)wWidth, (int)wHeight);
}

//----------------------------------------------------------------------
// Restore All Surfaces
//
// D3D9 native modda D3D9Backend::RestoreAll() cagrilir. Legacy DDraw7
// implementasyon kodu sadece PHOENIX_RENDER_BACKEND_D3D9=0 derlenirse
// kullanilir; normal Client build'inde aktif degildir.
//----------------------------------------------------------------------
bool CDirectDraw::RestoreAllSurfaces()
{
#if PHOENIX_RENDER_BACKEND_D3D9
	//------------------------------------------------------------------
	// Phase 4 — D3D9 backend restore yolu.
	//
	// D3D9Ex device'lar genelde D3DERR_DEVICELOST atmaz (Vista+ DXGI
	// model), ama Alt-Tab/RDP transitions'da D3DERR_DEVICEREMOVED gibi
	// durumlar olabilir. D3D9Backend::RestoreAll() bunu yakalar +
	// composition buffer + texture'i yeniden olusturur.
	//------------------------------------------------------------------
	CT_Done("DIAG: RestoreAllSurfaces (D3D9) called");
	bool bOK = phoenix::render::GetRenderBackend()->RestoreAll();
	if (bOK)
	{
		CT_Done("DIAG: RestoreAllSurfaces (D3D9) SUCCEEDED");
	}
	else
	{
		CT_Done("DIAG: RestoreAllSurfaces (D3D9) FAILED");
	}
	return bOK;
#else
	//
	// IDirectDraw7::RestoreAllSurfaces()
	// ` 모든 면을 Restore. offscreen까쏄  짊  .
	//
	if (m_pDD != NULL)
	{
		CT_Done("DIAG: RestoreAllSurfaces called");
		HRESULT hr = m_pDD->RestoreAllSurfaces();

		m_pDDSPrimary->Restore();
		m_pDDSBack->Restore();

		switch (hr)
		{
			case DD_OK :
				CT_Done("DIAG: RestoreAllSurfaces SUCCEEDED");
				return true;

			case DDERR_INVALIDOBJECT :
				CT_Done("DIAG: Restore All Surfaces Error! - InvalidObject");
				DirectDrawFailed("Restore All Surfaces Error! - InvalidObject");
				break;

			case DDERR_INVALIDPARAMS :
				CT_Done("DIAG: Restore All Surfaces Error! - InvalidParams");
				DirectDrawFailed("Restore All Surfaces Error! - InvalidParams");
				break;

			default:
				CT_Done("DIAG: Restore All Surfaces Error! - 0x%08X", hr);
				break;
		}
	}

	return false;
#endif // PHOENIX_RENDER_BACKEND_D3D9
}

//----------------------------------------------------------------------
// Flip Surface : Back --> Primary
//----------------------------------------------------------------------
void CDirectDraw::Flip()
{
#if PHOENIX_RENDER_BACKEND_D3D9
	//------------------------------------------------------------------
	// Phase 4 — D3D9 backend present yolu.
	//
	// D3D9Backend::Present() composition buffer'i (sysmem R5G6B5) ->
	// managed composition texture'a UpdateSurface ile yansitir, sonra
	// fullscreen quad ile back buffer'a cizip Present cagirir. Surface
	// lost durumunda backend kendi ic RestoreAll cagiriyor (Present
	// HRESULT D3DERR_DEVICELOST/DEVICEHUNG yakaliyor).
	//
	// DDraw7 yolundaki 5sn SetWindowPos(HWND_TOPMOST, SWP_NOACTIVATE)
	// exclusive D3D9Ex'te CheckDeviceState'i S_PRESENT_OCCLUDED +
	// IsWindowVisible=0 yapabiliyor (Vampire 4002 hang). D3D9 zaten
	// display'i sahiplenir; z-order dokunusunu burada yapmiyoruz.
	// Visibility log duruyor.
	//------------------------------------------------------------------
	if (!m_hWnd)
	{
		CT_Done("CDirectDraw::Flip (D3D9) SKIP - m_hWnd NULL (called before Init)");
		return;
	}

	static DWORD lastVisLog = 0;
	DWORD currentTime = timeGetTime();

	if (currentTime - lastVisLog > 5000)
	{
		BOOL bVisible = IsWindowVisible(m_hWnd);
		RECT rcW;
		GetWindowRect(m_hWnd, &rcW);
		CT_Done("DIAG: Visibility Audit (D3D9) - Visible:%d, Rect:(%d,%d)-(%d,%d)",
			bVisible, rcW.left, rcW.top, rcW.right, rcW.bottom);
		lastVisLog = currentTime;
	}

	phoenix::render::GetRenderBackend()->Present();

	static bool firstPresentLog = true;
	if (firstPresentLog)
	{
		CT_Done("DIAG: RenderBackend::Present FIRST CALL reached!");
		firstPresentLog = false;
	}
#else
	if (!m_pDDSPrimary)
	{
		CT_Done("CDirectDraw::Flip SKIP - m_pDDSPrimary NULL (called before Init)");
		return;
	}

	static DWORD lastRedInject = 0;
	static DWORD lastVisLog = 0;
	DWORD currentTime = timeGetTime();

	// Surface Lost Detection
	if (currentTime - lastVisLog > 5000) {
		BOOL bVisible = IsWindowVisible(m_hWnd);
		RECT rcW; GetWindowRect(m_hWnd, &rcW);
		HRESULT hrLPrimary = m_pDDSPrimary->IsLost();
		HRESULT hrLBack = m_pDDSBack->IsLost();

		CT_Done("DIAG: Visibility Audit - Visible:%d, Rect:(%d,%d)-(%d,%d), Lost(P/B):0x%08X/0x%08X, Ptrs(P/B)=%p/%p",
			bVisible, rcW.left, rcW.top, rcW.right, rcW.bottom, hrLPrimary, hrLBack, (void*)m_pDDSPrimary, (void*)m_pDDSBack);

		// Force Topmost again just in case
		SetWindowPos(m_hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		lastVisLog = currentTime;
	}

	HRESULT hRet;

	while (1)
	{
		//-------------------------------------------------------
		// Fullscreen Mode� ������� ���� 경우
		//-------------------------------------------------------
		if (m_bFullscreen)
		{
			if( s_bUseIMEHandle )
				hRet = m_pDDSPrimary->Blt( NULL, m_pDDSBack, NULL, DDBLT_WAIT, NULL );
			else
				hRet = m_pDDSPrimary->Flip(NULL, DDFLIP_WAIT);
		}

		//-------------------------------------------------------
		// Window Mode� ������� ���� 경우
		//-------------------------------------------------------
		else
		{
			hRet = m_pDDSPrimary->Blt(&m_rcScreen, m_pDDSBack, &m_rcViewport, DDBLT_WAIT, NULL);
		}

		if (hRet == DD_OK)
		{
			static bool firstFlipSuccessLog = true;
			if (firstFlipSuccessLog) {
				CT_Done("DIAG: Flip/Blt FIRST SUCCESS reached!");
				firstFlipSuccessLog = false;
			}
			break;
		}

		//-------------------------------------------------------
		// Lost �� Surface 복구
		//-------------------------------------------------------
		if (hRet == DDERR_SURFACELOST)
		{
			RestoreAllSurfaces();
		}

		if (hRet != DDERR_WASSTILLDRAWING)
			break;

	}
#endif // PHOENIX_RENDER_BACKEND_D3D9
}

//----------------------------------------------------------------------
// Flip to GDI Surface
//----------------------------------------------------------------------
void CDirectDraw::FlipToGDISurface()
{
#if PHOENIX_RENDER_BACKEND_D3D9
	//------------------------------------------------------------------
	// Phase 4 — D3D9'da GDI yuzeyine flip kavrami yok.
	//
	// DDraw7'de FlipToGDISurface fullscreen exclusive primary'i GDI ile
	// paylasilabilen surface'a getirir (MessageBox vs Win32 dialog'larin
	// gorunmesi icin). D3D9Ex windowed mode'da DWM zaten composition
	// yapiyor; fullscreen'de IDirect3DDevice9Ex::SetDialogBoxMode(TRUE)
	// gerekebilir ama Phoenix bunu aktif kullanmiyor. Backend wrapper
	// no-op + log.
	//------------------------------------------------------------------
	phoenix::render::GetRenderBackend()->FlipToGDISurface();
#else
	m_pDD->FlipToGDISurface();
#endif // PHOENIX_RENDER_BACKEND_D3D9
}

//----------------------------------------------------------------------
// Init Mask
//----------------------------------------------------------------------
// Lock�� ������ Draw� ����, ������ Mask볈����� 초기������.
//----------------------------------------------------------------------
void CDirectDraw::InitMask(bool b565)
{
 	//---------------------------------------------
	// 5:6:5
	//---------------------------------------------
	if (b565)
	{	
		//---------------------------------------------
		// Shift Mask
		//---------------------------------------------
		s_qwMASK_SHIFT[0]	= 0xFFFFFFFFFFFFFFFF;
		s_dwMASK_SHIFT[0]	= 0xFFFFFFFF;
		s_wMASK_SHIFT[0]	= 0xFFFF;

		// 01111 011111 01111
		// 0111 1011 1110 1111
		//    7    B    E    F
		s_qwMASK_SHIFT[1]	= 0x7BEF7BEF7BEF7BEF;
		s_dwMASK_SHIFT[1]	= 0x7BEF7BEF;
		s_wMASK_SHIFT[1]	= 0x7BEF;
		
		// 00111 001111 00111
		// 0011 1001 1110 0111
		//    3    9    E    7
		s_qwMASK_SHIFT[2]	= 0x39E739E739E739E7;
		s_dwMASK_SHIFT[2]	= 0x39E739E7;
		s_wMASK_SHIFT[2]	= 0x39E7;
		
		// 00011 000111 00011
		// 0001 1000 1110 0011
		//    1    8    E    3
		s_qwMASK_SHIFT[3]	= 0x18E318E318E318E3;
		s_dwMASK_SHIFT[3]	= 0x18E318E3;
		s_wMASK_SHIFT[3]	= 0x18E3;
		
		// 00001 000011 00001
		// 0000 1000 0110 0001
		//    0    8    6    1
		s_qwMASK_SHIFT[4]	= 0x0861086108610861;
		s_dwMASK_SHIFT[4]	= 0x08610861;
		s_wMASK_SHIFT[4]	= 0x0861;

		//---------------------------------------------
		// RGB Mask
		//---------------------------------------------
		s_qwMASK_RGB[0]	= 0xF800F800F800F800;	// R
		s_dwMASK_RGB[0]	= 0xF800F800;
		s_wMASK_RGB[0]	= 0xF800;

		s_qwMASK_RGB[1]	= 0x07E007E007E007E0;	// G
		s_dwMASK_RGB[1]	= 0x07E007E0;
		s_wMASK_RGB[1]	= 0x07E0;

		s_qwMASK_RGB[2]	= 0x001F001F001F001F;	// B
		s_dwMASK_RGB[2]	= 0x001F001F;
		s_wMASK_RGB[2]	= 0x001F;

		s_qwMASK_RGB[3]	= 0xFFE0FFE0FFE0FFE0;	// RG
		s_dwMASK_RGB[3]	= 0xFFE0FFE0;
		s_wMASK_RGB[3]	= 0xFFE0;

		s_qwMASK_RGB[4]	= 0xF81FF81FF81FF81F;	// RB
		s_dwMASK_RGB[4]	= 0xF81FF81F;
		s_wMASK_RGB[4]	= 0xF81F;

		s_qwMASK_RGB[5]	= 0x07FF07FF07FF07FF;	// GB
		s_dwMASK_RGB[5]	= 0x07FF07FF;
		s_wMASK_RGB[5]	= 0x07FF;

		

		//---------------------------------------------
		// ALPHA Mask
		//---------------------------------------------
		// 1111 1000 0001 1111 
		// 0000 0111 1110 0000
		s_qwMASK_ALPHA0 = 0xF81F07E0F81F07E0;
		s_qwMASK_ALPHA1 = 0x07E0F81F07E0F81F;
		s_dwMASK_ALPHA0 = 0xF81F07E0;
		s_dwMASK_ALPHA1 = 0x07E0F81F;
		s_wMASK_ALPHA0	= 0xF81F;
		s_wMASK_ALPHA1	= 0x07E0;

		//---------------------------------------------
		// 1 Byte RGB ShiftCount & Mask
		//---------------------------------------------
		s_bSHIFT_R		= 11; 		
		s_bSHIFT_G		= 6; 		
		s_bSHIFT_B		= 0; 		

		//---------------------------------------------
		// 1 Byte RGB ShiftCount & Mask for 4444
		//---------------------------------------------
		s_bSHIFT4_R		= 12; 		
		s_bSHIFT4_G		= 7; 		
		s_bSHIFT4_B		= 1; 		

		//---------------------------------------------
		// 기본 ����
		//---------------------------------------------
		RED				= 0xF800;
		GREEN			= 0x07E0;
		BLUE			= 0x001F;
		WHITE			= 0xFFFF;
	}		
		
	//---------------------------------------------
	// 5:5:5
	//---------------------------------------------
	else
	{	
		//---------------------------------------------
		// Shift Mask
		//---------------------------------------------
		s_qwMASK_SHIFT[0]	= 0x7FFF7FFF7FFF7FFF;
		s_dwMASK_SHIFT[0]	= 0x7FFF7FFF;
		s_wMASK_SHIFT[0]	= 0x7FFF;

		// 0 01111 01111 01111
		// 0011 1101 1110 1111
		//    3    D    E    F
		s_qwMASK_SHIFT[1]	= 0x3DEF3DEF3DEF3DEF;
		s_dwMASK_SHIFT[1]	= 0x3DEF3DEF;
		s_wMASK_SHIFT[1]	= 0x3DEF;
		
		// 0 00111 00111 00111
		// 0001 1100 1110 0111
		//    1    C    E    7
		s_qwMASK_SHIFT[2]	= 0x1CE71CE71CE71CE7;
		s_dwMASK_SHIFT[2]	= 0x1CE71CE7;
		s_wMASK_SHIFT[2]	= 0x1CE7;
		
		// 0 00011 00011 00011
		// 0000 1100 0110 0011
		//    0    C    6    3
		s_qwMASK_SHIFT[3]	= 0x0C630C630C630C63;
		s_dwMASK_SHIFT[3]	= 0x0C630C63;
		s_wMASK_SHIFT[3]	= 0x0C63;
		
		// 0 00001 00001 00001
		// 0000 0100 0010 0001
		//    0    4    2    1
		s_qwMASK_SHIFT[4]	= 0x0421042104210421;
		s_dwMASK_SHIFT[4]	= 0x04210421;
		s_wMASK_SHIFT[4]	= 0x0421;	

		//---------------------------------------------
		// RGB Mask
		//---------------------------------------------
		s_qwMASK_RGB[0]	= 0x7C007C007C007C00;	// R
		s_dwMASK_RGB[0]	= 0x7C007C00;
		s_wMASK_RGB[0]	= 0x7C00;

		s_qwMASK_RGB[1]	= 0x03E003E003E003E0;	// G
		s_dwMASK_RGB[1]	= 0x03E003E0;
		s_wMASK_RGB[1]	= 0x03E0;

		s_qwMASK_RGB[2]	= 0x001F001F001F001F;	// B
		s_dwMASK_RGB[2]	= 0x001F001F;
		s_wMASK_RGB[2]	= 0x001F;

		s_qwMASK_RGB[3]	= 0x7FE07FE07FE07FE0;	// RG
		s_dwMASK_RGB[3]	= 0x7FE07FE0;
		s_wMASK_RGB[3]	= 0x7FE0;

		s_qwMASK_RGB[4]	= 0x7C1F7C1F7C1F7C1F;	// RB
		s_dwMASK_RGB[4]	= 0x7C1F7C1F;
		s_wMASK_RGB[4]	= 0x7C1F;

		s_qwMASK_RGB[5]	= 0x03FF03FF03FF03FF;	// GB
		s_dwMASK_RGB[5]	= 0x03FF03FF;
		s_wMASK_RGB[5]	= 0x03FF;

		//---------------------------------------------
		// ALPHA Mask
		//---------------------------------------------
		s_qwMASK_ALPHA0 = 0x7C1F03E07C1F03E0;
		s_qwMASK_ALPHA1 = 0x03E07C1F03E07C1F;
		s_dwMASK_ALPHA0 = 0x7C1F03E0;
		s_dwMASK_ALPHA1 = 0x03E07C1F;
		s_wMASK_ALPHA0	= 0x7C1F;
		s_wMASK_ALPHA1	= 0x03E0;

		//---------------------------------------------
		// 1 Byte RGB ShiftCount & Mask
		//---------------------------------------------
		s_bSHIFT_R		= 10; 		
		s_bSHIFT_G		= 5; 		
		s_bSHIFT_B		= 0; 

		//---------------------------------------------
		// 1 Byte RGB ShiftCount & Mask for 4444
		//---------------------------------------------
		s_bSHIFT4_R		= 11; 		
		s_bSHIFT4_G		= 6;		
		s_bSHIFT4_B		= 1;	
		
		//---------------------------------------------
		// 기본 ����
		//---------------------------------------------
		RED				= 0x7C00;
		GREEN			= 0x03E0;
		BLUE			= 0x001F;
		WHITE			= 0x7FFF;
	} 
}

//----------------------------------------------------------------------
// Gamma Control�� 짛�����짗� �� check
//
// `FullMode�� ��못된 ��면크� 짠����� ������ ����처리� ����.
//----------------------------------------------------------------------
bool CDirectDraw::CheckGammaControl()
{
#if PHOENIX_RENDER_BACKEND_D3D9
	//------------------------------------------------------------------
	// Phase 4 — D3D9 backend uzerinden sorgu.
	//
	// D3D9Backend::IsGammaSupported() fullscreen + valid device varsa
	// TRUE doner (windowed'da silent ignore + FALSE). Phoenix UI bu
	// degeri okuyup gamma slider'i toggle ediyor.
	//------------------------------------------------------------------
	return phoenix::render::GetRenderBackend()->IsGammaSupported();
#else
	if (m_pDDGammaControl!=NULL)
	{
		return true;
	}

	DDCAPS	driver;

	::ZeroMemory(&driver, sizeof(driver));
	driver.dwSize = sizeof(driver);

	// DirectDraw�� Cap..�� ��������.
	if ( m_pDD->GetCaps(&driver, NULL) != DD_OK )
	{
		DirectDrawFailed("GetCaps() Error");
	}

	// Hardware Support?
	if (driver.dwCaps2 & DDCAPS2_PRIMARYGAMMA)
	{
		// DX Object Interface Query
		if (FAILED( m_pDDSPrimary->QueryInterface(IID_IDirectDrawGammaControl, (LPVOID *)&m_pDDGammaControl)) )
		{
			DirectDrawFailed("Gamma Control QueryInterface FAILED!");
			return false;
		}

		// ������ GammaRamp� ������.
		m_pDDGammaControl->GetGammaRamp(0, &m_DDGammaRamp);

		return true;
	}

	return false;
#endif // PHOENIX_RENDER_BACKEND_D3D9
}

//----------------------------------------------------------------------
// GammaRamp 값을 ������ 것으� 바꾼��.
//----------------------------------------------------------------------
// test�� ����
//----------------------------------------------------------------------
void CDirectDraw::RestoreGammaRamp()
{
#if PHOENIX_RENDER_BACKEND_D3D9
	//------------------------------------------------------------------
	// Phase 4 — D3D9 backend baseline gamma'yi (Init aninda yakalanan)
	// geri yukler. CDirectDraw::m_DDGammaRamp burada anlamsiz (DDraw7
	// gamma struct'i), backend kendi state'ini saklar.
	//------------------------------------------------------------------
	phoenix::render::GetRenderBackend()->RestoreGammaRamp();
#else
	if (m_pDDGammaControl!=NULL)
	{
		m_pDDGammaControl->SetGammaRamp(0, &m_DDGammaRamp);
	}
#endif // PHOENIX_RENDER_BACKEND_D3D9
}

//----------------------------------------------------------------------
// GammaRamp 값을 바꾼��.
//----------------------------------------------------------------------
// 0(����) ~ 100(����) ~ 200(밝음)
// ������ ��� 결과...
// 50 ~ 150 ���� ������ 값만 ����� 좋다� ��각된��
//----------------------------------------------------------------------
void CDirectDraw::SetGammaRamp(WORD step)
{
#if PHOENIX_RENDER_BACKEND_D3D9
	//------------------------------------------------------------------
	// Phase 4 — D3D9 backend SetGammaRamp.
	//
	// D3D9Backend kendi step'i (0..200, 100=neutral) + m_AddGammaStep
	// state'ini sakliyor; ramp hesaplamasi orjinal CDirectDraw
	// mantigi ile birebir. step==0xffff "mevcut step'i koru" sentineli.
	//
	// Phoenix-static m_GammaStep'i de senkron tutalim — saved-state
	// kodu (settings yazici) bu degeri okuyor olabilir.
	//------------------------------------------------------------------
	if (step != 0xffff)
	{
		m_GammaStep = step;
	}
	phoenix::render::GetRenderBackend()->SetGammaRamp(m_GammaStep);
#else
	if(step == 0xffff)
		step = m_GammaStep;
	else
		m_GammaStep = step;

	if (m_pDDGammaControl!=NULL)
	{
		DDGAMMARAMP	ddgr;

		ZeroMemory(&ddgr, sizeof(ddgr));

		int maxValue[3];
		int stepValue[3];

		for(int i = 0; i < 3; i++)
		{
			WORD addGammaStep = max(-100, min(100, m_AddGammaStep[i]));
			if(m_AddGammaStep[i] > 0)
				step = m_GammaStep + (200 - m_GammaStep)*addGammaStep/100;
			else
				step = m_GammaStep + m_GammaStep*addGammaStep/100;

			if(step<100)
			{
				maxValue[i] = 0;
				stepValue[i] = 100 - step;
			}
			else
			{
				maxValue[i] = 0xFFFF;
				stepValue[i] = step - 100;
			}
		}

		WORD r, g, b;

		for (int i =0; i<256; i++)
		{
			r = m_DDGammaRamp.red[i];
			g = m_DDGammaRamp.green[i];
			b = m_DDGammaRamp.blue[i];

			ddgr.red[i] = (WORD)(r + (maxValue[0]-r)*stepValue[0]/100);
			ddgr.green[i] = (WORD)(g + (maxValue[1]-g)*stepValue[1]/100);
			ddgr.blue[i] = (WORD)(b + (maxValue[2]-b)*stepValue[2]/100);
		}

		m_pDDGammaControl->SetGammaRamp(0, &ddgr);
	}
#endif // PHOENIX_RENDER_BACKEND_D3D9
}

void	CDirectDraw::SetAddGammaRamp(WORD rStep, WORD gStep, WORD bStep)
{
#if PHOENIX_RENDER_BACKEND_D3D9
	//------------------------------------------------------------------
	// Phase 4 — D3D9 backend SetAddGammaRamp.
	//
	// Backend kendi m_AddGammaStep state'ini saklayip ramp'i SetGammaRamp
	// uzerinden uyguluyor. Phoenix-static m_AddGammaStep'i de senkron
	// tutalim (mevcut DDraw7 yolu da ayni semantik).
	//------------------------------------------------------------------
	m_AddGammaStep[0] = rStep;
	m_AddGammaStep[1] = gStep;
	m_AddGammaStep[2] = bStep;
	phoenix::render::GetRenderBackend()->SetAddGammaRamp(rStep, gStep, bStep);
#else
	m_AddGammaStep[0] = rStep;
	m_AddGammaStep[1] = gStep;
	m_AddGammaStep[2] = bStep;

	SetGammaRamp();
#endif // PHOENIX_RENDER_BACKEND_D3D9
}

//----------------------------------------------------------------------
// Check 5:5:5 or 5:6:5?
//----------------------------------------------------------------------
// ������ Surface� ���� 만들���� 
// �� ���� 찍어보고 5:5:5��� 5:6:5��� ��������.
//----------------------------------------------------------------------
bool CDirectDraw::Check565()
{
#if PHOENIX_RENDER_BACKEND_D3D9
	//------------------------------------------------------------------
	// Phase 4 — D3D9 backend composition buffer her zaman R5G6B5.
	//
	// Phase 1 D3D9Backend::CreateCompositionResources() composition
	// buffer'i (sprite codec hedef) DAIMA D3DFMT_R5G6B5 olusturuyor;
	// arka taraf back buffer X8R8G8B8 ama Phoenix sprite kodu sadece
	// composition buffer'i goruyor. Dolayisiyla 5:6:5 yolu (s_*MASK_*
	// kalibrasyonu) garanti.
	//------------------------------------------------------------------
	CT_Done("DIAG: Check565 (D3D9) -> TRUE (composition buffer R5G6B5)");
	return true;
#else
	/*
	LPDIRECTDRAWSURFACE7	pDDSurface = NULL;
	DDSURFACEDESC2			ddsd;

	// Surface�� �� ��� ����
	// m_pSurface� OffScreen��� ��������.
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof( ddsd );
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;// | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth  = 1;
	ddsd.dwHeight = 1;

	if ( m_pDD->CreateSurface( &ddsd, &pDDSurface, NULL )  !=  DD_OK )
	{
		DirectDrawFailed("DD-Create OffSurface for Check5:6:5 Error!");
		return FALSE;
	}

	// check for 16bit 5:6:5 or 5:5:5
	HDC hDC;	

	// (0,0)��  �� ���� 출력����.
	pDDSurface->GetDC(&hDC);
	SetPixel(hDC,0,0,RGB(255,255,255));
	pDDSurface->ReleaseDC(hDC);

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

	// Lock�� 걸고 출력�� ���� ��������.
	pDDSurface->Lock(NULL, &ddsd, 0, NULL);
	*/
	/*
	HRESULT hr;
	while (1)
	{
		hr = pDDSurface->Lock(NULL, &ddsd, 0, NULL);
		if( hr == DD_OK )
		{
			break;
		}
		if( hr == DDERR_SURFACELOST )
		{
			HRESULT ret = pDDSurface->Restore();
			
			if (ret == false)
				break;
			else
				hr = DD_OK;
		}
		if( hr != DDERR_WASSTILLDRAWING )
		{
			break;
		}
	}

	int a;
	switch (hr)
	{
		case DD_OK : a = 0; break;
		case DDERR_INVALIDOBJECT   : a = 0; break;
		case DDERR_INVALIDPARAMS   : a = 0; break;
		case DDERR_OUTOFMEMORY   : a = 0; break;
		case DDERR_SURFACEBUSY   : a = 0; break;
		case DDERR_SURFACELOST   : a = 0; break;
		case DDERR_WASSTILLDRAWING   : a = 0; break;
	}
	*/

	//unsigned short* lpSurface = (WORD*)ddsd.lpSurface;
	//pDDSurface->Unlock(NULL);

	// release �� 줋�.
	//pDDSurface->Release();

	// (0,0)�� ����� ����
	//return  *lpSurface == 0xFFFF;

	
	DDSURFACEDESC2 ddsd;

	m_pDDSBack->GetSurfaceDesc(&ddsd);
	ddsd.dwSize = sizeof(ddsd);  

	m_pDDSBack->Lock(NULL,&ddsd,DDLOCK_WAIT,NULL);
	m_pDDSBack->Unlock(NULL);

	CT_Done("DIAG: Pixel Format - RMask: 0x%08X, GMask: 0x%08X, BMask: 0x%08X, RGBBitCount: %d", 
		ddsd.ddpfPixelFormat.dwRBitMask, ddsd.ddpfPixelFormat.dwGBitMask, ddsd.ddpfPixelFormat.dwBBitMask, ddsd.ddpfPixelFormat.dwRGBBitCount);

	if( (ddsd.ddpfPixelFormat.dwRBitMask == 0xf800)
			&& (ddsd.ddpfPixelFormat.dwGBitMask==0x07e0)
			&& (ddsd.ddpfPixelFormat.dwBBitMask == 0x001f) )
	{
		CT_Done("DIAG: Pixel Format Mode: 565");
		return TRUE;			// 565
	}

	CT_Done("DIAG: Pixel Format Mode: 555 (Fallback)");
	return FALSE;			// 555
#endif // PHOENIX_RENDER_BACKEND_D3D9
}

//----------------------------------------------------------------------
// MMX� 짛����� system���?
//----------------------------------------------------------------------
bool CDirectDraw::CheckMMX()
{
    // Modern baseline (DX9/Win7+) assumes MMX/SSE is always available.
    return true;
}

void CDirectDraw::DirectDrawFailed(const char * str) 
{
	// DirectDraw� ������.
	ReleaseAll();

	MessageBox(m_hWnd, str, NULL, MB_OK);
	PostMessage(m_hWnd, WM_CLOSE, 0, 0);
}

/*-----------------------------------------------------------------------------
- OnMove
- Window mode�� 경우 Window move�� ��(WM_MOVE) ��������.
-----------------------------------------------------------------------------*/
void CDirectDraw::OnMove()
{
	if (!m_hWnd)
	{
		CT_Done("CDirectDraw::OnMove SKIP - m_hWnd NULL (called before Init)");
		return;
	}

	// Retrieve the window position after a move
	if (!m_bFullscreen)
	{
		GetWindowRect(m_hWnd, &m_rcWindow);
		GetClientRect(m_hWnd, &m_rcViewport);
		GetClientRect(m_hWnd, &m_rcScreen);
		ClientToScreen(m_hWnd, (POINT*)&m_rcScreen.left);
		ClientToScreen(m_hWnd, (POINT*)&m_rcScreen.right);

		CT_Done("DIAG: OnMove Updated Rects - Screen: (%d,%d)-(%d,%d)", 
			m_rcScreen.left, m_rcScreen.top, m_rcScreen.right, m_rcScreen.bottom);
	}
}

/*-----------------------------------------------------------------------------
- Get_Count_Rbit
- red�� bit mask count� 구한��.
-----------------------------------------------------------------------------*/
int CDirectDraw::Get_Count_Rbit()
{
	DWORD i, count_r, t_r;

	count_r = 0;
	t_r = Get_R_Bitmask();
	for (int i = 0; i < Get_BPP(); i++)
	{
		if (t_r & (DWORD)(1<<i)) count_r++;
	}

	return count_r;
}

/*-----------------------------------------------------------------------------
- Get_Count_Gbit
- green�� bit mask count� 구한��.
-----------------------------------------------------------------------------*/
int CDirectDraw::Get_Count_Gbit()
{
	DWORD i, count_g, t_g;

	count_g = 0;
	t_g = Get_G_Bitmask();
	for (int i = 0; i < Get_BPP(); i++)
	{
		if (t_g & (DWORD)(1<<i)) count_g++;
	}

	return count_g;
}

/*-----------------------------------------------------------------------------
- Get_Count_Bbit
- blut�� bit mask count� 구한��.
-----------------------------------------------------------------------------*/
int CDirectDraw::Get_Count_Bbit()
{
	DWORD i, count_b, t_b;

	count_b = 0;
	t_b = Get_B_Bitmask();
	for (int i = 0; i < Get_BPP(); i++)
	{
		if (t_b & (DWORD)(1<<i)) count_b++;
	}

	return count_b;
}

/*-----------------------------------------------------------------------------
- Get_R_Bitmask
- red�� bit mask� 구한��.
-----------------------------------------------------------------------------*/
DWORD CDirectDraw::Get_R_Bitmask()
{
#if PHOENIX_RENDER_BACKEND_D3D9
	//------------------------------------------------------------------
	// Phase 4 — D3D9 composition buffer R5G6B5 sabit. Phoenix sprite
	// codec sadece composition buffer ile etkilesir (back buffer
	// X8R8G8B8 olsa da Get_*_Bitmask cagrilari composition kalibrasyonu
	// icin). DDraw7 yolundaki m_pDDSPrimary->GetPixelFormat == 0xF800.
	//------------------------------------------------------------------
	return 0xF800;
#else
	DDPIXELFORMAT ddpf;

	ddpf.dwSize = sizeof(ddpf);
	m_pDDSPrimary->GetPixelFormat(&ddpf);
	return ddpf.dwRBitMask;
#endif // PHOENIX_RENDER_BACKEND_D3D9
}

/*-----------------------------------------------------------------------------
- Get_G_Bitmask
- green�� bit mask� 구한��.
-----------------------------------------------------------------------------*/
DWORD CDirectDraw::Get_G_Bitmask()
{
#if PHOENIX_RENDER_BACKEND_D3D9
	// R5G6B5: green 6-bit, 0x07E0 (Phase 1 D3D9Backend composition buffer).
	return 0x07E0;
#else
	DDPIXELFORMAT ddpf;

	ddpf.dwSize = sizeof(ddpf);
	m_pDDSPrimary->GetPixelFormat(&ddpf);
	return ddpf.dwGBitMask;
#endif // PHOENIX_RENDER_BACKEND_D3D9
}

/*-----------------------------------------------------------------------------
- Get_B_Bitmask
- blue�� bit mask� 구한��.
-----------------------------------------------------------------------------*/
DWORD CDirectDraw::Get_B_Bitmask()
{
#if PHOENIX_RENDER_BACKEND_D3D9
	// R5G6B5: blue 5-bit, 0x001F.
	return 0x001F;
#else
	DDPIXELFORMAT ddpf;

	ddpf.dwSize = sizeof(ddpf);
	m_pDDSPrimary->GetPixelFormat(&ddpf);
	return ddpf.dwBBitMask;
#endif // PHOENIX_RENDER_BACKEND_D3D9
}

/*-----------------------------------------------------------------------------
- Get_BPP
- Get bit per pixel
-----------------------------------------------------------------------------*/
DWORD CDirectDraw::Get_BPP()
{
#if PHOENIX_RENDER_BACKEND_D3D9
	//------------------------------------------------------------------
	// Phase 4 — composition buffer 16 bit (R5G6B5). Sprite codec'in
	// referans aldigi BPP bu; back buffer 32 bit ama Phoenix kodu
	// composition path'inde calistigindan 16 dondurmek dogru.
	//------------------------------------------------------------------
	return 16;
#else
	HRESULT        hr;
	DDPIXELFORMAT  ddpf;

	ddpf.dwSize = sizeof(ddpf);
	hr = m_pDDSPrimary->GetPixelFormat(&ddpf);
	if (hr != DD_OK) DirectDrawFailed("Get_BPP() Error!");

	return ddpf.dwRGBBitCount;
#endif // PHOENIX_RENDER_BACKEND_D3D9
}
