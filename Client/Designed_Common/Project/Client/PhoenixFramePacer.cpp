/*-----------------------------------------------------------------------------

	PhoenixFramePacer.cpp

	2026-09-02 O2 — Reborn Option portu: FPS limiti + smooth frame pacing +
	OptionRenewal.set startup apply. Detay: PhoenixFramePacer.h

-----------------------------------------------------------------------------*/

#include <windows.h>
#include <mmsystem.h>
#include <psapi.h>
#include <stdio.h>
#include <string.h>

#include "PhoenixFramePacer.h"
#include "DXLib/IRenderBackend.h"
#include "crash_trace_tool.h"

#pragma comment(lib, "winmm.lib")

namespace
{
	int  g_FrameLimit        = 60;     // 0 = limitsiz
	bool g_bSmoothPacing     = false;
	bool g_bTimerPeriodOn    = false;  // timeBeginPeriod(1) aktif mi
	LONGLONG g_QpcFreq       = 0;
	LONGLONG g_NextFrameQpc  = 0;      // hedef frame baslangici (QPC tick)

	// plan F5 — VSync/pacer etkilesimi: VSync ON iken Present(1) refresh'e
	// kilitler; pacer yalnizca limit < refresh ise anlamli.
	bool g_bVSyncOn          = true;   // startup default'u VSync ON ile ayni
	int  g_RefreshRate       = 0;      // 0 = henuz sorgulanmadi/bilinmiyor

