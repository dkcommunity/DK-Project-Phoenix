//#define __3D_IMAGE_OBJECT__					// by sonee
//----------------------------------------------------------------------
// MTopViewDraw.cpp
//----------------------------------------------------------------------
//
// 복잡한 Draw 함수 부분만 따로 빼놓는다.
// 작업하기가 영 불편해서 
//----------------------------------------------------------------------
#include "Client_PCH.h"
#pragma warning(disable:4786)

#include "DX3D.h"
#include <math.h>
#include <list>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include "MZone.h"
#include "MCreature.h"
#include "MFakeCreature.h"
#include "MPlayer.h"
#include "MItem.h"
#include "MImageObject.h"
#include "MAnimationObject.h"
#include "MShadowAnimationObject.h"
#include "DarkAssert.h"

#include "MMovingEffect.h"
#include "MScreenEffect.h"
#include "MTopView.h" 
#include "MItemTable.h"
#include "MCreatureTable.h"
#include "MWeather.h"
#include "CMessageArray.h"
#include "DXLib.h"
#include "SP.h"
#include "FL2.h"
//#include "2D.h"
#include "ClientConfig.h"
#include "VS_UI_BASE.h"
#include "VS_UI_Mouse_Pointer.h"
#include "UIFunction.h"
#include "AddonDef.h"
#include "MZoneTable.h"
#include "MGameTime.h"
#include "UserOption.h"
#include "UserInformation.h"
#include "SpriteIDDef.h"
#include "MGameStringTable.h"
//#include "MZoneInfo.h"
#include "MObjectSelector.h"
#include "MPortal.h"
#include "EffectSpriteTypeDef.h"
#include "MScreenEffectManager.h"
#include "MEffectSpriteTypeTable.h"
#include "Profiler.h"
//#include "MFileDef.h"
#include "Properties.h"
#include "MEventManager.h"
#include "DebugInfo.h"
#include "MItemOptionTable.h"
#include "Client.h"
#include "SkillDef.h"
#include "vs_ui_item.h"
#include "MTopViewDraw.inl"

extern int defaultSkinColor;
extern int defaultCoatColor;
extern int defaultTrouserColor;

namespace {
	POINT pointTemp;
	RECT rect;
};

enum
{
	PHOENIX_SHADOW_LOOP_DARKNESS = 1,
	PHOENIX_SHADOW_LOOP_GUNFIELD = 2,
	PHOENIX_SHADOW_LOOP_FAKE = 3,
	PHOENIX_SHADOW_LOOP_PLAYER = 4,
	PHOENIX_SHADOW_LOOP_OUTPUT_MAP = 5
};

enum
{
	PHOENIX_SHADOW_SUB_DRAW_CREATURE_SHADOW = 0,
	PHOENIX_SHADOW_SUB_SLAYER = 1,
	PHOENIX_SHADOW_SUB_VAMPIRE = 2,
	PHOENIX_SHADOW_SUB_OUSTERS = 3,
	PHOENIX_SHADOW_SUB_ADV_SLAYER = 4,
	PHOENIX_SHADOW_SUB_ADV_VAMPIRE = 5,
	PHOENIX_SHADOW_SUB_ADV_OUSTERS = 6,
	PHOENIX_SHADOW_SUB_COUNT = 7
};

namespace {

const int PHOENIX_SHADOW_MAX_UNIQUE_CREATURES = 4096;
const char* const kPhoenixShadowSubNames[PHOENIX_SHADOW_SUB_COUNT] =
{
	"DrawCreatureShadow",
	"DrawShadowSlayer",
	"DrawShadowVampire",
	"DrawShadowOusters",
	"DrawShadowAdvSlayer",
	"DrawShadowAdvVampire",
	"DrawShadowAdvOusters"
};

struct PhoenixShadowPerfFrame
{
	bool active;
	int tileSearch;
	unsigned int creatureCount;
	unsigned int visibleTileCount;
	unsigned int shadowDrawCalls;
	unsigned int uniqueCreatureShadows;
	unsigned int duplicateShadowCalls;
	unsigned int maxDuplicatePerCreature;
	unsigned int uniqueOverflow;
	unsigned int surfaceUnlockLockPairs;
	unsigned int shadowBlits;
	unsigned int darknessCalls;
	unsigned int gunfieldCalls;
	unsigned int fakeCalls;
	unsigned int playerCalls;
	unsigned int outputMapCalls;
	unsigned int creatureIds[PHOENIX_SHADOW_MAX_UNIQUE_CREATURES];
	unsigned short creatureDrawCounts[PHOENIX_SHADOW_MAX_UNIQUE_CREATURES];
	unsigned __int64 beginTick;
	double totalMs;
	double subMs[PHOENIX_SHADOW_SUB_COUNT];
};

struct PhoenixShadowPerfPeriod
{
	unsigned int frames;
	unsigned __int64 sumShadowDrawCalls;
	unsigned __int64 sumUniqueCreatureShadows;
	unsigned __int64 sumDuplicateShadowCalls;
	unsigned __int64 sumSurfaceUnlockLockPairs;
	unsigned __int64 sumShadowBlits;
	double sumShadowMs;
	double subMs[PHOENIX_SHADOW_SUB_COUNT];
	double maxShadowMs;
	PhoenixShadowPerfFrame spikeFrame;
};

PhoenixShadowPerfFrame g_PhoenixShadowFrame;
PhoenixShadowPerfPeriod g_PhoenixShadowPeriod;
double g_PhoenixShadowTicksToMs = 0.0;
DWORD g_PhoenixShadowLastProbeCheckMs = 0;
DWORD g_PhoenixShadowLastFlushMs = 0;
bool g_PhoenixShadowProbeEnabled = false;

bool Phoenix_ShadowPerf_FileExists(const char* path)
{
	FILE* fp = fopen(path, "rb");
	if (!fp)
		return false;
	fclose(fp);
	return true;
}

bool Phoenix_ShadowPerf_IsEnabled()
{
	// Shadow perf probe production'da dosya uretmesin.
	return false;

	DWORD now = GetTickCount();
	if (g_PhoenixShadowLastProbeCheckMs == 0 || now - g_PhoenixShadowLastProbeCheckMs >= 1000)
	{
		g_PhoenixShadowProbeEnabled =
			Phoenix_ShadowPerf_FileExists("shadow_perf_probe.on") ||
			Phoenix_ShadowPerf_FileExists("frame_perf_probe.on");
		g_PhoenixShadowLastProbeCheckMs = now;
	}
	return g_PhoenixShadowProbeEnabled;
}

void Phoenix_ShadowPerf_InitTimer()
{
	if (g_PhoenixShadowTicksToMs != 0.0)
		return;

	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	g_PhoenixShadowTicksToMs = freq.QuadPart ? (1000.0 / (double)freq.QuadPart) : 0.0;
}

unsigned __int64 Phoenix_ShadowPerf_Now()
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	return (unsigned __int64)now.QuadPart;
}

double Phoenix_ShadowPerf_TicksToMs(unsigned __int64 ticks)
{
	Phoenix_ShadowPerf_InitTimer();
	return (double)ticks * g_PhoenixShadowTicksToMs;
}

struct PhoenixShadowCrashProbeKey
{
	int body;
	int requestedAction;
	int tableAction;
	int direction;
	int frame;
	const char* reason;
};

bool Phoenix_ShadowCrashProbe_AlreadyLogged(int body, int requestedAction,
	int tableAction, int direction, int frame, const char* reason)
{
	const int kMaxKeys = 128;
	static PhoenixShadowCrashProbeKey s_keys[kMaxKeys];
	static int s_keyCount = 0;

	for (int i = 0; i < s_keyCount; ++i)
	{
		if (s_keys[i].body == body &&
			s_keys[i].requestedAction == requestedAction &&
			s_keys[i].tableAction == tableAction &&
			s_keys[i].direction == direction &&
			s_keys[i].frame == frame &&
			strcmp(s_keys[i].reason, reason) == 0)
			return true;
	}

	if (s_keyCount < kMaxKeys)
	{
		s_keys[s_keyCount].body = body;
		s_keys[s_keyCount].requestedAction = requestedAction;
		s_keys[s_keyCount].tableAction = tableAction;
		s_keys[s_keyCount].direction = direction;
		s_keys[s_keyCount].frame = frame;
		s_keys[s_keyCount].reason = reason;
		++s_keyCount;
	}

	return false;
}

// 2026-09-04 LIVE hazirlik: shadow lookup hata path'i oyuncu makinesinde
// Client.exe yanina shadow_crash_probe.csv olusturuyordu (guard yoktu).
// Oyuncular hicbir log/csv gormemeli — flag default 0. Cagri yerleri
// korunuyor (kural 4.1); yeni shadow debug'inda 1 yap, rebuild.
#ifndef PHOENIX_SHADOW_CRASH_PROBE_ENABLED
#define PHOENIX_SHADOW_CRASH_PROBE_ENABLED 0
#endif

void Phoenix_ShadowCrashProbe_WriteRisk(const MCreature* pCreature,
	const char* shadowFunc, int body, int requestedAction, int tableAction,
	int direction, int frame, int actionValid, int directionValid,
	int frameValid, const void* resourcePtr, int resourceLoaded,
	int frameCount, const char* reason)
{
#if !PHOENIX_SHADOW_CRASH_PROBE_ENABLED
	(void)pCreature; (void)shadowFunc; (void)body; (void)requestedAction;
	(void)tableAction; (void)direction; (void)frame; (void)actionValid;
	(void)directionValid; (void)frameValid; (void)resourcePtr;
	(void)resourceLoaded; (void)frameCount; (void)reason;
	return;
#else
	if (Phoenix_ShadowCrashProbe_AlreadyLogged(body, requestedAction,
		tableAction, direction, frame, reason))
		return;

	const bool fileAlreadyExists = Phoenix_ShadowPerf_FileExists("shadow_crash_probe.csv");
	FILE* fp = fopen("shadow_crash_probe.csv", "a");
	if (!fp)
		return;

	if (!fileAlreadyExists)
	{
		fprintf(fp,
			"frame_no,tick,creature_ptr,creature_id,creature_class,creature_type,"
			"creature_action,passed_action,direction,frame,shadow_func,"
			"shadow_resource_key,shadow_table_index,action_valid,direction_valid,"
			"frame_valid,resource_ptr,resource_loaded,frame_count,crash_risk_reason\n");
	}

	fprintf(fp,
		"%lu,%lu,%p,%lu,%d,%d,%d,%d,%d,%d,%s,%d,%d,%d,%d,%d,%p,%d,%d,%s\n",
		(unsigned long)g_CurrentFrame,
		GetTickCount(),
		(const void*)pCreature,
		pCreature ? (unsigned long)pCreature->GetID() : 0,
		pCreature ? (int)pCreature->GetClassType() : -1,
		pCreature ? (int)pCreature->GetCreatureType() : -1,
		pCreature ? (int)pCreature->GetAction() : -1,
		requestedAction,
		direction,
		frame,
		shadowFunc,
		body,
		tableAction,
		actionValid,
		directionValid,
		frameValid,
		resourcePtr,
		resourceLoaded,
		frameCount,
		reason);

	fclose(fp);
#endif
}

bool Phoenix_ShadowLookupFrameArray(CCreatureFramePack& framePack,
	int body, int action, int direction, int frame, FRAME_ARRAY** ppFrames,
	int* pFrameCount, int* pActionValid, int* pDirectionValid,
	int* pFrameValid, const char** ppReason)
{
	if (ppFrames)
		*ppFrames = NULL;
	if (pFrameCount)
		*pFrameCount = 0;
	if (pActionValid)
		*pActionValid = 0;
	if (pDirectionValid)
		*pDirectionValid = 0;
	if (pFrameValid)
		*pFrameValid = 0;
	if (ppReason)
		*ppReason = "ok";

	if (body < 0 || body >= (int)framePack.GetSize())
	{
		if (ppReason)
			*ppReason = "body_out_of_range";
		return false;
	}

	ACTION_FRAME_ARRAY& actions = framePack[body];
	if (action < 0 || action >= (int)actions.GetSize())
	{
		if (ppReason)
			*ppReason = "action_out_of_range";
		return false;
	}
	if (pActionValid)
		*pActionValid = 1;

	DIRECTION_FRAME_ARRAY& directions = actions[action];
	if (direction < 0 || direction >= (int)directions.GetSize())
	{
		if (ppReason)
			*ppReason = "direction_out_of_range";
		return false;
	}
	if (pDirectionValid)
		*pDirectionValid = 1;

	FRAME_ARRAY& frames = directions[direction];
	const int frameCount = (int)frames.GetSize();
	if (pFrameCount)
		*pFrameCount = frameCount;

	if (frame < 0 || frame >= frameCount)
	{
		if (ppReason)
			*ppReason = "frame_out_of_range";
		return false;
	}
	if (pFrameValid)
		*pFrameValid = 1;

	if (ppFrames)
		*ppFrames = &frames;
	return true;
}

