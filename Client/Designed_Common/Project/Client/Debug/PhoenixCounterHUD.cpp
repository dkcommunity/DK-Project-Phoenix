//----------------------------------------------------------------------
// PhoenixCounterHUD.cpp — 5-counter HUD implementation
//----------------------------------------------------------------------
// Referans: counter_system_audit_2026-05-23.md +
//            imgui_counter_hud_system_plan_2026-05-23.md
//
// HUD penceresi PhoenixImGui::Render() icinden cagrilir. Authentication
// gate'in DISINDA — tum playerlar gorebilsin (HPBAR gibi).
//
// Pozisyon: ilk acilista sag-ust kose (10px kenar bosluklu, resolution-aware).
// Pencere draggable kalir; PhoenixImGui ini persistence'i global kapatir.
//----------------------------------------------------------------------

#include "PhoenixCounterHUD.h"
#include "imgui.h"

#include <cstdio>

// 2026-05-23 — Mode gate icin g_Mode + MODE_GAME enum erisim.
// HUD sadece gameplay sirasinda gorulur (title/login/karakter secimde gizli).
#include "../ClientDef.h"   // CLIENT_MODE enum, g_Mode extern
#include "../MMoneyManager.h" // g_pMoneyManager, Market DianJuan/JiFen bakiyesi

namespace PhoenixImGui {

namespace {

void DrawCounterBlock(const char* title, const char* value, const ImVec4& titleColor)
{
    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Text, titleColor);
    ImGui::TextUnformatted(title);
    ImGui::PopStyleColor();

