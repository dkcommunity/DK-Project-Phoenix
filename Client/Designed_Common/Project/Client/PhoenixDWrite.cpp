//-----------------------------------------------------------------------------
// PhoenixDWrite.cpp
//
// FULL DX11 gecis Faz T1 (2026-09-01) — DirectWrite text motoru.
// Mimari aciklama: PhoenixDWrite.h basligindaki yorum.
//
// Akis:
//   HDC state (HFONT/renk/align/bkmode)  — caller FL2 kurar, biz okuruz
//   LOGFONT → IDWriteTextFormat          — font cache (hash key)
//   string  → GdiCompatibleTextLayout    — GDI metrik paritesi
//   layout  → IDWriteBitmapRenderTarget  — grayscale AA rasterize (beyaz/siyah)
//   BRT bits→ 8bpp coverage mask         — mask cache (font+string key)
//   mask×renk → premult scratch DIB      — GdiAlphaBlend ile hedefe
//-----------------------------------------------------------------------------

#include "PhoenixDWrite.h"
#include "crash_trace_tool.h"

#include <dwrite.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <math.h>
#include <stdio.h>

#pragma comment(lib, "dwrite.lib")

namespace
{

//-----------------------------------------------------------------------------
// COM release helper
//-----------------------------------------------------------------------------
template <typename T>
inline void SafeRelease(T*& p)
{
    if (p != NULL)
    {
        p->Release();
        p = NULL;
    }
}

//-----------------------------------------------------------------------------
// Global durum
//-----------------------------------------------------------------------------
IDWriteFactory*         g_pFactory = NULL;
IDWriteGdiInterop*      g_pGdiInterop = NULL;
IDWriteRenderingParams* g_pRenderParams = NULL;   // grayscale AA (cleartype=0)
bool                    g_bInitTried = false;
bool                    g_bInitOK = false;

// Yeniden kullanilabilir bitmap render target (grow-only)
IDWriteBitmapRenderTarget* g_pBRT = NULL;
int  g_BRTWidth = 0;
int  g_BRTHeight = 0;

// Premultiplied scratch DIB (grow-only) + kalici memory DC
HDC     g_hScratchDC = NULL;
HBITMAP g_hScratchBmp = NULL;
void*   g_pScratchBits = NULL;
int     g_ScratchW = 0;
int     g_ScratchH = 0;

//-----------------------------------------------------------------------------
// Font cache — LOGFONTA hash → TextFormat + metrikler
//-----------------------------------------------------------------------------
struct FontEntry
{
    IDWriteTextFormat* pFormat;
    float emSize;
    float lineHeight;   // "Ag" layout satir yuksekligi
    float baseline;     // satir baseline (TA_BASELINE icin)
    UINT  codepage;     // MBCS → wide donusum codepage'i
    BOOL  underline;
    BOOL  strike;
};

std::unordered_map<unsigned long long, FontEntry> g_FontCache;

//-----------------------------------------------------------------------------
// Mask cache — (fontKey, string) → coverage bitmap
//-----------------------------------------------------------------------------
struct MaskEntry
{
    int w;
    int h;
    float baseline;
    std::vector<BYTE> mask;   // w*h coverage (0..255)
};

std::unordered_map<unsigned long long, MaskEntry> g_MaskCache;
size_t g_MaskCacheBytes = 0;

// Butce: asilinca cache komple temizlenir (LRU defteri tutmaya degmez —
// yeniden rasterize ucuz, sicak stringler aninda geri dolar).
const size_t kMaskCacheMaxBytes   = 12u * 1024u * 1024u;
const size_t kMaskCacheMaxEntries = 2048;

//-----------------------------------------------------------------------------
// FNV-1a hash
//-----------------------------------------------------------------------------
inline unsigned long long HashBytes(const void* pData, size_t len, unsigned long long seed)
{
    const unsigned char* p = (const unsigned char*)pData;
    unsigned long long h = seed ? seed : 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i)
    {
        h ^= p[i];
        h *= 1099511628211ULL;
    }
    return h;
}

//-----------------------------------------------------------------------------
// Charset → codepage (MBCS string'lerin wide donusumu icin)
//-----------------------------------------------------------------------------
UINT CharsetToCodepage(BYTE charset)
{
    switch (charset)
    {
    case TURKISH_CHARSET:    return 1254;
    case GB2312_CHARSET:     return 936;
    case SHIFTJIS_CHARSET:   return 932;
    case HANGUL_CHARSET:     return 949;
    case ANSI_CHARSET:       return 1252;
    case EASTEUROPE_CHARSET: return 1250;
    case RUSSIAN_CHARSET:    return 1251;
    default:                 return CP_ACP;
    }
}

