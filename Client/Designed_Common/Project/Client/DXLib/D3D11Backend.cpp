//-----------------------------------------------------------------------------
// D3D11Backend.cpp
//
// Phoenix D3D11 native render backend — Legacy DX11 gecis P0 (2026-08-31).
//
// Mimari: D3D9Backend'in "CPU composition + GPU present" pattern'inin DX11
// portu. Sprite codec + tum CPU blit yollari dokunulmadan calisir:
//
//   Sprite codec -> DIB composition buffer (R5G6B5)
//   Present      -> Map(DISCARD) dynamic texture upload
//                -> fullscreen triangle (SV_VertexID, point sample)
//                -> IDXGISwapChain::Present (vsync — D3D9 INTERVAL_ONE paritesi)
//
// CPU blit implementasyonlari D3D9Surface'tan birebir port edildi (clip +
// bounds + colorkey + self-blit overlap semantigi ayni — Fix #11/#23 dahil).
//
// P0 bilinen eksikler (Dev_Log 2026-08-31):
//   - Vignette post-process henuz yok (D3D9PostProcess D3D9-only)
//   - Brightness/gamma: 2026-09-01'de eklendi — SetGammaRamp present-asamasi
//     carpani (PSMain g_Light.w / dx11gpu modulate renk), hardware ramp yok
//   - ImGui HUD item ikonlari text fallback (D3D9 texture yolu device NULL)
//-----------------------------------------------------------------------------

#include "D3D11Backend.h"
#include "D3D9Backend.h"      // BeginFrame/EndFrame frame-penceresi statikleri (FL2 DC cache)
#include "D3D11QuadPipeline.h" // Faz G0 — GPU quad pipeline + sprite texture cache
#include "D3D11SpriteCommands.h" // Faz G1 — dx11gpu sirali komut katmani
#include "../UserOption.h"       // 2026-09-01 — Option "Aydinlatma" (gamma) kayitli deger
#include "../crash_trace_tool.h"
#include "../Debug/PhoenixImGui.h"

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <emmintrin.h>        // SSE2 — 565 -> 8888 upload donusumu
#include <malloc.h>           // _aligned_malloc
#include <stdio.h>
#include <psapi.h>            // 2026-09-01 siyah ekran teshisi — modul dokumu
#pragma comment(lib, "psapi.lib")