// Addon shadow FPK erisimi — out-of-range durumda crash yerine probe yazar.
static bool Phoenix_ShadowLookupAddonFrameOrLog(
	const MCreature* pCreature,
	const char* shadowFunc,
	CCreatureFramePack& framePack,
	int clothesIndex,
	int action,
	int direction,
	int frame,
	FRAME_ARRAY** ppFA,
	int* pActionValid,
	int* pDirectionValid,
	int* pFrameValid,
	int* pFrameCount)
{
	int frameCount = 0;
	int actionValid = 0;
	int directionValid = 0;
	int frameValid = 0;
	const char* reason = "ok";

	if (Phoenix_ShadowLookupFrameArray(framePack, clothesIndex, action,
		direction, frame, ppFA, &frameCount, &actionValid, &directionValid,
		&frameValid, &reason))
	{
		if (pActionValid)
			*pActionValid = actionValid;
		if (pDirectionValid)
			*pDirectionValid = directionValid;
		if (pFrameValid)
			*pFrameValid = frameValid;
		if (pFrameCount)
			*pFrameCount = frameCount;
		return true;
	}

	Phoenix_ShadowCrashProbe_WriteRisk(pCreature, shadowFunc, clothesIndex,
		action, action, direction, frame, actionValid, directionValid,
		frameValid, ppFA ? *ppFA : NULL, 0, frameCount, reason);
	return false;
}

void Phoenix_ShadowPerf_ResetPeriod()
{
	memset(&g_PhoenixShadowPeriod, 0, sizeof(g_PhoenixShadowPeriod));
}

void Phoenix_ShadowPerf_AddCurrentFrameToPeriod()
{
	PhoenixShadowPerfFrame& frame = g_PhoenixShadowFrame;
	PhoenixShadowPerfPeriod& period = g_PhoenixShadowPeriod;

	period.frames++;
	period.sumShadowDrawCalls += frame.shadowDrawCalls;
	period.sumUniqueCreatureShadows += frame.uniqueCreatureShadows;
	period.sumDuplicateShadowCalls += frame.duplicateShadowCalls;
	period.sumSurfaceUnlockLockPairs += frame.surfaceUnlockLockPairs;
	period.sumShadowBlits += frame.shadowBlits;
	period.sumShadowMs += frame.totalMs;

	for (int i = 0; i < PHOENIX_SHADOW_SUB_COUNT; ++i)
		period.subMs[i] += frame.subMs[i];

	if (period.frames == 1 || frame.totalMs > period.maxShadowMs)
	{
		period.maxShadowMs = frame.totalMs;
		period.spikeFrame = frame;
	}
}

void Phoenix_ShadowPerf_SelectTopSubFunctions(const PhoenixShadowPerfFrame& frame, int outIndexes[5])
{
	for (int i = 0; i < 5; ++i)
		outIndexes[i] = -1;

	for (int slot = 0; slot < 5; ++slot)
	{
		int bestIndex = -1;
		double bestMs = -1.0;

		for (int i = 0; i < PHOENIX_SHADOW_SUB_COUNT; ++i)
		{
			bool alreadySelected = false;
			for (int selected = 0; selected < slot; ++selected)
			{
				if (outIndexes[selected] == i)
				{
					alreadySelected = true;
					break;
				}
			}

			if (!alreadySelected && frame.subMs[i] > bestMs)
			{
				bestMs = frame.subMs[i];
				bestIndex = i;
			}
		}

		outIndexes[slot] = bestIndex;
	}
}

void Phoenix_ShadowPerf_WriteCsv()
{
	// Shadow perf probe artik shadow_perf.csv dosyasi uretmesin.
	Phoenix_ShadowPerf_ResetPeriod();
	return;

	PhoenixShadowPerfPeriod& period = g_PhoenixShadowPeriod;
	if (period.frames == 0)
		return;

	const bool fileAlreadyExists = Phoenix_ShadowPerf_FileExists("shadow_perf.csv");
	FILE* fp = fopen("shadow_perf.csv", "a");
	if (!fp)
		return;

	if (!fileAlreadyExists)
	{
		fprintf(fp,
			"tick,frames,"
			"avg_shadow_draw_calls,max_shadow_draw_calls,"
			"avg_unique_creature_shadows,max_unique_creature_shadows,"
			"avg_duplicate_shadow_calls,max_duplicate_shadow_calls,max_duplicate_per_creature,"
			"avg_surface_unlock_lock,max_surface_unlock_lock,"
			"avg_shadow_blits,max_shadow_blits,"
			"sum_shadow_ms,avg_shadow_ms,max_frame_shadow_ms,"
			"spike_creature_count,spike_visible_tile_count,spike_tile_search,"
			"spike_shadow_draw_calls,spike_unique_creature_shadows,spike_duplicate_shadow_calls,spike_unique_overflow,"
			"spike_surface_unlock_lock,spike_shadow_blits,"
			"spike_darkness_calls,spike_gunfield_calls,spike_fake_calls,spike_player_calls,spike_outputmap_calls,"
			"sum_drawcreatureshadow_ms,sum_slayer_ms,sum_vampire_ms,sum_ousters_ms,"
			"sum_adv_slayer_ms,sum_adv_vampire_ms,sum_adv_ousters_ms,"
			"top1_name,top1_ms,top2_name,top2_ms,top3_name,top3_ms,top4_name,top4_ms,top5_name,top5_ms\n");
	}

	const PhoenixShadowPerfFrame& spike = period.spikeFrame;
	const double invFrames = period.frames ? (1.0 / (double)period.frames) : 0.0;
	int top[5];
	Phoenix_ShadowPerf_SelectTopSubFunctions(spike, top);

	fprintf(fp,
		"%lu,%u,"
		"%.2f,%u,"
		"%.2f,%u,"
		"%.2f,%u,%u,"
		"%.2f,%u,"
		"%.2f,%u,"
		"%.4f,%.4f,%.4f,"
		"%u,%u,%d,"
		"%u,%u,%u,%u,"
		"%u,%u,"
		"%u,%u,%u,%u,%u,"
		"%.4f,%.4f,%.4f,%.4f,"
		"%.4f,%.4f,%.4f,"
		"%s,%.4f,%s,%.4f,%s,%.4f,%s,%.4f,%s,%.4f\n",
		GetTickCount(), period.frames,
		(double)period.sumShadowDrawCalls * invFrames, spike.shadowDrawCalls,
		(double)period.sumUniqueCreatureShadows * invFrames, spike.uniqueCreatureShadows,
		(double)period.sumDuplicateShadowCalls * invFrames, spike.duplicateShadowCalls, spike.maxDuplicatePerCreature,
		(double)period.sumSurfaceUnlockLockPairs * invFrames, spike.surfaceUnlockLockPairs,
		(double)period.sumShadowBlits * invFrames, spike.shadowBlits,
		period.sumShadowMs, period.sumShadowMs * invFrames, period.maxShadowMs,
		spike.creatureCount, spike.visibleTileCount, spike.tileSearch,
		spike.shadowDrawCalls, spike.uniqueCreatureShadows, spike.duplicateShadowCalls, spike.uniqueOverflow,
		spike.surfaceUnlockLockPairs, spike.shadowBlits,
		spike.darknessCalls, spike.gunfieldCalls, spike.fakeCalls, spike.playerCalls, spike.outputMapCalls,
		period.subMs[PHOENIX_SHADOW_SUB_DRAW_CREATURE_SHADOW],
		period.subMs[PHOENIX_SHADOW_SUB_SLAYER],
		period.subMs[PHOENIX_SHADOW_SUB_VAMPIRE],
		period.subMs[PHOENIX_SHADOW_SUB_OUSTERS],
		period.subMs[PHOENIX_SHADOW_SUB_ADV_SLAYER],
		period.subMs[PHOENIX_SHADOW_SUB_ADV_VAMPIRE],
		period.subMs[PHOENIX_SHADOW_SUB_ADV_OUSTERS],
		top[0] >= 0 ? kPhoenixShadowSubNames[top[0]] : "", top[0] >= 0 ? spike.subMs[top[0]] : 0.0,
		top[1] >= 0 ? kPhoenixShadowSubNames[top[1]] : "", top[1] >= 0 ? spike.subMs[top[1]] : 0.0,
		top[2] >= 0 ? kPhoenixShadowSubNames[top[2]] : "", top[2] >= 0 ? spike.subMs[top[2]] : 0.0,
		top[3] >= 0 ? kPhoenixShadowSubNames[top[3]] : "", top[3] >= 0 ? spike.subMs[top[3]] : 0.0,
		top[4] >= 0 ? kPhoenixShadowSubNames[top[4]] : "", top[4] >= 0 ? spike.subMs[top[4]] : 0.0);

	fclose(fp);
	Phoenix_ShadowPerf_ResetPeriod();
}

class PhoenixShadowPerfScope
{
public:
	explicit PhoenixShadowPerfScope(int subFunction)
		: m_subFunction(subFunction), m_beginTick(0)
	{
		if (g_PhoenixShadowFrame.active)
			m_beginTick = Phoenix_ShadowPerf_Now();
	}

	~PhoenixShadowPerfScope()
	{
		if (!g_PhoenixShadowFrame.active || m_beginTick == 0)
			return;
		if (m_subFunction < 0 || m_subFunction >= PHOENIX_SHADOW_SUB_COUNT)
			return;
		g_PhoenixShadowFrame.subMs[m_subFunction] +=
			Phoenix_ShadowPerf_TicksToMs(Phoenix_ShadowPerf_Now() - m_beginTick);
	}

private:
	int m_subFunction;
	unsigned __int64 m_beginTick;
};

} // namespace

extern "C" void Phoenix_ShadowPerf_BeginFrame(int creatureCount, int visibleTileCount, int tileSearch)
{
	if (!Phoenix_ShadowPerf_IsEnabled())
	{
		g_PhoenixShadowFrame.active = false;
		return;
	}

	memset(&g_PhoenixShadowFrame, 0, sizeof(g_PhoenixShadowFrame));
	g_PhoenixShadowFrame.active = true;
	g_PhoenixShadowFrame.creatureCount = creatureCount > 0 ? (unsigned int)creatureCount : 0;
	g_PhoenixShadowFrame.visibleTileCount = visibleTileCount > 0 ? (unsigned int)visibleTileCount : 0;
	g_PhoenixShadowFrame.tileSearch = tileSearch;
	g_PhoenixShadowFrame.beginTick = Phoenix_ShadowPerf_Now();
}

extern "C" void Phoenix_ShadowPerf_EndFrame()
{
	if (!g_PhoenixShadowFrame.active)
		return;

	g_PhoenixShadowFrame.totalMs =
		Phoenix_ShadowPerf_TicksToMs(Phoenix_ShadowPerf_Now() - g_PhoenixShadowFrame.beginTick);
	g_PhoenixShadowFrame.active = false;
	Phoenix_ShadowPerf_AddCurrentFrameToPeriod();

	DWORD now = GetTickCount();
	if (g_PhoenixShadowLastFlushMs == 0)
		g_PhoenixShadowLastFlushMs = now;

	if (now - g_PhoenixShadowLastFlushMs >= 1000)
	{
		Phoenix_ShadowPerf_WriteCsv();
		g_PhoenixShadowLastFlushMs = now;
	}
}

extern "C" void Phoenix_ShadowPerf_CountSurfaceUnlockLock()
{
	if (g_PhoenixShadowFrame.active)
		g_PhoenixShadowFrame.surfaceUnlockLockPairs++;
}

extern "C" void Phoenix_ShadowPerf_CountLoopCandidate(int path)
{
	if (!g_PhoenixShadowFrame.active)
		return;

	switch (path)
	{
	case PHOENIX_SHADOW_LOOP_DARKNESS:
		g_PhoenixShadowFrame.darknessCalls++;
		break;
	case PHOENIX_SHADOW_LOOP_GUNFIELD:
		g_PhoenixShadowFrame.gunfieldCalls++;
		break;
	case PHOENIX_SHADOW_LOOP_FAKE:
		g_PhoenixShadowFrame.fakeCalls++;
		break;
	case PHOENIX_SHADOW_LOOP_PLAYER:
		g_PhoenixShadowFrame.playerCalls++;
		break;
	case PHOENIX_SHADOW_LOOP_OUTPUT_MAP:
		g_PhoenixShadowFrame.outputMapCalls++;
		break;
	default:
		break;
	}
}

extern "C" void Phoenix_ShadowPerf_CountBlit()
{
	if (g_PhoenixShadowFrame.active)
		g_PhoenixShadowFrame.shadowBlits++;
}

extern "C" void Phoenix_ShadowPerf_RecordCreatureShadow(unsigned int creatureId)
{
	if (!g_PhoenixShadowFrame.active)
		return;

	PhoenixShadowPerfFrame& frame = g_PhoenixShadowFrame;
	frame.shadowDrawCalls++;

	for (unsigned int i = 0; i < frame.uniqueCreatureShadows; ++i)
	{
		if (frame.creatureIds[i] == creatureId)
		{
			if (frame.creatureDrawCounts[i] < 0xffff)
				frame.creatureDrawCounts[i]++;
			frame.duplicateShadowCalls++;
			if (frame.creatureDrawCounts[i] > frame.maxDuplicatePerCreature)
				frame.maxDuplicatePerCreature = frame.creatureDrawCounts[i];
			return;
		}
	}

	if (frame.uniqueCreatureShadows < PHOENIX_SHADOW_MAX_UNIQUE_CREATURES)
	{
		frame.creatureIds[frame.uniqueCreatureShadows] = creatureId;
		frame.creatureDrawCounts[frame.uniqueCreatureShadows] = 1;
		frame.uniqueCreatureShadows++;
		if (frame.maxDuplicatePerCreature < 1)
			frame.maxDuplicatePerCreature = 1;
	}
	else
	{
		frame.uniqueOverflow++;
	}
}

