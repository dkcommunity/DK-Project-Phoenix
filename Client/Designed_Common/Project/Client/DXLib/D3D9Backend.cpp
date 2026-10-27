//-----------------------------------------------------------------------------
// D3D9Backend.cpp
//
// Phoenix D3D9 native port — Phase 1 implementasyonu.
//
// IRenderBackend + IRenderSurface'in Direct3D 9 implementasyonu. 16-bit
// composition buffer pattern ile sprite codec dokunulmadan calisir.
//
// 2026-05-14 Phase 1
//-----------------------------------------------------------------------------

#include "D3D9Backend.h"
#include "D3D11Backend.h"     // 2026-08-31 DX11 P0 — GetRenderBackend factory secimi
#include "../crash_trace_tool.h"
#include "../frame_perf_probe.h"
#include "D3D9PostProcess.h"  // Phase 7 (2026-05-14) — vignette post-process
#include "../UserOption.h"    // 2026-05-16 — Vignette runtime toggle
#include <limits.h>           // 2026-05-16 Dalga 2 — INT_MIN/INT_MAX (dirty rect init)
// 2026-05-18 ImGui Hibrit Admin Menu — Initialize/Render/OnDeviceReset entry points
#include "../Debug/PhoenixImGui.h"

namespace phoenix
{
namespace render
{

//-----------------------------------------------------------------------------
// Yardimci: 16-bit R5G6B5 -> 32-bit A8R8G8B8 pixel cevirici. FillSurface/
// FillRect 32-bit hedef icin kullanilir (32-bit back buffer paths Phase 4+).
//-----------------------------------------------------------------------------
static inline DWORD Expand565To8888(WORD pixel)
{
    DWORD r = ((pixel >> 11) & 0x1F);
    DWORD g = ((pixel >> 5)  & 0x3F);
    DWORD b = ( pixel        & 0x1F);
    // 5/6 bit'i 8 bit'e dogru genislet (en yaygin formul: shift + replicate)
    r = (r << 3) | (r >> 2);
    g = (g << 2) | (g >> 4);
    b = (b << 3) | (b >> 2);
    return 0xFF000000 | (r << 16) | (g << 8) | b;
}

//-----------------------------------------------------------------------------
// D3D9Surface
//-----------------------------------------------------------------------------
D3D9Surface::D3D9Surface(IDirect3DSurface9* pSurface, int width, int height,
                         SurfaceFormat format, bool owned)
    : m_pSurface(pSurface)
    , m_Width(width)
    , m_Height(height)
    , m_Format(format)
    , m_ClipLeft(0)
    , m_ClipTop(0)
    , m_ClipRight(width)
    , m_ClipBottom(height)
    , m_ColorKey(0)
    , m_bHasColorKey(false)
    , m_bOwned(owned)
    , m_bLocked(false)
    , m_LockCount(0)
{
    ZeroMemory(&m_CachedLock, sizeof(m_CachedLock));
}

D3D9Surface::~D3D9Surface()
{
    if (m_bOwned && m_pSurface != NULL)
    {
        m_pSurface->Release();
        m_pSurface = NULL;
    }
}

//-----------------------------------------------------------------------------
// 2026-09-02 crash fix (D3D11 paritesi) — composition singleton wrapper'i
// Resize/Reset dongusunde delete/new edilmez; alttaki COM surface degisince
// ayni wrapper objesi yeni surface'a yeniden baglanir. "Yol 1" wrapper'lari
// (g_pBack, g_pLast, VS_UI Base) bu pointer'i sakladigi icin obje kimligi
// korunmak zorunda.
//-----------------------------------------------------------------------------
void D3D9Surface::Rebind(IDirect3DSurface9* pSurface, int width, int height)
{
    if (m_bOwned && m_pSurface != NULL)
        m_pSurface->Release();

    m_pSurface   = pSurface;
    m_Width      = width;
    m_Height     = height;
    m_ClipLeft   = 0;
    m_ClipTop    = 0;
    m_ClipRight  = width;
    m_ClipBottom = height;
    m_bLocked    = false;
    m_LockCount  = 0;
    ZeroMemory(&m_CachedLock, sizeof(m_CachedLock));
}

bool D3D9Surface::Lock(LockedRect& outRect, DWORD flags)
{
    // Fix #12 diagnostic (2026-05-14): entry trace.
    CT_Done("DIAG-12: D3D9Surface::Lock ENTER this=%p m_pSurface=%p count=%d",
            this, m_pSurface, m_LockCount);

    if (!m_pSurface)
        return false;

    // 2026-05-14 Fix #11: Re-entrant Lock. Composition buffer singleton'i
    // birden fazla CDirectDrawSurface tarafindan paylasiliyor (g_pBack +
    // g_pLast); BltColorKey de src+dst ayni surface ise iki kez Lock cagrir.
    // SYSTEMMEM surface D3D9'da re-entrant LockRect destegi sunmaz — ikinci
    // LockRect undefined behavior → driver heap corruption. Cached pBits/
    // pitch geri donduruyoruz; refcount 0'a indiginde gercek UnlockRect.
    if (m_LockCount > 0)
    {
        outRect.pBits  = m_CachedLock.pBits;
        outRect.pitch  = m_CachedLock.Pitch;
        outRect.width  = m_Width;
        outRect.height = m_Height;
        outRect.format = m_Format;
        ++m_LockCount;
        CT_Done("DIAG-12: D3D9Surface::Lock CACHED-RETURN count now=%d pBits=%p pitch=%d",
                m_LockCount, outRect.pBits, outRect.pitch);
        return true;
    }

    DWORD d3dFlags = 0;
    if (flags & LockFlag_ReadOnly)
        d3dFlags |= D3DLOCK_READONLY;
    if (flags & LockFlag_Discard)
        d3dFlags |= D3DLOCK_DISCARD;
    // D3D9'da WAIT default davranis — DDLOCK_WAIT karsiligi yok (her zaman bloklar)

    CT_Done("DIAG-12: D3D9Surface::Lock calling LockRect d3dFlags=0x%X", d3dFlags);
    HRESULT hr = m_pSurface->LockRect(&m_CachedLock, NULL, d3dFlags);
    CT_Done("DIAG-12: D3D9Surface::Lock LockRect returned hr=0x%08X pBits=%p Pitch=%d",
            hr, m_CachedLock.pBits, m_CachedLock.Pitch);

    if (FAILED(hr))
    {
        // D3DERR_DEVICELOST veya benzeri durumda 1 kere restore + retry
        if (hr == D3DERR_DEVICELOST || hr == D3DERR_DEVICEHUNG)
        {
            if (Restore())
            {
                hr = m_pSurface->LockRect(&m_CachedLock, NULL, d3dFlags);
            }
        }
        if (FAILED(hr))
        {
            static DWORD lastLogTick = 0;
            DWORD now = GetTickCount();
            if (now - lastLogTick > 5000)
            {
                CT_Done("DIAG: D3D9Surface::Lock FAILED 0x%08X (this:%p)", hr, this);
                lastLogTick = now;
            }
            ZeroMemory(&m_CachedLock, sizeof(m_CachedLock));
            return false;
        }
    }

    // Sprite codec kontrati: pBits + pitch (BYTE cinsinden)
    outRect.pBits  = m_CachedLock.pBits;
    outRect.pitch  = m_CachedLock.Pitch;
    outRect.width  = m_Width;
    outRect.height = m_Height;
    outRect.format = m_Format;

    m_bLocked = true;
    m_LockCount = 1;
    CT_Done("DIAG-12: D3D9Surface::Lock SUCCESS RETURN pBits=%p pitch=%d w=%d h=%d",
            outRect.pBits, outRect.pitch, outRect.width, outRect.height);
    return true;
}

bool D3D9Surface::Unlock()
{
    if (!m_pSurface)
        return false;

    // 2026-05-14 Fix #11: Ref-counted Unlock. Sadece son Unlock cagrisinda
    // UnlockRect cagirilir (kalan caller'lar surface'i lock'lu tutar).
    if (m_LockCount <= 0)
        return false;

    --m_LockCount;
    if (m_LockCount > 0)
        return true;  // Hala bir baska caller tutuyor

    HRESULT hr = m_pSurface->UnlockRect();
    m_bLocked = false;
    ZeroMemory(&m_CachedLock, sizeof(m_CachedLock));
    return SUCCEEDED(hr);
}

void D3D9Surface::SetClip(const RECT& rc)
{
    m_ClipLeft   = rc.left;
    m_ClipTop    = rc.top;
    m_ClipRight  = rc.right;
    m_ClipBottom = rc.bottom;
}

void D3D9Surface::SetClipNull()
{
    m_ClipLeft   = 0;
    m_ClipTop    = 0;
    m_ClipRight  = m_Width;
    m_ClipBottom = m_Height;
}

void D3D9Surface::SetTransparency(DWORD colorValue)
{
    // D3D9'da surface-level SetColorKey YOK. ColorKey'i state'te saklayip
    // BltColorKey/BltStretchKey path'lerinde manuel filtre uygulayacagiz
    // (Phase 4'te ID3DXSprite + D3DRS_ALPHATESTENABLE'a tasinabilir).
    m_ColorKey = colorValue;
    m_bHasColorKey = true;
}

//-----------------------------------------------------------------------------
// BltColorKey
//
// DDraw7 BltFast(DDBLTFAST_SRCCOLORKEY) muadili. D3D9'da surface-level color
// key olmadigi icin manuel pixel filtre yapiyoruz (sysmem -> sysmem). Bu
// path 16-bit R5G6B5 surfaces icin optimize edilmis; baska formatlar icin
// performans dusurucu fallback.
//-----------------------------------------------------------------------------
void D3D9Surface::BltColorKey(POINT* pDestPoint, IRenderSurface* pSrc, RECT* pSrcRect)
{
    if (!m_pSurface || !pDestPoint || !pSrc || !pSrcRect)
        return;

    // Clip — DDraw7Surface ile ayni mantik
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

    D3D9Surface* pSrcD3D = static_cast<D3D9Surface*>(pSrc);

    // 2026-05-15: D3D9 native port pattern — DDraw7 BltFast hardware kirpiyordu,
    // manuel sysmem bounds clamp. Round 3 sweep (BltNoColorKey ile pattern parite).
    // Source rect SOURCE surface bounds disinda ise pS[x] (line ~263 inner loop)
    // OOB pointer dereference -> AV. Sprite atlas blit kritik yol (zone enter +
    // her frame sprite render); rect dogrulanmasi zorunlu.
    if (pSrcRect->left < 0) pSrcRect->left = 0;
    if (pSrcRect->top  < 0) pSrcRect->top  = 0;
    if (pSrcRect->right  > pSrcD3D->GetWidth())  pSrcRect->right  = pSrcD3D->GetWidth();
    if (pSrcRect->bottom > pSrcD3D->GetHeight()) pSrcRect->bottom = pSrcD3D->GetHeight();
    if (pSrcRect->right <= pSrcRect->left || pSrcRect->bottom <= pSrcRect->top)
        return;

    // Dest point negative veya surface boyutu disi defansif (clip set yapildi
    // ama caller m_ClipLeft/Top'i runtime degistirebilir).
    if (pDestPoint->x < 0 || pDestPoint->y < 0)
        return;
    if (pDestPoint->x >= m_Width || pDestPoint->y >= m_Height)
        return;

    // Dest tarafinda copy genisligi/yuksekligi surface boyutu disi kalmasin
    int maxCopyW = m_Width  - pDestPoint->x;
    int maxCopyH = m_Height - pDestPoint->y;
    if ((pSrcRect->right - pSrcRect->left) > maxCopyW)
        pSrcRect->right = pSrcRect->left + maxCopyW;
    if ((pSrcRect->bottom - pSrcRect->top) > maxCopyH)
        pSrcRect->bottom = pSrcRect->top + maxCopyH;
    if (pSrcRect->right <= pSrcRect->left || pSrcRect->bottom <= pSrcRect->top)
        return;

    LockedRect srcLock, dstLock;
    if (!pSrcD3D->Lock(srcLock, LockFlag_ReadOnly))
        return;
    if (!Lock(dstLock, LockFlag_None))
    {
        pSrcD3D->Unlock();
        return;
    }

    // 2026-05-15: D3D9 native port pattern — Lock sonrasi pBits/pitch sanity.
    // Lock TRUE donmus olabilir ama backend driver edge case'inde pBits NULL
    // veya pitch <= 0 olabilir; pS[x] / pD[x] dereferansi guaranteed AV.
    if (!srcLock.pBits || !dstLock.pBits || srcLock.pitch <= 0 || dstLock.pitch <= 0)
    {
        Unlock();
        pSrcD3D->Unlock();
        return;
    }

    const int copyW = pSrcRect->right  - pSrcRect->left;
    const int copyH = pSrcRect->bottom - pSrcRect->top;
    // Phase 3 (2026-05-14): IRenderSurface contract — GetColorKey + HasColorKey.
    // Eski GetTransparency / HasTransparency D3D9-spesifik isimlerden taşındı.
    const WORD colorKey = (WORD)(pSrcD3D->HasColorKey()
                                  ? pSrcD3D->GetColorKey()
                                  : m_ColorKey);

    // 16-bit fast path — Phoenix sprite atlas'lari hep R5G6B5
    if (m_Format == SurfaceFormat::R5G6B5 && pSrcD3D->GetFormat() == SurfaceFormat::R5G6B5)
    {
        const BYTE* pSrcBase = (const BYTE*)srcLock.pBits
                              + pSrcRect->top * srcLock.pitch
                              + pSrcRect->left * 2;
        BYTE* pDstBase = (BYTE*)dstLock.pBits
                       + pDestPoint->y * dstLock.pitch
                       + pDestPoint->x * 2;

        for (int y = 0; y < copyH; ++y)
        {
            const WORD* pS = (const WORD*)(pSrcBase + y * srcLock.pitch);
            WORD*       pD = (WORD*)      (pDstBase + y * dstLock.pitch);
            for (int x = 0; x < copyW; ++x)
            {
                WORD c = pS[x];
                if (c != colorKey)
                    pD[x] = c;
            }
        }
    }
    // Diger format kombinasyonlari Phase 1'de YOK — Phoenix sprite codec
    // 16-bit'e bagli. Phase 4'te 32-bit surfaces eklenirse buraya genisletilir.

    Unlock();
    pSrcD3D->Unlock();
}

//-----------------------------------------------------------------------------
// BltNoColorKey
//
// DDraw7 BltFast(DDBLTFAST_NOCOLORKEY) muadili — opaque blit. D3D9'da
// IDirect3DDevice9::UpdateSurface (sysmem -> sysmem) veya manuel memcpy.
//-----------------------------------------------------------------------------
void D3D9Surface::BltNoColorKey(POINT* pDestPoint, IRenderSurface* pSrc, RECT* pSrcRect)
{
    if (!m_pSurface || !pDestPoint || !pSrc || !pSrcRect)
        return;

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

    D3D9Surface* pSrcD3D = static_cast<D3D9Surface*>(pSrc);

    // 2026-05-15 round 2: source rect bounds validation. DDraw7 BltFast
    // hardware path donanim driver tarafindan kirpiliyordu (illegal rect →
    // DDERR_INVALIDRECT). D3D9 native port'ta sysmem -> sysmem manuel memcpy
    // kullaniyoruz; rect dogrulanmazsa pSrcBase/pDstBase OOB pointer alir ve
    // memmove vcruntime'da AV firlatir.
    //
    // Crash imzasi: vcruntime140.dll!memcpy/memmove fault, call chain
    // CDirectDrawSurface::BltNoColorkey → IRenderSurface::BltNoColorKey
    // (bu fonksiyon) → memmove. Zone enter ilk frame'inde rectReuse
    // (MTopView.cpp:14704) m_FirstZonePixel / m_TileSurfaceFirstZonePixel
    // sync olmadan negative/oversize uretebilir; veya cloud surface
    // (EVENTBACKGROUNDID_CLOUD) gercek boyutu SURFACE_WIDTH x SURFACE_HEIGHT
    // (g_pUserInformation resolution) ile uyumsuz ise CloudRect
    // (MTopView.cpp:15730) source bounds dis.
    if (pSrcRect->left < 0) pSrcRect->left = 0;
    if (pSrcRect->top  < 0) pSrcRect->top  = 0;
    if (pSrcRect->right  > pSrcD3D->GetWidth())  pSrcRect->right  = pSrcD3D->GetWidth();
    if (pSrcRect->bottom > pSrcD3D->GetHeight()) pSrcRect->bottom = pSrcD3D->GetHeight();
    if (pSrcRect->right <= pSrcRect->left || pSrcRect->bottom <= pSrcRect->top)
        return;

    // Dest point bounds (clip yukarida set edildi ama negative ya da
    // m_Width/m_Height taşan deger guvende olmali).
    if (pDestPoint->x < 0 || pDestPoint->y < 0)
        return;
    if (pDestPoint->x >= m_Width || pDestPoint->y >= m_Height)
        return;

    // Dest tarafinda copy genisligi/yuksekligi surface boyutu disi kalmasin
    int maxCopyW = m_Width  - pDestPoint->x;
    int maxCopyH = m_Height - pDestPoint->y;
    if ((pSrcRect->right - pSrcRect->left) > maxCopyW)
        pSrcRect->right = pSrcRect->left + maxCopyW;
    if ((pSrcRect->bottom - pSrcRect->top) > maxCopyH)
        pSrcRect->bottom = pSrcRect->top + maxCopyH;
    if (pSrcRect->right <= pSrcRect->left || pSrcRect->bottom <= pSrcRect->top)
        return;

    LockedRect srcLock, dstLock;
    if (!pSrcD3D->Lock(srcLock, LockFlag_ReadOnly))
        return;
    if (!Lock(dstLock, LockFlag_None))
    {
        pSrcD3D->Unlock();
        return;
    }

    // 2026-05-15 round 2: Lock sonrasi pBits / pitch sanity. Lock TRUE
    // donmus olabilir ama backend driver bug edge case'inde pBits NULL
    // veya pitch <= 0 olabilir; bu durumda memmove guaranteed AV.
    if (!srcLock.pBits || !dstLock.pBits || srcLock.pitch <= 0 || dstLock.pitch <= 0)
    {
        Unlock();
        pSrcD3D->Unlock();
        return;
    }

    const int copyW = pSrcRect->right  - pSrcRect->left;
    const int copyH = pSrcRect->bottom - pSrcRect->top;

    // 16-bit memmove fast path
    if (m_Format == SurfaceFormat::R5G6B5 && pSrcD3D->GetFormat() == SurfaceFormat::R5G6B5)
    {
        const BYTE* pSrcBase = (const BYTE*)srcLock.pBits
                              + pSrcRect->top * srcLock.pitch
                              + pSrcRect->left * 2;
        BYTE* pDstBase = (BYTE*)dstLock.pBits
                       + pDestPoint->y * dstLock.pitch
                       + pDestPoint->x * 2;

        const int bytesPerRow = copyW * 2;

        // Fix #23 (2026-05-15): self-blit scroll overlap-aware. memcpy UB
        // overlapping regions icin (Phoenix m_pTileSurface scroll on movement
        // = self-blit BltNoColorkey). memmove intra-row overlap'i handle eder;
        // inter-row icin direction-aware iteration (dst memory dst_base >
        // src_base ise BACKWARD; aksi halde forward).
        //
        // Tile flickering belirti: oyuncu hareketinde tile'lar bir frame
        // kayboldu — memcpy forward iteration scroll-down/right'ta yazilan
        // dst pikselleri sonraki iterasyonda source olarak okuyup corrupt
        // ediyordu.
        if (pDstBase > pSrcBase)
        {
            // Dst memory daha sonraki adreste → backward row iteration
            for (int y = copyH - 1; y >= 0; --y)
            {
                memmove(pDstBase + y * dstLock.pitch,
                        pSrcBase + y * srcLock.pitch,
                        bytesPerRow);
            }
        }
        else
        {
            // Dst memory daha onceki adreste veya farkli buffer → forward
            for (int y = 0; y < copyH; ++y)
            {
                memmove(pDstBase + y * dstLock.pitch,
                        pSrcBase + y * srcLock.pitch,
                        bytesPerRow);
            }
        }
    }

    Unlock();
    pSrcD3D->Unlock();
}

//-----------------------------------------------------------------------------
// BltStretchKey
//
// DDraw7 Blt(DDBLT_KEYSRC + DDBLTFX_ARITHSTRETCHY) muadili. Nearest-neighbor
// stretch + colorkey. Phoenix'te tek call site: tile colorkey blit
// (CDirectDrawSurface.cpp:1287). Phase 1'de yine sysmem -> sysmem manuel
// pixel kopya — performans kritik degil (frame basina ~1-2 kez).
//-----------------------------------------------------------------------------
void D3D9Surface::BltStretchKey(RECT* pDestRect, IRenderSurface* pSrc, RECT* pSrcRect)
{
    if (!m_pSurface || !pDestRect || !pSrc || !pSrcRect)
        return;

    D3D9Surface* pSrcD3D = static_cast<D3D9Surface*>(pSrc);

    // 2026-05-15: D3D9 native port pattern — DDraw7 Blt hardware driver source +
    // dest rect bounds kirpiyordu (DDERR_INVALIDRECT). Round 3 sweep
    // (BltNoColorKey ile parite). Stretch loop pS[sx] / pD[pDestRect->left+dx]
    // hesabi rect dogrulanmadan OOB pointer üretebilir; özellikle tile colorkey
    // blit (CDirectDrawSurface.cpp:1287 tek call site) Phoenix'te zone enter
    // sirasinda farkli koordinatlarla cagrilabilir.
    if (pSrcRect->left < 0) pSrcRect->left = 0;
    if (pSrcRect->top  < 0) pSrcRect->top  = 0;
    if (pSrcRect->right  > pSrcD3D->GetWidth())  pSrcRect->right  = pSrcD3D->GetWidth();
    if (pSrcRect->bottom > pSrcD3D->GetHeight()) pSrcRect->bottom = pSrcD3D->GetHeight();
    if (pSrcRect->right <= pSrcRect->left || pSrcRect->bottom <= pSrcRect->top)
        return;

    // Dest rect bounds — clip rect mevcut state'i ihmal etmiyoruz ama negative
    // veya surface boyut disi degerleri red et + clamp et.
    if (pDestRect->left < 0) pDestRect->left = 0;
    if (pDestRect->top  < 0) pDestRect->top  = 0;
    if (pDestRect->right  > m_Width)  pDestRect->right  = m_Width;
    if (pDestRect->bottom > m_Height) pDestRect->bottom = m_Height;
    if (pDestRect->right <= pDestRect->left || pDestRect->bottom <= pDestRect->top)
        return;

    LockedRect srcLock, dstLock;
    if (!pSrcD3D->Lock(srcLock, LockFlag_ReadOnly))
        return;
    if (!Lock(dstLock, LockFlag_None))
    {
        pSrcD3D->Unlock();
        return;
    }

    // 2026-05-15: D3D9 native port pattern — Lock sonrasi pBits/pitch sanity.
    if (!srcLock.pBits || !dstLock.pBits || srcLock.pitch <= 0 || dstLock.pitch <= 0)
    {
        Unlock();
        pSrcD3D->Unlock();
        return;
    }

    const int srcW = pSrcRect->right  - pSrcRect->left;
    const int srcH = pSrcRect->bottom - pSrcRect->top;
    const int dstW = pDestRect->right  - pDestRect->left;
    const int dstH = pDestRect->bottom - pDestRect->top;

    // Phase 3 (2026-05-14): IRenderSurface contract — GetColorKey + HasColorKey.
    const WORD colorKey = (WORD)(pSrcD3D->HasColorKey()
                                  ? pSrcD3D->GetColorKey()
                                  : m_ColorKey);

    if (dstW <= 0 || dstH <= 0 || srcW <= 0 || srcH <= 0)
    {
        Unlock();
        pSrcD3D->Unlock();
        return;
    }

    if (m_Format == SurfaceFormat::R5G6B5 && pSrcD3D->GetFormat() == SurfaceFormat::R5G6B5)
    {
        // Nearest-neighbor stretch + colorkey
        for (int dy = 0; dy < dstH; ++dy)
        {
            const int sy = pSrcRect->top + (dy * srcH) / dstH;
            const WORD* pS = (const WORD*)((const BYTE*)srcLock.pBits + sy * srcLock.pitch);
            WORD*       pD = (WORD*)      ((BYTE*)      dstLock.pBits + (pDestRect->top + dy) * dstLock.pitch);
            for (int dx = 0; dx < dstW; ++dx)
            {
                const int sx = pSrcRect->left + (dx * srcW) / dstW;
                WORD c = pS[sx];
                if (c != colorKey)
                    pD[pDestRect->left + dx] = c;
            }
        }
    }

    Unlock();
    pSrcD3D->Unlock();
}

void D3D9Surface::FillSurface(WORD color)
{
    // Fix #12 diagnostic (2026-05-14): FillSurface c0000374 crash izleme.
    CT_Done("DIAG-12: D3D9Surface::FillSurface ENTER this=%p m_pSurface=%p m_W=%d m_H=%d count=%d",
            this, m_pSurface, m_Width, m_Height, m_LockCount);

    if (!m_pSurface)
    {
        CT_Done("DIAG-12: D3D9Surface::FillSurface EARLY-EXIT m_pSurface NULL");
        return;
    }

    LockedRect rect;
    if (!Lock(rect, LockFlag_None))
    {
        CT_Done("DIAG-12: D3D9Surface::FillSurface Lock FAILED");
        return;
    }

    CT_Done("DIAG-12: D3D9Surface::FillSurface Lock OK pBits=%p pitch=%d (expect>=%d)",
            rect.pBits, rect.pitch, m_Width*2);

    // 2026-05-15: D3D9 native port pattern — DDraw7 BltFast hardware kirpiyordu,
    // manuel sysmem bounds clamp. Round 3 sweep (BltNoColorKey ile parite).
    // Lock TRUE donmus olabilir ama backend driver edge case'inde pBits NULL
    // veya pitch <= 0 olabilir; pRow[x] iteration sirasinda guaranteed AV.
    // m_Width / m_Height sanity (negative veya 0) — surface zero-sized ise
    // hicbir fill yapilamaz; ayrica pitch >= m_Width * bpp olmali (driver
    // garanti eder ama defansif).
    if (!rect.pBits || rect.pitch <= 0 || m_Width <= 0 || m_Height <= 0)
    {
        CT_Done("DIAG-12: D3D9Surface::FillSurface SANITY FAIL pBits=%p pitch=%d w=%d h=%d",
                rect.pBits, rect.pitch, m_Width, m_Height);
        Unlock();
        return;
    }

    if (m_Format == SurfaceFormat::R5G6B5 ||
        m_Format == SurfaceFormat::X1R5G5B5 ||
        m_Format == SurfaceFormat::A1R5G5B5 ||
        m_Format == SurfaceFormat::A4R4G4B4)
    {
        // pitch byte cinsinden; her satirda en fazla pitch/2 WORD yazilabilir
        const int maxX = rect.pitch / 2;
        const int writeW = (m_Width < maxX) ? m_Width : maxX;
        for (int y = 0; y < m_Height; ++y)
        {
            WORD* pRow = (WORD*)((BYTE*)rect.pBits + y * rect.pitch);
            for (int x = 0; x < writeW; ++x)
                pRow[x] = color;
        }
    }
    else if (m_Format == SurfaceFormat::X8R8G8B8 || m_Format == SurfaceFormat::A8R8G8B8)
    {
        const DWORD c32 = Expand565To8888(color);
        const int maxX = rect.pitch / 4;
        const int writeW = (m_Width < maxX) ? m_Width : maxX;
        for (int y = 0; y < m_Height; ++y)
        {
            DWORD* pRow = (DWORD*)((BYTE*)rect.pBits + y * rect.pitch);
            for (int x = 0; x < writeW; ++x)
                pRow[x] = c32;
        }
    }

    CT_Done("DIAG-12: D3D9Surface::FillSurface fill loop done, calling Unlock");
    Unlock();
    CT_Done("DIAG-12: D3D9Surface::FillSurface Unlock done EXIT");
}

void D3D9Surface::FillRect(RECT* pRect, WORD color)
{
    if (!m_pSurface || !pRect)
        return;

    RECT rect = *pRect;
    if (rect.left   > m_ClipRight)  return;
    if (rect.top    > m_ClipBottom) return;
    if (rect.left   < m_ClipLeft)   rect.left   = m_ClipLeft;
    if (rect.right  > m_ClipRight)  rect.right  = m_ClipRight;
    if (rect.top    < m_ClipTop)    rect.top    = m_ClipTop;
    if (rect.bottom > m_ClipBottom) rect.bottom = m_ClipBottom;

    if (rect.right <= rect.left || rect.bottom <= rect.top)
        return;

    // 2026-05-15: D3D9 native port pattern — DDraw7 BltFast hardware kirpiyordu,
    // manuel sysmem bounds clamp. Round 3 sweep (BltNoColorKey ile parite).
    // Clip rect caller tarafindan m_Width/m_Height disinda set edilebilir
    // (SetClip RECT validation yapmiyor); rect.right > m_Width ise pRow[x]
    // pitch tasarsa next row'a yaziyor — race blit content corruption +
    // potansiyel AV.
    if (rect.left < 0)        rect.left   = 0;
    if (rect.top  < 0)        rect.top    = 0;
    if (rect.right  > m_Width)  rect.right  = m_Width;
    if (rect.bottom > m_Height) rect.bottom = m_Height;
    if (rect.right <= rect.left || rect.bottom <= rect.top)
        return;

    LockedRect locked;
    if (!Lock(locked, LockFlag_None))
        return;

    // 2026-05-15: D3D9 native port pattern — Lock sonrasi pBits/pitch sanity.
    if (!locked.pBits || locked.pitch <= 0)
    {
        Unlock();
        return;
    }

    if (m_Format == SurfaceFormat::R5G6B5 ||
        m_Format == SurfaceFormat::X1R5G5B5 ||
        m_Format == SurfaceFormat::A1R5G5B5 ||
        m_Format == SurfaceFormat::A4R4G4B4)
    {
        // pitch byte cinsinden; her satirda en fazla pitch/2 WORD yazilabilir
        const int maxX = locked.pitch / 2;
        const int safeRight = (rect.right < maxX) ? rect.right : maxX;
        for (int y = rect.top; y < rect.bottom; ++y)
        {
            WORD* pRow = (WORD*)((BYTE*)locked.pBits + y * locked.pitch);
            for (int x = rect.left; x < safeRight; ++x)
                pRow[x] = color;
        }
    }
    else if (m_Format == SurfaceFormat::X8R8G8B8 || m_Format == SurfaceFormat::A8R8G8B8)
    {
        const DWORD c32 = Expand565To8888(color);
        const int maxX = locked.pitch / 4;
        const int safeRight = (rect.right < maxX) ? rect.right : maxX;
        for (int y = rect.top; y < rect.bottom; ++y)
        {
            DWORD* pRow = (DWORD*)((BYTE*)locked.pBits + y * locked.pitch);
            for (int x = rect.left; x < safeRight; ++x)
                pRow[x] = c32;
        }
    }

    Unlock();
}

bool D3D9Surface::Restore()
{
    // D3D9'da surface-level Restore yok; device-level reset gerek. Caller
    // (D3D9Backend::RestoreAll) device reset yapar, surface'lar
    // D3DPOOL_DEFAULT'sa cagiran taraf rebind eder. Phase 1'de sysmem
    // surface'lar (R5G6B5 composition + atlas) reset gerektirmez.
    //
    // Asla false dondurmuyoruz cunku sysmem surface kayip degildir; bu sayede
    // Lock retry loop'lari sonsuza degil sonludur.
    return true;
}

//-----------------------------------------------------------------------------
// D3D9Backend
//-----------------------------------------------------------------------------
D3D9Backend::D3D9Backend()
    : m_pD3D(NULL)
    , m_pDevice(NULL)
    , m_hWnd(NULL)
    , m_Width(0)
    , m_Height(0)
    , m_bFullscreen(false)
    , m_pCompositionBuffer(NULL)
    , m_pCompositionSurface(NULL)
    , m_pCompositionTexture(NULL)
    , m_pCompositionTexLevel(NULL)
    , m_pCachedBack(NULL)
    , m_pCachedPrimary(NULL)
    , m_GammaStep(100)
    , m_HasDirty(false)
    , m_FullDirty(false)
{
    ZeroMemory(&m_FullscreenDisplayMode, sizeof(m_FullscreenDisplayMode));
    m_FullscreenDisplayMode.Size = sizeof(m_FullscreenDisplayMode);
    ZeroMemory(&m_PresentParams, sizeof(m_PresentParams));
    ZeroMemory(&m_GammaRamp, sizeof(m_GammaRamp));
    m_AddGammaStep[0] = 0;
    m_AddGammaStep[1] = 0;
    m_AddGammaStep[2] = 0;
    // Dirty rect baslangic: bos (max-int min/max). MarkDirty union'la expand.
    m_DirtyRect.left   = INT_MAX;
    m_DirtyRect.top    = INT_MAX;
    m_DirtyRect.right  = INT_MIN;
    m_DirtyRect.bottom = INT_MIN;

    // Identity gamma ramp baslangic (0..255 -> 0x0000..0xFFFF)
    for (int i = 0; i < 256; ++i)
    {
        WORD v = (WORD)((i * 0xFFFF) / 255);
        m_GammaRamp.red[i]   = v;
        m_GammaRamp.green[i] = v;
        m_GammaRamp.blue[i]  = v;
    }
}

D3D9Backend::~D3D9Backend()
{
    Shutdown();
}

D3D9Backend& D3D9Backend::Instance()
{
    static D3D9Backend s_Instance;
    return s_Instance;
}

bool D3D9Backend::PickFullscreenDisplayMode(WORD width, WORD height)
{
    ZeroMemory(&m_FullscreenDisplayMode, sizeof(m_FullscreenDisplayMode));
    m_FullscreenDisplayMode.Size = sizeof(m_FullscreenDisplayMode);

    if (!m_pD3D)
        return false;

    D3DDISPLAYMODEFILTER filter = {};
    filter.Size = sizeof(filter);
    filter.Format = D3DFMT_X8R8G8B8;
    filter.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;

    const UINT modeCount = m_pD3D->GetAdapterModeCountEx(D3DADAPTER_DEFAULT, &filter);
    UINT bestRefresh = 0;
    bool foundExact = false;
    for (UINT i = 0; i < modeCount; ++i)
    {
        D3DDISPLAYMODEEX mode = {};
        mode.Size = sizeof(mode);
        if (FAILED(m_pD3D->EnumAdapterModesEx(D3DADAPTER_DEFAULT, &filter, i, &mode)))
            continue;
        if (mode.Width != width || mode.Height != height)
            continue;
        if (!foundExact || mode.RefreshRate > bestRefresh)
        {
            m_FullscreenDisplayMode = mode;
            m_FullscreenDisplayMode.Size = sizeof(m_FullscreenDisplayMode);
            bestRefresh = mode.RefreshRate;
            foundExact = true;
        }
    }

    if (foundExact)
    {
        CT_Done("D3D9Backend::PickFullscreenDisplayMode exact %ux%u @%uHz",
                m_FullscreenDisplayMode.Width, m_FullscreenDisplayMode.Height,
                m_FullscreenDisplayMode.RefreshRate);
        return true;
    }

    // Exact resolution yoksa masaustu refresh + istenen WxH ile dene.
    D3DDISPLAYMODEEX desktop = {};
    desktop.Size = sizeof(desktop);
    D3DDISPLAYROTATION rotation = D3DDISPLAYROTATION_IDENTITY;
    if (FAILED(m_pD3D->GetAdapterDisplayModeEx(D3DADAPTER_DEFAULT, &desktop, &rotation)))
    {
        CT_Done("D3D9Backend::PickFullscreenDisplayMode GetAdapterDisplayModeEx FAILED");
        return false;
    }

    m_FullscreenDisplayMode.Size = sizeof(m_FullscreenDisplayMode);
    m_FullscreenDisplayMode.Width = width;
    m_FullscreenDisplayMode.Height = height;
    m_FullscreenDisplayMode.RefreshRate = desktop.RefreshRate ? desktop.RefreshRate : 60;
    m_FullscreenDisplayMode.Format = D3DFMT_X8R8G8B8;
    m_FullscreenDisplayMode.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
    CT_Done("D3D9Backend::PickFullscreenDisplayMode fallback %ux%u @%uHz (desktop Hz)",
            m_FullscreenDisplayMode.Width, m_FullscreenDisplayMode.Height,
            m_FullscreenDisplayMode.RefreshRate);
    return true;
}

bool D3D9Backend::Init(HWND hWnd, WORD width, WORD height, ScreenMode mode, bool /*use3D*/, bool /*useIME*/)
{
    if (m_pDevice)
    {
        // Zaten init edilmis — Phase 1'de re-init handle etmiyoruz, caller
        // Shutdown() cagirsin.
        return true;
    }

    m_hWnd        = hWnd;
    m_Width       = width;
    m_Height      = height;
    m_bFullscreen = (mode == ScreenMode::Fullscreen);

    // 1. IDirect3D9Ex (Vista+ — modern Windows DWM iyi gecisme)
    HRESULT hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &m_pD3D);
    if (FAILED(hr) || !m_pD3D)
    {
        CT_Done("D3D9Backend::Init Direct3DCreate9Ex FAILED 0x%08X", hr);
        return false;
    }

