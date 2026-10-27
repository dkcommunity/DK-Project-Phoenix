//----------------------------------------------------------------------
// CDirectDrawSurface.cpp
//----------------------------------------------------------------------
#include "CDirectDrawSurface.h"
#include "IRenderBackend.h"
#include "IRenderSurface.h"
#if PHOENIX_RENDER_BACKEND_D3D9
// Phase 5 (2026-05-14): D3D9 native texture format port. InitTextureSurface
// flag ON path'inde D3D9Backend uzerinden IDirect3DTexture9 yaratip
// D3D9Surface delegate'ini m_pRenderSurface'a baglar. m_pDDSurface NULL
// kalir (DDraw7 escape hatch flag ON modunda devre disi).
#include "D3D9Backend.h"
#include "D3D9Facade.h"    // 2026-08-31 DX11 P0 — GetCompositionRenderSurface
#include "D3D11Backend.h"  // 2026-08-31 DX11 P0 — InitTextureSurface DX11 dali
#endif
#include <stdio.h>
#include "../crash_trace_tool.h"
#include "../Phoenix_D3D9TextDC.h"  // Fix #17 — D3D9 composition HDC for GDI_Text
#include "../PhoenixDWrite.h"       // FULL DX11 gecis Faz T3 — DWrite text (GDI fallback'li)
#include "D3D11SpriteCommands.h"    // Faz G5 — GammaBox ham yazisi dirty bildirimi

// Phase 2 (2026-05-14): IRenderSurface delegate path. Bu dosyadaki
// Lock/Unlock/Blt/Fill/Restore artik m_pRenderSurface uzerinden gecer.
// Sprite codec (CSpriteSurface.cpp 13670 LOC) dokunulmaz; public API
// (GetSurfacePointer/Pitch/Lock/Blt vs.) byte-byte ayni davranisi
// gosterir. m_pDDSurface raw escape hatch sadece Gamma*Pixel + CopyBitmap
// + SaveToBMP + GDI_Text gibi DDraw7-spesifik utility'lerde kullanilir.
using namespace phoenix::render;

#if PHOENIX_RENDER_BACKEND_D3D9
//----------------------------------------------------------------------
// Phase 5 yardimcisi: DDPIXELFORMAT bitmask -> D3D9 D3DFMT enum + Phoenix
// SurfaceFormat eslestirmesi.
//
// Phoenix CTexturePack ve *TexturePartManager.cpp dosyalari
// CDirect3D::GetPixelFormat4444()/GetPixelFormat1555() static DDPIXELFORMAT
// pointer'larini kullanarak texture format tercih ediyor. D3D9 path'inde
// bu pointer'larin alpha/RGB bit mask'larini okuyup D3DFMT_* karsiligini
// donduruyoruz; bilinmeyen format DDP_NULL veya R5G6B5 (Phoenix sprite
// codec default).
//
// 5:6:5  -> R5G6B5    (alpha bits=0, RGB bits=5/6/5)
// 1:5:5:5-> A1R5G5B5  (alpha bits=1, RGB bits=5/5/5)
// 4:4:4:4-> A4R4G4B4  (alpha bits=4, RGB bits=4/4/4)
// Default-> R5G6B5    (pDDPF=NULL, sprite codec default)
//----------------------------------------------------------------------
static void MapDDPixelFormatToD3D9(const LPDDPIXELFORMAT pDDPF,
                                   D3DFORMAT& outD3DFmt,
                                   SurfaceFormat& outSurfaceFmt) {
  // Default — pDDPF NULL ise sprite codec'in default 16-bit format'i
  outD3DFmt     = D3DFMT_R5G6B5;
  outSurfaceFmt = SurfaceFormat::R5G6B5;
  if (pDDPF == NULL)
    return;

  // DDPIXELFORMAT.dwRGBAlphaBitMask 0 ise alpha yok (565), aksi halde
  // bit sayisina gore 1555 / 4444 ayirt et.
  if (pDDPF->dwFlags & DDPF_ALPHAPIXELS) {
    // Alpha mask bit count: 0x8000 -> 1 bit, 0xF000 -> 4 bit
    DWORD a = pDDPF->dwRGBAlphaBitMask;
    if (a == 0x8000) {
      // 1:5:5:5
      outD3DFmt     = D3DFMT_A1R5G5B5;
      outSurfaceFmt = SurfaceFormat::A1R5G5B5;
    } else if (a == 0xF000) {
      // 4:4:4:4
      outD3DFmt     = D3DFMT_A4R4G4B4;
      outSurfaceFmt = SurfaceFormat::A4R4G4B4;
    }
    // Diger varyantlar (24/32-bit) Phoenix'te kullanilmiyor — sessizce
    // R5G6B5 default'una dus.
  } else {
    // Alpha yok -> 5:6:5
    outD3DFmt     = D3DFMT_R5G6B5;
    outSurfaceFmt = SurfaceFormat::R5G6B5;
  }
}
#endif // PHOENIX_RENDER_BACKEND_D3D9

// Yardimci: clip RECT olustur. RECT POD oldugu icin C++17 brace-init
// MSVC 2022'de garanti — yine de tek noktada toplamak okunabilirligi
// artiriyor.
static inline RECT MakeClipRect(int l, int t, int r, int b)
{
    RECT rc;
    rc.left   = l;
    rc.top    = t;
    rc.right  = r;
    rc.bottom = b;
    return rc;
}

#if PHOENIX_RENDER_BACKEND_D3D9
namespace
{
struct PhoenixGdiTextDibCache
{
  PhoenixGdiTextDibCache()
      : hdc(NULL), bitmap(NULL), oldBitmap(NULL), bits(NULL),
        width(0), height(0), pitchPixels(0)
  {
  }

  ~PhoenixGdiTextDibCache()
  {
    Release();
  }

  void Release()
  {
    if (hdc != NULL && oldBitmap != NULL)
    {
      SelectObject(hdc, oldBitmap);
      oldBitmap = NULL;
    }
    if (bitmap != NULL)
    {
      DeleteObject(bitmap);
      bitmap = NULL;
    }
    if (hdc != NULL)
    {
      DeleteDC(hdc);
      hdc = NULL;
    }
    bits = NULL;
    width = 0;
    height = 0;
    pitchPixels = 0;
  }

  bool Ensure(int requiredWidth, int requiredHeight)
  {
    if (requiredWidth < 1)
      requiredWidth = 1;
    if (requiredHeight < 1)
      requiredHeight = 1;

    if (hdc != NULL && bitmap != NULL && bits != NULL &&
        width >= requiredWidth && height >= requiredHeight)
    {
      return true;
    }

    Release();

    width = requiredWidth;
    height = requiredHeight;
    pitchPixels = width;

    hdc = CreateCompatibleDC(NULL);
    if (hdc == NULL)
    {
      Release();
      return false;
    }

    BITMAPINFO bitmapInfo;
    ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = -height;  // top-down DIB
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    void* dibBits = NULL;
    bitmap = CreateDIBSection(NULL, &bitmapInfo, DIB_RGB_COLORS,
                              &dibBits, NULL, 0);
    if (bitmap == NULL || dibBits == NULL)
    {
      Release();
      return false;
    }

    oldBitmap = static_cast<HBITMAP>(SelectObject(hdc, bitmap));
    bits = static_cast<DWORD*>(dibBits);
    return true;
  }

  HDC hdc;
  HBITMAP bitmap;
  HBITMAP oldBitmap;
  DWORD* bits;
  int width;
  int height;
  int pitchPixels;
};

static PhoenixGdiTextDibCache g_PhoenixGdiTextDibCache;

static inline DWORD Phoenix_ColorRefToDibPixel(COLORREF color)
{
  return (static_cast<DWORD>(GetRValue(color)) << 16) |
         (static_cast<DWORD>(GetGValue(color)) << 8) |
         static_cast<DWORD>(GetBValue(color));
}

static inline WORD Phoenix_DibPixelToSurfaceWord(DWORD dibPixel)
{
  const BYTE r = static_cast<BYTE>((dibPixel >> 16) & 0xFF);
  const BYTE g = static_cast<BYTE>((dibPixel >> 8) & 0xFF);
  const BYTE b = static_cast<BYTE>(dibPixel & 0xFF);

  if (CDirectDraw::Is565())
    return static_cast<WORD>(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));

  return static_cast<WORD>(((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3));
}

static bool Phoenix_DrawGdiTextViaDib(CDirectDrawSurface* surface,
                                      int x, int y, const char* str,
                                      COLORREF fcolor, COLORREF bcolor,
                                      bool transparentBackground)
{
  if (surface == NULL || str == NULL || str[0] == '\0')
    return true;

  const int textLength = static_cast<int>(strlen(str));
  if (textLength <= 0)
    return true;

  if (!g_PhoenixGdiTextDibCache.Ensure(64, 32))
    return false;

  SIZE textSize;
  if (!GetTextExtentPoint32A(g_PhoenixGdiTextDibCache.hdc, str,
                             textLength, &textSize))
  {
    textSize.cx = textLength * 8;
    textSize.cy = 16;
  }

  const int textWidth = textSize.cx + 2;
  const int textHeight = textSize.cy + 2;
  if (textWidth <= 0 || textHeight <= 0)
    return true;

  if (!g_PhoenixGdiTextDibCache.Ensure(textWidth, textHeight))
    return false;

  // Near-black key avoids the old magenta fringe problem on anti-aliased text.
  const COLORREF transparentKeyColor = RGB(0, 0, 1);
  const DWORD transparentKey = Phoenix_ColorRefToDibPixel(transparentKeyColor);
  const DWORD fillColor = transparentBackground
                              ? transparentKey
                              : Phoenix_ColorRefToDibPixel(bcolor);

  for (int row = 0; row < textHeight; ++row)
  {
    DWORD* dst = g_PhoenixGdiTextDibCache.bits +
                 row * g_PhoenixGdiTextDibCache.pitchPixels;
    for (int col = 0; col < textWidth; ++col)
      dst[col] = fillColor;
  }

  SetTextColor(g_PhoenixGdiTextDibCache.hdc, fcolor);
  if (transparentBackground)
  {
    SetBkMode(g_PhoenixGdiTextDibCache.hdc, TRANSPARENT);
  }
  else
  {
    SetBkMode(g_PhoenixGdiTextDibCache.hdc, OPAQUE);
    SetBkColor(g_PhoenixGdiTextDibCache.hdc, bcolor);
  }

  // Faz T3: DWrite dene (grayscale AA, ayni DIB'e blend), olmazsa GDI
  Phoenix_TextOutDW(g_PhoenixGdiTextDibCache.hdc, 0, 0, str, textLength);

  int destLeft = x;
  int destTop = y;
  int destRight = x + textWidth;
  int destBottom = y + textHeight;

  if (destLeft < surface->GetClipLeft())
    destLeft = surface->GetClipLeft();
  if (destTop < surface->GetClipTop())
    destTop = surface->GetClipTop();
  if (destRight > surface->GetClipRight())
    destRight = surface->GetClipRight();
  if (destBottom > surface->GetClipBottom())
    destBottom = surface->GetClipBottom();
  if (destLeft < 0)
    destLeft = 0;
  if (destTop < 0)
    destTop = 0;
  if (destRight > surface->GetWidth())
    destRight = surface->GetWidth();
  if (destBottom > surface->GetHeight())
    destBottom = surface->GetHeight();

  if (destRight <= destLeft || destBottom <= destTop)
    return true;

  WORD* targetBits = NULL;
  WORD targetPitchBytes = 0;
  surface->LockW(targetBits, targetPitchBytes);
  if (targetBits == NULL || targetPitchBytes == 0)
    return false;

  const int targetPitchWords = targetPitchBytes / sizeof(WORD);
  const int sourceLeft = destLeft - x;
  const int sourceTop = destTop - y;

  for (int row = 0; row < destBottom - destTop; ++row)
  {
    const DWORD* src = g_PhoenixGdiTextDibCache.bits +
                       (sourceTop + row) * g_PhoenixGdiTextDibCache.pitchPixels +
                       sourceLeft;
    WORD* dst = targetBits + (destTop + row) * targetPitchWords + destLeft;

    for (int col = 0; col < destRight - destLeft; ++col)
    {
      const DWORD srcColor = src[col] & 0x00FFFFFF;
      if (!transparentBackground || srcColor != transparentKey)
        dst[col] = Phoenix_DibPixelToSurfaceWord(srcColor);
    }
  }

  surface->Unlock();
  return true;
}
}
#endif // PHOENIX_RENDER_BACKEND_D3D9

// #include "DebugInfo.h"
#ifdef _DEBUG
#define OUTPUT_DEBUG2
#endif

// #include "2D.h"
// #include <assert.h>

//----------------------------------------------------------------------
// gamma - -; not my code~~ - from sigi
//----------------------------------------------------------------------
#define GAMMA_MASK565_1PIXEL1 00000000000000001111100000011111b
#define GAMMA_MASK565_1PIXEL2 00000000000000000000011111100000b
#define GAMMA_MASK565_2PIXEL1 11111000000111110000011111100000b
#define GAMMA_MASK565_2PIXEL2 00000111111000001111100000011111b
#define GAMMA_MASK565_4PIXEL_R 11111000000000001111100000000000b
#define GAMMA_MASK565_4PIXEL_G 00000111111000000000011111100000b
#define GAMMA_MASK565_4PIXEL_B 00000000000111110000000000011111b

#define GAMMA_MASK555_1PIXEL1 00000000000000000111110000011111b
#define GAMMA_MASK555_1PIXEL2 00000000000000000000001111100000b
#define GAMMA_MASK555_2PIXEL1 01111100000111110000001111100000b
#define GAMMA_MASK555_2PIXEL2 00000011111000000111110000011111b
#define GAMMA_MASK555_4PIXEL_R 01111100000000000111110000000000b
#define GAMMA_MASK555_4PIXEL_G 00000011111000000000001111100000b
#define GAMMA_MASK555_4PIXEL_B 00000000000111110000000000011111b

