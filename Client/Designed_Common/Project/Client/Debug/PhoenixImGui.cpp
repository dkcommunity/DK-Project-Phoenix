//----------------------------------------------------------------------
// PhoenixImGui.cpp — bootstrap implementation
//----------------------------------------------------------------------

#include "PhoenixImGui.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_dx11.h"   // 2026-08-31 DX11 P0 — D3D11Backend modu

#include "UserInformation.h"   // g_pUserInformation, attrOperator
#include "PhoenixMobaHUD.h"
#include "PhoenixAuctionHouseHUD.h"

// ImGui Win32 backend mesaj handler (forward decl)
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// DebugConsole (DebugConsole.cpp) — admin tool tab cizimi
namespace PhoenixImGui { extern void DrawDebugConsole(bool* p_open); }
namespace PhoenixImGui { extern void DrawAdminQuickButtons(); }

// 2026-05-23 — Counter HUD (PhoenixCounterHUD.cpp) — sag ust kose 5-sayac HUD.
// Authentication gate'in DISINDA — tum playerlar gorur (HPBAR gibi).
// Counter audit (counter_system_audit_2026-05-23.md) sonucu: 5 sayacin 4'u
// Phoenix'te yok, SEÇENEK B (UI Mock) ile sifirdan baslar, gelecek seansta
// server-side implementasyon ile gercek veri kaynagi entegre edilecek.
namespace PhoenixImGui {
    extern void DrawCounterHUD(bool* p_open);
    extern bool g_showCounterHUD;
}

namespace PhoenixImGui {

// Module-local state
static bool g_initialized       = false;
static bool g_showDebugConsole  = false;
static bool g_showDemoWindow    = false;   // dev test icin, default OFF
static HWND g_hwnd              = NULL;
static IDirect3DDevice9* g_device = NULL;

// 2026-08-31 DX11 P0: aktif impl secimi. g_useDX11=true ise ImGui_ImplDX11_*
// cagrilir; g_device NULL kalir (HUD texture yollari text fallback'e duser).
static bool g_useDX11                    = false;
static ID3D11Device*        g_d11Device  = NULL;
static ID3D11DeviceContext* g_d11Context = NULL;

// 2026-05-18 frame-state guard: Phoenix InitGame/DrawTitleLoading -> Flip ->
// Present cagri yapiyor ama CGameUpdate::UpdateDraw HENUZ devreye girmedi
// (NewFrame cagrilmadi). NewFrame'siz Render -> ImGui Begin/EndFrame mismatch
// -> ErrorRecovery crash. Bu flag NewFrame+Render eslesmesini garanti eder.
static bool g_frameStarted      = false;

// Tilde (`) toggle edge-detect — VK_OEM_3 single key (modifier yok)
static bool g_lastToggleState   = false;

//----------------------------------------------------------------------
// Lifecycle
//----------------------------------------------------------------------
void Initialize(HWND hwnd, IDirect3DDevice9* device)
{
    if (g_initialized) return;
    if (hwnd == NULL || device == NULL) return;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Phoenix 800x600 / 1024x768 — ImGui default font 13px. Admin tool
    // okunabilirligi icin global scale normal tutulur; debug penceresi kendi
    // font scale'ini az miktarda artirir.
    io.FontGlobalScale = 1.00f;

    // ImGui window positions remain draggable during the current game session,
    // but no imgui.ini is loaded or written under GameData.
    io.IniFilename = NULL;

    // 2026-05-18: Software cursor — Phoenix kendi custom sprite cursor cizdigi
    // ve OS cursor'unu gizledigi icin, ImGui hardware-cursor tetiklemesi
    // gorunmez. ImGui kendi cursor'u draw data'ya ekleyebilir ama bunu sadece
    // menu acikken yapacagiz (NewFrame icinde dinamik) — yoksa menu kapaliyken
    // iki cursor (Phoenix sprite + ImGui sprite) overlap eder.
    io.MouseDrawCursor = false;   // default: Phoenix cursor only

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(device);

    g_hwnd          = hwnd;
    g_device        = device;
    g_useDX11       = false;
    g_initialized   = true;
}

//----------------------------------------------------------------------
// 2026-08-31 DX11 P0 — D3D11Backend modu init. Ortak ImGui context ayarlari
// Initialize() ile birebir ayni; yalniz renderer impl DX11.
//----------------------------------------------------------------------
void InitializeDX11(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context)
{
    if (g_initialized) return;
    if (hwnd == NULL || device == NULL || context == NULL) return;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.FontGlobalScale = 1.00f;
    io.IniFilename = NULL;
    io.MouseDrawCursor = false;   // default: Phoenix cursor only

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device, context);

