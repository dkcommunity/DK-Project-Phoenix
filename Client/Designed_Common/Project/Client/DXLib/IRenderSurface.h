//-----------------------------------------------------------------------------
// IRenderSurface.h
//
// Phoenix D3D9 native surface abstraction.
//
// Backend-bagimsiz surface arabirimi. Aktif implementasyon D3D9Surface'tir ve
// IDirect3DSurface9/IDirect3DTexture9 uzerinden calisir.
//
// Sprite codec kontrati (CSpriteSurface.cpp 13670 LOC):
//
//     Lock(rect)  -> rect.pBits  = surface memory base
//     Lock(rect)  -> rect.pitch  = byte stride
//     Unlock()
//
// Bu kontrat aynen korunur — sprite codec abstraction layer eklenmesine
// rahmen davranis degistirmez.
//
// 2026-05-14 Phase 0
//-----------------------------------------------------------------------------

#ifndef __PHOENIX_IRENDER_SURFACE_H__
#define __PHOENIX_IRENDER_SURFACE_H__

#include "RenderTypes.h"

namespace phoenix
{
namespace render
{

//-----------------------------------------------------------------------------
// IRenderSurface
//
// Surface (offscreen/texture/back-buffer) abstraction.
//-----------------------------------------------------------------------------
class IRenderSurface
{
public:
    virtual ~IRenderSurface() {}

    //-------------------------------------------------------------------------
    // Lock / Unlock — sprite codec kritik kontrati
    //
    // Lock basariliysa outRect.pBits + outRect.pitch dolu. pitch BYTE cinsinden.
    // Surface lost durumunda Wait flag ile restore + retry. Unlock'i bizzat
    // sirayla cagirmak zorunlu (DDraw7 ve D3D9 ikisi de bunu gerektirir).
    //-------------------------------------------------------------------------
    virtual bool Lock(LockedRect& outRect, DWORD flags) = 0;
    virtual bool Unlock() = 0;
    virtual bool IsLocked() const = 0;

    //-------------------------------------------------------------------------
    // Surface metadata
    //-------------------------------------------------------------------------
    virtual int           GetWidth() const = 0;
    virtual int           GetHeight() const = 0;
    virtual SurfaceFormat GetFormat() const = 0;

    //-------------------------------------------------------------------------
    // Clip rect — CDirectDrawSurface'de inline m_Clip{Left,Top,Right,Bottom}
    // Phoenix render kodu surface'in icindeki clip alanini Blt sirasinda
    // kullaniyor (CDirectDrawSurface::Blt clip mantigi).
    //-------------------------------------------------------------------------
    virtual void SetClip(const RECT& rc) = 0;
    virtual void SetClipNull() = 0;
    virtual int  GetClipLeft() const = 0;
    virtual int  GetClipTop() const = 0;
    virtual int  GetClipRight() const = 0;
    virtual int  GetClipBottom() const = 0;

    //-------------------------------------------------------------------------
    // Transparent color (DDraw7 SetColorKey muadili). DDraw7 path'inde Blt
    // sirasinda DDCKEY_SRCBLT olarak kullaniliyor. D3D9 path'inde ID3DXSprite
    // veya alpha-test ile karsilanir.
    //
    // Phase 3 (2026-05-14): Contract tutarliligi — GetColorKey + HasColorKey
    // pure virtual eklendi. DDraw7Surface ve D3D9Surface state'i kendi
    // tarafinda tutar; Phase 4'te BltColorKey path'lerinde alpha-test
    // referansi olarak kullanilir (D3D9Backend bu state olmadan colorkey
    // pixel filtre uygulayamaz — magenta sprite kok-sebebinin parcasi).
    //
    // SetTransparency: arka uyumluluk icin korundu (Phoenix kod tabaninda
    // 12+ call site). SetColorKey aliasi olarak ileride asagidaki public
    // API'ye gecirilebilir; semantik ayni.
    //-------------------------------------------------------------------------
    virtual void  SetTransparency(DWORD colorValue) = 0;
    virtual DWORD GetColorKey() const = 0;
    virtual bool  HasColorKey() const = 0;

    //-------------------------------------------------------------------------
    // Blt operasyonlari
    //
    // BltColorKey      : src colorkey ile blit (sprite atlas -> back)
    // BltNoColorKey    : opaque blit
    // BltStretchKey    : colorkey + stretch (DDBLT_KEYSRC + DDBLTFX_ARITHSTRETCHY)
    // FillSurface      : tum surface'i tek renge boya
    // FillRect         : rect alani tek renge boya
    //
    // Phase 3 notu: DDraw7 path'i Blt overload'larinda DDBLT_KEYSRC /
    // DDBLTFAST_SRCCOLORKEY kullanmaya devam ediyor (mevcut davranis, regression 0).
    // D3D9 path'i ise SetTransparency ile saklanan colorkey'i manuel pixel
    // filtre olarak uyguluyor (D3D9'da surface-level SetColorKey API'si YOK).
    // Phase 4 switch sonrasi magenta sprite hatasi D3D9 path'inde otomatik
    // duzelir cunku BltColorKey/BltStretchKey colorkey state'ini ayri tutar.
    //-------------------------------------------------------------------------
    virtual void BltColorKey(POINT* pDestPoint, IRenderSurface* pSrc, RECT* pSrcRect) = 0;
    virtual void BltNoColorKey(POINT* pDestPoint, IRenderSurface* pSrc, RECT* pSrcRect) = 0;
    virtual void BltStretchKey(RECT* pDestRect, IRenderSurface* pSrc, RECT* pSrcRect) = 0;
    virtual void FillSurface(WORD color) = 0;
    virtual void FillRect(RECT* pRect, WORD color) = 0;

    //-------------------------------------------------------------------------
    // Surface lost recovery
    //-------------------------------------------------------------------------
    virtual bool Restore() = 0;
};

} // namespace render
} // namespace phoenix

#endif // __PHOENIX_IRENDER_SURFACE_H__