//-----------------------------------------------------------------------------
// Config — text_engine.cfg / PHOENIX_TEXT_ENGINE ("dwrite" → acik).
// render_backend.cfg pattern'inin aynisi; ilk cagrida okunur, cache'lenir.
//-----------------------------------------------------------------------------
bool IsDWriteSelected()
{
    static int s_Selected = -1;
    if (s_Selected < 0)
    {
        s_Selected = 0;

        char envVal[32] = { 0 };
        DWORD n = GetEnvironmentVariableA("PHOENIX_TEXT_ENGINE", envVal, sizeof(envVal));
        if (n > 0 && n < sizeof(envVal))
        {
            if (_stricmp(envVal, "dwrite") == 0)
                s_Selected = 1;
            CT_Done("Phoenix text engine (env): %s", s_Selected ? "DWrite" : "GDI");
            return s_Selected == 1;
        }

        char modPath[MAX_PATH] = { 0 };
        if (GetModuleFileNameA(NULL, modPath, MAX_PATH) > 0)
        {
            char* pSlash = strrchr(modPath, '\\');
            if (pSlash != NULL)
            {
                *(pSlash + 1) = '\0';
                char cfgPath[MAX_PATH];
                _snprintf(cfgPath, MAX_PATH - 1, "%stext_engine.cfg", modPath);
                cfgPath[MAX_PATH - 1] = '\0';

                FILE* f = fopen(cfgPath, "rt");
                if (f != NULL)
                {
                    char line[64] = { 0 };
                    if (fgets(line, sizeof(line), f) != NULL)
                    {
                        for (int i = (int)strlen(line) - 1;
                             i >= 0 && (line[i] == '\n' || line[i] == '\r' || line[i] == ' ' || line[i] == '\t');
                             --i)
                            line[i] = '\0';
                        if (_stricmp(line, "dwrite") == 0)
                            s_Selected = 1;
                    }
                    fclose(f);
                }
            }
        }

        CT_Done("Phoenix text engine: %s", s_Selected ? "DWrite (text_engine.cfg)" : "GDI (varsayilan)");
    }
    return s_Selected == 1;
}

//-----------------------------------------------------------------------------
// PhoenixTextRenderer — IDWriteTextRenderer implementasyonu.
// Layout callback'lerini IDWriteBitmapRenderTarget'a beyaz renkle iletir
// (BRT onceden siyaha temizlenir; sonuc pikselin yesil kanali = coverage).
//-----------------------------------------------------------------------------
class PhoenixTextRenderer : public IDWriteTextRenderer
{
public:
    // Singleton — heap yonetimi yok, ref-count dummy
    static PhoenixTextRenderer& Instance()
    {
        static PhoenixTextRenderer s_Inst;
        return s_Inst;
    }

    // IUnknown
    STDMETHOD(QueryInterface)(REFIID riid, void** ppOut)
    {
        if (ppOut == NULL)
            return E_POINTER;
        if (riid == __uuidof(IDWriteTextRenderer) ||
            riid == __uuidof(IDWritePixelSnapping) ||
            riid == __uuidof(IUnknown))
        {
            *ppOut = this;
            return S_OK;
        }
        *ppOut = NULL;
        return E_NOINTERFACE;
    }
    STDMETHOD_(ULONG, AddRef)()  { return 1; }
    STDMETHOD_(ULONG, Release)() { return 1; }

    // IDWritePixelSnapping
    STDMETHOD(IsPixelSnappingDisabled)(void* /*ctx*/, BOOL* isDisabled)
    {
        *isDisabled = FALSE;
        return S_OK;
    }
    STDMETHOD(GetCurrentTransform)(void* /*ctx*/, DWRITE_MATRIX* transform)
    {
        transform->m11 = 1.0f; transform->m12 = 0.0f;
        transform->m21 = 0.0f; transform->m22 = 1.0f;
        transform->dx = 0.0f;  transform->dy = 0.0f;
        return S_OK;
    }
    STDMETHOD(GetPixelsPerDip)(void* /*ctx*/, FLOAT* pixelsPerDip)
    {
        *pixelsPerDip = 1.0f;
        return S_OK;
    }

    // IDWriteTextRenderer
    STDMETHOD(DrawGlyphRun)(void* /*ctx*/, FLOAT baselineX, FLOAT baselineY,
                            DWRITE_MEASURING_MODE measuringMode,
                            DWRITE_GLYPH_RUN const* glyphRun,
                            DWRITE_GLYPH_RUN_DESCRIPTION const* /*desc*/,
                            IUnknown* /*effect*/)
    {
        if (g_pBRT == NULL)
            return E_FAIL;
        return g_pBRT->DrawGlyphRun(baselineX, baselineY, measuringMode,
                                    glyphRun, g_pRenderParams,
                                    RGB(255, 255, 255), NULL);
    }

    STDMETHOD(DrawUnderline)(void* /*ctx*/, FLOAT originX, FLOAT originY,
                             DWRITE_UNDERLINE const* u, IUnknown* /*effect*/)
    {
        return FillWhiteRect(originX, originY + u->offset, u->width, u->thickness);
    }

    STDMETHOD(DrawStrikethrough)(void* /*ctx*/, FLOAT originX, FLOAT originY,
                                 DWRITE_STRIKETHROUGH const* s, IUnknown* /*effect*/)
    {
        return FillWhiteRect(originX, originY + s->offset, s->width, s->thickness);
    }

