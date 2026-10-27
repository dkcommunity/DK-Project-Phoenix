//-----------------------------------------------------------------------------
// D3D9Backend.h
//
// Phoenix D3D9 native render backend.
//
// IRenderBackend + IRenderSurface'in Direct3D 9 implementasyonu. Phoenix
// sprite codec'i (CSpriteSurface.cpp 13670 LOC) DOKUNULMAZ kalir; 16-bit
// composition buffer pattern ile sprite codec'in `Lock()` kontrati korunur:
//
//     Lock(rect) -> rect.pBits = D3DLOCKED_RECT.pBits     (sysmem R5G6B5 base)
//     Lock(rect) -> rect.pitch = D3DLOCKED_RECT.Pitch     (byte stride)
//     Unlock()
//
// D3D9 device 32-bit primary (D3DFMT_X8R8G8B8 — modern Windows DWM uyumlu),
// sprite composition 16-bit offscreen plain surface (D3DPOOL_SYSTEMMEM +
// D3DFMT_R5G6B5). Present pipeline:
//
//     1. Sprite codec composition buffer'a write
//     2. UpdateSurface(compositionBuffer -> compositionTexture)
//     3. Fullscreen quad ile back buffer'a cizim (TLVERTEX, XYZRHW)
//     4. Present()
//
// 2026-06-12 itibariyla D3D9Backend tek aktif render backend'dir.
//-----------------------------------------------------------------------------

#ifndef __PHOENIX_D3D9_BACKEND_H__
#define __PHOENIX_D3D9_BACKEND_H__

#include "IRenderBackend.h"
#include "IRenderSurface.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// Defensive: TU'da daha onceden DX7 D3D.h dahil edilmis olabilir (DIRECT3D_VERSION
// 0x0700 set'li). Windows SDK d3d9.h `#if(DIRECT3D_VERSION >= 0x0900)` gate'i
// ile icerigini atlar; IDirect3DDevice9Ex tanimsiz kalir. d3d9.h dahil etmeden
// once versiyon 0x0900'e cikariyoruz.
#undef DIRECT3D_VERSION
#define DIRECT3D_VERSION 0x0900
#include <d3d9.h>

namespace phoenix
{
namespace render
{

class D3D9Backend;

//-----------------------------------------------------------------------------
// D3D9Surface
//
// IRenderSurface'i IDirect3DSurface9 ile implement eder. 3 rol:
//   - composition buffer (sysmem R5G6B5, sprite codec hedef)
//   - offscreen plain (sysmem/managed, render-time atlas vs)
//   - texture-bound surface (IDirect3DTexture9::GetSurfaceLevel donus)
//
// m_bOwned=true ise Release() destructor'da; aksi halde caller sorumlu
// (texture surface refs vs.).
//-----------------------------------------------------------------------------
class D3D9Surface : public IRenderSurface
{
public:
    D3D9Surface(IDirect3DSurface9* pSurface, int width, int height,
                SurfaceFormat format, bool owned);
    virtual ~D3D9Surface();

    // IRenderSurface
    virtual bool Lock(LockedRect& outRect, DWORD flags);
    virtual bool Unlock();
    virtual bool IsLocked() const { return m_bLocked; }

    virtual int           GetWidth() const  { return m_Width; }
    virtual int           GetHeight() const { return m_Height; }
    virtual SurfaceFormat GetFormat() const { return m_Format; }

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

    // D3D9-specific accessor — D3D9Surface arasi Blt operasyonu icin.
    IDirect3DSurface9* GetRawD3DSurface() const { return m_pSurface; }

    // 2026-09-02 crash fix — composition singleton wrapper'ini yeni COM
    // surface'a yeniden baglar (obje kimligi korunur; Yol 1 tuketicileri
    // dangling kalmaz). Detay: D3D9Backend.cpp impl yorumu.
    void Rebind(IDirect3DSurface9* pSurface, int width, int height);

private:
    IDirect3DSurface9* m_pSurface;
    int                m_Width;
    int                m_Height;
    SurfaceFormat      m_Format;
    int                m_ClipLeft;
    int                m_ClipTop;
    int                m_ClipRight;
    int                m_ClipBottom;
    DWORD              m_ColorKey;       // SetTransparency degerini saklar
    bool               m_bHasColorKey;   // SetTransparency cagirildi mi
    bool               m_bOwned;         // Release on dtor?
    bool               m_bLocked;

