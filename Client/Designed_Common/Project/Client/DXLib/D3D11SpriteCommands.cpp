//-----------------------------------------------------------------------------
// D3D11SpriteCommands.cpp
//
// FULL DX11 gecis Faz G1 (2026-09-01) — komut katmani implementasyonu.
// Mimari aciklama: D3D11SpriteCommands.h
//-----------------------------------------------------------------------------

#include "D3D11SpriteCommands.h"
#include "IRenderBackend.h"	// O2: GetSpriteLinearFilter
#include "../crash_trace_tool.h"

#include <d3d11.h>
#include <string.h>
#include <vector>
#include <emmintrin.h>	// 2026-09-03 FPS fix — SSE2 diff/convert (SCPERF verisi)

namespace phoenix
{
namespace render
{

namespace
{

template <typename T>
inline void SC_SafeRelease(T*& p)
{
    if (p != NULL)
    {
        p->Release();
        p = NULL;
    }
}

// 565 -> RGBA8 (R,G,B,A bellek sirasi) — alpha sabit 255
inline DWORD Expand565ToRGBA(WORD px)
{
    DWORD r = (px >> 11) & 0x1F;
    DWORD g = (px >> 5)  & 0x3F;
    DWORD b =  px        & 0x1F;
    r = (r << 3) | (r >> 2);
    g = (g << 2) | (g >> 4);
    b = (b << 3) | (b >> 2);
    return 0xFF000000u | (b << 16) | (g << 8) | r;
}

// ---------------------------------------------------------------------------
// 2026-09-03 FPS fix — SSE2 565->RGBA8888 genisletme (4 px, 32-bit lane).
// SCPERF olcumu (kullanici 06:16 testi): oyun ici saniyenin ~780ms'i bu
// dosyanin skaler piksel donguierinde gidiyordu (emitMs~480 + blMs~155 +
// flush/snap). Skaler Expand565ToRGBA ile ayni bit sonucu uretir (r dusuk
// byte, alpha FF) — davranis degisikligi yok, yalniz vektorlestirme.
// ---------------------------------------------------------------------------
static inline __m128i SC_Expand565x4(__m128i v)
{
    const __m128i m1F = _mm_set1_epi32(0x1F);
    __m128i r = _mm_and_si128(_mm_srli_epi32(v, 11), m1F);
    __m128i g = _mm_and_si128(_mm_srli_epi32(v, 5), _mm_set1_epi32(0x3F));
    __m128i b = _mm_and_si128(v, m1F);
    r = _mm_or_si128(_mm_slli_epi32(r, 3), _mm_srli_epi32(r, 2));
    g = _mm_or_si128(_mm_slli_epi32(g, 2), _mm_srli_epi32(g, 4));
    b = _mm_or_si128(_mm_slli_epi32(b, 3), _mm_srli_epi32(b, 2));
    return _mm_or_si128(_mm_or_si128(r, _mm_slli_epi32(g, 8)),
                        _mm_or_si128(_mm_slli_epi32(b, 16),
                                     _mm_set1_epi32((int)0xFF000000)));
}

// 2026-09-01 overflow fix — dikdortgen kesisim testi (bos kesisim = false).
// CPU dirty bolgesi ile gelen quad'in dest'i KESISMIYORSA siralama pikselleri
// etkilemez -> checkpoint segment'i ertelenebilir (slot tuketmez).
inline bool SC_RectsOverlap(const RECT& a, const RECT& b)
{
    return a.left < b.right && b.left < a.right &&
           a.top < b.bottom && b.top < a.bottom;
}

// ---------------------------------------------------------------------------
// 2026-09-03 kural 9.2 instrumentation (GECICI) — FPS drop + minimap jitter
// teshisi. RenderFrame her ~1 sn'de bir ozet satiri CT_OBJ kanalina yazar
// (crash_trace_obj.log, CT_OBJ_ENABLED su an 1). Frame basina disk I/O YOK —
// yalniz sayac toplanir. Teshis bitince CT_OBJ_ENABLED=0 ile no-op (kural
// 9.4); cagri yerleri korunur (kural 4.1).
// ---------------------------------------------------------------------------
struct SCPerf
{
    int     frames;        // RenderFrame basarili
    int     worldFrames;   // m_bWorldFrame set olan frame
    int     flushes;       // FlushToAccum cagrisi
    int     baselineUps;   // baseline upload (Map+kopya) sayisi
    int     segEmits;      // segment ureten EmitSegment
    int     segEmpty;      // diff bos cikan EmitSegment
    __int64 segPx;         // emit edilen dirty alan (px)
    int     quadsLast;     // son frame'in quad sayisi
    double  emitMs;        // EmitSegment toplam suresi
    double  baselineMs;    // baseline upload toplam suresi
    double  flushMs;       // FlushToAccum toplam suresi (baseline+emit dahil)
    double  renderMs;      // RenderFrame toplam suresi
    double  snapMs;        // SnapshotComposition toplam suresi (ResetFrame)
    DWORD   lastLogTick;
};
SCPerf g_scPerf = {};

double SC_NowMs()
{
    static LARGE_INTEGER s_Freq = {};
    if (s_Freq.QuadPart == 0)
        QueryPerformanceFrequency(&s_Freq);
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart * 1000.0 / (double)s_Freq.QuadPart;
}

} // namespace

//-----------------------------------------------------------------------------
// Komut kaydi
//-----------------------------------------------------------------------------
struct D3D11SpriteCommands::Cmd
{
    // LIGHT (Faz G4): isik grid'i BLEND_MULTIPLY fullscreen quad'i —
    // linear sampler ile cizilir (sprite'lar point kalir).
    enum Type { QUAD, SEGMENT, LIGHT };
    Type                         type;
    // QUAD
    ID3D11ShaderResourceView*    pSRV;      // SEGMENT'te segment SRV'si
    int                          texW;
    int                          texH;
    RECT                         dest;
    RECT                         src;
    DWORD                        colorRGBA;
    D3D11QuadPipeline::BlendMode blend;
};

typedef std::vector<D3D11SpriteCommands::Cmd> SCCmdVec;

//-----------------------------------------------------------------------------
// 2026-09-02 crash fix — QUAD komutlari SRV'ye AddRef ile SAHIP OLUR.
//
// Root cause (WER dump Client.exe.1924.dmp, d3d11 SetShaderResources AV):
// D3D11SpriteTextureCache::InsertRGBA ayni key yeniden eklendiginde ve
// OnFrameEnd LRU eviction'da eski SRV'yi ANINDA Release ediyordu. Faz G6
// palet-animasyonlu efekt texture'lari ayni frame icinde ayni key'e yeni
// generation ile re-insert tetikleyince, frame basinda kaydedilmis quad'in
// pointer'i RenderFrame'e gelmeden dangling kaliyordu. Cozum: RecordQuad
// AddRef alir, komut listesi bosaltilirken (ResetFrame/Shutdown) Release
// edilir — eviction/replace artik yasam suresini etkileyemez.
// SEGMENT (m_pSegSRV havuzu) ve LIGHT (m_pLightSRV uyesi) SRV'leri sinif
// tarafindan sahiplenildigi icin ref alinmaz.
//-----------------------------------------------------------------------------
inline void SC_ReleaseQuadRefs(SCCmdVec& cmds)
{
    for (size_t i = 0; i < cmds.size(); ++i)
    {
        D3D11SpriteCommands::Cmd& c = cmds[i];
        if (c.type == D3D11SpriteCommands::Cmd::QUAD && c.pSRV != NULL)
        {
            c.pSRV->Release();
            c.pSRV = NULL;
        }
    }
}

D3D11SpriteCommands::D3D11SpriteCommands()
    : m_pCmds(new SCCmdVec())
    , m_pDevice(NULL)
    , m_pContext(NULL)
    , m_pCompBits(NULL)
    , m_CompPitch(0)
    , m_Width(0)
    , m_Height(0)
    , m_pBaseline(NULL)
    , m_pPrev(NULL)
    , m_pSegScratch(NULL)
    , m_pBaselineTex(NULL)
    , m_pBaselineSRV(NULL)
    , m_pAccumTex(NULL)
    , m_pAccumRTV(NULL)
    , m_pAccumSRV(NULL)
    , m_bAccumSeeded(false)
    , m_bWorldFrame(false)
    , m_pLightTex(NULL)
    , m_pLightSRV(NULL)
    , m_LightTexW(0)
    , m_LightTexH(0)
    , m_bDirty(false)
    , m_SegmentsUsed(0)
    , m_QuadsThisFrame(0)
    , m_bOverflow(false)
    , m_bBaselineApplied(false)
    , m_bInit(false)
{
    for (int i = 0; i < kMaxSegments; ++i)
    {
        m_pSegTex[i] = NULL;
        m_pSegSRV[i] = NULL;
    }
    memset(&m_DirtyRect, 0, sizeof(m_DirtyRect));
}

D3D11SpriteCommands::~D3D11SpriteCommands()
{
    Shutdown();
    delete (SCCmdVec*)m_pCmds;
    m_pCmds = NULL;
}

D3D11SpriteCommands& D3D11SpriteCommands::Instance()
{
    static D3D11SpriteCommands s_Inst;
    return s_Inst;
}

bool D3D11SpriteCommands::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
                               void* pCompBits, int compPitchBytes,
                               int width, int height)
{
    if (m_bInit)
        return true;
    if (!pDevice || !pContext || !pCompBits || width <= 0 || height <= 0)
        return false;

    m_pDevice = pDevice;
    m_pContext = pContext;
    m_pCompBits = pCompBits;
    m_CompPitch = compPitchBytes;
    m_Width = width;
    m_Height = height;

    // CPU snapshot'lar
    m_pBaseline = new WORD[(size_t)width * height];
    m_pPrev     = new WORD[(size_t)width * height];
    // 2026-09-03 FPS fix #2 — segment diff scratch (aciklama header'da)
    m_pSegScratch = new DWORD[(size_t)width * height];

    // GPU: baseline dynamic RGBA8, tam ekran
    D3D11_TEXTURE2D_DESC td;
    ZeroMemory(&td, sizeof(td));
    td.Width = width;
    td.Height = height;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DYNAMIC;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    td.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = m_pDevice->CreateTexture2D(&td, NULL, &m_pBaselineTex);
    if (SUCCEEDED(hr))
        hr = m_pDevice->CreateShaderResourceView(m_pBaselineTex, NULL, &m_pBaselineSRV);
    // 2026-09-03 FPS fix #2 — segment ring DEFAULT usage: her emit'te
    // fullscreen DISCARD rename yerine UpdateSubresource ile yalniz degisen
    // kutu tasinir (rename havuzu stall'i biter). CPU erisimi yok; yazim
    // m_pSegScratch uzerinden yapilir.
    D3D11_TEXTURE2D_DESC ts = td;
    ts.Usage = D3D11_USAGE_DEFAULT;
    ts.CPUAccessFlags = 0;
    for (int i = 0; i < kMaxSegments && SUCCEEDED(hr); ++i)
    {
        hr = m_pDevice->CreateTexture2D(&ts, NULL, &m_pSegTex[i]);
        if (SUCCEEDED(hr))
            hr = m_pDevice->CreateShaderResourceView(m_pSegTex[i], NULL, &m_pSegSRV[i]);
    }
    // 2026-09-01 — kalici accumulation RT (DEFAULT usage: hem render target
    // hem shader resource; frame'ler arasi icerigi korur)
    if (SUCCEEDED(hr))
    {
        D3D11_TEXTURE2D_DESC ta = td;
        ta.Usage = D3D11_USAGE_DEFAULT;
        ta.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        ta.CPUAccessFlags = 0;
        hr = m_pDevice->CreateTexture2D(&ta, NULL, &m_pAccumTex);
        if (SUCCEEDED(hr))
            hr = m_pDevice->CreateRenderTargetView(m_pAccumTex, NULL, &m_pAccumRTV);
        if (SUCCEEDED(hr))
            hr = m_pDevice->CreateShaderResourceView(m_pAccumTex, NULL, &m_pAccumSRV);
    }
    if (FAILED(hr))
    {
        CT_Done("SpriteCommands: texture init FAILED 0x%08X", hr);
        Shutdown();
        return false;
    }

    m_bAccumSeeded = false;
    m_bInit = true;
    SnapshotComposition();
    ((SCCmdVec*)m_pCmds)->reserve(4096);

    CT_Done("SpriteCommands: init OK %dx%d, %d segment butcesi",
            width, height, (int)kMaxSegments);
    return true;
}

void D3D11SpriteCommands::Shutdown()
{
    SC_SafeRelease(m_pLightSRV);
    SC_SafeRelease(m_pLightTex);
    m_LightTexW = 0;
    m_LightTexH = 0;
    SC_SafeRelease(m_pAccumSRV);
    SC_SafeRelease(m_pAccumRTV);
    SC_SafeRelease(m_pAccumTex);
    m_bAccumSeeded = false;
    m_bWorldFrame = false;
    SC_SafeRelease(m_pBaselineSRV);
    SC_SafeRelease(m_pBaselineTex);
    for (int i = 0; i < kMaxSegments; ++i)
    {
        SC_SafeRelease(m_pSegSRV[i]);
        SC_SafeRelease(m_pSegTex[i]);
    }

    delete[] m_pBaseline;
    delete[] m_pPrev;
    delete[] m_pSegScratch;
    m_pBaseline = NULL;
    m_pPrev = NULL;
    m_pSegScratch = NULL;

    if (m_pCmds != NULL)
    {
        // 2026-09-02 crash fix — QUAD SRV ref'lerini birak
        SC_ReleaseQuadRefs(*(SCCmdVec*)m_pCmds);
        ((SCCmdVec*)m_pCmds)->clear();
    }

    m_pDevice = NULL;
    m_pContext = NULL;
    m_pCompBits = NULL;
    m_bInit = false;
    m_bDirty = false;
    m_SegmentsUsed = 0;
    m_QuadsThisFrame = 0;
    m_bOverflow = false;
}

bool D3D11SpriteCommands::IsActive() const
{
    return m_bInit && !m_bOverflow && IsD3D11GpuPathSelected() &&
           D3D11QuadPipeline::Instance().IsReady();
}

void D3D11SpriteCommands::MarkDirty(const RECT* pRect)
{
    if (!m_bInit)
        return;

    RECT r;
    if (pRect == NULL)
    {
        r.left = 0;
        r.top = 0;
        r.right = m_Width;
        r.bottom = m_Height;
    }
    else
    {
        r = *pRect;
        if (r.left < 0) r.left = 0;
        if (r.top < 0) r.top = 0;
        if (r.right > m_Width) r.right = m_Width;
        if (r.bottom > m_Height) r.bottom = m_Height;
        if (r.left >= r.right || r.top >= r.bottom)
            return;
    }

    if (!m_bDirty)
    {
        m_DirtyRect = r;
        m_bDirty = true;
    }
    else
    {
        if (r.left < m_DirtyRect.left)     m_DirtyRect.left = r.left;
        if (r.top < m_DirtyRect.top)       m_DirtyRect.top = r.top;
        if (r.right > m_DirtyRect.right)   m_DirtyRect.right = r.right;
        if (r.bottom > m_DirtyRect.bottom) m_DirtyRect.bottom = r.bottom;
    }
}

//-----------------------------------------------------------------------------
// ForceDirty — 2026-09-03 R6 (mimari aciklama header'da). PREV bolgede
// comp'un bit tersine cevrilir -> EmitSegment her pikseli "degisti" gorur.
//-----------------------------------------------------------------------------
void D3D11SpriteCommands::ForceDirty(const RECT* pRect)
{
    if (!IsActive() || m_pPrev == NULL || m_pCompBits == NULL || pRect == NULL)
        return;

    RECT r = *pRect;
    if (r.left < 0) r.left = 0;
    if (r.top < 0) r.top = 0;
    if (r.right > m_Width) r.right = m_Width;
    if (r.bottom > m_Height) r.bottom = m_Height;
    if (r.left >= r.right || r.top >= r.bottom)
        return;

    for (int y = r.top; y < r.bottom; ++y)
    {
        const WORD* pComp = (const WORD*)((const BYTE*)m_pCompBits + (size_t)y * m_CompPitch);
        WORD*       pPrev = m_pPrev + (size_t)y * m_Width;
        for (int x = r.left; x < r.right; ++x)
            pPrev[x] = (WORD)(pComp[x] ^ 0xFFFFu);
    }
    MarkDirty(&r);
}

//-----------------------------------------------------------------------------
// EmitSegment — dirty bolgede composition'i PREV ile karsilastir; degisen
// pikseller alpha=255, degismeyenler alpha=0 olarak segment texture'ina
// yazilir. PREV dirty bolgede guncellenir. Komut listesine SEGMENT eklenir.
//-----------------------------------------------------------------------------
bool D3D11SpriteCommands::EmitSegment()
{
    if (!m_bDirty)
        return true;
    if (m_SegmentsUsed >= kMaxSegments)
        return false;

    const double tEmit0 = SC_NowMs();   // 9.2 instrumentation
    const int idx = m_SegmentsUsed;
    const RECT r = m_DirtyRect;

    // 2026-09-03 FPS fix #2 — diff artik CPU scratch'e yazilir ve GPU'ya
    // YALNIZ degisen piksellerin siki sinir kutusu UpdateSubresource ile
    // tasinir (segment texture'lari DEFAULT usage). Onceki tasarim her
    // emit'te fullscreen dynamic texture'i DISCARD ile map ediyordu:
    // frame basina 12-17 segment x ~8MB rename tahsisi surucunun rename
    // havuzunu tuketip periyodik stall uretiyordu (SCPERF: kotu saniyelerde
    // blMs ~250 / flMs ~690 / rfMs ~376 ms/s). Semantik AYNI: degismeyen px
    // 0 (seffaf), degisen px RGBA (alpha FF) + PREV=cur.
    // 2026-09-03 FPS fix — skaler px dongusu SSE2 (SCPERF: emitMs ~480ms/s idi).
    bool bAnyChanged = false;   // Faz G5 — bos diff'te slot tuketme
    int tightL = r.right, tightR = r.left;   // degisen px siki kutusu
    int tightT = r.bottom, tightB = r.top;
    for (int y = r.top; y < r.bottom; ++y)
    {
        const WORD* pComp = (const WORD*)((const BYTE*)m_pCompBits + (size_t)y * m_CompPitch);
        WORD*       pPrev = m_pPrev + (size_t)y * m_Width;
        DWORD*      pDst  = m_pSegScratch + (size_t)y * m_Width;

        bool bRowChanged = false;
        int x = r.left;
        for (; x + 8 <= r.right; x += 8)
        {
            const __m128i c  = _mm_loadu_si128((const __m128i*)(pComp + x));
            const __m128i p  = _mm_loadu_si128((const __m128i*)(pPrev + x));
            const __m128i eq = _mm_cmpeq_epi16(c, p);
            if (_mm_movemask_epi8(eq) == 0xFFFF)
            {
                // 8 px degismedi — seffaf
                _mm_storeu_si128((__m128i*)(pDst + x),     _mm_setzero_si128());
                _mm_storeu_si128((__m128i*)(pDst + x + 4), _mm_setzero_si128());
                continue;
            }
            bRowChanged = true;
            if (x < tightL)      tightL = x;
            if (x + 8 > tightR)  tightR = x + 8;
            const __m128i zero = _mm_setzero_si128();
            // eq word'leri 32-bit lane'e ac (esit lane = tum bitler 1) —
            // andnot ile esit px'ler 0'lanir, degisenler RGBA kalir
            _mm_storeu_si128((__m128i*)(pDst + x),
                _mm_andnot_si128(_mm_unpacklo_epi16(eq, eq),
                                 SC_Expand565x4(_mm_unpacklo_epi16(c, zero))));
            _mm_storeu_si128((__m128i*)(pDst + x + 4),
                _mm_andnot_si128(_mm_unpackhi_epi16(eq, eq),
                                 SC_Expand565x4(_mm_unpackhi_epi16(c, zero))));
        }
        for (; x < r.right; ++x)   // 8'lik blok kalani
        {
            const WORD cur = pComp[x];
            if (cur == pPrev[x])
            {
                pDst[x] = 0;              // degismedi → seffaf
            }
            else
            {
                pDst[x] = Expand565ToRGBA(cur);
                bRowChanged = true;
                if (x < tightL)      tightL = x;
                if (x + 1 > tightR)  tightR = x + 1;
            }
        }
        if (bRowChanged)
        {
            bAnyChanged = true;
            if (y < tightT) tightT = y;
            tightB = y + 1;
        }
        // PREV = comp (satirin dirty araligi)
        memcpy(pPrev + r.left, pComp + r.left, (size_t)(r.right - r.left) * 2);
    }

    // Faz G5 — hicbir pixel degismediyse komut/slot gereksiz (fullscreen
    // boundary diff'lerinin cogu boyle biter); GPU'ya hicbir sey tasinmaz
    if (!bAnyChanged)
    {
        g_scPerf.segEmpty++;
        g_scPerf.emitMs += SC_NowMs() - tEmit0;
        m_bDirty = false;
        return true;
    }

    // Siki kutu -> GPU. Kutu disindaki texture icerigi bayat kalabilir;
    // sorun degil: SEGMENT quad'i yalniz src=dest=siki kutuyu sample eder.
    D3D11_BOX box;
    box.left = (UINT)tightL;
    box.top = (UINT)tightT;
    box.right = (UINT)tightR;
    box.bottom = (UINT)tightB;
    box.front = 0;
    box.back = 1;
    m_pContext->UpdateSubresource(m_pSegTex[idx], 0, &box,
                                  m_pSegScratch + (size_t)tightT * m_Width + tightL,
                                  (UINT)m_Width * 4, 0);

    Cmd c;
    c.type = Cmd::SEGMENT;
    c.pSRV = m_pSegSRV[idx];
    c.texW = m_Width;
    c.texH = m_Height;
    c.dest.left = tightL;  c.dest.top = tightT;
    c.dest.right = tightR; c.dest.bottom = tightB;
    c.src = c.dest;
    c.colorRGBA = 0xFFFFFFFFu;
    c.blend = D3D11QuadPipeline::BLEND_ALPHA;
    ((SCCmdVec*)m_pCmds)->push_back(c);

    ++m_SegmentsUsed;
    m_bDirty = false;
    g_scPerf.segEmits++;
    g_scPerf.segPx += (__int64)(tightR - tightL) * (tightB - tightT);
    g_scPerf.emitMs += SC_NowMs() - tEmit0;
    return true;
}

bool D3D11SpriteCommands::RecordQuad(ID3D11ShaderResourceView* pSRV, int texW, int texH,
                                     const RECT& dest, const RECT& src,
                                     DWORD colorRGBA, D3D11QuadPipeline::BlendMode blend)
{
    if (!IsActive() || pSRV == NULL)
        return false;

    // CPU bu quad'dan once composition'a yazdiysa VE yazdigi bolge bu
    // quad'in dest'iyle KESISIYORSA aradaki katmani cikar. Kesismiyorsa
    // siralama sonucu etkilemez -> checkpoint ertelenir (2026-09-01 overflow
    // fix: oyun ici text/sprite interleave'i her frame 11+ segment tuketip
    // overflow'a dusuyordu -> frame ortasi CPU fallback karisimi menu ve
    // siyah bolgelerde nokta nokta karincalanma yapiyordu).
    // Son segment slotu frame-sonu final katmanina REZERVE (text kaybolmasin):
    // ara checkpoint'ler yalniz kMaxSegments-1'e kadar kullanabilir.
    // 2026-09-03 R4 — butce dolarsa CPU fallback yerine ara flush: birikmis
    // komutlar accum'a islenir, ring bosalir, kayit kesintisiz devam eder.
    if (m_bDirty && SC_RectsOverlap(m_DirtyRect, dest))
    {
        if (m_SegmentsUsed >= kMaxSegments - 1 && !FlushToAccum())
        {
            m_bOverflow = true;
            return false;
        }
        if (!EmitSegment())
        {
            m_bOverflow = true;
            return false;
        }
    }

    Cmd c;
    c.type = Cmd::QUAD;
    c.pSRV = pSRV;
    // 2026-09-02 crash fix — cache eviction/replace'e karsi yasam suresi
    // garantisi (bkz. SC_ReleaseQuadRefs aciklamasi)
    pSRV->AddRef();
    c.texW = texW;
    c.texH = texH;
    c.dest = dest;
    c.src = src;
    c.colorRGBA = colorRGBA;
    c.blend = blend;
    ((SCCmdVec*)m_pCmds)->push_back(c);

    ++m_QuadsThisFrame;
    return true;
}

bool D3D11SpriteCommands::RecordSolidQuad(const RECT& dest, DWORD colorRGBA,
                                          D3D11QuadPipeline::BlendMode blend)
{
    if (!IsActive())
        return false;

    // 2026-09-01 overflow fix — kesisim yoksa checkpoint erteleme (bkz.
    // RecordQuad aciklamasi). 2026-09-03 R4 — butce dolarsa ara flush.
    if (m_bDirty && SC_RectsOverlap(m_DirtyRect, dest))
    {
        if (m_SegmentsUsed >= kMaxSegments - 1 && !FlushToAccum())
        {
            m_bOverflow = true;
            return false;
        }
        if (!EmitSegment())
        {
            m_bOverflow = true;
            return false;
        }
    }

    Cmd c;
    c.type = Cmd::QUAD;
    c.pSRV = NULL;                 // NULL = solid (RenderFrame ayirt eder)
    c.texW = 1;
    c.texH = 1;
    c.dest = dest;
    c.src.left = 0; c.src.top = 0; c.src.right = 1; c.src.bottom = 1;
    c.colorRGBA = colorRGBA;
    c.blend = blend;
    ((SCCmdVec*)m_pCmds)->push_back(c);

    ++m_QuadsThisFrame;
    return true;
}

//-----------------------------------------------------------------------------
// RecordClear — 2026-09-02 cursor-iz fix (mimari aciklama header'da).
// Tam ekran opak fill: dirty birikimi atilir (fill ustunu ortuyordu),
// fullscreen solid quad kaydedilir, PREV fill rengine resetlenir.
//-----------------------------------------------------------------------------
bool D3D11SpriteCommands::RecordClear(WORD color565)
{
    if (!IsActive())
        return false;

    // Fill onceki CPU yazilarinin ustune biner — checkpoint gereksiz
    // (legacy'de de fill'den once yazilanlar gorunmezdi).
    m_bDirty = false;

    Cmd c;
    c.type = Cmd::QUAD;
    c.pSRV = NULL;                 // NULL = solid (RenderFrame ayirt eder)
    c.texW = 1;
    c.texH = 1;
    c.dest.left = 0;  c.dest.top = 0;
    c.dest.right = m_Width;  c.dest.bottom = m_Height;
    c.src.left = 0; c.src.top = 0; c.src.right = 1; c.src.bottom = 1;
    c.colorRGBA = Expand565ToRGBA(color565);
    c.blend = D3D11QuadPipeline::BLEND_OPAQUE;
    ((SCCmdVec*)m_pCmds)->push_back(c);
    ++m_QuadsThisFrame;

    // PREV = fill rengi: accum silindi, bu frame CPU'nun yeniden cizecegi
    // ayni-pixel icerik diff'te tekrar ciksin (caller comp'u da ayni renge
    // CPU-fill eder — comp/prev tutarli kalir).
    if (m_pPrev != NULL)
    {
        const size_t count = (size_t)m_Width * m_Height;
        for (size_t i = 0; i < count; ++i)
            m_pPrev[i] = color565;
    }

    return true;
}

//-----------------------------------------------------------------------------
// RecordLightFilter — Faz G4 (2026-09-01).
//
// CPU Phoenix_ApplyLightBufferFilter3DToSurface / P1 compare-darken'in
// dx11gpu karsiligi. Karartma carpimsaldir: keep = (256 - nibble*17)/256
// (CPU'daki (c*(256-alpha8)+128)>>8 formulu). Grid, keep faktoru gri RGBA8
// texture'a yazilir ve BLEND_MULTIPLY fullscreen quad'i olarak SIRALI
// komut listesine girer — o ana kadarki her sey karartilir, sonrasi (UI)
// dokunulmaz. Linear sample = CPU 8.8 hucre interpolasyonu paritesi.
//-----------------------------------------------------------------------------
bool D3D11SpriteCommands::RecordLightFilter(const BYTE* cells4, int fw, int fh,
                                            int destW, int destH)
{
    // Isik filtresi yalniz world (zone) ciziminde submit edilir — bu frame
    // world frame'idir (overflow olsa bile isaretle: RenderFrame baseline
    // sifirlamasi iz birakmayi onler).
    if (m_bInit)
        m_bWorldFrame = true;

    if (!IsActive() || cells4 == NULL || fw <= 0 || fh <= 0 ||
        destW <= 0 || destH <= 0)
        return false;

    // Bos grid — karartma yok, basarili no-op (CPU yolundaki anyRowAlpha
    // erken cikisiyla ayni davranis)
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
        return true;

    // World'un CPU'da cizilen HER SEYI (isim yazilari, bubble text —
    // hook'suz text yollari dahil) karartma ALTINDA kalmali → fullscreen
    // diff checkpoint (Faz G5: MarkDirty(NULL); degisen pixel yoksa
    // EmitSegment slot tuketmez)
    MarkDirty(NULL);
    if (m_SegmentsUsed >= kMaxSegments - 1 && !FlushToAccum())
    {
        m_bOverflow = true;
        return false;
    }
    if (!EmitSegment())
    {
        m_bOverflow = true;
        return false;
    }

    // Light texture — boyut degistiyse yeniden kur (RGBA8: MULTIPLY blend
    // dst*src.rgb okur, R8'de G/B kanallari 0 sample edilirdi)
    if (m_pLightTex == NULL || m_LightTexW != fw || m_LightTexH != fh)
    {
        SC_SafeRelease(m_pLightSRV);
        SC_SafeRelease(m_pLightTex);
        m_LightTexW = 0;
        m_LightTexH = 0;

        D3D11_TEXTURE2D_DESC td;
        ZeroMemory(&td, sizeof(td));
        td.Width          = fw;
        td.Height         = fh;
        td.MipLevels      = 1;
        td.ArraySize      = 1;
        td.Format         = DXGI_FORMAT_R8G8B8A8_UNORM;
        td.SampleDesc.Count = 1;
        td.Usage          = D3D11_USAGE_DYNAMIC;
        td.BindFlags      = D3D11_BIND_SHADER_RESOURCE;
        td.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        HRESULT hr = m_pDevice->CreateTexture2D(&td, NULL, &m_pLightTex);
        if (SUCCEEDED(hr))
            hr = m_pDevice->CreateShaderResourceView(m_pLightTex, NULL, &m_pLightSRV);
        if (FAILED(hr))
        {
            CT_Done("SpriteCommands: light tex FAILED 0x%08X (%dx%d)", hr, fw, fh);
            SC_SafeRelease(m_pLightSRV);
            SC_SafeRelease(m_pLightTex);
            return false;
        }
        m_LightTexW = fw;
        m_LightTexH = fh;
        CT_Done("SpriteCommands: light tex OK %dx%d", fw, fh);
    }

    // Grid yukle — nibble → keep faktoru (0..255 UNORM). Alpha=255:
    // MULTIPLY'in alpha-test PS'i (clip a<0.5) hicbir pikseli atmasin.
    D3D11_MAPPED_SUBRESOURCE map;
    HRESULT hr = m_pContext->Map(m_pLightTex, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
    if (FAILED(hr) || map.pData == NULL)
        return false;

    for (int y = 0; y < fh; ++y)
    {
        const BYTE* pS = cells4 + y * fw;
        DWORD*      pD = (DWORD*)((BYTE*)map.pData + (size_t)y * map.RowPitch);
        for (int x = 0; x < fw; ++x)
        {
            const int alpha8 = (pS[x] & 0x0F) * 17;
            // UNORM: sampled = keep/255; hedef carpan (256-alpha8)/256
            const DWORD keep = (DWORD)((255 * (256 - alpha8) + 128) / 256);
            pD[x] = 0xFF000000u | (keep << 16) | (keep << 8) | keep;
        }
    }
    m_pContext->Unmap(m_pLightTex, 0);

    Cmd c;
    c.type = Cmd::LIGHT;
    c.pSRV = m_pLightSRV;
    c.texW = fw;
    c.texH = fh;
    c.dest.left = 0;  c.dest.top = 0;
    c.dest.right = (destW < m_Width) ? destW : m_Width;
    c.dest.bottom = (destH < m_Height) ? destH : m_Height;
    c.src.left = 0;  c.src.top = 0;
    c.src.right = fw;  c.src.bottom = fh;
    c.colorRGBA = 0xFFFFFFFFu;
    c.blend = D3D11QuadPipeline::BLEND_MULTIPLY;
    ((SCCmdVec*)m_pCmds)->push_back(c);
    return true;
}

//-----------------------------------------------------------------------------
// RecordWorldBoundary — Faz G5 (2026-09-01).
//
// MTopView world cizimini bitirip UI'ya gecmeden once cagrilir
// (GPULightFilter_Deactivate hatti — her frame, gece/gunduz farketmez).
// Composition'daki CPU birikimi (dirty-mark'lanmis olsun olmasin: isim
// yazilari, bubble text, hook'suz yollar) fullscreen diff ile segment'e
// cikar. Boylece world icerigi UI quad'larinin ALTINDA kalir; final
// segment'e sarkip UI ustune binmez. Degisiklik yoksa slot tuketilmez.
//-----------------------------------------------------------------------------
bool D3D11SpriteCommands::RecordWorldBoundary()
{
    // MTopView zone cizimi bu cagriyi her oyun-ici frame'de yapar (gunduz/
    // gece farketmez) — frame world frame'i olarak isaretlenir. Overflow
    // gate'inden ONCE: overflow frame'inde de baseline sifirlamasi gerekir.
    if (m_bInit)
        m_bWorldFrame = true;

    if (!IsActive())
        return false;

    MarkDirty(NULL);
    if (m_SegmentsUsed >= kMaxSegments - 1 && !FlushToAccum())
    {
        m_bOverflow = true;
        return false;
    }
    if (!EmitSegment())
    {
        m_bOverflow = true;
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
// FlushToAccum — 2026-09-03 R4.
//
// Birikmis komutlari (gerekirse baseline dahil) accumulation RT'ye HEMEN
// isler, komut listesini bosaltir ve segment ring'ini ayni frame icinde
// yeniden kullanilabilir yapar. Iki cagiran var:
//   1. Record* yollari — segment butcesi dolunca (eski davranis: m_bOverflow
//      + frame sonuna kadar CPU fallback; kullanici-gorunur sonucu menu
//      icinde SIYAH BOLGELERDI: fallback'e dusen statik menu icerigi
//      sonraki frame'lerin diff'ine yakalanamiyor, yalniz baseline'da
//      kaliyor ve world quad'lari onu ortuyordu).
//   2. RenderFrame — frame sonu normal cizim ayni kod yolundan gecer.
//
// Segment texture'lari Map(WRITE_DISCARD) ile yeniden yazildigi icin GPU
// hazard yok (driver rename). Baseline karari frame'de bir kez verilir
// (m_bBaselineApplied); flush world-boundary'den once tetiklenirse (nadir)
// o frame baseline'siz devam eder — bir frame'lik iz riski, kalici degil.
//-----------------------------------------------------------------------------
bool D3D11SpriteCommands::FlushToAccum()
{
    if (!m_bInit)
        return false;

    D3D11QuadPipeline& qp = D3D11QuadPipeline::Instance();
    if (!qp.IsReady())
        return false;

    RECT full;
    full.left = 0;
    full.top = 0;
    full.right = m_Width;
    full.bottom = m_Height;

    // 0. Accum SRV onceki frame'de PS'e bagli kalmis olabilir — RTV olarak
    //    baglamadan once slotu temizle (D3D11 hazard).
    ID3D11ShaderResourceView* pNullSRV = NULL;
    m_pContext->PSSetShaderResources(0, 1, &pNullSRV);

    // 1. Baseline -> accum (frame'in ILK flush'inda):
    //    - Ilk frame / device recreate sonrasi: accum tohumlanir.
    //    - WORLD frame (RecordWorldBoundary/RecordLightFilter gordu):
    //      legacy oyun-ici semantigi — her frame composition tabaniyla
    //      SIFIRDAN cizilir; onceki frame'in GPU sprite'lari silinir
    //      (yoksa hareketli yaratiklar accum'da iz birakir). CPU'nun
    //      "silme" blit'leri composition'da ayni pikselleri yazdigi icin
    //      diff'e yakalanmaz — tek dogru siligici baseline'dir.
    //    - MENU frame'inde baseline cizilmez: accum kalicidir (bir kez
    //      cizilen arka plan sonraki frame'lerde yasamaya devam eder).
    //    - m_bWorldFrame artik world ciziminin BASINDA set edilir
    //      (MarkWorldFrame, MTopView::DrawZone girisi) — erken flush'ta
    //      karar dogru verilir (flicker fix, aciklama header'da).
    const double tFlush0 = SC_NowMs();   // 9.2 instrumentation
    if (!m_bBaselineApplied && (!m_bAccumSeeded || m_bWorldFrame))
    {
        const double tBl0 = SC_NowMs();
        D3D11_MAPPED_SUBRESOURCE map;
        if (FAILED(m_pContext->Map(m_pBaselineTex, 0, D3D11_MAP_WRITE_DISCARD, 0, &map)))
            return false;
        // 2026-09-03 FPS fix — SSE2 (SCPERF: blMs ~155ms/s idi, skaler
        // 2M px donusumu her world frame'de). Bit sonucu skalerle ayni.
        for (int y = 0; y < m_Height; ++y)
        {
            const WORD* pSrc = m_pBaseline + (size_t)y * m_Width;
            DWORD*      pDst = (DWORD*)((BYTE*)map.pData + (size_t)y * map.RowPitch);
            int x = 0;
            const __m128i zero = _mm_setzero_si128();
            for (; x + 8 <= m_Width; x += 8)
            {
                const __m128i s = _mm_loadu_si128((const __m128i*)(pSrc + x));
                _mm_storeu_si128((__m128i*)(pDst + x),
                                 SC_Expand565x4(_mm_unpacklo_epi16(s, zero)));
                _mm_storeu_si128((__m128i*)(pDst + x + 4),
                                 SC_Expand565x4(_mm_unpackhi_epi16(s, zero)));
            }
            for (; x < m_Width; ++x)
                pDst[x] = Expand565ToRGBA(pSrc[x]);
        }
        m_pContext->Unmap(m_pBaselineTex, 0);

        qp.BeginFrame(m_pAccumRTV, m_Width, m_Height);
        qp.DrawTexturedQuad(m_pBaselineSRV, m_Width, m_Height, full, full,
                            0xFFFFFFFFu, D3D11QuadPipeline::BLEND_OPAQUE);
        qp.Flush();
        m_bAccumSeeded = true;
        g_scPerf.baselineUps++;
        g_scPerf.baselineMs += SC_NowMs() - tBl0;
    }
    m_bBaselineApplied = true;

    // 2. Sirali cizim — accum RT uzerine (icerik birikir, legacy backbuffer)
    SCCmdVec& cmds = *(SCCmdVec*)m_pCmds;
    if (!cmds.empty())
    {
        qp.BeginFrame(m_pAccumRTV, m_Width, m_Height);
        for (size_t i = 0; i < cmds.size(); ++i)
        {
            const Cmd& c = cmds[i];
            if (c.type == Cmd::LIGHT)
            {
                // Faz G4 — isik grid'i linear sample edilir (hucre gecisleri
                // yumusak; CPU 8.8 interpolasyon paritesi), sonra frame'in
                // default sampler'ina donulur (O2: Option "Linear filtering")
                qp.SetLinearFilter(true);
                qp.DrawTexturedQuad(c.pSRV, c.texW, c.texH, c.dest, c.src,
                                    c.colorRGBA, c.blend);
                qp.SetLinearFilter(GetSpriteLinearFilter());
            }
            else if (c.pSRV != NULL)
                qp.DrawTexturedQuad(c.pSRV, c.texW, c.texH, c.dest, c.src,
                                    c.colorRGBA, c.blend);
            else
                qp.DrawSolidQuad(c.dest, c.colorRGBA, c.blend);
        }
        qp.Flush();

        // Islenen komutlarin SRV ref'leri birakilir, liste bosaltilir
        SC_ReleaseQuadRefs(cmds);
        cmds.clear();
    }

    // 3. Segment ring bosaldi — ayni frame'de yeniden kullanilabilir
    //    (Map DISCARD rename'i GPU hazard'i onler)
    m_SegmentsUsed = 0;
    g_scPerf.flushes++;
    g_scPerf.flushMs += SC_NowMs() - tFlush0;
    return true;
}

//-----------------------------------------------------------------------------
// RenderFrame — Present'ten cagrilir.
//-----------------------------------------------------------------------------
bool D3D11SpriteCommands::RenderFrame(ID3D11RenderTargetView* pRTV, float brightness)
{
    if (!m_bInit || pRTV == NULL || !IsD3D11GpuPathSelected())
        return false;

    D3D11QuadPipeline& qp = D3D11QuadPipeline::Instance();
    if (!qp.IsReady())
        return false;

    const double tRf0 = SC_NowMs();   // 9.2 instrumentation

    // Frame sonu: CPU'nun yazdiklari (text, portlanmamis Blt'ler, raw
    // GetSurfacePointer erisimleri — hepsi dirty-mark'lanamayabilir) final
    // segment olarak en uste gelir. Bu yuzden KOSULSUZ full-screen diff:
    // hook'suz yazilar da PREV karsilastirmasiyla yakalanir.
    // 2026-09-03 R4 — ring doluysa once ara flush (slot acilir).
    MarkDirty(NULL);
    if (m_SegmentsUsed >= kMaxSegments && !FlushToAccum())
        return false;
    if (!EmitSegment())
    {
        // Olmamali (flush slot acti) — final katmani kaybetmemek icin
        // frame'i normal CPU-composition present'e devret.
        CT_Done("SpriteCommands: final segment FAILED, CPU present fallback");
        return false;
    }

    // 2026-09-01 kalici backbuffer semantigi: komutlar accumulation RT'ye
    // cizilir (frame'ler arasi yasar), backbuffer accum'un kopyasini alir.
    // Boylece "bir kez ciz, flip'le" pattern'indeki legacy ekranlar (login/
    // select/char list) GPU quad iceriklerini sonraki frame'lerde kaybetmez.
    // R4 sonrasi baseline + komut cizimi FlushToAccum icinde.
    if (!FlushToAccum())
        return false;

    RECT full;
    full.left = 0;
    full.top = 0;
    full.right = m_Width;
    full.bottom = m_Height;

    // 3. Accum -> backbuffer opak kopya (BeginFrame yeni RTV baglar; accum
    //    RTV cozulur, SRV olarak sample edilebilir).
    //    2026-09-01 — Option "Aydinlatma" (gamma): D3D9 windowed post-process
    //    brightness paritesi. UI slider araligi 50..100 (yalniz karartma),
    //    modulate renkle birebir ifade edilir: out = accum * brightness.
    DWORD presentColor = 0xFFFFFFFFu;
    if (brightness < 0.999f)
    {
        int b8 = (int)(brightness * 255.0f + 0.5f);
        if (b8 < 0)   b8 = 0;
        if (b8 > 255) b8 = 255;
        presentColor = D3D11QuadPipeline::MakeColor((BYTE)b8, (BYTE)b8, (BYTE)b8, 255);
    }
    qp.BeginFrame(pRTV, m_Width, m_Height);
    qp.DrawTexturedQuad(m_pAccumSRV, m_Width, m_Height, full, full,
                        presentColor, D3D11QuadPipeline::BLEND_OPAQUE);
    qp.Flush();

    D3D11SpriteTextureCache::Instance().OnFrameEnd();

    // 9.2 instrumentation — 1 Hz ozet (frame basina disk I/O yok)
    g_scPerf.frames++;
    if (m_bWorldFrame)
        g_scPerf.worldFrames++;
    g_scPerf.quadsLast = m_QuadsThisFrame;
    g_scPerf.renderMs += SC_NowMs() - tRf0;
    {
        const DWORD now = GetTickCount();
        if (g_scPerf.lastLogTick == 0)
            g_scPerf.lastLogTick = now;
        else if (now - g_scPerf.lastLogTick >= 1000)
        {
            CT_OBJ("SCPERF: fr=%d wf=%d q=%d fl=%d blUp=%d segE=%d seg0=%d "
                   "px=%I64d | blMs=%.1f emMs=%.1f flMs=%.1f rfMs=%.1f snMs=%.1f",
                   g_scPerf.frames, g_scPerf.worldFrames, g_scPerf.quadsLast,
                   g_scPerf.flushes, g_scPerf.baselineUps, g_scPerf.segEmits,
                   g_scPerf.segEmpty, g_scPerf.segPx, g_scPerf.baselineMs,
                   g_scPerf.emitMs, g_scPerf.flushMs, g_scPerf.renderMs,
                   g_scPerf.snapMs);
            const DWORD keep = now;
            SCPerf zero = {};
            g_scPerf = zero;
            g_scPerf.lastLogTick = keep;
        }
    }
    return true;
}

void D3D11SpriteCommands::ResetFrame()
{
    if (!m_bInit)
        return;

    // 2026-09-02 crash fix — QUAD SRV ref'lerini birak (AddRef RecordQuad'da)
    SC_ReleaseQuadRefs(*(SCCmdVec*)m_pCmds);
    ((SCCmdVec*)m_pCmds)->clear();
    m_QuadsThisFrame = 0;
    m_SegmentsUsed = 0;
    m_bOverflow = false;
    m_bDirty = false;
    m_bWorldFrame = false;
    m_bBaselineApplied = false;   // 2026-09-03 R4 — baseline karari frame basi
    SnapshotComposition();
}

void D3D11SpriteCommands::SnapshotComposition()
{
    if (m_pCompBits == NULL || m_pBaseline == NULL)
        return;

    const double tSnap0 = SC_NowMs();   // 9.2 instrumentation
    for (int y = 0; y < m_Height; ++y)
    {
        const void* pSrc = (const BYTE*)m_pCompBits + (size_t)y * m_CompPitch;
        memcpy(m_pBaseline + (size_t)y * m_Width, pSrc, (size_t)m_Width * 2);
    }
    memcpy(m_pPrev, m_pBaseline, (size_t)m_Width * m_Height * 2);
    g_scPerf.snapMs += SC_NowMs() - tSnap0;
}

} // namespace render
} // namespace phoenix
