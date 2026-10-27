/*-----------------------------------------------------------------------------

	PhoenixFramePacer.h

	2026-09-02 O2 — Reborn Option portu: FPS limiti + smooth frame pacing.

	Reborn paritesi (Application.cpp ApplyFrameLimitPacing):
	  - Frame limit: hedef FPS'e gore ana dongude bekleme (0 = limitsiz)
	  - Smooth pacing: timeBeginPeriod(1) ile 1ms zamanlayici cozunurlugu +
	    Sleep tabanli bekleme; kapaliyken kaba Sleep/spin karisimi
	  - VSync ON iken limit>refresh degerlerinin etkisi swapchain'de sinirlanir

	Phoenix_ApplyRenewalRuntimeOptions(): UserSet\OptionRenewal.set dosyasini
	okuyup pacer + render runtime ayarlarini (VSync, brightness, linear filter,
	texture cache) startup'ta uygular — Option menusu hic acilmasa da gecerli.

-----------------------------------------------------------------------------*/

#pragma once

// Hedef FPS (0 = limitsiz). Menuden veya startup'tan cagrilir.
void Phoenix_Pacer_SetFrameLimit(int fps);
int  Phoenix_Pacer_GetFrameLimit();

// Smooth pacing: timeBeginPeriod(1) yonetimi (acikken 1ms cozunurluk).
void Phoenix_Pacer_SetSmoothPacing(bool bOn);
bool Phoenix_Pacer_GetSmoothPacing();

// 2026-09-03 plan F5 — VSync/pacer etkilesim matrisi. VSync durumu her
// degistiginde cagrilir (startup apply + option menu toggle). VSync ON iken
// Present(1) zaten refresh'e kilitler; pacer yalnizca limit < refresh ise
// devreye girer (aksi halde iki mekanizma cakisip duzensiz tempo yaratir
// ve 120/144 secimleri "calismiyor" hissi verir).
void Phoenix_Pacer_NotifyVSync(bool bOn);

// Ana dongude her frame sonunda cagrilir (CWinMain::Loop).
void Phoenix_Pacer_Pace();

// OptionRenewal.set -> pacer + render runtime ayarlari (startup apply).
void Phoenix_ApplyRenewalRuntimeOptions();

// 2026-09-03 plan F6 — ADV bit6 "Skip intro media" koprusu (Reborn
// Application::ShouldSkipIntroMedia paritesi). Startup'ta OptionRenewal.set
// AdvFlags'ten okunur; menu toggle'inda canli guncellenir. Tuketici:
// COpeningUpdate::PlayMPG (MODE_OPENING kisa devre -> MODE_MAINMENU).
// NOT: ADV bit 1-4 (NPC names / chain effects / spell flashes / UI anim)
// Reborn'da da persist-only — orada da runtime tuketicisi yok (dogrulandi,
// Application.cpp 2326-2335: yalniz bit0/5/6 sabit tanimli). Parity geregi
// Phoenix'te de persist-only birakildi.
bool Phoenix_ShouldSkipIntroMedia();
void Phoenix_SetSkipIntroMedia(bool bSkip);