    // 2026-05-14 Fix #11: Re-entrant Lock support. Composition buffer shared
    // by g_pBack + g_pLast (singleton instance); BltColorKey ic akisi src+dst
    // ayni surface ise iki kez LockRect cagirir → SYSTEMMEM surface'da driver
    // undefined behavior → c0000374 heap corruption. Ref counting:
    //   - m_LockCount == 0 : ilk Lock cagrisinda LockRect cagir, sonuc cache'le
    //   - m_LockCount  > 0 : cached pBits/pitch dondur, ++m_LockCount
    //   - Unlock: --m_LockCount; 0'a inince UnlockRect cagir
    int                m_LockCount;
    D3DLOCKED_RECT     m_CachedLock;
};

//-----------------------------------------------------------------------------
// D3D9Backend
//
// IRenderBackend'i Direct3D 9 (IDirect3D9Ex / IDirect3DDevice9Ex) ile
// implement eder. 16-bit composition buffer pattern ile sprite codec
// dokunulmadan calisir.
//
// Cizim mimarisi:
//
//   Sprite codec -> m_pCompositionBuffer (sysmem R5G6B5)
//   Frame son    -> UpdateSurface(buffer -> texture)
//   BeginScene   -> SetTexture + DrawPrimitiveUP (fullscreen quad TLVERTEX)
//   EndScene     -> Present
//
// PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE (vsync off, Phoenix
// MTopView frame-pacing'i koruyacak).
//-----------------------------------------------------------------------------
class D3D9Backend : public IRenderBackend
{
public:
    D3D9Backend();
    virtual ~D3D9Backend();

    static D3D9Backend& Instance();

    // IRenderBackend
    virtual bool Init(HWND hWnd, WORD width, WORD height, ScreenMode mode, bool use3D, bool useIME);
    virtual void Shutdown();
    virtual bool RestoreAll();
    virtual bool Resize(WORD width, WORD height);	// O3 — canli cozunurluk

    virtual void Present();
    virtual void FlipToGDISurface();

    virtual IRenderSurface* CreateOffscreenSurface(int width, int height, DWORD flags);
    virtual IRenderSurface* CreateTextureSurface(int width, int height, DWORD flags);
    virtual IRenderSurface* GetBackBuffer();
    virtual IRenderSurface* GetPrimarySurface();

    virtual bool IsFullscreen() const;
    virtual WORD GetScreenWidth() const  { return m_Width; }
    virtual WORD GetScreenHeight() const { return m_Height; }
    virtual SurfaceFormat GetPrimaryFormat() const;

    virtual bool IsGammaSupported() const;
    virtual void SetGammaRamp(WORD step);
    virtual void RestoreGammaRamp();
    virtual void SetAddGammaRamp(WORD r, WORD g, WORD b);

    virtual void OnMove();
    virtual HWND GetWindow() const { return m_hWnd; }

    // D3D9-specific accessor — composition buffer'i sprite codec'e expose
    // etmek icin (Phase 4'te kullanilacak). Caller Lock/Unlock kontratina
    // bagli.
    D3D9Surface* GetCompositionSurface() { return m_pCompositionSurface; }

    // 2026-05-16 Dalga 2 — C2: Present dirty-rect tracking.
    //
    // Conservative phase 1: MarkDirty cagrilari composition buffer'in
    // degisen bolgesini birlesim rect olarak akumule eder; Present icindeki
    // UpdateSurface bu rect ile cagrilir. MarkDirty hic cagrilmadiysa
    // (legacy callsite'lar) Present default'u "tum surface" olarak davranir
    // (NULL src rect) — backward compat. MarkDirtyFull() exposed: caller
    // explicit "full frame" sinyali verebilir (ornegin sahne tam yenileme).
    //
    // Thread safety: Phoenix render path single-threaded (MTopView::Draw
    // ana thread'de). Mutex yok.
    void MarkDirty(const RECT& r);
    void MarkDirtyFull();
    bool IsDirtyTracked() const { return m_HasDirty; }

