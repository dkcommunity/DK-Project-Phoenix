/*-----------------------------------------------------------------------------

	CDirectDrawSurface.h

	DirectX class library component.

	1999.12.28. sigi, KJTINC

-----------------------------------------------------------------------------*/

//----------------------------------------------------------------------
// CDirectDrawSurface.h
//----------------------------------------------------------------------
//
//
// 5:6:5 와  5:5:5의 차이는 
// Bit Mask에 의해 처리되는 부분에서 발생하는데
// CDirectDraw class의 InitMask()에서 Video Card에 맞는
// 적절한 Mask를 생성하므로 그냥~ CDirectDraw의 Mask를 사용하면 된다.
//
//
//
//----------------------------------------------------------------------
/*

  < DDSurface가 생성되는 MEMORY >

	- DDSCAPS_SYSTEMMEMORY는 Lock을 사용할 때 빠르다.
	- DDSCAPS_VIDEOMEMORY는 Blt를 사용할 때 빠르다.
	  단, VIDEOMEMORY가 부족하면 Surface를 생성하지 않으므로 error!
	- 지정을 안 해 주면 VIDEOMEMORY가 사용가능하면 사용하고 
   	  아니면 SYSTEMMEMORY를 사용한다.
	
  < Alpha Blending >
    - 사용하려면 DDSCAPS_SYSTEMMEMORY를 사용하는게 좋다.
	- Surface --> Surface로의 Alpha Blending보다
	  Memory(SpriteFile) --> Surface로의 Alpha Blending이 빠르다.

*/
//----------------------------------------------------------------------

#ifndef	__CDIRECTDRAWSURFACE_H__
#define	__CDIRECTDRAWSURFACE_H__


#include "CDirectDraw.h"

//----------------------------------------------------------------------
// 2026-05-16 R2: POOL_DEFAULT static-pool sentinel-bit.
//   InitTextureSurfaceStatic, InitTextureSurface'a delegate ederken bu
//   bayragi dwCaps'in en yuksek bit'inde set eder. InitTextureSurface body
//   bayragi gorunce D3D9 path icin D3DPOOL_DEFAULT + D3DUSAGE_DYNAMIC
//   secer (MANAGED yerine).
//
//   Bit secimi: 0x80000000 = DDSCAPS_OPTIMIZED ile cakisir, AMA Phoenix
//   static atlas caller'larinin TUMU dwCaps=0 ile cagiriyor; bayrak bizim
//   namespace'imizde guvenle kullanilabilir. DDraw7 fallback'i icinde
//   bayrak strip edilir (legacy validation kirilmasin diye).
//----------------------------------------------------------------------
#ifndef PHOENIX_TEXCAPS_STATIC_POOL
#define PHOENIX_TEXCAPS_STATIC_POOL  ((DWORD)0x80000000)
#endif

//----------------------------------------------------------------------
// Phase 2 (2026-05-14): D3D9 port FAZ 3 — IRenderSurface delegate.
// CDirectDrawSurface artik IRenderSurface'i icine alir; Lock/Blt/Fill/
// Restore cagrilari delegate uzerinden yapilir. m_pDDSurface raw
// LPDIRECTDRAWSURFACE7 escape hatch olarak korunur (sprite codec
// GetSurface() kullanicilari + Gamma*Pixel doruk-memory erisimi
// icin gerekli). Phase 4'te tum caller'lar IRenderSurface'a tasininca
// kalkar.
//----------------------------------------------------------------------
namespace phoenix { namespace render { class IRenderSurface; } }

/*-----------------------------------------------------------------------------
  Class DirectDraw Surface
-----------------------------------------------------------------------------*/
class CDirectDrawSurface : public CDirectDraw 
{
public :
	CDirectDrawSurface();
	~CDirectDrawSurface();

	// text out for debugging.
	void	GDI_Text(int x, int y, const char *str, COLORREF fcolor, 
																 COLORREF bcolor = 0, 
																 bool option = true);
	void	ShowFPS(int x, int y, COLORREF bcolor, COLORREF fcolor); // Displays the FPS.


	//------------------------------------------------------------
	// Gamma
	//------------------------------------------------------------
	void			GammaBox555(RECT* pRect, int p);
	void			GammaBox565(RECT* pRect, int p);
	static void		Gamma4Pixel(void *pDest, int len, int p)	{ (*s_GammaFunction)(pDest, len, p); }		
	static void		Gamma4Pixel565(void *pDest, int len, int p);
	static void		Gamma4Pixel555(void *pDest, int len, int p);
	static void		SetGammaFunction()
	{
		if (CDirectDraw::Is565())
			s_GammaFunction = Gamma4Pixel565;
		else 
			s_GammaFunction = Gamma4Pixel555;
	}