#define PHOENIX_SHADOW_COUNT_BLIT() Phoenix_ShadowPerf_CountBlit()


int AdvanceComboActionFrameShadowTable[7][3][4] = 
{
	// 슬레이어 남 / 여자 검  
	13,26,39,58, 10,21,32,49, 8,17,26,41 ,
		13,26,39,58, 10,21,32,49, 8,17,26,41 ,

		// 슬레이어 남 / 여자 도
		14,29,44,65, 12,25,38,57, 10,21,32,49 ,
		14,29,44,65, 12,25,38,57, 10,21,32,49 ,

		// 벰파이어 남 / 여자
		12,25,38,57, 10,21,32,49, 8,17,26,41,  
		12,26,40,59, 10,21,32,49, 8,17,26,41,  

		// 아우스터  
		12,26,40,59, 10,21,32,49, 8,17,26,41 ,
};



int GetAdvancementPartFromItemClass( ITEM_CLASS itemClass , TYPE_FRAMEID frameID);
int ConvAdvancementSlayerActionFromSlayerAction( int CurAction, MCreatureWear* pCreatureWear );
int ConvAdvancementOustersActionFromOusterAction( int CurAction, bool bChakram );
int GetAdvancementVampireActionFromVampireAction( int CurAction, const MCreature* pCreature );
BYTE GetCreatureActionCountMax( const MCreature* pCreature, int action );



//by viva

int GetAdvancementVampireAction171[];
int ConvAdvancementSlayerActionFromSlayerAction171(int CurAction, MCreatureWear* pCreatureWear);
int GetAdvancementPartFromItemClass171( ITEM_CLASS itemClass, TYPE_FRAMEID frameID, int itemType);