//----------------------------------------------------------------------
// static
//----------------------------------------------------------------------
void (*CDirectDrawSurface::s_GammaFunction)(void *pDest, int len,
                                            int p) = Gamma4Pixel565;

#ifdef OUTPUT_DEBUG2
int g_SurfaceCount = 0;
#endif

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------
CDirectDrawSurface::CDirectDrawSurface() {
  m_pDDSurface = NULL;
  m_pRenderSurface = NULL;
  // Fix #11 (2026-05-14): Default owned=true (kendi yarattigi delegate'i siler).
  // D3D9 modunda InitBacksurface + InitOffsurface (full-screen) singleton
  // composition surface'i atar ve owned=false yapar (singleton silinmemeli).
  m_bRenderSurfaceOwned = true;
  m_Width = 0;
  m_Height = 0;
  m_ClipLeft = 0;
  m_ClipTop = 0;
  m_ClipRight = 0;
  m_ClipBottom = 0;
  m_bBackSurface = false;
  m_bLock = false;

  // 2026-05-14: m_ddsd ctor'da zero'lanir. Onceden uninit'di — D3D9 compat
  // path'inde Lock fail durumda m_ddsd.lpSurface garbage kaldi, sprite codec
  // (BltSprite vs.) WORD* cast yapinca garbage adres → segfault. Zero
  // garbage'i NULL'a indirir; BltSprite NULL guard'lari yakar.
  ZeroMemory(&m_ddsd, sizeof(m_ddsd));
  m_ddsd.dwSize = sizeof(m_ddsd);

#ifdef OUTPUT_DEBUG2
  g_SurfaceCount++;
#endif
}

CDirectDrawSurface::~CDirectDrawSurface() {
  // Phase 2: IRenderSurface delegate'i once sil. owned=false ile olusturuldugu
  // icin underlying m_pDDSurface'i release etmez — asagidaki m_pDDSurface
  // release path'i hala calisir, double-release riski yok.
  //
  // Fix #11 (2026-05-14): m_bRenderSurfaceOwned=false ise D3D9Backend
  // singleton composition surface'ini paylasiyoruz; silinmez (backend dtor
  // hallediyor). Owned=true normal davranis.
  if (m_pRenderSurface) {
    if (m_bRenderSurfaceOwned) {
      delete m_pRenderSurface;
    }
    m_pRenderSurface = NULL;
  }

  // BackSurface의 경우는
  // CDirectDraw에서 Release하기 때문에 Release해서는 안된다.
  if (!m_bBackSurface && m_pDDSurface != NULL) {
    m_pDDSurface->Release();
    m_pDDSurface = NULL;
  }

#ifdef OUTPUT_DEBUG2
  g_SurfaceCount--;
#endif
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// BackBuffer로 사용하는 Surface이다.
// CDirectDraw의 Init에서 생성된 걸 이용한다.
//
// `이미 초기화되어 있으면 false를 반환한다.
//----------------------------------------------------------------------
bool CDirectDrawSurface::InitBacksurface() {
  bool check = false;
  if (m_pDDSurface)
    check = true;

  m_pDDSurface = m_pDDSBack;
  m_Width = CDirectDraw::m_ScreenWidth;
  m_Height = CDirectDraw::m_ScreenHeight;
  m_ClipRight = m_Width;
  m_ClipBottom = m_Height;

  m_bBackSurface = true;

  // Phase 2: IRenderSurface delegate kur. Backbuffer surface'i CDirectDraw
  // tarafindan release edildigi icin owned=false. Eski m_pRenderSurface varsa
  // sil — surface yeniden init durumunda double-init bug'i onlenir.
  //
  // Fix #11 (2026-05-14): m_bRenderSurfaceOwned bayragina gore sil.
  if (m_pRenderSurface) {
    if (m_bRenderSurfaceOwned) {
      delete m_pRenderSurface;
    }
    m_pRenderSurface = NULL;
  }
  m_bRenderSurfaceOwned = true;

#if PHOENIX_RENDER_BACKEND_D3D9
  //--------------------------------------------------------------------
  // 2026-05-14 Fix #11: D3D9 modunda back surface = D3D9Backend composition
  // singleton'i DOGRUDAN. Onceki Fix #10 her InitBacksurface/InitOffsurface
  // cagrisi icin yeni bir D3D9Surface wrapper yaratiyordu (g_pBack + g_pLast
  // farkli D3D9Surface instances'lar). Ikisi ayni IDirect3DSurface9'a isaret
  // ettigi icin SYSTEMMEM surface'a iki bagimsiz LockRect cagrisi mumkundu
  // — driver undefined behavior → c0000374 heap corruption.
  //
  // Yeni strateji: singleton'i paylas. D3D9Surface'a eklenen ref-counted
  // Lock/Unlock sayesinde iki paylasilan caller (g_pBack + g_pLast veya
  // BltColorKey src+dst) ayni surface'i guvenli ekrana yazabilir. Singleton
  // sahiplik D3D9Backend'de kalir; m_bRenderSurfaceOwned=false isaretiyle
  // destructor singleton'i silmez.
  //--------------------------------------------------------------------
  // 2026-08-31 DX11 P0: concrete D3D9Backend yerine facade — aktif backend'in
  // (D3D9 veya D3D11) composition singleton'i doner; hazir degilse NULL.
  phoenix::render::IRenderSurface* pComp = phoenix::render::GetCompositionRenderSurface();
  if (pComp != NULL) {
    m_pRenderSurface = pComp;
    m_bRenderSurfaceOwned = false;
    CT_Done("DIAG: InitBacksurface — composition singleton bound (this:%p comp:%p)",
            this, pComp);
  } else {
    CT_Done("DIAG: InitBacksurface — composition surface NULL (backend init incomplete?)");
  }
#else
  if (m_pDDSurface) {
    m_pRenderSurface = new DDraw7Surface(m_pDDSurface, m_Width, m_Height,
                                          /*owned=*/false);
  }
#endif

  if (check)
    return false;
  else
    return true;
}

//----------------------------------------------------------------------
// dwCaps는 DDSCAPS_VIDEOMEMORY(default)나
//          DDSCAPS_SYSTEMMEMORY로 쓸 수 있다.
//
// system memory는 Lock/Unlock을 이용할 경우 빠르지만 Blt 등은 느리고..
// video memory는 그 반대다. (중요!)
//
// Lock과 Blt의 사용 비율을 고려해서 Surface의 memory 위치를 결정해야 한다.
//
// 이미 초기화되어 있으면 false를 반환한다.
//----------------------------------------------------------------------
bool CDirectDrawSurface::InitOffsurface(int wWidth, int wHeight, DWORD dwCaps) {
  bool check = false;
  if (m_pDDSurface) {
    // Phase 2: eski delegate'i once sil — m_pDDSurface'i wrap ediyor olabilir
    // Fix #11: m_bRenderSurfaceOwned bayragina gore sil.
    if (m_pRenderSurface) {
      if (m_bRenderSurfaceOwned) {
        delete m_pRenderSurface;
      }
      m_pRenderSurface = NULL;
    }
    m_pDDSurface->Release();
    m_pDDSurface = NULL;
    check = true;
  }

  // Fix #11: m_pDDSurface NULL ama m_pRenderSurface dolu olabilir
  // (D3D9 modunda InitOffsurface yeniden cagrisi). Burada da temizlik.
  if (m_pRenderSurface) {
    if (m_bRenderSurfaceOwned) {
      delete m_pRenderSurface;
    }
    m_pRenderSurface = NULL;
  }
  m_bRenderSurfaceOwned = true;

  DDSURFACEDESC2 ddsd;

  // Surface에 대한 정보 설정
  // m_pSurface를 OffScreen으로 설정한다.
  ZeroMemory(&ddsd, sizeof(ddsd));
  ddsd.dwSize = sizeof(ddsd);
  ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | dwCaps;

  /*
  if (dwCaps & DDSCAPS_VIDEOMEMORY
          && wWidth > m_ScreenWidth || wHeight > m_ScreenHeight)
  {
          ddsd.ddsCaps.dwCaps2 = DDCAPS2_WIDESURFACES;
  }
  */

  ddsd.dwWidth = wWidth;
  ddsd.dwHeight = wHeight;

  // 2026-05-14: D3D9 modunda **CreateSurface CAGRILMIYOR**. Surface yaratimi
  // D3D9Backend uzerinden yapilir.
  // Eski compat-only no-op'lar ekrana hicbir sey vermiyordu (siyah ekran).
  //
  // Yeni strateji: full-screen surface (splash screen g_pLast vs.) D3D9Backend
  // composition buffer'ina baglanir — sprite codec yazilari 16-bit R5G6B5
  // sysmem buffer'a iner, Present sirasinda ekrana akar. Kucuk surface'lar
  // (ProfileManager 55x70 vs.) iiyice NULL kalir; sprite codec NULL guard
  // ile no-op olur — bu compat use-case'leri zaten gorulemez (off-screen).
#if PHOENIX_RENDER_BACKEND_D3D9
  m_Width      = wWidth;
  m_Height     = wHeight;
  m_ClipRight  = wWidth;
  m_ClipBottom = wHeight;
  m_bBackSurface = false;

  // Fix #21 (2026-05-15): Heuristik SIKILASTIRILDI. Onceki Fix #11 yaklasimi
  // ">=512x384" surface'larin tumunu singleton'a wire'liyordu — ama composition
  // 1280x1024 iken farkli boyuttaki surface (orn. m_pTileSurface 1296x912)
  // wire edilince sprite codec pitch overflow yapiyor:
  //   - Composition pitch = 1280*2 = 2560
  //   - Sprite codec m_Width=1296 → per row 2592 byte yazi (32 byte taşma)
  //   - Cumulative row drift → eventually past buffer end → heap/AV
  //
  // YENI strateji:
  //   1. Dimensions composition ile TAM ESLESIYORSA → singleton paylas (g_pBack,
  //      g_pLast — screen-size surfaces). Ref-counted Lock + zero alloc overhead.
  //   2. Dimensions FARKLI ise (m_pTileSurface 1296x912, vs.) → DEDICATED D3D9
  //      surface yarat (CreateOffscreenSurface owned=true). Bu surface'in
  //      kendi IDirect3DSurface9 + kendi pitch + kendi byte boyutu var, sprite
  //      codec overflow yok. ~2.4MB ek alloc (1296*912*2) — kabul edilebilir.
  //
  // Composition'a Present sirasinda nasil entegre olur? m_pTileSurface bagimsiz
  // bir D3D9 surface — Phoenix render loop (MTopView::Draw) m_pTileSurface'i
  // sonradan g_pBack'e Blt eder (BltNoColorkey). CDirectDrawSurface delegate
  // BltColorKey/BltNoColorKey path'i 2 wrapper arasinda pixel copy yapar
  // (D3D9Surface::BltColorKey CPU memcpy fallback path) — singleton (g_pBack)
  // composition buffer'a tile data'sini push eder. Present pipeline finalize.
  // 2026-08-31 DX11 P0: concrete D3D9Backend yerine facade dispatch.
  phoenix::render::IRenderSurface* pComp = phoenix::render::GetCompositionRenderSurface();

  if (pComp != NULL
      && wWidth == pComp->GetWidth() && wHeight == pComp->GetHeight())
  {
    // Yol 1: Dimensions exact match — singleton paylas.
    m_pRenderSurface = pComp;
    m_bRenderSurfaceOwned = false;
    CT_Done("DIAG: InitOffsurface (%dx%d) — composition singleton bound (this:%p comp:%p)",
            wWidth, wHeight, this, pComp);
    return true;
  }

  // Yol 2: Farkli boyut — dedicated D3D9 surface yarat (SYSTEMMEM R5G6B5).
  // Cok kucuk surface'lar (cursor icons, dialog ornament 1x1 placeholders) icin
  // bile dedicated alloc — invisible olmasi yerine dogru render. Alloc fail
  // sessiz no-op (caller NULL surface'i hicbir sey yapmaz, sprite codec
  // NULL guard sayesinde crash etmez).
  if (wWidth >= 1 && wHeight >= 1)
  {
    IRenderSurface* pDedicated =
        phoenix::render::GetRenderBackend()->CreateOffscreenSurface(wWidth, wHeight, /*flags*/0);
    if (pDedicated != NULL)
    {
      m_pRenderSurface = pDedicated;
      m_bRenderSurfaceOwned = true;  // CDirectDrawSurface dtor sileceqi icin owned
      CT_Done("DIAG: InitOffsurface D3D9 (%dx%d) — dedicated surface allocated (this:%p)",
              wWidth, wHeight, this);
      return true;
    }
    CT_Done("DIAG: InitOffsurface D3D9 (%dx%d) — dedicated alloc FAILED (out of sysmem?)",
            wWidth, wHeight);
  }
  return true;
#else

  // DIAG: m_pDD NULL check ek. D3D9 modda Init() minimal IDD7 olusturuyor;
  // o da fail ettiyse buradan early-exit (surface yaratilamaz, ama crash da yok).
  if (m_pDD == NULL) {
    CT_Done("DIAG: InitOffsurface FAIL — m_pDD NULL (CDirectDraw::Init compat IDD7 olusturamadi)");
    return false;
  }

  HRESULT hrCreate = m_pDD->CreateSurface(&ddsd, &m_pDDSurface, NULL);
  if (hrCreate != DD_OK) {
    CT_Done("DIAG: InitOffsurface FAIL — CreateSurface hr=0x%08X (w=%d h=%d caps=0x%08X)",
            hrCreate, wWidth, wHeight, (DWORD)(DDSCAPS_OFFSCREENPLAIN | dwCaps));
    m_pDDSurface = NULL;
    return false;
  }

  // member init...
  m_Width = wWidth;
  m_Height = wHeight;
  m_ClipRight = wWidth;
  m_ClipBottom = wHeight;
  m_bBackSurface = false;

  // Phase 2: IRenderSurface delegate. CDirectDrawSurface destructor
  // m_pDDSurface'i release ettigi icin owned=false.
  m_pRenderSurface = new DDraw7Surface(m_pDDSurface, wWidth, wHeight,
                                        /*owned=*/false);

  // 투명색을 기본으로 설정해둔다.
  SetTransparency(0);

  if (check)
    return false;
  else
    return true;
#endif
}

//----------------------------------------------------------------------
// BMP file을 load한다.
//
// `내부에서 offscreen을 만들어 load한다.
//----------------------------------------------------------------------
bool CDirectDrawSurface::InitTextureSurfaceFromBMP(LPCSTR szBitmap,
                                                   DWORD dwCaps) {
  // 이미 사용중이면 지우고 Load한다.
  // Fix #11: m_bRenderSurfaceOwned bayragina gore sil.
  if (m_pDDSurface != NULL) {
    if (m_pRenderSurface) {
      if (m_bRenderSurfaceOwned) {
        delete m_pRenderSurface;
      }
      m_pRenderSurface = NULL;
    }
    m_pDDSurface->Release();
    m_pDDSurface = NULL;
  }
  m_bRenderSurfaceOwned = true;

  HBITMAP hbm;
  BITMAP bm;

  //
  //  Try to load the bitmap as a resource, if that fails, try it as a file
  //
  hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, 0, 0,
                           LR_CREATEDIBSECTION);
  if (hbm == NULL)
    hbm = (HBITMAP)LoadImage(NULL, szBitmap, IMAGE_BITMAP, 0, 0,
                             LR_LOADFROMFILE | LR_CREATEDIBSECTION);
  if (hbm == NULL)
    return NULL;
  //
  // Get size of the bitmap
  //
  GetObject(hbm, sizeof(bm), &bm);

  InitTextureSurface(bm.bmWidth, bm.bmHeight, dwCaps);

  CopyBitmap(hbm, 0, 0, 0, 0);
  DeleteObject(hbm);

  return true;
}

