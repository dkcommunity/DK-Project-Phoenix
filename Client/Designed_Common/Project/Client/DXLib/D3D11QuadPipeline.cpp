//-----------------------------------------------------------------------------
// D3D11QuadPipeline.cpp
//
// FULL DX11 gecis Faz G0 (2026-09-01) — GPU quad pipeline + sprite texture
// cache implementasyonu. Mimari aciklama: D3D11QuadPipeline.h
//-----------------------------------------------------------------------------

#include "D3D11QuadPipeline.h"
#include "../crash_trace_tool.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <emmintrin.h>   // SSE2 — 565 -> RGBA8 donusum
#include <stdio.h>
#include <string.h>
#include <unordered_map>
#include <vector>

namespace phoenix
{
namespace render
{

//-----------------------------------------------------------------------------
// 2026-09-02 O2 — Reborn Option runtime ayarlari (IRenderBackend.h bildirimleri).
// NOT: anonim namespace DISINDA — external linkage sart (vs_ui_lib + pacer
// bu sembolleri cagirir).
//-----------------------------------------------------------------------------
static bool g_bSpriteLinearDefault = false;  // frame'in default sampler'i
static bool g_bCacheHighBudget     = false;  // 256MB -> 512MB

void SetSpriteLinearFilter(bool bOn)   { g_bSpriteLinearDefault = bOn; }
bool GetSpriteLinearFilter()           { return g_bSpriteLinearDefault; }
void SetSpriteCacheHighBudget(bool bOn){ g_bCacheHighBudget = bOn; }
bool IsSpriteCacheHighBudget()         { return g_bCacheHighBudget; }

namespace
{

template <typename T>
inline void QP_SafeRelease(T*& p)
{
    if (p != NULL)
    {
        p->Release();
        p = NULL;
    }
}

// Batch kapasitesi: 4096 quad/flush (6 vertex/quad). VB ring 2x kapasite.
const int kMaxQuadsPerBatch = 4096;
const int kVertsPerQuad     = 6;
const int kVBQuadCapacity   = kMaxQuadsPerBatch * 2;

// Sprite cache butcesi
const size_t kSpriteCacheMaxBytes  = 256u * 1024u * 1024u;  // 256 MB (default)
const size_t kSpriteCacheHighBytes = 512u * 1024u * 1024u;  // 512 MB (Option: High)
const unsigned int kEvictAgeFrames = 600;                    // ~10 sn @60fps

inline size_t SpriteCacheBudget()
{
    return g_bCacheHighBudget ? kSpriteCacheHighBytes : kSpriteCacheMaxBytes;
}

//-----------------------------------------------------------------------------
// Quad shader — piksel koordinat VS'te NDC'ye cevrilir; PS tex * color.
//-----------------------------------------------------------------------------
const char* s_QuadShaderSource =
    "cbuffer QuadCB : register(b0)\n"
    "{\n"
    "    float4 g_Screen; // x=1/w, y=1/h, z,w=pad\n"
    "};\n"
    "Texture2D    g_Tex : register(t0);\n"
    "SamplerState g_Smp : register(s0);\n"
    "struct VSIn  { float2 pos : POSITION; float2 uv : TEXCOORD0; float4 col : COLOR0; };\n"
    "struct VSOut { float4 pos : SV_Position; float2 uv : TEXCOORD0; float4 col : COLOR0; };\n"
    "VSOut VSMain(VSIn i)\n"
    "{\n"
    "    VSOut o;\n"
    "    o.pos = float4(i.pos.x * g_Screen.x * 2.0 - 1.0,\n"
    "                   1.0 - i.pos.y * g_Screen.y * 2.0, 0.0, 1.0);\n"
    "    o.uv  = i.uv;\n"
    "    o.col = i.col;\n"
    "    return o;\n"
    "}\n"
    "float4 PSMain(VSOut i) : SV_Target\n"
    "{\n"
    "    return g_Tex.Sample(g_Smp, i.uv) * i.col;\n"
    "}\n"
    // Faz G3 - alpha-test varyanti: MULTIPLY/MIN blend'de seffaf texel
    // dst'yi karartir (0 ile min/carpim) -> a<0.5 pikseli tamamen at.
    "float4 PSMainClip(VSOut i) : SV_Target\n"
    "{\n"
    "    float4 t = g_Tex.Sample(g_Smp, i.uv);\n"
    "    clip(t.a - 0.5);\n"
    "    return t * i.col;\n"
    "}\n";

//-----------------------------------------------------------------------------
// SSE2: bir satir R5G6B5 -> R8G8B8A8 (RGBA bellek sirasi), opsiyonel colorkey
// (eslesen piksel tamamen seffaf: 0x00000000).
//-----------------------------------------------------------------------------
void ConvertRow565ToRGBA(const WORD* pSrc, DWORD* pDst, int count,
                         bool hasKey, WORD key)
{
    for (int x = 0; x < count; ++x)
    {
        const WORD px = pSrc[x];
        if (hasKey && px == key)
        {
            pDst[x] = 0;
            continue;
        }
        DWORD r = (px >> 11) & 0x1F;
        DWORD g = (px >> 5)  & 0x3F;
        DWORD b =  px        & 0x1F;
        r = (r << 3) | (r >> 2);
        g = (g << 2) | (g >> 4);
        b = (b << 3) | (b >> 2);
        // R8G8B8A8_UNORM bellek sirasi: R,G,B,A -> little-endian DWORD: A<<24|B<<16|G<<8|R
        pDst[x] = 0xFF000000u | (b << 16) | (g << 8) | r;
    }
}

} // namespace

//-----------------------------------------------------------------------------
// D3D11QuadPipeline
//-----------------------------------------------------------------------------

D3D11QuadPipeline::D3D11QuadPipeline()
    : m_pDevice(NULL)
    , m_pContext(NULL)
    , m_pVS(NULL)
    , m_pPS(NULL)
    , m_pPSClip(NULL)
    , m_pInputLayout(NULL)
    , m_pVB(NULL)
    , m_pCB(NULL)
    , m_pSamplerPoint(NULL)
    , m_pSamplerLinear(NULL)
    , m_pRasterScissorOff(NULL)
    , m_pRasterScissorOn(NULL)
    , m_pDepthOff(NULL)
    , m_pWhiteTex(NULL)
    , m_pWhiteSRV(NULL)
    , m_pCpuVerts(NULL)
    , m_QuadCount(0)
    , m_VBOffsetQuads(0)
    , m_pCurrentSRV(NULL)
    , m_CurrentBlend(BLEND_OPAQUE)
    , m_bLinearSampler(false)
    , m_bScissorOn(false)
    , m_RTWidth(0)
    , m_RTHeight(0)
    , m_bFrameActive(false)
{
    for (int i = 0; i < BLEND_MODE_COUNT; ++i)
        m_pBlendStates[i] = NULL;
    memset(&m_Scissor, 0, sizeof(m_Scissor));
}

D3D11QuadPipeline::~D3D11QuadPipeline()
{
    Shutdown();
}

D3D11QuadPipeline& D3D11QuadPipeline::Instance()
{
    static D3D11QuadPipeline s_Inst;
    return s_Inst;
}

bool D3D11QuadPipeline::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    if (m_pVS != NULL)
        return true;   // idempotent
    if (pDevice == NULL || pContext == NULL)
        return false;

