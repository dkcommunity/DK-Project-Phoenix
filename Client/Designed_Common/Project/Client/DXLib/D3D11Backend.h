//-----------------------------------------------------------------------------
// D3D11Backend.h
//
// Phoenix D3D11 native render backend — Legacy DX11 gecis P0 (2026-08-31).
//
// IRenderBackend + IRenderSurface'in Direct3D 11 implementasyonu. Mimari
// D3D9Backend ile ayni "16-bit CPU composition + GPU present" pattern'ini
// korur; sprite codec (CSpriteSurface.cpp) ve tum CPU blit yollari
// DOKUNULMADAN calisir:
//
//     Sprite codec -> composition buffer (CPU bellek, R5G6B5)
//     Present      -> Map(DISCARD) ile dynamic texture upload
//                  -> fullscreen triangle (VS: SV_VertexID, PS: point sample)
//                  -> IDXGISwapChain::Present
//
// D3D9Backend'ten farklar:
//   - Composition buffer GDI DIB section (CreateDIBSection 16bpp 565).
//     Boylece hem sprite codec'in Lock() kontrati (pBits+pitch) hem de
//     FL2/GDI_Text'in HDC ihtiyaci (Phoenix_AcquireD3D9DC muadili) ayni
//     bellek uzerinden karsilanir. D3D9'daki IDirect3DSurface9::GetDC
//     yerine DIB'in kalici memory DC'si kullanilir.
//   - Offscreen/texture surface'lar duz CPU bellegi (_aligned_malloc).
//     Device-lost kavrami yok; Restore daima basarili.
//   - Upload formati: DXGI_FORMAT_B5G6R5_UNORM destekleniyorsa direkt
//     565 kopya; desteklenmiyorsa CPU'da 565 -> BGRA8888 donusum (SSE2).
//
// Secim: GetRenderBackend() factory'si render_backend.cfg / env degiskeni
// uzerinden D3D9 (varsayilan) veya D3D11 secer. Bkz. IsD3D11Selected().
//-----------------------------------------------------------------------------

#ifndef __PHOENIX_D3D11_BACKEND_H__
#define __PHOENIX_D3D11_BACKEND_H__

#include "IRenderBackend.h"
#include "IRenderSurface.h"

// d3d11.h bu header'a DAHIL EDILMEZ — COM arabirimleri forward declare.
// (CDirectDrawSurface.cpp gibi DX7 header'li TU'lar bu header'i include
// edebilir; d3d11.h yalnizca D3D11Backend.cpp icinde acilir.)
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11Buffer;
struct ID3D11SamplerState;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11RasterizerState;
struct ID3D11BlendState;
struct ID3D11DepthStencilState;

namespace phoenix
{
namespace render
{

//-----------------------------------------------------------------------------
// D3D11Surface
//
// IRenderSurface'in CPU bellek implementasyonu. Iki depolama modu:
//   - DIB section  : composition buffer (GDI text DC gereksinimi)
//   - aligned heap : offscreen/texture surface'lar (atlas, tile, cache)
//
// Tum Blt/Fill operasyonlari D3D9Surface'taki CPU dongulerinin birebir
// portudur (clip + bounds + colorkey semantigi ayni). Lock kalici pointer
// dondurur; re-entrant Lock ref-count ile desteklenir (D3D9 Fix #11 paritesi
// — davranis ayni kalir, gercek kilitleme olmadigindan sadece sayac).
//-----------------------------------------------------------------------------
class D3D11Surface : public IRenderSurface
{
public:
    // useDIB=true: CreateDIBSection (565 bitfields) + memory DC.
    // useDIB=false: _aligned_malloc (16 byte — SSE2 yollari icin).
    D3D11Surface(int width, int height, SurfaceFormat format, bool useDIB);
    virtual ~D3D11Surface();

    // Olusturma basarili mi (DIB/heap alloc fail durumunda false)
    bool IsValid() const { return m_pBits != NULL; }

    // 2026-09-02 crash fix — YERINDE yeniden boyutlandirma. Composition
    // singleton'i InitOffsurface/InitBacksurface "Yol 1" wrapper'lari
    // (g_pBack, g_pLast, VS_UI Base) tarafindan raw pointer olarak
    // saklanir; obje delete/new edilirse hepsi dangling kalir
    // (CWaitUIUpdate FillSurface AV, dump p16364). Reinit obje kimligini
    // korur, yalniz DIB/heap depoyu yeni boyutla yeniden kurar.
    bool Reinit(int width, int height);

    // IRenderSurface
    virtual bool Lock(LockedRect& outRect, DWORD flags);
    virtual bool Unlock();
    virtual bool IsLocked() const { return m_LockCount > 0; }

    virtual int           GetWidth() const  { return m_Width; }
    virtual int           GetHeight() const { return m_Height; }
    virtual SurfaceFormat GetFormat() const { return m_Format; }

    virtual void SetClip(const RECT& rc);
    virtual void SetClipNull();
    virtual int  GetClipLeft() const   { return m_ClipLeft; }
    virtual int  GetClipTop() const    { return m_ClipTop; }
    virtual int  GetClipRight() const  { return m_ClipRight; }
    virtual int  GetClipBottom() const { return m_ClipBottom; }

