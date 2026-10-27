#include "MCreature.h"
#include "vs_ui.h"
#include "VS_UI_GameCommon.h"  // S_SLOT + g_char_slot_ingame için
#include <windows.h>

// 1. UI Packet Event Handlers
// MIGRATED → GameUI.cpp (UI_RunHollyWar/Military/PayEvent/TMall):
//   real implementation, referans body'leri ile, modernize edildi.
//   Boş stub'lar 2026-05-05 kaldırıldı.

// 2. Character Info Handlers — GERÇEK IMPLEMENTATION
// FAZ 4 (2026-05-03): eski stub'lar boş body → CharInfo penceresi açıldığında
// hpsteal/luck/allres/mpsteal değerleri set edilmiyor → render path her frame
// yeniden compute ediyor (FPS drop / freeze semptomu).
// Referans impl: Referans Proje/Designed_Common/Project/Client/GameUI.cpp:1338-1356
// Trivial field assignment to g_char_slot_ingame.
void __cdecl UI_SetCharInfoLuck(int value)    { g_char_slot_ingame.luck    = value; }
void __cdecl UI_SetCharInfoHpSteal(int value) { g_char_slot_ingame.hpsteal = value; }
void __cdecl UI_SetCharInfoMpSteal(int value) { g_char_slot_ingame.mpsteal = value; }
void __cdecl UI_SetCharInfoAllRes(int value)  { g_char_slot_ingame.allres  = value; }