    m_pDevice = pDevice;
    m_pContext = pContext;

    // 1. Shader
    ID3DBlob* pVSBlob = NULL;
    ID3DBlob* pPSBlob = NULL;
    ID3DBlob* pErr = NULL;

    HRESULT hr = D3DCompile(s_QuadShaderSource, strlen(s_QuadShaderSource),
                            "phoenix_dx11_quad", NULL, NULL,
                            "VSMain", "vs_4_0", 0, 0, &pVSBlob, &pErr);
    if (FAILED(hr))
    {
        CT_Done("QuadPipeline: VS compile FAILED 0x%08X (%s)", hr,
                pErr ? (const char*)pErr->GetBufferPointer() : "?");
        QP_SafeRelease(pErr);
        return false;
    }
    QP_SafeRelease(pErr);

    hr = D3DCompile(s_QuadShaderSource, strlen(s_QuadShaderSource),
                    "phoenix_dx11_quad", NULL, NULL,
                    "PSMain", "ps_4_0", 0, 0, &pPSBlob, &pErr);
    if (FAILED(hr))
    {
        CT_Done("QuadPipeline: PS compile FAILED 0x%08X (%s)", hr,
                pErr ? (const char*)pErr->GetBufferPointer() : "?");
        QP_SafeRelease(pErr);
        QP_SafeRelease(pVSBlob);
        return false;
    }
    QP_SafeRelease(pErr);