    g_hwnd          = hwnd;
    g_device        = NULL;       // D3D9 device yok — HUD texture yollari fallback
    g_d11Device     = device;
    g_d11Context    = context;
    g_useDX11       = true;
    g_initialized   = true;
}

void Shutdown()
{
    if (!g_initialized) return;
    ShutdownMobaQueueHUD();
    ShutdownAuctionHouseHUD();
    if (g_useDX11)
        ImGui_ImplDX11_Shutdown();
    else
        ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    g_initialized   = false;
    g_hwnd          = NULL;
    g_device        = NULL;
    g_d11Device     = NULL;
    g_d11Context    = NULL;
    g_useDX11       = false;

    // 2026-09-01 crash fix: context yok edildi — frame state'i tasima.
    // RestoreAll frame ORTASINDA Shutdown+Init yaparsa, stale
    // g_frameStarted=true taze context'te (NewFrame gorulmemis,
    // g.Font=NULL) ImGui::Begin cagrilmasina ve AV'ye yol acar.
    g_frameStarted  = false;
}

//----------------------------------------------------------------------
// Per-frame
//----------------------------------------------------------------------
void NewFrame()
{
    if (!g_initialized) return;

    // 2026-05-18 Dinamik cursor: menu acikken Phoenix sprite cursor menu
    // arkasinda kaldigindan ImGui kendi software cursor'unu cizer. Menu
    // kapaliyken Phoenix cursor zaten gorunur — iki cursor overlap olmasin
    // diye ImGui cursor disable.
    //
    // 2026-05-23 v4 CURSOR FIX (Counter HUD): HUD daima acik (HPBAR gibi)
    // oldugundan kullanici fareyi HUD uzerine getirince Phoenix sprite cursor
    // HUD altinda kaliyor — kullanici cursor'i goremiyor. Cozum: WantCaptureMouse
    // gate eklendi. ImGui pencereleri uzerine gelince (HUD veya DebugConsole)
    // ImGui kendi software cursor'unu cizer; oyun alanindayken Phoenix sprite
    // cursor (default davranis korunur, overlap yok).
    //
    // WantCaptureMouse onceki frame'in son state'ini tasir — 1 frame gecikme
    // 60FPS'de ~16ms, kullanici fark etmez. Fare HUD'a girince 1 frame sonra
    // ImGui cursor gorunur, ciktiktan sonra 1 frame sonra Phoenix cursor.
    const bool mouseOverImGui = ImGui::GetIO().WantCaptureMouse;
    ImGui::GetIO().MouseDrawCursor = g_showDebugConsole || mouseOverImGui;

    if (g_useDX11)
        ImGui_ImplDX11_NewFrame();
    else
        ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    g_frameStarted = true;
}