	//------------------------------------------------------------
	// Init
	//------------------------------------------------------------
	bool	InitBacksurface();	
	bool	InitOffsurface(int wWidth, int wHeight, DWORD dwCaps = 0);	
	bool	InitFromBMP(LPCSTR szBitmap, DWORD dwCaps = 0);
	bool	InitTextureSurface(int wWidth, int wHeight, DWORD dwCaps = 0, const LPDDPIXELFORMAT pDDPF = NULL);
	bool	InitTextureSurfaceFromBMP(LPCSTR szBitmap, DWORD dwCaps = 0);

	// 2026-05-16 R2: POOL_DEFAULT pool static texture variant
	//   Atlas/sprite-pack texture'lar (CTexturePack, CTexturePartManager turevleri)
	//   write-once at load + read-many at draw karakterli. D3DPOOL_MANAGED ile
	//   D3D9 driver *iki kopya* (sysmem + vidmem) tutar — Phoenix runtime ~150 MB
	//   sysmem ek yuke maruz. D3DPOOL_DEFAULT + D3DUSAGE_DYNAMIC ile sadece vidmem
	//   kopyasi var, sysmem kazanci buyuk. Lock semantic'i KORUNUR (DYNAMIC pool
	//   Lock/Unlock disardan calismaya devam eder).
	//
	//   Sentinel-bit pattern: dwCaps PHOENIX_TEXCAPS_STATIC_POOL bayragini set eder
	//   ve InitTextureSurface'a delegate eder. Driver pool/usage secimi bu bayraga
	//   gore yapilir. Bayrak D3D9 path icin kullanilir; DDraw7 fallback'inde
	//   striplenir (legacy DDSCAPS validation kirilmasin diye).
	//
	//   Detay: DXLib/CDirectDrawSurface.cpp InitTextureSurface body.
	bool	InitTextureSurfaceStatic(int wWidth, int wHeight, DWORD dwCaps = 0, const LPDDPIXELFORMAT pDDPF = NULL);

	//------------------------------------------------------------
	// Load bitmap from File
	//------------------------------------------------------------
	bool	ReLoadBMP(LPCSTR szBitmap);		
	bool	SaveToBMP(const char* szFilename);

	//------------------------------------------------------------
	// Restore Lost Surface
	//------------------------------------------------------------
	bool	Restore();

	//------------------------------------------------------------
	// Set Transparency color
	// `Blt할 때 반드시 해줘야 한다.
	//
	// Phase 3 (2026-05-14): IRenderSurface contract'a uygun colorkey
	// accessor'lari da expose edildi. Public API'ye yeni metot eklemek
	// 16+ caller dosyasini etkilemez — sadece ileri-uyumlu (Phase 4 switch
	// sonrasi caller'lar GetColorKey/HasColorKey ile alpha-test referansini
	// okuyabilir). Mevcut SetTransparency davranisi DEGISMEDI.
	//------------------------------------------------------------
	void	SetTransparency(DWORD dwValue);
	DWORD	GetColorKey() const;
	bool	HasColorKey() const;

	//------------------------------------------------------------
	// Blt
	//------------------------------------------------------------
	void	Blt(POINT* pPoint, CDirectDrawSurface* SourceSurface, RECT*  pRect);
	void	BltNoColorkey(POINT* pPoint, CDirectDrawSurface* SourceSurface, RECT*  pRect);
	void	Blt(RECT* pDestRect, CDirectDrawSurface* SourceSurface, RECT* pSourceRect);	

	// 현재 surface의 pPoint에 PrimarySurface의 pRect를 blt한다.
	void	BltPrimarySurface(POINT* pPoint, RECT* pRect);

	//------------------------------------------------------------
	// DirectDraw 제공 Drawing 함수들...
	//------------------------------------------------------------
	void	FillSurface(WORD color);
	void	FillRect(RECT* rect, WORD Color);
		
	//------------------------------------------------------------
	// Get
	//------------------------------------------------------------
	inline void *	GetSurfacePointer() { return m_ddsd.lpSurface; }
	inline long		GetSurfacePitch() { return m_ddsd.lPitch; }
	inline LPDIRECTDRAWSURFACE7 &	GetSurface() { return m_pDDSurface; } const
	inline DDSURFACEDESC2 * GetDDSD() { return &m_ddsd; }
	inline int		GetWidth() const	{ return m_Width; } // no const...
	inline int		GetHeight() const	{ return m_Height; } // no const...