    hr = m_pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
                                       pVSBlob->GetBufferSize(), NULL, &m_pVS);
    if (SUCCEEDED(hr))
        hr = m_pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
                                          pPSBlob->GetBufferSize(), NULL, &m_pPS);

    // Faz G3 - alpha-test PS varyanti (MULTIPLY/MIN blend'leri icin)
    if (SUCCEEDED(hr))
    {
        ID3DBlob* pPSClipBlob = NULL;
        hr = D3DCompile(s_QuadShaderSource, strlen(s_QuadShaderSource),
                        "phoenix_dx11_quad", NULL, NULL,
                        "PSMainClip", "ps_4_0", 0, 0, &pPSClipBlob, &pErr);
        if (SUCCEEDED(hr))
            hr = m_pDevice->CreatePixelShader(pPSClipBlob->GetBufferPointer(),
                                              pPSClipBlob->GetBufferSize(), NULL, &m_pPSClip);
        else
            CT_Done("QuadPipeline: PSClip compile FAILED 0x%08X (%s)", hr,
                    pErr ? (const char*)pErr->GetBufferPointer() : "?");
        QP_SafeRelease(pErr);
        QP_SafeRelease(pPSClipBlob);
    }

    // 2. Input layout
    if (SUCCEEDED(hr))
    {
        const D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, 8,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        hr = m_pDevice->CreateInputLayout(layout, 3, pVSBlob->GetBufferPointer(),
                                          pVSBlob->GetBufferSize(), &m_pInputLayout);
    }
    QP_SafeRelease(pVSBlob);
    QP_SafeRelease(pPSBlob);
    if (FAILED(hr) || !m_pVS || !m_pPS || !m_pInputLayout)
    {
        CT_Done("QuadPipeline: shader/layout create FAILED 0x%08X", hr);
        Shutdown();
        return false;
    }

    // 3. Dynamic VB ring + CB
    D3D11_BUFFER_DESC vbd;
    ZeroMemory(&vbd, sizeof(vbd));
    vbd.ByteWidth      = kVBQuadCapacity * kVertsPerQuad * sizeof(Vertex);
    vbd.Usage          = D3D11_USAGE_DYNAMIC;
    vbd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = m_pDevice->CreateBuffer(&vbd, NULL, &m_pVB);
    if (FAILED(hr)) { CT_Done("QuadPipeline: VB FAILED 0x%08X", hr); Shutdown(); return false; }

    D3D11_BUFFER_DESC cbd;
    ZeroMemory(&cbd, sizeof(cbd));
    cbd.ByteWidth      = 16;
    cbd.Usage          = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = m_pDevice->CreateBuffer(&cbd, NULL, &m_pCB);
    if (FAILED(hr)) { CT_Done("QuadPipeline: CB FAILED 0x%08X", hr); Shutdown(); return false; }

    // 4. Sampler + raster (scissor on/off) + depth
    D3D11_SAMPLER_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.Filter   = D3D11_FILTER_MIN_MAG_MIP_POINT;   // legacy sprite = point sample
    sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.MaxLOD   = D3D11_FLOAT32_MAX;
    hr = m_pDevice->CreateSamplerState(&sd, &m_pSamplerPoint);
    if (FAILED(hr)) { CT_Done("QuadPipeline: sampler FAILED 0x%08X", hr); Shutdown(); return false; }

    // Faz G4 — linear varyant (isik grid'inin hucreler arasi yumusak gecisi;
    // CPU yolundaki 8.8 fixed-point interpolasyonun donanim karsiligi)
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    hr = m_pDevice->CreateSamplerState(&sd, &m_pSamplerLinear);
    if (FAILED(hr)) { CT_Done("QuadPipeline: linear sampler FAILED 0x%08X", hr); Shutdown(); return false; }

    D3D11_RASTERIZER_DESC rd;
    ZeroMemory(&rd, sizeof(rd));
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;
    rd.DepthClipEnable = TRUE;
    rd.ScissorEnable = FALSE;
    hr = m_pDevice->CreateRasterizerState(&rd, &m_pRasterScissorOff);
    if (SUCCEEDED(hr))
    {
        rd.ScissorEnable = TRUE;
        hr = m_pDevice->CreateRasterizerState(&rd, &m_pRasterScissorOn);
    }
    if (FAILED(hr)) { CT_Done("QuadPipeline: raster FAILED 0x%08X", hr); Shutdown(); return false; }

    D3D11_DEPTH_STENCIL_DESC dd;
    ZeroMemory(&dd, sizeof(dd));
    dd.DepthEnable = FALSE;
    dd.StencilEnable = FALSE;
    hr = m_pDevice->CreateDepthStencilState(&dd, &m_pDepthOff);
    if (FAILED(hr)) { CT_Done("QuadPipeline: depth FAILED 0x%08X", hr); Shutdown(); return false; }

    // 5. Blend state'ler
    D3D11_BLEND_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    // OPAQUE
    bd.RenderTarget[0].BlendEnable = FALSE;
    hr = m_pDevice->CreateBlendState(&bd, &m_pBlendStates[BLEND_OPAQUE]);

    // ALPHA (straight)
    if (SUCCEEDED(hr))
    {
        bd.RenderTarget[0].BlendEnable    = TRUE;
        bd.RenderTarget[0].SrcBlend       = D3D11_BLEND_SRC_ALPHA;
        bd.RenderTarget[0].DestBlend      = D3D11_BLEND_INV_SRC_ALPHA;
        bd.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
        bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        bd.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
        hr = m_pDevice->CreateBlendState(&bd, &m_pBlendStates[BLEND_ALPHA]);
    }

    // ADDITIVE
    if (SUCCEEDED(hr))
    {
        bd.RenderTarget[0].DestBlend      = D3D11_BLEND_ONE;
        bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        hr = m_pDevice->CreateBlendState(&bd, &m_pBlendStates[BLEND_ADDITIVE]);
    }

    // Faz G3 - effect blend'leri. Alpha kanalinda dst korunur (ZERO/ONE);
    // backbuffer alpha'si zaten kullanilmiyor.
    // MULTIPLY: out = dst * src   (EffectDarker d*s>>5)
    if (SUCCEEDED(hr))
    {
        bd.RenderTarget[0].SrcBlend       = D3D11_BLEND_ZERO;
        bd.RenderTarget[0].DestBlend      = D3D11_BLEND_SRC_COLOR;
        bd.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ZERO;
        bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        bd.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
        hr = m_pDevice->CreateBlendState(&bd, &m_pBlendStates[BLEND_MULTIPLY]);
    }

    // INVMULT: out = dst * (1-src)   (EffectDodgeBurn d*(32-s)>>5)
    if (SUCCEEDED(hr))
    {
        bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
        hr = m_pDevice->CreateBlendState(&bd, &m_pBlendStates[BLEND_INVMULT]);
    }

    // MIN: out = min(dst, src)   (EffectDarken; MIN/MAX'ta faktorler yok sayilir)
    if (SUCCEEDED(hr))
    {
        bd.RenderTarget[0].SrcBlend       = D3D11_BLEND_ONE;
        bd.RenderTarget[0].DestBlend      = D3D11_BLEND_ONE;
        bd.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_MIN;
        bd.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ZERO;
        bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        bd.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
        hr = m_pDevice->CreateBlendState(&bd, &m_pBlendStates[BLEND_MIN]);
    }

    // MAX: out = max(dst, src)   (EffectLighten; seffaf texel rgb=0 dogal no-op)
    if (SUCCEEDED(hr))
    {
        bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
        hr = m_pDevice->CreateBlendState(&bd, &m_pBlendStates[BLEND_MAX]);
    }

    // SCREEN: out = src*(1-dst) + dst = s+d-sd   (EffectScreen tablo formulu;
    // seffaf texel rgb=0 dogal no-op)
    if (SUCCEEDED(hr))
    {
        bd.RenderTarget[0].SrcBlend  = D3D11_BLEND_INV_DEST_COLOR;
        bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
        bd.RenderTarget[0].BlendOp   = D3D11_BLEND_OP_ADD;
        hr = m_pDevice->CreateBlendState(&bd, &m_pBlendStates[BLEND_SCREEN]);
    }
    if (FAILED(hr)) { CT_Done("QuadPipeline: blend FAILED 0x%08X", hr); Shutdown(); return false; }

    // 6. 1x1 beyaz texture (solid quad)
    const DWORD whitePx = 0xFFFFFFFFu;
    D3D11_TEXTURE2D_DESC td;
    ZeroMemory(&td, sizeof(td));
    td.Width = 1;
    td.Height = 1;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_IMMUTABLE;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    D3D11_SUBRESOURCE_DATA init;
    init.pSysMem = &whitePx;
    init.SysMemPitch = 4;
    init.SysMemSlicePitch = 0;
    hr = m_pDevice->CreateTexture2D(&td, &init, &m_pWhiteTex);
    if (SUCCEEDED(hr))
        hr = m_pDevice->CreateShaderResourceView(m_pWhiteTex, NULL, &m_pWhiteSRV);
    if (FAILED(hr)) { CT_Done("QuadPipeline: white tex FAILED 0x%08X", hr); Shutdown(); return false; }

    // 7. CPU staging
    m_pCpuVerts = new Vertex[kMaxQuadsPerBatch * kVertsPerQuad];

    CT_Done("QuadPipeline: init OK (batch=%d quad)", kMaxQuadsPerBatch);
    return true;
}