    // Exclusive fullscreen: D3D9Ex CreateDeviceEx/ResetEx icin D3DDISPLAYMODEEX*
    // zorunlu (NULL = INVALIDCALL). Refresh rate EnumAdapterModesEx ile secilir.
    D3DDISPLAYMODEEX* pFullscreenMode = NULL;
    if (m_bFullscreen)
    {
        if (!PickFullscreenDisplayMode(width, height))
        {
            CT_Done("D3D9Backend::Init PickFullscreenDisplayMode FAILED");
            m_pD3D->Release();
            m_pD3D = NULL;
            return false;
        }
        pFullscreenMode = &m_FullscreenDisplayMode;
    }

    // 2. PresentParams
    ZeroMemory(&m_PresentParams, sizeof(m_PresentParams));
    m_PresentParams.Windowed               = m_bFullscreen ? FALSE : TRUE;
    m_PresentParams.BackBufferWidth        = width;
    m_PresentParams.BackBufferHeight       = height;
    m_PresentParams.BackBufferFormat       = D3DFMT_X8R8G8B8;     // 32-bit primary
    // 2026-08-29: VSync kaldirildi — tek back buffer (legacy baseline).
    m_PresentParams.BackBufferCount        = 1;
    m_PresentParams.MultiSampleType        = D3DMULTISAMPLE_NONE;
    m_PresentParams.MultiSampleQuality     = 0;
    m_PresentParams.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    m_PresentParams.hDeviceWindow          = hWnd;
    m_PresentParams.EnableAutoDepthStencil = TRUE;
    m_PresentParams.AutoDepthStencilFormat = D3DFMT_D24S8;
    m_PresentParams.Flags                  = 0;
    // Windowed: 0 zorunlu. Exclusive: EnumAdapterModesEx secilen Hz.
    m_PresentParams.FullScreen_RefreshRateInHz =
        m_bFullscreen ? m_FullscreenDisplayMode.RefreshRate : 0;
    // 2026-08-31: Tearing root cause fix — sunum vblank'e kilitlendi (INTERVAL_ONE).
    // IMMEDIATE + uncapped ana dongu, menu/scroll hareketinde scanout tear'i
    // yaratiyordu (Dev_Log 2026-08-31 analiz). Orijinal DDraw yolu da
    // Flip(DDFLIP_WAIT) ile vblank-senkronluydu — referans davranisla uyumlu.
    // Monitor kac Hz ise o FPS'e kilitlenir (PickFullscreenDisplayMode max Hz secer).
    m_PresentParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    // 3. CreateDeviceEx — exclusive'te pFullscreenDisplayMode NULL olamaz.
    DWORD behavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    hr = m_pD3D->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                 behavior, &m_PresentParams, pFullscreenMode, &m_pDevice);
    if (FAILED(hr))
    {
        // SW vertex processing fallback (DX9 minimum spec)
        behavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        hr = m_pD3D->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                     behavior, &m_PresentParams, pFullscreenMode, &m_pDevice);
    }
    if (FAILED(hr) || !m_pDevice)
    {
        CT_Done("D3D9Backend::Init CreateDeviceEx FAILED 0x%08X fs=%d rate=%u",
                hr, (int)m_bFullscreen, m_PresentParams.FullScreen_RefreshRateInHz);
        m_pD3D->Release();
        m_pD3D = NULL;
        return false;
    }

    // CreateDeviceEx PresentParams boyutunu degistirebilir — composition
    // buffer'i gercek backbuffer ile senkron tut.
    if (m_PresentParams.BackBufferWidth > 0 && m_PresentParams.BackBufferHeight > 0)
    {
        m_Width  = (WORD)m_PresentParams.BackBufferWidth;
        m_Height = (WORD)m_PresentParams.BackBufferHeight;
    }

    // Exclusive: HWND'yi backbuffer boyutuna zorla (InitApp yanlis boyut biraktiysa).
    if (m_bFullscreen && hWnd != NULL)
    {
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, (int)m_Width, (int)m_Height,
                     SWP_SHOWWINDOW | SWP_NOCOPYBITS);
    }

    // Exclusive HW gamma: GetGammaRamp bazen mode-switch aninda bozuk doner;
    // identity baseline ile basla, sonra opsiyonel UserOption uygular.
    if (m_bFullscreen)
    {
        for (int i = 0; i < 256; ++i)
        {
            WORD v = (WORD)((i * 0xFFFF) / 255);
            m_GammaRamp.red[i]   = v;
            m_GammaRamp.green[i] = v;
            m_GammaRamp.blue[i]  = v;
        }
        m_pDevice->SetGammaRamp(0, D3DSGR_NO_CALIBRATION, &m_GammaRamp);
    }

    CT_Done("D3D9Backend::Init CreateDeviceEx OK %dx%d %s rate=%u",
            (int)m_Width, (int)m_Height, m_bFullscreen ? "fullscreen" : "windowed",
            m_PresentParams.FullScreen_RefreshRateInHz);

    // 4. Composition resources
    if (!CreateCompositionResources())
    {
        CT_Done("D3D9Backend::Init CreateCompositionResources FAILED");
        Shutdown();
        return false;
    }

    // 5. Baseline gamma — exclusive'te identity zaten Init'te set edildi.
    // GetGammaRamp mode-switch aninda gri/bozuk ramp dondurebiliyordu; atlandi.
    if (m_bFullscreen)
    {
        // no-op: identity ramp yukarida yazildi
    }

    // 6. Phase 7 (2026-05-14) — post-process shader (vignette).
    // Fail soft: shader compile/create fail edebilir (ps_2_0 desteksiz GPU
    // teorik olarak DX9 minimum spec'inde olsa da) — Apply otomatik no-op.
    D3D9PostProcess_Init(m_pDevice);

    // 2026-05-16: UserOption'dan per-effect runtime toggle'lari uygula (Vignette).
    // PostProcess_Init basarisiz olsa bile setter'lar global state'i guncelliyor;
    // bir sonraki re-init (or load saved option) dogru state'le baslar.
    if (g_pUserOption)
    {
        D3D9PostProcess_SetVignetteEnabled(g_pUserOption->VignetteEnabled ? true : false);
        D3D9PostProcess_SetBrightnessStep((!m_bFullscreen && g_pUserOption->UseGammaControl) ? (unsigned short)g_pUserOption->GammaValue : 100);
    }

    // 2026-05-18 ImGui Hibrit Admin Menu: D3D9 device hazir, ImGui backend
    // context + Win32 input handler kurulur. attrOperator gate Render() icinde
    // her frame check edilir; non-operator user'da panel hiç cizilmez.
    PhoenixImGui::Initialize(hWnd, m_pDevice);

    return true;
}