//----------------------------------------------------------------------
//
// Texture로 사용할 Surface를 생성한다.
//
// 이미 초기화되어 있으면 false를 반환한다.
//----------------------------------------------------------------------
bool CDirectDrawSurface::InitTextureSurface(int wWidth, int wHeight,
                                            DWORD dwCaps,
                                            const LPDDPIXELFORMAT pDDPF) {
#ifdef OUTPUT_DEBUG
  if (g_pDebugMessage) {
    DEBUG_ADD_FORMAT("InitTextureSurface: (%d, %d)", wWidth, wHeight);
  }
#endif

  bool check = false;
  if (m_pDDSurface) {
    // Phase 2: eski delegate'i once sil
    // Fix #11: m_bRenderSurfaceOwned bayragina gore sil.
    if (m_pRenderSurface) {
      if (m_bRenderSurfaceOwned) {
        delete m_pRenderSurface;
      }
      m_pRenderSurface = NULL;
    }
    m_pDDSurface->Release();
    m_pDDSurface = NULL;
    check = true;
  } else if (m_pRenderSurface) {
    // Phase 5: flag ON modunda m_pDDSurface NULL ama m_pRenderSurface (D3D9
    // delegate) dolu olabilir — bu durumda tek basina temizle.
    // Fix #11: owned=false ise singleton, silmemeli.
    if (m_bRenderSurfaceOwned) {
      delete m_pRenderSurface;
    }
    m_pRenderSurface = NULL;
    check = true;
  }
  m_bRenderSurfaceOwned = true;

#if PHOENIX_RENDER_BACKEND_D3D9
  //--------------------------------------------------------------------
  // Phase 5 — D3D9 native texture path
  //
  // Flag ON modunda DDraw7 surface yaratimi atlanir; D3D9Backend uzerinden
  // IDirect3DTexture9 (D3DPOOL_MANAGED) olusturulur. Surface level 0 alinip
  // D3D9Surface delegate olarak m_pRenderSurface'a baglanir; sprite codec
  // (CSpriteSurface) Lock/Unlock yolu Phase 2 abstraction sayesinde
  // dokunulmadan D3D9 D3DLOCKED_RECT.pBits/Pitch uzerinden veri yazar.
  //
  // pDDPF parametresi (CDirect3D::GetPixelFormat4444/1555/...) MapDDPixel-
  // FormatToD3D9 ile D3DFMT_* enum'a cevrilir. pDDPF NULL ise sprite codec
  // default'u R5G6B5 kullanilir.
  //--------------------------------------------------------------------
  {
    // 2026-08-31 DX11 P0: DX11 modunda texture surface = CPU bellek surface
    // (istenen formatta). Sprite codec CPU Lock/Unlock ile doldurur; D3D7
    // texture-draw yollari D3D9 modundaki gibi devre disi kaldigi icin GPU
    // texture'a gerek yok. Format mapping asagidaki MapDDPixelFormatToD3D9
    // ile ayni (yalniz SurfaceFormat kismi kullanilir).
    if (phoenix::render::IsD3D11Selected()) {
      D3DFORMAT     d3dFmt11;
      SurfaceFormat surfaceFmt11;
      MapDDPixelFormatToD3D9(pDDPF, d3dFmt11, surfaceFmt11);

      phoenix::render::D3D11Surface* pMem =
          phoenix::render::D3D11Backend::Instance().CreateMemorySurface(
              wWidth, wHeight, surfaceFmt11);
      if (pMem == NULL) {
        CT_Done("DIAG: InitTextureSurface D3D11 CreateMemorySurface FAILED (%dx%d)",
                wWidth, wHeight);
        return false;
      }

      m_pRenderSurface = pMem;
      m_bRenderSurfaceOwned = true;

      // D3D9 dali ile ayni state duzeni (m_pDDSurface NULL, m_ddsd sifir).
      m_pDDSurface = NULL;
      ZeroMemory(&m_ddsd, sizeof(m_ddsd));
      m_ddsd.dwSize = sizeof(m_ddsd);

      m_Width      = wWidth;
      m_Height     = wHeight;
      m_ClipLeft   = 0;
      m_ClipTop    = 0;
      m_ClipRight  = wWidth;
      m_ClipBottom = wHeight;
      m_bBackSurface = false;
      return check ? false : true;
    }

    D3D9Backend& backend = D3D9Backend::Instance();
    IDirect3DDevice9Ex* pDev = backend.GetD3D9Device();
    if (!pDev) {
      // D3D9 device henuz init edilmedi — Phase 4.5 lifecycle agent'in
      // sirasi gelecek. Bu durumda hata loglamayalim (sessiz fail) cunku
      // CTexturePack constructor erken cagrilabilir.
      CT_Done("DIAG: InitTextureSurface D3D9 device NULL (early init?)");
      return false;
    }

    D3DFORMAT     d3dFmt;
    SurfaceFormat surfaceFmt;
    MapDDPixelFormatToD3D9(pDDPF, d3dFmt, surfaceFmt);

    // 2026-05-16 R2: POOL_DEFAULT static-pool sentinel-bit.
    //   PHOENIX_TEXCAPS_STATIC_POOL set ise CTexturePack/CTexturePartManager
    //   turevleri uzerinden gelen "write-once" atlas alloc. DYNAMIC + DEFAULT
    //   pool secilir — sysmem ikiz kopya elenir, ~150 MB tasarruf hedefi.
    //
    //   Set DEGIL ise eski semantik (MANAGED + USAGE=0) korunur. MTopView
    //   m_pLightBufferTexture (frame-by-frame light buffer) ve diger dinamik
    //   path'ler bu yola devam eder.
    //
    // 2026-05-17 FAZ 3 Fix #1: POOL_DEFAULT lost-device recreate ROLLBACK.
    //   R2 sonrasi gozlenen ALT-TAB resume + zone transition "FPS drop" aslinda
    //   POOL_DEFAULT atlas surfaces'in D3DERR_DEVICELOST sonrasi recreate
    //   edilmemesi -> Lock() garbage -> Blt no-op -> content-missing frames.
    //   D3D9Backend::RestoreAll yalnizca composition texture + cached back
    //   buffer'i recreate ediyor; CTexturePack / CShadowPartManager /
    //   CSpriteTexturePartManager / CNormalSpriteTexturePartManager /
    //   CTexturePartManager uzerinden olusan ~yuzlerce atlas D3D9 reset sonrasi
    //   olu kaliyordu.
    //
    //   Geri MANAGED'a doneriz — driver lost-device recreate'i otomatik yapar
    //   (sysmem backing kopyasindan). PHOENIX_TEXCAPS_STATIC_POOL bayragi API
    //   olarak kalir (InitTextureSurfaceStatic ismi de) — gelecekteki Option A
    //   (registry + recreate) implementasyonunda tekrar DEFAULT'a alinabilir.
    //   Su an POOL_DEFAULT yolu disabled; bayrak gorunce de MANAGED+USAGE=0
    //   secilir.
    //
    //   R2'nin ALT-TAB 4->30 FPS kazanci aslinda D3D9Backend::Present focus-
    //   loss Sleep + Method B render skip'ten geliyordu (D3D9Backend.cpp:1100-
    //   1120, CWinMain D5b) — pool secimi degil. ~150 MB sysmem tasarrufu
    //   kaybedilir (kabul edilebilir; modern PC'lerde sorun degil).
    const bool bStaticPool = (dwCaps & PHOENIX_TEXCAPS_STATIC_POOL) != 0;
    (void)bStaticPool;  // FAZ 3 Fix #1: gecici olarak yolu disable; bayrak gorseli korur
    const D3DPOOL poolKind  = D3DPOOL_MANAGED;
    const DWORD   usageBits = 0;

    IDirect3DTexture9* pTex = NULL;
    HRESULT hr = pDev->CreateTexture((UINT)wWidth, (UINT)wHeight,
                                     1,          // 1 mip-level (no mipchain)
                                     usageBits,
                                     d3dFmt,
                                     poolKind,
                                     &pTex, NULL);
    if (FAILED(hr) || !pTex) {
      CT_Done("DIAG: InitTextureSurface CreateTexture FAILED 0x%08X "
              "(w=%d h=%d fmt=%d)", hr, wWidth, wHeight, (int)d3dFmt);
      return false;
    }

    IDirect3DSurface9* pLevel = NULL;
    hr = pTex->GetSurfaceLevel(0, &pLevel);
    if (FAILED(hr) || !pLevel) {
      CT_Done("DIAG: InitTextureSurface GetSurfaceLevel FAILED 0x%08X", hr);
      pTex->Release();
      return false;
    }

    // GetSurfaceLevel AddRef yapar; texture'i Release ediyoruz ama surface
    // hala canli kalir (D3D9 internal refcount). D3D9Surface destruct'da
    // surface Release edildiginde texture'in son ref'i dusup memory free
    // olur (managed pool driver tarafi handle).
    pTex->Release();

    // Delegate atamasi — Lock/Blt/Fill/Restore artik D3D9Surface uzerinden
    // gecer. owned=true: D3D9Surface dtor surface Release eder.
    m_pRenderSurface = new D3D9Surface(pLevel, wWidth, wHeight,
                                       surfaceFmt, /*owned=*/true);

    // m_pDDSurface NULL kalir — flag ON modunda DDraw7 escape hatch
    // kullanilmaz. m_ddsd Lock zamani LockedRect ile doldurulur (Lock()
    // implementasyonu Phase 2'de gerceklesti).
    m_pDDSurface = NULL;
    ZeroMemory(&m_ddsd, sizeof(m_ddsd));
    m_ddsd.dwSize = sizeof(m_ddsd);

    m_Width      = wWidth;
    m_Height     = wHeight;
    m_ClipLeft   = 0;
    m_ClipTop    = 0;
    m_ClipRight  = m_Width;
    m_ClipBottom = m_Height;
    m_bBackSurface = false;
    // dwCaps PHOENIX_TEXCAPS_STATIC_POOL disindaki DDSCAPS_* bitler D3D9
    // modunda anlamsiz — yukaridaki pool/usage hesabi dwCaps'i tuketti.

    // Phase 2 InitTextureSurface down-flow ile parite: check (eski surface
    // varsa true) durumunda historical return false (legacy davranis).
    return check ? false : true;
  }
#else
#ifdef OUTPUT_DEBUG
  if (g_pDebugMessage) {
    DEBUG_ADD("Before ZeroMemory");
  }
#endif

  DDSURFACEDESC2 ddsd;

  // Surface에 대한 정보 설정
  // m_pSurface를 OffScreen으로 설정한다.
  ZeroMemory(&ddsd, sizeof(ddsd));
  ddsd.dwSize = sizeof(ddsd);

  ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH; // | DDSD_CKSRCBLT;
  // 2026-05-16 R2: PHOENIX_TEXCAPS_STATIC_POOL bayragi sadece D3D9 pool
  // secimi icin; DDraw7 fallback DDSCAPS_OPTIMIZED (0x80000000) ile cakisir,
  // DDraw7 surface validation kirilmasin diye strip ediyoruz.
  ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | (dwCaps & ~PHOENIX_TEXCAPS_STATIC_POOL);
  ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
  ddsd.dwWidth = wWidth;
  ddsd.dwHeight = wHeight;

#ifdef OUTPUT_DEBUG
  if (g_pDebugMessage) {
    DEBUG_ADD("Set PixelFormat");
  }
#endif

  // pixelformat을 임의로 정하는 경우
  if (pDDPF != NULL) {
    ddsd.dwFlags |= DDSD_PIXELFORMAT;
    memcpy(&ddsd.ddpfPixelFormat, pDDPF, sizeof(DDPIXELFORMAT));
  }

#ifdef OUTPUT_DEBUG
  if (g_pDebugMessage) {
    DEBUG_ADD("Before CreateSurface");
  }
#endif

  HRESULT hr = m_pDD->CreateSurface(&ddsd, &m_pDDSurface, NULL);
  if (hr != DD_OK) {
#ifdef OUTPUT_DEBUG
    if (g_pDebugMessage) {
      DEBUG_ADD("CreateSurface Error");

      switch (hr) {
      case DDERR_INCOMPATIBLEPRIMARY:
        DEBUG_ADD("DD-Create TextureSurface Error : DDERR_INCOMPATIBLEPRIMARY");
        break;
      case DDERR_INVALIDCAPS:
        DEBUG_ADD("DD-Create TextureSurface Error : DDERR_INVALIDCAPS");
        break;
      case DDERR_INVALIDOBJECT:
        DEBUG_ADD("DD-Create TextureSurface Error : DDERR_INVALIDOBJECT");
        break;
      case DDERR_INVALIDPARAMS:
        DEBUG_ADD("DD-Create TextureSurface Error : DDERR_INVALIDPARAMS");
        break;
      case DDERR_INVALIDPIXELFORMAT:
        DEBUG_ADD("DD-Create TextureSurface Error : DDERR_INVALIDPIXELFORMAT");
        break;
      case DDERR_NOALPHAHW:
        DEBUG_ADD("DD-Create TextureSurface Error : DDERR_NOALPHAHW");
        break;
      case DDERR_NOCOOPERATIVELEVELSET:
        DEBUG_ADD(
            "DD-Create TextureSurface Error : DDERR_NOCOOPERATIVELEVELSET");
        break;
      case DDERR_NODIRECTDRAWHW:
        DEBUG_ADD("DD-Create TextureSurface Error : DDERR_NODIRECTDRAWHW");
        break;
      case DDERR_NOEMULATION:
        DEBUG_ADD("DD-Create TextureSurface Error : DDERR_NOEMULATION");
        break;
      case DDERR_NOEXCLUSIVEMODE:
        DEBUG_ADD("DD-Create TextureSurface Error : DDERR_NOEXCLUSIVEMODE");
        break;
      case DDERR_NOFLIPHW:
        DEBUG_ADD("DD-Create TextureSurface Error : DDERR_NOFLIPHW");
        break;
      case DDERR_NOMIPMAPHW:
        DEBUG_ADD("DD-Create TextureSurface Error : DDERR_NOMIPMAPHW");
        break;
      case DDERR_NOOVERLAYHW:
        DEBUG_ADD("DD-Create TextureSurface Error : DDERR_NOOVERLAYHW");
        break;
      case DDERR_NOZBUFFERHW:
        DEBUG_ADD("DD-Create TextureSurface Error : DDERR_NOZBUFFERHW");
        break;
      case DDERR_OUTOFMEMORY:
        DEBUG_ADD("DD-Create TextureSurface Error : DDERR_OUTOFMEMORY");
        break;
      case DDERR_OUTOFVIDEOMEMORY:
        DEBUG_ADD("DD-Create TextureSurface Error : DDERR_OUTOFVIDEOMEMORY");
        break;
      case DDERR_PRIMARYSURFACEALREADYEXISTS:
        DEBUG_ADD("DD-Create TextureSurface Error : "
                  "DDERR_PRIMARYSURFACEALREADYEXISTS");
        break;
      case DDERR_UNSUPPORTEDMODE:
        DEBUG_ADD("DD-Create TextureSurface Error : DDERR_UNSUPPORTEDMODE");
        break;
      };
    }
#endif

    /*
    switch (hr)
    {
            case DDERR_INCOMPATIBLEPRIMARY		:
    DirectDrawFailed("DD-Create TextureSurface Error :
    DDERR_INCOMPATIBLEPRIMARY"); break; case DDERR_INVALIDCAPS
    : DirectDrawFailed("DD-Create TextureSurface Error : DDERR_INVALIDCAPS");
    break; case DDERR_INVALIDOBJECT			:
    DirectDrawFailed("DD-Create TextureSurface Error : DDERR_INVALIDOBJECT");
    break; case DDERR_INVALIDPARAMS			:
    DirectDrawFailed("DD-Create TextureSurface Error : DDERR_INVALIDPARAMS");
    break; case DDERR_INVALIDPIXELFORMAT		:
    DirectDrawFailed("DD-Create TextureSurface Error :
    DDERR_INVALIDPIXELFORMAT"); break; case DDERR_NOALPHAHW
    : DirectDrawFailed("DD-Create TextureSurface Error : DDERR_NOALPHAHW");
    break; case DDERR_NOCOOPERATIVELEVELSET	: DirectDrawFailed("DD-Create
    TextureSurface Error : DDERR_NOCOOPERATIVELEVELSET"); break; case
    DDERR_NODIRECTDRAWHW			: DirectDrawFailed("DD-Create
    TextureSurface Error : DDERR_NODIRECTDRAWHW"); break; case DDERR_NOEMULATION
    : DirectDrawFailed("DD-Create TextureSurface Error : DDERR_NOEMULATION");
    break; case DDERR_NOEXCLUSIVEMODE			:
    DirectDrawFailed("DD-Create TextureSurface Error : DDERR_NOEXCLUSIVEMODE");
    break; case DDERR_NOFLIPHW					:
    DirectDrawFailed("DD-Create TextureSurface Error : DDERR_NOFLIPHW"); break;
            case DDERR_NOMIPMAPHW				:
    DirectDrawFailed("DD-Create TextureSurface Error : DDERR_NOMIPMAPHW");
    break; case DDERR_NOOVERLAYHW				:
    DirectDrawFailed("DD-Create TextureSurface Error : DDERR_NOOVERLAYHW");
    break; case DDERR_NOZBUFFERHW				:
    DirectDrawFailed("DD-Create TextureSurface Error : DDERR_NOZBUFFERHW");
    break; case DDERR_OUTOFMEMORY				:
    DirectDrawFailed("DD-Create TextureSurface Error : DDERR_OUTOFMEMORY");
    break; case DDERR_OUTOFVIDEOMEMORY			:
    DirectDrawFailed("DD-Create TextureSurface Error : DDERR_OUTOFVIDEOMEMORY");
    break; case DDERR_PRIMARYSURFACEALREADYEXISTS  : DirectDrawFailed("DD-Create
    TextureSurface Error : DDERR_PRIMARYSURFACEALREADYEXISTS"); break; case
    DDERR_UNSUPPORTEDMODE			: DirectDrawFailed("DD-Create
    TextureSurface Error : DDERR_UNSUPPORTEDMODE"); break;
    };
    */

    return false;
  } else {
#ifdef OUTPUT_DEBUG
    if (g_pDebugMessage) {
      DEBUG_ADD("After CreateSurface");
    }
#endif
  }

  // member init...
  m_Width = wWidth;
  m_Height = wHeight;
  m_ClipRight = m_Width;
  m_ClipBottom = m_Height;
  m_bBackSurface = false;

  // Phase 2: IRenderSurface delegate. Texture surface CDirectDrawSurface
  // dtor tarafindan release edildigi icin owned=false. DDraw7Surface ayni
  // raw DDraw7 surface'i wrap eder; double-release riski yok.
  m_pRenderSurface = new DDraw7Surface(m_pDDSurface, wWidth, wHeight,
                                        /*owned=*/false);

  if (check)
    return false;
  else
    return true;
#endif // PHOENIX_RENDER_BACKEND_D3D9
}