void D3D11QuadPipeline::Shutdown()
{
    delete[] m_pCpuVerts;
    m_pCpuVerts = NULL;

    QP_SafeRelease(m_pWhiteSRV);
    QP_SafeRelease(m_pWhiteTex);
    for (int i = 0; i < BLEND_MODE_COUNT; ++i)
        QP_SafeRelease(m_pBlendStates[i]);
    QP_SafeRelease(m_pDepthOff);
    QP_SafeRelease(m_pRasterScissorOn);
    QP_SafeRelease(m_pRasterScissorOff);
    QP_SafeRelease(m_pSamplerLinear);
    QP_SafeRelease(m_pSamplerPoint);
    QP_SafeRelease(m_pCB);
    QP_SafeRelease(m_pVB);
    QP_SafeRelease(m_pInputLayout);
    QP_SafeRelease(m_pPSClip);
    QP_SafeRelease(m_pPS);
    QP_SafeRelease(m_pVS);

    m_pDevice = NULL;
    m_pContext = NULL;
    m_QuadCount = 0;
    m_bFrameActive = false;
}

void D3D11QuadPipeline::BeginFrame(ID3D11RenderTargetView* pRTV, int rtWidth, int rtHeight)
{
    if (!IsReady() || m_pContext == NULL || pRTV == NULL)
        return;

    m_RTWidth = rtWidth;
    m_RTHeight = rtHeight;
    m_QuadCount = 0;
    m_pCurrentSRV = NULL;
    m_CurrentBlend = BLEND_OPAQUE;
    // Faz G4: frame point sampler ile baslardi; O2 sonrasi default'u Option
    // "Linear texture filtering" belirler (pixel-perfect secilirse false).
    m_bLinearSampler = g_bSpriteLinearDefault;
    m_bScissorOn = false;
    m_bFrameActive = true;

    // Screen sabitleri
    D3D11_MAPPED_SUBRESOURCE map;
    if (SUCCEEDED(m_pContext->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &map)))
    {
        float* f = (float*)map.pData;
        f[0] = (rtWidth  > 0) ? (1.0f / rtWidth)  : 0.0f;
        f[1] = (rtHeight > 0) ? (1.0f / rtHeight) : 0.0f;
        f[2] = 0.0f;
        f[3] = 0.0f;
        m_pContext->Unmap(m_pCB, 0);
    }

    m_pContext->OMSetRenderTargets(1, &pRTV, NULL);

    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    vp.Width    = (float)rtWidth;
    vp.Height   = (float)rtHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    m_pContext->RSSetViewports(1, &vp);
}