namespace phoenix
{
namespace render
{

//-----------------------------------------------------------------------------
// 2026-09-02 O2 — Reborn Option runtime ayarlari (IRenderBackend.h bildirimleri).
// Global state: backend her frame okur; menu/startup istedigi an set eder.
//-----------------------------------------------------------------------------
static bool g_bVSyncEnabled     = true;  // default: mevcut davranis (interval 1)
static int  g_UserBrightnessPct = 50;    // 0..100, 50 = notr

void SetVSyncEnabled(bool bOn)       { g_bVSyncEnabled = bOn; }
bool IsVSyncEnabled()                { return g_bVSyncEnabled; }

void SetUserBrightness(int percent)
{
    if (percent < 0)   percent = 0;
    if (percent > 100) percent = 100;
    g_UserBrightnessPct = percent;
}
int GetUserBrightness()              { return g_UserBrightnessPct; }

//-----------------------------------------------------------------------------
// COM release helper
//-----------------------------------------------------------------------------
template <typename T>
static inline void SafeRelease(T*& p)
{
    if (p != NULL)
    {
        p->Release();
        p = NULL;
    }
}

//-----------------------------------------------------------------------------
// Yardimci: 16-bit R5G6B5 -> 32-bit A8R8G8B8 (D3D9Backend ile ayni formul).
// FillSurface/FillRect 32-bit format dallari icin.
//-----------------------------------------------------------------------------
static inline DWORD Expand565To8888_11(WORD pixel)
{
    DWORD r = ((pixel >> 11) & 0x1F);
    DWORD g = ((pixel >> 5)  & 0x3F);
    DWORD b = ( pixel        & 0x1F);
    r = (r << 3) | (r >> 2);
    g = (g << 2) | (g >> 4);
    b = (b << 3) | (b >> 2);
    return 0xFF000000 | (r << 16) | (g << 8) | b;
}

//-----------------------------------------------------------------------------
// SSE2: bir satir R5G6B5 -> B8G8R8A8 donusum (upload fallback yolu).
// dst DWORD dizisi (BGRA byte sirasi = 0xAARRGGBB little-endian DWORD).
//-----------------------------------------------------------------------------
static void ConvertRow565ToBGRA_SSE2(const WORD* pSrc, DWORD* pDst, int count)
{
    int x = 0;

    const __m128i maskR5 = _mm_set1_epi32(0x1F);
    const __m128i maskG6 = _mm_set1_epi32(0x3F);
    const __m128i alphaFF = _mm_set1_epi32((int)0xFF000000);
    const __m128i zero = _mm_setzero_si128();

    for (; x + 8 <= count; x += 8)
    {
        __m128i px = _mm_loadu_si128((const __m128i*)(pSrc + x));   // 8 x WORD

        // WORD -> DWORD lane genisletme (lo 4 + hi 4)
        __m128i lo = _mm_unpacklo_epi16(px, zero);
        __m128i hi = _mm_unpackhi_epi16(px, zero);

        for (int half = 0; half < 2; ++half)
        {
            __m128i v = (half == 0) ? lo : hi;

            __m128i r5 = _mm_and_si128(_mm_srli_epi32(v, 11), maskR5);
            __m128i g6 = _mm_and_si128(_mm_srli_epi32(v, 5),  maskG6);
            __m128i b5 = _mm_and_si128(v, maskR5);

            // 5/6 bit -> 8 bit replicate: (r<<3)|(r>>2), (g<<2)|(g>>4), (b<<3)|(b>>2)
            __m128i r8 = _mm_or_si128(_mm_slli_epi32(r5, 3), _mm_srli_epi32(r5, 2));
            __m128i g8 = _mm_or_si128(_mm_slli_epi32(g6, 2), _mm_srli_epi32(g6, 4));
            __m128i b8 = _mm_or_si128(_mm_slli_epi32(b5, 3), _mm_srli_epi32(b5, 2));

            // 0xFF000000 | r<<16 | g<<8 | b  (B8G8R8A8 bellek sirasi B,G,R,A)
            __m128i outPx = _mm_or_si128(alphaFF,
                             _mm_or_si128(_mm_slli_epi32(r8, 16),
                              _mm_or_si128(_mm_slli_epi32(g8, 8), b8)));

            _mm_storeu_si128((__m128i*)(pDst + x + half * 4), outPx);
        }
    }

    // Kuyruk — scalar
    for (; x < count; ++x)
        pDst[x] = Expand565To8888_11(pSrc[x]);
}

//-----------------------------------------------------------------------------
// Faz G1 — composition'a CPU yazan D3D11Surface yollari komut katmanina
// bolgeyi bildirir (dx11gpu sirali cizim). Composition disi surface'larda
// ve dx11gpu disindaki modlarda no-op (IsCompositionTarget false).
//-----------------------------------------------------------------------------
static inline void G1_MarkCompDirty(void* pBits, const RECT* pRect)
{
    D3D11SpriteCommands& sc = D3D11SpriteCommands::Instance();
    if (sc.IsCompositionTarget(pBits))
        sc.MarkDirty(pRect);
}

//-----------------------------------------------------------------------------
// D3D11Surface
//-----------------------------------------------------------------------------
D3D11Surface::D3D11Surface(int width, int height, SurfaceFormat format, bool useDIB)
    : m_pBits(NULL)
    , m_Pitch(0)
    , m_Width(width)
    , m_Height(height)
    , m_Format(format)
    , m_bDIB(useDIB)
    , m_hDIB(NULL)
    , m_hMemDC(NULL)
    , m_ClipLeft(0)
    , m_ClipTop(0)
    , m_ClipRight(width)
    , m_ClipBottom(height)
    , m_ColorKey(0)
    , m_bHasColorKey(false)
    , m_LockCount(0)
{
    AllocateStorage();
}

//-----------------------------------------------------------------------------
// Depo kurulumu — m_Width/m_Height/m_Format/m_bDIB uzerinden calisir.
// Ctor ve Reinit ortak yolu (2026-09-02 crash fix ile ayrildi).
//-----------------------------------------------------------------------------
void D3D11Surface::AllocateStorage()
{
    const int width  = m_Width;
    const int height = m_Height;
    const SurfaceFormat format = m_Format;
    const bool useDIB = m_bDIB;

    if (width <= 0 || height <= 0)
        return;

    const bool b16 = (format == SurfaceFormat::R5G6B5 ||
                      format == SurfaceFormat::X1R5G5B5 ||
                      format == SurfaceFormat::A1R5G5B5 ||
                      format == SurfaceFormat::A4R4G4B4);
    const int bpp = b16 ? 2 : 4;

    if (useDIB && b16 && format == SurfaceFormat::R5G6B5)
    {
        // GDI DIB section — 16bpp 565 bitfields, top-down. FL2/GDI_Text
        // dogrudan bu bellek uzerine TextOut yazar (composition = ekran).
        struct
        {
            BITMAPINFOHEADER bmiHeader;
            DWORD            bmiMasks[3];
        } bi;
        ZeroMemory(&bi, sizeof(bi));
        bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth       = width;
        bi.bmiHeader.biHeight      = -height;    // top-down
        bi.bmiHeader.biPlanes      = 1;
        bi.bmiHeader.biBitCount    = 16;
        bi.bmiHeader.biCompression = BI_BITFIELDS;
        bi.bmiMasks[0] = 0xF800;   // R
        bi.bmiMasks[1] = 0x07E0;   // G
        bi.bmiMasks[2] = 0x001F;   // B

        void* pDIBBits = NULL;
        m_hDIB = CreateDIBSection(NULL, (const BITMAPINFO*)&bi, DIB_RGB_COLORS,
                                  &pDIBBits, NULL, 0);
        if (m_hDIB != NULL && pDIBBits != NULL)
        {
            m_pBits = pDIBBits;
            // DIB satirlari DWORD hizali
            m_Pitch = ((width * 16 + 31) / 32) * 4;

            m_hMemDC = CreateCompatibleDC(NULL);
            if (m_hMemDC != NULL)
            {
                SelectObject(m_hMemDC, m_hDIB);
            }
            else
            {
                CT_Done("D3D11Surface: CreateCompatibleDC FAILED (%dx%d) — text DC yok", width, height);
            }
        }
        else
        {
            CT_Done("D3D11Surface: CreateDIBSection FAILED (%dx%d)", width, height);
        }
        return;
    }

    // Duz CPU bellegi — 16 byte hizali (SSE2 isleme yollari icin), pitch de
    // 16'ya yuvarlanir.
    m_bDIB = false;
    m_Pitch = (width * bpp + 15) & ~15;
    m_pBits = _aligned_malloc((size_t)m_Pitch * (size_t)height, 16);
    if (m_pBits != NULL)
    {
        ZeroMemory(m_pBits, (size_t)m_Pitch * (size_t)height);
    }
    else
    {
        CT_Done("D3D11Surface: _aligned_malloc FAILED (%dx%d pitch=%d)", width, height, m_Pitch);
    }
}

D3D11Surface::~D3D11Surface()
{
    ReleaseStorage();
}

void D3D11Surface::ReleaseStorage()
{
    if (m_bDIB)
    {
        if (m_hMemDC != NULL)
        {
            DeleteDC(m_hMemDC);
            m_hMemDC = NULL;
        }
        if (m_hDIB != NULL)
        {
            DeleteObject(m_hDIB);
            m_hDIB = NULL;
        }
        m_pBits = NULL;
    }
    else if (m_pBits != NULL)
    {
        _aligned_free(m_pBits);
        m_pBits = NULL;
    }
    m_Pitch = 0;
}

//-----------------------------------------------------------------------------
// 2026-09-02 crash fix — yerinde yeniden boyutlandirma (detay header'da).
// NOT: heap yolunda AllocateStorage m_bDIB'i false'a cekmisti; composition
// gibi DIB surface'larda m_bDIB true kalir, ayni tur depo yeniden kurulur.
//-----------------------------------------------------------------------------
bool D3D11Surface::Reinit(int width, int height)
{
    if (width <= 0 || height <= 0)
        return false;
    if (width == m_Width && height == m_Height && IsValid())
        return true;

    ReleaseStorage();

    m_Width      = width;
    m_Height     = height;
    m_ClipLeft   = 0;
    m_ClipTop    = 0;
    m_ClipRight  = width;
    m_ClipBottom = height;
    m_LockCount  = 0;

    AllocateStorage();

    CT_Done("D3D11Surface::Reinit %dx%d %s", width, height,
            IsValid() ? "OK" : "FAILED");
    return IsValid();
}

bool D3D11Surface::Lock(LockedRect& outRect, DWORD /*flags*/)
{
    if (m_pBits == NULL)
        return false;

    // CPU bellek — kilitleme yok, kalici pointer. Ref-count yalnizca
    // IsLocked() sorgulari + D3D9 Fix #11 davranis paritesi icin tutulur.
    outRect.pBits  = m_pBits;
    outRect.pitch  = m_Pitch;
    outRect.width  = m_Width;
    outRect.height = m_Height;
    outRect.format = m_Format;
    ++m_LockCount;
    return true;
}

bool D3D11Surface::Unlock()
{
    if (m_LockCount <= 0)
        return false;
    --m_LockCount;
    return true;
}

void D3D11Surface::SetClip(const RECT& rc)
{
    m_ClipLeft   = rc.left;
    m_ClipTop    = rc.top;
    m_ClipRight  = rc.right;
    m_ClipBottom = rc.bottom;
}

void D3D11Surface::SetClipNull()
{
    m_ClipLeft   = 0;
    m_ClipTop    = 0;
    m_ClipRight  = m_Width;
    m_ClipBottom = m_Height;
}

void D3D11Surface::SetTransparency(DWORD colorValue)
{
    m_ColorKey = colorValue;
    m_bHasColorKey = true;
}

//-----------------------------------------------------------------------------
// BltColorKey — D3D9Surface::BltColorKey birebir port (clip + bounds + 16-bit
// colorkey filtre).
//-----------------------------------------------------------------------------
void D3D11Surface::BltColorKey(POINT* pDestPoint, IRenderSurface* pSrc, RECT* pSrcRect)
{
    if (m_pBits == NULL || !pDestPoint || !pSrc || !pSrcRect)
        return;

    if (pDestPoint->x >= m_ClipRight || pDestPoint->y >= m_ClipBottom)
        return;

    // Faz G1 — dirty bolge (clip oncesi superset; MarkDirty ekrana kirpar)
    {
        RECT dr;
        dr.left   = pDestPoint->x;
        dr.top    = pDestPoint->y;
        dr.right  = pDestPoint->x + (pSrcRect->right - pSrcRect->left);
        dr.bottom = pDestPoint->y + (pSrcRect->bottom - pSrcRect->top);
        G1_MarkCompDirty(m_pBits, &dr);
    }

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

    D3D11Surface* pSrcMem = static_cast<D3D11Surface*>(pSrc);

    // Source rect bounds clamp (OOB deref guard — D3D9 Round 3 sweep paritesi)
    if (pSrcRect->left < 0) pSrcRect->left = 0;
    if (pSrcRect->top  < 0) pSrcRect->top  = 0;
    if (pSrcRect->right  > pSrcMem->GetWidth())  pSrcRect->right  = pSrcMem->GetWidth();
    if (pSrcRect->bottom > pSrcMem->GetHeight()) pSrcRect->bottom = pSrcMem->GetHeight();
    if (pSrcRect->right <= pSrcRect->left || pSrcRect->bottom <= pSrcRect->top)
        return;

    if (pDestPoint->x < 0 || pDestPoint->y < 0)
        return;
    if (pDestPoint->x >= m_Width || pDestPoint->y >= m_Height)
        return;

    int maxCopyW = m_Width  - pDestPoint->x;
    int maxCopyH = m_Height - pDestPoint->y;
    if ((pSrcRect->right - pSrcRect->left) > maxCopyW)
        pSrcRect->right = pSrcRect->left + maxCopyW;
    if ((pSrcRect->bottom - pSrcRect->top) > maxCopyH)
        pSrcRect->bottom = pSrcRect->top + maxCopyH;
    if (pSrcRect->right <= pSrcRect->left || pSrcRect->bottom <= pSrcRect->top)
        return;

    LockedRect srcLock, dstLock;
    if (!pSrcMem->Lock(srcLock, LockFlag_ReadOnly))
        return;
    if (!Lock(dstLock, LockFlag_None))
    {
        pSrcMem->Unlock();
        return;
    }

    if (!srcLock.pBits || !dstLock.pBits || srcLock.pitch <= 0 || dstLock.pitch <= 0)
    {
        Unlock();
        pSrcMem->Unlock();
        return;
    }

    const int copyW = pSrcRect->right  - pSrcRect->left;
    const int copyH = pSrcRect->bottom - pSrcRect->top;
    const WORD colorKey = (WORD)(pSrcMem->HasColorKey()
                                  ? pSrcMem->GetColorKey()
                                  : m_ColorKey);

    if (m_Format == SurfaceFormat::R5G6B5 && pSrcMem->GetFormat() == SurfaceFormat::R5G6B5)
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

    Unlock();
    pSrcMem->Unlock();
}

//-----------------------------------------------------------------------------
// BltNoColorKey — D3D9Surface portu (Fix #23 self-blit overlap-aware dahil).
//-----------------------------------------------------------------------------
void D3D11Surface::BltNoColorKey(POINT* pDestPoint, IRenderSurface* pSrc, RECT* pSrcRect)
{
    if (m_pBits == NULL || !pDestPoint || !pSrc || !pSrcRect)
        return;

    if (pDestPoint->x >= m_ClipRight || pDestPoint->y >= m_ClipBottom)
        return;

    // Faz G1 — dirty bolge (clip oncesi superset)
    {
        RECT dr;
        dr.left   = pDestPoint->x;
        dr.top    = pDestPoint->y;
        dr.right  = pDestPoint->x + (pSrcRect->right - pSrcRect->left);
        dr.bottom = pDestPoint->y + (pSrcRect->bottom - pSrcRect->top);
        G1_MarkCompDirty(m_pBits, &dr);
    }

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

    D3D11Surface* pSrcMem = static_cast<D3D11Surface*>(pSrc);

    if (pSrcRect->left < 0) pSrcRect->left = 0;
    if (pSrcRect->top  < 0) pSrcRect->top  = 0;
    if (pSrcRect->right  > pSrcMem->GetWidth())  pSrcRect->right  = pSrcMem->GetWidth();
    if (pSrcRect->bottom > pSrcMem->GetHeight()) pSrcRect->bottom = pSrcMem->GetHeight();
    if (pSrcRect->right <= pSrcRect->left || pSrcRect->bottom <= pSrcRect->top)
        return;

    if (pDestPoint->x < 0 || pDestPoint->y < 0)
        return;
    if (pDestPoint->x >= m_Width || pDestPoint->y >= m_Height)
        return;

    int maxCopyW = m_Width  - pDestPoint->x;
    int maxCopyH = m_Height - pDestPoint->y;
    if ((pSrcRect->right - pSrcRect->left) > maxCopyW)
        pSrcRect->right = pSrcRect->left + maxCopyW;
    if ((pSrcRect->bottom - pSrcRect->top) > maxCopyH)
        pSrcRect->bottom = pSrcRect->top + maxCopyH;
    if (pSrcRect->right <= pSrcRect->left || pSrcRect->bottom <= pSrcRect->top)
        return;

    LockedRect srcLock, dstLock;
    if (!pSrcMem->Lock(srcLock, LockFlag_ReadOnly))
        return;
    if (!Lock(dstLock, LockFlag_None))
    {
        pSrcMem->Unlock();
        return;
    }

    if (!srcLock.pBits || !dstLock.pBits || srcLock.pitch <= 0 || dstLock.pitch <= 0)
    {
        Unlock();
        pSrcMem->Unlock();
        return;
    }

    const int copyW = pSrcRect->right  - pSrcRect->left;
    const int copyH = pSrcRect->bottom - pSrcRect->top;

    if (m_Format == SurfaceFormat::R5G6B5 && pSrcMem->GetFormat() == SurfaceFormat::R5G6B5)
    {
        const BYTE* pSrcBase = (const BYTE*)srcLock.pBits
                              + pSrcRect->top * srcLock.pitch
                              + pSrcRect->left * 2;
        BYTE* pDstBase = (BYTE*)dstLock.pBits
                       + pDestPoint->y * dstLock.pitch
                       + pDestPoint->x * 2;

        const int bytesPerRow = copyW * 2;

        // Fix #23 paritesi: self-blit scroll overlap — direction-aware satir
        // iterasyonu (dst > src ise backward).
        if (pDstBase > pSrcBase)
        {
            for (int y = copyH - 1; y >= 0; --y)
            {
                memmove(pDstBase + y * dstLock.pitch,
                        pSrcBase + y * srcLock.pitch,
                        bytesPerRow);
            }
        }
        else
        {
            for (int y = 0; y < copyH; ++y)
            {
                memmove(pDstBase + y * dstLock.pitch,
                        pSrcBase + y * srcLock.pitch,
                        bytesPerRow);
            }
        }
    }

    Unlock();
    pSrcMem->Unlock();
}

//-----------------------------------------------------------------------------
// BltStretchKey — D3D9Surface portu (nearest-neighbor stretch + colorkey).
//-----------------------------------------------------------------------------
void D3D11Surface::BltStretchKey(RECT* pDestRect, IRenderSurface* pSrc, RECT* pSrcRect)
{
    if (m_pBits == NULL || !pDestRect || !pSrc || !pSrcRect)
        return;

    G1_MarkCompDirty(m_pBits, pDestRect);   // Faz G1 — dirty bolge

    D3D11Surface* pSrcMem = static_cast<D3D11Surface*>(pSrc);

    if (pSrcRect->left < 0) pSrcRect->left = 0;
    if (pSrcRect->top  < 0) pSrcRect->top  = 0;
    if (pSrcRect->right  > pSrcMem->GetWidth())  pSrcRect->right  = pSrcMem->GetWidth();
    if (pSrcRect->bottom > pSrcMem->GetHeight()) pSrcRect->bottom = pSrcMem->GetHeight();
    if (pSrcRect->right <= pSrcRect->left || pSrcRect->bottom <= pSrcRect->top)
        return;

    if (pDestRect->left < 0) pDestRect->left = 0;
    if (pDestRect->top  < 0) pDestRect->top  = 0;
    if (pDestRect->right  > m_Width)  pDestRect->right  = m_Width;
    if (pDestRect->bottom > m_Height) pDestRect->bottom = m_Height;
    if (pDestRect->right <= pDestRect->left || pDestRect->bottom <= pDestRect->top)
        return;

    LockedRect srcLock, dstLock;
    if (!pSrcMem->Lock(srcLock, LockFlag_ReadOnly))
        return;
    if (!Lock(dstLock, LockFlag_None))
    {
        pSrcMem->Unlock();
        return;
    }

    if (!srcLock.pBits || !dstLock.pBits || srcLock.pitch <= 0 || dstLock.pitch <= 0)
    {
        Unlock();
        pSrcMem->Unlock();
        return;
    }

    const int srcW = pSrcRect->right  - pSrcRect->left;
    const int srcH = pSrcRect->bottom - pSrcRect->top;
    const int dstW = pDestRect->right  - pDestRect->left;
    const int dstH = pDestRect->bottom - pDestRect->top;

    const WORD colorKey = (WORD)(pSrcMem->HasColorKey()
                                  ? pSrcMem->GetColorKey()
                                  : m_ColorKey);

    if (dstW <= 0 || dstH <= 0 || srcW <= 0 || srcH <= 0)
    {
        Unlock();
        pSrcMem->Unlock();
        return;
    }

    if (m_Format == SurfaceFormat::R5G6B5 && pSrcMem->GetFormat() == SurfaceFormat::R5G6B5)
    {
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
    pSrcMem->Unlock();
}

void D3D11Surface::FillSurface(WORD color)
{
    if (m_pBits == NULL || m_Width <= 0 || m_Height <= 0 || m_Pitch <= 0)
        return;

    // 2026-09-02 cursor-iz fix — dx11gpu: composition tam-ekran fill'i
    // SIRALI komut olarak da kaydedilir (RecordClear: fullscreen opak quad
    // + PREV resetlemesi — detay D3D11SpriteCommands.h). Kayit basarili
    // olsa da CPU fill DEVAM eder: comp == prev tutarliligi ve raw
    // readback paritesi icin. Kayit yoksa eski yol (MarkDirty + fill).
    bool bClearRecorded = false;
    {
        D3D11SpriteCommands& sc = D3D11SpriteCommands::Instance();
        if (sc.IsCompositionTarget(m_pBits))
            bClearRecorded = sc.RecordClear(color);
    }
    if (!bClearRecorded)
        G1_MarkCompDirty(m_pBits, NULL);    // Faz G1 — tum ekran dirty

    if (m_Format == SurfaceFormat::R5G6B5 ||
        m_Format == SurfaceFormat::X1R5G5B5 ||
        m_Format == SurfaceFormat::A1R5G5B5 ||
        m_Format == SurfaceFormat::A4R4G4B4)
    {
        const int maxX = m_Pitch / 2;
        const int writeW = (m_Width < maxX) ? m_Width : maxX;
        for (int y = 0; y < m_Height; ++y)
        {
            WORD* pRow = (WORD*)((BYTE*)m_pBits + y * m_Pitch);
            for (int x = 0; x < writeW; ++x)
                pRow[x] = color;
        }
    }
    else if (m_Format == SurfaceFormat::X8R8G8B8 || m_Format == SurfaceFormat::A8R8G8B8)
    {
        const DWORD c32 = Expand565To8888_11(color);
        const int maxX = m_Pitch / 4;
        const int writeW = (m_Width < maxX) ? m_Width : maxX;
        for (int y = 0; y < m_Height; ++y)
        {
            DWORD* pRow = (DWORD*)((BYTE*)m_pBits + y * m_Pitch);
            for (int x = 0; x < writeW; ++x)
                pRow[x] = c32;
        }
    }
}

void D3D11Surface::FillRect(RECT* pRect, WORD color)
{
    if (m_pBits == NULL || !pRect)
        return;

    G1_MarkCompDirty(m_pBits, pRect);       // Faz G1 — dirty bolge (superset)

    RECT rect = *pRect;
    if (rect.left   > m_ClipRight)  return;
    if (rect.top    > m_ClipBottom) return;
    if (rect.left   < m_ClipLeft)   rect.left   = m_ClipLeft;
    if (rect.right  > m_ClipRight)  rect.right  = m_ClipRight;
    if (rect.top    < m_ClipTop)    rect.top    = m_ClipTop;
    if (rect.bottom > m_ClipBottom) rect.bottom = m_ClipBottom;

    if (rect.right <= rect.left || rect.bottom <= rect.top)
        return;

    if (rect.left < 0)          rect.left   = 0;
    if (rect.top  < 0)          rect.top    = 0;
    if (rect.right  > m_Width)  rect.right  = m_Width;
    if (rect.bottom > m_Height) rect.bottom = m_Height;
    if (rect.right <= rect.left || rect.bottom <= rect.top)
        return;

    if (m_Format == SurfaceFormat::R5G6B5 ||
        m_Format == SurfaceFormat::X1R5G5B5 ||
        m_Format == SurfaceFormat::A1R5G5B5 ||
        m_Format == SurfaceFormat::A4R4G4B4)
    {
        const int maxX = m_Pitch / 2;
        const int safeRight = (rect.right < maxX) ? rect.right : maxX;
        for (int y = rect.top; y < rect.bottom; ++y)
        {
            WORD* pRow = (WORD*)((BYTE*)m_pBits + y * m_Pitch);
            for (int x = rect.left; x < safeRight; ++x)
                pRow[x] = color;
        }
    }
    else if (m_Format == SurfaceFormat::X8R8G8B8 || m_Format == SurfaceFormat::A8R8G8B8)
    {
        const DWORD c32 = Expand565To8888_11(color);
        const int maxX = m_Pitch / 4;
        const int safeRight = (rect.right < maxX) ? rect.right : maxX;
        for (int y = rect.top; y < rect.bottom; ++y)
        {
            DWORD* pRow = (DWORD*)((BYTE*)m_pBits + y * m_Pitch);
            for (int x = rect.left; x < safeRight; ++x)
                pRow[x] = c32;
        }
    }
}

bool D3D11Surface::Restore()
{
    // CPU bellek surface kaybolmaz.
    return true;
}

//-----------------------------------------------------------------------------
// D3D11Backend
//-----------------------------------------------------------------------------
D3D11Backend::D3D11Backend()
    : m_pDevice(NULL)
    , m_pContext(NULL)
    , m_pSwapChain(NULL)
    , m_pRTV(NULL)
    , m_pUploadTex(NULL)
    , m_pUploadSRV(NULL)
    , m_pSampler(NULL)
    , m_pVS(NULL)
    , m_pPS(NULL)
    , m_pRasterizer(NULL)
    , m_pBlend(NULL)
    , m_pDepthOff(NULL)
    , m_pWorldTex(NULL)
    , m_pWorldSRV(NULL)
    , m_pLightTex(NULL)
    , m_pLightSRV(NULL)
    , m_pSamplerLinear(NULL)
    , m_pPresentCB(NULL)
    , m_LightTexW(0)
    , m_LightTexH(0)
    , m_LightActive(false)
    , m_LightClipW(0)
    , m_LightClipH(0)
    , m_GammaStep(100)
    , m_Brightness(1.0f)
    , m_pCompositionSurface(NULL)
    , m_hWnd(NULL)
    , m_Width(0)
    , m_Height(0)
    , m_bFullscreen(false)
    , m_bBorderlessActive(false)
    , m_bTex565(false)
{
}

D3D11Backend::~D3D11Backend()
{
    Shutdown();
}

D3D11Backend& D3D11Backend::Instance()
{
    static D3D11Backend s_Instance;
    return s_Instance;
}

//-----------------------------------------------------------------------------
// Shader kaynagi — fullscreen triangle (vertex buffer'siz, SV_VertexID) +
// point-sample kopya. Composition 1:1 backbuffer'a esler.
//
// 2026-09-01 P1 — GPU isik filtresi:
//   t0 = final composition (F, UI dahil)
//   t1 = world snapshot   (W, filtre aninda alinan kopya — UI oncesi)
//   t2 = filter grid      (R8, 0..1 karartma; bilinear = CPU 8.8 muadili)
// Kural: F == W ise piksel filtreden sonra degismemis (world pikseli) —
// karartilir. F != W ise sonradan cizilmis (UI/text/cursor) — dokunulmaz.
// Bu, CPU yolundaki "filtre once uygulanir, UI ustune gelir" sirasinin
// birebir GPU karsiligidir. keep = 1 - alpha*(255/256) — CPU'daki
// (c*(256-alpha8)+128)>>8 formulunun float esdegeri.
//-----------------------------------------------------------------------------
static const char* s_ShaderSource =
    "Texture2D    g_Tex      : register(t0);\n"
    "Texture2D    g_WorldTex : register(t1);\n"
    "Texture2D    g_LightTex : register(t2);\n"
    "SamplerState g_Smp      : register(s0);\n"
    "SamplerState g_SmpLin   : register(s1);\n"
    "cbuffer PresentCB : register(b0)\n"
    "{\n"
    "    float4 g_Light; // x=active(0/1), y=clipW, z=clipH, w=brightness\n"
    "};\n"
    "struct VSOut { float4 pos : SV_Position; float2 uv : TEXCOORD0; };\n"
    "VSOut VSMain(uint id : SV_VertexID)\n"
    "{\n"
    "    VSOut o;\n"
    "    float2 uv = float2((id << 1) & 2, id & 2);\n"
    "    o.uv  = uv;\n"
    "    o.pos = float4(uv.x * 2.0 - 1.0, 1.0 - uv.y * 2.0, 0.0, 1.0);\n"
    "    return o;\n"
    "}\n"
    "float4 PSMain(VSOut i) : SV_Target\n"
    "{\n"
    "    float3 f = g_Tex.Sample(g_Smp, i.uv).rgb;\n"
    "    if (g_Light.x > 0.5)\n"
    "    {\n"
    "        float2 px = i.pos.xy;\n"
    "        if (px.x < g_Light.y && px.y < g_Light.z)\n"
    "        {\n"
    "            float3 w = g_WorldTex.Sample(g_Smp, i.uv).rgb;\n"
    "            if (all(f == w))\n"
    "            {\n"
    "                float a = g_LightTex.Sample(g_SmpLin, px / g_Light.yz).r;\n"
    "                f *= (1.0 - a * (255.0 / 256.0));\n"
    "            }\n"
    "        }\n"
    "    }\n"
    "    // Option 'Aydinlatma' brightness (1.0 = notr). CB bagli degilse\n"
    "    // w=0 okunur — sifir korumasi ekrani karartmamak icin sart.\n"
    "    if (g_Light.w > 0.0) f *= g_Light.w;\n"
    "    return float4(f, 1.0);\n"
    "}\n";

// ---------------------------------------------------------------------------
// 2026-09-01 siyah ekran teshisi (kural 9.2): D3D11CreateDeviceAndSwapChain
// tum denemelerde DXGI_ERROR_UNSUPPORTED donuyor ama ayni parametreler temiz
// bir x86 process'te calisiyor (tmp_deploy\_tmp_dx11_probe.c). Fark client
// process ortaminda — asagidaki matris failure aninda kosulur:
//   1. Device-only HARDWARE (swapchain'siz)  -> D3D11 mi DXGI mi ayrimi
//   2. Device-only WARP                      -> donanim/surucu ayrimi
//   3. DXGI factory + adapter dokumu         -> adapter gorunurlugu
//   4. Yuklu modul dokumu                    -> hook/overlay DLL avi
// Root cause bulununca cagri korunur (kural 4.1), sadece kosul disi kalir.
// ---------------------------------------------------------------------------
static void DiagnoseDeviceCreateFailure()
{
    ID3D11Device* pDev = NULL;
    ID3D11DeviceContext* pCtx = NULL;
    D3D_FEATURE_LEVEL fl = (D3D_FEATURE_LEVEL)0;

    HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
                                   NULL, 0, D3D11_SDK_VERSION, &pDev, &fl, &pCtx);
    CT_Done("DIAG dx11: device-only HARDWARE hr=0x%08X fl=0x%04X", hr, (unsigned)fl);
    if (pCtx) { pCtx->Release(); pCtx = NULL; }
    if (pDev) { pDev->Release(); pDev = NULL; }

    hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP, NULL, 0,
                           NULL, 0, D3D11_SDK_VERSION, &pDev, &fl, &pCtx);
    CT_Done("DIAG dx11: device-only WARP hr=0x%08X fl=0x%04X", hr, (unsigned)fl);
    if (pCtx) { pCtx->Release(); pCtx = NULL; }
    if (pDev) { pDev->Release(); pDev = NULL; }