//----------------------------------------------------------------------
// 2026-05-16 R2: POOL_DEFAULT pool static-texture variant.
//
// Atlas/sprite-pack texture'lar (CTexturePack, CTexturePartManager
// turevleri, CSpriteTexturePartManager, CNormalSpriteTexturePartManager,
// CShadowPartManager) "write-once at load + read-many at draw" karakterli.
// D3DPOOL_MANAGED ile D3D9 driver sysmem + vidmem iki kopya tutar; Phoenix
// runtime ~150 MB sysmem ek yuke maruz. POOL_DEFAULT + USAGE_DYNAMIC ile
// sadece vidmem kopyasi tutulur; Lock/Unlock disardan calismaya devam eder
// (DYNAMIC pool semantic'i).
//
// Implementasyon: dwCaps'a PHOENIX_TEXCAPS_STATIC_POOL bayragini OR ile
// ekleyip InitTextureSurface'a delegate ediyoruz. Body bayragi gorunce
// D3D9 path icin POOL_DEFAULT + USAGE_DYNAMIC secer; DDraw7 fallback'inde
// bayrak strip edilir.
//
// Dinamik (frame-by-frame) texture'lar (orn. MTopView.cpp:3834
// m_pLightBufferTexture, light filter) bu yola GIRMEMELI — onlar
// InitTextureSurface kullanmaya devam etsin (MANAGED + USAGE=0).
//----------------------------------------------------------------------
bool CDirectDrawSurface::InitTextureSurfaceStatic(int wWidth, int wHeight,
                                                  DWORD dwCaps,
                                                  const LPDDPIXELFORMAT pDDPF) {
  return InitTextureSurface(wWidth, wHeight,
                            dwCaps | PHOENIX_TEXCAPS_STATIC_POOL,
                            pDDPF);
}

//----------------------------------------------------------------------
// Restore Surface
//----------------------------------------------------------------------
bool CDirectDrawSurface::Restore() {
  // Phase 2: IRenderSurface delegate path. DDraw7Surface::Restore icinde
  // RestoreAllSurfaces + 100x m_pDDSurface->Restore loop mantigi aynisi
  // bulunur — sprite codec ile davranis byte-byte korunur.
  if (m_pRenderSurface) {
    return m_pRenderSurface->Restore();
  }

  // Fallback (m_pRenderSurface NULL ise, init asamasinda olabilir): eski
  // raw DDraw7 yolu.
  RestoreAllSurfaces(); // 내부에서 error 처리.

  HRESULT hr;
  // restore current surface
  // if (m_pDDSurface->IsLost() == DDERR_SURFACELOST)
  //{

  // restore 100번 시도... - -;
  for (int i = 0; i < 100; i++) {
    hr = m_pDDSurface->Restore();

    if (hr == DD_OK) {
      return true;
    }
  }

  /*
  switch (hr)
  {
          case DDERR_GENERIC : DirectDrawFailed("DXSurface
  Restore:DDERR_GENERIC"); break; case DDERR_IMPLICITLYCREATED :
  DirectDrawFailed("DXSurface Restore:DDERR_IMPLICITLYCREATED"); break; case
  DDERR_INCOMPATIBLEPRIMARY : DirectDrawFailed("DXSurface
  Restore:DDERR_INCOMPATIBLEPRIMARY"); break; case DDERR_INVALIDOBJECT :
  DirectDrawFailed("DXSurface Restore:DDERR_INVALIDOBJECT"); break; case
  DDERR_INVALIDPARAMS : DirectDrawFailed("DXSurface
  Restore:DDERR_INVALIDPARAMS"); break; case DDERR_NOEXCLUSIVEMODE :
  DirectDrawFailed("DXSurface Restore:DDERR_NOEXCLUSIVEMODE"); break; case
  DDERR_OUTOFMEMORY : DirectDrawFailed("DXSurface Restore:DDERR_OUTOFMEMORY");
  break; case DDERR_UNSUPPORTED : DirectDrawFailed("DXSurface
  Restore:DDERR_UNSUPPORTED"); break; case DDERR_WRONGMODE :
  DirectDrawFailed("DXSurface Restore:DDERR_WRONGMODE"); break;
  }
  */
  //}

  return false;
}

//----------------------------------------------------------------------
// BMP file을 load한다.
//
// `내부에서 offscreen을 만들어 load한다.
//----------------------------------------------------------------------
bool CDirectDrawSurface::InitFromBMP(LPCSTR szBitmap, DWORD dwCaps) {
  // 이미 사용중이면 지우고 Load한다.
  // Fix #11: m_bRenderSurfaceOwned bayragina gore sil.
  if (m_pDDSurface != NULL) {
    if (m_pRenderSurface) {
      if (m_bRenderSurfaceOwned) {
        delete m_pRenderSurface;
      }
      m_pRenderSurface = NULL;
    }
    m_pDDSurface->Release();
    m_pDDSurface = NULL;
  }
  m_bRenderSurfaceOwned = true;

  HBITMAP hbm;
  BITMAP bm;

  //
  //  Try to load the bitmap as a resource, if that fails, try it as a file
  //
  hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, 0, 0,
                           LR_CREATEDIBSECTION);
  if (hbm == NULL)
    hbm = (HBITMAP)LoadImage(NULL, szBitmap, IMAGE_BITMAP, 0, 0,
                             LR_LOADFROMFILE | LR_CREATEDIBSECTION);
  if (hbm == NULL)
    return NULL;
  //
  // Get size of the bitmap
  //
  GetObject(hbm, sizeof(bm), &bm);

  if (!InitOffsurface(bm.bmWidth, bm.bmHeight, dwCaps)) {
    DeleteObject(hbm);
    DirectDrawFailed("Load BMP Failed!");
    return false;
  }

  CopyBitmap(hbm, 0, 0, 0, 0);
  DeleteObject(hbm);

  return true;
}

//----------------------------------------------------------------------
// 한번 Load되었던 BMP file을 다시 Load한다.
//
// `음... 이거 잘 안되는 거 같다. - 2000.1.5.
//----------------------------------------------------------------------
bool CDirectDrawSurface::ReLoadBMP(LPCSTR szBitmap) {
  //-----------------------------------------------------------------------------
  // Name: DDReLoadBitmap()
  // Desc: Load a bitmap from a file or resource into a directdraw surface.
  //       normaly used to re-load a surface after a restore.
  //-----------------------------------------------------------------------------

  HBITMAP hbm;
  HRESULT hr;

  //
  //  Try to load the bitmap as a resource, if that fails, try it as a file
  //
  hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, 0, 0,
                           LR_CREATEDIBSECTION);
  if (hbm == NULL)
    hbm = (HBITMAP)LoadImage(NULL, szBitmap, IMAGE_BITMAP, 0, 0,
                             LR_LOADFROMFILE | LR_CREATEDIBSECTION);
  if (hbm == NULL) {
    OutputDebugString("handle is null\n");
    return false;
  }

  hr = CopyBitmap(hbm, 0, 0, 0, 0);
  if (hr != DD_OK) {
    // DirectDrawFailed("Load BMP("+CString(szBitmap)+") Failed!");
    DeleteObject(hbm);
    DirectDrawFailed("Re-Load BMP Failed!");
    return false;
  }
  DeleteObject(hbm);

  return true;
}