void D3D11QuadPipeline::ApplyPipelineState()
{
    const UINT stride = sizeof(Vertex);
    const UINT offset = 0;

    m_pContext->IASetInputLayout(m_pInputLayout);
    m_pContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pContext->VSSetShader(m_pVS, NULL, 0);
    m_pContext->VSSetConstantBuffers(0, 1, &m_pCB);
    // Faz G3 - MULTIPLY/MIN'de seffaf texel dst'yi bozar -> alpha-test PS
    const bool bNeedClip = (m_CurrentBlend == BLEND_MULTIPLY ||
                            m_CurrentBlend == BLEND_MIN) && m_pPSClip != NULL;
    m_pContext->PSSetShader(bNeedClip ? m_pPSClip : m_pPS, NULL, 0);
    // Faz G4 — isik grid'i linear, sprite'lar point sample edilir
    m_pContext->PSSetSamplers(0, 1,
        (m_bLinearSampler && m_pSamplerLinear != NULL) ? &m_pSamplerLinear
                                                       : &m_pSamplerPoint);
    m_pContext->OMSetDepthStencilState(m_pDepthOff, 0);

    const float blendFactor[4] = { 0, 0, 0, 0 };
    m_pContext->OMSetBlendState(m_pBlendStates[m_CurrentBlend], blendFactor, 0xFFFFFFFF);

    if (m_bScissorOn)
    {
        D3D11_RECT rc;
        rc.left   = m_Scissor.left;
        rc.top    = m_Scissor.top;
        rc.right  = m_Scissor.right;
        rc.bottom = m_Scissor.bottom;
        m_pContext->RSSetScissorRects(1, &rc);
        m_pContext->RSSetState(m_pRasterScissorOn);
    }
    else
    {
        m_pContext->RSSetState(m_pRasterScissorOff);
    }

    m_pContext->PSSetShaderResources(0, 1, &m_pCurrentSRV);
}

void D3D11QuadPipeline::Flush()
{
    if (!m_bFrameActive || m_QuadCount <= 0 || m_pContext == NULL)
    {
        m_QuadCount = 0;
        return;
    }

    // Ring VB'ye yaz: sigmiyorsa DISCARD ile basa sar
    D3D11_MAP mapType = D3D11_MAP_WRITE_NO_OVERWRITE;
    if (m_VBOffsetQuads + m_QuadCount > kVBQuadCapacity)
    {
        m_VBOffsetQuads = 0;
        mapType = D3D11_MAP_WRITE_DISCARD;
    }

    D3D11_MAPPED_SUBRESOURCE map;
    HRESULT hr = m_pContext->Map(m_pVB, 0, mapType, 0, &map);
    if (FAILED(hr))
    {
        CT_Done("QuadPipeline: VB map FAILED 0x%08X", hr);
        m_QuadCount = 0;
        return;
    }

    Vertex* pDst = (Vertex*)map.pData + m_VBOffsetQuads * kVertsPerQuad;
    memcpy(pDst, m_pCpuVerts, m_QuadCount * kVertsPerQuad * sizeof(Vertex));
    m_pContext->Unmap(m_pVB, 0);

    ApplyPipelineState();
    m_pContext->Draw(m_QuadCount * kVertsPerQuad, m_VBOffsetQuads * kVertsPerQuad);

    m_VBOffsetQuads += m_QuadCount;
    m_QuadCount = 0;
}