int	GetAdvancementOustersAction171[];
//----------------------------------------------------------------
// Draw CreatureShadow
//----------------------------------------------------------------
// 2D/3D와 관련해서...
// Lock / Unlock은 외부에서 해줘야 한다.
//----------------------------------------------------------------
void
MTopView::DrawCreatureShadow(POINT* pPoint, MCreature* pCreature)
{
	PhoenixShadowPerfScope shadowPerfScope(PHOENIX_SHADOW_SUB_DRAW_CREATURE_SHADOW);
	Phoenix_ShadowPerf_RecordCreatureShadow((unsigned int)pCreature->GetID());

	if(g_pEventManager->GetEventByFlag(EVENTFLAG_NOT_DRAW_CREATURE_SHADOW)
		|| pCreature->IsGhost(1) && pCreature->IsGhost(2) && pCreature->IsGhost(4))
		return;

	if(pCreature->IsOusters() && pCreature->IsPlayer() && g_pPlayer->IsOusters() == false && pCreature->IsInGroundElemental())
	{
		return;
	}

	if(pCreature->IsSkipShadow())
	{
		return;
	}
	int type = pCreature->GetCreatureType();

	if(type>1989)
	{
		return;
	}
	// EFFECTSTATUS_GHOST 에 걸려있으면 남도 못보고 나도 못본다
	if(pCreature->HasEffectStatus(EFFECTSTATUS_GHOST))return;

	// 2004, 6, 7 sobeit add start - 펫일 경우 주인인 다크니스 안에 있으면 안보이게..
	if(pCreature->IsFakeCreature())
	{
		MCreature *OwnerCreature = g_pZone->GetCreature(((MFakeCreature*)pCreature)->GetOwnerID());
		if(OwnerCreature)
		{
			if(OwnerCreature->IsInDarkness())
				return;
		} 
	}
	// 2004, 6, 7 sobeit add end

	if(
		(g_pPlayer->HasEffectStatus(EFFECTSTATUS_YELLOW_POISON_TO_CREATURE) || 
		g_pPlayer->HasEffectStatus(EFFECTSTATUS_FLARE ) ||
		g_pPlayer->HasEffectStatus(EFFECTSTATUS_BLINDNESS))
		&& pCreature != g_pPlayer 
		)
	{
		// 거리에 따라서 출력 여부를 결정한다.
		int sx,sy,ex,ey;

		sx = g_pPlayer->GetX() - 1;
		ex = g_pPlayer->GetX() + 1;

		sy = g_pPlayer->GetY() - 1;
		ey = g_pPlayer->GetY() + 1;

		if( pCreature->GetX() < sx || pCreature->GetX() > ex ||
			pCreature->GetY() < sy || pCreature->GetY() > ey )
			return;
	}

#ifdef OUTPUT_DEBUG_DRAW_PROCESS
	DEBUG_ADD( "Start DrawCreatureShadow" );
#endif

	bool bBlendingShadow = false;
	bool bSlayerPet_ShowTurret = false;

	int body, action, direction, frame, creature_type, frameCount;

	frameCount  = pCreature->GetCreatureFrameCount();

	for(int FrameIndex = 0; FrameIndex< frameCount ; FrameIndex++)
	{
		action		= pCreature->GetAction();		
		body		= pCreature->GetCreatureFrameID(FrameIndex);
		direction	= pCreature->GetDirection();
		frame		= pCreature->GetFrame();
		creature_type = pCreature->GetCreatureType();


		if ( pCreature->IsPlayer() && pCreature->IsSlayer())

		{

			const MCreatureWear::ADDON_INFO& info =  ((MCreatureWear*)pCreature)->GetAddonInfo(ADDON_MOTOR);



			if ( info.bAddon )

			{

				MString strName = (*g_pItemTable)[info.ItemClass][info.ItemType].HName;

				std::string strSTDName = strName.GetString();



				if ( strSTDName.find("왕폭곬苦") != -1 && frame != 0 )//왕폭곬苦칡辜났緞綾

				{

					if ( frame == 3 )

						frame = frame - 1;

				}





			}

		} 
		// 2004, 7, 23 sobeit modify start
		switch(creature_type)
		{
		case 672:
			if(action == ACTION_MOVE )
			{
				frame = (g_CurrentFrame + pCreature->GetID()) % 11;	
			}
			break;
		case 717:
		case 721:
		case 723:// 질드레 or or 질드레 분신 or 각성 질드레
			if( pCreature->HasEffectStatus( EFFECTSTATUS_GDR_FLOATING )/* && action == ACTION_MOVE */)
			{
				action = ACTION_VAMPIRE_DRAIN;
				frame = 6 + (frame & 0x07);		// 흡혈 동작에서 반복 frame의 시작이 6이다.		
			}
			if(pCreature->IsDead())
			{
				if(pCreature->GetActionCount()>=pCreature->GetActionCountMax())
				{
					if(creature_type == 723)
						frame = 46; // 무릎꿇은 상태로..나둔당..
				}
			}
			break;
		case 722:// 질드레 얼음 석상 - 그림자 없음
			return;
		case 636:// 생선가게 주인 - 퀘스트용 일단 누워있게..나중에 분석점 하고 수정하자..
			{
				action = 6;
				frame = 14;
			}
			break;
		case 698:// 대지정령
			{
				if(!pCreature->HasEffectStatus(EFFECTSTATUS_COMA) && !pCreature->IsDead())
				{
					action = ACTION_STAND;
				}
				else
					action = ACTION_DIE;

				direction = 2;
			}
			break;
		case 433:// 테페즈 관
			{
				if(!pCreature->HasEffectStatus(EFFECTSTATUS_COMA) && !pCreature->IsDead())
					action = ACTION_STAND;
				else
					action = ACTION_DIE;

				direction = 1;
			}
			break;
		case 674:
			bBlendingShadow = true;
			break;
		case 675:
			{
				action = ACTION_MAGIC;
				direction = 3;
				frame = 0;
				bBlendingShadow= true;
			}
			break;
		case 702:
		case 703:
		case 704:
		case 1562:
			if( action == ACTION_STAND || action == ACTION_MOVE )
				bSlayerPet_ShowTurret = true;
			break;
		case 730:
		case 731:
		case 732: // 웨이포인트 3종
			{
				if(pCreature->IsDead()) return;
				action = ACTION_STAND;
				frame = 0;
				direction = 2;
				bBlendingShadow= true;
			}
			break;
		case 743:
		case 744:
		case 745:
		case 746:
		case 747:
		case 748: // 장애물 5종
		case 1613:
			{
				if(pCreature->IsDead())
				{
					action = ACTION_DIE;
					direction = 2;
					if(pCreature->GetActionCount()>=pCreature->GetActionCountMax()) return;
				}
				else
				{
					action = ACTION_STAND;
					frame = 0;
					direction = 2;
				}
			}
			break;
		case 2004:
		case 2005:
		case 2006:
		case 2007:
			{
				if(pCreature->IsDead())
				{
					action = ACTION_DIE;
					direction = 0;
					if(pCreature->GetActionCount()>=pCreature->GetActionCountMax()) return;
				}
				else
				{
					action = ACTION_STAND;
					frame = 0;
					direction = 0;
				}
			}
			break;
		case 1704: // 뒈掉裂쳔-璘塘
		case 1714: // 뒈掉裂쳔櫓-1
		//case 1730:
			{
				if(pCreature->IsDead())
				{
					action = ACTION_DIE;
					direction = 2;
				}
				else
				{
					action = ACTION_STAND;
					direction = 2;
				}
			}
			break;
		case 738: // 플레져 스테이션
		case 739: // 랜드 마인
		case 741: // 오토 터렛
			{ 
				//			if(pCreature->IsDead()) return;
				//			if(pCreature->HasEffectStatus(EFFECTSTATUS_HIDE_TO_ATTACKER)) // 공성전시 공격측은 안보임
				//			{
				//				if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_1) || 
				//					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_2) || 
				//					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_3) || 
				//					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_4) || 
				//					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_5) )
				//					return;
				//			}
				action = ACTION_STAND;
				frame = 0;
				direction = 2;
				bBlendingShadow= true;
			}
			break;
		case 740: // 크레이모어
			{
				//			if(pCreature->IsDead()) return;
				//			if(pCreature->HasEffectStatus(EFFECTSTATUS_HIDE_TO_ATTACKER)) // 공성전시 공격측은 안보임
				//			{
				//				if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_1) || 
				//					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_2) || 
				//					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_3) || 
				//					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_4) || 
				//					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_5) )
				//					return;
				//			}
				action = ACTION_STAND;
				frame = 0;
				direction = 1;
			}
		case 734: // 길드 타워
			{
				direction = 1;
				if(pCreature->IsAlive())
				{
					action = ACTION_STAND;
					frame = 0;
				}
			}
			break;
		case 753:// 서먼 고어 그랜드
			{
				direction = 2;
			//	if(pCreature->IsDead())
			//	{
			//		action = ACTION_DIE;
			//		if(pCreature->GetActionCount()>=pCreature->GetActionCountMax()) return;

			//	}
			}
			break;

		case 1358:// 서먼 고어 그랜드

			{

				direction = 2;

		//		if(pCreature->IsDead())

		//		{

		//			action = ACTION_DIE;

		//			if(pCreature->GetActionCount()>=pCreature->GetActionCountMax()) return;



		//		}

			}

			break;
		case 764:
		case 765: // 그레이트 러피언 2차
		case 891:
		case 2014:
		case 2015:
			if(action == ACTION_DIE && (creature_type == 765 || creature_type == 891 || creature_type == 2015))
				direction = 2;

			if(action == ACTION_DRAINED)
			{
				action = ACTION_ATTACK;
				if(frame>4)
				{
					frame = 4;

					if(g_CurrentFrame&0x01)
					{
						direction = ((direction - 1 ) + 8)%8;
						pCreature->SetCurrentDirection(direction);
					}
				}
			}
			break;
			//20081124 승직 캐릭터가 변신했을때 마법 공격 액션이 들어오면 바꿔 준다.
		case CREATURETYPE_WER_WOLF://긴의빈뗍혤돨땡鱗
			if(action == 113)
			{
				action = ACTION_ATTACK;
				pCreature->SetAction(action);
			}
			break;
		case CREATURETYPE_ROCKET_LUNCHER:// 로켓 런쳐
#if	__CONTENTS(__SECOND_RARE_SKILL)
		case CREATURETYPE_XRL_ROCKET_LUNCHER:
#endif //__SECOND_RARE_SKILL			
			frame = 0;
			action = ACTION_MOVE;
			break;
		}

		bool isSlayerCharacter = pCreature->IsSlayerCharacter();
		bool isOustersCharacter = pCreature->IsOusters() && pCreature->GetCreatureType() != CREATURETYPE_OUSTERS_OPERATOR
			&&	creature_type != 717 && creature_type != 721 && creature_type != 723;

		//----------------------------------------------------------
		// Hallu상태
		//----------------------------------------------------------
		if (pCreature->IsHallu())
		{
			// 여기서 이번 턴의 그림을 결정한다. - -;
			pCreature->DetermineHalluActionFrame();

			isSlayerCharacter = false;
			isOustersCharacter = false;

			body	= pCreature->GetHalluCreatureFrameID();
			action = pCreature->GetHalluAction();
			frame = pCreature->GetHalluFrame();
			creature_type = pCreature->GetHalluCreatureType();
		}

		//////////////////////////////////////////////////////
		// 2005.11.21 by chyaya
		// 할루시네이션에 걸렸을 경우 캐릭터를 몬스터로 처리하기 때문에
		// 승직 캐릭터로 취급하지 않고 일반 캐릭터로 취급해 처리한다.
		// 아래의 변수를 사용하여 하드 코딩했다 -_-;;

		bool bHallu = pCreature->IsHallu();

		//
		//////////////////////////////////////////////////////


		// 성물은 무조건 아래, 크리스마스트리
		if(
			creature_type >= 371 && creature_type <= 376 || 
			creature_type >= 560 && creature_type <= 563 || 
			creature_type == 482 || 
			creature_type >= 526 && creature_type <= 549 || 
			creature_type >= 550 && creature_type <= 557 || 
			creature_type == 650 ||
			creature_type >= 660 && creature_type <= 669 ||
			creature_type == 672 ||
			creature_type == 673 ||
			creature_type == 808 ||	// 미클리스는 무조건 아래
			creature_type >= 810 && creature_type <= 812 ||
			creature_type == 819	// 눈사람
			)
		{
			direction = 2;
			bBlendingShadow = true;
		}


		// 임시 고정 그림자.
		/*
		if (pCreature->GetID()==g_pPlayer->GetID())
		{
		POINT pointTemp;

		if (pCreature->IsAlive())
		{
		pointTemp.x = pPoint->x - 5;
		pointTemp.y = pPoint->y - 30;
		//pointTemp.y = pPoint->y + cy + pCreature->GetSY();

		//m_pSurface->BltSprite(&pointTemp, &m_EtcSPK[ 1 ]);
		if (CDirect3D::IsHAL())
		{	
		int sprite = 0;

		m_pSurface->Unlock();
		// EffectTexture 설정
		//CDirect3D::GetDevice()->SetTexture(0, m_EffectTPK[sprite].GetSurface());
		CSpriteSurface* pSurface = m_pCreatureShadowManager->GetTexture(sprite);
		CDirect3D::GetDevice()->SetTexture(0, pSurface->GetSurface());

		RECT rect = { pointTemp.x, 
		pointTemp.y, 
		pointTemp.x + m_pCreatureShadowManager->GetWidth(sprite),
		pointTemp.y + m_pCreatureShadowManager->GetHeight(sprite)
		};


		m_TextureEffect.DrawEffect2D( &rect );

		m_pSurface->Lock();			
		}
		else
		{	
		int sprite = 0;
		m_pSurface->BltShadowSpriteDarkness( &pointTemp, &m_CreatureSSPK[sprite], 1 );
		}
		}

		return;
		}
		*/	

		// FAZ 4 (2026-05-05): SpriteTypes[0] OOB guard (SafeSpriteType0 helper).
		// Yeni spawn yaratıkların CreatureTable entry SpriteTypes boş olabilir.
		int _vmpSprT0 = SafeSpriteType0(pCreature->GetCreatureType());
		bool bPlayerVampire =
			(g_pCreatureSpriteTable != NULL && _vmpSprT0 >= 0 && _vmpSprT0 < (int)g_pCreatureSpriteTable->GetSize())
			&& (*g_pCreatureSpriteTable)[_vmpSprT0].IsPlayerVampireOnlySprite()
			//			&& (pCreature==g_pPlayer || pCreature->HasEffectStatus(EFFECTSTATUS_COMA))
			&& pCreature->GetActionCount()>=pCreature->GetActionCountMax()	
			//				&& !pCreature->IsExistAttachEffect()//20090302 이펙트가 coma가 있는데 비어 있을리가 있나요 따라서 제거
			&& pCreature->IsDead();


		bool bTeenVersion = 
			(
			g_pUserInformation->GoreLevel == false
			&& !(
			(creature_type >= 526 && creature_type <= 549) || 
			(
			creature_type >= 371 && creature_type <= 376 || 
			creature_type >= 560 && creature_type <= 563
			) || 
			creature_type == 482 || 
			creature_type == 650 || 
			creature_type == 670 || 
			creature_type == 819 || 
			creature_type == 672 ||
			creature_type == 673 ||
			creature_type == 730 || // 웨이 포인트1
			creature_type == 731 || // 웨이 포인트2
			creature_type == 732 || // 웨이 포인트3
			creature_type == 636 // 생선 가게 아저씨
			)	// 성물 // 크리스마스트리
			|| (creature_type >= 377 && creature_type <= 386)
			|| creature_type == 480 // 다크 가디언은 먼지로 표시
			)
			&&
			(
			pCreature->GetActionCount()>=pCreature->GetActionCountMax()/2
			&& pCreature->IsDead()
			)
			&&
			(
			creature_type < 431		// 바토리
			|| creature_type > 436	// 테페즈
			);


		//---------------------------------------------------------
		// PC Vampire인 경우 죽으면 '먼지'로 표현한다.
		//
		// player는 죽어도 아이템으로 변하지 않기 때문에..
		// 틴버전에선 모든 시체를 '먼지'로 표현한다.
		//---------------------------------------------------------	
		if (bPlayerVampire || bTeenVersion)
		{
			if(pCreature->IsSlayer())
			{
				CSprite *pSprite = &m_EtcSPK[ SPRITEID_SLAYER_CROSS_SHADOW ];
				// 좌표 보정
				POINT pointTemp;
				pointTemp.x = pPoint->x - 5;
				pointTemp.y = pPoint->y;
				PHOENIX_SHADOW_COUNT_BLIT();
				m_pSurface->BltSprite(&pointTemp, pSprite);
			}

#ifdef OUTPUT_DEBUG_DRAW_PROCESS
			DEBUG_ADD("EndDrawCreatureShadow-S");
#endif

			return;		
		}

		//------------------------------------------------
		// Slayer인 경우 Darkness 안을 볼 수 없다.
		//------------------------------------------------
		if (pCreature->IsInDarkness() && !pCreature->IsNPC() && !g_pPlayer->IsVampire()
			&& g_pPlayer!=pCreature && (!g_pPlayer->HasEffectStatus( EFFECTSTATUS_LIGHTNESS)|| g_pZone->GetID() == 3001) )
		{
#ifdef OUTPUT_DEBUG_DRAW_PROCESS
			DEBUG_ADD("EndDrawCreatureSha-D");
#endif

			return;
		}

		POINT pointTemp;


		//------------------------------------------------
		// 땅 속에 묻힌 애들은 그림자 없다.
		//// 투명 상태에서는 그림자 없다 
		//------------------------------------------------
		bool bPerfectCasket = pCreature->IsInCasket() && pCreature->GetCasketCount()==0;
		if (pCreature->IsUndergroundCreature()
			// 포탈 이동
			|| pCreature->IsCutHeight()
			// 관에 완전히 들어가는 경우
			|| bPerfectCasket
			//|| pCreature->GetInvisibleCount()==64
			)
		{
#ifdef OUTPUT_DEBUG_DRAW_PROCESS
			DEBUG_ADD( "End DrawCreatureShadow" );
#endif

			//----------------------------------------------------
			// 관 출력
			//----------------------------------------------------
			if (bPerfectCasket)
			{
				// 그림자는 +1
				int casketType = pCreature->GetCasketType();
				int casketID = SPRITEID_CASKET_1 + pCreature->GetCasketType()*2 + 1;

				if (casketID < m_EtcSPK.GetSize())
				{
					CSprite* pSpriteImage = &m_EtcSPK[casketID-1];
					CSprite* pSprite = &m_EtcSPK[casketID];

					if (pSprite!=NULL)
					{
						// tile의 중심에서 세운다.
						pointTemp.x = pPoint->x + TILE_X_HALF - (pSpriteImage->GetWidth()>>1);
						pointTemp.y = pPoint->y + TILE_Y - pSpriteImage->GetHeight();

						const POINT shadowPoint[] =
						{
							{ -27, 28 },
							{ -27, 23 },
							{ -31, 28 }
						};

						pointTemp.x += shadowPoint[casketType].x;
						pointTemp.y += shadowPoint[casketType].y;

						PHOENIX_SHADOW_COUNT_BLIT();
						m_pSurface->BltSprite(&pointTemp, pSprite);
					}
				}
			}


#ifdef OUTPUT_DEBUG_DRAW_PROCESS
			DEBUG_ADD("EndDrawCreature-C");
#endif

			return;
		}

		//register int i;

		//----------------------------------------------------------
		// 머리가 없는 경우
		//----------------------------------------------------------
		if (!pCreature->HasHead() 
			&& pCreature->GetActionCount()==pCreature->GetActionCountMax())
		{	
			if (action==ACTION_DIE)
			{
				// 실제로는..마지막 frame + 1이다.
				//frame	= (*g_pCreatureTable)[creature_type].GetActionCount( action );
				frame	= GetCreatureActionCountMax( pCreature, action );
			}
		}

		// Teen버전인 경우 죽는 동작 애니메이션 안함
		// 처음엔 마지막프레임전까지의 동작만 0으로 세팅했는데 저 위쪽에서 재로 찍을 경우 return해버려서 의미없음
		if(g_pUserInformation->GoreLevel == false && action == ACTION_DIE
			&&
			(
			creature_type < 431		// 바토리
			|| creature_type > 436	// 테페즈
			)
			)
		{
			frame = 0;
		}

		// Creature가 존재하는 높이만큼 빼준다.

		// 그림자는 안 빼준다.
		//pPoint->y -= pCreature->GetZ();

		//------------------------------------------------
		//
		//       일반적인 Creature 출력
		//
		//------------------------------------------------

		//------------------------------------------------
		//
		//         Body출력
		//
		//------------------------------------------------
		//		if (!isSlayerCharacter && !isOustersCharacter && body != FRAMEID_NULL)
		//		if(pCreature->IsExistBody())
		if (body != FRAMEID_NULL)
		{
			bool bBat = pCreature->GetCreatureType() == CREATURETYPE_BAT;
			bool bWolf = pCreature->GetCreatureType() == CREATURETYPE_WOLF;
			bool bWerWolf = pCreature->GetCreatureType() == CREATURETYPE_WER_WOLF;//긴의緞綾槻벎
			bool bGhost = pCreature->GetCreatureType() == CREATURETYPE_VAMPIRE_GHOST;
#if __CONTENTS(__FAST_TRANSFORTER)
			bool bFlitterMous = pCreature->GetCreatureType()==CREATURETYPE_FLITTERMOUSE;
			bool bNede = pCreature->GetCreatureType()==CREATURETYPE_NEDE;
			bool bKltl = pCreature->GetCreatureType()==CREATURETYPE_KLTL;
#endif //__FAST_TRANSFORTER			
#if __CONTENTS(__SECOND_TRANSFORTER)
			bool bShapeOfDemon = pCreature->GetCreatureType()==CREATURETYPE_SHAPE_OF_DEMON;

#endif //__SECOND_TRANSFORTER

			bool bTransform = bBat || bWolf || bWerWolf || bHallu || bGhost
#if __CONTENTS(__FAST_TRANSFORTER)
				|| bFlitterMous
								|| bNede
								|| bKltl
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
				|| bShapeOfDemon

#endif //__SECOND_TRANSFORTER
				;

			if(pCreature->IsAdvancementClass() && !bTransform)
			{
				DrawShadowAdvancementClassVampireCharacter(
					pPoint, pCreature, action, direction, frame, body,
					bBlendingShadow, bSlayerPet_ShowTurret);
			}
			else
			{
				bool player = pCreature->IsPlayerOnly() &&
					!pCreature->IsOperator()	&&
					!bTransform;

				DrawShadowVampireCharacter(pPoint, pCreature, action, direction, frame, body, player,
					bBlendingShadow, bSlayerPet_ShowTurret);
			}

		}	

		//----------------------------------------
		//
		//            복장 출력
		//
		//----------------------------------------
		// 모든 입을 수 있는 종류의 옷 중에서 
		// 입고 있는 옷을 확인해서 출력해줘야 한다.

		//if (pCreature->IsWear())
		if (isSlayerCharacter)
		{
			if(pCreature->IsRide_Fast_Transforter())
				return;
			else if( pCreature->IsAdvancementClass() && !bHallu)
				DrawShadowAdvancementClassSlayerCharacter( pPoint, pCreature, action, direction, frame );
			else
				DrawShadowSlayerCharacter( pPoint, pCreature, action, direction, frame );
		}
		else if(isOustersCharacter)
		{
			if( pCreature->IsAdvancementClass() && !bHallu)
				DrawShadowAdvancementClassOustersCharacter( pPoint, pCreature, action, direction, frame );
			else
				DrawShadowOustersCharacter( pPoint, pCreature, action, direction, frame );

		}
	}
#ifdef OUTPUT_DEBUG_DRAW_PROCESS
	DEBUG_ADD( "End DrawCreatureShadow" );
#endif


}