	int QueryDisplayRefreshRate()
	{
		DEVMODE dm;
		memset(&dm, 0, sizeof(dm));
		dm.dmSize = sizeof(dm);
		if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm) &&
		    dm.dmDisplayFrequency > 1)	// 0/1 = "donanim default" (bilinmiyor)
			return (int)dm.dmDisplayFrequency;
		return 0;
	}

	// plan F5 — FPS olcumlu dogrulama: 5 saniyede bir olculen FPS loglanir
	// (dusuk hacim; CT_DONE_ENABLED=0 iken tamamen sessiz).
	LONGLONG g_MeasureStartQpc = 0;
	int      g_MeasureFrames   = 0;

	void MeasureFps(LONGLONG nowQpc)
	{
		g_MeasureFrames++;
		if (g_MeasureStartQpc == 0)
		{
			g_MeasureStartQpc = nowQpc;
			g_MeasureFrames = 0;
			return;
		}
		const LONGLONG elapsed = nowQpc - g_MeasureStartQpc;
		if (elapsed >= g_QpcFreq * 5)
		{
			const double fps = (double)g_MeasureFrames * (double)g_QpcFreq / (double)elapsed;

			// 2026-09-03 R1 — bellek watermark (dump p7816 bad_alloc, 2GB VA
			// tukenmesi): private bytes + toplam VA rezervi 5 sn'de bir
			// loglanir. Bir sizinti varsa trend burada gorunur; LAA sonrasi
			// tavan ~4GB. Maliyet ihmal edilebilir (5 sn'de 2 API cagrisi).
			SIZE_T privMB = 0, vaMB = 0;
			PROCESS_MEMORY_COUNTERS_EX pmc;
			memset(&pmc, 0, sizeof(pmc));
			pmc.cb = sizeof(pmc);
			if (GetProcessMemoryInfo(GetCurrentProcess(),
			                         (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
				privMB = pmc.PrivateUsage >> 20;
			MEMORYSTATUSEX ms;
			memset(&ms, 0, sizeof(ms));
			ms.dwLength = sizeof(ms);
			if (GlobalMemoryStatusEx(&ms))
				vaMB = (SIZE_T)((ms.ullTotalVirtual - ms.ullAvailVirtual) >> 20);

			CT_Done("Pacer: measured FPS=%.1f limit=%d vsync=%d refresh=%d smooth=%d mem(priv=%uMB va=%uMB)",
			        fps, g_FrameLimit, g_bVSyncOn ? 1 : 0, g_RefreshRate,
			        g_bSmoothPacing ? 1 : 0, (unsigned)privMB, (unsigned)vaMB);
			g_MeasureStartQpc = nowQpc;
			g_MeasureFrames = 0;
		}
	}

	// OptionRenewal.set bit tanimlari — VS_UI_OptionRenewal.cpp ile ayni
	// (Reborn Application.cpp flag sirasi). Buraya kopya: UI modulune link
	// bagimliligi olmadan startup'ta okuyabilmek icin.
	const unsigned int kGraphVSyncBit          = 1u << 0;
	const unsigned int kAdv2SmoothPacingBit    = 1u << 0;
	const unsigned int kAdv2SpriteLinearBit    = 1u << 2;
	const unsigned int kAdv2TexCacheHighBit    = 1u << 3;
	const unsigned int kAdv2HqUiScalingBit     = 1u << 5;
	const unsigned int kAdv2PixelPerfectBit    = 1u << 6;

	// VS_UI_OptionRenewal.cpp kDefault* degerleriyle ayni tutulmali.
	const unsigned int kDefaultGraphFlags = 0x000001DFu;	// bit0 (VSync) dahil
	const unsigned int kDefaultAdv2Flags  = kAdv2SpriteLinearBit | kAdv2HqUiScalingBit;

	// plan F6 — ADV bit6 (Skip intro media). Default AdvFlags=0x3F: bit6
	// kapali (intro oynar) — VS_UI_OptionRenewal kDefaultAdvFlags ile ayni.
	const unsigned int kAdvSkipIntroBit   = 1u << 6;
	bool g_bSkipIntroMedia = false;
}

bool Phoenix_ShouldSkipIntroMedia()
{
	return g_bSkipIntroMedia;
}

void Phoenix_SetSkipIntroMedia(bool bSkip)
{
	g_bSkipIntroMedia = bSkip;
}

void Phoenix_Pacer_SetFrameLimit(int fps)
{
	if (fps < 0)
		fps = 0;
	g_FrameLimit = fps;
	g_NextFrameQpc = 0;	// tempo bastan kurulsun
}

int Phoenix_Pacer_GetFrameLimit()
{
	return g_FrameLimit;
}

void Phoenix_Pacer_SetSmoothPacing(bool bOn)
{
	g_bSmoothPacing = bOn;
	if (bOn && !g_bTimerPeriodOn)
	{
		if (timeBeginPeriod(1) == TIMERR_NOERROR)
			g_bTimerPeriodOn = true;
	}
	else if (!bOn && g_bTimerPeriodOn)
	{
		timeEndPeriod(1);
		g_bTimerPeriodOn = false;
	}
}

bool Phoenix_Pacer_GetSmoothPacing()
{
	return g_bSmoothPacing;
}

void Phoenix_Pacer_NotifyVSync(bool bOn)
{
	g_bVSyncOn = bOn;
	// Refresh rate degisebilir (cozunurluk/monitor degisimi) — her VSync
	// degisiminde tazele (UI-tetikli, dusuk frekans; per-frame maliyet yok).
	g_RefreshRate = QueryDisplayRefreshRate();
	g_NextFrameQpc = 0;	// tempo bastan
	CT_Done("Pacer: NotifyVSync on=%d refresh=%d limit=%d",
	        bOn ? 1 : 0, g_RefreshRate, g_FrameLimit);
}

void Phoenix_Pacer_Pace()
{
	if (g_QpcFreq == 0)
	{
		LARGE_INTEGER f;
		QueryPerformanceFrequency(&f);
		g_QpcFreq = f.QuadPart;
		if (g_QpcFreq == 0)
			return;
	}

	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	// plan F5 — olcum tum ciktilardan once (limit kapaliyken de gorunur)
	MeasureFps(now.QuadPart);

	if (g_FrameLimit <= 0)
		return;

	// plan F5 — VSync ON iken Present(1) zaten refresh kadansinda kapiyor.
	// Limit >= refresh ise pacer devre disi: iki bekleme mekanizmasi
	// cakisip frame suresini refresh'in ustune tasiyabiliyor (120/144
	// secimlerinin "calismiyor" gorunmesinin koku). Limit < refresh ise
	// (or. 144Hz monitorde 60 limiti) pacer gecerli kalir.
	if (g_bVSyncOn && g_RefreshRate > 0 && g_FrameLimit >= g_RefreshRate)
		return;

	const LONGLONG target = g_QpcFreq / g_FrameLimit;

	if (g_NextFrameQpc == 0)
	{
		g_NextFrameQpc = now.QuadPart + target;
		return;
	}

	// Hedef zamana kadar bekle: kalan > 2ms ise Sleep (smooth pacing'de 1ms
	// cozunurluk), son dilim Sleep(0) yield ile inceltilir.
	while (now.QuadPart < g_NextFrameQpc)
	{
		LONGLONG remainMs = (g_NextFrameQpc - now.QuadPart) * 1000 / g_QpcFreq;
		if (remainMs > 2)
			Sleep((DWORD)(remainMs - 1));
		else
			Sleep(0);
		QueryPerformanceCounter(&now);
	}

	// Sonraki hedef; uzun stall (loading, pencere tasima) sonrasi frame
	// borcu biriktirmemek icin gecikme 4 frame'i astiysa tempo sifirlanir.
	g_NextFrameQpc += target;
	if (now.QuadPart - g_NextFrameQpc > target * 4)
		g_NextFrameQpc = now.QuadPart + target;
}

void Phoenix_ApplyRenewalRuntimeOptions()
{
	// Defaults — dosya yoksa/ilk kurulum: mevcut davranis (VSync on, 60 fps)
	unsigned int graphFlags = kDefaultGraphFlags;
	unsigned int advFlags   = 0x0000003Fu;	// kDefaultAdvFlags (bit6 kapali)
	unsigned int adv2Flags  = kDefaultAdv2Flags;
	int frameLimit = 60;
	int brightness = 50;

	// CWD = GameData (client calisma dizini) — VS_UI_OptionRenewal ile ayni yol
	FILE* fp = fopen("UserSet\\OptionRenewal.set", "rt");
	if (fp != NULL)
	{
		char line[256];
		while (fgets(line, sizeof(line), fp) != NULL)
		{
			unsigned int u; int v;
			if (sscanf(line, "GraphFlags=%u", &u) == 1)          graphFlags = u;
			else if (sscanf(line, "AdvFlags=%u", &u) == 1)       advFlags   = u;
			else if (sscanf(line, "Adv2Flags=%u", &u) == 1)      adv2Flags  = u;
			else if (sscanf(line, "FrameLimit=%d", &v) == 1)     frameLimit = v;
			else if (sscanf(line, "Brightness=%d", &v) == 1)     brightness = v;
		}
		fclose(fp);
	}

	// plan F6 — Skip intro media (ADV bit6, Reborn paritesi)
	Phoenix_SetSkipIntroMedia((advFlags & kAdvSkipIntroBit) != 0);

	phoenix::render::SetVSyncEnabled((graphFlags & kGraphVSyncBit) != 0);
	Phoenix_Pacer_NotifyVSync((graphFlags & kGraphVSyncBit) != 0);	// plan F5
	phoenix::render::SetUserBrightness(brightness);
	Phoenix_Pacer_SetFrameLimit(frameLimit);
	Phoenix_Pacer_SetSmoothPacing((adv2Flags & kAdv2SmoothPacingBit) != 0);

	// Pixel-perfect linear filtreyi ezer (Reborn ApplyAdvanced2RuntimeOptions
	// paritesi). HQ UI scaling da linear sampler ister.
	const bool bLinear = ((adv2Flags & (kAdv2SpriteLinearBit | kAdv2HqUiScalingBit)) != 0)
	                     && (adv2Flags & kAdv2PixelPerfectBit) == 0;
	phoenix::render::SetSpriteLinearFilter(bLinear);
	phoenix::render::SetSpriteCacheHighBudget((adv2Flags & kAdv2TexCacheHighBit) != 0);

	CT_Done("RenewalRuntimeOptions: vsync=%d bright=%d fpslimit=%d smooth=%d linear=%d cacheHigh=%d",
	        (graphFlags & kGraphVSyncBit) ? 1 : 0, brightness, frameLimit,
	        (adv2Flags & kAdv2SmoothPacingBit) ? 1 : 0, bLinear ? 1 : 0,
	        (adv2Flags & kAdv2TexCacheHighBit) ? 1 : 0);
}