void D3D11QuadPipeline::AppendQuad(ID3D11ShaderResourceView* pSRV,
                                   const RECT& dest, float u0, float v0,
                                   float u1, float v1,
                                   DWORD colorRGBA, BlendMode blend)
{
    if (!m_bFrameActive || pSRV == NULL)
        return;

    // State degisimi -> once flush
    if ((m_pCurrentSRV != NULL && m_pCurrentSRV != pSRV) ||
        (m_QuadCount > 0 && m_CurrentBlend != blend) ||
        m_QuadCount >= kMaxQuadsPerBatch)
    {
        Flush();
    }
    m_pCurrentSRV = pSRV;
    m_CurrentBlend = blend;

    const float x0 = (float)dest.left;
    const float y0 = (float)dest.top;
    const float x1 = (float)dest.right;
    const float y1 = (float)dest.bottom;

    Vertex* v = m_pCpuVerts + m_QuadCount * kVertsPerQuad;
    v[0].x = x0; v[0].y = y0; v[0].u = u0; v[0].v = v0; v[0].rgba = colorRGBA;
    v[1].x = x1; v[1].y = y0; v[1].u = u1; v[1].v = v0; v[1].rgba = colorRGBA;
    v[2].x = x0; v[2].y = y1; v[2].u = u0; v[2].v = v1; v[2].rgba = colorRGBA;
    v[3].x = x1; v[3].y = y0; v[3].u = u1; v[3].v = v0; v[3].rgba = colorRGBA;
    v[4].x = x1; v[4].y = y1; v[4].u = u1; v[4].v = v1; v[4].rgba = colorRGBA;
    v[5].x = x0; v[5].y = y1; v[5].u = u0; v[5].v = v1; v[5].rgba = colorRGBA;
    ++m_QuadCount;
}

void D3D11QuadPipeline::DrawTexturedQuad(ID3D11ShaderResourceView* pSRV,
                                         int texW, int texH,
                                         const RECT& dest, const RECT& src,
                                         DWORD colorRGBA, BlendMode blend)
{
    if (texW <= 0 || texH <= 0)
        return;
    const float invW = 1.0f / (float)texW;
    const float invH = 1.0f / (float)texH;
    AppendQuad(pSRV, dest,
               src.left * invW, src.top * invH,
               src.right * invW, src.bottom * invH,
               colorRGBA, blend);
}

void D3D11QuadPipeline::DrawSolidQuad(const RECT& dest, DWORD colorRGBA, BlendMode blend)
{
    AppendQuad(m_pWhiteSRV, dest, 0.0f, 0.0f, 1.0f, 1.0f, colorRGBA, blend);
}

void D3D11QuadPipeline::SetScissor(const RECT* pRect)
{
    const bool wantOn = (pRect != NULL);
    if (wantOn == m_bScissorOn &&
        (!wantOn || (pRect->left == m_Scissor.left && pRect->top == m_Scissor.top &&
                     pRect->right == m_Scissor.right && pRect->bottom == m_Scissor.bottom)))
        return;   // degisiklik yok

    Flush();
    m_bScissorOn = wantOn;
    if (wantOn)
        m_Scissor = *pRect;
}

// Faz G4 (2026-09-01) — sampler degisimi batch sinirini zorlar
void D3D11QuadPipeline::SetLinearFilter(bool bLinear)
{
    if (bLinear == m_bLinearSampler)
        return;
    Flush();
    m_bLinearSampler = bLinear;
}

//-----------------------------------------------------------------------------
// D3D11SpriteTextureCache
//-----------------------------------------------------------------------------

struct D3D11SpriteTextureCache::Entry
{
    ID3D11Texture2D*          pTex;
    ID3D11ShaderResourceView* pSRV;
    int                       width;
    int                       height;
    unsigned int              generation;
    unsigned int              lastUseFrame;
    size_t                    bytes;
};

typedef std::unordered_map<const void*, D3D11SpriteTextureCache::Entry> QPEntryMap;

D3D11SpriteTextureCache::D3D11SpriteTextureCache()
    : m_pImpl(new QPEntryMap())
    , m_pDevice(NULL)
    , m_pContext(NULL)
    , m_FrameCounter(0)
    , m_TotalBytes(0)
    , m_EntryCount(0)
{
}

D3D11SpriteTextureCache::~D3D11SpriteTextureCache()
{
    Shutdown();
    delete (QPEntryMap*)m_pImpl;
    m_pImpl = NULL;
}

D3D11SpriteTextureCache& D3D11SpriteTextureCache::Instance()
{
    static D3D11SpriteTextureCache s_Inst;
    return s_Inst;
}

bool D3D11SpriteTextureCache::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    if (pDevice == NULL || pContext == NULL)
        return false;
    m_pDevice = pDevice;
    m_pContext = pContext;
    return true;
}