bool D3D9Backend::CreateCompositionResources()
{
    if (!m_pDevice)
        return false;

    // 16-bit composition buffer — sysmem, R5G6B5. Sprite codec hedefi.
    HRESULT hr = m_pDevice->CreateOffscreenPlainSurface(
        m_Width, m_Height, D3DFMT_R5G6B5, D3DPOOL_SYSTEMMEM,
        &m_pCompositionBuffer, NULL);
    if (FAILED(hr) || !m_pCompositionBuffer)
    {
        CT_Done("D3D9Backend::CreateCompositionResources OffscreenPlain FAILED 0x%08X", hr);
        return false;
    }

    // 2026-09-02 crash fix: wrapper objesi bir kez yaratilir, sonraki
    // Create dongulerinde (Resize/Reset) yeni buffer'a Rebind edilir —
    // "Yol 1" tuketicileri (g_pBack/g_pLast/VS_UI Base) dangling kalmaz.
    if (m_pCompositionSurface == NULL)
        m_pCompositionSurface = new D3D9Surface(m_pCompositionBuffer,
                                                m_Width, m_Height,
                                                SurfaceFormat::R5G6B5,
                                                /*owned=*/false);
    else
        m_pCompositionSurface->Rebind(m_pCompositionBuffer, m_Width, m_Height);

    // Composition buffer'i present etmek icin managed texture
    // (D3DPOOL_DEFAULT + D3DUSAGE_DYNAMIC: hizli CPU upload).
    hr = m_pDevice->CreateTexture(m_Width, m_Height, 1,
                                   D3DUSAGE_DYNAMIC,
                                   D3DFMT_R5G6B5, D3DPOOL_DEFAULT,
                                   &m_pCompositionTexture, NULL);
    if (FAILED(hr) || !m_pCompositionTexture)
    {
        CT_Done("D3D9Backend::CreateCompositionResources Texture FAILED 0x%08X", hr);
        return false;
    }

    // 2026-05-16 C1: surface level 0 cache — Present her frame yeniden almasin.
    // Cache nullsa Present fallback path'i kullanir.
    HRESULT hrLevel = m_pCompositionTexture->GetSurfaceLevel(0, &m_pCompositionTexLevel);
    if (FAILED(hrLevel))
        m_pCompositionTexLevel = NULL;

    return true;
}