	//------------------------------------------------------------
	// Clip 영역
	//------------------------------------------------------------
	///*
	inline void		SetClipLeft(int Left)				{ if (Left>=0) m_ClipLeft = Left; }
	inline void		SetClipTop(int Top)					{ if (Top>=0) m_ClipTop = Top; }
	inline void		SetClipLeftTop(int Left, int Top)	{ if (Left>=0) m_ClipLeft = Left; 
															if (Top>=0) m_ClipTop = Top; }

	inline void		SetClipRight(int Right)			{ if (Right<=m_Width) m_ClipRight = Right; }
	inline void		SetClipBottom(int Bottom)		{ if (Bottom<=m_Height) m_ClipBottom = Bottom; }
	inline void		SetClipRightBottom(int Right, int Bottom)	{ if (Right<=m_Width) m_ClipRight = Right; 
														if (Bottom<=m_Height) m_ClipBottom = Bottom; }
	
	inline void		SetClip(RECT* pRect)			{ SetClipLeftTop(pRect->left, pRect->top); SetClipRightBottom(pRect->right, pRect->bottom); }
	inline void		SetClipNULL()					{ m_ClipLeft=0; m_ClipTop=0; m_ClipRight=m_Width; m_ClipBottom=m_Height; }

	inline int		GetClipRight() const			{ return m_ClipRight; } // no const...
	inline int		GetClipBottom() const			{ return m_ClipBottom; } // no const...
	inline int		GetClipLeft() const				{ return m_ClipLeft; } // no const...
	inline int		GetClipTop() const				{ return m_ClipTop; } // no const...	
	
	
	//*/

	//
	// ! Pitch와 Surface pointer를 얻으려면, CDirectDraw의 GetDDDesc()를 이용한다.
	//

	//------------------------------------------------------------
	// Lock / Unlock
	//------------------------------------------------------------		
	bool		Lock();
	bool		Unlock();
	bool		IsLock() const		{ return m_bLock; }

	// 음... 그냥 cast해서 쓰는게...
	void	LockW(WORD*& lpSurface, WORD& lPitch);
	//void	LockDW(DWORD*& lpSurface, WORD& lPitch);
	//void	LockQW(QWORD*& lpSurface, WORD& lPitch);

	//------------------------------------------------------------		
	// operator
	//------------------------------------------------------------		
	operator const LPDIRECTDRAWSURFACE7&()	{ return m_pDDSurface; }
	
	// class 내부에서 쓰는 함수
protected :
	bool	CopyBitmap(HBITMAP hbm, int x, int y, int dx, int dy);

protected :
	LPDIRECTDRAWSURFACE7	m_pDDSurface;

	// Phase 2 (2026-05-14): IRenderSurface delegate. m_pRenderSurface
	// genelde m_pDDSurface'i wrap eden DDraw7Surface — sprite codec'in
	// Lock/Unlock yolu artik IRenderSurface uzerinden gecer. Init*
	// metotlari surface olusturup hem m_pDDSurface'a hem
	// m_pRenderSurface'a atar. Sahiplik m_pRenderSurface tarafindadir
	// (owned=false) cunku CDirectDrawSurface destructor m_pDDSurface'i
	// kendi release eder; double-release engellenir.
	phoenix::render::IRenderSurface*	m_pRenderSurface;

	// Fix #11 (2026-05-14): m_pRenderSurface sahiplik bayragi. Default
	// true (CDirectDrawSurface kendisi yarattigi delegate'i siler). D3D9
	// modunda InitBacksurface + InitOffsurface (full-screen) D3D9Backend
	// singleton composition surface'ini paylasarak ref-counted Lock
	// kullanir; bu durumda owned=false (singleton'i silmemeli).
	bool					m_bRenderSurfaceOwned;

	// Surface의 크기, 쓰임새가 많으므로 member로 가지고 있는다.
	int					m_Width;
	int					m_Height;

	// surface가 back surface인가를 나타내는 flag.
	bool					m_bBackSurface;	

	// Lock이 된 상태인가?
	bool					m_bLock;	

	// 실제 크기보다 작은.. Clip이 되어서 보여지는 영역을 설정	
	int					m_ClipRight;
	int					m_ClipBottom;
	int					m_ClipLeft;
	int					m_ClipTop;

	static	void		(*s_GammaFunction)(void *pDest, int len, int p);
};

#endif