// 3. Drawing / Surface
// InitializeGL: GL.lib (binary, source erişilemez) içindeki global state init
// ediyordu (bpp + R/G/B mask). Modern Phoenix'te dgVoodoo + DDraw zaten BPP/RGB
// init eder; GL.lib'in iç state'ine bağlı kullanım yok. No-op kalır.
// GL.lib bağımlılığı kaldırıldığında bu fonksiyon ve çağrı sitesi de temizlenebilir.
void __cdecl InitializeGL(int, int, int, int) {}
// FAZ 4 (2026-05-03): SetSurfaceInfo / FillRect / rectangle GERÇEK IMPLEMENTATION
// (eski stub'lar boş body → surface_info uninitialized stack memory ile çıkıyor →
// BltLockedClip / BltClipWidth garbage p_dest'e yazıyor → random crash).
// Modern projede gerçek tanım YOK (referansta GL.lib binary'sindeydi —
// `Designed_Common/Lib/ui/GL.lib`, source erişilemez).
// DirectDraw belgelenmiş davranış: DDSURFACEDESC2'den 4 alan kopyala.
// Pattern exemplar: g_GetCtrlPushState (PS.LIB binary → Win32 GetKeyState).
// Brief I (jules_task_I_clientmissingstubs_cleanup_roadmap.md) Cat (A) +
// REPLACE-FROM-MODERN tavsiyesi, exemplar onay.
void __cdecl SetSurfaceInfo(S_SURFACEINFO *info, const _DDSURFACEDESC2 *p_ddsd)
{
    if (!info || !p_ddsd) return;
    info->p_surface = p_ddsd->lpSurface;
    info->width     = (int)p_ddsd->dwWidth;
    info->height    = (int)p_ddsd->dwHeight;
    info->pitch     = (int)p_ddsd->lPitch;
}
void __cdecl FillRect(S_SURFACEINFO *info, Rect *rect, int color)
{
    if (!info || !rect || !info->p_surface) return;
    if (info->pitch <= 0 || rect->w <= 0 || rect->h <= 0) return;
    int x0 = rect->x, y0 = rect->y;
    int x1 = x0 + rect->w, y1 = y0 + rect->h;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > info->width)  x1 = info->width;
    if (y1 > info->height) y1 = info->height;
    if (x0 >= x1 || y0 >= y1) return;
    BYTE *base = (BYTE*)info->p_surface + y0 * info->pitch;
    WORD c = (WORD)color;
    for (int y = y0; y < y1; ++y) {
        WORD *row = (WORD*)base + x0;
        for (int x = x0; x < x1; ++x) row[x - x0] = c;
        base += info->pitch;
    }
}
void __cdecl rectangle(S_SURFACEINFO *info, Rect *rect, int color)
{
    if (!info || !rect || !info->p_surface) return;
    if (info->pitch <= 0 || rect->w <= 0 || rect->h <= 0) return;
    int x0 = rect->x, y0 = rect->y;
    int x1 = x0 + rect->w - 1, y1 = y0 + rect->h - 1;
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

// 4. MCreature
// bool MCreature::IsRide_Fast_Transforter(void) { return false; }
// void MCreature::SetHellBikeEffect(unsigned short) {}

// 5. Variables & Misc
// class CMessageArray *g_pWorldChatingMessage = nullptr;
// class CInfomationBord *g_pInfomationBord = nullptr;

// char g_mark[MARK_MAX][9][4] = {0};
// FAZ 4 (2026-05-02): g_GetCtrlPushState GERÇEK IMPLEMENTATION (eski stub
// `return false` her zaman, hotkey-with-Ctrl davranışı ölüydü).
// Modern projede gerçek tanım YOK (referansta PS.LIB binary'sindeydi).
// Win32 GetKeyState ile Ctrl tuşunun anlık basılı durumunu döndür.
// Hotkey'ler (Ctrl+Tab gear, Ctrl+M minimap, Ctrl+I CharInfo, vb.) artık çalışır.
bool __cdecl g_GetCtrlPushState(void) {
    return (GetKeyState(VK_CONTROL) & 0x8000) != 0;
}

// 6. Various System Stubs
// MIGRATED → GameUI.cpp:
//   UI_OkMixingForgeCoreZap (referans line 4622) — real impl
//   UI_PopupStrMessage (referans line 4739) — real impl
//   Boş stub'lar 2026-05-05 kaldırıldı.
//
// FAZ 4 (2026-05-03): SetGearInfo stub KALDIRILDI — gerçek impl
// PacketFunction.cpp:2143/2146'da mevcut (4-arg __GEAR_SWAP_CHANGE veya 2-arg).
// Stub farklı 3-arg signature (GearInfo*, BloodBibleSignInfo*, bool) idi —
// muhtemelen çağrılmıyordu. Karakter görünmemesi semptomu: stub çağrılırsa
// gear info hiç set edilmiyor → player render fail.

// 7. C_VS_UI methods
// void C_VS_UI::RunRepairItemAll(unsigned long, unsigned long) {}

// 8. Skill Effects
// MIGRATED → PacketFunction.cpp:
//   SkillBatStorm1 (referans line 5207) — real impl
//   SkillSPECTORINVERSE (referans line 5332) — real impl
//   Add_FakeCreature_Wild_Wolf_Strike (referans line 7588) — real impl
//   Boş stub'lar 2026-05-05 kaldırıldı.

// 9. UI Object C_VS_UI_TIME_COUNT — STUB DEVRE DISI (FAZ 4, 2026-05-02)
// Gercek implementasyon vs_ui_lib icindeki VS_UI_TIMECOUNT.cpp'de mevcut
// (Designed_Common/Project/NEW - VS_UI/src/VS_UI_TIMECOUNT.cpp).
// Bu stub'lar GEMINI tarafindan link error fix icin yaratilmis ama gercek
// kod aktif. Stub'lari kaldirmak vs_ui_lib symbol'lerinin kullanilmasini
// saglar — DESC sizeof paradoksu (Client TU=248 vs Lib=252) bu cakismaya
// bagli olabilir.
#if 0  // FAZ 4 stub disable test
#include "VS_UI_TIMECOUNT.h"
C_VS_UI_TIME_COUNT::C_VS_UI_TIME_COUNT(unsigned long time, char const *desc) {}
C_VS_UI_TIME_COUNT::~C_VS_UI_TIME_COUNT(void) {}
void C_VS_UI_TIME_COUNT::KeyboardControl(unsigned int, unsigned int, long) {}
bool C_VS_UI_TIME_COUNT::MouseControl(unsigned int, int, int) { return false; }
void C_VS_UI_TIME_COUNT::Show(void) {}
void C_VS_UI_TIME_COUNT::Process(void) {}
void C_VS_UI_TIME_COUNT::Run(unsigned long) {}
bool C_VS_UI_TIME_COUNT::IsPixel(int, int) { return false; }
void C_VS_UI_TIME_COUNT::WindowEventReceiver(unsigned long) {}
void C_VS_UI_TIME_COUNT::ShowButtonDescription(class C_VS_UI_EVENT_BUTTON *) {}
void C_VS_UI_TIME_COUNT::ShowButtonWidget(class C_VS_UI_EVENT_BUTTON *) {}
void C_VS_UI_TIME_COUNT::Finish(void) {}
void C_VS_UI_TIME_COUNT::Start(void) {}
#endif // FAZ 4 stub disable test

// TouchSense wrapper stubs — STUB ZORUNLU (FAZ 4 KAYIT, 2026-05-02)
// vs_ui_lib içinde CImm.cpp gerçek tanımı var, AMA TouchSense SDK lib
// (CImmDevice/Periodic/Project/Effect dllimport) modern Phoenix build'inde
// YOK. CImm.cpp vs_ui_lib içinde derlense de unresolved external veriyor.
// Stub Client.exe için gerek. Gelecek: TouchSense SDK ekle veya CImm.cpp'i
// vs_ui_lib'den çıkar.
#include "CImm.h"
CImm::CImm() {}
CImm::~CImm() {}
void CImm::Enable() {}
void CImm::Disable() {}
void CImm::ForceAction(int sound_id) const {}
void CImm::ForceUI(unsigned int ID) const {}
void CImm::ForceSkill(const int sound_id) const {}
void CImm::ForceInventory(const int sound_id) const {}
void CImm::ForceUseItem(const int sound_id) const {}

// 10. Missing globals from WinMain.cpp cleanup
DWORD g_CurrentFrame = 0;
int g_bLButtonDown = 0;
HWND g_hWnd = NULL;
// FAZ 1.6-1.8 — UI_ResultReceiver stub kaldırıldı. Gerçek tanım GameUI.cpp'deki
// #if 0 guard'ı kaldırılarak aktive edildi. Bu boş stub tüm UI mesajlarını
// (UI_CLOSE_OPTION, UI_TERMINATION, UI_RUN_NEWUSER_REGISTRATION, vs.) sessizce
// yutuyordu — Option X butonu, Exit "Evet", NewID butonu hepsi ölüydü.
int g_Dimension = 0;
// LoadWorldMapInfo: gerçek impl WinMain.cpp:1804'te (BOOL signature, full body).
// Stub `return 1;` kaldırıldı — link order'da real impl tercih edilecek.
// Removed stub WindowProc that broke CreateWindowEx

int g_LeftPremiumDays = 0;
bool g_bFamily = false;
bool gbl_show_item = false;
int g_mouse_x = 0;
int g_mouse_y = 0;
bool gbl_info_show = false;

// 11. Legacy CRT compatibility shim for VC6-compiled libraries (Jpeg.lib)
// Modern MSVC removed the __iob symbol; provide it via __acrt_iob_func
#include <cstdio>
extern "C" {
    FILE _iob[] = { *stdin, *stdout, *stderr };
    FILE* __cdecl __iob_func(void) { return _iob; }
}