    IDXGIFactory1* pFactory = NULL;
    hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory);
    CT_Done("DIAG dx11: CreateDXGIFactory1 hr=0x%08X", hr);
    if (pFactory != NULL)
    {
        for (UINT i = 0; i < 8; ++i)
        {
            IDXGIAdapter1* pAdapter = NULL;
            if (pFactory->EnumAdapters1(i, &pAdapter) != S_OK || pAdapter == NULL)
                break;
            DXGI_ADAPTER_DESC1 desc;
            ZeroMemory(&desc, sizeof(desc));
            pAdapter->GetDesc1(&desc);
            char name[128] = { 0 };
            WideCharToMultiByte(CP_ACP, 0, desc.Description, -1, name, sizeof(name) - 1, NULL, NULL);
            CT_Done("DIAG dx11: adapter %u '%s' flags=0x%X vidmem=%uMB",
                    i, name, desc.Flags, (unsigned)(desc.DedicatedVideoMemory >> 20));
            pAdapter->Release();
        }
        pFactory->Release();
    }

    HMODULE mods[512];
    DWORD needed = 0;
    if (EnumProcessModules(GetCurrentProcess(), mods, sizeof(mods), &needed))
    {
        const int count = (int)(needed / sizeof(HMODULE));
        for (int i = 0; i < count && i < 512; ++i)
        {
            char path[MAX_PATH] = { 0 };
            if (GetModuleFileNameA(mods[i], path, MAX_PATH) > 0)
            {
                // Sistem disi + supheli (hook/overlay) modulleri one cikarmak
                // yerine hepsini dokuyoruz — filtre analizi log uzerinden.
                CT_Done("DIAG dx11: module %02d %s", i, path);
            }
        }
    }
}