    virtual void  SetTransparency(DWORD colorValue);
    virtual DWORD GetColorKey() const { return m_ColorKey; }
    virtual bool  HasColorKey() const { return m_bHasColorKey; }

    virtual void BltColorKey(POINT* pDestPoint, IRenderSurface* pSrc, RECT* pSrcRect);
    virtual void BltNoColorKey(POINT* pDestPoint, IRenderSurface* pSrc, RECT* pSrcRect);
    virtual void BltStretchKey(RECT* pDestRect, IRenderSurface* pSrc, RECT* pSrcRect);
    virtual void FillSurface(WORD color);
    virtual void FillRect(RECT* pRect, WORD color);

    virtual bool Restore();

    // D3D11-specific accessor'lar
    void* GetPixels() const      { return m_pBits; }
    int   GetPitchBytes() const  { return m_Pitch; }
    // Composition DIB surface'inin kalici memory DC'si (GDI text icin).
    // Heap surface'ta NULL.
    HDC   GetMemoryDC() const    { return m_hMemDC; }

private:
    void*         m_pBits;      // pixel base (DIB veya aligned heap)
    int           m_Pitch;      // byte stride
    int           m_Width;
    int           m_Height;
    SurfaceFormat m_Format;
    bool          m_bDIB;       // true: DIB section, false: aligned heap
    HBITMAP       m_hDIB;       // DIB section handle (yalniz m_bDIB)
    HDC           m_hMemDC;     // DIB'e secili kalici memory DC (yalniz m_bDIB)

    int           m_ClipLeft;
    int           m_ClipTop;
    int           m_ClipRight;
    int           m_ClipBottom;
    DWORD         m_ColorKey;
    bool          m_bHasColorKey;
    int           m_LockCount;  // re-entrant Lock pariteleri (D3D9 Fix #11)

    // Depo yonetimi — ctor/dtor/Reinit ortak yolu
    void AllocateStorage();
    void ReleaseStorage();
};

//-----------------------------------------------------------------------------
// D3D11Backend
//
// IRenderBackend'in Direct3D 11 + DXGI implementasyonu. Composition CPU'da
// kalir (D3D9 pariteleri), yalnizca upload + quad + Present DX11'e gecer.
//-----------------------------------------------------------------------------
class D3D11Backend : public IRenderBackend
{
public:
    D3D11Backend();
    virtual ~D3D11Backend();

    static D3D11Backend& Instance();

    // IRenderBackend
    virtual bool Init(HWND hWnd, WORD width, WORD height, ScreenMode mode, bool use3D, bool useIME);
    virtual void Shutdown();
    virtual bool RestoreAll();
    virtual bool Resize(WORD width, WORD height);	// O3 — canli cozunurluk
    // 2026-09-02 fullscreen fix — canli pencere modu (Reborn ApplyWindowMode
    // paritesi, borderless CDS_FULLSCREEN; DXGI exclusive kullanilmaz)
    virtual bool SetFullscreenMode(bool bFullscreen);

    virtual void Present();
    virtual void FlipToGDISurface();

    virtual IRenderSurface* CreateOffscreenSurface(int width, int height, DWORD flags);
    virtual IRenderSurface* CreateTextureSurface(int width, int height, DWORD flags);
    virtual IRenderSurface* GetBackBuffer();
    virtual IRenderSurface* GetPrimarySurface();

    virtual bool IsFullscreen() const { return m_bFullscreen; }
    virtual WORD GetScreenWidth() const  { return m_Width; }
    virtual WORD GetScreenHeight() const { return m_Height; }
    virtual SurfaceFormat GetPrimaryFormat() const;

    virtual bool IsGammaSupported() const;
    virtual void SetGammaRamp(WORD step);
    virtual void RestoreGammaRamp();
    virtual void SetAddGammaRamp(WORD r, WORD g, WORD b);

    virtual void OnMove();
    virtual HWND GetWindow() const { return m_hWnd; }

    // Backend init edilmis mi (facade IsD3D11BackendActive icin)
    bool IsInitialized() const { return m_pDevice != NULL; }

    // Composition surface accessor (CDirectDrawSurface Yol 1/Yol 2 binding)
    D3D11Surface* GetCompositionSurface() { return m_pCompositionSurface; }

    // InitTextureSurface DX11 dali — istenen formatta CPU surface
    D3D11Surface* CreateMemorySurface(int width, int height, SurfaceFormat fmt);