void D3D9Backend::ReleaseCompositionResources()
{
    // 2026-09-02 crash fix: wrapper SILINMEZ (obje kimligi korunur, Yol 1
    // tuketicileri pointer'i saklar). Alttaki COM surface release edilecegi
    // icin gecici olarak NULL'a baglanir; CreateCompositionResources yeni
    // buffer ile Rebind eder. Kalici silme yalniz Shutdown'da.
    if (m_pCompositionSurface)
        m_pCompositionSurface->Rebind(NULL, 0, 0);
    // 2026-05-16 C1: cached surface level — texture'dan once release (texture'a
    // AddRef tutuyor; aksi halde texture release sayisi yetersiz kalir).
    if (m_pCompositionTexLevel)
    {
        m_pCompositionTexLevel->Release();
        m_pCompositionTexLevel = NULL;
    }
    if (m_pCompositionTexture)
    {
        m_pCompositionTexture->Release();
        m_pCompositionTexture = NULL;
    }
    if (m_pCompositionBuffer)
    {
        m_pCompositionBuffer->Release();
        m_pCompositionBuffer = NULL;
    }
}

//-----------------------------------------------------------------------------
// 2026-09-02 O3 — canli cozunurluk degisimi.
// Device korunur; PresentParams backbuffer boyutu degistirilip ResetEx yapilir,
// composition kaynaklari yeni boyutla geri gelir.
//-----------------------------------------------------------------------------
bool D3D9Backend::Resize(WORD width, WORD height)
{
    if (!m_pDevice || width == 0 || height == 0)
        return false;
    if (width == m_Width && height == m_Height)
        return true;

    CT_Done("D3D9Backend::Resize %dx%d -> %dx%d",
            (int)m_Width, (int)m_Height, (int)width, (int)height);

    // Exclusive fullscreen: yeni boyut icin display mode sec (ResetEx zorunlu arg)
    D3DDISPLAYMODEEX* pFullscreenMode = NULL;
    if (m_bFullscreen)
    {
        if (!PickFullscreenDisplayMode(width, height))
        {
            CT_Done("D3D9Backend::Resize PickFullscreenDisplayMode FAILED");
            return false;
        }
        pFullscreenMode = &m_FullscreenDisplayMode;
    }

    // POOL_DEFAULT + composition kaynaklarini birak (RestoreAll deseni)
    if (m_pCachedBack)
    {
        delete m_pCachedBack;
        m_pCachedBack = NULL;
    }
    ReleaseCompositionResources();
    PhoenixImGui::OnDeviceLost();

    const UINT oldW = m_PresentParams.BackBufferWidth;
    const UINT oldH = m_PresentParams.BackBufferHeight;
    m_PresentParams.BackBufferWidth  = width;
    m_PresentParams.BackBufferHeight = height;
    m_PresentParams.FullScreen_RefreshRateInHz =
        m_bFullscreen ? m_FullscreenDisplayMode.RefreshRate : 0;

    HRESULT hr = m_pDevice->ResetEx(&m_PresentParams, pFullscreenMode);
    if (FAILED(hr))
    {
        CT_Done("D3D9Backend::Resize ResetEx FAILED 0x%08X — eski boyuta donuluyor", hr);
        m_PresentParams.BackBufferWidth  = oldW;
        m_PresentParams.BackBufferHeight = oldH;
        // Eski boyutla toparlanma denemesi (device operational kalsin)
        if (SUCCEEDED(m_pDevice->ResetEx(&m_PresentParams, pFullscreenMode)))
        {
            CreateCompositionResources();
            PhoenixImGui::OnDeviceReset();
        }
        return false;
    }

    m_Width  = width;
    m_Height = height;

    if (!CreateCompositionResources())
    {
        CT_Done("D3D9Backend::Resize CreateCompositionResources FAILED");
        return false;
    }
    PhoenixImGui::OnDeviceReset();

    CT_Done("D3D9Backend::Resize OK %dx%d", (int)m_Width, (int)m_Height);
    return true;
}

