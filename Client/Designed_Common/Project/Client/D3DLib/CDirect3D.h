//----------------------------------------------------------------------
// CDirect3D.h
//----------------------------------------------------------------------
//
// Texture로 사용할 Surface에 추가되면 좋은(!?) flag
//
//    ddsd.ddsCaps.dwCaps2 |= DDSCAPS2_TEXTUREMANAGE;
//
// 그러나, Hardware가속이 되는 상태여야만 한다.
//----------------------------------------------------------------------
#ifndef	__CDIRECT3D_H__
#define	__CDIRECT3D_H__

#ifndef	D3D_OVERLOADS
#define	D3D_OVERLOADS
#endif

#include <D3D.h>


#include <ddraw.h>

//#include "d3dtextr.h"
//#include "d3dutil.h"

#include "CDirectDraw.h"

//-----------------------------------------------------------------------------
// Phase 4 (D3D9 native port FAZ 3) — D3D9 device member.
//
// PHOENIX_RENDER_BACKEND_D3D9=1 aktivasyonu sonrasi CDirect3D class'i hem
// legacy D3D7 device'i hem de D3D9 device'i tutar. Flag set degilse (default)
// D3D9 member'i tum kullanim noktalarinda NULL kalir, eski yol degismez.
//
// RenderTypes.h flag tanimi icerir.
//-----------------------------------------------------------------------------
#include "RenderTypes.h"

// !! D3D9 native port — d3d9.h **DAHIL EDILMEZ** burada.
//
// Onceden Phase 4 skeleton bu header'a `#include <d3d9.h>` koymustu, ama:
//   1. CDirect3D.h zaten DX7 D3D.h dahil ediyor (yukarida).
//   2. DX7 D3D.h + Windows SDK d3d9.h ayni TU'da coexist edemez —
//      `_D3DVECTOR`, `_D3DLIGHTTYPE`, `_D3DSHADEMODE` vs. tekrar tanim (C2011).
//   3. d3d9.h **sadece DXLib/D3D9Backend.h** tarafindan dahil edilir
//      (orada D3D.h yok, ddraw.h kullaniliyor, conflict yok).
//
// CDirect3D class'inin D3D9 device referansina ihtiyaci olan kodlar (CheckHAL,
// Init, MTopView Clear) **D3D9Facade** uzerinden dolayli cagri yapar. Facade
// header (`DXLib/D3D9Facade.h`) d3d9.h dahil etmez; sadece namespace fonksiyon
// imzalari. Implementasyonu D3D9Facade.cpp DXLib derleme biriminde, orada
// d3d9.h gorulur.
//
// Yan etki: CDirect3D class'tan `GetDevice9()` ve `m_pD3D9D` member'lari
// kaldirildi (yukaridaki conflict sebebiyle bu class'in d3d9 vtable'a erismesi
// imkansizdi). D3D9 device sahibi D3D9Backend::Instance() (DXLib singleton).




class CDirect3D : public CDirectDraw {
	public :
		CDirect3D() {}
		~CDirect3D() {}

		//--------------------------------------------------
		// 초기화
		//--------------------------------------------------
		static bool				Init();
		static void				Release();

		//--------------------------------------------------
		// Restore
		//--------------------------------------------------
		static BOOL				IsLost();
		static void				Restore();

		//--------------------------------------------------
		// Is Support HAL?
		//--------------------------------------------------
		static bool				CheckHAL();

		//--------------------------------------------------
		// Get
		//--------------------------------------------------
		static const LPDIRECT3DDEVICE7	GetDevice() { return m_pD3DD; }
		// Phase 4.5: D3D9 modunda da dinamik m_bHAL doner. CDirect3D::Init() D3D9
		// device aldiginda m_bHAL=true atar (CDirect3D.cpp). MTopView D3DCLEAR_ZBUFFER
		// + diger if(IsHAL()) bloklari D3D9 modunda aktif olur.
		static bool						IsHAL()		{ return m_bHAL; }

		// D3D9 device accessor: CDirect3D'den kaldirildi (mimari conflict, bkz.
		// header basi yorumu). D3D9 device sahibi: `phoenix::render::D3D9Backend
		// ::Instance().GetD3D9Device()` (DXLib). DX7-free TU'larda dogrudan
		// kullanilabilir. DX7 D3D.h dahil eden dosyalar (CDirect3D.cpp,
		// MTopView.cpp) `DXLib/D3D9Facade.h` cagirir (d3d9.h gormez).

		//--------------------------------------------------
		// Texture 생성 관련 정보
		//--------------------------------------------------
		static void		GetTextureSize(int& width, int& height);
		static DWORD	GetTextureWidthMax()	{ return m_TextureWidthMax; }
		static DWORD	GetTextureHeightMax()	{ return m_TextureHeightMax; }

		static bool		IsTexturePow2()			{ return m_bTexturePow2; }
		static bool		IsTextureSquareOnly()	{ return m_bTextureSquareOnly; }

		static const LPDDPIXELFORMAT	GetPixelFormat4444() { return &m_PixelFormat4444; }
		static const LPDDPIXELFORMAT	GetPixelFormat1555() { return &m_PixelFormat1555; }

		//--------------------------------------------------
		// PixelFormat
		//--------------------------------------------------
		static bool				FindBestPixelFormat(int bitsA, int bitsR, int bitsG, int bitsB, 
													LPDDPIXELFORMAT pddpfBestMatch);

		// Device : Texture
		//static void				SetTexture(DWORD dwStage, const char* filename);

	public :
		// 3D Rendering Mode
		/*
		enum CD3DMODE 
		{
			MODE_NULL = 0,
			MODE_HARDWARE,
			MODE_MMX,
			MODE_SOFTWARE
		};
		*/		

	protected :
		static bool				Init(GUID* pDriverGUID, const GUID* pDeviceGUID);
		static bool				CheckDeviceForTexture();

	private :
		static WORD				GetNumberOfBits( DWORD dwMask );
		static HRESULT WINAPI	EnumZBufferCallback(DDPIXELFORMAT* DDP_Format ,VOID* DDP_Desired);
		static HRESULT WINAPI	EnumTextureCallback( DDPIXELFORMAT* pddpf, VOID* );


	protected :
		//CD3DMODE			m_Mode;		// 3D Rendering Mode

		static bool						m_bHAL;

		static LPDIRECT3D7				m_pD3D;
		static LPDIRECT3DDEVICE7		m_pD3DD;
		static LPDIRECTDRAWSURFACE7		m_pDDZBuffer;

		// D3D9 static device member'lari kaldirildi (CDirect3D class artik d3d9
		// tipleri tutmaz — bkz. header basi yorumu). D3D9 ownership: D3D9Backend
		// singleton (DXLib). CDirect3D D3D9-aware kodu D3D9Facade ile dolayli.

		// PixelFormat
	    static DDPIXELFORMAT			m_PixelFormats[100]; // Stores pixel formats
	    static DWORD					m_dwNumPixelFormats;

		//------------------------------------------------------------------
		// Texture 생성 관련 정보
		//------------------------------------------------------------------
		// 하드웨어가 지원하는 Texture의 최대 크기
		static int						m_TextureWidthMax;
		static int						m_TextureHeightMax;

		static bool						m_bTexturePow2;		// 2^n만 되는가?		
		static bool						m_bTextureSquareOnly;	// 정사각형만 되는가?

	public :
		// PixelFormat
		static DDPIXELFORMAT			m_PixelFormat4444;
		static DDPIXELFORMAT			m_PixelFormat1555;
};

#endif
