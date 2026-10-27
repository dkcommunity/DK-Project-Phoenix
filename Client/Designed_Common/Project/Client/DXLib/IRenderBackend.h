//-----------------------------------------------------------------------------
// IRenderBackend.h
//
// Phoenix D3D9 native render backend arabirimi.
//
// 2026-06-12: D3D9Backend tek aktif implementasyon. CDirectDraw ve
// CDirectDrawSurface eski public isimleri korur, fakat factory D3D9Backend'e
// gider.
//-----------------------------------------------------------------------------

#ifndef __PHOENIX_IRENDER_BACKEND_H__
#define __PHOENIX_IRENDER_BACKEND_H__

#include "RenderTypes.h"

namespace phoenix
{
namespace render
{

class IRenderSurface;

//-----------------------------------------------------------------------------
// IRenderBackend
//
// Render device abstraction. Init/teardown, primary+back buffer access,
// Present (Flip), gamma kontrol, format query.
//-----------------------------------------------------------------------------
class IRenderBackend
{
public:
    virtual ~IRenderBackend() {}

    //-------------------------------------------------------------------------
    // Device lifecycle
    //
    // Init           : device + primary/back surface olustur
    // Shutdown       : tum surface + device release
    // RestoreAll     : lost device + lost surface recovery
    //-------------------------------------------------------------------------
    virtual bool Init(HWND hWnd, WORD width, WORD height, ScreenMode mode, bool use3D, bool useIME) = 0;
    virtual void Shutdown() = 0;
    virtual bool RestoreAll() = 0;

    //-------------------------------------------------------------------------
    // 2026-09-02 O3 — canli cozunurluk degisimi (PhoenixResolutionLive).
    // Device korunur; swapchain/backbuffer + composition + tam-ekran GPU
    // kaynaklari yeni boyutla yeniden kurulur. Basarisizlikta false doner,
    // caller rollback yapar. Default: desteklenmiyor (DDraw7Backend).
    //-------------------------------------------------------------------------
    virtual bool Resize(WORD /*width*/, WORD /*height*/) { return false; }

    //-------------------------------------------------------------------------
    // 2026-09-02 fullscreen fix — canli pencere modu degisimi (Reborn
    // ApplyWindowMode paritesi). D3D11: borderless CDS_FULLSCREEN toggle.
    // Default: desteklenmiyor (D3D9/DDraw7 exclusive akisi restart ister) —
    // caller false gorurse restart-pending yoluna duser.
    //-------------------------------------------------------------------------
    virtual bool SetFullscreenMode(bool /*bFullscreen*/) { return false; }

    //-------------------------------------------------------------------------
// Present. D3D9Backend composition buffer'i back buffer'a tasir ve Present eder.
    //-------------------------------------------------------------------------
    virtual void Present() = 0;
    virtual void FlipToGDISurface() = 0;

    //-------------------------------------------------------------------------
// Surface factory — D3D9Surface olusturur veya mevcut composition wrapper'ini verir.
    // Yeni surface heap'te olusturulur; caller delete sorumludur (ya da
    // CDirectDrawSurface gibi container alir).
    //-------------------------------------------------------------------------
    virtual IRenderSurface* CreateOffscreenSurface(int width, int height, DWORD flags) = 0;
    virtual IRenderSurface* CreateTextureSurface(int width, int height, DWORD flags) = 0;
    virtual IRenderSurface* GetBackBuffer() = 0;
    virtual IRenderSurface* GetPrimarySurface() = 0;

    //-------------------------------------------------------------------------
    // Display state
    //-------------------------------------------------------------------------
    virtual bool IsFullscreen() const = 0;
    virtual WORD GetScreenWidth() const = 0;
    virtual WORD GetScreenHeight() const = 0;
    virtual SurfaceFormat GetPrimaryFormat() const = 0;

    //-------------------------------------------------------------------------
    // Gamma kontrol — DDraw7 IDirectDrawGammaControl muadili. D3D9'da
    // IDirect3DDevice9::SetGammaRamp ile karsilanir.
    //-------------------------------------------------------------------------
    virtual bool IsGammaSupported() const = 0;
    virtual void SetGammaRamp(WORD step) = 0;
    virtual void RestoreGammaRamp() = 0;
    virtual void SetAddGammaRamp(WORD r, WORD g, WORD b) = 0;

    //-------------------------------------------------------------------------
    // Window event hook (DDraw7 OnMove muadili).
    //-------------------------------------------------------------------------
    virtual void OnMove() = 0;
    virtual HWND GetWindow() const = 0;
};

//-----------------------------------------------------------------------------
// Backend factory / accessor — D3D9Backend singleton'i doner.
//-----------------------------------------------------------------------------
IRenderBackend* GetRenderBackend();

//-----------------------------------------------------------------------------
// 2026-09-02 O2 — Reborn Option runtime ayarlari (global; backend her frame
// okur, cagri sirasi/init bagimliligi yok).
//   VSync         : D3D11 swapchain present interval (d3d9/ddraw7: sabit 1, no-op)
//   UserBrightness: 0..100, 50 = notr (D3D11 present modulate carpani)
//   SpriteLinear  : dx11gpu quad pipeline default sampler (nearest/linear)
//   CacheHigh     : sprite texture cache butcesi 256MB -> 512MB
//-----------------------------------------------------------------------------
void SetVSyncEnabled(bool bOn);
bool IsVSyncEnabled();
void SetUserBrightness(int percent);
int  GetUserBrightness();
void SetSpriteLinearFilter(bool bOn);
bool GetSpriteLinearFilter();
void SetSpriteCacheHighBudget(bool bOn);
bool IsSpriteCacheHighBudget();

} // namespace render
} // namespace phoenix

#endif // __PHOENIX_IRENDER_BACKEND_H__