//----------------------------------------------------------------------
// Bitmap을 Surface로 Copy한다.
//----------------------------------------------------------------------
bool CDirectDrawSurface::CopyBitmap(HBITMAP hbm, int x, int y, int dx, int dy) {
  HDC hdcImage;
  HDC hdc;
  BITMAP bm;
  DDSURFACEDESC2 ddsd;
  HRESULT hr;

  if (hbm == NULL || m_pDDSurface == NULL)
    return false;
  //
  // Make sure this surface is restored.
  //
  m_pDDSurface->Restore();
  //
  // Select bitmap into a memoryDC so we can use it.
  //
  hdcImage = CreateCompatibleDC(NULL);
  if (!hdcImage)
  {
    OutputDebugString("createcompatible dc failed\n");
    return false;
  }
  HGDIOBJ hOldBitmap = SelectObject(hdcImage, hbm);
  //
  // Get size of the bitmap
  //
  GetObject(hbm, sizeof(bm), &bm);
  dx = dx == 0 ? bm.bmWidth : dx; // Use the passed size, unless zero
  dy = dy == 0 ? bm.bmHeight : dy;
  //
  // Get size of surface.
  //
  ddsd.dwSize = sizeof(ddsd);
  ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
  m_pDDSurface->GetSurfaceDesc(&ddsd);

  if ((hr = m_pDDSurface->GetDC(&hdc)) == DD_OK) {
    StretchBlt(hdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, hdcImage, x, y, dx, dy,
               SRCCOPY);
    m_pDDSurface->ReleaseDC(hdc);
  }
  if (hOldBitmap != NULL)
    SelectObject(hdcImage, hOldBitmap);
  DeleteDC(hdcImage);

  return true;
}

//----------------------------------------------------------------------
// transparency color를 dwValue로 설정한다. (필수!!)
// Phase 2: IRenderSurface delegate. DDraw7Surface::SetTransparency raw
// SetColorKey cagirir; D3D9Surface state'te saklar (BltColorKey'de filtre).
//----------------------------------------------------------------------
void CDirectDrawSurface::SetTransparency(DWORD dwValue) {
  // Phase 2: IRenderSurface delegate. DDraw7Surface::SetTransparency
  // raw m_pDDSurface->SetColorKey cagirir; D3D9Surface state'te saklar
  // ve BltColorKey path'inde filtre uygular. Delegate yoluyla iki backend
  // de tek bir API uzerinden yonetilir.
  if (m_pRenderSurface) {
    m_pRenderSurface->SetTransparency(dwValue);
    return;
  }

  // Fallback (m_pRenderSurface yoksa) eski raw yol
  if (m_pDDSurface) {
    DDCOLORKEY ddck;

    // 투명색을 dwValue로 설정
    // ( Low에서 High까지 )
    ddck.dwColorSpaceLowValue = dwValue;
    ddck.dwColorSpaceHighValue = dwValue;

    m_pDDSurface->SetColorKey(DDCKEY_SRCBLT, &ddck);
  }
}

//----------------------------------------------------------------------
// Phase 3 (2026-05-14): GetColorKey + HasColorKey public accessor'lari.
// IRenderSurface contract'inda Phase 3 ile pure virtual hale geldi.
// CDirectDrawSurface caller'lari (16+ dosya) buna direkt erisebilir, ama
// Phase 3'te kullanim yok — Phase 4 switch sonrasi composition path'de
// alpha-test referansi olarak kullanilabilecek. Mevcut DDraw7 davranisi
// raw m_pDDSurface->GetColorKey yerine state-based donus; iki backend
// arasinda parity korumak icin tercih edildi.
//----------------------------------------------------------------------
DWORD CDirectDrawSurface::GetColorKey() const {
  if (m_pRenderSurface) {
    return m_pRenderSurface->GetColorKey();
  }
  // Fallback: delegate yoksa colorkey state takip etmedik, 0 don
  return 0;
}

bool CDirectDrawSurface::HasColorKey() const {
  if (m_pRenderSurface) {
    return m_pRenderSurface->HasColorKey();
  }
  return false;
}

//----------------------------------------------------------------------
// Surface 전체를 한 색깔로 채우기
// Phase 2: IRenderSurface delegate (eskisinde Blt(DDBLT_COLORFILL)).
//----------------------------------------------------------------------
void CDirectDrawSurface::FillSurface(WORD color) {
  if (m_pRenderSurface) {
    m_pRenderSurface->FillSurface(color);
    return;
  }

  // Fallback eski raw DDraw7 yolu (m_pRenderSurface NULL ise). D3D9 modda
  // InitOffsurface CreateSurface fail durumunda m_pDDSurface da NULL kalabilir;
  // NULL guard zorunlu (uncaught crash sebebiyle InitSurface assert ettiriyordu).
  if (m_pDDSurface == NULL) {
    return;
  }

  DDBLTFX ddbltfx;

  ddbltfx.dwSize = sizeof(DDBLTFX);
  ddbltfx.dwFillColor = color;

  m_pDDSurface->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
}

//----------------------------------------------------------------------
// 사각형 그리기
// Phase 2: IRenderSurface delegate. Clip mantigi delegate icindekiyle
// ozdes — CDirectDrawSurface seviyesinde clip kullanmaya gerek yok.
// AMA delegate clip state'i kendi sahiplenir; m_ClipLeft/m_ClipRight
// callerlardan gelen state ile sync olmali (SetClip metotlari).
//----------------------------------------------------------------------
void CDirectDrawSurface::FillRect(RECT *pRect, WORD color) {
  if (m_pRenderSurface) {
    // Delegate'in clip state'ini guncelle — CDirectDrawSurface inline
    // SetClip*'lar m_ClipLeft/Right/Top/Bottom'a yaziyor, delegate
    // bilmiyor. FillRect/Blt cagrisindan once syncle.
    m_pRenderSurface->SetClip(MakeClipRect(m_ClipLeft, m_ClipTop,
                                            m_ClipRight, m_ClipBottom));
    m_pRenderSurface->FillRect(pRect, color);
    return;
  }

  // Fallback eski raw DDraw7 yolu. D3D9 modda InitOffsurface fail durumunda
  // m_pDDSurface NULL kalabilir — NULL guard.
  if (m_pDDSurface == NULL) {
    return;
  }

  RECT rect = *pRect;
  // pRect clipping
  if (rect.left > m_ClipRight)
    return;
  if (rect.top > m_ClipBottom)
    return;
  if (rect.left < m_ClipLeft)
    rect.left = m_ClipLeft;
  if (rect.right > m_ClipRight)
    rect.right = m_ClipRight;
  if (rect.top < m_ClipTop)
    rect.top = m_ClipTop;
  if (rect.bottom > m_ClipBottom)
    rect.bottom = m_ClipBottom;

  DDBLTFX ddbltfx;

  ddbltfx.dwSize = sizeof(ddbltfx);
  ddbltfx.dwFillColor = color;

  m_pDDSurface->Blt(&rect, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &ddbltfx);
}

//----------------------------------------------------------------------
// Lock() : Surface에 직접 그리기 위한 정보
//
// lpSurface : surface memory 시작 위치
// lPitch    : 한 줄 수
//
// Surface에 대해서 Lock()을 사용한 후는 반드시 Unlock()을 해줘야 한다.
//
// `실패하면 false를 반환하는 데, 이 때는 Unlock()을 해주지 말아야 한다.
//----------------------------------------------------------------------