    STDMETHOD(DrawInlineObject)(void* /*ctx*/, FLOAT /*x*/, FLOAT /*y*/,
                                IDWriteInlineObject* /*obj*/, BOOL /*sideways*/,
                                BOOL /*rtl*/, IUnknown* /*effect*/)
    {
        return S_OK;   // inline object kullanilmiyor
    }

private:
    static HRESULT FillWhiteRect(FLOAT x, FLOAT y, FLOAT w, FLOAT h)
    {
        if (g_pBRT == NULL)
            return E_FAIL;
        HDC dc = g_pBRT->GetMemoryDC();
        if (dc == NULL)
            return E_FAIL;
        RECT rc;
        rc.left   = (LONG)floorf(x);
        rc.top    = (LONG)floorf(y);
        rc.right  = (LONG)ceilf(x + w);
        rc.bottom = (LONG)ceilf(y + h);
        if (rc.bottom <= rc.top)
            rc.bottom = rc.top + 1;
        PatBlt(dc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, WHITENESS);
        return S_OK;
    }
};

//-----------------------------------------------------------------------------
// Lazy init
//-----------------------------------------------------------------------------
bool EnsureInit()
{
    if (g_bInitTried)
        return g_bInitOK;
    g_bInitTried = true;

    HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                                     __uuidof(IDWriteFactory),
                                     (IUnknown**)&g_pFactory);
    if (FAILED(hr) || g_pFactory == NULL)
    {
        CT_Done("PhoenixDWrite: DWriteCreateFactory FAILED 0x%08X", hr);
        return false;
    }

    hr = g_pFactory->GetGdiInterop(&g_pGdiInterop);
    if (FAILED(hr) || g_pGdiInterop == NULL)
    {
        CT_Done("PhoenixDWrite: GetGdiInterop FAILED 0x%08X", hr);
        SafeRelease(g_pFactory);
        return false;
    }

    // Grayscale AA: cleartypeLevel=0 → RGB kanallari esit coverage.
    // gamma=1.0 → beyaz/siyah render lineer; yesil kanal = coverage.
    hr = g_pFactory->CreateCustomRenderingParams(
        1.0f,   // gamma
        0.0f,   // enhancedContrast
        0.0f,   // clearTypeLevel (0 = grayscale)
        DWRITE_PIXEL_GEOMETRY_FLAT,
        DWRITE_RENDERING_MODE_DEFAULT,
        &g_pRenderParams);
    if (FAILED(hr) || g_pRenderParams == NULL)
    {
        CT_Done("PhoenixDWrite: CreateCustomRenderingParams FAILED 0x%08X", hr);
        SafeRelease(g_pGdiInterop);
        SafeRelease(g_pFactory);
        return false;
    }

    g_bInitOK = true;
    CT_Done("PhoenixDWrite: init OK");
    return true;
}

//-----------------------------------------------------------------------------
// Font entry olustur/cache'ten getir
//-----------------------------------------------------------------------------
unsigned long long FontKeyFromLogfont(const LOGFONTA& lf)
{
    unsigned long long h = 0;
    h = HashBytes(&lf.lfHeight, sizeof(lf.lfHeight), h);
    h = HashBytes(&lf.lfWeight, sizeof(lf.lfWeight), h);
    h = HashBytes(&lf.lfItalic, sizeof(lf.lfItalic), h);
    h = HashBytes(&lf.lfUnderline, sizeof(lf.lfUnderline), h);
    h = HashBytes(&lf.lfStrikeOut, sizeof(lf.lfStrikeOut), h);
    h = HashBytes(&lf.lfCharSet, sizeof(lf.lfCharSet), h);
    h = HashBytes(lf.lfFaceName, strlen(lf.lfFaceName), h);
    return h;
}