void D3D9Backend::Shutdown()
{
    // ImGui DX9 backend device-owned buffers/textures tutar. Backend kapatma
    // yolunda device Release edilmeden once idempotent olarak serbest birak.
    PhoenixImGui::Shutdown();

    if (m_pCachedBack)
    {
        delete m_pCachedBack;
        m_pCachedBack = NULL;
    }
    if (m_pCachedPrimary)
    {
        delete m_pCachedPrimary;
        m_pCachedPrimary = NULL;
    }

    ReleaseCompositionResources();

    // 2026-09-02 crash fix: wrapper objesinin kalici silinmesi yalniz burada
    // (ReleaseCompositionResources artik silmiyor — Rebind ile korunuyor).
    if (m_pCompositionSurface)
    {
        delete m_pCompositionSurface;
        m_pCompositionSurface = NULL;
    }

    // Phase 7 (2026-05-14) — post-process shader release. Device release'ten
    // ONCE; shader device-owned (CreatePixelShader).
    D3D9PostProcess_Shutdown();

    if (m_pDevice)
    {
        m_pDevice->Release();
        m_pDevice = NULL;
    }
    if (m_pD3D)
    {
        m_pD3D->Release();
        m_pD3D = NULL;
    }

    m_hWnd = NULL;
}

bool D3D9Backend::RestoreAll()
{
    if (!m_pDevice)
        return false;

    HRESULT hr = m_pDevice->CheckDeviceState(m_hWnd);
    if (hr == S_OK || hr == S_PRESENT_OCCLUDED)
        return true;

    // 2026-05-17 FAZ 3 Fix #1: entry trace — RestoreAll trigger nedeni
    // (CheckDeviceState donus kodu) ve sonrasi recreate sayisi log.
    OutputDebugStringA("[D3D9Backend::RestoreAll] ENTER (device state changed, recreating POOL_DEFAULT resources)\n");
    CT_Done("D3D9Backend::RestoreAll ENTER hr=0x%08X (device state)", hr);

    // Lost / hung — device reset gerek. D3DPOOL_DEFAULT kaynaklari (texture +
    // cached back buffer) reset oncesi release; reset sonrasi yeniden alinir.
    // sysmem composition buffer kayip degil — reset surecini gecer.
    // 2026-05-16 C1: cached surface level texture'a AddRef tutuyor — texture'dan
    // once release ki texture release sayisi dogru olsun.
    //
    // 2026-05-17 FAZ 3 Fix #1: Suanda RestoreAll yalnizca composition texture
    // ve cached back buffer'i recreate ediyor. CDirectDrawSurface uzerinden
    // olusturulan atlas surface'lari (CTexturePack vs.) artik D3DPOOL_MANAGED
    // — driver kendi recreate eder. Atlas registry + manual recreate'i ileri
    // faz Option A icin saklandi (POOL_DEFAULT yolu yeniden acilirsa).
    int recreatedCount = 0;

    if (m_pCompositionTexLevel)
    {
        m_pCompositionTexLevel->Release();
        m_pCompositionTexLevel = NULL;
    }
    if (m_pCompositionTexture)
    {
        m_pCompositionTexture->Release();
        m_pCompositionTexture = NULL;
    }
    if (m_pCachedBack)
    {
        delete m_pCachedBack;       // owned=true -> back-buffer ref Release eder
        m_pCachedBack = NULL;
    }

    // 2026-05-18 ImGui Hibrit Admin Menu: ResetEx oncesi backend POOL_DEFAULT
    // objelerini invalidate et (font texture vs.) — yoksa Reset FAIL veya
    // garbage texture. Reset sonrasi CreateDeviceObjects ile yenilenir.
    PhoenixImGui::OnDeviceLost();

    // Exclusive fullscreen ResetEx: D3DDISPLAYMODEEX* zorunlu (NULL = fail).
    D3DDISPLAYMODEEX* pFullscreenMode = m_bFullscreen ? &m_FullscreenDisplayMode : NULL;
    hr = m_pDevice->ResetEx(&m_PresentParams, pFullscreenMode);
    if (FAILED(hr))
    {
        OutputDebugStringA("[D3D9Backend::RestoreAll] ResetEx FAILED\n");
        CT_Done("D3D9Backend::RestoreAll ResetEx FAILED 0x%08X", hr);
        return false;
    }

    // Texture'i yeniden olustur — caller bir sonraki Present'te kullanir.
    hr = m_pDevice->CreateTexture(m_Width, m_Height, 1,
                                   D3DUSAGE_DYNAMIC,
                                   D3DFMT_R5G6B5, D3DPOOL_DEFAULT,
                                   &m_pCompositionTexture, NULL);
    if (FAILED(hr))
    {
        OutputDebugStringA("[D3D9Backend::RestoreAll] CreateTexture (composition) FAILED\n");
        return false;
    }
    ++recreatedCount;  // composition texture

    // 2026-05-16 C1: yeniden olusturulan texture icin surface level cache yenile.
    HRESULT hrLevel = m_pCompositionTexture->GetSurfaceLevel(0, &m_pCompositionTexLevel);
    if (FAILED(hrLevel))
        m_pCompositionTexLevel = NULL;

    // 2026-05-17 FAZ 3 Fix #1: exit trace + recreate sayisi.
    char szLog[160];
    _snprintf_s(szLog, sizeof(szLog), _TRUNCATE,
                "[D3D9Backend::RestoreAll] EXIT OK — %d POOL_DEFAULT surface(s) recreated "
                "(composition texture). Atlas surfaces MANAGED — driver auto-recreate.\n",
                recreatedCount);
    OutputDebugStringA(szLog);
    CT_Done("D3D9Backend::RestoreAll EXIT OK recreated=%d", recreatedCount);

    // 2026-05-18 ImGui Hibrit Admin Menu: ResetEx + composition texture yeniden
    // olusturuldu, ImGui backend objelerini de tekrar yarat (font atlas vs.).
    PhoenixImGui::OnDeviceReset();

    return true;
}

