//-----------------------------------------------------------------------------
// RenderTypes.h
//
// Phoenix D3D9 native render layer.
//
// Render backend ve surface arabirimlerinin ortak veri tipleri. Bu header
// sadece POD struct + enum'lar tasir; aktif implementasyon D3D9Backend'dir.
//
// 2026-05-14 Phase 0
//-----------------------------------------------------------------------------

#ifndef __PHOENIX_RENDER_TYPES_H__
#define __PHOENIX_RENDER_TYPES_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>  // RECT, POINT, DWORD, WORD, BYTE

//-----------------------------------------------------------------------------
// PHOENIX_RENDER_BACKEND_D3D9
//
// 2026-06-12 itibariyla render backend tek yoldur: D3D9Backend.
// CMake bu makroyu global 1 olarak set eder. Header tek basina derlenirse de
// guvenli default 1'dir; DDraw7/dgVoodoo rollback yolu aktif degildir.
//-----------------------------------------------------------------------------
#ifndef PHOENIX_RENDER_BACKEND_D3D9
#define PHOENIX_RENDER_BACKEND_D3D9 1
#endif

namespace phoenix
{
namespace render
{

//-----------------------------------------------------------------------------
// SurfaceFormat
//
// Backend-bagimsiz pixel format enum'u. DDraw7 DDPIXELFORMAT bitmask karmasasi
// + D3D9 D3DFMT_* karmasasi yerine sade liste. Phoenix'in kullandigi formatlar:
//
//   - R5G6B5     : 16-bit yuksek yesil bit (en yaygin sprite composition format)
//   - X1R5G5B5   : 16-bit display alt klas
//   - A1R5G5B5   : 16-bit alpha (sprite colorkey muadili)
//   - A4R4G4B4   : 16-bit alpha 4-bit
//   - X8R8G8B8   : 32-bit display (primary, modern wrapper'lar zorluyor)
//   - A8R8G8B8   : 32-bit alpha
//-----------------------------------------------------------------------------
enum class SurfaceFormat
{
    Unknown      = 0,
    R5G6B5       = 1,    // 16-bit 5:6:5 (Phoenix sprite codec default)
    X1R5G5B5     = 2,    // 16-bit 5:5:5 (fallback)
    A1R5G5B5     = 3,    // 16-bit alpha 1-bit
    A4R4G4B4     = 4,    // 16-bit alpha 4-bit
    X8R8G8B8     = 5,    // 32-bit no alpha
    A8R8G8B8     = 6     // 32-bit with alpha
};

//-----------------------------------------------------------------------------
// LockedRect
//
// DDraw7 m_ddsd.lpSurface + lPitch + Width/Height kontratinin abstraction'i.
// Phoenix sprite codec (CSpriteSurface.cpp 50+ memcpy) bu kontrata bagli:
//
//   - pBits   : surface memory baslangici (DDraw7'de DDSURFACEDESC2.lpSurface)
//   - pitch   : satir genisligi BYTE cinsinden (DDraw7'de DDSURFACEDESC2.lPitch)
//   - width   : surface piksel genisligi
//   - height  : surface piksel yuksekligi
//   - format  : pixel format
//
// !! ONEMLI: pitch byte cinsinden — sprite codec WORD* cast yapip pitch'i 1
// satir gece icin BYTE adres aritmetigi kullaniyor. DDraw7'de zaten bu davranis,
// D3D9'da da `D3DLOCKED_RECT::Pitch` ayni semantik.
//-----------------------------------------------------------------------------
struct LockedRect
{
    void*         pBits;
    int           pitch;
    int           width;
    int           height;
    SurfaceFormat format;

    LockedRect()
        : pBits(NULL), pitch(0), width(0), height(0), format(SurfaceFormat::Unknown)
    {}
};

//-----------------------------------------------------------------------------
// LockFlags
//
// Lock cagrisi davranis bayraklari. DDraw7 DDLOCK_* karsiliklari:
//
//   - Wait      : surface lost durumunda restore + retry (DDLOCK_WAIT)
//   - ReadOnly  : okuma-amacli lock (D3D9 D3DLOCK_READONLY)
//   - Discard   : icerik invalidate (D3D9 D3DLOCK_DISCARD, DDraw7'de yok)
//-----------------------------------------------------------------------------
enum LockFlags
{
    LockFlag_None     = 0,
    LockFlag_Wait     = 1 << 0,
    LockFlag_ReadOnly = 1 << 1,
    LockFlag_Discard  = 1 << 2
};

//-----------------------------------------------------------------------------
// BltFlags
//
// Blt cagrisi davranis bayraklari. DDraw7 DDBLT_*/DDBLTFAST_* karsiliklari.
//-----------------------------------------------------------------------------
enum BltFlags
{
    BltFlag_None        = 0,
    BltFlag_Wait        = 1 << 0,   // DDBLT_WAIT
    BltFlag_SrcColorKey = 1 << 1,   // DDBLT_KEYSRC / DDBLTFAST_SRCCOLORKEY
    BltFlag_NoColorKey  = 1 << 2,   // DDBLTFAST_NOCOLORKEY (default = colorkey)
    BltFlag_ColorFill   = 1 << 3,   // DDBLT_COLORFILL
    BltFlag_StretchY    = 1 << 4    // DDBLTFX_ARITHSTRETCHY (sprite Blt overload)
};

//-----------------------------------------------------------------------------
// SurfaceCreateFlags
//
// Surface create davranis bayraklari. DDSCAPS_* karsiliklari abstract.
//-----------------------------------------------------------------------------
enum SurfaceCreateFlags
{
    SurfaceCreate_None         = 0,
    SurfaceCreate_VideoMemory  = 1 << 0,  // DDSCAPS_VIDEOMEMORY
    SurfaceCreate_SystemMemory = 1 << 1,  // DDSCAPS_SYSTEMMEMORY (Lock hizli)
    SurfaceCreate_Texture      = 1 << 2,  // DDSCAPS_TEXTURE
    SurfaceCreate_Offscreen    = 1 << 3,  // DDSCAPS_OFFSCREENPLAIN
    SurfaceCreate_3DDevice     = 1 << 4   // DDSCAPS_3DDEVICE
};

//-----------------------------------------------------------------------------
// ScreenMode
//
// CDirectDraw::SCREENMODE ile birebir uyum (FULLSCREEN/WINDOWMODE).
//-----------------------------------------------------------------------------
enum class ScreenMode
{
    Fullscreen = 0,
    Windowed   = 1
};

} // namespace render
} // namespace phoenix

#endif // __PHOENIX_RENDER_TYPES_H__