const FontEntry* GetFontEntry(HDC hdc, unsigned long long* pOutKey)
{
    HFONT hFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
    if (hFont == NULL)
        return NULL;

    LOGFONTA lf;
    if (GetObjectA(hFont, sizeof(lf), &lf) == 0)
        return NULL;

    const unsigned long long key = FontKeyFromLogfont(lf);
    if (pOutKey != NULL)
        *pOutKey = key;

    std::unordered_map<unsigned long long, FontEntry>::iterator it = g_FontCache.find(key);
    if (it != g_FontCache.end())
        return (it->second.pFormat != NULL) ? &it->second : NULL;

    // Yeni entry — basarisizlik da cache'lenir (pFormat NULL) ki her frame
    // ayni fail yolunu tekrar denemeyelim.
    FontEntry fe;
    fe.pFormat = NULL;
    fe.emSize = 12.0f;
    fe.lineHeight = 14.0f;
    fe.baseline = 11.0f;
    fe.codepage = CharsetToCodepage(lf.lfCharSet);
    fe.underline = lf.lfUnderline;
    fe.strike = lf.lfStrikeOut;

    // LOGFONTA → LOGFONTW
    LOGFONTW lfw;
    memset(&lfw, 0, sizeof(lfw));
    memcpy(&lfw, &lf, offsetof(LOGFONTA, lfFaceName));
    MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, -1, lfw.lfFaceName, LF_FACESIZE);

    // Font family adi + weight/style — GDI interop uzerinden (fallback: face adi)
    WCHAR familyName[LF_FACESIZE * 2] = L"";
    DWRITE_FONT_WEIGHT weight = (lf.lfWeight >= FW_BOLD) ? DWRITE_FONT_WEIGHT_BOLD
                                                         : DWRITE_FONT_WEIGHT_NORMAL;
    DWRITE_FONT_STYLE style = lf.lfItalic ? DWRITE_FONT_STYLE_ITALIC
                                          : DWRITE_FONT_STYLE_NORMAL;
    float unitsAscentDescent = 0.0f;
    float unitsPerEm = 0.0f;

    IDWriteFont* pFont = NULL;
    HRESULT hr = g_pGdiInterop->CreateFontFromLOGFONT(&lfw, &pFont);
    if (SUCCEEDED(hr) && pFont != NULL)
    {
        weight = pFont->GetWeight();
        style = pFont->GetStyle();

        DWRITE_FONT_METRICS fm;
        pFont->GetMetrics(&fm);
        unitsPerEm = (float)fm.designUnitsPerEm;
        unitsAscentDescent = (float)(fm.ascent + fm.descent);

        IDWriteFontFamily* pFamily = NULL;
        if (SUCCEEDED(pFont->GetFontFamily(&pFamily)) && pFamily != NULL)
        {
            IDWriteLocalizedStrings* pNames = NULL;
            if (SUCCEEDED(pFamily->GetFamilyNames(&pNames)) && pNames != NULL)
            {
                UINT32 nameLen = 0;
                pNames->GetStringLength(0, &nameLen);
                if (nameLen > 0 && nameLen < ARRAYSIZE(familyName))
                    pNames->GetString(0, familyName, nameLen + 1);
                SafeRelease(pNames);
            }
            SafeRelease(pFamily);
        }
        SafeRelease(pFont);
    }
    if (familyName[0] == L'\0')
    {
        // Interop fail — face adini dogrudan dene
        wcsncpy(familyName, lfw.lfFaceName, ARRAYSIZE(familyName) - 1);
        CT_Done("PhoenixDWrite: CreateFontFromLOGFONT fail (%s) — face adi denenecek",
                lf.lfFaceName);
    }

    // Em size — GDI lfHeight semantigi:
    //   <0 : -lfHeight = character height = em (pixelsPerDip=1)
    //   >0 : cell height → em = h * unitsPerEm / (ascent+descent)
    //    0 : default (12)
    if (lf.lfHeight < 0)
        fe.emSize = (float)(-lf.lfHeight);
    else if (lf.lfHeight > 0 && unitsAscentDescent > 0.0f)
        fe.emSize = (float)lf.lfHeight * unitsPerEm / unitsAscentDescent;
    else if (lf.lfHeight > 0)
        fe.emSize = (float)lf.lfHeight * 0.75f;
    else
        fe.emSize = 12.0f;

    hr = g_pFactory->CreateTextFormat(familyName, NULL, weight, style,
                                      DWRITE_FONT_STRETCH_NORMAL,
                                      fe.emSize, L"", &fe.pFormat);
    if (FAILED(hr) || fe.pFormat == NULL)
    {
        // Son care: Segoe UI
        hr = g_pFactory->CreateTextFormat(L"Segoe UI", NULL, weight, style,
                                          DWRITE_FONT_STRETCH_NORMAL,
                                          fe.emSize, L"", &fe.pFormat);
        if (FAILED(hr) || fe.pFormat == NULL)
        {
            CT_Done("PhoenixDWrite: CreateTextFormat FAILED 0x%08X (%s)", hr, lf.lfFaceName);
            fe.pFormat = NULL;
            g_FontCache[key] = fe;
            return NULL;
        }
    }
    fe.pFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    // Satir metrikleri — "Ag" ornek layout
    IDWriteTextLayout* pProbe = NULL;
    hr = g_pFactory->CreateGdiCompatibleTextLayout(L"Ag", 2, fe.pFormat,
                                                   4096.0f, 256.0f, 1.0f, NULL,
                                                   FALSE, &pProbe);
    if (SUCCEEDED(hr) && pProbe != NULL)
    {
        DWRITE_LINE_METRICS lm;
        UINT32 lineCount = 0;
        if (SUCCEEDED(pProbe->GetLineMetrics(&lm, 1, &lineCount)) && lineCount >= 1)
        {
            fe.lineHeight = lm.height;
            fe.baseline = lm.baseline;
        }
        SafeRelease(pProbe);
    }

    g_FontCache[key] = fe;
    return &g_FontCache[key];
}

//-----------------------------------------------------------------------------
// MBCS → wide (stack oncelikli)
//-----------------------------------------------------------------------------
int ToWide(UINT codepage, const char* str, int len, wchar_t* stackBuf, int stackCap,
           std::vector<wchar_t>& heapBuf, const wchar_t** ppOut)
{
    if (len <= 0)
    {
        *ppOut = stackBuf;
        return 0;
    }

    int need = MultiByteToWideChar(codepage, 0, str, len, NULL, 0);
    if (need <= 0)
    {
        *ppOut = stackBuf;
        return 0;
    }

    wchar_t* pDst;
    if (need <= stackCap)
    {
        pDst = stackBuf;
    }
    else
    {
        heapBuf.resize((size_t)need);
        pDst = &heapBuf[0];
    }

    int written = MultiByteToWideChar(codepage, 0, str, len, pDst, need);
    *ppOut = pDst;
    return (written > 0) ? written : 0;
}