void	MTopView::DrawShadowSlayerCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame )
{
	PhoenixShadowPerfScope shadowPerfScope(PHOENIX_SHADOW_SUB_SLAYER);

	if(pCreature->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
	{
		//frame = pCreature->GetInstallTurretCount();
		if(ACTION_STAND == action)
		{
			direction = 2;
			frame = pCreature->GetInstallTurretCount();
		}
		const int body = 222;
		const int requestedAction = action;
		int tableAction = action;
		FRAME_ARRAY* pFA = NULL;
		int frameCount = 0;
		int actionValid = 0;
		int directionValid = 0;
		int frameValid = 0;
		const char* reason = "ok";

		if (!Phoenix_ShadowLookupFrameArray(m_CreatureShadowFPK, body, tableAction,
			direction, frame, &pFA, &frameCount, &actionValid, &directionValid,
			&frameValid, &reason))
		{
			if (reason != NULL && strcmp(reason, "action_out_of_range") == 0)
			{
				tableAction = ACTION_ATTACK;
				FRAME_ARRAY* pFallbackFA = NULL;
				int fallbackFrameCount = 0;
				int fallbackActionValid = 0;
				int fallbackDirectionValid = 0;
				int fallbackFrameValid = 0;
				const char* fallbackReason = "ok";
				if (Phoenix_ShadowLookupFrameArray(m_CreatureShadowFPK, body,
					tableAction, direction, frame, &pFallbackFA, &fallbackFrameCount,
					&fallbackActionValid, &fallbackDirectionValid, &fallbackFrameValid,
					&fallbackReason))
				{
					Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
						"DrawShadowSlayerCharacter", body,
						requestedAction, tableAction, direction, frame, 0,
						fallbackDirectionValid, fallbackFrameValid, pFallbackFA, 1,
						fallbackFrameCount, "action_out_of_range_fallback_attack");
					pFA = pFallbackFA;
					frameCount = fallbackFrameCount;
					actionValid = fallbackActionValid;
					directionValid = fallbackDirectionValid;
					frameValid = fallbackFrameValid;
				}
				else
				{
					Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
						"DrawShadowSlayerCharacter", body,
						requestedAction, tableAction, direction, frame, 0,
						fallbackDirectionValid, fallbackFrameValid, pFallbackFA, 0,
						fallbackFrameCount, fallbackReason);
					return;
				}
			}
			else
			{
				Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
					"DrawShadowSlayerCharacter", body,
					requestedAction, tableAction, direction, frame, actionValid,
					directionValid, frameValid, pFA, 0, frameCount, reason);
				return;
			}
		}

		if (pFA != NULL && pFA->GetSize() > frame)
		{
			CFrame&	Frame = (*pFA)[frame];

			int sprite = Frame.GetSpriteID(),	//m_CreatureFPK[body][action][direction][frame].GetSpriteID(),
				cx		= Frame.GetCX(),	//m_CreatureFPK[body][action][direction][frame].GetCX(),
				cy		= Frame.GetCY();	//m_CreatureFPK[body][action][direction][frame].GetCY();

			if (sprite < 0 || sprite >= (int)m_CreatureSSPK.GetSize())
			{
				Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
					"DrawShadowSlayerCharacter", body,
					requestedAction, tableAction, direction, frame, actionValid,
					directionValid, frameValid, pFA, 0, frameCount,
					"shadow_sprite_out_of_range");
				return;
			}
			CShadowSprite* pSprite = &GetCreatureShadowSprite(sprite, 222);
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();

			PHOENIX_SHADOW_COUNT_BLIT();
			m_pSurface->BltShadowSprite(&pointTemp, pSprite);

		}
	}
	else
	{
		MCreatureWear*	pCreatureWear = (MCreatureWear*)pCreature;

		action = SlayerActionSpecialization(pCreatureWear, action);

		// Creature의 Action에 맞는 add-on을 출력한다.
		//action = pCreature->GetAction();


		//int id = pCreature->GetID();

		WORD clothes;
		BYTE clothesType;
		for (int i=0; i<ADDON_MAX; i++)
		{
			// Creature의 현재 방향에 따라서...
			// 옷을 출력해주는 순서가 다를 수 있다.
			clothesType = MCreatureWear::s_AddonOrder[pCreature->GetDirection()][i];

			// i번째 종류의 옷을 입고 있다면 출력해 준다.
			const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(clothesType);

			if (addonInfo.bAddon)
			{
				clothes = addonInfo.FrameID;

				CCreatureFramePack& addonShadowFPK = pCreature->IsMale() ? m_AddonManShadowFPK : m_AddonWomanShadowFPK;
				CShadowSpritePack&	addonSSPK = pCreature->IsMale() ? m_AddonManSSPK : m_AddonWomanSSPK;

				FRAME_ARRAY* pFA = NULL;
				int frameCount = 0;
				int actionValid = 0;
				int directionValid = 0;
				int frameValid = 0;

				if (Phoenix_ShadowLookupAddonFrameOrLog(pCreature,
					"DrawShadowSlayerCharacter", addonShadowFPK, (int)clothes,
					action, direction, frame, &pFA, &actionValid,
					&directionValid, &frameValid, &frameCount))
				{
					CFrame& Frame = (*pFA)[frame];

					int sprite = Frame.GetSpriteID();
					int cx = Frame.GetCX();
					int cy = Frame.GetCY();

					if (sprite < 0 || sprite >= (int)addonSSPK.GetSize())
					{
						Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
							"DrawShadowSlayerCharacter", (int)clothes,
							action, action, direction, frame, actionValid,
							directionValid, frameValid, pFA, 0, frameCount,
							"shadow_sprite_out_of_range");
					}
					else
					{
						pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
						pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
						CShadowSprite* pSprite = &addonSSPK[sprite];

						if (g_pUserOption->BlendingShadow)
						{
							PHOENIX_SHADOW_COUNT_BLIT();
							m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
						}
						else
						{
							PHOENIX_SHADOW_COUNT_BLIT();
							m_pSurface->BltShadowSprite( &pointTemp, pSprite );
						}
					}
				}
#if __CONTENTS(__FAST_TRANSFORTER)
				//윙바이크 타고 있을때는 다른 파츠를 그려 주지 않는다. 어찌 되도 윙바이크 먼저 검사 할수 밖에 없기 때문에 
				//다른 부위를 그리고 나서 연산하게 되면 어쩌나 하는 걱정은 일단 접어 두자.
				if ( clothesType == ADDON_MOTOR 
					&& addonInfo.ItemClass == ITEM_CLASS_MOTORCYCLE 
					&& addonInfo.ItemType == 8)
					break;
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
				//윙바이크 타고 있을때는 다른 파츠를 그려 주지 않는다. 어찌 되도 윙바이크 먼저 검사 할수 밖에 없기 때문에 
				//다른 부위를 그리고 나서 연산하게 되면 어쩌나 하는 걱정은 일단 접어 두자.
				if ( clothesType == ADDON_MOTOR 
					&& addonInfo.ItemClass == ITEM_CLASS_MOTORCYCLE 
					&& addonInfo.ItemType == 9)
					break;

#endif //__SECOND_TRANSFORTER
			}
		}
	}
}

void	MTopView::DrawShadowVampireCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame, int body, bool player, bool bBlendingShadow , bool bSlayerPet_ShowTurret )
{
	PhoenixShadowPerfScope shadowPerfScope(PHOENIX_SHADOW_SUB_VAMPIRE);

	//////////////////////////////////////////////////////////////////////////
	// Set PC Vampire Coat Start - chyaya 2007.05.15

	int		drawAction	= action;
	int		drawBody	= body;

	if(player)
	{
		MCreatureWear *pCreatureWear = (MCreatureWear *)pCreature;

		const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);

		bool bCoat = ((addonInfo.bAddon) && (addonInfo.ItemClass == ITEM_CLASS_VAMPIRE_COAT));

		if(bCoat)
		{
			drawBody	= addonInfo.FrameID;
		}

		BYTE actionStep = pCreature->GetActionStep();

		drawAction = MCreature::GetVampireActionStep(drawAction, actionStep);
	}

	CCreatureFramePack* pFramePack = NULL;

	if(player)
	{
		pFramePack = &m_VampireShadowFPK;
	}
	else
	{
		pFramePack = &m_CreatureShadowFPK;
	}

	// Set PC Vampire Coat End - chyaya 2007.05.15
	//////////////////////////////////////////////////////////////////////////

	FRAME_ARRAY* pFA = NULL;
	int frameCount = 0;
	int actionValid = 0;
	int directionValid = 0;
	int frameValid = 0;

	if (Phoenix_ShadowLookupAddonFrameOrLog(pCreature,
		"DrawShadowVampireCharacter", *pFramePack, drawBody, drawAction,
		direction, frame, &pFA, &actionValid, &directionValid, &frameValid,
		&frameCount))
	{
		CFrame& Frame = (*pFA)[frame];

		int sprite = Frame.GetSpriteID(),
			cx = Frame.GetCX(),
			cy = Frame.GetCY();

		// 좌표 보정
		pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
		pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();


		CShadowSprite* pSprite = NULL;

		if(player)
		{
			if (sprite < 0 || sprite >= (int)m_VampireSSPK.GetSize())
			{
				Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
					"DrawShadowVampireCharacter", drawBody, drawAction,
					drawAction, direction, frame, actionValid, directionValid,
					frameValid, pFA, 0, frameCount, "shadow_sprite_out_of_range");
			}
			else
			{
				pSprite = &m_VampireSSPK[sprite];
			}
		}
		else
		{
			pSprite = &GetCreatureShadowSprite(sprite, drawBody);
		}

		if (pSprite != NULL)
		{
	//	if (bBlendingShadow)
	//	{
			PHOENIX_SHADOW_COUNT_BLIT();
			m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
	//	}
		}
		//else
		//{
			//m_pSurface->BltShadowSprite( &pointTemp, pSprite );
			//m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
		//	m_pSurface->BltShadowSpriteSmall( &pointTemp, pSprite, 1 );
		//}
	}



	//////////////////////////////////////////////////////////////////////////
	// Draw PC Vampire Weapon Start - chyaya 2007.05.15

	if(player)	
	{
		MCreatureWear *pCreatureWear = dynamic_cast<MCreatureWear *>(pCreature);

		if(pCreatureWear/* && !pCreatureWear->IsGhost(2)*/)
		{
			const MCreatureWear::ADDON_INFO& addonInfoWeapon = pCreatureWear->GetAddonInfo(ADDON_RIGHTHAND);
			bool bWeapon = addonInfoWeapon.bAddon && addonInfoWeapon.ItemClass == ITEM_CLASS_VAMPIRE_WEAPON;

			const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);

			if(bWeapon)
			{
				int drawWeapon = addonInfo.FrameID % 2;

				FRAME_ARRAY* pFA = NULL;
				int frameCount = 0;
				int actionValid = 0;
				int directionValid = 0;
				int frameValid = 0;

				if (Phoenix_ShadowLookupAddonFrameOrLog(pCreature,
					"DrawShadowVampireCharacter", m_VampireShadowFPK, drawWeapon,
					drawAction, direction, frame, &pFA, &actionValid,
					&directionValid, &frameValid, &frameCount))
				{
					CFrame& Frame = (*pFA)[frame];
					int sprite = Frame.GetSpriteID();
					int cx = Frame.GetCX();
					int cy = Frame.GetCY();

					if (sprite < 0 || sprite >= (int)m_VampireSSPK.GetSize())
					{
						Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
							"DrawShadowVampireCharacter", drawWeapon, drawAction,
							drawAction, direction, frame, actionValid,
							directionValid, frameValid, pFA, 0, frameCount,
							"shadow_sprite_out_of_range");
					}
					else
					{
						CShadowSprite* pSprite = &m_VampireSSPK[sprite];

						pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
						pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();

						if (bBlendingShadow)
						{
							PHOENIX_SHADOW_COUNT_BLIT();
							m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
						}
						else
						{
							PHOENIX_SHADOW_COUNT_BLIT();
							m_pSurface->BltShadowSprite( &pointTemp, pSprite );
						}
					}

				} // lookup ok

			} // if(bWeapon)

		} // if(pCreatureWear && && !pCreatureWear->IsGhost(2))

	} // if(player)

	// Draw PC Vampire Weapon End - chyaya 2007.05.15
	//////////////////////////////////////////////////////////////////////////




	// 슬레 펫인 경우는 터렛 찍어줘야 된다-ㅅ-;;;; 하드하드..아아-_-/~
	if( bSlayerPet_ShowTurret )
	{
		MFakeCreature *pFakeCreature = (MFakeCreature *)pCreature;
		BYTE direct = pFakeCreature->GetTurretDirection();

		FRAME_ARRAY* pFA = NULL;
		int frameCount = 0;
		int actionValid = 0;
		int directionValid = 0;
		int frameValid = 0;
		const char* reason = "ok";
		const int turretAction = ACTION_VAMPIRE_DRAIN;
		const int turretDirection = 2;
		const char* turretFunc = "DrawShadowVampireCharacter_turret";

		if (!Phoenix_ShadowLookupFrameArray(m_CreatureShadowFPK, body, turretAction,
			turretDirection, (int)direct, &pFA, &frameCount, &actionValid,
			&directionValid, &frameValid, &reason))
		{
			Phoenix_ShadowCrashProbe_WriteRisk(pCreature, turretFunc, body,
				turretAction, turretAction, turretDirection, (int)direct,
				actionValid, directionValid, frameValid, pFA, 0, frameCount, reason);
		}
		else if (pFA != NULL)
		{
		CFrame& Frame = (*pFA)[direct];
		int sprite = Frame.GetSpriteID(),
			cx		= Frame.GetCX(),
			cy		= Frame.GetCY();

		if (sprite < 0 || sprite >= (int)m_CreatureSSPK.GetSize())
		{
			Phoenix_ShadowCrashProbe_WriteRisk(pCreature, turretFunc, body,
				turretAction, turretAction, turretDirection, (int)direct,
				actionValid, directionValid, frameValid, pFA, 0, frameCount,
				"shadow_sprite_out_of_range");
		}
		else
		{

		if( pCreature->GetCreatureType() == 702 )
		{
			POINT pointGap[8] = 
			{
				{ 11, 6 },	// 좌
				{ 8, 2 },	// 좌하
				{ 0, 0 },	// 하
				{ -7, 2 },	// 우하
				{ -10, 6 },	// 우
				{ -7, 10 },	// 우상
				{ 0, 12 },	// 상
				{ 8, 10 },	// 좌상
			};
			cx += pointGap[pFakeCreature->GetDirection()].x;
			cy += pointGap[pFakeCreature->GetDirection()].y;
		}
		if( pFakeCreature->GetTurretDelay() > 0 )
		{
			extern POINT g_DirectionValue[MAX_DIRECTION];
			BYTE direct = (pFakeCreature->GetTurretDirection()/4+2)%8;
			cx -= g_DirectionValue[direct].x*pFakeCreature->GetTurretDelay()*2;
			cy -= g_DirectionValue[direct].y*pFakeCreature->GetTurretDelay()*2;
		}

		// 좌표 보정
		pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
		pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();

		CShadowSprite* pSprite = &GetCreatureShadowSprite(sprite, body);

		if (bBlendingShadow)
		{
			PHOENIX_SHADOW_COUNT_BLIT();
			m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
		}
		else
		{
			PHOENIX_SHADOW_COUNT_BLIT();
			m_pSurface->BltShadowSprite( &pointTemp, pSprite );
		}
		}
		}
	}
}