//-----------------------------------------------------------------------------
// Present
//
// 16-bit composition buffer pattern:
//   1. compositionBuffer (sysmem) -> compositionTexture (default pool) upload
//   2. Fullscreen quad TLVERTEX cizimi
//   3. Present()
//-----------------------------------------------------------------------------
void D3D9Backend::Present()
{
    const bool fpEnabled = Phoenix_FrameProbe_Enabled();
    LONGLONG fpTotalStart = 0;
    double fpStateMs = 0.0;
    double fpUploadMs = 0.0;
    double fpSceneMs = 0.0;
    double fpImguiMs = 0.0;
    double fpPresentMs = 0.0;
    HRESULT fpStateHr = S_OK;
    HRESULT fpPresentHr = S_OK;
    const int fpForeground = (m_hWnd != NULL) ? (int)(GetForegroundWindow() == m_hWnd) : 0;
    const int fpIconic = (m_hWnd != NULL) ? (int)IsIconic(m_hWnd) : 0;
    const int fpVisible = (m_hWnd != NULL) ? (int)IsWindowVisible(m_hWnd) : 0;

    if (fpEnabled)
        fpTotalStart = Phoenix_FrameProbe_Now();

    if (!m_pDevice || !m_pCompositionBuffer || !m_pCompositionTexture)
    {
        if (fpEnabled)
        {
            const LONGLONG fpEnd = Phoenix_FrameProbe_Now();
            Phoenix_FrameProbe_RecordPresent(
                Phoenix_FrameProbe_Ms(fpTotalStart, fpEnd),
                0.0, 0.0, 0.0, 0.0, 0.0,
                fpStateHr, fpPresentHr, 4,
                fpForeground, fpIconic, fpVisible);
        }
        return;
    }

    // Focus loss gercek occlusion degildir. Pencere gorunur durumdayken
    // baska pencereye odaklanmak render'i 30 FPS'e dusurmemeli. Minimize
    // (IsIconic) disinda CheckDeviceState OCCLUDED Present'i durdurmaz —
    // exclusive D3D9Ex false OCCLUDED + vis=0 latch (Vampire 4002).
    if (m_hWnd != NULL)
    {
        if (IsIconic(m_hWnd))
        {
            // Vampire hang yakalama: Sleep oncesi yaz; donunca son satir ICONIC olur.
            static DWORD s_LastPresentIconicTrace = 0;
            DWORD nowIconic = GetTickCount();
            if (nowIconic - s_LastPresentIconicTrace > 1000) {
                s_LastPresentIconicTrace = nowIconic;
                CT_BD("[PRESENT] ICONIC sleep100 fg=%d vis=%d",
                      fpForeground, fpVisible);
            }
            Sleep(100);
            if (fpEnabled)
            {
                const LONGLONG fpEnd = Phoenix_FrameProbe_Now();
                Phoenix_FrameProbe_RecordPresent(
                    Phoenix_FrameProbe_Ms(fpTotalStart, fpEnd),
                    fpStateMs, 0.0, 0.0, 0.0, 0.0,
                    fpStateHr, fpPresentHr, 1,
                    fpForeground, fpIconic, fpVisible);
            }
            return;
        }

        LONGLONG fpStateStart = 0;
        if (fpEnabled)
            fpStateStart = Phoenix_FrameProbe_Now();
        const HRESULT hrState = m_pDevice->CheckDeviceState(m_hWnd);
        fpStateHr = hrState;
        if (fpEnabled)
            fpStateMs = Phoenix_FrameProbe_Ms(fpStateStart, Phoenix_FrameProbe_Now());
        if (hrState == S_PRESENT_OCCLUDED)
        {
            // Exclusive D3D9Ex: CheckDeviceState OCCLUDED + !IsIconic false
            // pozitif olabiliyor (Vampire 4002: fg=0 vis=0 iconic=0). Sleep+
            // return kareyi sonsuz atlayinca ekran donuyor, GS timeout.
            // Minimize degilse Present'e devam; asil Present() OCCLUDED
            // donerse zaten gozardi ediliyor (asagida).
            static DWORD s_LastPresentOccludedTrace = 0;
            DWORD nowOcc = GetTickCount();
            if (nowOcc - s_LastPresentOccludedTrace > 1000) {
                s_LastPresentOccludedTrace = nowOcc;
                CT_BD("[PRESENT] OCCLUDED continue (not iconic) hr=0x%08X fg=%d vis=%d iconic=%d",
                      (unsigned)hrState, fpForeground, fpVisible, fpIconic);
            }
        }
        else if (hrState != S_OK)
        {
            static DWORD s_LastPresentRestoreTrace = 0;
            DWORD nowRestore = GetTickCount();
            if (nowRestore - s_LastPresentRestoreTrace > 1000) {
                s_LastPresentRestoreTrace = nowRestore;
                CT_BD("[PRESENT] RESTORE hr=0x%08X fg=%d vis=%d iconic=%d",
                      (unsigned)hrState, fpForeground, fpVisible, fpIconic);
            }
            RestoreAll();
            if (fpEnabled)
            {
                const LONGLONG fpEnd = Phoenix_FrameProbe_Now();
                Phoenix_FrameProbe_RecordPresent(
                    Phoenix_FrameProbe_Ms(fpTotalStart, fpEnd),
                    fpStateMs, 0.0, 0.0, 0.0, 0.0,
                    fpStateHr, fpPresentHr, 3,
                    fpForeground, fpIconic, fpVisible);
            }
            return;
        }
    }

    // 2026-05-16 Dalga 2 — M1: frame penceresini ac. FL2 DC cache + diger
    // per-frame state'lerin "yeni frame" sinyaline ihtiyaci var.
    BeginFrame();

    // 1. Composition buffer -> texture upload
    // 2026-05-16 C1: cached level kullan (Create'te alindi); cache nullsa
    // fallback'te per-frame al — eski davranis korunur.
    IDirect3DSurface9* pTexSurface = m_pCompositionTexLevel;
    HRESULT hr = S_OK;
    LONGLONG fpUploadStart = 0;
    if (fpEnabled)
        fpUploadStart = Phoenix_FrameProbe_Now();
    if (!pTexSurface)
    {
        hr = m_pCompositionTexture->GetSurfaceLevel(0, &pTexSurface);
    }
    if (SUCCEEDED(hr) && pTexSurface)
    {
        // 2026-05-16 Dalga 2 — C2: dirty-rect aware UpdateSurface.
        // Conservative phase 1: dirty rect explicit isaretlenmemis veya
        // FullDirty ise NULL src/dst (eski davranis 1280x1024 tam kopya).
        // MarkDirty cagrilmis ve FullDirty degilse, birlesim rect ile
        // partial upload — UI heavy frame'lerde bandwidth dususu.
        //
        // D3D9 UpdateSurface kontrati: src rect SISTEMMEM source bounds icinde
        // olmali; dst rect olarak NULL gecince src rect dimensions kullanilir
        // (dst origin = src origin). Phoenix'te composition buffer ve texture
        // 1:1 (ayni w/h), bu yuzden src rect'i dst icin de gecerli.
        if (m_HasDirty && !m_FullDirty)
        {
            RECT src = m_DirtyRect;
            // Clamp surface bounds icinde (negative/oversize'a karsi defansif)
            if (src.left   < 0)         src.left   = 0;
            if (src.top    < 0)         src.top    = 0;
            if (src.right  > m_Width)   src.right  = m_Width;
            if (src.bottom > m_Height)  src.bottom = m_Height;
            if (src.right  > src.left && src.bottom > src.top)
            {
                // Dst point = src.left/top — composition 1:1 texture mapping
                POINT dstPt = { src.left, src.top };
                m_pDevice->UpdateSurface(m_pCompositionBuffer, &src, pTexSurface, &dstPt);
            }
            else
            {
                // Rect yozlasti (max/min ters) — full fallback
                m_pDevice->UpdateSurface(m_pCompositionBuffer, NULL, pTexSurface, NULL);
            }
        }
        else
        {
            // Backward-compat: tam surface kopya (caller MarkDirty cagirmadi)
            m_pDevice->UpdateSurface(m_pCompositionBuffer, NULL, pTexSurface, NULL);
        }
        // Sadece fallback path AddRef yapti — sadece o pointer'i release et.
        if (pTexSurface != m_pCompositionTexLevel)
            pTexSurface->Release();
    }
    if (fpEnabled)
        fpUploadMs = Phoenix_FrameProbe_Ms(fpUploadStart, Phoenix_FrameProbe_Now());

    // 2. Quad + Present
    LONGLONG fpSceneStart = 0;
    if (fpEnabled)
        fpSceneStart = Phoenix_FrameProbe_Now();
    hr = m_pDevice->BeginScene();
    if (SUCCEEDED(hr))
    {
        // Exclusive FS'te onceki frame discard/undefined backbuffer gri gorunebiliyor;
        // quad oncesi clear ile temiz canvas.
        m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

        // Render state baseline — sprite codec icin hicbir blend gerekmiyor
        m_pDevice->SetRenderState(D3DRS_LIGHTING,       FALSE);
        m_pDevice->SetRenderState(D3DRS_ZENABLE,        FALSE);
        m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,  FALSE);
        m_pDevice->SetRenderState(D3DRS_CULLMODE,       D3DCULL_NONE);

        // Texture stage 0 — sade kopya
        m_pDevice->SetTexture(0, m_pCompositionTexture);
        m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
        m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
        m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
        m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
        m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
        m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);

        // Fullscreen quad — XYZRHW (pre-transformed, viewport-space)
        struct QuadVertex
        {
            float x, y, z, rhw;
            float u, v;
        };
        const float fw = (float)m_Width;
        const float fh = (float)m_Height;
        // D3D9 texel center offset: pre-transformed vertex'lerde 0.5 piksel
        // shift gerekiyor (D3D9 sampling kurali). Aksi halde fullscreen
        // texture half-pixel kayar.
        const float half = 0.5f;
        QuadVertex quad[4] =
        {
            { -half,      -half,      0.0f, 1.0f, 0.0f, 0.0f },
            {  fw - half, -half,      0.0f, 1.0f, 1.0f, 0.0f },
            { -half,       fh - half, 0.0f, 1.0f, 0.0f, 1.0f },
            {  fw - half,  fh - half, 0.0f, 1.0f, 1.0f, 1.0f }
        };

        m_pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);

        // Phase 7 (2026-05-14) — post-process pixel shader (vignette).
        D3D9PostProcess_Apply(m_pDevice);
        m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(QuadVertex));
        D3D9PostProcess_Reset(m_pDevice);

        m_pDevice->SetTexture(0, NULL);
        m_pDevice->EndScene();
    }
    if (fpEnabled)
        fpSceneMs = Phoenix_FrameProbe_Ms(fpSceneStart, Phoenix_FrameProbe_Now());

    // 2026-05-18 ImGui Hibrit Admin Menu: BeginScene/EndScene dishinda kendi
    // BeginScene yapacagi icin burada Present'tan ONCE Render() cagriliyor.
    // ImGui_ImplDX9 internal BeginScene/EndScene + DrawIndexedPrimitive eder.
    // Non-operator karakter'de Render no-op (Render icinde gate var).
    LONGLONG fpImguiStart = 0;
    if (fpEnabled)
        fpImguiStart = Phoenix_FrameProbe_Now();
    PhoenixImGui::Render();
    if (fpEnabled)
        fpImguiMs = Phoenix_FrameProbe_Ms(fpImguiStart, Phoenix_FrameProbe_Now());

    LONGLONG fpPresentStart = 0;
    if (fpEnabled)
        fpPresentStart = Phoenix_FrameProbe_Now();
    {
        static DWORD s_LastPresentPulse = 0;
        const DWORD nowPulse = GetTickCount();
        const bool pulse = (nowPulse - s_LastPresentPulse > 1000);
        if (pulse) {
            s_LastPresentPulse = nowPulse;
            CT_BD("[PRESENT] before device Present hrState=0x%08X fg=%d vis=%d iconic=%d",
                  (unsigned)fpStateHr, fpForeground, fpVisible, fpIconic);
        }
        hr = m_pDevice->Present(NULL, NULL, NULL, NULL);
        if (pulse) {
            CT_BD("[PRESENT] after device Present hr=0x%08X", (unsigned)hr);
        }
    }
    fpPresentHr = hr;
    if (fpEnabled)
        fpPresentMs = Phoenix_FrameProbe_Ms(fpPresentStart, Phoenix_FrameProbe_Now());
    if (hr == D3DERR_DEVICELOST || hr == D3DERR_DEVICEHUNG || hr == D3DERR_DEVICEREMOVED)
    {
        RestoreAll();
    }
    // D3D9Ex'te S_PRESENT_OCCLUDED success ama frame sunulamadi (window
    // gizli/desktop locked). D5 (focus-loss frame skip) call site bunu
    // m_bLastPresentOccluded uzerinden ogrenir. Phase 1: gozardi et —
    // CWinMain main loop kendi IsActive() check'iyle frame skip yapacak.

    // 2026-05-16 Dalga 2 — C2/M1: frame penceresini kapat. Dirty state reset,
    // FL2 DC cache release (eger frame icinde acildi ise EndFrame icinde).
    m_HasDirty = false;
    m_FullDirty = false;
    m_DirtyRect.left   = INT_MAX;
    m_DirtyRect.top    = INT_MAX;
    m_DirtyRect.right  = INT_MIN;
    m_DirtyRect.bottom = INT_MIN;

    EndFrame();

    if (fpEnabled)
    {
        const LONGLONG fpEnd = Phoenix_FrameProbe_Now();
        Phoenix_FrameProbe_RecordPresent(
            Phoenix_FrameProbe_Ms(fpTotalStart, fpEnd),
            fpStateMs, fpUploadMs, fpSceneMs, fpImguiMs, fpPresentMs,
            fpStateHr, fpPresentHr, 0,
            fpForeground, fpIconic, fpVisible);
    }
}