//-----------------------------------------------------------------------------
// BRT boyut garantisi (grow-only)
//-----------------------------------------------------------------------------
bool EnsureBRT(int w, int h)
{
    if (g_pBRT != NULL && w <= g_BRTWidth && h <= g_BRTHeight)
        return true;

    const int newW = (w > g_BRTWidth) ? ((w + 63) & ~63) : g_BRTWidth;
    const int newH = (h > g_BRTHeight) ? ((h + 31) & ~31) : g_BRTHeight;

    SafeRelease(g_pBRT);
    g_BRTWidth = 0;
    g_BRTHeight = 0;

    HRESULT hr = g_pGdiInterop->CreateBitmapRenderTarget(NULL, newW, newH, &g_pBRT);
    if (FAILED(hr) || g_pBRT == NULL)
    {
        CT_Done("PhoenixDWrite: CreateBitmapRenderTarget FAILED 0x%08X (%dx%d)", hr, newW, newH);
        g_pBRT = NULL;
        return false;
    }
    g_pBRT->SetPixelsPerDip(1.0f);
    g_BRTWidth = newW;
    g_BRTHeight = newH;
    return true;
}

// BRT DIB bit erisimi
BYTE* GetBRTBits(int* pPitch)
{
    if (g_pBRT == NULL)
        return NULL;
    HDC dc = g_pBRT->GetMemoryDC();
    if (dc == NULL)
        return NULL;
    HBITMAP hbm = (HBITMAP)GetCurrentObject(dc, OBJ_BITMAP);
    if (hbm == NULL)
        return NULL;
    DIBSECTION ds;
    if (GetObject(hbm, sizeof(ds), &ds) != sizeof(ds) || ds.dsBm.bmBits == NULL)
        return NULL;
    *pPitch = ds.dsBm.bmWidthBytes;
    // BitmapRenderTarget DIB'i bottom-up olabilir; bmHeight isaretine gore
    // satir adresleme asagida yapiliyor (negatif pitch kullanmiyoruz —
    // CreateBitmapRenderTarget top-down 32bpp DIB olusturur, dsBmih.biHeight
    // negatifse zaten top-down'dur).
    return (BYTE*)ds.dsBm.bmBits;
}

//-----------------------------------------------------------------------------
// Rasterize: wide string → mask cache entry
//-----------------------------------------------------------------------------
const MaskEntry* GetOrRasterize(const FontEntry& fe, unsigned long long maskKey,
                                const wchar_t* wstr, int wlen)
{
    std::unordered_map<unsigned long long, MaskEntry>::iterator it = g_MaskCache.find(maskKey);
    if (it != g_MaskCache.end())
        return &it->second;

    // Butce kontrolu
    if (g_MaskCache.size() >= kMaskCacheMaxEntries || g_MaskCacheBytes >= kMaskCacheMaxBytes)
    {
        g_MaskCache.clear();
        g_MaskCacheBytes = 0;
    }

    MaskEntry entry;
    entry.w = 0;
    entry.h = 0;
    entry.baseline = fe.baseline;

    if (wlen <= 0)
    {
        MaskEntry& stored0 = g_MaskCache[maskKey];
        stored0 = entry;
        return &stored0;
    }

    IDWriteTextLayout* pLayout = NULL;
    HRESULT hr = g_pFactory->CreateGdiCompatibleTextLayout(
        wstr, (UINT32)wlen, fe.pFormat, 8192.0f, 512.0f, 1.0f, NULL, FALSE, &pLayout);
    if (FAILED(hr) || pLayout == NULL)
        return NULL;

    if (fe.underline)
    {
        DWRITE_TEXT_RANGE range = { 0, (UINT32)wlen };
        pLayout->SetUnderline(TRUE, range);
    }
    if (fe.strike)
    {
        DWRITE_TEXT_RANGE range = { 0, (UINT32)wlen };
        pLayout->SetStrikethrough(TRUE, range);
    }

    DWRITE_TEXT_METRICS tm;
    hr = pLayout->GetMetrics(&tm);
    if (FAILED(hr))
    {
        SafeRelease(pLayout);
        return NULL;
    }

    DWRITE_LINE_METRICS lm;
    UINT32 lineCount = 0;
    if (SUCCEEDED(pLayout->GetLineMetrics(&lm, 1, &lineCount)) && lineCount >= 1)
        entry.baseline = lm.baseline;

    const int w = (int)ceilf(tm.widthIncludingTrailingWhitespace) + 1;
    const int h = (int)ceilf(tm.height) + 1;
    if (w <= 1 || h <= 1 || w > 4096 || h > 1024)
    {
        // Bos veya anormal olcum — bos entry (cizim no-op, GDI'ye dusme yok)
        SafeRelease(pLayout);
        MaskEntry& storedE = g_MaskCache[maskKey];
        storedE = entry;
        return &storedE;
    }

    if (!EnsureBRT(w, h))
    {
        SafeRelease(pLayout);
        return NULL;
    }

    // BRT'yi siyaha temizle (yalniz kullanilacak bolge)
    int brtPitch = 0;
    BYTE* pBits = GetBRTBits(&brtPitch);
    if (pBits == NULL)
    {
        SafeRelease(pLayout);
        return NULL;
    }
    for (int y = 0; y < h; ++y)
        memset(pBits + y * brtPitch, 0, (size_t)w * 4);

    // Rasterize — beyaz glyph'ler siyah zemine
    hr = pLayout->Draw(NULL, &PhoenixTextRenderer::Instance(), 0.0f, 0.0f);
    SafeRelease(pLayout);
    if (FAILED(hr))
    {
        CT_Done("PhoenixDWrite: layout Draw FAILED 0x%08X", hr);
        return NULL;
    }

    // Coverage cikar — yesil kanal
    entry.w = w;
    entry.h = h;
    entry.mask.resize((size_t)w * (size_t)h);
    for (int y = 0; y < h; ++y)
    {
        const BYTE* pRow = pBits + y * brtPitch;
        BYTE* pDst = &entry.mask[(size_t)y * w];
        for (int x = 0; x < w; ++x)
            pDst[x] = pRow[x * 4 + 1];   // G
    }

    g_MaskCacheBytes += entry.mask.size();
    MaskEntry& stored = g_MaskCache[maskKey];
    stored = entry;
    return &stored;
}