    ImGui::Indent(12.0f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
    ImGui::TextUnformatted(value);
    ImGui::PopStyleColor();
    ImGui::Unindent(12.0f);
}

} // namespace

//----------------------------------------------------------------------
// Global state
//----------------------------------------------------------------------

// Tum sayaclar 0'dan baslar (CounterState default constructor)
CounterState g_CounterState;

// HUD default acik — HPBAR gibi auto-load
bool g_showCounterHUD = true;

//----------------------------------------------------------------------
// DrawCounterHUD — sag ust kose HUD overlay
//----------------------------------------------------------------------
// p_open: NULL gecilirse pencere kapatma butonu yok (pure HUD).
//         Non-NULL gecilirse close button gorulur.
//
// Renkler:
//   - Title:       altin sari   (1.00, 0.85, 0.40)
//   - Monster:     turuncu      (1.00, 0.50, 0.30)
//   - PK:          kirmizi      (1.00, 0.30, 0.30)
//   - Time:        acik mavi    (0.50, 0.80, 1.00)
//   - Promotion/Cash:   yesil        (0.40, 1.00, 0.50)
//   - Mileage/Point:     sari         (1.00, 1.00, 0.40)
//----------------------------------------------------------------------
void DrawCounterHUD(bool* p_open)
{
    if (!g_showCounterHUD) return;
    if (p_open && !*p_open) return;

    // 2026-05-23 GAME MODE GATE — Counter HUD sadece oyun icinde gorulur.
    // Title screen, login, server selection, character selection ekranlarinda
    // gizli olmali (kullanici istegi: "sadece oyuna girildiginde gostermeli").
    if (g_Mode != MODE_GAME) return;

    // Baslik ustte, sayaç/deger altta olacak sekilde daha dikey layout.
    const float kDefaultW = 145.0f;
    const float kDefaultH = 330.0f;

    // Pozisyon — ilk acilis sag-ust kose, sonra kullanici tasiyabilir.
    // Ini persistence kapali oldugu icin default sadece her process basinda uygulanir.
    ImGuiIO& io = ImGui::GetIO();
    const float kEdgeMargin = 10.0f;
    ImVec2 default_pos(io.DisplaySize.x - kDefaultW - kEdgeMargin, kEdgeMargin);
    ImGui::SetNextWindowPos(default_pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(kDefaultW, kDefaultH), ImGuiCond_FirstUseEver);

    // 2026-05-23 v3 OKUNABILIRLIK FIX:
    // • Background opacity 0.80 → 0.92 (daha opak, metin daha kontrast)
    // • Border + TitleBar daha parlak
    // • Pencere padding (ic bosluk) artirildi
    // • Item spacing (satir araliği) genisledi
    ImGui::PushStyleColor(ImGuiCol_WindowBg,        ImVec4(0.04f, 0.04f, 0.08f, 0.92f));
    ImGui::PushStyleColor(ImGuiCol_Border,          ImVec4(0.75f, 0.55f, 0.25f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg,         ImVec4(0.10f, 0.08f, 0.04f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive,   ImVec4(0.20f, 0.15f, 0.08f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed,ImVec4(0.08f, 0.06f, 0.03f, 0.90f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,    ImVec2(12.0f, 10.0f)); //ImVec2(12.0f, 10.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,      ImVec2(8.0f, 8.0f));

    // 2026-05-23 KULLANICI ISTEKLERI:
    //   • Hareket ettirilebilir → NoMove KALDIRILDI
    //   • Shrinking (collapse) → NoCollapse + NoTitleBar KALDIRILDI
    //     (collapse oku TitleBar icinde gozukur, dolayisi ile TitleBar gerekli)
    //   • Kapatma tusu OLMASIN → p_open = NULL gecilir (ImGui close button cizmez)
    //   • Disk persistence YOK -> PhoenixImGui io.IniFilename = NULL
    //
    // Korunan flag'ler:
    //   • NoFocusOnAppearing + NoBringToFrontOnFocus → gameplay'i engellemez
    //   • NoNav → klavye navigation HUD'a girmesin
    //   • NoScrollbar → 5 satir sabit, scroll gerek yok
    const ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    // p_open = NULL: close button (X) ciizilmez. Pencere kapatilamaz HUD.
    // Title bar collapse oku (▼/►) gorulur, kullanici tikla → kucult/buyut.
    // Pencere baslik: "Phoenix Counters" (TitleBar metni — ImGui cizer).
    const bool bBodyVisible = ImGui::Begin("Counters", NULL, flags);

    // 2026-08-31 KENAR CLAMP FIX: ImGui default'u pencereyi yatayda son
    // ~19px gorunur kalana kadar ekran disina kaydirmaya izin verir —
    // bar sag kenardan tasip kesik gorunuyordu (kose "gorunmez duvar"
    // sikayeti). Klasik oyun UI davranisi: pencere her zaman tamamen
    // ekran icinde tutulur. Collapsed iken GetWindowSize title-bar
    // boyutunu dondurur, acikken tam boyutu — iki durumda da dogru.
    {
        const ImVec2 hudPos  = ImGui::GetWindowPos();
        const ImVec2 hudSize = ImGui::GetWindowSize();
        float fMaxX = io.DisplaySize.x - hudSize.x;
        float fMaxY = io.DisplaySize.y - hudSize.y;
        if (fMaxX < 0.0f) fMaxX = 0.0f;
        if (fMaxY < 0.0f) fMaxY = 0.0f;
        ImVec2 hudClamped = hudPos;
        if (hudClamped.x < 0.0f) hudClamped.x = 0.0f;
        else if (hudClamped.x > fMaxX) hudClamped.x = fMaxX;
        if (hudClamped.y < 0.0f) hudClamped.y = 0.0f;
        else if (hudClamped.y > fMaxY) hudClamped.y = fMaxY;
        if (hudClamped.x != hudPos.x || hudClamped.y != hudPos.y)
            ImGui::SetWindowPos(hudClamped);
    }

    if (!bBodyVisible)
    {
        // Collapsed durumda body cizilmez — End() yine de cagrilmali
        ImGui::End();
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(5);
        return;
    }

    // 2026-05-23 v3 OKUNABILIRLIK FIX:
    // PhoenixImGui.cpp:54'de io.FontGlobalScale=0.95 (global olarak kucuk).
    // Bu HUD penceresine ozel buyutme — SetWindowFontScale Begin/End arasinda
    // sadece bu pencerenin font'unu olceklendirir (digerleri etkilenmez).
    // 1.35 = ~17.5px (default 13px x 0.95 global x 1.35 = 16.78 efektif).
    ImGui::SetWindowFontScale(1.35f);

    // 2026-05-23 v3 — Daha parlak/doygun renkler (kontrast artisi)
    // Eski paletteki bazi renkler (0.50, 0.30, 0.30) okunmasi zordu.
    // Yeni palet tum 0.7+ doygunluk, beyaz/gri yerine canli ton.
    char value[64];

    std::snprintf(value, sizeof(value), "%llu / %llu",
        (unsigned long long)g_CounterState.monsterCount,
        (unsigned long long)g_CounterState.monsterGoal);
    DrawCounterBlock("Monster Count", value, ImVec4(1.00f, 0.65f, 0.35f, 1.00f));

    std::snprintf(value, sizeof(value), "%u / %u",
        g_CounterState.pkCount,
        g_CounterState.pkGoal);
    DrawCounterBlock("PK Count", value, ImVec4(1.00f, 0.50f, 0.50f, 1.00f));

    std::snprintf(value, sizeof(value), "%u / %u",
        g_CounterState.timeCount,
        g_CounterState.timeGoal);
    DrawCounterBlock("Time Count", value, ImVec4(0.55f, 0.90f, 1.00f, 1.00f));

    std::snprintf(value, sizeof(value), "%d",
        g_pMoneyManager ? g_pMoneyManager->GetDianJuan() : 0);
    DrawCounterBlock("Market Cash", value, ImVec4(0.55f, 1.00f, 0.60f, 1.00f));

    std::snprintf(value, sizeof(value), "%d",
        g_pMoneyManager ? g_pMoneyManager->GetJiFen() : 0);
    DrawCounterBlock("Market Points", value, ImVec4(1.00f, 0.95f, 0.40f, 1.00f));

    ImGui::Separator();

    ImGui::End();
    ImGui::PopStyleVar(3);     // WindowBorderSize, WindowPadding, ItemSpacing
    ImGui::PopStyleColor(5);   // WindowBg, Border, TitleBg, TitleBgActive, TitleBgCollapsed
}

//----------------------------------------------------------------------
// Counter update helpers
//----------------------------------------------------------------------

void IncrementMonsterCount(uint64_t delta)
{
    g_CounterState.monsterCount += delta;
}

void SetMonsterCount(uint64_t value)
{
    g_CounterState.monsterCount = value;
}

void SetMonsterGoal(uint64_t value)
{
    g_CounterState.monsterGoal = (value > 0) ? value : 2000;
}

void IncrementPKCount()
{
    g_CounterState.pkCount++;
}

void UpdateTimeCount()
{
    g_CounterState.timeCount++;
}

void AddPromotionPoints(uint32_t amount)
{
    g_CounterState.promotionPoints += amount;
}

void AddMileagePoints(uint64_t amount)
{
    g_CounterState.mileagePoints += amount;
}

void ResetAllCounters()
{
    g_CounterState = CounterState();   // Default ctor — tum sayaclar 0
}

//----------------------------------------------------------------------
// Visibility toggle
//----------------------------------------------------------------------

void SetCounterHUDVisible(bool visible)
{
    g_showCounterHUD = visible;
}

bool IsCounterHUDVisible()
{
    return g_showCounterHUD;
}

} // namespace PhoenixImGui
