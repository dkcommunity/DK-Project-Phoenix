//-----------------------------------------------------------------------------
// Phoenix_D3D9TextDC.h
//
// Fix #17 (2026-05-14): D3D9 native port FAZ 3 — text rendering wrapper.
//
// Legacy FL2 (Hangul) + CDirectDrawSurface::GDI_Text DDraw7 surface API'sine
// bagimliydi: gpC_fl2_surface->GetDC() veya m_pDDSurface->GetDC(). D3D9
// modunda DDraw7 surface yok; bu wrapper D3D9Backend composition buffer'in
// IDirect3DSurface9::GetDC()'si uzerinden HDC saglar. TextOut tekrar
// composition buffer'a yazar, Present pipeline ekrana cikartir.
//
// Kullanim:
//   HDC hdc = Phoenix_AcquireD3D9DC();
//   if (hdc != NULL) {
//       TextOut(hdc, x, y, str, len);  // veya GDI metric/render
//       Phoenix_ReleaseD3D9DC(hdc);
//   }
//
// D3D9 backend init edilmemis veya GetDC fail ederse NULL doner; caller
// fallback path'e gecmeli (memory DC veya skip).
//-----------------------------------------------------------------------------

#ifndef __PHOENIX_D3D9_TEXTDC_H__
#define __PHOENIX_D3D9_TEXTDC_H__

#include <windows.h>

// Composition buffer'in IDirect3DSurface9::GetDC()'sini ister; success ise HDC,
// fail ise NULL doner. Caller TextOut/SetTextColor/SetBkMode vs. cagirabilir.
// Composition buffer locked durumdaysa NULL doner (D3D9 GetDC kontrati).
HDC Phoenix_AcquireD3D9DC();

// Phoenix_AcquireD3D9DC'den alinan HDC'yi serbest birakir. NULL hdc no-op.
void Phoenix_ReleaseD3D9DC(HDC hdc);

#endif // __PHOENIX_D3D9_TEXTDC_H__
