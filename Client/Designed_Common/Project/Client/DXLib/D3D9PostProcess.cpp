//-----------------------------------------------------------------------------
// D3D9PostProcess.cpp
//
// Phoenix Phase 7 (2026-05-14) — vignette pixel shader.
//
// HLSL kaynak inline string olarak compile edilir (D3DCompile, d3dcompiler.dll
// Windows SDK shipped). Pre-compiled .fxc kullanmiyoruz — build adimi yok.
// Target ps_2_0: GeForce FX 5200+ + GeForce 6000/7000/8000+ + her modern GPU.
//
// State'ler:
//   m_pPS: compiled IDirect3DPixelShader9
//   m_bEnabled: toggle (F11)
//
// Hata durumda (compile fail, device NULL): m_pPS NULL kalir, Apply no-op.
//-----------------------------------------------------------------------------

#include "D3D9PostProcess.h"
#include "../crash_trace_tool.h"
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

namespace
{
    IDirect3DPixelShader9* g_pPS       = NULL;
    bool                   g_bEnabled  = false; // 2026-08-29: post-process default OFF (CRT scanline kaldirildi)

    // 2026-05-16: Per-effect runtime toggle (Option > OTHER tab).
    // UserOption load'dan sonra SetVignetteEnabled cagrilir.
    bool                   g_bVignetteEnabled = false;
    float                  g_fBrightnessScale = 1.0f;

    // Inline HLSL — vignette + brightness.
    // ps_2_0: 64 instruction slot, basic ALU. Yeterli (~15 instruction).
    //
    // Composition buffer 16-bit R5G6B5 — sampler birim olarak okur, float
    // RGB'ye expand eder (donanim), shader 32-bit float-precision ile islem,
    // back buffer 32-bit X8R8G8B8'e yazar (Present). Renk derinligi sinifi
    // korunur (1:1 piksel).
    //
    // Vignette: ekran merkezinden uzaklasinca smoothstep ile karart
    // (g_VignetteRadius'tan 1.0'a karariyor; g_VignetteIntensity max %50).
    //
    // 2026-05-16: Per-effect toggle — c0 float register kullanilir:
    //   c0.x : vignette weight (0 = devre disi, 1 = aktif)
    //   c0.y : brightness scale (1 = neutral, 0.5 karanlik, 1.5 aydinlik)
    // weight=0 oldugunda lerp() vignette efektini "1.0" (no-op) baseline ile
    // karistirir — efekt görünmez ama shader degisimi olmaz.
    //
    // Cikti: composition_color * vignette (selectif).
    const char* g_ShaderHLSL =
        "sampler2D s_composition : register(s0);\n"
        "float4    g_Effects     : register(c0);\n"   // x=vignette weight, y=brightness scale
        "\n"
        "static const float VIGNETTE_RADIUS    = 0.65;\n"
        "static const float VIGNETTE_INTENSITY = 0.45;\n"
        "\n"
        "float4 main(float2 uv : TEXCOORD0) : COLOR0\n"
        "{\n"
        "    float4 color = tex2D(s_composition, uv);\n"
        "    color.rgb = saturate(color.rgb * g_Effects.y);\n"
        "\n"
        "    // Vignette — radial darken from screen center (weight=g_Effects.x)\n"
        "    float2 center = uv - 0.5;\n"
        "    float dist = length(center);\n"
        "    float vignette_raw = 1.0 - smoothstep(VIGNETTE_RADIUS, 1.0, dist) * VIGNETTE_INTENSITY;\n"
        "    float vignette = lerp(1.0, vignette_raw, g_Effects.x);\n"
        "    color.rgb *= vignette;\n"
        "\n"
        "    return color;\n"
        "}\n";
}