void D3D11SpriteTextureCache::Shutdown()
{
    QPEntryMap* pMap = (QPEntryMap*)m_pImpl;
    if (pMap != NULL)
    {
        for (QPEntryMap::iterator it = pMap->begin(); it != pMap->end(); ++it)
        {
            QP_SafeRelease(it->second.pSRV);
            QP_SafeRelease(it->second.pTex);
        }
        pMap->clear();
    }
    m_TotalBytes = 0;
    m_EntryCount = 0;
    m_pDevice = NULL;
    m_pContext = NULL;
}

ID3D11ShaderResourceView* D3D11SpriteTextureCache::GetOrCreate(
    const void* key, unsigned int generation,
    const WORD* pPixels, int width, int height, int pitchBytes,
    bool hasColorKey, WORD colorKey, int* pOutW, int* pOutH)
{
    if (m_pDevice == NULL || key == NULL || pPixels == NULL ||
        width <= 0 || height <= 0)
        return NULL;

    QPEntryMap& map = *(QPEntryMap*)m_pImpl;

    QPEntryMap::iterator it = map.find(key);
    if (it != map.end())
    {
        Entry& e = it->second;
        if (e.generation == generation && e.width == width && e.height == height)
        {
            e.lastUseFrame = m_FrameCounter;
            if (pOutW) *pOutW = e.width;
            if (pOutH) *pOutH = e.height;
            return e.pSRV;
        }
        // Icerik degisti — eskisini dusur, yeniden yukle
        m_TotalBytes -= e.bytes;
        --m_EntryCount;
        QP_SafeRelease(e.pSRV);
        QP_SafeRelease(e.pTex);
        map.erase(it);
    }

    // 565 -> RGBA8 donusum (satir satir; colorkey alpha'ya cevrilir)
    std::vector<DWORD> rgba((size_t)width * height);
    for (int y = 0; y < height; ++y)
    {
        const WORD* pSrcRow = (const WORD*)((const BYTE*)pPixels + y * pitchBytes);
        ConvertRow565ToRGBA(pSrcRow, &rgba[(size_t)y * width], width,
                            hasColorKey, colorKey);
    }

    D3D11_TEXTURE2D_DESC td;
    ZeroMemory(&td, sizeof(td));
    td.Width = width;
    td.Height = height;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_IMMUTABLE;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA init;
    init.pSysMem = &rgba[0];
    init.SysMemPitch = width * 4;
    init.SysMemSlicePitch = 0;

    Entry e;
    e.pTex = NULL;
    e.pSRV = NULL;
    e.width = width;
    e.height = height;
    e.generation = generation;
    e.lastUseFrame = m_FrameCounter;
    e.bytes = (size_t)width * height * 4;

    HRESULT hr = m_pDevice->CreateTexture2D(&td, &init, &e.pTex);
    if (SUCCEEDED(hr) && e.pTex)
        hr = m_pDevice->CreateShaderResourceView(e.pTex, NULL, &e.pSRV);
    if (FAILED(hr) || e.pSRV == NULL)
    {
        CT_Done("SpriteTexCache: create FAILED 0x%08X (%dx%d)", hr, width, height);
        QP_SafeRelease(e.pSRV);
        QP_SafeRelease(e.pTex);
        return NULL;
    }

    map[key] = e;
    m_TotalBytes += e.bytes;
    ++m_EntryCount;

    if (pOutW) *pOutW = width;
    if (pOutH) *pOutH = height;
    return e.pSRV;
}

//-----------------------------------------------------------------------------
// Faz G1 — iki adimli cache erisimi (decode maliyeti yalniz miss'te)
//-----------------------------------------------------------------------------
ID3D11ShaderResourceView* D3D11SpriteTextureCache::Lookup(const void* key,
                                                          unsigned int generation,
                                                          int* pOutW, int* pOutH)
{
    if (m_pDevice == NULL || key == NULL)
        return NULL;

    QPEntryMap& map = *(QPEntryMap*)m_pImpl;
    QPEntryMap::iterator it = map.find(key);
    if (it == map.end() || it->second.generation != generation)
        return NULL;

    Entry& e = it->second;
    e.lastUseFrame = m_FrameCounter;
    if (pOutW) *pOutW = e.width;
    if (pOutH) *pOutH = e.height;
    return e.pSRV;
}

