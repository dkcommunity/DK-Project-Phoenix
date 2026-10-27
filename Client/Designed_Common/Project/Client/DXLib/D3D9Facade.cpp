//-----------------------------------------------------------------------------
// D3D9Facade.cpp
//
// D3D9Facade.h implementasyonu — d3d9.h burada dahil edilir (DXLib derleme
// birimi; D3D.h YOK, DXLib sadece ddraw.h kullanir, conflict yok).
//
// Sebep: bkz. D3D9Facade.h header yorumu.
//-----------------------------------------------------------------------------

#include "D3D9Facade.h"
#include "D3D9Backend.h"   // d3d9.h transitively
#include "D3D11Backend.h"  // 2026-08-31 DX11 P0 — native backend sorgulari
#include "D3D11SpriteCommands.h"  // 2026-09-03 flicker fix — MarkWorldFrame

namespace phoenix
{
namespace render
{

bool IsD3D9BackendActive()
{
    return D3D9Backend::Instance().GetD3D9Device() != NULL;
}

bool IsD3D11BackendActive()
{
    return D3D11Backend::Instance().IsInitialized();
}

bool IsNativeBackendActive()
{
    return IsD3D9BackendActive() || IsD3D11BackendActive();
}

IRenderSurface* GetCompositionRenderSurface()
{
    // DX11 secili + init edilmisse onun composition'i; degilse D3D9.
    if (IsD3D11Selected())
    {
        D3D11Surface* pComp = D3D11Backend::Instance().GetCompositionSurface();
        if (pComp != NULL && pComp->IsValid())
            return pComp;
        return NULL;
    }

    D3D9Surface* pComp = D3D9Backend::Instance().GetCompositionSurface();
    if (pComp != NULL && pComp->GetRawD3DSurface() != NULL)
        return pComp;
    return NULL;
}

bool GPULightFilter_Submit(const unsigned char* cells4, int fw, int fh,
                           int destW, int destH)
{
    // Yalniz DX11 modunda anlamli — D3D9 CPU yolunda kalir.
    if (!IsD3D11Selected() || !D3D11Backend::Instance().IsInitialized())
        return false;
    return D3D11Backend::Instance().SubmitLightFilter(cells4, fw, fh, destW, destH);
}

void GPULightFilter_Deactivate()
{
    if (IsD3D11Selected() && D3D11Backend::Instance().IsInitialized())
        D3D11Backend::Instance().DeactivateLightFilter();
}

void GPUWorldFrame_Begin()
{
    // 2026-09-03 flicker fix — aciklama header'da. Komut katmani kendi
    // ic guard'larina sahip (m_bInit + dx11gpu secimi).
    if (IsD3D11Selected() && D3D11Backend::Instance().IsInitialized())
        D3D11SpriteCommands::Instance().MarkWorldFrame();
}

bool GetD3D9DeviceCaps(int& widthMax, int& heightMax, bool& pow2, bool& squareOnly)
{
    IDirect3DDevice9Ex* pDev = D3D9Backend::Instance().GetD3D9Device();
    if (pDev == NULL) {
        return false;
    }

    D3DCAPS9 caps;
    ZeroMemory(&caps, sizeof(caps));
    if (FAILED(pDev->GetDeviceCaps(&caps))) {
        return false;
    }

    widthMax     = (caps.MaxTextureWidth)  ? (int)caps.MaxTextureWidth  : 2048;
    heightMax    = (caps.MaxTextureHeight) ? (int)caps.MaxTextureHeight : 2048;
    pow2         = (caps.TextureCaps & D3DPTEXTURECAPS_POW2)       != 0;
    squareOnly   = (caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY) != 0;
    return true;
}

void ClearZBuffer()
{
    IDirect3DDevice9Ex* pDev = D3D9Backend::Instance().GetD3D9Device();
    if (pDev != NULL) {
        pDev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L);
    }
}

void RestoreD3D9Backend()
{
    D3D9Backend::Instance().RestoreAll();
}

} // namespace render
} // namespace phoenix