bool D3D11Backend::CreateDeviceResources()
{
    // 1. Device + swapchain
    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(scd));
    scd.BufferCount        = 2;
    scd.BufferDesc.Width   = m_Width;
    scd.BufferDesc.Height  = m_Height;
    scd.BufferDesc.Format  = DXGI_FORMAT_B8G8R8A8_UNORM;
    scd.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow       = m_hWnd;
    scd.SampleDesc.Count   = 1;
    scd.SampleDesc.Quality = 0;
    // Baslangicta windowed olustur; exclusive istegi SetFullscreenState ile
    // (DXGI onerilen akis). ALLOW_MODE_SWITCH: FS gecisinde display mode degisir.
    scd.Windowed           = TRUE;
    scd.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;   // Win10+
    scd.Flags              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    const D3D_FEATURE_LEVEL levels[] =
    {
        D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3
    };
    D3D_FEATURE_LEVEL flOut = D3D_FEATURE_LEVEL_11_0;

    UINT createFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

    // Deneme sirasi:
    //   1. FLIP_DISCARD (Win10+) + tam feature-level listesi
    //   2. Ayni swap-effect, 11_1'siz liste (eski runtime E_INVALIDARG doner)
    //   3-4. Legacy DISCARD swap-effect ile ayni ikili (Win8.1 oncesi)
    HRESULT hr = E_FAIL;
    for (int attempt = 0; attempt < 4; ++attempt)
    {
        const bool bLegacySwap = (attempt >= 2);
        const bool bSkip111    = (attempt % 2 == 1);

        scd.SwapEffect  = bLegacySwap ? DXGI_SWAP_EFFECT_DISCARD : DXGI_SWAP_EFFECT_FLIP_DISCARD;
        scd.BufferCount = bLegacySwap ? 1 : 2;

        hr = D3D11CreateDeviceAndSwapChain(
            NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createFlags,
            bSkip111 ? (levels + 1) : levels,
            bSkip111 ? (ARRAYSIZE(levels) - 1) : ARRAYSIZE(levels),
            D3D11_SDK_VERSION,
            &scd, &m_pSwapChain, &m_pDevice, &flOut, &m_pContext);

        if (SUCCEEDED(hr) && m_pDevice && m_pSwapChain)
            break;

        CT_Done("D3D11Backend: create attempt %d FAILED 0x%08X (legacySwap=%d skip111=%d)",
                attempt, hr, (int)bLegacySwap, (int)bSkip111);
    }
    if (FAILED(hr) || !m_pDevice || !m_pSwapChain)
    {
        CT_Done("D3D11Backend: D3D11CreateDeviceAndSwapChain FAILED 0x%08X", hr);
        DiagnoseDeviceCreateFailure();   // 2026-09-01 siyah ekran teshisi
        return false;
    }

    CT_Done("D3D11Backend: device OK featureLevel=0x%04X %dx%d",
            (unsigned)flOut, (int)m_Width, (int)m_Height);

    // 2. RTV
    ID3D11Texture2D* pBackTex = NULL;
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackTex);
    if (FAILED(hr) || !pBackTex)
    {
        CT_Done("D3D11Backend: GetBuffer FAILED 0x%08X", hr);
        return false;
    }
    hr = m_pDevice->CreateRenderTargetView(pBackTex, NULL, &m_pRTV);
    pBackTex->Release();
    if (FAILED(hr) || !m_pRTV)
    {
        CT_Done("D3D11Backend: CreateRenderTargetView FAILED 0x%08X", hr);
        return false;
    }

    // 3. Upload texture (565 veya BGRA fallback)
    if (!CreateUploadTexture())
        return false;

    // 4. Shaderlar — runtime D3DCompile (d3dcompiler_47, Win8+ inbox)
    ID3DBlob* pVSBlob = NULL;
    ID3DBlob* pPSBlob = NULL;
    ID3DBlob* pErr = NULL;

    hr = D3DCompile(s_ShaderSource, strlen(s_ShaderSource), "phoenix_dx11_present",
                    NULL, NULL, "VSMain", "vs_4_0", 0, 0, &pVSBlob, &pErr);
    if (FAILED(hr))
    {
        CT_Done("D3D11Backend: VS compile FAILED 0x%08X (%s)", hr,
                pErr ? (const char*)pErr->GetBufferPointer() : "?");
        SafeRelease(pErr);
        return false;
    }
    SafeRelease(pErr);

    hr = D3DCompile(s_ShaderSource, strlen(s_ShaderSource), "phoenix_dx11_present",
                    NULL, NULL, "PSMain", "ps_4_0", 0, 0, &pPSBlob, &pErr);
    if (FAILED(hr))
    {
        CT_Done("D3D11Backend: PS compile FAILED 0x%08X (%s)", hr,
                pErr ? (const char*)pErr->GetBufferPointer() : "?");
        SafeRelease(pErr);
        SafeRelease(pVSBlob);
        return false;
    }
    SafeRelease(pErr);

    hr = m_pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
                                       pVSBlob->GetBufferSize(), NULL, &m_pVS);
    if (SUCCEEDED(hr))
        hr = m_pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
                                          pPSBlob->GetBufferSize(), NULL, &m_pPS);
    SafeRelease(pVSBlob);
    SafeRelease(pPSBlob);
    if (FAILED(hr) || !m_pVS || !m_pPS)
    {
        CT_Done("D3D11Backend: shader create FAILED 0x%08X", hr);
        return false;
    }

    // 5. Sampler (point + clamp) + raster/blend/depth state
    D3D11_SAMPLER_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.Filter   = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.MaxLOD   = D3D11_FLOAT32_MAX;
    hr = m_pDevice->CreateSamplerState(&sd, &m_pSampler);
    if (FAILED(hr)) { CT_Done("D3D11Backend: sampler FAILED 0x%08X", hr); return false; }

    D3D11_RASTERIZER_DESC rd;
    ZeroMemory(&rd, sizeof(rd));
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;
    rd.DepthClipEnable = TRUE;
    hr = m_pDevice->CreateRasterizerState(&rd, &m_pRasterizer);
    if (FAILED(hr)) { CT_Done("D3D11Backend: rasterizer FAILED 0x%08X", hr); return false; }

    D3D11_BLEND_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.RenderTarget[0].BlendEnable = FALSE;
    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = m_pDevice->CreateBlendState(&bd, &m_pBlend);
    if (FAILED(hr)) { CT_Done("D3D11Backend: blend FAILED 0x%08X", hr); return false; }

    D3D11_DEPTH_STENCIL_DESC dd;
    ZeroMemory(&dd, sizeof(dd));
    dd.DepthEnable = FALSE;
    dd.StencilEnable = FALSE;
    hr = m_pDevice->CreateDepthStencilState(&dd, &m_pDepthOff);
    if (FAILED(hr)) { CT_Done("D3D11Backend: depth state FAILED 0x%08X", hr); return false; }

    // 6. 2026-09-01 P1 — GPU isik filtresi: linear sampler (grid bilinear
    // upsample, CPU 8.8 sabit-nokta interpolasyonun muadili) + present CB.
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    hr = m_pDevice->CreateSamplerState(&sd, &m_pSamplerLinear);
    if (FAILED(hr)) { CT_Done("D3D11Backend: linear sampler FAILED 0x%08X", hr); return false; }

    D3D11_BUFFER_DESC cbd;
    ZeroMemory(&cbd, sizeof(cbd));
    cbd.ByteWidth      = 16;   // float4 g_Light
    cbd.Usage          = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = m_pDevice->CreateBuffer(&cbd, NULL, &m_pPresentCB);
    if (FAILED(hr)) { CT_Done("D3D11Backend: present CB FAILED 0x%08X", hr); return false; }

    return true;
}