bool D3D9PostProcess_Init(IDirect3DDevice9* pDevice)
{
    if (!pDevice)
    {
        CT_Done("D3D9PostProcess::Init FAIL — device NULL");
        return false;
    }

    if (g_pPS)
    {
        // Zaten init edilmis — re-init sessiz no-op
        return true;
    }

    ID3DBlob* pCode  = NULL;
    ID3DBlob* pError = NULL;

    HRESULT hr = D3DCompile(
        g_ShaderHLSL,
        strlen(g_ShaderHLSL),
        "phoenix_postprocess.hlsl",   // virtual filename for error reports
        NULL,                          // no macros
        NULL,                          // no includes
        "main",                        // entry point
        "ps_2_0",                      // target shader model
        0,                             // Flags1
        0,                             // Flags2
        &pCode,
        &pError);

    if (FAILED(hr))
    {
        if (pError)
        {
            CT_Done("D3D9PostProcess::Init D3DCompile FAILED 0x%08X: %s",
                    hr, (const char*)pError->GetBufferPointer());
            pError->Release();
        }
        else
        {
            CT_Done("D3D9PostProcess::Init D3DCompile FAILED 0x%08X (no error blob)", hr);
        }
        if (pCode) pCode->Release();
        return false;
    }

    if (pError) { pError->Release(); pError = NULL; }

    hr = pDevice->CreatePixelShader(
        (const DWORD*)pCode->GetBufferPointer(),
        &g_pPS);

    pCode->Release();
    pCode = NULL;

    if (FAILED(hr) || !g_pPS)
    {
        CT_Done("D3D9PostProcess::Init CreatePixelShader FAILED 0x%08X", hr);
        g_pPS = NULL;
        return false;
    }

    CT_Done("D3D9PostProcess::Init OK — vignette shader hazir (ps_2_0)");
    return true;
}

void D3D9PostProcess_Shutdown()
{
    if (g_pPS)
    {
        g_pPS->Release();
        g_pPS = NULL;
        CT_Done("D3D9PostProcess::Shutdown — shader released");
    }
}

bool D3D9PostProcess_Toggle()
{
    g_bEnabled = !g_bEnabled;
    CT_Done("D3D9PostProcess::Toggle -> %s", g_bEnabled ? "ON" : "OFF");
    return g_bEnabled;
}

bool D3D9PostProcess_IsEnabled()
{
    return g_bEnabled && g_pPS != NULL;
}

extern "C" void D3D9PostProcess_SetVignetteEnabled(bool enabled)
{
    g_bVignetteEnabled = enabled;
    if (enabled)
        g_bEnabled = true;
    CT_Done("D3D9PostProcess::SetVignetteEnabled -> %s", enabled ? "ON" : "OFF");
}

extern "C" void D3D9PostProcess_SetBrightnessStep(unsigned short step)
{
    if (step < 50)
        step = 50;
    if (step > 150)
        step = 150;

    g_fBrightnessScale = (float)step / 100.0f;
}

bool D3D9PostProcess_IsAnyEnabled()
{
    return g_bVignetteEnabled || g_fBrightnessScale != 1.0f;
}

void D3D9PostProcess_Apply(IDirect3DDevice9* pDevice)
{
    if (!pDevice || !g_pPS || !g_bEnabled)
        return;

    // 2026-05-16: Her ikisi off ve brightness neutral ise post-process
    // tamamen skip (fixed-function texture stage'i degistirme).
    if (!D3D9PostProcess_IsAnyEnabled())
        return;

    // c0 = (vignette_weight, brightness_scale, 0, 0)
    const float effects[4] =
    {
        g_bVignetteEnabled ? 1.0f : 0.0f,
        g_fBrightnessScale,
        0.0f,
        0.0f
    };
    pDevice->SetPixelShaderConstantF(0, effects, 1);
    pDevice->SetPixelShader(g_pPS);
}

void D3D9PostProcess_Reset(IDirect3DDevice9* pDevice)
{
    if (!pDevice)
        return;
    pDevice->SetPixelShader(NULL);
}
