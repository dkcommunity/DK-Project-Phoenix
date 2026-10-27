//-----------------------------------------------------------------------------
// DDraw7Backend.cpp
//
// Phoenix D3D9 native port — Phase 0 abstraction layer + Phase 1 follow-up.
//
// IRenderBackend + IRenderSurface'in DirectDraw 7 implementasyonu. Mevcut
// CDirectDraw static state'ini WRAP eder. Phase 1'de CDirectDraw'a eklenen
// GetPrimary()/GetBack() accessor'lari kullanilarak surface wrap basitlestirildi.
//
// 2026-05-14 Phase 0 + Phase 1 follow-up
//-----------------------------------------------------------------------------

#include "DDraw7Backend.h"
#include "CDirectDraw.h"
#include "../crash_trace_tool.h"

// Phase 4: compile-time backend selection.
// PHOENIX_RENDER_BACKEND_D3D9=1 ise factory D3D9Backend'i doner.
#if PHOENIX_RENDER_BACKEND_D3D9
#include "D3D9Backend.h"
#endif

namespace phoenix
{
namespace render
{

//-----------------------------------------------------------------------------
// Yardimci: DDPIXELFORMAT -> SurfaceFormat cevirici
//-----------------------------------------------------------------------------
static SurfaceFormat ConvertDDPixelFormat(const DDPIXELFORMAT& ddpf)
{
    if (ddpf.dwRGBBitCount == 16)
    {
        if (ddpf.dwGBitMask == 0x07E0)
            return SurfaceFormat::R5G6B5;        // 5:6:5
        if (ddpf.dwGBitMask == 0x03E0 && (ddpf.dwFlags & DDPF_ALPHAPIXELS))
            return SurfaceFormat::A1R5G5B5;
        if (ddpf.dwGBitMask == 0x03E0)
            return SurfaceFormat::X1R5G5B5;      // 5:5:5
        if (ddpf.dwGBitMask == 0x00F0)
            return SurfaceFormat::A4R4G4B4;
    }
    else if (ddpf.dwRGBBitCount == 32)
    {
        if (ddpf.dwFlags & DDPF_ALPHAPIXELS)
            return SurfaceFormat::A8R8G8B8;
        return SurfaceFormat::X8R8G8B8;
    }

    return SurfaceFormat::Unknown;
}

//-----------------------------------------------------------------------------
// DDraw7Surface
//-----------------------------------------------------------------------------
DDraw7Surface::DDraw7Surface(LPDIRECTDRAWSURFACE7 pSurface, int width, int height, bool owned)
    : m_pDDSurface(pSurface)
    , m_Width(width)
    , m_Height(height)
    , m_ClipLeft(0)
    , m_ClipTop(0)
    , m_ClipRight(width)
    , m_ClipBottom(height)
    , m_ColorKey(0)         // Phase 3: D3D9Surface ile parite
    , m_bHasColorKey(false) // Phase 3: SetTransparency cagrilana kadar false
    , m_bOwned(owned)
    , m_bLocked(false)
{
    ZeroMemory(&m_LockedDesc, sizeof(m_LockedDesc));
}

DDraw7Surface::~DDraw7Surface()
{
    if (m_bOwned && m_pDDSurface != NULL)
    {
        m_pDDSurface->Release();
        m_pDDSurface = NULL;
    }
}

bool DDraw7Surface::Lock(LockedRect& outRect, DWORD flags)
{
    if (!m_pDDSurface)
        return false;

    // 2026-05-14: D3D9 modunda sistem ddraw.dll Lock() cagrisi NVIDIA driver
    // path'inde crash veriyor (Blt/SetColorKey ile ayni sebep). D3D9 modunda
    // DDraw7Surface compat-only — gercek lock D3D9Surface'de yapilir. Bu
    // path sprite load fail eder (return false) ama crash etmez.
#if PHOENIX_RENDER_BACKEND_D3D9
    (void)outRect;
    (void)flags;
    return false;
#endif

    ZeroMemory(&m_LockedDesc, sizeof(m_LockedDesc));
    m_LockedDesc.dwSize = sizeof(m_LockedDesc);

    DWORD ddLockFlags = 0;
    if (flags & LockFlag_Wait)
        ddLockFlags |= DDLOCK_WAIT;
    if (flags & LockFlag_ReadOnly)
        ddLockFlags |= DDLOCK_READONLY;
    // DDraw7'de DISCARD yok — D3D9'da gelecek

    HRESULT hr;
    while (1)
    {
        hr = m_pDDSurface->Lock(NULL, &m_LockedDesc, ddLockFlags, NULL);

        if (hr == DD_OK)
            break;

        if (hr == DDERR_SURFACELOST)
        {
            if (!Restore())
                return false;
            continue;
        }

        if (hr != DDERR_WASSTILLDRAWING)
        {
            static DWORD lastLogTick = 0;
            DWORD now = GetTickCount();
            if (now - lastLogTick > 5000)
            {
                CT_Done("DIAG: DDraw7Surface::Lock FAILED 0x%08X (this:%p)", hr, this);
                lastLogTick = now;
            }
            return false;
        }
    }

    outRect.pBits  = m_LockedDesc.lpSurface;
    outRect.pitch  = m_LockedDesc.lPitch;
    outRect.width  = (int)m_LockedDesc.dwWidth;
    outRect.height = (int)m_LockedDesc.dwHeight;
    outRect.format = ConvertDDPixelFormat(m_LockedDesc.ddpfPixelFormat);

    m_bLocked = true;
    return true;
}

bool DDraw7Surface::Unlock()
{
    if (!m_pDDSurface || !m_bLocked)
        return false;

    // bkz. Lock yorumu — D3D9 modunda Unlock no-op. m_bLocked false set
    // edilir (state'i temizle).
#if PHOENIX_RENDER_BACKEND_D3D9
    m_bLocked = false;
    return true;
#endif

    HRESULT hr;
    while (1)
    {
        hr = m_pDDSurface->Unlock(NULL);
        if (hr == DD_OK || hr == DDERR_NOTLOCKED)
            break;

        if (hr == DDERR_SURFACELOST)
        {
            if (!Restore())
                return false;
            continue;
        }

        // Beklenmedik hata
        return false;
    }

    m_bLocked = false;
    return true;
}

SurfaceFormat DDraw7Surface::GetFormat() const
{
    if (!m_pDDSurface)
        return SurfaceFormat::Unknown;

    DDPIXELFORMAT ddpf;
    ZeroMemory(&ddpf, sizeof(ddpf));
    ddpf.dwSize = sizeof(ddpf);
    if (m_pDDSurface->GetPixelFormat(&ddpf) != DD_OK)
        return SurfaceFormat::Unknown;

    return ConvertDDPixelFormat(ddpf);
}

void DDraw7Surface::SetClip(const RECT& rc)
{
    m_ClipLeft   = rc.left;
    m_ClipTop    = rc.top;
    m_ClipRight  = rc.right;
    m_ClipBottom = rc.bottom;
}

void DDraw7Surface::SetClipNull()
{
    m_ClipLeft   = 0;
    m_ClipTop    = 0;
    m_ClipRight  = m_Width;
    m_ClipBottom = m_Height;
}

void DDraw7Surface::SetTransparency(DWORD colorValue)
{
    // Phase 3 (2026-05-14): state'i her durumda sakla — D3D9Surface ile parite.
    // m_pDDSurface NULL olsa bile (init oncesi cagri vs.) state set edilir,
    // sonradan reset edilen surface'i ayni colorkey ile init etmek mumkun.
    m_ColorKey = colorValue;
    m_bHasColorKey = true;

    if (!m_pDDSurface)
        return;

    // 2026-05-14: D3D9 native port + sistem ddraw.dll NORMAL coop modunda
    // IDirectDrawSurface7::SetColorKey heap corruption tetikliyor (ProfileManager
    // InitProfiles stack CDirectDrawSurface 55x70). State zaten m_ColorKey'de
    // saklandi; sprite codec pixel-level transparency yapiyor, DDraw colorkey
    // sadece eski DDraw Blt path'inde kullaniliyordu (D3D9 modunda inaktif).
    // Skip SetColorKey ile crash bypass; D3D9 path'in colorkey handling
    // D3D9Surface'de zaten dogru.
#if PHOENIX_RENDER_BACKEND_D3D9
    // No-op — state retained for compatibility queries
#else
    DDCOLORKEY ck;
    ck.dwColorSpaceLowValue  = colorValue;
    ck.dwColorSpaceHighValue = colorValue;
    m_pDDSurface->SetColorKey(DDCKEY_SRCBLT, &ck);
#endif
}

void DDraw7Surface::BltColorKey(POINT* pDestPoint, IRenderSurface* pSrc, RECT* pSrcRect)
{
    if (!m_pDDSurface || !pDestPoint || !pSrc || !pSrcRect)
        return;

    // bkz. FillSurface yorumu — D3D9 modunda BltFast no-op (ddraw.dll +
    // NVIDIA driver NORMAL coop'ta crash).
#if PHOENIX_RENDER_BACKEND_D3D9
    return;
#endif

    // Clip — CDirectDrawSurface::Blt mantigi
    if (pDestPoint->x >= m_ClipRight || pDestPoint->y >= m_ClipBottom)
        return;

    if (pDestPoint->x < m_ClipLeft)
    {
        pSrcRect->left += (m_ClipLeft - pDestPoint->x);
        pDestPoint->x = m_ClipLeft;
    }
    else if ((m_ClipRight - pDestPoint->x) < (pSrcRect->right - pSrcRect->left))
    {
        pSrcRect->right = pSrcRect->left + (m_ClipRight - pDestPoint->x);
    }

    if (pDestPoint->y < m_ClipTop)
    {
        pSrcRect->top += (m_ClipTop - pDestPoint->y);
        pDestPoint->y = m_ClipTop;
    }
    else if ((m_ClipBottom - pDestPoint->y) < (pSrcRect->bottom - pSrcRect->top))
    {
        pSrcRect->bottom = pSrcRect->top + (m_ClipBottom - pDestPoint->y);
    }

    DDraw7Surface* pSrcDD = static_cast<DDraw7Surface*>(pSrc);
    LPDIRECTDRAWSURFACE7 srcRaw = pSrcDD->GetRawDDSurface();
    if (!srcRaw)
        return;

    HRESULT hr;
    while (1)
    {
        hr = m_pDDSurface->BltFast((DWORD)pDestPoint->x, (DWORD)pDestPoint->y, srcRaw,
                                   pSrcRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
        if (hr == DD_OK)
            break;

        if (hr == DDERR_SURFACELOST)
        {
            if (!Restore() || !pSrcDD->Restore())
                break;
        }
        else if (hr != DDERR_WASSTILLDRAWING)
        {
            break;
        }
    }
}

void DDraw7Surface::BltNoColorKey(POINT* pDestPoint, IRenderSurface* pSrc, RECT* pSrcRect)
{
    if (!m_pDDSurface || !pDestPoint || !pSrc || !pSrcRect)
        return;

    // bkz. FillSurface yorumu — D3D9 modunda BltFast no-op.
#if PHOENIX_RENDER_BACKEND_D3D9
    return;
#endif

    if (pDestPoint->x >= m_ClipRight || pDestPoint->y >= m_ClipBottom)
        return;

    if (pDestPoint->x < m_ClipLeft)
    {
        pSrcRect->left += (m_ClipLeft - pDestPoint->x);
        pDestPoint->x = m_ClipLeft;
    }
    if ((m_ClipRight - pDestPoint->x) < (pSrcRect->right - pSrcRect->left))
    {
        pSrcRect->right = pSrcRect->left + (m_ClipRight - pDestPoint->x);
    }

    if (pDestPoint->y < m_ClipTop)
    {
        pSrcRect->top += (m_ClipTop - pDestPoint->y);
        pDestPoint->y = m_ClipTop;
    }
    if ((m_ClipBottom - pDestPoint->y) < (pSrcRect->bottom - pSrcRect->top))
    {
        pSrcRect->bottom = pSrcRect->top + (m_ClipBottom - pDestPoint->y);
    }

    DDraw7Surface* pSrcDD = static_cast<DDraw7Surface*>(pSrc);
    LPDIRECTDRAWSURFACE7 srcRaw = pSrcDD->GetRawDDSurface();
    if (!srcRaw)
        return;

    HRESULT hr;
    while (1)
    {
        hr = m_pDDSurface->BltFast((DWORD)pDestPoint->x, (DWORD)pDestPoint->y, srcRaw,
                                   pSrcRect, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
        if (hr == DD_OK)
            break;

        if (hr == DDERR_SURFACELOST)
        {
            if (!Restore() || !pSrcDD->Restore())
                break;
        }
        else if (hr != DDERR_WASSTILLDRAWING)
        {
            break;
        }
    }
}

void DDraw7Surface::BltStretchKey(RECT* pDestRect, IRenderSurface* pSrc, RECT* pSrcRect)
{
    if (!m_pDDSurface || !pDestRect || !pSrc || !pSrcRect)
        return;

    // bkz. FillSurface yorumu — D3D9 modunda Blt no-op.
#if PHOENIX_RENDER_BACKEND_D3D9
    return;
#endif

    DDraw7Surface* pSrcDD = static_cast<DDraw7Surface*>(pSrc);
    LPDIRECTDRAWSURFACE7 srcRaw = pSrcDD->GetRawDDSurface();
    if (!srcRaw)
        return;

    DDBLTFX ddbltfx;
    ZeroMemory(&ddbltfx, sizeof(ddbltfx));
    ddbltfx.dwSize = sizeof(ddbltfx);
    ddbltfx.dwDDFX = DDBLTFX_ARITHSTRETCHY | DDBLT_DDFX;

    HRESULT hr;
    while (1)
    {
        hr = m_pDDSurface->Blt(pDestRect, srcRaw, pSrcRect,
                               DDBLT_WAIT | DDBLT_KEYSRC, &ddbltfx);
        if (hr == DD_OK)
            break;

        if (hr == DDERR_SURFACELOST)
        {
            if (!Restore() || !pSrcDD->Restore())
                break;
        }
        else if (hr != DDERR_WASSTILLDRAWING)
        {
            break;
        }
    }
}

void DDraw7Surface::FillSurface(WORD color)
{
    if (!m_pDDSurface)
        return;

    // 2026-05-14: D3D9 native port + sistem ddraw.dll NORMAL coop modunda
    // IDirectDrawSurface7::Blt cagrisi NVIDIA driver (nvd3dum.dll) icinden
    // gecerken heap corruption tetikliyor. D3D9 modunda gercek render
    // D3D9Backend uzerinden; DDraw7Surface sadece legacy compat (m_pDD
    // query objesi). Blt no-op — gercek fill D3D9Surface'de yapilir.
#if PHOENIX_RENDER_BACKEND_D3D9
    (void)color;
    return;
#else
    DDBLTFX ddbltfx;
    ZeroMemory(&ddbltfx, sizeof(ddbltfx));
    ddbltfx.dwSize = sizeof(DDBLTFX);
    ddbltfx.dwFillColor = color;

    m_pDDSurface->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
#endif
}

void DDraw7Surface::FillRect(RECT* pRect, WORD color)
{
    if (!m_pDDSurface || !pRect)
        return;

#if PHOENIX_RENDER_BACKEND_D3D9
    // bkz. FillSurface yorumu — D3D9 modunda Blt no-op.
    (void)color;
    return;
#else
    RECT rect = *pRect;
    if (rect.left > m_ClipRight)   return;
    if (rect.top  > m_ClipBottom)  return;
    if (rect.left   < m_ClipLeft)   rect.left   = m_ClipLeft;
    if (rect.right  > m_ClipRight)  rect.right  = m_ClipRight;
    if (rect.top    < m_ClipTop)    rect.top    = m_ClipTop;
    if (rect.bottom > m_ClipBottom) rect.bottom = m_ClipBottom;

    DDBLTFX ddbltfx;
    ZeroMemory(&ddbltfx, sizeof(ddbltfx));
    ddbltfx.dwSize = sizeof(ddbltfx);
    ddbltfx.dwFillColor = color;

    m_pDDSurface->Blt(&rect, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &ddbltfx);
#endif
}

bool DDraw7Surface::Restore()
{
    if (!m_pDDSurface)
        return false;

    // Primary'i de restore et — CDirectDrawSurface::Restore mantigi
    CDirectDraw::RestoreAllSurfaces();

    for (int i = 0; i < 100; ++i)
    {
        if (m_pDDSurface->Restore() == DD_OK)
            return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// DDraw7Backend
//-----------------------------------------------------------------------------
DDraw7Backend::DDraw7Backend()
    : m_pCachedBack(NULL)
    , m_pCachedPrimary(NULL)
{
}

DDraw7Backend::~DDraw7Backend()
{
    Shutdown();
}

DDraw7Backend& DDraw7Backend::Instance()
{
    static DDraw7Backend s_Instance;
    return s_Instance;
}

bool DDraw7Backend::Init(HWND hWnd, WORD width, WORD height, ScreenMode mode, bool use3D, bool useIME)
{
    // CDirectDraw zaten static singleton. Phase 0'da double-init etmiyoruz —
    // mevcut CDirectDraw::Init zinciri akliyor.
    CDirectDraw::SCREENMODE ddMode = (mode == ScreenMode::Fullscreen)
                                          ? CDirectDraw::FULLSCREEN
                                          : CDirectDraw::WINDOWMODE;
    return CDirectDraw::Init(hWnd, width, height, ddMode, use3D, useIME);
}

void DDraw7Backend::Shutdown()
{
    delete m_pCachedBack;
    m_pCachedBack = NULL;
    delete m_pCachedPrimary;
    m_pCachedPrimary = NULL;

    CDirectDraw::ReleaseAll();
}

bool DDraw7Backend::RestoreAll()
{
    return CDirectDraw::RestoreAllSurfaces();
}

void DDraw7Backend::Present()
{
    CDirectDraw::Flip();
}

void DDraw7Backend::FlipToGDISurface()
{
    CDirectDraw::FlipToGDISurface();
}

IRenderSurface* DDraw7Backend::CreateOffscreenSurface(int width, int height, DWORD flags)
{
    LPDIRECTDRAW7 pDD = CDirectDraw::GetDD();
    if (!pDD)
        return NULL;

    DDSURFACEDESC2 ddsd;
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.dwWidth  = (DWORD)width;
    ddsd.dwHeight = (DWORD)height;

    DWORD ddCaps = DDSCAPS_OFFSCREENPLAIN;
    if (flags & SurfaceCreate_VideoMemory)
        ddCaps |= DDSCAPS_VIDEOMEMORY;
    if (flags & SurfaceCreate_SystemMemory)
        ddCaps |= DDSCAPS_SYSTEMMEMORY;
    if (flags & SurfaceCreate_3DDevice)
        ddCaps |= DDSCAPS_3DDEVICE;
    ddsd.ddsCaps.dwCaps = ddCaps;

    LPDIRECTDRAWSURFACE7 pRaw = NULL;
    if (pDD->CreateSurface(&ddsd, &pRaw, NULL) != DD_OK)
        return NULL;

    return new DDraw7Surface(pRaw, width, height, /*owned=*/true);
}

IRenderSurface* DDraw7Backend::CreateTextureSurface(int width, int height, DWORD flags)
{
    LPDIRECTDRAW7 pDD = CDirectDraw::GetDD();
    if (!pDD)
        return NULL;

    DDSURFACEDESC2 ddsd;
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.dwWidth  = (DWORD)width;
    ddsd.dwHeight = (DWORD)height;

    DWORD ddCaps = DDSCAPS_TEXTURE;
    if (flags & SurfaceCreate_VideoMemory)
        ddCaps |= DDSCAPS_VIDEOMEMORY;
    if (flags & SurfaceCreate_SystemMemory)
        ddCaps |= DDSCAPS_SYSTEMMEMORY;
    ddsd.ddsCaps.dwCaps  = ddCaps;
    ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;

    LPDIRECTDRAWSURFACE7 pRaw = NULL;
    if (pDD->CreateSurface(&ddsd, &pRaw, NULL) != DD_OK)
        return NULL;

    return new DDraw7Surface(pRaw, width, height, /*owned=*/true);
}

IRenderSurface* DDraw7Backend::GetBackBuffer()
{
    // Phase 1 follow-up: CDirectDraw::GetBack() accessor mevcut, dogrudan
    // raw LPDIRECTDRAWSURFACE7 alip wrap'liyoruz (önceki temp-container yolu
    // gereksiz fazlaliklik).
    if (!m_pCachedBack && CDirectDraw::GetDD())
    {
        LPDIRECTDRAWSURFACE7 pRaw = CDirectDraw::GetBack();
        if (pRaw)
        {
            m_pCachedBack = new DDraw7Surface(pRaw,
                                              CDirectDraw::GetScreenWidth(),
                                              CDirectDraw::GetScreenHeight(),
                                              /*owned=*/false);  // CDirectDraw release sorumlu
        }
    }
    return m_pCachedBack;
}

IRenderSurface* DDraw7Backend::GetPrimarySurface()
{
    // Phase 1 follow-up: CDirectDraw::GetPrimary() accessor eklendi, primary
    // surface artik wrap edilebiliyor. Caller'lar (FlipToGDISurface, gamma
    // path'ler) Phase 1'de raw LPDIRECTDRAWSURFACE7 yerine IRenderSurface
    // alabilir.
    if (!m_pCachedPrimary && CDirectDraw::GetDD())
    {
        LPDIRECTDRAWSURFACE7 pRaw = CDirectDraw::GetPrimary();
        if (pRaw)
        {
            m_pCachedPrimary = new DDraw7Surface(pRaw,
                                                 CDirectDraw::GetScreenWidth(),
                                                 CDirectDraw::GetScreenHeight(),
                                                 /*owned=*/false);  // CDirectDraw release sorumlu
        }
    }
    return m_pCachedPrimary;
}

bool DDraw7Backend::IsFullscreen() const
{
    return CDirectDraw::IsFullscreen();
}

WORD DDraw7Backend::GetScreenWidth() const
{
    return CDirectDraw::GetScreenWidth();
}

WORD DDraw7Backend::GetScreenHeight() const
{
    return CDirectDraw::GetScreenHeight();
}

SurfaceFormat DDraw7Backend::GetPrimaryFormat() const
{
    return CDirectDraw::Is565() ? SurfaceFormat::R5G6B5 : SurfaceFormat::X1R5G5B5;
}

bool DDraw7Backend::IsGammaSupported() const
{
    return CDirectDraw::IsSupportGammaControl();
}

void DDraw7Backend::SetGammaRamp(WORD step)
{
    CDirectDraw::SetGammaRamp(step);
}

void DDraw7Backend::RestoreGammaRamp()
{
    CDirectDraw::RestoreGammaRamp();
}

void DDraw7Backend::SetAddGammaRamp(WORD r, WORD g, WORD b)
{
    CDirectDraw::SetAddGammaRamp(r, g, b);
}

void DDraw7Backend::OnMove()
{
    CDirectDraw::OnMove();
}

HWND DDraw7Backend::GetWindow() const
{
    return CDirectDraw::GetHwnd();
}

//-----------------------------------------------------------------------------
// Factory accessor
//
// Phase 4: PHOENIX_RENDER_BACKEND_D3D9 compile-time flag'e gore backend
// secimi yapilir.
//   - 0 (default): DDraw7Backend (legacy, dgVoodoo wrapper kullanir)
//   - 1: D3D9Backend (Phoenix native D3D9, magenta sprite cozuldu)
//
// Flag CMake'den (PHOENIX_USE_D3D9_BACKEND option) veya manuel define ile
// set edilir. Default 0 — kullanici opt-in yapana kadar mevcut DDraw7 path
// canli (regression 0 hedefi).
//-----------------------------------------------------------------------------
IRenderBackend* GetRenderBackend()
{
#if PHOENIX_RENDER_BACKEND_D3D9
    return &D3D9Backend::Instance();
#else
    return &DDraw7Backend::Instance();
#endif
}

} // namespace render
} // namespace phoenix
