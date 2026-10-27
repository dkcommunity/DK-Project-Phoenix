//----------------------------------------------------------------------
// CTexturePack.cpp
//----------------------------------------------------------------------

#include "CTexturePack.h"
#include "CDirect3D.h"
#include <stdio.h>
#include <io.h>
#include <fstream>
extern HWND g_hWnd;

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------

CTexturePack::CTexturePack()
{
	m_nTextures = 0;
	m_pTextures = NULL;
}

CTexturePack::~CTexturePack()
{
	// array를 메모리에서 제거한다.
	Release();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Init
//----------------------------------------------------------------------
void
CTexturePack::Init(int count)
{
	// 개수가 없을 경우 
	if (count==0) 
		return;

	// 일단 해제
	Release();

	// 메모리 잡기
	m_nTextures = count;
	m_pTextures = new CSpriteSurface [m_nTextures];
}


//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
CTexturePack::Release()
{
	if (m_pTextures != NULL)
	{
		// 모든 CTexture를 지운다.
		delete [] m_pTextures;

		m_pTextures = NULL;		
		m_nTextures = 0;
	}
}

//----------------------------------------------------------------------
// Init
//----------------------------------------------------------------------
// AlphaSpritePack의 정보로 TexturePack을 생성한다.
//
// PixelFormat은 4:4:4:4만.. 일단 지원하기로 한다.
// AlphaSpritePack자체가 Alpha값이 있으므로... 적당~하다고 생각함.
//----------------------------------------------------------------------
bool
CTexturePack::Init( CAlphaSpritePack& ASPK )
{
	//--------------------------------------------
	// 4:4:4:4에 맞는 PixelFormat찾기
	//--------------------------------------------
	/*
	DDPIXELFORMAT	PixelFormat4444;
	if (!CDirect3D::FindBestPixelFormat(4,4,4,4, &PixelFormat4444))
	{
		//InitFail("Initialize Failed! - Can't Find PixelFormat");
		return false;
	}
	*/

	// 생성할 TextureSurface의 크기
	int width, height;


	/*
	DDSCAPS2 ddsCaps2;
	DWORD    dwTotal;   
	DWORD    dwFree;     
	*/

	//---------------------------------------------
	// memory잡기
	//---------------------------------------------
	Init( ASPK.GetSize() );
	
	//---------------------------------------------
	// ASPK ---> TextureSurface 로 변환
	//---------------------------------------------
	for (int sur=0; sur<m_nTextures; sur++)
	{		
		//---------------------------------------------------
		// Video Memory 남은거 보기
		//---------------------------------------------------
		/*
		ZeroMemory(&ddsCaps2, sizeof(ddsCaps2)); 
		ddsCaps2.dwCaps = DDSCAPS_TEXTURE; 

		HRESULT hr = CDirectDraw::GetDD()->GetAvailableVidMem(&ddsCaps2, &dwTotal, &dwFree);

		if (FAILED(hr))
			return false;

		char str[80];
		sprintf(str, "[%d/%d] mem=%ld/%ld", sur+1, m_nTextures, dwFree, dwTotal);
		MessageBox( g_hWnd, str, NULL, MB_OK);
		*/

		width = ASPK[sur].GetWidth();
		height = ASPK[sur].GetHeight();
		CDirect3D::GetTextureSize(width, height);

		// 크기 보기
		/*
		char str[80];
		sprintf(str, "[%d/%d] mem=%d/%d", sur+1, m_nTextures, width, height);
		MessageBox( g_hWnd, str, NULL, MB_OK);
		*/
	

		//---------------------------------------------------
		// TextureSurface 생성
		//---------------------------------------------------		
		// 2026-05-16 R2: Static atlas (write-once + read-many) — POOL_DEFAULT.
		m_pTextures[sur].InitTextureSurfaceStatic(width, height, 0, CDirect3D::GetPixelFormat4444());
		POINT point;
		point.x = 0;
		point.y = 0;
		m_pTextures[sur].Lock();

		int i;

		//---------------------------------------------------
		// Texture Surface 초기화
		//---------------------------------------------------
		WORD *pSurface = (WORD*)m_pTextures[sur].GetSurfacePointer();
				//,	*pSurfaceTemp;
		long pitch	= m_pTextures[sur].GetSurfacePitch();

		DWORD width2 = width << 1;	// *2 
		for (int i =0; i<height; i++)
		{
			/*
			WORD* pSurfaceTemp = pSurface;
			for (int j=0; j<width; j++)
			{
				*pSurfaceTemp = 0x8000;
				pSurfaceTemp++;
			}
			*/
			memset(pSurface, 0, width2);
			pSurface = (WORD*)((BYTE*)pSurface + pitch);
		}

		// AlphaSprite출력
		m_pTextures[sur].BltAlphaSprite4444(&point, &ASPK[ sur ]);
		
		m_pTextures[sur].Unlock();

		// PixelFormat에 따라 Colorkey가 바뀔 수 있다. T_T;;
		//m_pTextures[sur].SetTransparency( 0x0000 );
	}

	return true;
}

//----------------------------------------------------------------------
// InitPart
//----------------------------------------------------------------------
// Shadow SpritePack의 일부를... Texture로 생성
//----------------------------------------------------------------------
bool
CTexturePack::InitPart( CShadowSpritePack& SSPK, TYPE_SPRITEID firstShadowSpriteID, TYPE_SPRITEID lastShadowSpriteID )
{
	//--------------------------------------------
	// TextureSurface'in Max boyutu + caps bayraklari hakkindaki bilgi
	//
	// Phase 5 (2026-05-14): Caps lookup tek noktada toplandi. CDirect3D class
	// hem D3D7 (legacy) hem D3D9 (Phase 4 init) init asamasinda
	// m_TextureWidthMax/Height/bPow2/bSquareOnly degerlerini doldurur. Burada
	// flag-bagimsiz okuruz; eski D3D7 GetCaps cagrisi yapilmaz (D3D9 modunda
	// CDirect3D::GetDevice() NULL doner ve compile-fail/runtime crash riski
	// vardi).
	//--------------------------------------------
	const int  maxWidth        = CDirect3D::GetTextureWidthMax();
	const int  maxHeight       = CDirect3D::GetTextureHeightMax();
	const bool bTexturePow2    = CDirect3D::IsTexturePow2();
	const bool bSquareOnly     = CDirect3D::IsTextureSquareOnly();

	// 생성할 TextureSurface의 크기
	int width, height;


	/*
	DDSCAPS2 ddsCaps2;
	DWORD    dwTotal;   
	DWORD    dwFree;     
	*/
	
	//---------------------------------------------
	// SSPK ---> TextureSurface 로 변환
	//---------------------------------------------
	for (int sur=firstShadowSpriteID; sur<lastShadowSpriteID; sur++)
	{		
		//---------------------------------------------------
		// Video Memory 남은거 보기
		//---------------------------------------------------
		/*
		ZeroMemory(&ddsCaps2, sizeof(ddsCaps2)); 
		ddsCaps2.dwCaps = DDSCAPS_TEXTURE; 

		HRESULT hr = CDirectDraw::GetDD()->GetAvailableVidMem(&ddsCaps2, &dwTotal, &dwFree);

		if (FAILED(hr))
			return false;

		char str[80];
		sprintf(str, "[%d/%d] mem=%ld/%ld", sur+1, m_nTextures, dwFree, dwTotal);
		MessageBox( g_hWnd, str, NULL, MB_OK);
		*/

		//---------------------------------------------------------------
		// 하드웨어가 지원하는 TextureSurface의 크기를 결정한다.
		//---------------------------------------------------------------
		// Adjust width and height to be powers of 2, if the device requires it
		// Phase 5: caps lookup CDirect3D static accessor'lar uzerinden
		// (flag-aware — D3D7 ve D3D9 ikisi de ayni durumu doldurur).
		width	= SSPK[sur].GetWidth();
		height	= SSPK[sur].GetHeight();
		if( bTexturePow2 )
		{
			for( width=1;  SSPK[sur].GetWidth()>width;   width<<=1 );
			for( height=1; SSPK[sur].GetHeight()>height; height<<=1 );
		}

		width  = min( width,  maxWidth );
		height = min( height, maxHeight );

		// Make the texture square, if the driver requires it
		if( bSquareOnly )
		{
			if( width > height )
				height = width;
			else
				width  = height;
		}

		// 크기 보기
		/*
		char str[80];
		sprintf(str, "[%d/%d] mem=%d/%d", sur+1, m_nTextures, width, height);
		MessageBox( g_hWnd, str, NULL, MB_OK);
		*/
	

		//---------------------------------------------------
		// TextureSurface 생성
		//---------------------------------------------------		
		// 2026-05-16 R2: Static atlas (write-once + read-many) — POOL_DEFAULT.
		m_pTextures[sur].InitTextureSurfaceStatic(width, height, 0, CDirect3D::GetPixelFormat4444());
		POINT point;
		point.x = 0;
		point.y = 0;
		m_pTextures[sur].Lock();

		int i;

		//---------------------------------------------------
		// Texture Surface 초기화
		//---------------------------------------------------
		WORD *pSurface = (WORD*)m_pTextures[sur].GetSurfacePointer();
				//,	*pSurfaceTemp;
		long pitch	= m_pTextures[sur].GetSurfacePitch();

		DWORD width2 = width << 1;	// *2 
		for (int i =0; i<height; i++)
		{
			/*
			WORD* pSurfaceTemp = pSurface;
			for (int j=0; j<width; j++)
			{
				*pSurfaceTemp = 0x8000;
				pSurfaceTemp++;
			}
			*/
			memset(pSurface, 0, width2);
			pSurface = (WORD*)((BYTE*)pSurface + pitch);
		}

		// ShadowSprite출력
		m_pTextures[sur].BltShadowSprite4444(&point, &SSPK[ sur ], 0x8000);
		
		m_pTextures[sur].Unlock();

		// PixelFormat에 따라 Colorkey가 바뀔 수 있다. T_T;;
		//m_pTextures[sur].SetTransparency( 0x0000 );
	}

	return true;
}