bool D3D11Backend::CreateUploadTexture()
{
    // Once B5G6R5 dene (565 direkt kopya — donusum maliyeti sifir).
    // 16-bit DXGI formatlari D3D11.1'de opsiyonel; CheckFormatSupport sart.
    m_bTex565 = false;
    UINT support = 0;
    HRESULT hr = m_pDevice->CheckFormatSupport(DXGI_FORMAT_B5G6R5_UNORM, &support);
    const UINT need = D3D11_FORMAT_SUPPORT_TEXTURE2D | D3D11_FORMAT_SUPPORT_SHADER_SAMPLE;
    if (SUCCEEDED(hr) && (support & need) == need)
        m_bTex565 = true;

    D3D11_TEXTURE2D_DESC td;
    ZeroMemory(&td, sizeof(td));
    td.Width          = m_Width;
    td.Height         = m_Height;
    td.MipLevels      = 1;
    td.ArraySize      = 1;
    td.Format         = m_bTex565 ? DXGI_FORMAT_B5G6R5_UNORM : DXGI_FORMAT_B8G8R8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage          = D3D11_USAGE_DYNAMIC;
    td.BindFlags      = D3D11_BIND_SHADER_RESOURCE;
    td.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = m_pDevice->CreateTexture2D(&td, NULL, &m_pUploadTex);
    if (FAILED(hr) && m_bTex565)
    {
        // 565 destegi teoride var ama dynamic-usage reddedildi — BGRA fallback
        CT_Done("D3D11Backend: 565 dynamic tex FAILED 0x%08X — BGRA fallback", hr);
        m_bTex565 = false;
        td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        hr = m_pDevice->CreateTexture2D(&td, NULL, &m_pUploadTex);
    }
    if (FAILED(hr) || !m_pUploadTex)
    {
        CT_Done("D3D11Backend: upload texture FAILED 0x%08X", hr);
        return false;
    }

    hr = m_pDevice->CreateShaderResourceView(m_pUploadTex, NULL, &m_pUploadSRV);
    if (FAILED(hr) || !m_pUploadSRV)
    {
        CT_Done("D3D11Backend: upload SRV FAILED 0x%08X", hr);
        return false;
    }

    // 2026-09-01 P1: world snapshot texture — upload tex ile AYNI format/boyut.
    // Ayni donusum yolundan gectigi icin PS'teki F==W karsilastirmasi
    // degismemis piksellerde bit-kesin esittir. Olusturulamazsa GPU filtre
    // devre disi kalir (SubmitLightFilter false doner, CPU fallback calisir).
    hr = m_pDevice->CreateTexture2D(&td, NULL, &m_pWorldTex);
    if (SUCCEEDED(hr) && m_pWorldTex)
    {
        hr = m_pDevice->CreateShaderResourceView(m_pWorldTex, NULL, &m_pWorldSRV);
        if (FAILED(hr) || !m_pWorldSRV)
        {
            CT_Done("D3D11Backend: world SRV FAILED 0x%08X — GPU lightfilter kapali", hr);
            SafeRelease(m_pWorldSRV);
            SafeRelease(m_pWorldTex);
        }
    }
    else
    {
        CT_Done("D3D11Backend: world texture FAILED 0x%08X — GPU lightfilter kapali", hr);
        m_pWorldTex = NULL;
    }

    CT_Done("D3D11Backend: upload texture OK format=%s worldTex=%d",
            m_bTex565 ? "B5G6R5 (direct)" : "B8G8R8A8 (SSE2 convert)",
            m_pWorldTex ? 1 : 0);
    return true;
}

void D3D11Backend::ReleaseDeviceResources()
{
    if (m_pContext)
        m_pContext->ClearState();

    SafeRelease(m_pDepthOff);
    SafeRelease(m_pBlend);
    SafeRelease(m_pRasterizer);
    SafeRelease(m_pPS);
    SafeRelease(m_pVS);
    SafeRelease(m_pSampler);
    SafeRelease(m_pUploadSRV);
    SafeRelease(m_pUploadTex);

    // 2026-09-01 P1 — GPU isik filtresi kaynaklari
    SafeRelease(m_pPresentCB);
    SafeRelease(m_pSamplerLinear);
    SafeRelease(m_pLightSRV);
    SafeRelease(m_pLightTex);
    SafeRelease(m_pWorldSRV);
    SafeRelease(m_pWorldTex);
    m_LightTexW = 0;
    m_LightTexH = 0;
    m_LightActive = false;

    SafeRelease(m_pRTV);

    if (m_pSwapChain)
    {
        // DXGI kural: release oncesi windowed'a don (exclusive FS kilidi kalmasin)
        m_pSwapChain->SetFullscreenState(FALSE, NULL);
        SafeRelease(m_pSwapChain);
    }
    SafeRelease(m_pContext);
    SafeRelease(m_pDevice);
}

