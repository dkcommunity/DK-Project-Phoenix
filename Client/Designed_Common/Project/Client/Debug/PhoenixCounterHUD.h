//----------------------------------------------------------------------
// PhoenixCounterHUD.h
//----------------------------------------------------------------------
// 5-counter HUD overlay — orijinal DarkEden CHS ekranindaki sag-ust kose
// sayac penceresini ImGui ile yeniden olusturur.
//
// Sayaclar (5 adet):
//   1. Monster Count    — Oldurulen monster sayisi / hedef
//   2. PK Count         — Player kill (PvP) sayisi / hedef
//   3. Time Count       — Oyun suresi (dakika cinsinden) / hedef
//   4. Market Cash      — DianJuan bakiyesi (MMoneyManager)
//   5. Market Points    — JiFen bakiyesi (MMoneyManager)
//
// 2026-05-23 — Counter Audit (counter_system_audit_2026-05-23.md) sonucunda
// 5 sayacin 4'unun Phoenix'te YOK oldugu tespit edildi. Bu HUD penceresi
// SEÇENEK B (UI Mock) olarak hayata gecirildi — tum sayaclar 0'dan baslar,
// gercek tracking icin gelecek seansta server-side implementasyon gerekli.
//
// Mimari:
//   - PhoenixImGui::Render() icinden cagrilir (DrawDebugConsole pattern)
//   - Authentication YOK — HUD tum playerlar tarafindan goruntulenir (HPBAR gibi)
//   - Pozisyon: ilk acilista ekranin sag-ust kosesi (resolution-aware)
//   - Pencere draggable/collapsible; disk persistence yok (imgui.ini yazilmaz)
//   - Auto-load: g_showCounterHUD = true default (oyun girisinde otomatik gorunur)
//
// Veri kaynagi (mevcut):
//   - Tum sayaclar 0 (default constructor)
//   - Increment helper'lar mevcut ama otomatik tetiklenmez
//   - Gelecek seansta: server packet'lerden gerçek deger alinacak veya
//     Phoenix mevcut verileri (quest progress, play time, vb.) ile map edilecek
//
//----------------------------------------------------------------------

#ifndef _PHOENIX_COUNTER_HUD_H_
#define _PHOENIX_COUNTER_HUD_H_

#include <cstdint>

namespace PhoenixImGui {

//----------------------------------------------------------------------
// Counter state — runtime accumulator
//----------------------------------------------------------------------
struct CounterState
{
    uint64_t monsterCount;       // Olduurulen monster sayisi
    uint64_t monsterGoal;        // Hedef (varsayilan 2000, screenshot referansi)
    uint32_t pkCount;            // PK (PvP) sayisi
    uint32_t pkGoal;             // Hedef (varsayilan 100, screenshot referansi)
    uint32_t timeCount;          // Oyun suresi (dakika)
    uint32_t timeGoal;           // Hedef (varsayilan 60, screenshot referansi)
    uint32_t promotionPoints;    // Legacy mock field; HUD Market Cash'i MMoneyManager'dan okur
    uint64_t mileagePoints;      // Legacy mock field; HUD Market Points'i MMoneyManager'dan okur

    CounterState()
        : monsterCount(0), monsterGoal(2000)
        , pkCount(0), pkGoal(100)
        , timeCount(0), timeGoal(60)
        , promotionPoints(0)
        , mileagePoints(0)
    {}
};

// Global counter state — PhoenixCounterHUD.cpp'de tanimli
extern CounterState g_CounterState;

// HUD penceresinin gorunurlugu — default TRUE (HPBAR gibi auto-load)
extern bool g_showCounterHUD;

//----------------------------------------------------------------------
// Counter HUD rendering — PhoenixImGui::Render() icinden cagrilir
//----------------------------------------------------------------------
// p_open: pencereyi kapatma flag'i (NULL = kapatilamaz HUD, mevcut: NULL geciyor)
void DrawCounterHUD(bool* p_open);

//----------------------------------------------------------------------
// Counter update helpers — gelecek seansta server packet'lerden cagrilir
//----------------------------------------------------------------------
// Su an manuel/test amacli kullanilabilir. Increment sonrasi HUD otomatik
// yenilenir (ImGui her frame redraw).

void IncrementMonsterCount(uint64_t delta = 1);
void SetMonsterCount(uint64_t value);
void SetMonsterGoal(uint64_t value);
void IncrementPKCount();
void UpdateTimeCount();              // Her dakika cagrilir (1 dakika += 1)
void AddPromotionPoints(uint32_t amount);
void AddMileagePoints(uint64_t amount);

// Counter reset — yeni sezon/event basinda manuel reset icin
void ResetAllCounters();

// HUD goruunur/gizli toggle — opsiyonel (su an daima acik)
void SetCounterHUDVisible(bool visible);
bool IsCounterHUDVisible();

} // namespace PhoenixImGui

#endif // _PHOENIX_COUNTER_HUD_H_