void Render()
{
    if (!g_initialized) return;

    // 2026-05-18 v2 FRAME PATTERN: Phoenix birden cok Present per logical frame
    // cagirabiliyor (DrawTitleLoading loading bar guncellemesi, double-buffer
    // flip vs.). Eski v1 fix (frameStarted Render'da false yap) yaniyp-sonme
    // yaratiyordu — ilk Present ImGui'yi cizdi, ikinci Present'ta DrawData
    // invalidate olmustu (frameStarted=false'tan dolayi Render skip).
    //
    // v2 ayrim:
    //   1. ImGui::Render() — UI'yi DrawData'ya finalize et (NewFrame eslesi).
    //      Sadece frame baslamissa ve henuz Render edilmemisse calisir.
    //   2. ImGui_ImplDX9_RenderDrawData(...) — cached DrawData'yi D3D9'a blit.
    //      HER Present'ta cagrilir (overlay her frame gorunsun, yanip-sonme yok).
    //      DrawData NewFrame'e kadar valid kalir.

    // ---- 1) Frame finalize (UI cizimi + ImGui::Render) ----
    if (g_frameStarted)
    {
        // Authentication gate — sadece operator karakter cizebilsin.
        // attrOperator.GetValue() 3 flag (MAC/RELEASELOG/PLAYERSHAPE), client-local
        // (DB-verified degil). Production grade icin gelecekte GCSetAdminMode
        // packet + DB Competence column gerekir; su an client-side gate yeterli.
        const bool isOperator = (g_pUserInformation != NULL
            && (g_pUserInformation->attrOperator.GetValue() & OPERATORTYPE_AUTHMASK) != 0);

        if (isOperator)
        {
            DrawAdminQuickButtons();

            if (g_showDebugConsole)
                DrawDebugConsole(&g_showDebugConsole);

            if (g_showDemoWindow)
                ImGui::ShowDemoWindow(&g_showDemoWindow);
        }

        //--------------------------------------------------------------
        // 2026-05-23 — Counter HUD (Phoenix Counters)
        //--------------------------------------------------------------
        // Authentication gate'in DISINDA — tum playerlar gorebilsin
        // (HPBAR gibi). Pozisyon: ilk acilista sag-ust kose. Pencere
        // draggable kalir; gameplay input'unu gereksiz focus ile engellemez.
        // Tum sayaclar 0'dan baslar (mock veri kaynagi).
        // Detay: counter_system_audit_2026-05-23.md + plan dokumani.
        //--------------------------------------------------------------
        if (g_showCounterHUD)
            DrawCounterHUD(NULL);   // NULL = kapanma butonu yok (pure HUD)

        //--------------------------------------------------------------
        // 2026-06-24 — AOS/MOBA queue HUD
        //--------------------------------------------------------------
        // Authentication gate'in DISINDA — queue packet'i alan player
        // bekleme durumunu gorsun. Pencere draggable, ini/log output yok.
        //--------------------------------------------------------------
        DrawMobaQueueHUD(g_device);
        DrawAuctionHouseHUD(g_device);

        ImGui::Render();
        g_frameStarted = false;
    }

    // ---- 2) Cached DrawData'yi backend'e blit (her Present) ----
    ImDrawData* drawData = ImGui::GetDrawData();
    if (drawData != NULL && drawData->Valid)
    {
        if (g_useDX11)
            ImGui_ImplDX11_RenderDrawData(drawData);
        else
            ImGui_ImplDX9_RenderDrawData(drawData);
    }
}

