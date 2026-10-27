#include "client_PCH.h"

#include "MCreature.h"
#include "VS_UI_GameCommon.h"
#include "vs_ui.h"

#include <cstdio>
#include <windows.h>

// Eski GL.lib icin kaynak yok. VS_UI kodunun kullandigi aktif yuzey
// yardimcilari burada modern build icin tek yerde saglanir.
void __cdecl InitializeGL(int, int, int, int) {}

void __cdecl SetSurfaceInfo(S_SURFACEINFO *info, const _DDSURFACEDESC2 *p_ddsd)
{
    if (!info || !p_ddsd) return;

    info->p_surface = p_ddsd->lpSurface;
    info->width = (int)p_ddsd->dwWidth;
    info->height = (int)p_ddsd->dwHeight;
    info->pitch = (int)p_ddsd->lPitch;
}

void __cdecl FillRect(S_SURFACEINFO *info, Rect *rect, int color)
{
    if (!info || !rect || !info->p_surface) return;
    if (info->pitch <= 0 || rect->w <= 0 || rect->h <= 0) return;

    int x0 = rect->x;
    int y0 = rect->y;
    int x1 = x0 + rect->w;
    int y1 = y0 + rect->h;

    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > info->width) x1 = info->width;
    if (y1 > info->height) y1 = info->height;
    if (x0 >= x1 || y0 >= y1) return;

    BYTE *base = (BYTE*)info->p_surface + y0 * info->pitch;
    WORD c = (WORD)color;

    for (int y = y0; y < y1; ++y) {
        WORD *row = (WORD*)base + x0;
        for (int x = x0; x < x1; ++x) {
            row[x - x0] = c;
        }
        base += info->pitch;
    }
}

void __cdecl rectangle(S_SURFACEINFO *info, Rect *rect, int color)
{
    if (!info || !rect || !info->p_surface) return;
    if (info->pitch <= 0 || rect->w <= 0 || rect->h <= 0) return;

    int x0 = rect->x;
    int y0 = rect->y;
    int x1 = x0 + rect->w - 1;
    int y1 = y0 + rect->h - 1;

    if (x1 < 0 || y1 < 0 || x0 >= info->width || y0 >= info->height) return;

    WORD c = (WORD)color;
    int cx0 = x0 < 0 ? 0 : x0;
    int cx1 = x1 >= info->width ? info->width - 1 : x1;
    int cy0 = y0 < 0 ? 0 : y0;
    int cy1 = y1 >= info->height ? info->height - 1 : y1;

    if (y0 >= 0 && y0 < info->height) {
        WORD *row = (WORD*)((BYTE*)info->p_surface + y0 * info->pitch);
        for (int x = cx0; x <= cx1; ++x) row[x] = c;
    }

    if (y1 != y0 && y1 >= 0 && y1 < info->height) {
        WORD *row = (WORD*)((BYTE*)info->p_surface + y1 * info->pitch);
        for (int x = cx0; x <= cx1; ++x) row[x] = c;
    }

    for (int y = cy0; y <= cy1; ++y) {
        WORD *row = (WORD*)((BYTE*)info->p_surface + y * info->pitch);
        if (x0 >= 0 && x0 < info->width) row[x0] = c;
        if (x1 != x0 && x1 >= 0 && x1 < info->width) row[x1] = c;
    }
}

// Eski PS.LIB karsiligi. Ctrl tabanli UI hotkey davranisini korur.
bool __cdecl g_GetCtrlPushState(void)
{
    return (GetKeyState(VK_CONTROL) & 0x8000) != 0;
}

// Eski Client.cpp / NEW - VS_UI WinMain global durumlari. Tek tanim burada.
DWORD g_CurrentFrame = 0;
BOOL g_bLButtonDown = FALSE;
HWND g_hWnd = NULL;
int g_Dimension = 0;
int g_LeftPremiumDays = 0;
bool g_bFamily = false;
bool gbl_show_item = false;
int g_mouse_x = 0;
int g_mouse_y = 0;
bool gbl_info_show = false;

#if _MSC_VER >= 1900
extern "C" {
    FILE _iob[] = { *stdin, *stdout, *stderr };
    FILE* __cdecl __iob_func(void) { return _iob; }
}
#endif