//-----------------------------------------------------------------------------
// MarkDirty / MarkDirtyFull (2026-05-16 Dalga 2 — C2)
//
// Composition buffer'in degisen bolgesini akumule eder. Birlesim rect
// (min-left/top, max-right/bottom). FullDirty sinyali bir kez set edilirse
// rect ihmal edilir (caller "tam frame yenilendi" diyor — Present NULL upload).
//-----------------------------------------------------------------------------
void D3D9Backend::MarkDirty(const RECT& r)
{
    if (m_FullDirty)
        return;  // Zaten full — rect islemenin anlami yok

    // Bos/yozlasik rect ihmal et
    if (r.right <= r.left || r.bottom <= r.top)
        return;

    if (!m_HasDirty)
    {
        m_DirtyRect = r;
        m_HasDirty = true;
        return;
    }

    // Birlesim (union)
    if (r.left   < m_DirtyRect.left)   m_DirtyRect.left   = r.left;
    if (r.top    < m_DirtyRect.top)    m_DirtyRect.top    = r.top;
    if (r.right  > m_DirtyRect.right)  m_DirtyRect.right  = r.right;
    if (r.bottom > m_DirtyRect.bottom) m_DirtyRect.bottom = r.bottom;
}

void D3D9Backend::MarkDirtyFull()
{
    m_FullDirty = true;
    m_HasDirty  = true;  // Present check'i isDirtyTracked && !FullDirty
    // Rect set etmeye gerek yok — FullDirty kazanir
}