//----------------------------------------------------------------------
// Win32 message handler
//----------------------------------------------------------------------
bool WndProcHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (!g_initialized) return false;

    // 2026-05-18 NOTE: F12 ImGui isolation kaldirildi. F12 crash debug
    // sirasinda denedik, ama gercek sebep VS Debugger "Debug.BreakAll" hotkey
    // (F12 default keybind) idi — Phoenix masum. F12 normal Phoenix kullanim
    // (skill atama hotkey) icin route edilir. ImGui Ctrl+Shift kombinasyonu
    // kendi keybind'i.

    // 1) ImGui backend kendi mesaj handler'i (state update; return value
    //    nadiren true). Mesaj ImGui'ye bildirilir — IO.MousePos, MouseDown,
    //    KeysDown vs. doldurulur.
    ImGui_ImplWin32_WndProcHandler(hwnd, msg, wp, lp);

    // 2) 2026-05-18 KEYBIND v4 — DEL (VK_DELETE) tek tus toggle:
    // Kullanici karari: Ctrl+Shift kombinasyonu yerine tek tus DEL.
    // Phoenix Client.cpp:1437'de VK_DELETE case var (#ifdef OUTPUT_DEBUG ile
    // DebugMessage->Clear), ImGui burada once consume edip return true ile
    // Phoenix WindowProc'a iletmiyor. Phoenix native VK_DELETE handler
    // tetiklenmez. ImGui_ImplWin32_WndProcHandler asagida cagriliyor (state
    // update icin), ama bizim erken return Phoenix WindowProc'a iletmiyor.
    //
    // 2026-05-20 AUTH GATE: Normal oyunculara DEL toggle calisiyordu — Render()
    // icinde auth gate var (window invisible normal player'a) ama state hala
    // toggle ediliyordu ve input gate (MOUSE/KEYBOARD) yan etkisi olusuyordu.
    // attrOperator gate Render() ile race-parity (line 130-131).
    if (msg == WM_KEYDOWN && wp == VK_DELETE && !g_lastToggleState)
    {
        // Auth check: sadece operator karakter DEL toggle yapabilsin
        const bool isOperator = (g_pUserInformation != NULL
            && (g_pUserInformation->attrOperator.GetValue() & OPERATORTYPE_AUTHMASK) != 0);

        if (isOperator)
        {
            ToggleDebugConsole();
            g_lastToggleState = true;
            return true;
        }
        // Operator degil: DEL Phoenix WindowProc'a iletilsin (default behavior)
        g_lastToggleState = true;
    }
    else if (msg == WM_KEYUP && wp == VK_DELETE)
    {
        g_lastToggleState = false;
    }

    // 3) 2026-05-18 INPUT FOCUS GUARD: ImGui penceresi uzerine tiklandiginda
    //    Phoenix de ayni WM_LBUTTONDOWN'i isleyip karakter hareket ettiriyordu
    //    (kullanici raporu: "menu'de tikladigimda karakter o yone yuruyor").
    //    WantCaptureMouse/WantCaptureKeyboard true ise ilgili mesajlar Phoenix
    //    WindowProc'a iletilmesin — true don, ana handler DefWindowProc'a gecsin.
    const ImGuiIO& io = ImGui::GetIO();

    if (io.WantCaptureMouse)
    {
        switch (msg)
        {
            case WM_MOUSEMOVE:
            case WM_LBUTTONDOWN: case WM_LBUTTONUP: case WM_LBUTTONDBLCLK:
            case WM_RBUTTONDOWN: case WM_RBUTTONUP: case WM_RBUTTONDBLCLK:
            case WM_MBUTTONDOWN: case WM_MBUTTONUP: case WM_MBUTTONDBLCLK:
            case WM_XBUTTONDOWN: case WM_XBUTTONUP: case WM_XBUTTONDBLCLK:
            case WM_MOUSEWHEEL:  case WM_MOUSEHWHEEL:
                return true;   // Phoenix kalan WindowProc'u atla
        }
    }

    if (io.WantCaptureKeyboard)
    {
        switch (msg)
        {
            case WM_KEYDOWN: case WM_KEYUP:
            case WM_SYSKEYDOWN: case WM_SYSKEYUP:
            case WM_CHAR:    case WM_DEADCHAR:
            case WM_SYSCHAR: case WM_SYSDEADCHAR:
                return true;
        }
    }

    return false;
}

//----------------------------------------------------------------------
// D3D9 lost device handling (ALT-TAB, resolution change)
//----------------------------------------------------------------------
void OnDeviceLost()
{
    if (!g_initialized) return;
    if (g_useDX11)
        ImGui_ImplDX11_InvalidateDeviceObjects();
    else
        ImGui_ImplDX9_InvalidateDeviceObjects();
}

void OnDeviceReset()
{
    if (!g_initialized) return;
    if (g_useDX11)
        ImGui_ImplDX11_CreateDeviceObjects();
    else
        ImGui_ImplDX9_CreateDeviceObjects();
}

//----------------------------------------------------------------------
// Input fokus query
//----------------------------------------------------------------------
bool WantCaptureMouse()
{
    if (!g_initialized) return false;
    return ImGui::GetIO().WantCaptureMouse;
}

bool WantCaptureKeyboard()
{
    if (!g_initialized) return false;
    return ImGui::GetIO().WantCaptureKeyboard;
}

//----------------------------------------------------------------------
// DebugConsole visibility
//----------------------------------------------------------------------
void ToggleDebugConsole()
{
    g_showDebugConsole = !g_showDebugConsole;
}

bool IsDebugConsoleVisible()
{
    return g_showDebugConsole;
}

} // namespace PhoenixImGui
