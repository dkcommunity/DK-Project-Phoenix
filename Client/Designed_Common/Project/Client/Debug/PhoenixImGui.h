//----------------------------------------------------------------------
// PhoenixImGui.h
//----------------------------------------------------------------------
// Phoenix ImGui bootstrap — Hibrit Admin Menu mimarisi:
//   - VS_UI native admin menu (LSHIFT+RSHIFT, OperatorCategory) DOKUNULMAZ
//   - ImGui ek DebugConsole (CTRL+SHIFT+F12) — yeni Item/Creature/Zone tab
//   - Authentication: attrOperator auth bits (SERVERAUTH/MAC/DEBUG), not PLAYERSHAPE
//
// 5 entry point (Phoenix WinMain / GameUpdate / D3D9 flow):
//   1. WndProcHandler() — WindowProc icinde ilk olarak cagir
//   2. Initialize() — CDirectDraw::Initialize() sonrasi (device hazir)
//   3. NewFrame() — CGameUpdate::UpdateDraw() basi
//   4. Render() — D3D9 Present() oncesi
//   5. Shutdown() — WM_DESTROY veya app exit
// + Lost Device: OnDeviceLost() / OnDeviceReset() (ALT-TAB)
//
// 2026-05-18 — ImGui v1.91.4 vendored to F:\Project Phoenix\thirdparty\imgui
//----------------------------------------------------------------------

#ifndef __PHOENIX_IMGUI_H__
#define __PHOENIX_IMGUI_H__

#include <windows.h>

struct IDirect3DDevice9;
struct ID3D11Device;
struct ID3D11DeviceContext;

namespace PhoenixImGui {

// Lifecycle ------------------------------------------------------------------
void Initialize(HWND hwnd, IDirect3DDevice9* device);
// 2026-08-31 Legacy DX11 gecis P0 — D3D11Backend uzerinden ImGui DX11 impl.
// NOT: HUD item ikonlari (Moba/Auction) D3D9 texture yoluna bagli; DX11
// modunda text fallback cizilir (P1'de DX11 texture yolu eklenecek).
void InitializeDX11(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);
void Shutdown();

// Per-frame ------------------------------------------------------------------
void NewFrame();
void Render();

// Win32 message handler — WindowProc icinde ilk olarak cagir.
// Return TRUE = ImGui mesaji yakaladi, Phoenix WindowProc kalan kodu atla.
bool WndProcHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

// D3D9 lost device callbacks (ALT-TAB / resolution change)
void OnDeviceLost();
void OnDeviceReset();

// Input fokus query — ImGui penceresi uzerindeyken Phoenix mouse/keyboard
// islememeli. CGameUpdate, MMouseProc vb. yerlerde gate olarak kullanilir.
bool WantCaptureMouse();
bool WantCaptureKeyboard();

// DebugConsole visibility (CTRL+SHIFT+F12 ile toggle)
void ToggleDebugConsole();
bool IsDebugConsoleVisible();

// 2026-05-23 — Counter HUD visibility (sag-ust kose 5-sayac overlay)
// Default true, oyun girisinde otomatik gorunur (HPBAR gibi auto-load).
// Tum playerlar gorebilir (operator-only DEGIL).
// Detay: PhoenixCounterHUD.h + counter_system_audit_2026-05-23.md
void SetCounterHUDVisible(bool visible);
bool IsCounterHUDVisible();

} // namespace PhoenixImGui

#endif