bool CDirectDrawSurface::Lock() {
  // Phase 2: IRenderSurface delegate. Sprite codec yolu artik buradan
  // gecer. KRITIK: m_ddsd static — GetSurfacePointer()/GetSurfacePitch()
  // ve GammaBox* hala m_ddsd.lpSurface + m_ddsd.lPitch okuyor. Lock
  // sonrasi m_ddsd'yi delegate'in dondurdugu LockedRect'ten dolduruyoruz
  // ki byte-byte aynisi davranis korunsun.
  if (m_pRenderSurface) {
    LockedRect locked;
    DWORD lockFlags = LockFlag_Wait;
    if (!m_pRenderSurface->Lock(locked, lockFlags)) {
      static DWORD lastLockFail = 0;
      if (GetTickCount() - lastLockFail > 5000) {
        CT_Done("DIAG: CDirectDrawSurface::Lock (delegate) FAILED (this:%p)", this);
        lastLockFail = GetTickCount();
      }
      return false;
    }

    // m_ddsd'yi delegate sonucundan doldur — sprite codec + GammaBox* +
    // GetSurfacePointer/Pitch hala m_ddsd uzerinden surface erisimi yapar.
    ZeroMemory(&m_ddsd, sizeof(m_ddsd));
    m_ddsd.dwSize        = sizeof(m_ddsd);
    m_ddsd.lpSurface     = locked.pBits;
    m_ddsd.lPitch        = locked.pitch;
    m_ddsd.dwWidth       = (DWORD)locked.width;
    m_ddsd.dwHeight      = (DWORD)locked.height;
    m_ddsd.dwFlags       = DDSD_PITCH | DDSD_LPSURFACE | DDSD_WIDTH | DDSD_HEIGHT;

    m_bLock = true;
    return true;
  }

  // Fallback (delegate yoksa) — eski raw DDraw7 yolu
  // D3D9 modunda compat path: m_pRenderSurface NULL + m_pDDSurface NULL —
  // CDirectDrawSurface stack-local'larin Lock cagrisi NULL deref'e gidiyordu
  // (DrawTitleLoading pSurface->Lock crash). Sessiz fail.
  if (m_pDDSurface == NULL) {
    ZeroMemory(&m_ddsd, sizeof(m_ddsd));
    m_ddsd.dwSize = sizeof(m_ddsd);
    return false;
  }

  HRESULT hr;
  bool ret;

  ZeroMemory(&m_ddsd, sizeof(m_ddsd));
  m_ddsd.dwSize = sizeof(m_ddsd);

  while (1) {
#ifdef OUTPUT_DEBUG
    if (g_pDebugMessage) {
      DEBUG_ADD("Before Lock()");
    }
#endif

    hr = m_pDDSurface->Lock(NULL, &m_ddsd, DDLOCK_WAIT, NULL);
    if (hr != DD_OK) {
        static DWORD lastLockFail = 0;
        if (GetTickCount() - lastLockFail > 5000) {
            CT_Done("DIAG: CDirectDrawSurface::Lock FAILED: 0x%08X (this:%p)", hr, this);
            lastLockFail = GetTickCount();
        }
    }

#ifdef OUTPUT_DEBUG
    if (g_pDebugMessage) {
      DEBUG_ADD("After Lock()");
    }
#endif

    if (hr == DD_OK) {
      break;
    } else if (hr == DDERR_SURFACELOST) {
      ret = RestoreAllSurfaces();
      (void)ret;
      continue;
    } else if (hr != DDERR_WASSTILLDRAWING) {
      break;
    }
  }

  if (hr == DD_OK) {
    m_bLock = true;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
// LockW — sprite codec'in en yaygin Lock kontrati: WORD pointer + pitch
// Phase 2: IRenderSurface delegate path. m_ddsd da senkron tutulur ki
// GetSurfacePointer/Pitch + GammaBox* tutarsiz olmasin.
//----------------------------------------------------------------------
void CDirectDrawSurface::LockW(WORD *&lpSurface, WORD &lPitch) {
  if (m_pRenderSurface) {
    LockedRect locked;
    if (!m_pRenderSurface->Lock(locked, LockFlag_None)) {
      lpSurface = NULL;
      lPitch    = 0;
      return;
    }

    ZeroMemory(&m_ddsd, sizeof(m_ddsd));
    m_ddsd.dwSize    = sizeof(m_ddsd);
    m_ddsd.lpSurface = locked.pBits;
    m_ddsd.lPitch    = locked.pitch;
    m_ddsd.dwWidth   = (DWORD)locked.width;
    m_ddsd.dwHeight  = (DWORD)locked.height;
    m_ddsd.dwFlags   = DDSD_PITCH | DDSD_LPSURFACE | DDSD_WIDTH | DDSD_HEIGHT;

    lpSurface = (WORD *)locked.pBits;
    lPitch    = (WORD)locked.pitch;

    m_bLock = true;
    return;
  }

  // Fallback eski yol
  // Fix #16 (2026-05-14): D3D9 modunda m_pDDSurface NULL — sessiz fail
  // (lpSurface=NULL, sprite codec NULL guard ile no-op).
  if (m_pDDSurface == NULL) {
    lpSurface = NULL;
    lPitch    = 0;
    return;
  }

  HRESULT hRet;

  ZeroMemory(&m_ddsd, sizeof(m_ddsd));
  m_ddsd.dwSize = sizeof(m_ddsd);

  bool exitLoop = false;

  do {
    hRet = m_pDDSurface->Lock(NULL, &m_ddsd, 0, NULL);
    if (hRet == DDERR_SURFACELOST) {
      Restore();
      hRet = m_pDDSurface->Lock(NULL, &m_ddsd, 0, NULL);
    }

    else if (hRet != DDERR_WASSTILLDRAWING)
      exitLoop = true;

  } while (!exitLoop);

  lpSurface = (WORD *)m_ddsd.lpSurface;
  lPitch = (WORD)m_ddsd.lPitch;

  m_bLock = true;
}
/*
//----------------------------------------------------------------------
// LockDW
//----------------------------------------------------------------------
void CDirectDrawSurface::LockDW(DWORD*& lpSurface, WORD& lPitch)
{
        HRESULT			hRet;
        DDSURFACEDESC2	ddsdDesc;

        ddsdDesc.dwSize = sizeof(DDSURFACEDESC);


        bool exitLoop = false;

        do
        {
                hRet = m_pDDSurface->Lock(NULL, &ddsdDesc, 0, NULL);
                if (hRet == DDERR_SURFACELOST)
                {
                        Restore();
                        hRet = m_pDDSurface->Lock(NULL, &ddsdDesc, 0, NULL);
                }

                // 이거는 없어야 되는 code일까??
                else if(hRet != DDERR_WASSTILLDRAWING) exitLoop = true;

        } while(!exitLoop);

        lpSurface	= (DWORD*)ddsdDesc.lpSurface;
        lPitch		= (WORD)ddsdDesc.lPitch;
}

//----------------------------------------------------------------------
// LockDW
//----------------------------------------------------------------------
void CDirectDrawSurface::LockQW(QWORD*& lpSurface, WORD& lPitch)
{
        HRESULT			hRet;
        DDSURFACEDESC2	ddsdDesc;

        ddsdDesc.dwSize = sizeof(DDSURFACEDESC);


        bool exitLoop = false;

        do
        {
                hRet = m_pDDSurface->Lock(NULL, &ddsdDesc, 0, NULL);
                if (hRet == DDERR_SURFACELOST)
                {
                        Restore();
                        hRet = m_pDDSurface->Lock(NULL, &ddsdDesc, 0, NULL);
                }

                // 이거는 없어야 되는 code일까??
                else if(hRet != DDERR_WASSTILLDRAWING) exitLoop = true;

        } while(!exitLoop);

        lpSurface	= (QWORD*)ddsdDesc.lpSurface;
        lPitch		= (WORD)ddsdDesc.lPitch;
}*/

//----------------------------------------------------------------------
// Unlock()
// Phase 2: IRenderSurface delegate. Unlock'tan sonra m_ddsd.lpSurface
// invalid hale gelir; bunu temizlemiyoruz cunku eski kod da temizlemiyordu
// (sprite codec Lock yapip Unlock yapip yeniden Lock yapiyor — m_ddsd
// sonraki Lock'ta dolduruluyor).
//----------------------------------------------------------------------
bool CDirectDrawSurface::Unlock() {
  if (m_pRenderSurface) {
    bool ok = m_pRenderSurface->Unlock();
    if (ok) {
      m_bLock = false;
      return true;
    }
    return false;
  }

  // Fallback eski yol.
  // D3D9 modunda compat path: m_pRenderSurface NULL + m_pDDSurface NULL —
  // Unlock no-op (Lock zaten fail dondurdu).
  if (m_pDDSurface == NULL) {
    m_bLock = false;
    return true;
  }

  HRESULT hr;
  bool ret;

  while (1) {
    hr = m_pDDSurface->Unlock(NULL);
    if (hr == DD_OK || hr == DDERR_NOTLOCKED) {
      break;
    }
    if (hr == DDERR_SURFACELOST) {
      ret = Restore();
      if (ret == false)
        break;
      else
        hr = DD_OK;
    }
  }

  if (hr == DD_OK) {
    m_bLock = false;
    return true;
  }

  return false;
}

/*-----------------------------------------------------------------------------
- GammaBox 555
- 0 ~ 32까지..인가?? - -;;
-----------------------------------------------------------------------------*/
void CDirectDrawSurface::GammaBox555(RECT *pRect, int p) {
  // pRect clipping
  if (pRect->left > m_ClipRight)
    return;
  if (pRect->top > m_ClipBottom)
    return;
  if (pRect->left < m_ClipLeft)
    pRect->left = m_ClipLeft;
  if (pRect->right > m_ClipRight)
    pRect->right = m_ClipRight;
  if (pRect->top < m_ClipTop)
    pRect->top = m_ClipTop;
  if (pRect->bottom > m_ClipBottom)
    pRect->bottom = m_ClipBottom;

  // 2026-09-01 Transparan fix — dx11gpu: GammaBox composition'daki BAYAT
  // pikselleri karartir (world/UI icerigi GPU quad'larinda, composition'da
  // degil) — sonuc yanlis segment basar. Dogru karsilik SIRALI siyah alpha
  // quad'i: out = dst * p/32  <=>  siyah, srcAlpha = (32-p)/32.
  // Kayit basarisizsa (aktif degil / overflow) CPU yoluna dusulur.
  {
    phoenix::render::D3D11SpriteCommands &cmds =
        phoenix::render::D3D11SpriteCommands::Instance();
    if (cmds.IsActive() && cmds.IsCompositionTarget(m_ddsd.lpSurface)) {
      int keep = p;
      if (keep < 0)  keep = 0;
      if (keep > 32) keep = 32;
      int a8 = ((32 - keep) * 255) / 32;
      if (cmds.RecordSolidQuad(*pRect,
              phoenix::render::D3D11QuadPipeline::MakeColor(0, 0, 0, (BYTE)a8),
              phoenix::render::D3D11QuadPipeline::BLEND_ALPHA))
        return;
    }
  }

  long pitch = m_ddsd.lPitch;
  WORD *lpDest = (WORD *)((BYTE *)m_ddsd.lpSurface + pRect->top * pitch +
                          (pRect->left << 1));

  int dLen = pRect->right - pRect->left;

  int i = pRect->bottom - pRect->top;

  if (i > 0) {
    do {
      Gamma4Pixel555(lpDest, dLen, p);
      lpDest = (WORD *)((BYTE *)lpDest + pitch);
    } while (--i);
  }

  // Faz G5 (2026-09-01) — dx11gpu: ham gamma kutusu dirty bildirimi (CPU
  // fallback yolu — quad kaydi basarisizsa)
  {
    phoenix::render::D3D11SpriteCommands &cmds =
        phoenix::render::D3D11SpriteCommands::Instance();
    if (cmds.IsActive() && cmds.IsCompositionTarget(m_ddsd.lpSurface))
      cmds.MarkDirty(pRect);
  }
}

/*-----------------------------------------------------------------------------
- GammaBox 565
- 0 ~ 32까지..인가?? - -;;
-----------------------------------------------------------------------------*/
void CDirectDrawSurface::GammaBox565(RECT *pRect, int p) {
  if (pRect->bottom < m_ClipTop || pRect->top > m_ClipBottom ||
      pRect->right < m_ClipLeft || pRect->left > m_ClipRight)
    return;

  if (pRect->left < m_ClipLeft)
    pRect->left = m_ClipLeft;
  if (pRect->right > m_ClipRight)
    pRect->right = m_ClipRight;
  if (pRect->top < m_ClipTop)
    pRect->top = m_ClipTop;
  if (pRect->bottom > m_ClipBottom)
    pRect->bottom = m_ClipBottom;

  // 2026-09-01 Transparan fix — dx11gpu: sirali siyah alpha quad (bkz.
  // GammaBox555 aciklamasi). out = dst * p/32.
  {
    phoenix::render::D3D11SpriteCommands &cmds =
        phoenix::render::D3D11SpriteCommands::Instance();
    if (cmds.IsActive() && cmds.IsCompositionTarget(m_ddsd.lpSurface)) {
      int keep = p;
      if (keep < 0)  keep = 0;
      if (keep > 32) keep = 32;
      int a8 = ((32 - keep) * 255) / 32;
      if (cmds.RecordSolidQuad(*pRect,
              phoenix::render::D3D11QuadPipeline::MakeColor(0, 0, 0, (BYTE)a8),
              phoenix::render::D3D11QuadPipeline::BLEND_ALPHA))
        return;
    }
  }

  long pitch = m_ddsd.lPitch;
  WORD *lpDest = (WORD *)((BYTE *)m_ddsd.lpSurface + pRect->top * pitch +
                          (pRect->left << 1));

  int dLen = pRect->right - pRect->left;

  int i = pRect->bottom - pRect->top;

  if (i > 0) {
    do {
      Gamma4Pixel565(lpDest, dLen, p);
      lpDest = (WORD *)((BYTE *)lpDest + pitch);
    } while (--i);
  }

  // Faz G5 (2026-09-01) — dx11gpu: ham gamma kutusu dirty bildirimi (CPU
  // fallback yolu — quad kaydi basarisizsa)
  {
    phoenix::render::D3D11SpriteCommands &cmds =
        phoenix::render::D3D11SpriteCommands::Instance();
    if (cmds.IsActive() && cmds.IsCompositionTarget(m_ddsd.lpSurface))
      cmds.MarkDirty(pRect);
  }
}

/*-----------------------------------------------------------------------------
- Gamma4Pixel
-
-----------------------------------------------------------------------------*/
void CDirectDrawSurface::Gamma4Pixel565(void *pDest, int len, int p) {
  __asm
  {
		mov edi, pDest
		mov ecx, len
		shr ecx, 1
		jnc GAMMA_2PIXEL
;;one pixel
		mov edx, p
		mov eax, [edi]

		push eax
		push edx

		and eax, GAMMA_MASK565_1PIXEL1
		mul edx
		shr eax, 5
		and eax, GAMMA_MASK565_1PIXEL1

		mov ebx, eax
		pop edx
		pop eax

		and eax, GAMMA_MASK565_1PIXEL2
		mul edx
		shr eax, 5
		and eax, GAMMA_MASK565_1PIXEL2

		or eax, ebx
		mov [edi], ax
		add edi, 2

GAMMA_2PIXEL:
		shr ecx, 1
		jnc GAMMA_4PIXEL

;;two pixel
		mov edx, p
		mov eax, [edi]

		push eax
		push edx

		and eax, GAMMA_MASK565_2PIXEL1
		shr eax, 5
		mul edx
		and eax, GAMMA_MASK565_2PIXEL1

		mov ebx, eax
		pop edx
		pop eax

		and eax, GAMMA_MASK565_2PIXEL2
		mul edx
		shr eax, 5
		and eax, GAMMA_MASK565_2PIXEL2

		or eax, ebx
		mov [edi], eax
		add edi, 4

GAMMA_4PIXEL:
		test ecx, ecx
		jz GAMMA_END

		movd mm0, p
		punpcklwd mm0, mm0
		punpckldq mm0, mm0

		mov eax, GAMMA_MASK565_4PIXEL_R
		mov ebx, GAMMA_MASK565_4PIXEL_G
		mov edx, GAMMA_MASK565_4PIXEL_B

		movd mm1, eax
		movd mm2, ebx
		movd mm3, edx

		punpckldq mm1, mm1
		punpckldq mm2, mm2
		punpckldq mm3, mm3

GAMMA_4PIXEL_LOOP:
		movq mm4, [edi]
		movq mm5, mm4
		movq mm6, mm4

		pand mm4, mm1 ;; red
		pand mm5, mm2 ;; green

		pand mm6, mm3 ;; blue
		psrlw mm4, 5

		pmullw mm4, mm0
		psrlw mm5, 5

		pand mm4, mm1
		pmullw mm5, mm0

		pand mm5, mm2
		pmullw mm6, mm0

		psrlw mm6, 5
		pand mm6, mm3

		por mm4, mm5
		por mm4, mm6

		movq [edi], mm4

		add edi, 8
		dec ecx
		jnz GAMMA_4PIXEL_LOOP

GAMMA_END:
		emms
  }
}

/*-----------------------------------------------------------------------------
- Gamma4Pixel
-
-----------------------------------------------------------------------------*/
void CDirectDrawSurface::Gamma4Pixel555(void *pDest, int len, int p) {
  __asm
  {
		mov edi, pDest
		mov ecx, len
		shr ecx, 1
		jnc GAMMA_2PIXEL
;;one pixel
		mov edx, p
		mov eax, [edi]

		push eax
		push edx

		and eax, GAMMA_MASK555_1PIXEL1
		mul edx
		shr eax, 5
		and eax, GAMMA_MASK555_1PIXEL1

		mov ebx, eax
		pop edx
		pop eax

		and eax, GAMMA_MASK555_1PIXEL2
		mul edx
		shr eax, 5
		and eax, GAMMA_MASK555_1PIXEL2

		or eax, ebx
		mov [edi], ax
		add edi, 2

GAMMA_2PIXEL:
		shr ecx, 1
		jnc GAMMA_4PIXEL

;;two pixel
		mov edx, p
		mov eax, [edi]

		push eax
		push edx

		and eax, GAMMA_MASK555_2PIXEL1
		shr eax, 5
		mul edx
		and eax, GAMMA_MASK555_2PIXEL1

		mov ebx, eax
		pop edx
		pop eax

		and eax, GAMMA_MASK555_2PIXEL2
		mul edx
		shr eax, 5
		and eax, GAMMA_MASK555_2PIXEL2

		or eax, ebx
		mov [edi], eax
		add edi, 4

GAMMA_4PIXEL:
		test ecx, ecx
		jz GAMMA_END

		movd mm0, p
		punpcklwd mm0, mm0
		punpckldq mm0, mm0

		mov eax, GAMMA_MASK555_4PIXEL_R
		mov ebx, GAMMA_MASK555_4PIXEL_G
		mov edx, GAMMA_MASK555_4PIXEL_B

		movd mm1, eax
		movd mm2, ebx
		movd mm3, edx

		punpckldq mm1, mm1
		punpckldq mm2, mm2
		punpckldq mm3, mm3

GAMMA_4PIXEL_LOOP:
		movq mm4, [edi]
		movq mm5, mm4
		movq mm6, mm4

		pand mm4, mm1 ;; red
		pand mm5, mm2 ;; green

		pand mm6, mm3 ;; blue
		psrlw mm4, 5

		pmullw mm4, mm0
		psrlw mm5, 5

		pand mm4, mm1
		pmullw mm5, mm0

		pand mm5, mm2
		pmullw mm6, mm0

		psrlw mm6, 5
		pand mm6, mm3

		por mm4, mm5
		por mm4, mm6

		movq [edi], mm4

		add edi, 8
		dec ecx
		jnz GAMMA_4PIXEL_LOOP

GAMMA_END:
		emms
  }
}

//----------------------------------------------------------------------
// Copy : pDDsourceSurface의 rect 영역을
//        pDDTargetSurface의 point 위치로 Copy한다.
//
// `SetTransparency()로 투명색을 지정한다.
// `그냥 blt하려면 BltNoColorkey()를 사용한다.
//----------------------------------------------------------------------
void CDirectDrawSurface::Blt(POINT *pPoint, CDirectDrawSurface *SourceSurface,
                             RECT *pRect) {
  // Phase 2: IRenderSurface delegate. Clip mantigi DDraw7Surface::BltColorKey
  // icindekiyle ozdes; CDirectDrawSurface clip state'i delegate'e syncle.
  if (m_pRenderSurface && SourceSurface && SourceSurface->m_pRenderSurface) {
    m_pRenderSurface->SetClip(MakeClipRect(m_ClipLeft, m_ClipTop,
                                            m_ClipRight, m_ClipBottom));
    m_pRenderSurface->BltColorKey(pPoint, SourceSurface->m_pRenderSurface, pRect);
    return;
  }

  // Fallback eski raw DDraw7 yolu (delegate yoksa — init oncesi vs.)
  if (pPoint->x >= (int)m_ClipRight || pPoint->y >= (int)m_ClipBottom)
    return;

  // width clipping
  if (pPoint->x < m_ClipLeft) {
    pRect->left = pRect->left - pPoint->x + m_ClipLeft;
    pPoint->x = m_ClipLeft;
  } else if ((int)m_ClipRight - pPoint->x < pRect->right - pRect->left) {
    pRect->right = pRect->left + m_ClipRight - pPoint->x;
  }

  // height clipping
  if (pPoint->y < m_ClipTop) {
    pRect->top = pRect->top - pPoint->y + m_ClipTop;
    pPoint->y = m_ClipTop;
  } else if ((int)m_ClipBottom - pPoint->y < pRect->bottom - pRect->top) {
    pRect->bottom = pRect->top + m_ClipBottom - pPoint->y;
  }

  // Blit the current frame to the destination surface
  HRESULT hRet;

  while (1) {
    hRet =
        m_pDDSurface->BltFast(pPoint->x, pPoint->y, SourceSurface->m_pDDSurface,
                              pRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
    if (hRet == DD_OK)
      break;

    if (hRet == DDERR_SURFACELOST) {
      if (!Restore() || !SourceSurface->Restore())
        break;
    } else if (hRet != DDERR_WASSTILLDRAWING)
      break;
  }
}

//----------------------------------------------------------------------
// Copy : pDDsourceSurface의 rect 영역을
//        pDDTargetSurface의 point 위치로 Copy한다.
//----------------------------------------------------------------------
void CDirectDrawSurface::BltNoColorkey(POINT *pPoint,
                                       CDirectDrawSurface *SourceSurface,
                                       RECT *pRect) {
  // 2026-05-15: D3D9 zone enter crash #2 dual-layer fix (Layer 2 — method body).
  // Source surface veya dest/source raw surface pointer NULL ise BltFast/Blt
  // icinde memcpy NULL deref AV. CFilter::Blt4444 fix pattern parity. D3D9 native
  // port surface lifecycle (Restore yorum + IsHAL dinamigi + Init timing race)
  // edge case'lerini call-site disinda da koru.
  if (SourceSurface == NULL)
    return;
  if (pPoint == NULL || pRect == NULL)
    return;

  // Phase 2: IRenderSurface delegate. DDraw7Surface::BltNoColorKey icinde
  // clip + BltFast(NOCOLORKEY) mantigi ozdes.
  if (m_pRenderSurface && SourceSurface->m_pRenderSurface) {
    // 2026-05-15 round 2: D3D9 BltNoColorKey memcpy AV (vcruntime fault).
    // Round 1 NULL guard yetmedi — crash D3D9 path delegate sonrasi
    // IRenderSurface impl icindeki memcpy/memmove'da. Kok sebep: D3D9Surface
    // ::BltNoColorKey body source rect'i sadece DEST clip rect'e gore
    // kirpiyor, SOURCE surface boyutlarina karsi bounds-check YOK. rectReuse
    // (MTopView.cpp:14704) `rectScreen.left - rectTileSurface.left` formulu
    // zone enter ilk frame'inde m_FirstZonePixel / m_TileSurfaceFirstZonePixel
    // sync olmadan negative veya TILESURFACE_WIDTH/HEIGHT (1296x912) disi
    // deger uretebilir → src pointer OOB → memcpy AV.
    //
    // Layer 3 — call-side RECT bounds + clip bounds sanity. Width/height
    // <=0 ise blit anlamsiz; clip rect bozuksa (m_ClipRight<=m_ClipLeft)
    // SetClip 0-genislik clip kurar ve sonraki blitlerin tamamini OOB
    // hesaplamasina sokar.
    if (pRect->right <= pRect->left || pRect->bottom <= pRect->top)
      return;
    if ((int)m_ClipRight <= (int)m_ClipLeft || (int)m_ClipBottom <= (int)m_ClipTop)
      return;

    // 2026-08-31 PERF FIX (FPS analiz 1.2 — bosa self-copy): D3D9 yolunda
    // ekran boyutlu surface'lar (g_pBack, g_pLast) InitOffsurface Yol 1 ile
    // AYNI composition singleton'ina baglanir. CGameUpdate::UpdateDraw sonu
    // "Last->Back" tam ekran kopyasi bu yuzden ayni bellek uzerinde ayni
    // koordinatlara memcpy idi (~2.34MB/kare, sonucu degistirmeyen is).
    // Ayni alt-surface + ayni offset = matematiksel no-op → atla. Farkli
    // offsetli (gercek tasima) kopyalar aynen calisir; DDraw7 fallback
    // yolu etkilenmez.
    if (m_pRenderSurface == SourceSurface->m_pRenderSurface &&
        pPoint->x == pRect->left && pPoint->y == pRect->top)
      return;

    m_pRenderSurface->SetClip(MakeClipRect(m_ClipLeft, m_ClipTop,
                                            m_ClipRight, m_ClipBottom));
    m_pRenderSurface->BltNoColorKey(pPoint, SourceSurface->m_pRenderSurface, pRect);
    return;
  }

  // 2026-05-15: Fallback DDraw7 yolu raw surface pointer NULL guard
  // (D3D9 mode'da m_pRenderSurface set olmali, ama defansif — Phase 4
  // gecisi sirasinda Init eksik kalan path icin).
  if (m_pDDSurface == NULL || SourceSurface->m_pDDSurface == NULL)
    return;

  // Fallback eski raw DDraw7 yolu
  if (pPoint->x >= (int)m_ClipRight || pPoint->y >= (int)m_ClipBottom)
    return;

  // width clipping
  if (pPoint->x < m_ClipLeft) {
    pRect->left += -pPoint->x + m_ClipLeft;
    pPoint->x = m_ClipLeft;
  }
  if ((int)m_ClipRight - pPoint->x < pRect->right - pRect->left) {
    pRect->right = pRect->left + m_ClipRight - pPoint->x;
  }

  // height clipping
  if (pPoint->y < m_ClipTop) {
    pRect->top += -pPoint->y + m_ClipTop;
    pPoint->y = m_ClipTop;
  }
  if ((int)m_ClipBottom - pPoint->y < pRect->bottom - pRect->top) {
    pRect->bottom = pRect->top + m_ClipBottom - pPoint->y;
  }

  // Blit the current frame to the destination surface
  HRESULT hRet;

  while (1) {
    hRet =
        m_pDDSurface->BltFast(pPoint->x, pPoint->y, SourceSurface->m_pDDSurface,
                              pRect, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
    if (hRet == DD_OK)
      break;

    if (hRet == DDERR_SURFACELOST) {
      if (!Restore() || !SourceSurface->Restore())
        break;
    } else if (hRet != DDERR_WASSTILLDRAWING)
      break;
  }
}

//----------------------------------------------------------------------
// Copy : pDDsourceSurface의 rect 영역을
//        pDDTargetSurface의 point 위치로 Copy한다.
//
// `SetTransparency()로 투명색을 지정한다.
// `그냥 blt하려면 BltNoColorkey()를 사용한다.
// 임시로(-_-;) 투명색 무시.. ㅋㅋ - -;
//----------------------------------------------------------------------
void CDirectDrawSurface::Blt(RECT *pDestRect, CDirectDrawSurface *SourceSurface,
                             RECT *pSourceRect) {
  // Phase 2: IRenderSurface delegate. DDraw7Surface::BltStretchKey
  // DDBLT_KEYSRC + DDBLTFX_ARITHSTRETCHY kombinasyonunu sarmalar.
  //
  // Phase 3 (2026-05-14): BltStretchKey iki backend tarafindan da implement
  // ediliyor. DDraw7 raw DDBLT_KEYSRC kullanir (mevcut davranis, regression 0).
  // D3D9 ise SetTransparency ile saklanan colorkey'i okur (GetColorKey)
  // ve manuel pixel filtre + nearest-neighbor stretch yapar. Phase 4 switch
  // sonrasi composition pipeline'da D3D9 path aktif olunca dgVoodoo'nun
  // 32-bit promotion'da bozdugu magenta sprite hatasi kok-sebeben duzelir.
  if (m_pRenderSurface && SourceSurface && SourceSurface->m_pRenderSurface) {
    m_pRenderSurface->BltStretchKey(pDestRect, SourceSurface->m_pRenderSurface,
                                    pSourceRect);
    return;
  }

  // Fallback eski raw DDraw7 yolu
  HRESULT hRet;

  DDBLTFX ddbltfx;

  ddbltfx.dwSize = sizeof(ddbltfx);
  ddbltfx.dwDDFX = DDBLTFX_ARITHSTRETCHY | DDBLT_DDFX;

  while (1) {
    hRet = m_pDDSurface->Blt(pDestRect, SourceSurface->m_pDDSurface,
                             pSourceRect, DDBLT_WAIT | DDBLT_KEYSRC, &ddbltfx);

    if (hRet == DD_OK)
      break;

    if (hRet == DDERR_SURFACELOST) {
      if (!Restore() || !SourceSurface->Restore())
        break;
    } else if (hRet != DDERR_WASSTILLDRAWING)
      break;
  }
}

//----------------------------------------------------------------------
// 현재 surface의 pPoint에 PrimarySurface의 pRect를 blt한다.
//----------------------------------------------------------------------
void CDirectDrawSurface::BltPrimarySurface(POINT *pPoint, RECT *pRect) {
  // Phase 2: IRenderSurface delegate. Primary surface'i IRenderBackend
  // uzerinden alip BltNoColorKey ile blit yapiyoruz. Windowed mode'da
  // m_rcScreen offset Phoenix-spesifik — primary surface absolut konum
  // ister, bunu manuel uyguluyoruz (delegate icindeki clip mantigi
  // primary'nin tum bounds'i.
  IRenderBackend* pBackend = GetRenderBackend();
  IRenderSurface* pPrimary = pBackend ? pBackend->GetPrimarySurface() : NULL;

  if (m_pRenderSurface && pPrimary) {
    // Windowed mode offset (DDraw7 yolundakiyle ozdes)
    if (!m_bFullscreen) {
      pPoint->x += m_rcScreen.left;
      pPoint->y += m_rcScreen.top;
      pRect->left   += m_rcScreen.left;
      pRect->right  += m_rcScreen.left;
      pRect->top    += m_rcScreen.top;
      pRect->bottom += m_rcScreen.top;
    }

    m_pRenderSurface->SetClip(MakeClipRect(m_ClipLeft, m_ClipTop,
                                            m_ClipRight, m_ClipBottom));
    m_pRenderSurface->BltNoColorKey(pPoint, pPrimary, pRect);
    return;
  }

  // Fallback eski raw DDraw7 yolu (delegate veya primary erisilemiyorsa)
  if (pPoint->x >= (int)m_ClipRight || pPoint->y >= (int)m_ClipBottom)
    return;

  // width clipping
  if (pPoint->x < m_ClipLeft) {
    pRect->left = pRect->left - pPoint->x + m_ClipLeft;
    pPoint->x = m_ClipLeft;
  } else if ((int)m_ClipRight - pPoint->x < pRect->right - pRect->left) {
    pRect->right = pRect->left + m_ClipRight - pPoint->x;
  }

  // height clipping
  if (pPoint->y < m_ClipTop) {
    pRect->top = pRect->top - pPoint->y + m_ClipTop;
    pPoint->y = m_ClipTop;
  } else if ((int)m_ClipBottom - pPoint->y < pRect->bottom - pRect->top) {
    pRect->bottom = pRect->top + m_ClipBottom - pPoint->y;
  }

  // Blit the current frame to the destination surface
  HRESULT hRet;

  // 창모드일 경우에..
  if (!m_bFullscreen) {
    pPoint->x += m_rcScreen.left;
    pPoint->y += m_rcScreen.top;

    pRect->left += m_rcScreen.left;
    pRect->right += m_rcScreen.left;
    pRect->top += m_rcScreen.top;
    pRect->bottom += m_rcScreen.top;
  }

  while (1) {
    hRet = m_pDDSurface->BltFast(pPoint->x, pPoint->y, m_pDDSPrimary, pRect,
                                 DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
    if (hRet == DD_OK) {
      break;
    }

    if (hRet == DDERR_SURFACELOST) {
      if (!Restore() || !m_pDDSPrimary->Restore()) {
        break;
      }
    } else if (hRet != DDERR_WASSTILLDRAWING) {
      break;
    }
  }
}

//----------------------------------------------------------------------
// ShowFPS
//
//----------------------------------------------------------------------
void CDirectDrawSurface::ShowFPS(int x, int y, COLORREF fcolor,
                                 COLORREF bcolor) {
  static DWORD thisTickTime, oldTickTime;
  static DWORD frame, fps;
  static char framebuf[20];

  frame++;
  thisTickTime = GetTickCount();
  if (abs((int)(thisTickTime - oldTickTime)) >= 1000) {
    fps = frame;
    frame = 0;
    oldTickTime = GetTickCount();
  }

  sprintf(framebuf, "FPS = %3d", fps);
  GDI_Text(x, y, framebuf, fcolor, bcolor, false);
}

//----------------------------------------------------------------------
// GDI_Text
// option은 배경 투명 여부이다.
//----------------------------------------------------------------------
void CDirectDrawSurface::GDI_Text(int x, int y, const char *str,
                                  COLORREF fcolor, COLORREF bcolor,
                                  bool option) {
  HDC hdc = NULL;

  if (!str)
    return;

#if PHOENIX_RENDER_BACKEND_D3D9
  if (m_pDDSurface == NULL && m_pRenderSurface != NULL)
  {
    if (Phoenix_DrawGdiTextViaDib(this, x, y, str, fcolor, bcolor, option))
      return;
    // DIB path basarisizsa eski D3D9 HDC wrapper fallback'i korunur.
  }
#endif

  // Fix #17 (2026-05-14): D3D9 mode HDC tedariki. m_pDDSurface NULL ise
  // D3D9 composition buffer'in IDirect3DSurface9::GetDC()'sini al — text
  // gercek composition buffer'a yazilir, Present pipeline ekrana cikar.
  // m_pDDSurface dolu ise legacy DDraw7 yolu (DDraw7 modunda).
  bool fromD3D9 = false;
  if (m_pDDSurface != NULL) {
    m_pDDSurface->GetDC(&hdc);
  } else {
    hdc = Phoenix_AcquireD3D9DC();
    fromD3D9 = (hdc != NULL);
  }

  if (hdc == NULL)
    return;  // Hicbir backend HDC saglayamadi — sessiz skip.

  SetTextColor(hdc, fcolor);

  if (option)
    SetBkMode(hdc, TRANSPARENT);
  else
    SetBkColor(hdc, bcolor);

  // Faz T3: DWrite dene, olmazsa GDI TextOut
  Phoenix_TextOutDW(hdc, x, y, str, strlen(str));

  if (fromD3D9)
    Phoenix_ReleaseD3D9DC(hdc);
  else
    m_pDDSurface->ReleaseDC(hdc);
}

//----------------------------------------------------------------------
// Save BMP
//----------------------------------------------------------------------
//
// NOTE: The FrontBuffer parameter is needed only for 8-bit surfaces so that the
// system
//       palette can be accessed, for  15-,16-,24-bit surfaces it can be NULL
//
//----------------------------------------------------------------------
bool CDirectDrawSurface::SaveToBMP(const char *szFilename) {
  HANDLE file_out;

  //------------------------------------------------------------
  // Create the outputfile...
  //------------------------------------------------------------
  file_out = CreateFile(szFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
                        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

  if (file_out == INVALID_HANDLE_VALUE) {
    // DirectDrawFailed("Surface - Could not open output file for Bitmap:
    // Screenshot.bmp!");
    return false;
  }

  //------------------------------------------------------------
  // Save To File
  //------------------------------------------------------------
  HRESULT rval;
  DWORD numwrite;
  BITMAPFILEHEADER fh;
  BITMAPINFOHEADER bi;
  DWORD outpixel;
  int outbyte, loop, loop2, BufferIndex;
  BYTE *WriteBuffer;
  DDSURFACEDESC2 ddsd;
  int Width, Height, Pitch, Gap;

  //------------------------------------------------------------
  // First we need a ddsdription of the surface
  //------------------------------------------------------------
  ZeroMemory(&ddsd, sizeof(ddsd));
  ddsd.dwSize = sizeof(ddsd);

  rval = m_pDDSurface->GetSurfaceDesc(&ddsd);
  if (rval != DD_OK) {
    // DirectDrawFailed("Couldn't get surface ddsd for bitmap save");
    return false;
  }

  //------------------------------------------------------------
  // Setup output buffer stuff,
  // since Windows has paging and we're in flat mode,
  // I just made it as big as the bitmap
  //------------------------------------------------------------
  BufferIndex = 0;
  Gap = ddsd.dwWidth % 2;
  Width = ddsd.dwWidth + Gap;
  Height = ddsd.dwHeight;
  Pitch = ddsd.lPitch;
  WriteBuffer = new BYTE[Width * Height * 3]; // width*height*24-bit

  //------------------------------------------------------------
  // Write the file header
  //------------------------------------------------------------
  ((char *)&(fh.bfType))[0] = 'B';
  ((char *)&(fh.bfType))[1] = 'M';
  fh.bfSize = (long)(sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) +
                     Width * Height * 3); // Size in BYTES
  fh.bfReserved1 = 0;
  fh.bfReserved2 = 0;
  fh.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
  bi.biSize = sizeof(BITMAPINFOHEADER);
  bi.biWidth = Width - Gap;
  bi.biHeight = Height;
  bi.biPlanes = 1;
  bi.biBitCount = 24;
  bi.biCompression = BI_RGB;
  bi.biSizeImage = 0;
  bi.biXPelsPerMeter = 10000;
  bi.biYPelsPerMeter = 10000;
  bi.biClrUsed = 0;
  bi.biClrImportant = 0;

  WriteFile(file_out, (char *)&fh, sizeof(BITMAPFILEHEADER), &numwrite, NULL);
  WriteFile(file_out, (char *)&bi, sizeof(BITMAPINFOHEADER), &numwrite, NULL);

  //------------------------------------------------------------
  // 32 Bit
  //------------------------------------------------------------
  if (ddsd.ddpfPixelFormat.dwRGBBitCount == 32) {
    // lock the surface and start filling the output
    // buffer
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    rval = m_pDDSurface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
    if (rval != DD_OK) {
      // DirectDrawFailed("Couldn't lock source");
      delete[] WriteBuffer;
      return false;
    }

    BYTE *Bitmap_in = (BYTE *)ddsd.lpSurface;

    for (loop = Height - 1; loop >= 0; loop--) // Loop bottom up
      for (loop2 = 0; loop2 < Width; loop2++) {
        outpixel =
            *((DWORD *)(Bitmap_in + loop2 * 4 + loop * Pitch)); // Load a word

        // Load up the Blue component and output it
        outbyte = (((outpixel) & 0x000000ff)); // blue
        WriteBuffer[BufferIndex++] = outbyte;

        // Load up the green component and output it
        outbyte = (((outpixel >> 8) & 0x000000ff));
        WriteBuffer[BufferIndex++] = outbyte;

        // Load up the red component and output it
        outbyte = (((outpixel >> 16) & 0x000000ff));
        WriteBuffer[BufferIndex++] = outbyte;
      }

    // At this point the buffer should be full, so just write it out
    WriteFile(file_out, WriteBuffer, BufferIndex, &numwrite, NULL);

    // Now unlock the surface and we're done
    m_pDDSurface->Unlock(NULL);
  }
  //------------------------------------------------------------
  // 24 bit
  //------------------------------------------------------------
  if (ddsd.ddpfPixelFormat.dwRGBBitCount == 24) {
    // So easy just lock the surface and output

    // lock the surface and start filling the output
    // buffer

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    rval = m_pDDSurface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
    if (rval != DD_OK) {
      // DirectDrawFailed("Couldn't lock source");
      delete[] WriteBuffer;
      return false;
    }

    BYTE *Bitmap_in = (BYTE *)ddsd.lpSurface;

    for (loop = Height - 1; loop >= 0; loop--) // Loop bottom up
      for (loop2 = 0; loop2 < Width; loop2++) {

        // Load up the Blue component and output it
        WriteBuffer[BufferIndex++] =
            *(Bitmap_in + loop2 * 3 + 2 + loop * Pitch); // Bug fix 6-5

        // Load up the green component and output it
        WriteBuffer[BufferIndex++] =
            *(Bitmap_in + loop2 * 3 + 1 + loop * Pitch); // Bug fix 6-5

        // Load up the red component and output it
        WriteBuffer[BufferIndex++] = *(Bitmap_in + loop2 * 3 + loop * Pitch);
      }

    // At this point the buffer should be full, so just write it out
    WriteFile(file_out, WriteBuffer, BufferIndex, &numwrite, NULL);

    // Now unlock the surface and we're done
    m_pDDSurface->Unlock(NULL);
  }
  //------------------------------------------------------------
  // 16 Bit
  //------------------------------------------------------------
  else if (ddsd.ddpfPixelFormat.dwRGBBitCount == 16) {
    // lock the surface and start filling the output
    // buffer
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    rval = m_pDDSurface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
    if (rval != DD_OK) {
      // DirectDrawFailed("Couldn't lock source");
      delete[] WriteBuffer;
      return false;
    }

    BYTE *Bitmap_in = (BYTE *)ddsd.lpSurface;

    /*
    According to DirectX docs, dwRGBBitCount is 2,4,8,16,24,32, BUT what about
    15-bit surfaces (5,5,5) I don't really know if its needed but here we check
    the green bitmask to see if 5 or 6 bits are used for green.

    If the green bitmask equals 565 mode, do 16-bit mode, otherwise do 15-bit
    mode NOTE: We are reversing the component order (ie. BGR instead of RGB) and
    we are outputting it bottom up because BMP files are backwards and upside
    down.
    */

    //------------
    // 5:6:5
    //------------
    if (ddsd.ddpfPixelFormat.dwGBitMask == 0x07E0) {
      for (loop = Height - 1; loop >= 0; loop--) // Loop bottom up
      {
        for (loop2 = 0; loop2 < Width; loop2++) {

          if (loop2 >= Width - Gap) {
            WriteBuffer[BufferIndex++] = NULL;
            WriteBuffer[BufferIndex++] = NULL;
            WriteBuffer[BufferIndex++] = NULL;
          } else {

            outpixel = *((WORD *)(Bitmap_in + (loop2 << 1) +
                                  loop * Pitch)); // Load a word

            // Load up the Blue component and output it
            outbyte = (((outpixel) & 0x001f) << 3); // blue
            WriteBuffer[BufferIndex++] = outbyte;

            // Load up the green component and output it
            outbyte = (((outpixel >> 5) & 0x003f) << 2);
            WriteBuffer[BufferIndex++] = outbyte;

            // Load up the red component and output it
            outbyte = (((outpixel >> 11) & 0x001f) << 3);
            WriteBuffer[BufferIndex++] = outbyte;
          }
        }
      }
    }
    //----------
    // 5:5:5
    //----------
    else // Assume 555 mode. 15-bit mode
    {
      for (loop = Height - 1; loop >= 0; loop--) // Loop bottom up
      {
        for (loop2 = 0; loop2 < Width; loop2++) {
          if (loop2 >= Width - Gap) {
            WriteBuffer[BufferIndex++] = NULL;
            WriteBuffer[BufferIndex++] = NULL;
            WriteBuffer[BufferIndex++] = NULL;
          } else {

            outpixel = *(
                (WORD *)(Bitmap_in + loop2 * 2 + loop * Pitch)); // Load a word

            // Load up the Blue component and output it
            outbyte = (8 * ((outpixel) & 0x001f)); // blue
            WriteBuffer[BufferIndex++] = outbyte;

            // Load up the green component and output it
            outbyte = (8 * ((outpixel >> 5) & 0x001f));
            WriteBuffer[BufferIndex++] = outbyte;

            // Load up the red component and output it
            outbyte = (8 * ((outpixel >> 10) & 0x001f)); // BUG FIX here
            WriteBuffer[BufferIndex++] = outbyte;
          }
        }
      }
    }

    // At this point the buffer should be full, so just write it out
    WriteFile(file_out, WriteBuffer, BufferIndex, &numwrite, NULL);

    // Now unlock the surface and we're done
    m_pDDSurface->Unlock(NULL);

  }
  //------------------------------------------------------------
  // 8 Bit
  //------------------------------------------------------------
  else if (ddsd.ddpfPixelFormat.dwRGBBitCount == 8) // 8 bit surfaces
  {
    // Get the system palette so we can index each pixel to its corresponding
    // color, this is what the m_pDDSPrimary parameter is needed for

    if (m_pDDSPrimary == NULL) {
      // DirectDrawFailed("No Front Buffer for 8-bit BMP save");
      delete[] WriteBuffer;
      return false;
    }

    LPDIRECTDRAWPALETTE Pal;
    char bytepal[256 * 4];

    rval = m_pDDSPrimary->GetPalette(&Pal);
    if (rval != DD_OK) {
      // DirectDrawFailed("Surface - Couldn't get palette for 8-bit Bitmap
      // Save");
      delete[] WriteBuffer;
      return false;
    }

    Pal->GetEntries(0, 0, 256, (tagPALETTEENTRY *)&(bytepal[0]));
    Pal->Release();

    // lock the surface and start filling the output
    // buffer

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    rval = m_pDDSurface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
    if (rval != DD_OK) {
      // DirectDrawFailed("Couldn't lock source");
      delete[] WriteBuffer;
      return false;
    }

    BYTE *Bitmap_in = (BYTE *)ddsd.lpSurface;

    // Ok, now that we've got the palette and the 24-bit entries, we just look
    // up the color and output it NOTE: At the same time we are reversing the
    // component order (ie. BGR instead of RGB)
    //       and we are outputting it bottom up.
    for (loop = Height - 1; loop >= 0; loop--) // Loop bottom up
    {
      for (loop2 = 0; loop2 < Width; loop2++) {
        outpixel =
            *(Bitmap_in + loop2 + loop * Pitch); // Load a byte from the surface

        // Load up the Blue component and output it
        outbyte = bytepal[outpixel * 4 + 2]; // blue
        WriteBuffer[BufferIndex++] = outbyte;

        // Load up the Green component and output it
        outbyte = bytepal[outpixel * 4 + 1]; // green
        WriteBuffer[BufferIndex++] = outbyte;

        // Load up the Red component and output it
        outbyte = bytepal[outpixel * 4]; // red
        WriteBuffer[BufferIndex++] = outbyte;
      }
    }

    // At this point the buffer should be full, so just write it out
    WriteFile(file_out, WriteBuffer, BufferIndex, &numwrite, NULL);

    // Now unlock the surface and we're done
    m_pDDSurface->Unlock(NULL);
  }

  delete[] WriteBuffer;

  CloseHandle(file_out);

  return true;
}
