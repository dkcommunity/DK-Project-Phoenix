//-----------------------------------------------------------------------------
// D3D11QuadPipeline.h
//
// FULL DX11 gecis Faz G0 (2026-09-01) — GPU quad pipeline + sprite texture
// cache altyapisi ("dx11gpu" modu).
//
// P0/P1 backend'i CPU composition + GPU present idi. G fazlari sprite
// cizimini GPU'ya tasir: bu modul cizim ALTYAPISINI kurar, call-site
// baglantisi G1'de (Blt* komut katmani) yapilir. Bu dosya derlenir ama
// G1'e kadar kimse cagirmaz — davranis degisikligi SIFIR.
//
// Iki parca:
//   D3D11QuadPipeline      : batched textured/solid quad cizici.
//     - Dynamic VB ring (MAP_DISCARD/NO_OVERWRITE), triangle list
//     - Piksel koordinat -> NDC donusumu VS'te (cbuffer invW/invH)
//     - Blend modlari: OPAQUE / ALPHA (premultiplied degil, straight) /
//       ADDITIVE. Scissor destekli. Texture/blend/scissor degisince flush.
//     - Solid quad'lar 1x1 beyaz texture ile ayni shader'dan gecer.
//   D3D11SpriteTextureCache: 16-bit sprite pixel'ini (R5G6B5 + opsiyonel
//     colorkey) RGBA8 (BGRA) texture'a cevirip SRV olarak saklar.
//     - Anahtar: kaynak kimligi (pointer) + generation (icerik degisimi)
//     - LRU tahliye: bellek butcesi asilinca en eski frame'de kullanilan
//       entry'ler dusurulur.
//-----------------------------------------------------------------------------

#ifndef __PHOENIX_D3D11_QUAD_PIPELINE_H__
#define __PHOENIX_D3D11_QUAD_PIPELINE_H__

#include <windows.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11Buffer;
struct ID3D11SamplerState;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11RasterizerState;
struct ID3D11BlendState;
struct ID3D11DepthStencilState;

namespace phoenix
{
namespace render
{

//-----------------------------------------------------------------------------
// D3D11QuadPipeline
//-----------------------------------------------------------------------------
class D3D11QuadPipeline
{
public:
    enum BlendMode
    {
        BLEND_OPAQUE = 0,
        BLEND_ALPHA,      // straight alpha: src*a + dst*(1-a)
        BLEND_ADDITIVE,   // src*a + dst
        // Faz G3 (2026-09-01) — effect blend'leri (memcpyEffect* paritesi).
        // MULTIPLY ve MIN'de seffaf texel dst'yi bozar -> alpha-test PS
        // (clip a<0.5) otomatik devreye girer. SCREEN/MAX/INVMULT'ta
        // seffaf texel (rgb=0) dogal no-op'tur.
        BLEND_MULTIPLY,   // dst*src            (EffectDarker: d*s>>5)
        BLEND_INVMULT,    // dst*(1-src)        (EffectDodgeBurn: d*(32-s)>>5)
        BLEND_MIN,        // min(dst,src)       (EffectDarken)
        BLEND_MAX,        // max(dst,src)       (EffectLighten)
        BLEND_SCREEN,     // src+dst-src*dst    (EffectScreen tablo formulu)
        BLEND_MODE_COUNT
    };

    static D3D11QuadPipeline& Instance();

    // Backend device'i ile kurulur (D3D11Backend::Init sonrasi).
    bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Shutdown();
    bool IsReady() const { return m_pVS != NULL; }

    // Frame yasami: RT + viewport kur, batch state sifirla.
    void BeginFrame(ID3D11RenderTargetView* pRTV, int rtWidth, int rtHeight);
    // Birikmis quad'lari ciz (frame sonu veya state degisimi).
    void Flush();

    // Batch API — dest/src piksel koordinat (0,0 sol ust).
    // colorRGBA: 0xAABBGGRR degil — R,G,B,A ayri baytlar (RGBA siralamasi,
    // MakeColor ile uret). 0xFFFFFFFF = modulasyon yok.
    void DrawTexturedQuad(ID3D11ShaderResourceView* pSRV, int texW, int texH,
                          const RECT& dest, const RECT& src,
                          DWORD colorRGBA, BlendMode blend);
    void DrawSolidQuad(const RECT& dest, DWORD colorRGBA, BlendMode blend);

    // Scissor: NULL = kapat. Degisim flush tetikler.
    void SetScissor(const RECT* pRect);

    // Faz G4 (2026-09-01) — sampler secimi. Sprite'lar point (legacy 1:1),
    // isik grid'i linear (CPU 8.8 interpolasyon paritesi). Degisim flush
    // tetikler; BeginFrame point'e sifirlar.
    void SetLinearFilter(bool bLinear);

    static DWORD MakeColor(BYTE r, BYTE g, BYTE b, BYTE a)
    {
        return ((DWORD)r) | ((DWORD)g << 8) | ((DWORD)b << 16) | ((DWORD)a << 24);
    }

private:
    D3D11QuadPipeline();
    ~D3D11QuadPipeline();
    D3D11QuadPipeline(const D3D11QuadPipeline&);
    D3D11QuadPipeline& operator=(const D3D11QuadPipeline&);

    struct Vertex
    {
        float x, y;      // piksel
        float u, v;      // 0..1
        DWORD rgba;      // R8G8B8A8_UNORM input
    };