//-----------------------------------------------------------------------------
// Scratch premult DIB garantisi (grow-only)
//-----------------------------------------------------------------------------
bool EnsureScratch(int w, int h)
{
    if (g_hScratchDC != NULL && w <= g_ScratchW && h <= g_ScratchH)
        return true;

    const int newW = (w > g_ScratchW) ? ((w + 63) & ~63) : (g_ScratchW ? g_ScratchW : 64);
    const int newH = (h > g_ScratchH) ? ((h + 31) & ~31) : (g_ScratchH ? g_ScratchH : 32);

    if (g_hScratchDC == NULL)
    {
        g_hScratchDC = CreateCompatibleDC(NULL);
        if (g_hScratchDC == NULL)
            return false;
    }
    if (g_hScratchBmp != NULL)
    {
        DeleteObject(g_hScratchBmp);
        g_hScratchBmp = NULL;
        g_pScratchBits = NULL;
    }

    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = newW;
    bi.bmiHeader.biHeight = -newH;   // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    g_hScratchBmp = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, &g_pScratchBits, NULL, 0);
    if (g_hScratchBmp == NULL || g_pScratchBits == NULL)
    {
        CT_Done("PhoenixDWrite: scratch DIB FAILED (%dx%d)", newW, newH);
        g_hScratchBmp = NULL;
        g_pScratchBits = NULL;
        return false;
    }
    SelectObject(g_hScratchDC, g_hScratchBmp);
    g_ScratchW = newW;
    g_ScratchH = newH;
    return true;
}

//-----------------------------------------------------------------------------
// Mask'i renkle hedef HDC'ye bindir (alt-dikdortgen destekli).
// srcX/srcY/w/h: mask icinden alinacak bolge; destX/destY: hedef piksel.
//-----------------------------------------------------------------------------
bool BlendMask(HDC hdcDest, int destX, int destY, const MaskEntry& e,
               int srcX, int srcY, int w, int h, COLORREF color)
{
    if (w <= 0 || h <= 0 || e.w <= 0 || e.h <= 0)
        return true;   // bos — basarili no-op

    if (srcX < 0) { destX -= srcX; w += srcX; srcX = 0; }
    if (srcY < 0) { destY -= srcY; h += srcY; srcY = 0; }
    if (srcX + w > e.w) w = e.w - srcX;
    if (srcY + h > e.h) h = e.h - srcY;
    if (w <= 0 || h <= 0)
        return true;

    if (!EnsureScratch(w, h))
        return false;

    const BYTE r = GetRValue(color);
    const BYTE g = GetGValue(color);
    const BYTE b = GetBValue(color);

    // Premultiplied BGRA doldur
    for (int y = 0; y < h; ++y)
    {
        const BYTE* pM = &e.mask[(size_t)(srcY + y) * e.w + srcX];
        BYTE* pD = (BYTE*)g_pScratchBits + (size_t)y * g_ScratchW * 4;
        for (int x = 0; x < w; ++x)
        {
            const BYTE a = pM[x];
            pD[x * 4 + 0] = (BYTE)((b * a + 127) / 255);
            pD[x * 4 + 1] = (BYTE)((g * a + 127) / 255);
            pD[x * 4 + 2] = (BYTE)((r * a + 127) / 255);
            pD[x * 4 + 3] = a;
        }
    }

    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 255;
    bf.AlphaFormat = AC_SRC_ALPHA;

    return GdiAlphaBlend(hdcDest, destX, destY, w, h,
                         g_hScratchDC, 0, 0, w, h, bf) != FALSE;
}