bool D3D11Backend::Init(HWND hWnd, WORD width, WORD height, ScreenMode mode, bool /*use3D*/, bool /*useIME*/)
{
    if (m_pDevice)
        return true;   // re-init caller Shutdown cagirmali (D3D9 paritesi)

    // 2026-09-01 siyah ekran teshisi — entry breadcrumb (kural 9.5)
    CT_Done("D3D11Backend::Init entry hwnd=%p %dx%d mode=%d gpuPath=%d",
            (void*)hWnd, (int)width, (int)height, (int)mode,
            IsD3D11GpuPathSelected() ? 1 : 0);

    m_hWnd        = hWnd;
    m_Width       = width;
    m_Height      = height;
    m_bFullscreen = (mode == ScreenMode::Fullscreen);

    if (!CreateDeviceResources())
    {
        CT_Done("D3D11Backend::Init CreateDeviceResources FAILED");
        Shutdown();
        return false;
    }

    // Composition buffer — DIB section (GDI text DC dahil)
    m_pCompositionSurface = new D3D11Surface(m_Width, m_Height,
                                             SurfaceFormat::R5G6B5, /*useDIB=*/true);
    if (!m_pCompositionSurface->IsValid())
    {
        CT_Done("D3D11Backend::Init composition DIB FAILED");
        Shutdown();
        return false;
    }

    // Fullscreen istegi — 2026-09-02 fullscreen fix (kullanici raporu):
    // Eski yol DXGI exclusive SetFullscreenState(TRUE) idi; modern Windows'ta
    // init aninda DXGI_ERROR_NOT_CURRENTLY_AVAILABLE (0x887A0022) ile
    // reddediliyor ve sessizce windowed kaliyordu (crash_trace kaniti).
    // Reborn referansi (Application.cpp ApplyWindowMode) exclusive hic
    // kullanmaz: ChangeDisplaySettings(CDS_FULLSCREEN) + WS_POPUP borderless.
    // Ayni pariteye gecildi — swapchain windowed kalir, mode switch display
    // seviyesinde yapilir (basarisiz olamayacak kadar basit yol).
    if (m_bFullscreen && !ApplyBorderlessFullscreen(true))
    {
        CT_Done("D3D11Backend::Init borderless fullscreen FAILED — windowed devam");
        m_bFullscreen = false;
    }

    CT_Done("D3D11Backend::Init OK %dx%d %s",
            (int)m_Width, (int)m_Height, m_bFullscreen ? "fullscreen" : "windowed");

    // 2026-09-01 — kayitli Option "Aydinlatma" degerini uygula (D3D9Backend
    // Init'teki SetBrightnessStep paritesi). g_pUserOption backend'den once
    // yuklenir; yoksa notr kalir.
    if (g_pUserOption != NULL && g_pUserOption->UseGammaControl)
        SetGammaRamp((WORD)g_pUserOption->GammaValue);

    // ImGui — DX11 impl (admin menu + HUD'lar)
    PhoenixImGui::InitializeDX11(hWnd, m_pDevice, m_pContext);

    // FULL DX11 gecis Faz G0 (2026-09-01): dx11gpu modunda quad pipeline +
    // sprite texture cache kurulur. Kurulamazsa GPU sprite yolu kapali
    // kalir (G1 komut katmani CPU composition'a duser) — davranis kaybi yok.
    if (IsD3D11GpuPathSelected())
    {
        if (!D3D11QuadPipeline::Instance().Init(m_pDevice, m_pContext))
        {
            CT_Done("D3D11Backend::Init QuadPipeline FAILED — GPU sprite yolu kapali");
        }
        else
        {
            D3D11SpriteTextureCache::Instance().Init(m_pDevice, m_pContext);
            // Faz G1 — sirali komut katmani (composition DIB bilgisiyle)
            D3D11SpriteCommands::Instance().Init(
                m_pDevice, m_pContext,
                m_pCompositionSurface->GetPixels(),
                m_pCompositionSurface->GetPitchBytes(),
                m_Width, m_Height);
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// 2026-09-02 O3 — canli cozunurluk degisimi.
// Device/shader/sampler korunur; boyuta bagli kaynaklar (swapchain backbuffer
// RTV, upload/world tex, composition DIB, dx11gpu accum/segment katmani)
// yeni boyutla yeniden kurulur.
//-----------------------------------------------------------------------------
bool D3D11Backend::Resize(WORD width, WORD height)
{
    if (!m_pDevice || !m_pSwapChain || width == 0 || height == 0)
        return false;
    if (width == m_Width && height == m_Height)
        return true;

    CT_Done("D3D11Backend::Resize %dx%d -> %dx%d",
            (int)m_Width, (int)m_Height, (int)width, (int)height);

    // 1. Tam-ekran boyutuna bagli GPU kaynaklarini birak.
    //    SpriteCommands accum/baseline/segment'leri tamamen kapanir (asagida
    //    yeni boyutla Init edilir); QuadPipeline + texture cache boyut
    //    bagimsiz, dokunulmaz.
    const bool bGpuPath = IsD3D11GpuPathSelected() && D3D11QuadPipeline::Instance().IsReady();
    if (bGpuPath)
        D3D11SpriteCommands::Instance().Shutdown();

    m_pContext->OMSetRenderTargets(0, NULL, NULL);
    m_pContext->ClearState();
    SafeRelease(m_pRTV);
    SafeRelease(m_pUploadSRV);
    SafeRelease(m_pUploadTex);
    SafeRelease(m_pWorldSRV);
    SafeRelease(m_pWorldTex);
    m_LightActive = false;

    // 2. Swapchain backbuffer yeni boyut.
    HRESULT hr = m_pSwapChain->ResizeBuffers(0, width, height,
                                             DXGI_FORMAT_UNKNOWN,
                                             DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
    if (FAILED(hr))
    {
        CT_Done("D3D11Backend::Resize ResizeBuffers FAILED 0x%08X", hr);
        return false;
    }

    m_Width  = width;
    m_Height = height;

    // 3. RTV + upload/world texture'lar (CreateUploadTexture m_Width kullanir).
    ID3D11Texture2D* pBackTex = NULL;
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackTex);
    if (SUCCEEDED(hr) && pBackTex)
    {
        hr = m_pDevice->CreateRenderTargetView(pBackTex, NULL, &m_pRTV);
        pBackTex->Release();
    }
    if (FAILED(hr) || !m_pRTV)
    {
        CT_Done("D3D11Backend::Resize RTV FAILED 0x%08X", hr);
        return false;
    }
    if (!CreateUploadTexture())
    {
        CT_Done("D3D11Backend::Resize CreateUploadTexture FAILED");
        return false;
    }

    // 4. Composition DIB — YERINDE yeniden boyutlandirilir (2026-09-02 crash
    //    fix). ESKI KOD delete/new yapiyordu; "tuketiciler pointer'i tazeler"
    //    varsayimi YANLISTI: InitOffsurface/InitBacksurface "Yol 1" wrapper'lari
    //    (g_pBack, g_pLast, VS_UI Base) singleton pointer'ini m_pRenderSurface
    //    olarak SAKLAR — delete sonrasi dangling delegate ile ilk cizim AV
    //    (CWaitUIUpdate::UpdateDraw g_pLast->FillSurface, dump p16364).
    //    Reinit obje kimligini korur; wrapper'lar gecerli kalir.
    if (m_pCompositionSurface == NULL ||
        !m_pCompositionSurface->Reinit(m_Width, m_Height))
    {
        CT_Done("D3D11Backend::Resize composition DIB Reinit FAILED");
        return false;
    }

    // 5. dx11gpu komut katmani yeni composition bilgisiyle geri gelir.
    if (bGpuPath)
    {
        D3D11SpriteCommands::Instance().Init(
            m_pDevice, m_pContext,
            m_pCompositionSurface->GetPixels(),
            m_pCompositionSurface->GetPitchBytes(),
            m_Width, m_Height);
    }

    // 6. Fullscreen aktifse display mode yeni cozunurluge tasinir
    //    (borderless CDS_FULLSCREEN — pencere de yeni boyuta oturur).
    if (m_bFullscreen)
        ApplyBorderlessFullscreen(true);

    CT_Done("D3D11Backend::Resize OK %dx%d", (int)m_Width, (int)m_Height);
    return true;
}

//-----------------------------------------------------------------------------
// 2026-09-02 fullscreen fix — Reborn ApplyWindowMode paritesi
// (E:\DK Phoenix Reborn\Source\Platform\Application.cpp 26874).
// Fullscreen : display mode oyun cozunurlugune cekilir (CDS_FULLSCREEN,
//              gecici — OS cikista/minimize'da geri alir) + WS_POPUP topmost
//              pencere tum ekrani kaplar. DXGI exclusive KULLANILMAZ.
// Windowed   : display mode restore + overlapped pencere workarea ortasina
//              (Client.cpp InitApp windowed dali paritesi).
//-----------------------------------------------------------------------------
bool D3D11Backend::ApplyBorderlessFullscreen(bool bEnable)
{
    if (m_hWnd == NULL)
        return false;

    if (bEnable)
    {
        DEVMODE mode;
        ZeroMemory(&mode, sizeof(mode));
        mode.dmSize       = sizeof(mode);
        mode.dmPelsWidth  = m_Width;
        mode.dmPelsHeight = m_Height;
        mode.dmBitsPerPel = 32;
        mode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

        const LONG lRet = ChangeDisplaySettings(&mode, CDS_FULLSCREEN);
        if (lRet != DISP_CHANGE_SUCCESSFUL)
        {
            CT_Done("D3D11Backend::ApplyBorderlessFullscreen ON %dx%d FAILED lRet=%ld",
                    (int)m_Width, (int)m_Height, lRet);
            return false;
        }

        SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN);
        SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_APPWINDOW);
        SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, m_Width, m_Height,
                     SWP_FRAMECHANGED | SWP_SHOWWINDOW);
        m_bBorderlessActive = true;
        CT_Done("D3D11Backend::ApplyBorderlessFullscreen ON %dx%d OK",
                (int)m_Width, (int)m_Height);
        return true;
    }

    if (m_bBorderlessActive)
    {
        ChangeDisplaySettings(NULL, 0);   // kayitli desktop moduna don
        m_bBorderlessActive = false;
    }

    // InitApp windowed dali paritesi: overlapped (maximize kapali), client
    // alani oyun cozunurlugu, workarea ortasi.
    const DWORD style = (WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN)
                        & ~WS_MAXIMIZEBOX;
    SetWindowLongPtr(m_hWnd, GWL_STYLE, style);
    SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW);

    RECT rc = { 0, 0, (LONG)m_Width, (LONG)m_Height };
    AdjustWindowRectEx(&rc, style, FALSE, WS_EX_APPWINDOW);
    const int winW = rc.right - rc.left;
    const int winH = rc.bottom - rc.top;

    RECT rcWork = { 0, 0, (LONG)m_Width, (LONG)m_Height };
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);
    const int x = rcWork.left + ((rcWork.right - rcWork.left) - winW) / 2;
    const int y = rcWork.top  + ((rcWork.bottom - rcWork.top) - winH) / 2;

    SetWindowPos(m_hWnd, HWND_NOTOPMOST, x, y, winW, winH,
                 SWP_FRAMECHANGED | SWP_SHOWWINDOW);
    CT_Done("D3D11Backend::ApplyBorderlessFullscreen OFF -> windowed %dx%d", winW, winH);
    return true;
}

bool D3D11Backend::SetFullscreenMode(bool bFullscreen)
{
    if (m_pDevice == NULL || m_hWnd == NULL)
        return false;
    if (bFullscreen == m_bFullscreen)
        return true;

    if (!ApplyBorderlessFullscreen(bFullscreen))
        return false;

    m_bFullscreen = bFullscreen;
    CT_Done("D3D11Backend::SetFullscreenMode -> %s",
            bFullscreen ? "fullscreen" : "windowed");
    return true;
}

void D3D11Backend::Shutdown()
{
    PhoenixImGui::Shutdown();

    // Faz G0/G1 — GPU sprite altyapisi device'tan once birakilir
    D3D11SpriteCommands::Instance().Shutdown();
    D3D11SpriteTextureCache::Instance().Shutdown();
    D3D11QuadPipeline::Instance().Shutdown();

    ReleaseDeviceResources();

    if (m_pCompositionSurface)
    {
        delete m_pCompositionSurface;
        m_pCompositionSurface = NULL;
    }

    // Reborn shutdown paritesi (Application.cpp 27277): borderless fullscreen
    // aktifse display mode'u desktop'a geri al.
    if (m_bBorderlessActive)
    {
        ChangeDisplaySettings(NULL, 0);
        m_bBorderlessActive = false;
    }

    m_hWnd = NULL;
}