void	MTopView::DrawShadowOustersCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame )
{
	PhoenixShadowPerfScope shadowPerfScope(PHOENIX_SHADOW_SUB_OUSTERS);

	//  챠크람이 있다
	if(pCreature->HasEffectStatus(EFFECTSTATUS_OusterWings_New_1) || pCreature->HasEffectStatus(EFFECTSTATUS_OusterWings_New_2))

		return;


	MCreatureWear *pCreatureWear = (MCreatureWear *)pCreature;

	const MCreatureWear::ADDON_INFO& addonInfoChakram = pCreatureWear->GetAddonInfo(ADDON_RIGHTHAND);
	bool bChakram = addonInfoChakram.bAddon && addonInfoChakram.ItemClass == ITEM_CLASS_OUSTERS_CHAKRAM;

	int tempAction = action;
	if(bChakram == false)
	{
		switch(tempAction)
		{
		case ACTION_STAND:
			tempAction = ACTION_OUSTERS_STAND;
			break;

		case ACTION_MOVE:
			tempAction = ACTION_OUSTERS_MOVE;
			break;
		}
	}

	// 몸을 찍고 챠크람을 찍는다
	const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);

	if (addonInfo.bAddon && !pCreatureWear->IsGhost(1))
	{
		int clothes = addonInfo.FrameID;

		FRAME_ARRAY* pFA = NULL;
		int frameCount = 0;
		int actionValid = 0;
		int directionValid = 0;
		int frameValid = 0;

		if (Phoenix_ShadowLookupAddonFrameOrLog(pCreature,
			"DrawShadowOustersCharacter", m_OustersShadowFPK, clothes,
			tempAction, direction, frame, &pFA, &actionValid, &directionValid,
			&frameValid, &frameCount))
		{
			CFrame& Frame = (*pFA)[frame];

			int sprite = Frame.GetSpriteID();
			int cx = Frame.GetCX();
			int cy = Frame.GetCY();

			if (sprite < 0 || sprite >= (int)m_OustersSSPK.GetSize())
			{
				Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
					"DrawShadowOustersCharacter", clothes, tempAction,
					tempAction, direction, frame, actionValid, directionValid,
					frameValid, pFA, 0, frameCount, "shadow_sprite_out_of_range");
			}
			else
			{
				pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
				pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();

				CShadowSprite* pSprite = &m_OustersSSPK[sprite];

				if (g_pUserOption->BlendingShadow)
				{
					PHOENIX_SHADOW_COUNT_BLIT();
					m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
				}
				else
				{
					PHOENIX_SHADOW_COUNT_BLIT();
					m_pSurface->BltShadowSprite( &pointTemp, pSprite );
				}
			}
		}
	}
	if (bChakram && !pCreatureWear->IsGhost(2))
	{
		int clothes = addonInfoChakram.FrameID;

		FRAME_ARRAY* pFA = NULL;
		int frameCount = 0;
		int actionValid = 0;
		int directionValid = 0;
		int frameValid = 0;

		if (Phoenix_ShadowLookupAddonFrameOrLog(pCreature,
			"DrawShadowOustersCharacter", m_OustersShadowFPK, clothes,
			tempAction, direction, frame, &pFA, &actionValid, &directionValid,
			&frameValid, &frameCount))
		{
			CFrame& Frame = (*pFA)[frame];
			int sprite = Frame.GetSpriteID();
			int cx = Frame.GetCX();
			int cy = Frame.GetCY();

			if (sprite < 0 || sprite >= (int)m_OustersSSPK.GetSize())
			{
				Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
					"DrawShadowOustersCharacter", clothes, tempAction,
					tempAction, direction, frame, actionValid, directionValid,
					frameValid, pFA, 0, frameCount, "shadow_sprite_out_of_range");
			}
			else
			{
				CShadowSprite* pSprite = &m_OustersSSPK[sprite];

				pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
				pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();

				if (g_pUserOption->BlendingShadow)
				{
					PHOENIX_SHADOW_COUNT_BLIT();
					m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
				}
				else
				{
					PHOENIX_SHADOW_COUNT_BLIT();
					m_pSurface->BltShadowSprite( &pointTemp, pSprite );
				}
			}
		}
	}
}