    // 2026-05-16 Dalga 2 — M1: frame-level DC cache hook.
    // Present basinda BeginFrame, sonunda EndFrame cagirilir. FL2 g_FL2_GetDC
    // bu frame penceresi icinde gercek GetDC'yi bir kez yapip cache'ler;
    // birden fazla caller (Description, LISTBOX, vs.) ayni HDC'yi paylasir.
    static void BeginFrame();
    static void EndFrame();
    static bool IsFrameInProgress();

    // Phase 4: D3D9 device'i CDirect3D class'i + 3D render path'lerine expose
    // eder. PHOENIX_RENDER_BACKEND_D3D9=1 aktivasyonu sonrasi CDirect3D::Init
    // bu accessor uzerinden device referansini paylasir; legacy CDirect3D
    // path (IDirect3DDevice7 m_pD3DD) NULL kalir. 3D render path (CD3DObject,
    // CD3DTextureEffect, MTopViewDraw.inl SetRenderState/DrawPrimitive)
    // Phase 4.5/4.6'da D3D9 device'a yonlendirilir.
    IDirect3DDevice9Ex* GetD3D9Device() { return m_pDevice; }
    IDirect3D9Ex*       GetD3D9() { return m_pD3D; }

private:
    // Device + window
    IDirect3D9Ex*           m_pD3D;
    IDirect3DDevice9Ex*     m_pDevice;
    HWND                    m_hWnd;
    D3DPRESENT_PARAMETERS   m_PresentParams;
    // D3D9Ex exclusive fullscreen: CreateDeviceEx/ResetEx icin zorunlu.
    // Windowed'da kullanilmaz (NULL pointer gecilir).
    D3DDISPLAYMODEEX        m_FullscreenDisplayMode;
    WORD                    m_Width;
    WORD                    m_Height;
    bool                    m_bFullscreen;

    // Exclusive FS icin EnumAdapterModesEx ile en iyi refresh rate secer.
    bool PickFullscreenDisplayMode(WORD width, WORD height);

    // 16-bit composition buffer (sprite codec hedefi, sysmem R5G6B5)
    IDirect3DSurface9*      m_pCompositionBuffer;
    D3D9Surface*            m_pCompositionSurface;   // wrapper, owned=false

    // Composition buffer'i back buffer'a present etmek icin managed texture
    IDirect3DTexture9*      m_pCompositionTexture;

    // 2026-05-16 C1: GetSurfaceLevel(0) cache. Present her frame yeniden
    // alip/release ediyordu — ref-count traffic gereksiz. Bir kez al, texture
    // recreate'lerde (RestoreAll) yenile.
    IDirect3DSurface9*      m_pCompositionTexLevel;

    // Cache: back/primary surface wrapper'lari
    D3D9Surface*            m_pCachedBack;
    D3D9Surface*            m_pCachedPrimary;

    // Gamma state — D3D9'da SetGammaRamp Fullscreen'de hardware, Windowed'da
    // FAIL silent. Phase 1'de Phoenix'in CalculateRamp mantigini birebir tutmak
    // icin orjinal CDirectDraw::m_AddGammaStep + m_GammaStep eslestirilir.
    D3DGAMMARAMP            m_GammaRamp;
    WORD                    m_GammaStep;
    WORD                    m_AddGammaStep[3];

    // 2026-05-16 Dalga 2 — C2: dirty rect tracking. m_HasDirty false ise
    // Present "full surface" sinyali olarak UpdateSurface(NULL, NULL) yapar
    // (backward compat). MarkDirty cagrilari rect'i birlesim olarak expand
    // eder. Frame sonu (Present sonu) m_HasDirty = false reset.
    RECT                    m_DirtyRect;
    bool                    m_HasDirty;
    bool                    m_FullDirty;     // true ise rect ihmal, NULL UpdateSurface

    // Internal helpers
    bool CreateCompositionResources();
    void ReleaseCompositionResources();
    void CalculateGammaRamp();
};

} // namespace render
} // namespace phoenix

// 2026-05-16 Dalga 2 — M1: frame penceresi sorgulama (vs_ui_lib FL2.cpp).
extern "C" bool Phoenix_D3D9_IsFrameInProgress();

#endif // __PHOENIX_D3D9_BACKEND_H__
