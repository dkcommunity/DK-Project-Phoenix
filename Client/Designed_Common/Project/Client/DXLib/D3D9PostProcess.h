//-----------------------------------------------------------------------------
// D3D9PostProcess.h
//
// Phoenix D3D9 native port — Phase 7 (2026-05-14)
// Pixel-shader post-process effect chain. Composition buffer fullscreen quad
// uzerinde uygulanir; sprite codec + UI text dokunulmaz. Minimal MVP:
//   - Vignette: radial darken (sinema sin hiss)
// Tek pass single pixel shader (ps_2_0 target — universal GPU desteği).
//
// Kullanim:
//   D3D9Backend::Init -> D3D9PostProcess_Init(device)
//   D3D9Backend::Present -> SetTexture sonrasi D3D9PostProcess_Apply(device)
//                       -> DrawPrimitiveUP cagir
//                       -> D3D9PostProcess_Reset(device)
//   D3D9Backend::Shutdown -> D3D9PostProcess_Shutdown()
//
// F11 hotkey ile toggle (CDirectDraw key handler — phase ileri).
//-----------------------------------------------------------------------------

#ifndef __PHOENIX_D3D9_POSTPROCESS_H__
#define __PHOENIX_D3D9_POSTPROCESS_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// D3D9 surface header tek seferde dahil — DX7 D3D.h ile ayni TU'da olmaz
// (D3D9Backend.cpp + D3D9Facade.cpp guvenli; CDirect3D.cpp DX7-tarafli).
#undef DIRECT3D_VERSION
#define DIRECT3D_VERSION 0x0900
#include <d3d9.h>

// Init at backend startup. Returns true on success. Failure -> Apply no-op.
bool D3D9PostProcess_Init(IDirect3DDevice9* pDevice);

// Cleanup at backend shutdown.
void D3D9PostProcess_Shutdown();

// Aktivasyonu toggle et (F11 hotkey'inden cagirilir). Returns yeni state.
// 2026-05-16: Master toggle — Vignette kapali da olsa shader bind
// shader bind degisimi yapilmasin diye degeri korur.
bool D3D9PostProcess_Toggle();

// Mevcut state'i sorgu (UI overlay vs. icin).
bool D3D9PostProcess_IsEnabled();

// 2026-05-16: Per-effect runtime toggle (Option > OTHER tab).
// Default ON (eski hardcoded davranis korunur). Kullanici Vignette'i
// kapatirsa weight=0 olur (constant register), shader hala bind edilir ama
// efekt görünmez (lerp(1, raw, 0) = 1). Vignette off ve brightness neutral
// ise Apply tamamen erken cikar (post-process skip).
//
// VS_UI subsystem'i d3d9.h pull etmemek icin extern "C" linkage — boylece
// VS_UI_NewTitle.cpp forward declaration ile cagirabilir (mangling sorunsuz).
extern "C" void D3D9PostProcess_SetVignetteEnabled(bool enabled);
extern "C" void D3D9PostProcess_SetBrightnessStep(unsigned short step);

// Apply icinde "vignette off ve brightness neutral mi" diye sorgular; Present
// kullanir (varsa).
bool D3D9PostProcess_IsAnyEnabled();

// Present sirasinda DrawPrimitiveUP'tan ONCE cagir. Aktifse pixel shader
// bind eder; degilse no-op (fixed-function texture stage kalır).
void D3D9PostProcess_Apply(IDirect3DDevice9* pDevice);

// Present sirasinda DrawPrimitiveUP'tan SONRA cagir. SetPixelShader(NULL)
// ile fixed-function pipeline'a doner (sonraki frame icin temiz state).
void D3D9PostProcess_Reset(IDirect3DDevice9* pDevice);

#endif // __PHOENIX_D3D9_POSTPROCESS_H__