void	MTopView::DrawShadowAdvancementClassSlayerCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame )
{
	PhoenixShadowPerfScope shadowPerfScope(PHOENIX_SHADOW_SUB_ADV_SLAYER);

	//by viva
	int action_old = action;
	if(pCreature->HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
	{
		//frame = pCreature->GetInstallTurretCount();
		if(ACTION_STAND == action)
		{
			direction = 2;
			frame = pCreature->GetInstallTurretCount();
		}
		const int body = 222;
		const int requestedAction = action;
		int tableAction = action;
		FRAME_ARRAY* pFA = NULL;
		int frameCount = 0;
		int actionValid = 0;
		int directionValid = 0;
		int frameValid = 0;
		const char* reason = "ok";

		if (!Phoenix_ShadowLookupFrameArray(m_CreatureShadowFPK, body, tableAction,
			direction, frame, &pFA, &frameCount, &actionValid, &directionValid,
			&frameValid, &reason))
		{
			if (reason != NULL && strcmp(reason, "action_out_of_range") == 0)
			{
				tableAction = ACTION_ATTACK;
				FRAME_ARRAY* pFallbackFA = NULL;
				int fallbackFrameCount = 0;
				int fallbackActionValid = 0;
				int fallbackDirectionValid = 0;
				int fallbackFrameValid = 0;
				const char* fallbackReason = "ok";
				if (Phoenix_ShadowLookupFrameArray(m_CreatureShadowFPK, body,
					tableAction, direction, frame, &pFallbackFA, &fallbackFrameCount,
					&fallbackActionValid, &fallbackDirectionValid, &fallbackFrameValid,
					&fallbackReason))
				{
					Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
						"DrawShadowAdvancementClassSlayerCharacter", body,
						requestedAction, tableAction, direction, frame, 0,
						fallbackDirectionValid, fallbackFrameValid, pFallbackFA, 1,
						fallbackFrameCount, "action_out_of_range_fallback_attack");
					pFA = pFallbackFA;
					frameCount = fallbackFrameCount;
					actionValid = fallbackActionValid;
					directionValid = fallbackDirectionValid;
					frameValid = fallbackFrameValid;
				}
				else
				{
					Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
						"DrawShadowAdvancementClassSlayerCharacter", body,
						requestedAction, tableAction, direction, frame, 0,
						fallbackDirectionValid, fallbackFrameValid, pFallbackFA, 0,
						fallbackFrameCount, fallbackReason);
					return;
				}
			}
			else
			{
				Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
					"DrawShadowAdvancementClassSlayerCharacter", body,
					requestedAction, tableAction, direction, frame, actionValid,
					directionValid, frameValid, pFA, 0, frameCount, reason);
				return;
			}
		}

		if (pFA != NULL && pFA->GetSize() > frame)
		{
			CFrame&	Frame = (*pFA)[frame];

			int sprite = Frame.GetSpriteID(),	//m_CreatureFPK[body][action][direction][frame].GetSpriteID(),
				cx		= Frame.GetCX(),	//m_CreatureFPK[body][action][direction][frame].GetCX(),
				cy		= Frame.GetCY();	//m_CreatureFPK[body][action][direction][frame].GetCY();

			if (sprite < 0 || sprite >= (int)m_CreatureSSPK.GetSize())
			{
				Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
					"DrawShadowAdvancementClassSlayerCharacter", body,
					requestedAction, tableAction, direction, frame, actionValid,
					directionValid, frameValid, pFA, 0, frameCount,
					"shadow_sprite_out_of_range");
				return;
			}
			CShadowSprite* pSprite = &GetCreatureShadowSprite(sprite, 222);
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();

			PHOENIX_SHADOW_COUNT_BLIT();
			m_pSurface->BltShadowSprite(&pointTemp, pSprite);

		}
	}

	else if (  pCreature->HasEffectStatus(EFFECTSTATUS_INSTALL_MK1) )

	{

		//frame = pCreature->GetInstallTurretCount();

		if(ACTION_STAND == action || action == ACTION_MOVE || action == ACTION_ATTACK )

		{

			direction = pCreature->GetDirection();

			

			frame = 0;//pCreature->GetInstallTurretCount();

		

			const int body = 343;
			FRAME_ARRAY* pFA = NULL;
			int frameCount = 0;
			int actionValid = 0;
			int directionValid = 0;
			int frameValid = 0;
			const char* reason = "ok";

			if (!Phoenix_ShadowLookupFrameArray(m_CreatureShadowFPK, body, action,
				direction, frame, &pFA, &frameCount, &actionValid, &directionValid,
				&frameValid, &reason))
			{
				Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
					"DrawShadowAdvancementClassSlayerCharacter", body, action,
					action, direction, frame, actionValid, directionValid,
					frameValid, pFA, 0, frameCount, reason);
				return;
			}



		//	printf("%s땡鱗槨%d\n", pCreature->GetName(), action);



			if (pFA != NULL && pFA->GetSize() > frame)

			{

				CFrame&	Frame = (*pFA)[frame];



				int sprite = Frame.GetSpriteID(),	//m_CreatureFPK[body][action][direction][frame].GetSpriteID(),

					cx		= Frame.GetCX(),	//m_CreatureFPK[body][action][direction][frame].GetCX(),

					cy		= Frame.GetCY();	//m_CreatureFPK[body][action][direction][frame].GetCY();



				if (sprite < 0 || sprite >= (int)m_CreatureSSPK2.GetSize())
				{
					Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
						"DrawShadowAdvancementClassSlayerCharacter", body, action,
						action, direction, frame, actionValid, directionValid,
						frameValid, pFA, 0, frameCount,
						"shadow_sprite_out_of_range");
					return;
				}
				CShadowSprite* pSprite = &GetCreatureShadowSprite(sprite, 343);



				pointTemp.x = pPoint->x + cx-30;// + pCreature->GetSX();

				pointTemp.y = pPoint->y + cy+26;// + pCreature->GetSY();



				PHOENIX_SHADOW_COUNT_BLIT();
				m_pSurface->BltShadowSprite(&pointTemp, pSprite);



			}

		}

	} // MK1꿎桿
	else
	{
		MCreatureWear*	pCreatureWear = (MCreatureWear*)pCreature;

		action = ConvAdvancementSlayerActionFromSlayerAction( action, dynamic_cast< MCreatureWear* >(pCreature) );

		if( action == -1 )
			return;
		else
			action -= ADVANCEMENT_ACTION_START;

		// 2005.08.12 Sjheon 콤보 스킬 관련 Add
		/*
		int		iComboCnt = pCreature->GetCombo();
		if((action >= (ACTION_ADVANCEMENT_SLAYER_COMBO_SWARD_SLOW - ADVANCEMENT_ACTION_START)) && (iComboCnt < 4 && iComboCnt > 0 ))
		{
		//frame -= 8 ; 
		WORD  fCreatureType	= action - 	(ACTION_ADVANCEMENT_SLAYER_COMBO_SWARD_SLOW  - ADVANCEMENT_ACTION_START)  ; 

		WORD  fWeaponSpeed = pCreature->GetWeaponSpeed() ;

		WORD Frame = AdvanceComboActionFrameShadowTable[fCreatureType][fWeaponSpeed][iComboCnt - 1] + 1 ; 

		if(frame > (Frame - 1) )
		{

		frame = frame  - (Frame - 1) ; // ame_Save - (Frame  - ((iComboCnt -1) *2)); 
		frame += (2 * (iComboCnt - 1)) ; 
		//AdvanceComboCancelFrameTable[fCreatureType][fWeaponSpeed][ComboActionCnt - 1])

		action = action + 6 ; 
		}
		}*/
		// 2005.08.12 Sjheon 콤보 스킬 관련 End

		int clothes;
		BYTE clothesType;

		//by viva slayer

		const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetACAddonInfo( AC_ADDON_BODY );

		bool bNewCoat = addonInfo.bAddon && addonInfo.ItemClass == ITEM_CLASS_COAT &&
							(addonInfo.ItemType == 34 || addonInfo.ItemType == 35 ||	//181

				addonInfo.ItemType == 32 || addonInfo.ItemType == 33 ||		//171

				addonInfo.ItemType == 26 || addonInfo.ItemType == 27 ||		//161

				addonInfo.ItemType == 24 || addonInfo.ItemType == 25		//151

				);
		if(bNewCoat)
		{
			action = ConvAdvancementSlayerActionFromSlayerAction171( action_old, dynamic_cast< MCreatureWear* >(pCreature) );

			if( action == -1 )

				return;

			else

				action -= ADVANCEMENT_ACTION_START;

			CCreatureFramePack& slayerFPK = pCreature->IsMale() ? m_AdvancementSlayerManShadow171FPK : m_AdvancementSlayerWomanShadow171FPK;
			CShadowSpritePack& addonSSPK = pCreature->IsMale() ? m_AdvancementSlayerMan171SSPK : m_AdvancementSlayerWoman171SSPK;

			int Frame_Save = frame ; 
			int Action_Save= action;

			for (int i=0; i<AC_ADDON_MAX; i++)  // sjheon 2005.06.07
			{
				// Creature의 현재 방향에 따라서...
				// 옷을 출력해주는 순서가 다를 수 있다.
				clothesType = MCreatureWear::s_ACAddonOrder[ direction ][ i ];

				// i번째 종류의 옷을 입고 있다면 출력해 준다.
				const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetACAddonInfo(clothesType);

				if (addonInfo.bAddon || i == AC_ADDON_BODY )
				{
					clothes = GetAdvancementPartFromItemClass171( addonInfo.ItemClass , addonInfo.FrameID, addonInfo.ItemType);		

					if( clothes == -1)
						continue;

a1:
					{
						FRAME_ARRAY* pFA = NULL;
						int frameCount = 0;
						int actionValid = 0;
						int directionValid = 0;
						int frameValid = 0;

						if (Phoenix_ShadowLookupAddonFrameOrLog(pCreature,
							"DrawShadowAdvancementClassSlayerCharacter", slayerFPK,
							clothes, action, direction, frame, &pFA, &actionValid,
							&directionValid, &frameValid, &frameCount))
						{
							CFrame& Frame = (*pFA)[frame];

							int sprite = Frame.GetSpriteID();
							int cx = Frame.GetCX();
							int cy = Frame.GetCY();

							if (sprite < 0 || sprite >= (int)addonSSPK.GetSize())
							{
								Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
									"DrawShadowAdvancementClassSlayerCharacter", clothes,
									action, action, direction, frame, actionValid,
									directionValid, frameValid, pFA, 0, frameCount,
									"shadow_sprite_out_of_range");
							}
							else
							{
								pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
								pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
								CShadowSprite* pSprite = &addonSSPK[sprite];

								if (g_pUserOption->BlendingShadow)
								{
									PHOENIX_SHADOW_COUNT_BLIT();
									m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
								}
								else
								{
									PHOENIX_SHADOW_COUNT_BLIT();
									m_pSurface->BltShadowSprite( &pointTemp, pSprite );
								}
							}
						}
					}
#if __CONTENTS(__FAST_TRANSFORTER)
					//윙바이크 타고 있을때는 다른 파츠를 그려 주지 않는다. 어찌 되도 윙바이크 먼저 검사 할수 밖에 없기 때문에 
					//다른 부위를 그리고 나서 연산하게 되면 어쩌나 하는 걱정은 일단 접어 두자.
					if ( clothesType == AC_ADDON_MOTOR 
						&& addonInfo.ItemClass == ITEM_CLASS_MOTORCYCLE 
						&& addonInfo.ItemType == 8)
						break;
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
					//윙바이크 타고 있을때는 다른 파츠를 그려 주지 않는다. 어찌 되도 윙바이크 먼저 검사 할수 밖에 없기 때문에 
					//다른 부위를 그리고 나서 연산하게 되면 어쩌나 하는 걱정은 일단 접어 두자.
					if ( clothesType == AC_ADDON_MOTOR 
						&& addonInfo.ItemClass == ITEM_CLASS_MOTORCYCLE 
						&& addonInfo.ItemType == 9)
						break;

#endif //__SECOND_TRANSFORTER
				}
				if(clothes == 19 || clothes == 20)
				{
					clothes = clothes+2;
					goto a1;
				}
			}

		}
		else
		{


			CCreatureFramePack& slayerFPK = pCreature->IsMale() ? m_AdvancementSlayerManShadowFPK : m_AdvancementSlayerWomanShadowFPK;
			CShadowSpritePack& addonSSPK = pCreature->IsMale() ? m_AdvancementSlayerManSSPK : m_AdvancementSlayerWomanSSPK;

			int Frame_Save = frame ; 
			int Action_Save= action;

			for (int i=0; i<AC_ADDON_MAX; i++)  // sjheon 2005.06.07
			{
				// Creature의 현재 방향에 따라서...
				// 옷을 출력해주는 순서가 다를 수 있다.
				clothesType = MCreatureWear::s_ACAddonOrder[ direction ][ i ];

				// i번째 종류의 옷을 입고 있다면 출력해 준다.
				const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetACAddonInfo(clothesType);

				if (addonInfo.bAddon || i == AC_ADDON_BODY )
				{
					clothes = GetAdvancementPartFromItemClass( addonInfo.ItemClass , addonInfo.FrameID);		

					if( clothes == -1 )
						continue;
					if( clothes == 26)
						continue;

					FRAME_ARRAY* pFA = NULL;
					int frameCount = 0;
					int actionValid = 0;
					int directionValid = 0;
					int frameValid = 0;

					if (Phoenix_ShadowLookupAddonFrameOrLog(pCreature,
						"DrawShadowAdvancementClassSlayerCharacter", slayerFPK,
						clothes, action, direction, frame, &pFA, &actionValid,
						&directionValid, &frameValid, &frameCount))
					{
						CFrame& Frame = (*pFA)[frame];

						int sprite = Frame.GetSpriteID();
						int cx = Frame.GetCX();
						int cy = Frame.GetCY();

						if (sprite < 0 || sprite >= (int)addonSSPK.GetSize())
						{
							Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
								"DrawShadowAdvancementClassSlayerCharacter", clothes,
								action, action, direction, frame, actionValid,
								directionValid, frameValid, pFA, 0, frameCount,
								"shadow_sprite_out_of_range");
						}
						else
						{
							pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
							pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
							CShadowSprite* pSprite = &addonSSPK[sprite];

							if (g_pUserOption->BlendingShadow)
							{
								PHOENIX_SHADOW_COUNT_BLIT();
								m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
							}
							else
							{
								PHOENIX_SHADOW_COUNT_BLIT();
								m_pSurface->BltShadowSprite( &pointTemp, pSprite );
							}
						}
					}
#if __CONTENTS(__FAST_TRANSFORTER)
					//윙바이크 타고 있을때는 다른 파츠를 그려 주지 않는다. 어찌 되도 윙바이크 먼저 검사 할수 밖에 없기 때문에 
					//다른 부위를 그리고 나서 연산하게 되면 어쩌나 하는 걱정은 일단 접어 두자.
					if ( clothesType == AC_ADDON_MOTOR 
						&& addonInfo.ItemClass == ITEM_CLASS_MOTORCYCLE 
						&& addonInfo.ItemType == 8)
						break;
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
					//윙바이크 타고 있을때는 다른 파츠를 그려 주지 않는다. 어찌 되도 윙바이크 먼저 검사 할수 밖에 없기 때문에 
					//다른 부위를 그리고 나서 연산하게 되면 어쩌나 하는 걱정은 일단 접어 두자.
					if ( clothesType == AC_ADDON_MOTOR 
						&& addonInfo.ItemClass == ITEM_CLASS_MOTORCYCLE 
						&& addonInfo.ItemType == 9)
						break;
#endif //__SECOND_TRANSFORTER
				}
			}
		}
	}
}

void	MTopView::DrawShadowAdvancementClassVampireCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame, int body, bool bBlendingShadow , bool bSlayerPet_ShowTurret )
{
	PhoenixShadowPerfScope shadowPerfScope(PHOENIX_SHADOW_SUB_ADV_VAMPIRE);

	//	CCreatureFramePack& advanceVampireFPK = pCreature->IsMale() ? m_AdvancementVampireManShadow171FPK : m_AdvancementVampireWomanShadow171FPK;
	//	CShadowSpritePack& advanceVampireSSPK = pCreature->IsMale() ? m_AdvancementVampireMan171SSPK : m_AdvancementVampireWoman171SSPK;

	action = GetAdvancementVampireActionFromVampireAction( action, pCreature );

	if( action == -1 )
		return;
	else
		action -= ADVANCEMENT_ACTION_START;

	//by viva vampire
	int clothes = 0;
	MCreatureWear *pCreatureWear = (MCreatureWear *)pCreature;

	const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);

	bool bCoat = addonInfo.bAddon && addonInfo.ItemClass == ITEM_CLASS_VAMPIRE_COAT;

	bool bNewCoat = bCoat && (addonInfo.ItemType == 31 || addonInfo.ItemType == 30 ||

				addonInfo.ItemType ==28 || addonInfo.ItemType ==29 ||	//171

		addonInfo.ItemType == 22 || addonInfo.ItemType ==23 ||	//161

		addonInfo.ItemType == 20 || addonInfo.ItemType == 21);	//161



	if(bNewCoat)

	{

		if(addonInfo.ItemType == 30 || addonInfo.ItemType ==31)

			clothes = 1;

		action = GetAdvancementVampireAction171[action];

		if(action == -1)	return;

	}


	CCreatureFramePack& advanceVampireFPK = pCreature->IsMale() ? (bNewCoat ? m_AdvancementVampireManShadow171FPK : m_AdvancementVampireManShadowFPK) : (bNewCoat ? m_AdvancementVampireWomanShadow171FPK : m_AdvancementVampireWomanShadowFPK);

	CShadowSpritePack& advanceVampireSSPK = pCreature->IsMale() ? (bNewCoat ? m_AdvancementVampireMan171SSPK : m_AdvancementVampireManSSPK) : (bNewCoat ? m_AdvancementVampireWoman171SSPK : m_AdvancementVampireWomanSSPK);

	// 2005.08.12 Sjheon 콤보 스킬 관련 Add
	/*
	int		iComboCnt = pCreature->GetCombo();

	if((action >= (ACTION_ADVANCEMENT_COMBO_SLOW - ADVANCEMENT_ACTION_START)) && (iComboCnt < 4 && iComboCnt > 0 ))
	{
	//frame -= 8 ; 
	WORD  fCreatureType	= action - 	(ACTION_ADVANCEMENT_COMBO_SLOW  - ADVANCEMENT_ACTION_START)  ; 

	WORD  fWeaponSpeed = pCreature->GetWeaponSpeed() ;

	WORD Frame = AdvanceComboActionFrameShadowTable[fCreatureType][fWeaponSpeed][iComboCnt - 1] + 1 ; 

	if(frame > (Frame - 1) )
	{
	frame -= Frame  - ((iComboCnt -1) *2); 
	action += 3 ; 
	}
	}*/

	FRAME_ARRAY* pFA = NULL;
	int frameCount = 0;
	int actionValid = 0;
	int directionValid = 0;
	int frameValid = 0;

	if (Phoenix_ShadowLookupAddonFrameOrLog(pCreature,
		"DrawShadowAdvancementClassVampireCharacter", advanceVampireFPK,
		clothes, action, direction, frame, &pFA, &actionValid,
		&directionValid, &frameValid, &frameCount))
	{
		CFrame& Frame = (*pFA)[frame];

		int sprite = Frame.GetSpriteID(),
			cx = Frame.GetCX(),
			cy = Frame.GetCY();

		if (sprite < 0 || sprite >= (int)advanceVampireSSPK.GetSize())
		{
			Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
				"DrawShadowAdvancementClassVampireCharacter", clothes, action,
				action, direction, frame, actionValid, directionValid,
				frameValid, pFA, 0, frameCount, "shadow_sprite_out_of_range");
		}
		else
		{
			// 좌표 보정
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
			CShadowSprite* pSprite = &advanceVampireSSPK[sprite];

			if (bBlendingShadow)
			{
				PHOENIX_SHADOW_COUNT_BLIT();
				m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
			}
			else
			{
				PHOENIX_SHADOW_COUNT_BLIT();
				m_pSurface->BltShadowSprite( &pointTemp, pSprite );
			}
		}
	}		

	bool bWeapon = false;

	bool bBat = pCreature->GetCreatureType() == CREATURETYPE_BAT;
	bool bWolf = pCreature->GetCreatureType() == CREATURETYPE_WOLF;
	bool bWerWolf = pCreature->GetCreatureType() == CREATURETYPE_WER_WOLF;//긴의빈緞綾
	bool bGhost = pCreature->GetCreatureType() == CREATURETYPE_VAMPIRE_GHOST;