//-----------------------------------------------------------------------------
// TextOut cekirdegi — align/bkmode dahil
//-----------------------------------------------------------------------------
bool DrawStringCore(HDC hdc, int x, int y, const FontEntry& fe,
                    unsigned long long fontKey, const char* str, int len,
                    const RECT* pClipRect)
{
    wchar_t stackBuf[512];
    std::vector<wchar_t> heapBuf;
    const wchar_t* pWide = NULL;
    const int wlen = ToWide(fe.codepage, str, len, stackBuf, 512, heapBuf, &pWide);

    unsigned long long maskKey = fontKey;
    maskKey = HashBytes(pWide, (size_t)wlen * sizeof(wchar_t), maskKey);

    const MaskEntry* pMask = GetOrRasterize(fe, maskKey, pWide, wlen);
    if (pMask == NULL)
        return false;   // rasterize fail — caller GDI'ye dusecek

    // Hizalama — GDI SetTextAlign semantigi
    const UINT ta = GetTextAlign(hdc);
    int dx = x;
    int dy = y;
    if ((ta & TA_CENTER) == TA_CENTER)
        dx = x - pMask->w / 2;
    else if ((ta & TA_CENTER) == TA_RIGHT)
        dx = x - pMask->w;

    const UINT vert = ta & TA_BASELINE;   // 0=top, 8=bottom, 24=baseline
    if (vert == TA_BASELINE)
        dy = y - (int)(pMask->baseline + 0.5f);
    else if (vert == TA_BOTTOM)
        dy = y - pMask->h;

    // OPAQUE arka plan — GDI paritesi (mevcut bk color ile dolgu)
    if (GetBkMode(hdc) == OPAQUE && pMask->w > 0 && pMask->h > 0)
    {
        RECT rc = { dx, dy, dx + pMask->w, dy + pMask->h };
        ExtTextOutA(hdc, dx, dy, ETO_OPAQUE, &rc, "", 0, NULL);
    }

    int srcX = 0;
    int srcY = 0;
    int w = pMask->w;
    int h = pMask->h;
    if (pClipRect != NULL)
    {
        if (dx < pClipRect->left)   { srcX += (pClipRect->left - dx); w -= (pClipRect->left - dx); dx = pClipRect->left; }
        if (dy < pClipRect->top)    { srcY += (pClipRect->top - dy);  h -= (pClipRect->top - dy);  dy = pClipRect->top; }
        if (dx + w > pClipRect->right)  w = pClipRect->right - dx;
        if (dy + h > pClipRect->bottom) h = pClipRect->bottom - dy;
    }

    return BlendMask(hdc, dx, dy, *pMask, srcX, srcY, w, h, GetTextColor(hdc));
}

} // namespace

//-----------------------------------------------------------------------------
// Public API
//-----------------------------------------------------------------------------

bool PhoenixDWrite_IsEnabled()
{
    if (!IsDWriteSelected())
        return false;
    return EnsureInit();
}

BOOL PhoenixDWrite_TextOut(HDC hdc, int x, int y, LPCSTR str, int len)
{
    if (hdc == NULL || str == NULL || !PhoenixDWrite_IsEnabled())
        return FALSE;
    if (len <= 0)
        return TRUE;   // bos string — GDI TextOut da no-op basarili

    unsigned long long fontKey = 0;
    const FontEntry* pFE = GetFontEntry(hdc, &fontKey);
    if (pFE == NULL)
        return FALSE;

    return DrawStringCore(hdc, x, y, *pFE, fontKey, str, len, NULL) ? TRUE : FALSE;
}