ID3D11ShaderResourceView* D3D11SpriteTextureCache::InsertRGBA(const void* key,
                                                              unsigned int generation,
                                                              const DWORD* pRGBA,
                                                              int width, int height)
{
    if (m_pDevice == NULL || key == NULL || pRGBA == NULL ||
        width <= 0 || height <= 0)
        return NULL;

    QPEntryMap& map = *(QPEntryMap*)m_pImpl;

    // Ayni key'in eski/bayat entry'sini dusur
    QPEntryMap::iterator it = map.find(key);
    if (it != map.end())
    {
        m_TotalBytes -= it->second.bytes;
        --m_EntryCount;
        QP_SafeRelease(it->second.pSRV);
        QP_SafeRelease(it->second.pTex);
        map.erase(it);
    }

    D3D11_TEXTURE2D_DESC td;
    ZeroMemory(&td, sizeof(td));
    td.Width = width;
    td.Height = height;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_IMMUTABLE;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA init;
    init.pSysMem = pRGBA;
    init.SysMemPitch = width * 4;
    init.SysMemSlicePitch = 0;

    Entry e;
    e.pTex = NULL;
    e.pSRV = NULL;
    e.width = width;
    e.height = height;
    e.generation = generation;
    e.lastUseFrame = m_FrameCounter;
    e.bytes = (size_t)width * height * 4;

    HRESULT hr = m_pDevice->CreateTexture2D(&td, &init, &e.pTex);
    if (SUCCEEDED(hr) && e.pTex)
        hr = m_pDevice->CreateShaderResourceView(e.pTex, NULL, &e.pSRV);
    if (FAILED(hr) || e.pSRV == NULL)
    {
        CT_Done("SpriteTexCache: InsertRGBA FAILED 0x%08X (%dx%d)", hr, width, height);
        QP_SafeRelease(e.pSRV);
        QP_SafeRelease(e.pTex);
        return NULL;
    }

    map[key] = e;
    m_TotalBytes += e.bytes;
    ++m_EntryCount;
    return e.pSRV;
}

void D3D11SpriteTextureCache::Invalidate(const void* key)
{
    QPEntryMap& map = *(QPEntryMap*)m_pImpl;
    QPEntryMap::iterator it = map.find(key);
    if (it == map.end())
        return;
    m_TotalBytes -= it->second.bytes;
    --m_EntryCount;
    QP_SafeRelease(it->second.pSRV);
    QP_SafeRelease(it->second.pTex);
    map.erase(it);
}

void D3D11SpriteTextureCache::OnFrameEnd()
{
    ++m_FrameCounter;

    const size_t budget = SpriteCacheBudget();
    if (m_TotalBytes <= budget)
        return;

    // Butce asildi: once eskimis entry'ler (kEvictAgeFrames), yetmezse
    // en eski kullanim sirasiyla dusur.
    QPEntryMap& map = *(QPEntryMap*)m_pImpl;

    for (QPEntryMap::iterator it = map.begin();
         it != map.end() && m_TotalBytes > budget; )
    {
        if (m_FrameCounter - it->second.lastUseFrame > kEvictAgeFrames)
        {
            m_TotalBytes -= it->second.bytes;
            --m_EntryCount;
            QP_SafeRelease(it->second.pSRV);
            QP_SafeRelease(it->second.pTex);
            it = map.erase(it);
        }
        else
        {
            ++it;
        }
    }

    while (m_TotalBytes > budget && !map.empty())
    {
        QPEntryMap::iterator oldest = map.begin();
        for (QPEntryMap::iterator it = map.begin(); it != map.end(); ++it)
        {
            if (it->second.lastUseFrame < oldest->second.lastUseFrame)
                oldest = it;
        }
        m_TotalBytes -= oldest->second.bytes;
        --m_EntryCount;
        QP_SafeRelease(oldest->second.pSRV);
        QP_SafeRelease(oldest->second.pTex);
        map.erase(oldest);
    }
}

//-----------------------------------------------------------------------------
// dx11gpu modu secimi
//-----------------------------------------------------------------------------
bool IsD3D11GpuPathSelected()
{
    static int s_Selected = -1;
    if (s_Selected >= 0)
        return s_Selected == 1;

    s_Selected = 0;

    char envVal[32] = { 0 };
    DWORD n = GetEnvironmentVariableA("PHOENIX_RENDER_BACKEND", envVal, sizeof(envVal));
    if (n > 0 && n < sizeof(envVal))
    {
        if (_stricmp(envVal, "dx11gpu") == 0)
            s_Selected = 1;
        CT_Done("Phoenix GPU sprite yolu (env): %s", s_Selected ? "ACIK" : "kapali");
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
            _snprintf(cfgPath, MAX_PATH - 1, "%srender_backend.cfg", modPath);
            cfgPath[MAX_PATH - 1] = '\0';

            FILE* f = fopen(cfgPath, "rt");
            if (f != NULL)
            {
                char line[64] = { 0 };
                if (fgets(line, sizeof(line), f) != NULL)
                {
                    for (int i = (int)strlen(line) - 1;
                         i >= 0 && (line[i] == '\n' || line[i] == '\r' ||
                                    line[i] == ' ' || line[i] == '\t');
                         --i)
                        line[i] = '\0';
                    if (_stricmp(line, "dx11gpu") == 0)
                        s_Selected = 1;
                }
                fclose(f);
            }
        }
    }

    CT_Done("Phoenix GPU sprite yolu: %s (render_backend.cfg)",
            s_Selected ? "ACIK (dx11gpu)" : "kapali");
    return s_Selected == 1;
}

} // namespace render
} // namespace phoenix
