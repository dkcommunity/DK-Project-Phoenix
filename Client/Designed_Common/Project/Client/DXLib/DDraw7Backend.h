//-----------------------------------------------------------------------------
// DDraw7Backend.h
//
// Phoenix D3D9 native port — Phase 0 abstraction layer.
//
// IRenderBackend + IRenderSurface'in DirectDraw 7 implementasyonu. Mevcut
// CDirectDraw + CDirectDrawSurface static state'ini WRAP eder; ozel state
// tutmaz (Phoenix code base'i hala dogrudan CDirectDraw::m_pDD vs okumakta,
// double-state riski yok bu sayede).
//
// Phase 1+'da DDraw7Backend canli kalir — paralel D3D9Backend ile run-time
// switch icin. CDirectDraw header'inda hicbir degisiklik yok; Phase 0 sadece
// alternatif arabirim sunar.
//
// 2026-05-14 Phase 0
//-----------------------------------------------------------------------------

#ifndef __PHOENIX_DDRAW7_BACKEND_H__
#define __PHOENIX_DDRAW7_BACKEND_H__

#include "IRenderBackend.h"
#include "IRenderSurface.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <ddraw.h>

namespace phoenix
{
namespace render
{

class DDraw7Backend;

//-----------------------------------------------------------------------------
// DDraw7Surface
//
// IRenderSurface'i LPDIRECTDRAWSURFACE7 ile implement eder. Bir nesnenin
// ya kendi olusturdugu surface'i (m_bOwned=true) ya da back-buffer/primary
// referansini (m_bOwned=false) tutmasi mumkun.
//
// !! Phoenix sprite codec hala CDirectDrawSurface::GetSurface() ile raw
// LPDIRECTDRAWSURFACE7 alabilir; DDraw7Surface ondan bagimsizdir. Phase 0'da
// iki yol paralel yasiyor: legacy CDirectDrawSurface (sprite codec) +
// modern DDraw7Surface (yeni eklenecek call site'lar).
//-----------------------------------------------------------------------------
class DDraw7Surface : public IRenderSurface
{
public:
    DDraw7Surface(LPDIRECTDRAWSURFACE7 pSurface, int width, int height, bool owned);
    virtual ~DDraw7Surface();

    // IRenderSurface
    virtual bool Lock(LockedRect& outRect, DWORD flags);
    virtual bool Unlock();
    virtual bool IsLocked() const { return m_bLocked; }

    virtual int           GetWidth() const  { return m_Width; }
    virtual int           GetHeight() const { return m_Height; }
    virtual SurfaceFormat GetFormat() const;

    virtual void SetClip(const RECT& rc);
    virtual void SetClipNull();
    virtual int  GetClipLeft() const   { return m_ClipLeft; }
    virtual int  GetClipTop() const    { return m_ClipTop; }
    virtual int  GetClipRight() const  { return m_ClipRight; }
    virtual int  GetClipBottom() const { return m_ClipBottom; }

    virtual void  SetTransparency(DWORD colorValue);
    virtual DWORD GetColorKey() const { return m_ColorKey; }
    virtual bool  HasColorKey() const { return m_bHasColorKey; }

    virtual void BltColorKey(POINT* pDestPoint, IRenderSurface* pSrc, RECT* pSrcRect);
    virtual void BltNoColorKey(POINT* pDestPoint, IRenderSurface* pSrc, RECT* pSrcRect);
    virtual void BltStretchKey(RECT* pDestRect, IRenderSurface* pSrc, RECT* pSrcRect);
    virtual void FillSurface(WORD color);
    virtual void FillRect(RECT* pRect, WORD color);

    virtual bool Restore();

    // DDraw7-specific accessor — Phase 0 escape hatch. Phase 1+'da sprite
    // codec D3D9 path'inde IDirect3DSurface9 ile birlikte calismayacak —
    // bu accessor sadece DDraw7Backend canli oldugu surece geri donus.
    LPDIRECTDRAWSURFACE7 GetRawDDSurface() const { return m_pDDSurface; }

private:
    LPDIRECTDRAWSURFACE7  m_pDDSurface;
    DDSURFACEDESC2        m_LockedDesc;
    int                   m_Width;
    int                   m_Height;
    int                   m_ClipLeft;
    int                   m_ClipTop;
    int                   m_ClipRight;
    int                   m_ClipBottom;

    // Phase 3 (2026-05-14): colorkey state DDraw7 path'inde de tutuluyor.
    // DDraw7 cizim yolunda DDBLT_KEYSRC raw DirectDraw API'sine gider
    // (m_pDDSurface->SetColorKey -> donanim driver tarafi state); ama Phase 4
    // switch sonrasi composition path D3D9Backend BltColorKey'e gider, oradaki
    // pixel filtre HasColorKey() + GetColorKey() okur. Iki backend arasinda
    // davranis parity icin DDraw7Surface da state'i tutar.
    DWORD                 m_ColorKey;
    bool                  m_bHasColorKey;

    bool                  m_bOwned;       // Release on dtor?
    bool                  m_bLocked;
};

//-----------------------------------------------------------------------------
// DDraw7Backend
//
// IRenderBackend'i CDirectDraw static state'i kullanarak implement eder.
// Phase 0'da Init() CDirectDraw::Init'i delegate eder — yeni DDraw7 device
// olusturmaz, double-init riskini onler.
//-----------------------------------------------------------------------------
class DDraw7Backend : public IRenderBackend
{
public:
    DDraw7Backend();
    virtual ~DDraw7Backend();

    static DDraw7Backend& Instance();

    // IRenderBackend
    virtual bool Init(HWND hWnd, WORD width, WORD height, ScreenMode mode, bool use3D, bool useIME);
    virtual void Shutdown();
    virtual bool RestoreAll();

    virtual void Present();
    virtual void FlipToGDISurface();

    virtual IRenderSurface* CreateOffscreenSurface(int width, int height, DWORD flags);
    virtual IRenderSurface* CreateTextureSurface(int width, int height, DWORD flags);
    virtual IRenderSurface* GetBackBuffer();
    virtual IRenderSurface* GetPrimarySurface();

    virtual bool IsFullscreen() const;
    virtual WORD GetScreenWidth() const;
    virtual WORD GetScreenHeight() const;
    virtual SurfaceFormat GetPrimaryFormat() const;

    virtual bool IsGammaSupported() const;
    virtual void SetGammaRamp(WORD step);
    virtual void RestoreGammaRamp();
    virtual void SetAddGammaRamp(WORD r, WORD g, WORD b);

    virtual void OnMove();
    virtual HWND GetWindow() const;

private:
    // Phase 0'da kendi state yok — CDirectDraw static state'i kullaniliyor.
    // Backbuffer/primary IRenderSurface adapter objelerini yeniden olusturmaktan
    // kacinmak icin cache.
    DDraw7Surface* m_pCachedBack;
    DDraw7Surface* m_pCachedPrimary;
};

} // namespace render
} // namespace phoenix

#endif // __PHOENIX_DDRAW7_BACKEND_H__