#if __CONTENTS(__FAST_TRANSFORTER)
	bool bFlitterMous = pCreature->GetCreatureType()==CREATURETYPE_FLITTERMOUSE;
	bool bNede = pCreature->GetCreatureType()==CREATURETYPE_NEDE;
	bool bKltl = pCreature->GetCreatureType()==CREATURETYPE_KLTL;
#endif //__FAST_TRANSFORTER			
#if __CONTENTS(__SECOND_TRANSFORTER)
	bool bShapeOfDemon = pCreature->GetCreatureType()==CREATURETYPE_SHAPE_OF_DEMON;

#endif //__SECOND_TRANSFORTER

	//if( bWeapon && !bBat && !bWolf && !bWerWolf )
	if( bWeapon && !bBat && !bWolf && !bWerWolf && !bGhost
#if __CONTENTS(__FAST_TRANSFORTER)
		&& !bFlitterMous
		&& !bNede
		&& !bKltl
#endif //__FAST_TRANSFORTER;
#if __CONTENTS(__SECOND_TRANSFORTER)
		&& !bShapeOfDemon

#endif //__SECOND_TRANSFORTER
		)
	{
		FRAME_ARRAY* pFAW = NULL;
		int weaponFrameCount = 0;
		int weaponActionValid = 0;
		int weaponDirectionValid = 0;
		int weaponFrameValid = 0;

		if (Phoenix_ShadowLookupAddonFrameOrLog(pCreature,
			"DrawShadowAdvancementClassVampireCharacter", advanceVampireFPK, 1,
			action, direction, frame, &pFAW, &weaponActionValid,
			&weaponDirectionValid, &weaponFrameValid, &weaponFrameCount))
		{
			CFrame& Frame = (*pFAW)[frame];

			int sprite = Frame.GetSpriteID(),
				cx = Frame.GetCX(),
				cy = Frame.GetCY();

			if (sprite < 0 || sprite >= (int)advanceVampireSSPK.GetSize())
			{
				Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
					"DrawShadowAdvancementClassVampireCharacter", 1, action,
					action, direction, frame, weaponActionValid,
					weaponDirectionValid, weaponFrameValid, pFAW, 0,
					weaponFrameCount, "shadow_sprite_out_of_range");
			}
			else
			{
				// 좌표 보정
				pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
				pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();
				CShadowSprite* pSprite = &advanceVampireSSPK[sprite];

				if (bBlendingShadow)
				{
					PHOENIX_SHADOW_COUNT_BLIT();
					m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
				}
				else
				{
					PHOENIX_SHADOW_COUNT_BLIT();
					m_pSurface->BltShadowSprite( &pointTemp, pSprite );
				}
			}
		}
	}

	// 슬레 펫인 경우는 터렛 찍어줘야 된다-ㅅ-;;;; 하드하드..아아-_-/~
	if( bSlayerPet_ShowTurret )
	{
		MFakeCreature *pFakeCreature = (MFakeCreature *)pCreature;
		BYTE direct = pFakeCreature->GetTurretDirection();

		FRAME_ARRAY* pFA = NULL;
		int frameCount = 0;
		int actionValid = 0;
		int directionValid = 0;
		int frameValid = 0;
		const char* reason = "ok";
		const int turretAction = ACTION_VAMPIRE_DRAIN;
		const int turretDirection = 2;
		const char* turretFunc = "DrawShadowAdvancementClassVampireCharacter_turret";

		if (!Phoenix_ShadowLookupFrameArray(m_CreatureShadowFPK, body, turretAction,
			turretDirection, (int)direct, &pFA, &frameCount, &actionValid,
			&directionValid, &frameValid, &reason))
		{
			Phoenix_ShadowCrashProbe_WriteRisk(pCreature, turretFunc, body,
				turretAction, turretAction, turretDirection, (int)direct,
				actionValid, directionValid, frameValid, pFA, 0, frameCount, reason);
		}
		else if (pFA != NULL)
		{
		CFrame& Frame = (*pFA)[direct];
		int sprite = Frame.GetSpriteID(),
			cx		= Frame.GetCX(),
			cy		= Frame.GetCY();

		if (sprite < 0 || sprite >= (int)m_CreatureSSPK.GetSize())
		{
			Phoenix_ShadowCrashProbe_WriteRisk(pCreature, turretFunc, body,
				turretAction, turretAction, turretDirection, (int)direct,
				actionValid, directionValid, frameValid, pFA, 0, frameCount,
				"shadow_sprite_out_of_range");
		}
		else
		{

		if( pCreature->GetCreatureType() == 702 )
		{
			POINT pointGap[8] = 
			{
				{ 11, 6 },	// 좌
				{ 8, 2 },	// 좌하
				{ 0, 0 },	// 하
				{ -7, 2 },	// 우하
				{ -10, 6 },	// 우
				{ -7, 10 },	// 우상
				{ 0, 12 },	// 상
				{ 8, 10 },	// 좌상
			};
			cx += pointGap[pFakeCreature->GetDirection()].x;
			cy += pointGap[pFakeCreature->GetDirection()].y;
		}
		if( pFakeCreature->GetTurretDelay() > 0 )
		{
			extern POINT g_DirectionValue[MAX_DIRECTION];
			BYTE direct = (pFakeCreature->GetTurretDirection()/4+2)%8;
			cx -= g_DirectionValue[direct].x*pFakeCreature->GetTurretDelay()*2;
			cy -= g_DirectionValue[direct].y*pFakeCreature->GetTurretDelay()*2;
		}

		// 좌표 보정
		pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
		pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();

		CShadowSprite* pSprite = &GetCreatureShadowSprite(sprite, body);

		if (bBlendingShadow)
		{
			PHOENIX_SHADOW_COUNT_BLIT();
			m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
		}
		else
		{
			PHOENIX_SHADOW_COUNT_BLIT();
			m_pSurface->BltShadowSprite( &pointTemp, pSprite );
		}
		}
		}
	}
}

void	MTopView::DrawShadowAdvancementClassOustersCharacter( POINT *pPoint, MCreature* pCreature, int action, int direction, int frame )
{
	PhoenixShadowPerfScope shadowPerfScope(PHOENIX_SHADOW_SUB_ADV_OUSTERS);

	//  챠크람이 있다
	if(pCreature->HasEffectStatus(EFFECTSTATUS_OusterWings_New_1) || pCreature->HasEffectStatus(EFFECTSTATUS_OusterWings_New_2))

		return;


	MCreatureWear *pCreatureWear = (MCreatureWear *)pCreature;

	const MCreatureWear::ADDON_INFO& addonInfoChakram = pCreatureWear->GetAddonInfo(ADDON_RIGHTHAND);
	bool bChakram = addonInfoChakram.bAddon && addonInfoChakram.ItemClass == ITEM_CLASS_OUSTERS_CHAKRAM;
	bool bNewChakram = bChakram && (addonInfoChakram.ItemType == 21 || addonInfoChakram.ItemType == 22);//by viva

	int tempAction = ConvAdvancementOustersActionFromOusterAction( action, bChakram );		

	if( tempAction == -1 )
		return;
	else
		tempAction -= ADVANCEMENT_ACTION_START;

	// 2005.08.12 Sjheon 콤보 스킬 관련 Add
	/*
	int		iComboCnt = pCreature->GetCombo();

	if((action >= (ACTION_ADVANCEMENT_OUSTERS_COMBO_SLOW - ADVANCEMENT_ACTION_START)) && (iComboCnt < 4 && iComboCnt > 0 ))
	{
	//frame -= 8 ; 
	WORD  fCreatureType	= action - 	(ACTION_ADVANCEMENT_OUSTERS_COMBO_SLOW  - ADVANCEMENT_ACTION_START)  ; 

	WORD  fWeaponSpeed = pCreature->GetWeaponSpeed() ;

	WORD Frame = AdvanceComboActionFrameShadowTable[fCreatureType][fWeaponSpeed][iComboCnt - 1] + 1 ; 

	if(frame > (Frame - 1) )
	{
	frame -= Frame  - ((iComboCnt -1) *2); 
	tempAction += 3 ; 
	}
	}*/
	// 2005.08.12 Sjheon 콤보 스킬 관련 End


	// 몸을 찍고 챠크람을 찍는다
	const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);

	//by viva ousters
	bool bNewCoat = addonInfo.ItemClass == ITEM_CLASS_OUSTERS_COAT &&

			(addonInfo.ItemType == 16 || addonInfo.ItemType == 17 ||

	addonInfo.ItemType == 12 || addonInfo.ItemType == 13);

	CCreatureFramePack& oustersFPK = bNewCoat ? m_AdvancementOustersShadow171FPK : m_AdvancementOustersShadowFPK;

	CShadowSpritePack& oustersSSPK = bNewCoat ? m_AdvancementOusters171SSPK : m_AdvancementOustersSSPK;

	if(bNewCoat)

		tempAction = GetAdvancementOustersAction171[tempAction];

	if(tempAction == -1)	return;

	if (addonInfo.bAddon && !pCreatureWear->IsGhost(1))
	{
		//int clothes = addonInfo.FrameID;
		int clothes = 1;

		if(bNewCoat)

			if(addonInfo.ItemType == 17)

				clothes = 3;

			else 

				clothes = 2;

		FRAME_ARRAY* pFA = NULL;
		int frameCount = 0;
		int actionValid = 0;
		int directionValid = 0;
		int frameValid = 0;

		if (Phoenix_ShadowLookupAddonFrameOrLog(pCreature,
			"DrawShadowAdvancementClassOustersCharacter", oustersFPK, clothes,
			tempAction, direction, frame, &pFA, &actionValid, &directionValid,
			&frameValid, &frameCount))
		{
			CFrame& Frame = (*pFA)[frame];

			int sprite = Frame.GetSpriteID();
			int cx = Frame.GetCX();
			int cy = Frame.GetCY();

			if (sprite < 0 || sprite >= (int)oustersSSPK.GetSize())
			{
				Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
					"DrawShadowAdvancementClassOustersCharacter", clothes,
					tempAction, tempAction, direction, frame, actionValid,
					directionValid, frameValid, pFA, 0, frameCount,
					"shadow_sprite_out_of_range");
			}
			else
			{
				pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
				pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();

				CShadowSprite* pSprite = &oustersSSPK[sprite];

				if (g_pUserOption->BlendingShadow)
				{
					PHOENIX_SHADOW_COUNT_BLIT();
					m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
				}
				else
				{
					PHOENIX_SHADOW_COUNT_BLIT();
					m_pSurface->BltShadowSprite( &pointTemp, pSprite );
				}
			}
		}
	}
	if (bChakram && !pCreatureWear->IsGhost(2))
	{
		int clothes = addonInfoChakram.FrameID;

		if(bNewCoat)

			if(bNewChakram)

				clothes = 1;

			else

				clothes = 0;

		else

			clothes = 0;


		FRAME_ARRAY* pFA = NULL;
		int frameCount = 0;
		int actionValid = 0;
		int directionValid = 0;
		int frameValid = 0;

		if (Phoenix_ShadowLookupAddonFrameOrLog(pCreature,
			"DrawShadowAdvancementClassOustersCharacter", oustersFPK, clothes,
			tempAction, direction, frame, &pFA, &actionValid, &directionValid,
			&frameValid, &frameCount))
		{
			CFrame& Frame = (*pFA)[frame];
			int sprite = Frame.GetSpriteID();
			int cx = Frame.GetCX();
			int cy = Frame.GetCY();

			if (sprite < 0 || sprite >= (int)oustersSSPK.GetSize())
			{
				Phoenix_ShadowCrashProbe_WriteRisk(pCreature,
					"DrawShadowAdvancementClassOustersCharacter", clothes,
					tempAction, tempAction, direction, frame, actionValid,
					directionValid, frameValid, pFA, 0, frameCount,
					"shadow_sprite_out_of_range");
			}
			else
			{
				CShadowSprite* pSprite = &oustersSSPK[sprite];

				pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
				pointTemp.y = pPoint->y + cy;// + pCreature->GetSY();

				if (g_pUserOption->BlendingShadow)
				{
					PHOENIX_SHADOW_COUNT_BLIT();
					m_pSurface->BltShadowSpriteDarkness( &pointTemp, pSprite, 1 );
				}
				else
				{
					PHOENIX_SHADOW_COUNT_BLIT();
					m_pSurface->BltShadowSprite( &pointTemp, pSprite );
				}
			}
		}
	}
}