//-----------------------------------------------------------------------------
// BeginFrame / EndFrame (2026-05-16 Dalga 2 — M1)
//
// Frame penceresi statik state — ana thread'de tek instance Present cagirir.
// FL2.cpp (vs_ui_lib TU) DXLib include path'ine erisemiyor; bu yuzden bool
// IsFrameInProgress() free fn olarak da expose ediliyor (extern "C").
//-----------------------------------------------------------------------------
namespace
{
    static bool s_FrameInProgress = false;
}

// FL2.cpp tarafindaki frame DC cache temizleyici — vs_ui_lib include
// path'imizde olmadigi icin extern. EndFrame'den cagrilir.
extern "C" void Phoenix_FL2_FrameEnd_ReleaseCache();

void D3D9Backend::BeginFrame()
{
    s_FrameInProgress = true;
}

void D3D9Backend::EndFrame()
{
    // FL2 frame DC cache'i bosalt (varsa). Frame penceresi kapaniyor —
    // bir sonraki frame yeniden acquire.
    Phoenix_FL2_FrameEnd_ReleaseCache();
    s_FrameInProgress = false;
}

bool D3D9Backend::IsFrameInProgress()
{
    return s_FrameInProgress;
}

void D3D9Backend::FlipToGDISurface()
{
    // D3D9'da GDI compatibility yok (D3DFMT_X8R8G8B8 + DWM). DDraw7
    // FlipToGDISurface yalnizca fullscreen-fokus kaybinda GDI desktop'a
    // donmek icindi. Modern path bunu otomatik yapar — Phase 1'de no-op.
}

//-----------------------------------------------------------------------------
// Surface factory
//-----------------------------------------------------------------------------
IRenderSurface* D3D9Backend::CreateOffscreenSurface(int width, int height, DWORD flags)
{
    if (!m_pDevice)
        return NULL;

    // Phoenix offscreen surface'lari hep 16-bit R5G6B5 (sprite codec hedefi).
    D3DPOOL pool = (flags & SurfaceCreate_VideoMemory)
                    ? D3DPOOL_DEFAULT      // video memory
                    : D3DPOOL_SYSTEMMEM;   // CPU access hizli

    IDirect3DSurface9* pRaw = NULL;
    HRESULT hr = m_pDevice->CreateOffscreenPlainSurface(
        width, height, D3DFMT_R5G6B5, pool, &pRaw, NULL);
    if (FAILED(hr) || !pRaw)
        return NULL;

    return new D3D9Surface(pRaw, width, height, SurfaceFormat::R5G6B5, /*owned=*/true);
}

IRenderSurface* D3D9Backend::CreateTextureSurface(int width, int height, DWORD /*flags*/)
{
    if (!m_pDevice)
        return NULL;

    IDirect3DTexture9* pTex = NULL;
    HRESULT hr = m_pDevice->CreateTexture(width, height, 1, 0,
                                           D3DFMT_R5G6B5, D3DPOOL_MANAGED,
                                           &pTex, NULL);
    if (FAILED(hr) || !pTex)
        return NULL;

    IDirect3DSurface9* pLevel = NULL;
    hr = pTex->GetSurfaceLevel(0, &pLevel);
    if (FAILED(hr) || !pLevel)
    {
        pTex->Release();
        return NULL;
    }

    // Texture surface 'owned' degil — D3D9Surface release etmemeli, ama
    // texture'in kendisi de release edilmeli. Phase 1'de basit: surface
    // owns and Release iki refcount'u dustur. Texture proxy'ye yatirilmali
    // (Phase 4'te refcount manager). Simdilik texture'i Release ediyoruz
    // (surface bag aliyor refcount'a — yasari D3D9 internal):
    //
    // GetSurfaceLevel AddRef yapar; texture Release sonrasi surface yasiyor.
    pTex->Release();

    return new D3D9Surface(pLevel, width, height, SurfaceFormat::R5G6B5, /*owned=*/true);
}

IRenderSurface* D3D9Backend::GetBackBuffer()
{
    if (m_pCachedBack)
        return m_pCachedBack;
    if (!m_pDevice)
        return NULL;

    IDirect3DSurface9* pBack = NULL;
    HRESULT hr = m_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBack);
    if (FAILED(hr) || !pBack)
        return NULL;

    // X8R8G8B8 back buffer — sprite codec direkt hedef ALMAZ. Composition
    // buffer (R5G6B5 sysmem) hedef olur. Back buffer wrapper sadece advanced
    // call site'lar icin.
    m_pCachedBack = new D3D9Surface(pBack, m_Width, m_Height,
                                    SurfaceFormat::X8R8G8B8, /*owned=*/true);
    return m_pCachedBack;
}

IRenderSurface* D3D9Backend::GetPrimarySurface()
{
    // D3D9'da "primary surface" kavrami yok — front buffer Present yoluyla
    // sahnelenir. Caller'lar genelde back buffer'i istiyor. Phase 1'de
    // composition surface'i donduruyoruz (sprite codec hedefi).
    return m_pCompositionSurface;
}

//-----------------------------------------------------------------------------
// Display state
//-----------------------------------------------------------------------------
bool D3D9Backend::IsFullscreen() const
{
    return m_bFullscreen;
}

SurfaceFormat D3D9Backend::GetPrimaryFormat() const
{
    // Sprite codec icin hep R5G6B5 — composition buffer formati.
    // (Back buffer X8R8G8B8 ama sprite codec onu gormeyecek.)
    return SurfaceFormat::R5G6B5;
}

//-----------------------------------------------------------------------------
// Gamma
//-----------------------------------------------------------------------------
bool D3D9Backend::IsGammaSupported() const
{
    // Fullscreen: hardware gamma ramp.
    // Windowed: D3D9 SetGammaRamp silent-ignore eder; Phoenix post-process
    // brightness fallback'i ayni slider'i gorunur hale getirir.
    return m_pDevice != NULL;
}

void D3D9Backend::CalculateGammaRamp()
{
    if (!m_pDevice)
        return;

    D3DGAMMARAMP ramp;
    ZeroMemory(&ramp, sizeof(ramp));

    int maxValue[3];
    int stepValue[3];

    for (int i = 0; i < 3; ++i)
    {
        // CDirectDraw::SetGammaRamp mantigi 1:1 — clamp [-100, 100]
        int rawStep = (int)(short)m_AddGammaStep[i];  // signed yorumla
        if (rawStep < -100) rawStep = -100;
        if (rawStep >  100) rawStep =  100;

        int step;
        if (rawStep > 0)
            step = (int)m_GammaStep + (200 - (int)m_GammaStep) * rawStep / 100;
        else
            step = (int)m_GammaStep + (int)m_GammaStep * rawStep / 100;

        if (step < 100)
        {
            maxValue[i]  = 0;
            stepValue[i] = 100 - step;
        }
        else
        {
            maxValue[i]  = 0xFFFF;
            stepValue[i] = step - 100;
        }
    }

    for (int i = 0; i < 256; ++i)
    {
        WORD r = m_GammaRamp.red[i];
        WORD g = m_GammaRamp.green[i];
        WORD b = m_GammaRamp.blue[i];

        ramp.red[i]   = (WORD)(r + (maxValue[0] - r) * stepValue[0] / 100);
        ramp.green[i] = (WORD)(g + (maxValue[1] - g) * stepValue[1] / 100);
        ramp.blue[i]  = (WORD)(b + (maxValue[2] - b) * stepValue[2] / 100);
    }

    // D3D9 SetGammaRamp Fullscreen'de hardware ramp; Windowed'da ignore.
    // D3DSGR_NO_CALIBRATION — Phoenix raw degerleri istiyor.
    m_pDevice->SetGammaRamp(0, D3DSGR_NO_CALIBRATION, &ramp);
}

void D3D9Backend::SetGammaRamp(WORD step)
{
    if (step == 0xFFFF)
        step = m_GammaStep;
    else
        m_GammaStep = step;

    D3D9PostProcess_SetBrightnessStep(m_bFullscreen ? 100 : m_GammaStep);
    CalculateGammaRamp();
}

void D3D9Backend::RestoreGammaRamp()
{
    m_GammaStep = 100;
    m_AddGammaStep[0] = 0;
    m_AddGammaStep[1] = 0;
    m_AddGammaStep[2] = 0;
    D3D9PostProcess_SetBrightnessStep(100);
    CalculateGammaRamp();
}

void D3D9Backend::SetAddGammaRamp(WORD r, WORD g, WORD b)
{
    m_AddGammaStep[0] = r;
    m_AddGammaStep[1] = g;
    m_AddGammaStep[2] = b;
    CalculateGammaRamp();
}

//-----------------------------------------------------------------------------
// Window event
//-----------------------------------------------------------------------------
void D3D9Backend::OnMove()
{
    // D3D9 Present hWnd-bagimli; window-move handle gerekmiyor (Present
    // otomatik adapter offset hesaplar). DDraw7'de m_rcWindow + Blt rect
    // tracking lazimdi — D3D9'da no-op.
}

IRenderBackend* GetRenderBackend()
{
    // 2026-08-31 Legacy DX11 gecis P0: render_backend.cfg / env degiskeni
    // "dx11" secerse D3D11Backend; aksi halde D3D9 (varsayilan, sifir risk).
    // Secim process omru boyunca sabittir (IsD3D11Selected cache'ler).
    if (IsD3D11Selected())
        return &D3D11Backend::Instance();
    return &D3D9Backend::Instance();
}

} // namespace render
} // namespace phoenix

//-----------------------------------------------------------------------------
// Phoenix_D3D9_IsFrameInProgress (2026-05-16 Dalga 2 — M1)
//
// vs_ui_lib (FL2.cpp + Phoenix_D3D9TextDC.cpp) DXLib include path'ine
// erisemediginden namespace-public method'a dogrudan erisemez. Free fn
// wrapper — frame penceresi (BeginFrame..EndFrame) acik mi sorgulamasi.
// FL2 cache implementation'i bunu kullanarak per-frame GetDC tekrarlarini
// ortadan kaldirir.
//-----------------------------------------------------------------------------
extern "C" bool Phoenix_D3D9_IsFrameInProgress()
{
    return phoenix::render::D3D9Backend::IsFrameInProgress();
}