    //-------------------------------------------------------------------------
    // 2026-09-01 P1 — GPU isik filtresi (gece/vision karartmasi).
    //
    // CPU yolun (Phoenix_ApplyLightBufferFilter3DToSurface) GPU muadili.
    // MTopView DrawLightFilter aninda cagirir:
    //   - cells4: filter grid'i (row-major, dusuk nibble 0-15 = karartma)
    //   - fw/fh : grid boyutu; destW/destH: hedef alan (CLIPSURFACE = ekran)
    // Yapilanlar: grid 8-bit'e (x17) acilip kucuk R8 texture'a yuklenir; o
    // ANKI composition (world tamam, UI henuz yok) world-snapshot texture'ina
    // yuklenir; Present PS'i "final==world ise karart" uygular. Boylece CPU
    // cizim sirasi semantigi korunur (filtreden SONRA cizilenler kararmiyor).
    //
    // Donus: true = GPU devraldi (CPU apply atlanmali; grid bos ise filtre
    // pasiflenir, yine true). false = GPU hazir degil (caller CPU fallback).
    //-------------------------------------------------------------------------
    bool SubmitLightFilter(const BYTE* cells4, int fw, int fh, int destW, int destH);

    // Frame basinda cagrilir: filtre bolumu bu frame hic kosmayacaksa (zone
    // guard vs.) bayat karartma kalmasin. Submit yeniden aktifler.
    // Faz G5: dx11gpu modunda ayrica world/UI siniri checkpoint'i tetikler
    // (cagri noktasi = world cizimi bitti, UI baslamadi) — impl .cpp'de.
    void DeactivateLightFilter();

    // ImGui DX11 impl'e device/context expose
    ID3D11Device*        GetD3D11Device()  { return m_pDevice; }
    ID3D11DeviceContext* GetD3D11Context() { return m_pContext; }

private:
    // Device + swapchain
    ID3D11Device*            m_pDevice;
    ID3D11DeviceContext*     m_pContext;
    IDXGISwapChain*          m_pSwapChain;
    ID3D11RenderTargetView*  m_pRTV;

    // Upload texture + pipeline state
    ID3D11Texture2D*          m_pUploadTex;
    ID3D11ShaderResourceView* m_pUploadSRV;
    ID3D11SamplerState*       m_pSampler;
    ID3D11VertexShader*       m_pVS;
    ID3D11PixelShader*        m_pPS;
    ID3D11RasterizerState*    m_pRasterizer;
    ID3D11BlendState*         m_pBlend;
    ID3D11DepthStencilState*  m_pDepthOff;

    // 2026-09-01 P1 — GPU isik filtresi kaynaklari
    ID3D11Texture2D*          m_pWorldTex;     // world snapshot (upload tex ile ayni format)
    ID3D11ShaderResourceView* m_pWorldSRV;
    ID3D11Texture2D*          m_pLightTex;     // filter grid (R8, fw x fh)
    ID3D11ShaderResourceView* m_pLightSRV;
    ID3D11SamplerState*       m_pSamplerLinear;// grid bilinear upsample (CPU 8.8 muadili)
    ID3D11Buffer*             m_pPresentCB;    // b0: lightActive + clip boyutlari
    int                       m_LightTexW;
    int                       m_LightTexH;
    bool                      m_LightActive;
    int                       m_LightClipW;
    int                       m_LightClipH;

    // 2026-09-01 — Option "Aydinlatma" (gamma). D3D9 windowed post-process
    // brightness paritesi: hardware ramp yerine present asamasinda carpim.
    // m_GammaStep 50..150 (100 = notr), m_Brightness = step/100.
    WORD                      m_GammaStep;
    float                     m_Brightness;

    // Composition buffer (DIB section, R5G6B5)
    D3D11Surface*            m_pCompositionSurface;

    HWND                     m_hWnd;
    WORD                     m_Width;
    WORD                     m_Height;
    bool                     m_bFullscreen;
    bool                     m_bBorderlessActive; // CDS_FULLSCREEN display mode aktif mi
    bool                     m_bTex565;      // upload tex B5G6R5 mi (yoksa BGRA convert)

    // Reborn ApplyWindowMode paritesi (Application.cpp 26874): fullscreen =
    // ChangeDisplaySettings(CDS_FULLSCREEN) + WS_POPUP; windowed = mode
    // restore + overlapped pencere (workarea ortali). Swapchain hep windowed.
    bool ApplyBorderlessFullscreen(bool bEnable);

    bool CreateDeviceResources();     // device+swapchain+RTV+pipeline
    void ReleaseDeviceResources();    // composition DIB haric her sey
    bool CreateUploadTexture();
    void UploadComposition();         // DIB -> dynamic texture (Map/Unmap)
    // Composition DIB'i verilen dynamic texture'a yukler (F ve W ayni
    // donusum yolundan gecer — PS'teki esitlik karsilastirmasi bit-kesin).
    bool UploadCompositionTo(ID3D11Texture2D* pTex);
};

//-----------------------------------------------------------------------------
// Backend secimi — render_backend.cfg (Client.exe yaninda) ilk satiri
// "dx11" ise veya PHOENIX_RENDER_BACKEND env degiskeni "dx11" ise D3D11.
// Dosya yok / baska deger = D3D9 (varsayilan, sifir risk).
// Ilk cagrida okunur ve cache'lenir (process omru boyunca sabit).
//-----------------------------------------------------------------------------
bool IsD3D11Selected();

} // namespace render
} // namespace phoenix

#endif // __PHOENIX_D3D11_BACKEND_H__