bool D3D11Backend::RestoreAll()
{
    // Device-removed/reset kurtarma: composition DIB CPU'da yasadigi icin
    // icerik kaybolmaz; yalnizca GPU objeleri yeniden kurulur.
    if (!m_hWnd)
        return false;

    // 2026-09-01 crash fix: RestoreAllSurfaces legacy'de "lost surface
    // restore" (cogu zaman no-op) demek — MTopView zone girisinde
    // (m_bFirstTileDraw) her seferinde cagirir. D3D9Backend::RestoreAll
    // CheckDeviceState S_OK ise erken doner; ayni saglik kontrolu burada
    // da sart. Yoksa saglikli device frame ORTASINDA yeniden yaratilir,
    // ImGui context'i tazelenir ve stale g_frameStarted ile Begin
    // g.Font=NULL uzerinden AV verir (dump: Client.exe.27528.dmp,
    // ImGui::SetCurrentFont+0x171).
    if (m_pDevice != NULL)
    {
        const HRESULT reason = m_pDevice->GetDeviceRemovedReason();
        if (reason == S_OK)
            return true;    // device saglikli — recreate gereksiz (D3D9 paritesi)
        CT_Done("D3D11Backend::RestoreAll device removed reason=0x%08X", reason);
    }

    CT_Done("D3D11Backend::RestoreAll — device recreate");

    PhoenixImGui::Shutdown();

    // Faz G0/G1 — eski device'a bagli GPU sprite objeleri once dusurulur
    D3D11SpriteCommands::Instance().Shutdown();
    D3D11SpriteTextureCache::Instance().Shutdown();
    D3D11QuadPipeline::Instance().Shutdown();

    ReleaseDeviceResources();

    if (!CreateDeviceResources())
    {
        CT_Done("D3D11Backend::RestoreAll CreateDeviceResources FAILED");
        return false;
    }

    // 2026-09-02 fullscreen fix: exclusive yerine borderless re-apply
    // (display mode + pencere stili device kaybinda bozulmus olabilir).
    if (m_bFullscreen)
        ApplyBorderlessFullscreen(true);

    PhoenixImGui::InitializeDX11(m_hWnd, m_pDevice, m_pContext);

    // Faz G0/G1 — yeni device ile GPU sprite altyapisini yeniden kur
    if (IsD3D11GpuPathSelected())
    {
        if (D3D11QuadPipeline::Instance().Init(m_pDevice, m_pContext))
        {
            D3D11SpriteTextureCache::Instance().Init(m_pDevice, m_pContext);
            if (m_pCompositionSurface != NULL)
            {
                D3D11SpriteCommands::Instance().Init(
                    m_pDevice, m_pContext,
                    m_pCompositionSurface->GetPixels(),
                    m_pCompositionSurface->GetPitchBytes(),
                    m_Width, m_Height);
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// UploadComposition — DIB -> dynamic texture. Map(DISCARD) tum frame yeniden
// yazilir (dirty-rect P1 optimizasyonu; D3D9'daki MarkDirty cagrilari zaten
// mevcut kod tabaninda kullanilmiyor).
//-----------------------------------------------------------------------------
void D3D11Backend::UploadComposition()
{
    UploadCompositionTo(m_pUploadTex);
}

//-----------------------------------------------------------------------------
// UploadCompositionTo — composition DIB'i verilen dynamic texture'a yazar.
// Hem F (her Present) hem W (isik filtresi snapshot) bu yoldan gecer; ayni
// donusum = degismemis piksellerde bit-kesin esitlik.
//-----------------------------------------------------------------------------
bool D3D11Backend::UploadCompositionTo(ID3D11Texture2D* pTex)
{
    if (!m_pContext || !pTex || !m_pCompositionSurface)
        return false;

    D3D11_MAPPED_SUBRESOURCE mapped;
    HRESULT hr = m_pContext->Map(pTex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    if (FAILED(hr) || !mapped.pData)
        return false;

    const BYTE* pSrc = (const BYTE*)m_pCompositionSurface->GetPixels();
    const int srcPitch = m_pCompositionSurface->GetPitchBytes();
    BYTE* pDst = (BYTE*)mapped.pData;

    if (m_bTex565)
    {
        const int rowBytes = m_Width * 2;
        for (int y = 0; y < m_Height; ++y)
        {
            memcpy(pDst + y * mapped.RowPitch, pSrc + y * srcPitch, rowBytes);
        }
    }
    else
    {
        for (int y = 0; y < m_Height; ++y)
        {
            ConvertRow565ToBGRA_SSE2((const WORD*)(pSrc + y * srcPitch),
                                     (DWORD*)(pDst + y * mapped.RowPitch),
                                     m_Width);
        }
    }

    m_pContext->Unmap(pTex, 0);
    return true;
}

//-----------------------------------------------------------------------------
// SubmitLightFilter — 2026-09-01 P1.
//
// CPU Phoenix_ApplyLightBufferFilter3DToSurface muadili. MTopView
// DrawLightFilter aninda cagirir (world cizimi tamam, UI henuz yok):
//   1. Grid bos mu tarama (CPU yolundaki anyRowAlpha erken cikisi paritesi)
//   2. Grid'i R8 texture'a yukle (4-bit nibble x17 -> 0..255)
//   3. Composition'in su anki halini world snapshot texture'ina yukle
//   4. m_LightActive=true — Present PS'i compare-darken uygular
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// DeactivateLightFilter — MTopView her frame world cizimi bitince cagirir
// (filtre guard'lari kosmadan once). P1: bayat karartmayi soner. Faz G5:
// dx11gpu modunda bu an world/UI siniridir — composition'daki CPU birikimi
// (isim yazilari, hook'suz yollar) fullscreen diff checkpoint'iyle segment'e
// cikar ki UI quad'larinin altinda katmanlansin.
//-----------------------------------------------------------------------------
void D3D11Backend::DeactivateLightFilter()
{
    m_LightActive = false;

    if (IsD3D11GpuPathSelected())
        D3D11SpriteCommands::Instance().RecordWorldBoundary();
}

bool D3D11Backend::SubmitLightFilter(const BYTE* cells4, int fw, int fh, int destW, int destH)
{
    // Faz G4 (2026-09-01) — dx11gpu modunda filtre SIRALI komut olarak
    // kaydedilir: bu ana kadarki world cizimi (baseline+segment+quad)
    // MULTIPLY quad'i ile karartilir, sonraki UI dokunulmaz. P1
    // compare-darken bu modda kullanilamaz (world pikselleri composition'da
    // degil). Kayit basarisizsa (segment butcesi/overflow) CPU apply YINE
    // bastirilir — composition'i karartmak segment diff'ini bozar; o frame
    // karartmasiz kalir (transient, bir sonraki frame'de duzelir).
    if (IsD3D11GpuPathSelected() && D3D11SpriteCommands::Instance().IsActive())
    {
        m_LightActive = false;
        D3D11SpriteCommands::Instance().RecordLightFilter(cells4, fw, fh,
                                                          destW, destH);
        return true;
    }

    if (!m_pDevice || !m_pContext || !m_pWorldTex || cells4 == NULL ||
        fw <= 0 || fh <= 0 || destW <= 0 || destH <= 0)
        return false;

    // 1. Bos grid erken cikisi — karartma yok, GPU yine devralmis sayilir
    // (CPU apply de ayni durumda hicbir pikseli degistirmeden doner).
    bool anyAlpha = false;
    const int cellCount = fw * fh;
    for (int i = 0; i < cellCount; ++i)
    {
        if (cells4[i] & 0x0F)
        {
            anyAlpha = true;
            break;
        }
    }
    if (!anyAlpha)
    {
        m_LightActive = false;
        return true;
    }

    // 2. Light texture (R8) — boyut degistiyse yeniden kur
    if (m_pLightTex == NULL || m_LightTexW != fw || m_LightTexH != fh)
    {
        SafeRelease(m_pLightSRV);
        SafeRelease(m_pLightTex);
        m_LightTexW = 0;
        m_LightTexH = 0;

        D3D11_TEXTURE2D_DESC td;
        ZeroMemory(&td, sizeof(td));
        td.Width          = fw;
        td.Height         = fh;
        td.MipLevels      = 1;
        td.ArraySize      = 1;
        td.Format         = DXGI_FORMAT_R8_UNORM;
        td.SampleDesc.Count = 1;
        td.Usage          = D3D11_USAGE_DYNAMIC;
        td.BindFlags      = D3D11_BIND_SHADER_RESOURCE;
        td.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        HRESULT hr = m_pDevice->CreateTexture2D(&td, NULL, &m_pLightTex);
        if (FAILED(hr) || !m_pLightTex)
        {
            CT_Done("D3D11Backend: light tex FAILED 0x%08X (%dx%d)", hr, fw, fh);
            m_pLightTex = NULL;
            return false;
        }
        hr = m_pDevice->CreateShaderResourceView(m_pLightTex, NULL, &m_pLightSRV);
        if (FAILED(hr) || !m_pLightSRV)
        {
            CT_Done("D3D11Backend: light SRV FAILED 0x%08X", hr);
            SafeRelease(m_pLightSRV);
            SafeRelease(m_pLightTex);
            return false;
        }
        m_LightTexW = fw;
        m_LightTexH = fh;
        CT_Done("D3D11Backend: light tex OK %dx%d", fw, fh);
    }

    // 3. Grid yukle — nibble (0-15) x17 = 0..255 (CPU yolundaki olcek ayni)
    D3D11_MAPPED_SUBRESOURCE mapped;
    HRESULT hr = m_pContext->Map(m_pLightTex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    if (FAILED(hr) || !mapped.pData)
        return false;

    for (int y = 0; y < fh; ++y)
    {
        const BYTE* pS = cells4 + y * fw;
        BYTE* pD = (BYTE*)mapped.pData + y * mapped.RowPitch;
        for (int x = 0; x < fw; ++x)
            pD[x] = (BYTE)((pS[x] & 0x0F) * 17);
    }
    m_pContext->Unmap(m_pLightTex, 0);

    // 4. World snapshot — composition'in filtre ANINDAKI hali
    if (!UploadCompositionTo(m_pWorldTex))
        return false;

    m_LightClipW = destW;
    m_LightClipH = destH;
    m_LightActive = true;
    return true;
}

void D3D11Backend::Present()
{
    if (!m_pDevice || !m_pSwapChain || !m_pCompositionSurface)
        return;

    // Minimize: render anlamsiz — D3D9 paritesi (Sleep(100) + skip)
    if (m_hWnd != NULL && IsIconic(m_hWnd))
    {
        // Faz G1 — atlanan frame'in komutlari birikmesin (cache eviction'i
        // ile bayatlayabilecek SRV referanslari da temizlenir)
        D3D11SpriteCommands::Instance().ResetFrame();
        Sleep(100);
        return;
    }

    // Frame penceresi — FL2 DC cache'in "yeni frame" sinyali (D3D9 M1 paritesi;
    // statikler backend-agnostik, D3D9Backend uzerinden paylasilir).
    D3D9Backend::BeginFrame();

    const FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    m_pContext->ClearRenderTargetView(m_pRTV, clearColor);

    // Faz G1 — dx11gpu modu: frame'i sirali komut katmani cizer (baseline +
    // segment'ler + GPU quad'lar). Basarisizsa asagidaki normal CPU
    // composition yoluna dusulur (dogruluk kaybi yok, o frame quad'siz).
    // O2 — kullanici Brightness slider'i (50 = notr) gamma modulate'ine carpilir
    const float fEffBrightness = m_Brightness * (0.5f + (float)GetUserBrightness() / 100.0f);

    bool bGpuFrameDone = false;
    if (IsD3D11GpuPathSelected())
        bGpuFrameDone = D3D11SpriteCommands::Instance().RenderFrame(m_pRTV, fEffBrightness);

    // 2026-09-01 siyah ekran teshisi — saniyede 1 breadcrumb (CT_DONE=1 iken)
    {
        static DWORD s_LastPresentTrace = 0;
        DWORD nowTrace = GetTickCount();
        if (nowTrace - s_LastPresentTrace > 1000)
        {
            s_LastPresentTrace = nowTrace;
            CT_Done("D3D11 Present: gpuFrame=%d quads=%d segs=%d overflow=%d",
                    bGpuFrameDone ? 1 : 0,
                    D3D11SpriteCommands::Instance().GetQuadCount(),
                    D3D11SpriteCommands::Instance().GetSegmentCount(),
                    D3D11SpriteCommands::Instance().WasOverflow() ? 1 : 0);
        }
    }

    if (!bGpuFrameDone)
    {
    // 1. Composition -> GPU upload
    UploadComposition();

    // 2. Fullscreen triangle
    m_pContext->OMSetRenderTargets(1, &m_pRTV, NULL);
    m_pContext->OMSetBlendState(m_pBlend, NULL, 0xFFFFFFFF);
    m_pContext->OMSetDepthStencilState(m_pDepthOff, 0);
    m_pContext->RSSetState(m_pRasterizer);

    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    vp.Width    = (FLOAT)m_Width;
    vp.Height   = (FLOAT)m_Height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    m_pContext->RSSetViewports(1, &vp);

    m_pContext->IASetInputLayout(NULL);
    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pContext->VSSetShader(m_pVS, NULL, 0);
    m_pContext->PSSetShader(m_pPS, NULL, 0);

    // 2026-09-01 P1 — GPU isik filtresi parametreleri (b0). Aktiflik icin
    // SRV'lerin de mevcut olmasi sart; degilse PS erken cikar (x=0).
    const bool bLight = m_LightActive && m_pWorldSRV != NULL && m_pLightSRV != NULL;
    if (m_pPresentCB != NULL)
    {
        D3D11_MAPPED_SUBRESOURCE cbMapped;
        if (SUCCEEDED(m_pContext->Map(m_pPresentCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbMapped)) &&
            cbMapped.pData != NULL)
        {
            float* pCB = (float*)cbMapped.pData;
            pCB[0] = bLight ? 1.0f : 0.0f;
            pCB[1] = (float)m_LightClipW;
            pCB[2] = (float)m_LightClipH;
            pCB[3] = fEffBrightness; // Option "Aydinlatma" x Brightness slider (1.0 = notr)
            m_pContext->Unmap(m_pPresentCB, 0);
        }
        m_pContext->PSSetConstantBuffers(0, 1, &m_pPresentCB);
    }

    ID3D11ShaderResourceView* srvs[3] = { m_pUploadSRV, m_pWorldSRV, m_pLightSRV };
    ID3D11SamplerState* samplers[2] = { m_pSampler, m_pSamplerLinear };
    m_pContext->PSSetShaderResources(0, 3, srvs);
    m_pContext->PSSetSamplers(0, 2, samplers);
    m_pContext->Draw(3, 0);

    // SRV'leri unbind et (swapchain flip-model RTV/SRV hazard uyarilarina karsi)
    ID3D11ShaderResourceView* nullSRVs[3] = { NULL, NULL, NULL };
    m_pContext->PSSetShaderResources(0, 3, nullSRVs);
    } // !bGpuFrameDone — normal CPU composition yolu sonu

    // 3. ImGui overlay (admin menu / HUD) — RTV hala bagli
    PhoenixImGui::Render();

    // 4. Present — O2: VSync opsiyonu (default ON = D3D9 INTERVAL_ONE paritesi,
    // tearing fix 2026-08-31). OFF ise interval 0; frame temposu pacer'a kalir.
    HRESULT hr = m_pSwapChain->Present(IsVSyncEnabled() ? 1 : 0, 0);
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        CT_Done("D3D11Backend::Present device removed/reset 0x%08X — RestoreAll", hr);
        RestoreAll();
    }
    // DXGI_STATUS_OCCLUDED: pencere gizli — frame atlanmis olabilir, akisa
    // devam (D3D9 S_PRESENT_OCCLUDED davranis paritesi).

    D3D9Backend::EndFrame();

    // Faz G1 — sonraki frame icin baseline/prev snapshot + komut listesi reset
    // (dx11gpu disinda no-op). RenderFrame fail etmis olsa da cagrilir.
    if (IsD3D11GpuPathSelected())
        D3D11SpriteCommands::Instance().ResetFrame();
}

void D3D11Backend::FlipToGDISurface()
{
    // DXGI'de kavram yok — no-op (D3D9 paritesi).
}

//-----------------------------------------------------------------------------
// Surface factory — hepsi CPU bellek surface'i
//-----------------------------------------------------------------------------
IRenderSurface* D3D11Backend::CreateOffscreenSurface(int width, int height, DWORD /*flags*/)
{
    if (width <= 0 || height <= 0)
        return NULL;

    D3D11Surface* pSurf = new D3D11Surface(width, height, SurfaceFormat::R5G6B5, /*useDIB=*/false);
    if (!pSurf->IsValid())
    {
        delete pSurf;
        return NULL;
    }
    return pSurf;
}

IRenderSurface* D3D11Backend::CreateTextureSurface(int width, int height, DWORD /*flags*/)
{
    // DX11 modunda texture surface'lar da CPU bellek — sprite codec CPU
    // uzerinden doldurur, CPU blit ile composition'a akar (D3D7 texture-draw
    // yollari D3D9 modunda oldugu gibi devre disi).
    return CreateOffscreenSurface(width, height, 0);
}

D3D11Surface* D3D11Backend::CreateMemorySurface(int width, int height, SurfaceFormat fmt)
{
    if (width <= 0 || height <= 0)
        return NULL;

    D3D11Surface* pSurf = new D3D11Surface(width, height, fmt, /*useDIB=*/false);
    if (!pSurf->IsValid())
    {
        delete pSurf;
        return NULL;
    }
    return pSurf;
}

IRenderSurface* D3D11Backend::GetBackBuffer()
{
    // D3D9'da 32-bit backbuffer wrapper'i "advanced call site'lar" icindi;
    // DX11 modunda composition surface tum cizimlerin hedefi.
    return m_pCompositionSurface;
}

IRenderSurface* D3D11Backend::GetPrimarySurface()
{
    return m_pCompositionSurface;
}

SurfaceFormat D3D11Backend::GetPrimaryFormat() const
{
    return SurfaceFormat::R5G6B5;
}

//-----------------------------------------------------------------------------
// Gamma — 2026-09-01: Option "Aydinlatma" slider'i icin present-asamasi
// brightness (D3D9 windowed post-process paritesi — D3D9PostProcess.cpp
// SetBrightnessStep ile ayni clamp 50..150, step/100 carpani).
//   - dx11 (CPU composition) yolu: PSMain'de f *= g_Light.w (PresentCB).
//   - dx11gpu yolu: RenderFrame accum->backbuffer kopyasinda modulate renk.
// Hardware ramp / DXGI SetGammaControl kullanilmiyor (yalniz exclusive
// fullscreen'de calisir; borderless/windowed'da shader yolu tek dogru yol).
// SetAddGammaRamp (ekran flash efektleri) D3D9-windowed'da da uygulanmiyordu;
// parite geregi burada da no-op kalir.
//-----------------------------------------------------------------------------
bool D3D11Backend::IsGammaSupported() const
{
    return true;
}

void D3D11Backend::SetGammaRamp(WORD step)
{
    if (step == 0xFFFF)
        step = m_GammaStep;        // sentinel: mevcut degeri yeniden uygula
    else
        m_GammaStep = step;

    int clamped = (int)m_GammaStep;
    if (clamped < 50)  clamped = 50;
    if (clamped > 150) clamped = 150;
    m_Brightness = (float)clamped / 100.0f;

    CT_Done("D3D11Backend::SetGammaRamp step=%u brightness=%d%%",
            (unsigned)m_GammaStep, clamped);
}

void D3D11Backend::RestoreGammaRamp()
{
    m_GammaStep  = 100;
    m_Brightness = 1.0f;
    CT_Done("D3D11Backend::RestoreGammaRamp — notr");
}

void D3D11Backend::SetAddGammaRamp(WORD /*r*/, WORD /*g*/, WORD /*b*/)
{
    // D3D9-windowed paritesi: per-kanal add-gamma yalniz hardware ramp'te
    // vardi; shader brightness yolunda uygulanmiyor (bilincli no-op).
}

void D3D11Backend::OnMove()
{
    // DXGI Present pencere konumunu otomatik izler — no-op (D3D9 paritesi).
}

//-----------------------------------------------------------------------------
// Backend secimi
//-----------------------------------------------------------------------------
bool IsD3D11Selected()
{
    static int s_Selected = -1;
    if (s_Selected < 0)
    {
        s_Selected = 0;

        // 1. Env degiskeni onceligi: PHOENIX_RENDER_BACKEND=dx11|dx11gpu|d3d9
        // Faz G0: "dx11gpu" da DX11 backend'i secer (GPU sprite yolu ayrica
        // IsD3D11GpuPathSelected ile acilir).
        char envVal[32] = { 0 };
        DWORD n = GetEnvironmentVariableA("PHOENIX_RENDER_BACKEND", envVal, sizeof(envVal));
        if (n > 0 && n < sizeof(envVal))
        {
            if (_stricmp(envVal, "dx11") == 0 || _stricmp(envVal, "dx11gpu") == 0)
                s_Selected = 1;
            CT_Done("Phoenix render backend (env): %s", s_Selected ? "D3D11" : "D3D9");
            return s_Selected == 1;
        }

        // 2. Client.exe yanindaki render_backend.cfg — ilk satir "dx11"
        char modPath[MAX_PATH] = { 0 };
        if (GetModuleFileNameA(NULL, modPath, MAX_PATH) > 0)
        {
            char* pSlash = strrchr(modPath, '\\');
            if (pSlash != NULL)
            {
                *(pSlash + 1) = '\0';
                char cfgPath[MAX_PATH];
                _snprintf(cfgPath, MAX_PATH - 1, "%srender_backend.cfg", modPath);
                cfgPath[MAX_PATH - 1] = '\0';

                FILE* f = fopen(cfgPath, "rt");
                if (f != NULL)
                {
                    char line[64] = { 0 };
                    if (fgets(line, sizeof(line), f) != NULL)
                    {
                        // trim sag whitespace
                        for (int i = (int)strlen(line) - 1;
                             i >= 0 && (line[i] == '\n' || line[i] == '\r' || line[i] == ' ' || line[i] == '\t');
                             --i)
                            line[i] = '\0';
                        if (_stricmp(line, "dx11") == 0 || _stricmp(line, "dx11gpu") == 0)
                            s_Selected = 1;
                    }
                    fclose(f);
                }
            }
        }

        CT_Done("Phoenix render backend: %s", s_Selected ? "D3D11 (render_backend.cfg)" : "D3D9 (varsayilan)");
    }
    return s_Selected == 1;
}

} // namespace render
} // namespace phoenix