    void AppendQuad(ID3D11ShaderResourceView* pSRV,
                    const RECT& dest, float u0, float v0, float u1, float v1,
                    DWORD colorRGBA, BlendMode blend);
    void ApplyPipelineState();

    ID3D11Device*             m_pDevice;
    ID3D11DeviceContext*      m_pContext;

    ID3D11VertexShader*       m_pVS;
    ID3D11PixelShader*        m_pPS;
    ID3D11PixelShader*        m_pPSClip;       // Faz G3 — alpha-test varyanti
    ID3D11InputLayout*        m_pInputLayout;
    ID3D11Buffer*             m_pVB;           // dynamic ring
    ID3D11Buffer*             m_pCB;           // invW/invH
    ID3D11SamplerState*       m_pSamplerPoint;
    ID3D11SamplerState*       m_pSamplerLinear; // Faz G4 — isik grid'i icin
    ID3D11RasterizerState*    m_pRasterScissorOff;
    ID3D11RasterizerState*    m_pRasterScissorOn;
    ID3D11BlendState*         m_pBlendStates[BLEND_MODE_COUNT];
    ID3D11DepthStencilState*  m_pDepthOff;
    ID3D11Texture2D*          m_pWhiteTex;     // 1x1 beyaz (solid quad)
    ID3D11ShaderResourceView* m_pWhiteSRV;

    // Batch durumu
    Vertex*                   m_pCpuVerts;     // CPU staging (quad basina 6)
    int                       m_QuadCount;     // birikmis quad
    int                       m_VBOffsetQuads; // ring offset (NO_OVERWRITE)
    ID3D11ShaderResourceView* m_pCurrentSRV;
    BlendMode                 m_CurrentBlend;
    bool                      m_bLinearSampler; // Faz G4
    bool                      m_bScissorOn;
    RECT                      m_Scissor;
    int                       m_RTWidth;
    int                       m_RTHeight;
    bool                      m_bFrameActive;
};

//-----------------------------------------------------------------------------
// D3D11SpriteTextureCache
//-----------------------------------------------------------------------------
class D3D11SpriteTextureCache
{
public:
    static D3D11SpriteTextureCache& Instance();

    bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Shutdown();

    // 16-bit R5G6B5 pixel blogunu RGBA8 texture olarak dondurur (cache'li).
    //   key        : kaynak kimligi (sprite/surface pointer'i)
    //   generation : icerik degisim sayaci (ayni key + farkli gen = yeniden yukle)
    //   hasColorKey/colorKey: eslesen pikseller alpha=0 yazilir
    // Basarisizlikta NULL (caller CPU yoluna duser).
    ID3D11ShaderResourceView* GetOrCreate(const void* key, unsigned int generation,
                                          const WORD* pPixels, int width, int height,
                                          int pitchBytes,
                                          bool hasColorKey, WORD colorKey,
                                          int* pOutW = NULL, int* pOutH = NULL);

    // Faz G1 — iki adimli kullanim (decode maliyeti yalniz miss'te):
    //   Lookup hit → SRV; miss → caller RGBA decode edip InsertRGBA cagirir.
    // RGBA bellek sirasi R,G,B,A (DXGI_FORMAT_R8G8B8A8_UNORM).
    ID3D11ShaderResourceView* Lookup(const void* key, unsigned int generation,
                                     int* pOutW = NULL, int* pOutH = NULL);
    ID3D11ShaderResourceView* InsertRGBA(const void* key, unsigned int generation,
                                         const DWORD* pRGBA, int width, int height);

    // Kaynak yok edildiginde cagrilir (dangling SRV birakmamak icin).
    void Invalidate(const void* key);

    // Frame sonu: LRU tahliye (butce ustu + eskimis entry'ler).
    void OnFrameEnd();

    // Istatistik (ImGui HUD/G5 icin)
    size_t GetCacheBytes() const   { return m_TotalBytes; }
    int    GetCacheEntries() const { return m_EntryCount; }

    // impl .cpp'de tanimlanir (std::unordered_map header'a sizmasin);
    // public: cpp'deki namespace-scope map typedef'i erisebilsin diye.
    struct Entry;

private:
    D3D11SpriteTextureCache();
    ~D3D11SpriteTextureCache();
    D3D11SpriteTextureCache(const D3D11SpriteTextureCache&);
    D3D11SpriteTextureCache& operator=(const D3D11SpriteTextureCache&);
    void* m_pImpl;            // EntryMap*

    ID3D11Device*        m_pDevice;
    ID3D11DeviceContext* m_pContext;
    unsigned int         m_FrameCounter;
    size_t               m_TotalBytes;
    int                  m_EntryCount;
};

//-----------------------------------------------------------------------------
// "dx11gpu" modu — render_backend.cfg ilk satiri "dx11gpu" ise (veya env
// PHOENIX_RENDER_BACKEND=dx11gpu) sprite cizimi GPU quad yoluna gecer.
// IsD3D11Selected() bu degeri de dx11 kabul eder (backend ayni), bu bayrak
// yalnizca G1 komut katmaninin GPU yolunu acar.
//-----------------------------------------------------------------------------
bool IsD3D11GpuPathSelected();

} // namespace render
} // namespace phoenix

#endif // __PHOENIX_D3D11_QUAD_PIPELINE_H__
