//-----------------------------------------------------------------------------
// D3D9Facade.h
//
// Phoenix D3D9 native port — d3d9.h dahil etmeden D3D9Backend ile etkilesim.
//
// **Mimari neden**: DX7 D3D.h (legacy) ve Windows SDK d3d9.h ayni TU'da coexist
// edemez — `_D3DVECTOR`, `_D3DLIGHTTYPE`, `_D3DSHADEMODE` vs. enum/struct
// tekrar tanim hatalari (C2011). Phoenix Phase 4 mimarisinde CDirect3D.cpp ve
// MTopView.cpp ikisi de D3D7 D3D.h transitively dahil ediyor; bu dosyalarin
// D3D9 device'ina ulasimi icin facade gerekli.
//
// Bu header **d3d9.h DAHIL ETMEZ**; sadece POD parametre + namespace fonksiyon
// imzalari. Implementasyon `D3D9Facade.cpp` icinde (DXLib derleme birimi —
// d3d9.h orada dahil edilir, D3D.h orada YOK).
//
// 2026-05-14 Phase 4.5 / post-build-fix
//-----------------------------------------------------------------------------

#ifndef __PHOENIX_D3D9_FACADE_H__
#define __PHOENIX_D3D9_FACADE_H__

namespace phoenix
{
namespace render
{

class IRenderSurface;

//-----------------------------------------------------------------------------
// D3D9 backend aktif mi? (Device alindi mi?)
//-----------------------------------------------------------------------------
bool IsD3D9BackendActive();

//-----------------------------------------------------------------------------
// 2026-08-31 Legacy DX11 gecis P0:
//
// IsD3D11BackendActive  : D3D11Backend init edilmis mi?
// IsNativeBackendActive : D3D9 VEYA D3D11 backend aktif mi? Eski
//   IsD3D9BackendActive() gate'lerinin cogunun gercek anlami "CPU composition
//   backend calisiyor mu" oldugu icin (CPU light filter, CDirect3D HAL,
//   IsLost) bu gate'ler DX11 modunda da ayni dali almali. Yeni kod bu
//   fonksiyonu kullanmali; IsD3D9BackendActive yalniz gercekten D3D9 device
//   gerektiren yerlerde kalir (GetD3D9DeviceCaps, ClearZBuffer vs).
//-----------------------------------------------------------------------------
bool IsD3D11BackendActive();
bool IsNativeBackendActive();

//-----------------------------------------------------------------------------
// Aktif backend'in composition surface'i (sprite codec hedefi). Backend init
// edilmemis veya composition hazir degilse NULL. CDirectDrawSurface
// InitBacksurface/InitOffsurface Yol 1 binding'i bu erisimi kullanir —
// concrete backend tipine bagimlilik kalmaz.
//-----------------------------------------------------------------------------
IRenderSurface* GetCompositionRenderSurface();

//-----------------------------------------------------------------------------
// 2026-09-01 DX11 P1 — GPU isik filtresi facade'i.
//
// GPULightFilter_Submit: MTopView DrawLightFilter aninda cagirir. cells4 =
//   filter grid (row-major, dusuk nibble 0-15 karartma), fw/fh grid boyutu,
//   destW/destH hedef alan (CLIPSURFACE). true donerse GPU devraldi — CPU
//   Phoenix_ApplyLightBufferFilter3DToSurface ATLANMALI. false = GPU yok/
//   hazir degil, caller CPU yoluna duser.
// GPULightFilter_Deactivate: her frame filtre bolumu basinda cagrilir —
//   bu frame submit gelmezse (zone guard vs.) bayat karartma kalmasin.
// Ikisi de D3D9 modunda no-op'tur (Submit false doner).
//-----------------------------------------------------------------------------
bool GPULightFilter_Submit(const unsigned char* cells4, int fw, int fh,
                           int destW, int destH);
void GPULightFilter_Deactivate();

//-----------------------------------------------------------------------------
// 2026-09-03 flicker fix — world frame'in BASLADIGINI komut katmanina bildir.
// MTopView::DrawZone girisinde her world ciziminde cagrilir. dx11gpu komut
// katmani baseline (accum sifirlama) kararini frame'in ILK flush'inda verir;
// bayrak cizimin sonunda set edilirse erken flush yanlis karar veriyordu
// (sprite izleri / genel flicker). D3D9 ve dx11 modlarinda no-op.
//-----------------------------------------------------------------------------
void GPUWorldFrame_Begin();

//-----------------------------------------------------------------------------
// Texture cap query — D3D9 IDirect3DDevice9Ex::GetDeviceCaps wraps.
//
// Donus:
//   true  -> caps degerleri parametrelere yazildi
//   false -> device yok veya GetDeviceCaps fail (parametreler degismez)
//
// Sane defaults caller tarafindan set edilmeli (D3D9 init basarisiz ise).
//-----------------------------------------------------------------------------
bool GetD3D9DeviceCaps(int& widthMax, int& heightMax, bool& pow2, bool& squareOnly);

//-----------------------------------------------------------------------------
// Clear ZBuffer — MTopView::DrawProcess basinda her frame cagrilir.
//
// D3D9 device'i Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0L) cagirir. Device
// yoksa no-op (NULL device check icerde).
//-----------------------------------------------------------------------------
void ClearZBuffer();

//-----------------------------------------------------------------------------
// D3D9 backend restore (device lost sonrasi) — D3D9Backend::RestoreAll wraps.
// CDirect3D::Restore() icinden cagrilir.
//-----------------------------------------------------------------------------
void RestoreD3D9Backend();

} // namespace render
} // namespace phoenix

#endif // __PHOENIX_D3D9_FACADE_H__