int PhoenixDWrite_DrawText(HDC hdc, LPCSTR str, int len, LPRECT rect, UINT format)
{
    if (hdc == NULL || str == NULL || rect == NULL || !PhoenixDWrite_IsEnabled())
        return -1;
    if (len <= 0)
        return 0;

    unsigned long long fontKey = 0;
    const FontEntry* pFE = GetFontEntry(hdc, &fontKey);
    if (pFE == NULL)
        return -1;

    // FL2 kullanim profili: metin g_MakeLinefeedString ile onceden '\n'
    // sarilmis gelir; format & 3 yatay hizalamayi belirler (DT_LEFT/CENTER/
    // RIGHT). Satir satir ciz, rect'e clip et (GDI DrawText varsayilani
    // clip'li — DT_NOCLIP kullanilmiyor).
    const int lineHeight = (int)(pFE->lineHeight + 0.5f);
    const UINT hAlign = format & 3;   // 0=left 1=center 2=right

    int lineStart = 0;
    int lineIndex = 0;
    int i = 0;
    while (i <= len)
    {
        const bool atEnd = (i == len);
        if (atEnd || str[i] == '\n')
        {
            int lineLen = i - lineStart;
            // '\r' kuyrugu temizle
            while (lineLen > 0 && str[lineStart + lineLen - 1] == '\r')
                --lineLen;

            const int y = rect->top + lineIndex * lineHeight;
            if (y >= rect->bottom)
                break;

            if (lineLen > 0)
            {
                // Hizalama icin satir genisligi gerekli — mask zaten
                // rasterize edilecegi icin dogrudan cek.
                wchar_t stackBuf[512];
                std::vector<wchar_t> heapBuf;
                const wchar_t* pWide = NULL;
                const int wlen = ToWide(pFE->codepage, str + lineStart, lineLen,
                                        stackBuf, 512, heapBuf, &pWide);

                unsigned long long maskKey = fontKey;
                maskKey = HashBytes(pWide, (size_t)wlen * sizeof(wchar_t), maskKey);
                const MaskEntry* pMask = GetOrRasterize(*pFE, maskKey, pWide, wlen);
                if (pMask == NULL)
                    return -1;   // fail — caller GDI'ye dusecek (henuz cizim yapilmadiysa ideal; kismi cizimde de GDI ustune cizer, gorsel esdeger)

                int x = rect->left;
                if (hAlign == 1)
                    x = rect->left + ((rect->right - rect->left) - pMask->w) / 2;
                else if (hAlign == 2)
                    x = rect->right - pMask->w;

                if (GetBkMode(hdc) == OPAQUE && pMask->w > 0)
                {
                    RECT rcBk = { x, y, x + pMask->w, y + pMask->h };
                    ExtTextOutA(hdc, x, y, ETO_OPAQUE, &rcBk, "", 0, NULL);
                }

                int srcX = 0, srcY = 0, w = pMask->w, h = pMask->h;
                int dx = x, dy = y;
                if (dx < rect->left)   { srcX += rect->left - dx; w -= rect->left - dx; dx = rect->left; }
                if (dy < rect->top)    { srcY += rect->top - dy;  h -= rect->top - dy;  dy = rect->top; }
                if (dx + w > rect->right)  w = rect->right - dx;
                if (dy + h > rect->bottom) h = rect->bottom - dy;

                if (!BlendMask(hdc, dx, dy, *pMask, srcX, srcY, w, h, GetTextColor(hdc)))
                    return -1;
            }

            ++lineIndex;
            lineStart = i + 1;
        }
        ++i;
    }

    return lineIndex * lineHeight;
}

BOOL PhoenixDWrite_GetTextExtent(HDC hdc, LPCSTR str, int len, LPSIZE size)
{
    if (hdc == NULL || str == NULL || size == NULL || !PhoenixDWrite_IsEnabled())
        return FALSE;

    unsigned long long fontKey = 0;
    const FontEntry* pFE = GetFontEntry(hdc, &fontKey);
    if (pFE == NULL)
        return FALSE;

    if (len <= 0)
    {
        size->cx = 0;
        size->cy = (LONG)(pFE->lineHeight + 0.5f);
        return TRUE;
    }

    wchar_t stackBuf[512];
    std::vector<wchar_t> heapBuf;
    const wchar_t* pWide = NULL;
    const int wlen = ToWide(pFE->codepage, str, len, stackBuf, 512, heapBuf, &pWide);
    if (wlen <= 0)
    {
        size->cx = 0;
        size->cy = (LONG)(pFE->lineHeight + 0.5f);
        return TRUE;
    }

    IDWriteTextLayout* pLayout = NULL;
    HRESULT hr = g_pFactory->CreateGdiCompatibleTextLayout(
        pWide, (UINT32)wlen, pFE->pFormat, 8192.0f, 512.0f, 1.0f, NULL, FALSE, &pLayout);
    if (FAILED(hr) || pLayout == NULL)
        return FALSE;

    DWRITE_TEXT_METRICS tm;
    hr = pLayout->GetMetrics(&tm);
    SafeRelease(pLayout);
    if (FAILED(hr))
        return FALSE;

    size->cx = (LONG)ceilf(tm.widthIncludingTrailingWhitespace);
    size->cy = (LONG)ceilf(tm.height);
    if (size->cy <= 0)
        size->cy = (LONG)(pFE->lineHeight + 0.5f);
    return TRUE;
}

void PhoenixDWrite_Shutdown()
{
    for (std::unordered_map<unsigned long long, FontEntry>::iterator it = g_FontCache.begin();
         it != g_FontCache.end(); ++it)
    {
        SafeRelease(it->second.pFormat);
    }
    g_FontCache.clear();
    g_MaskCache.clear();
    g_MaskCacheBytes = 0;

    if (g_hScratchDC != NULL)
    {
        DeleteDC(g_hScratchDC);
        g_hScratchDC = NULL;
    }
    if (g_hScratchBmp != NULL)
    {
        DeleteObject(g_hScratchBmp);
        g_hScratchBmp = NULL;
        g_pScratchBits = NULL;
    }
    g_ScratchW = 0;
    g_ScratchH = 0;

    SafeRelease(g_pBRT);
    g_BRTWidth = 0;
    g_BRTHeight = 0;

    SafeRelease(g_pRenderParams);
    SafeRelease(g_pGdiInterop);
    SafeRelease(g_pFactory);
    g_bInitTried = false;
    g_bInitOK = false;
}
