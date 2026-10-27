//-----------------------------------------------------------------------------
// Phoenix_D3D9TextDC.cpp
//
// Fix #17 (2026-05-14): D3D9 composition buffer HDC wrapper implementasyonu.
// FL2.cpp (vs_ui_lib) + CDirectDrawSurface::GDI_Text (DXLib) tarafindan
// kullanilir. Caller D3D9Backend'i dogrudan goremedigi icin (vs_ui_lib
// include path'inde DXLib yok), bu Client/ root'unda wrapper saglar.
//
// D3D9 composition buffer SYSTEMMEM + R5G6B5; D3D9 spec'e gore GetDC bu
// format/pool kombinasyonunda destekli. Surface lock state'i clear olmali
// (caller sorumlu — sprite codec Lock/Unlock cycle'i disinda cagirilmali).
//-----------------------------------------------------------------------------

#include "Phoenix_D3D9TextDC.h"
#include "crash_trace_tool.h"
#include "DXLib/D3D9Backend.h"
#include "DXLib/D3D11Backend.h"   // 2026-08-31 DX11 P0 — DIB memory DC yolu

extern "C" void Phoenix_TextPerf_CountHdcAcquire();
extern "C" void Phoenix_TextPerf_CountHdcRelease();

HDC Phoenix_AcquireD3D9DC()
{
#if PHOENIX_RENDER_BACKEND_D3D9
    // 2026-08-31 DX11 P0: DX11 modunda composition buffer bir GDI DIB
    // section — kalici memory DC dogrudan doner (GetDC/ReleaseDC yok,
    // maliyet sifir). TextOut ayni composition bellegine yazar.
    if (phoenix::render::IsD3D11Selected())
    {
        phoenix::render::D3D11Surface* pComp11 =
            phoenix::render::D3D11Backend::Instance().GetCompositionSurface();
        if (pComp11 == NULL)
            return NULL;
        HDC hdc11 = pComp11->GetMemoryDC();
        if (hdc11 != NULL)
            Phoenix_TextPerf_CountHdcAcquire();
        return hdc11;
    }

    phoenix::render::D3D9Backend& backend = phoenix::render::D3D9Backend::Instance();
    phoenix::render::D3D9Surface* pComp = backend.GetCompositionSurface();
    if (pComp == NULL)
        return NULL;

    IDirect3DSurface9* pRaw = pComp->GetRawD3DSurface();
    if (pRaw == NULL)
        return NULL;

    HDC hdc = NULL;
    HRESULT hr = pRaw->GetDC(&hdc);
    if (FAILED(hr))
    {
        static DWORD lastLogTick = 0;
        DWORD now = GetTickCount();
        if (now - lastLogTick > 5000)
        {
            CT_Done("DIAG-17: Phoenix_AcquireD3D9DC GetDC FAILED 0x%08X", hr);
            lastLogTick = now;
        }
        return NULL;
    }
    Phoenix_TextPerf_CountHdcAcquire();
    return hdc;
#else
    return NULL;
#endif
}

void Phoenix_ReleaseD3D9DC(HDC hdc)
{
#if PHOENIX_RENDER_BACKEND_D3D9
    if (hdc == NULL)
        return;

    // 2026-08-31 DX11 P0: DIB memory DC kalici — release gerekmez. GDI batch
    // flush ile bekleyen TextOut'lar composition bellegine iner (Present
    // upload'i bayat pixel gormesin).
    if (phoenix::render::IsD3D11Selected())
    {
        Phoenix_TextPerf_CountHdcRelease();
        GdiFlush();
        return;
    }

    phoenix::render::D3D9Backend& backend = phoenix::render::D3D9Backend::Instance();
    phoenix::render::D3D9Surface* pComp = backend.GetCompositionSurface();
    if (pComp == NULL)
        return;

    IDirect3DSurface9* pRaw = pComp->GetRawD3DSurface();
    if (pRaw == NULL)
        return;

    Phoenix_TextPerf_CountHdcRelease();
    pRaw->ReleaseDC(hdc);
#endif
}
