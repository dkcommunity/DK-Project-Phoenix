// FL2.cpp

#include "client_PCH.h"

#include "FL2.h"
#include "VS_UI_Base.h"
#include "Phoenix_D3D9TextDC.h"  // Fix #17 — D3D9 composition buffer HDC wrapper
#include "PhoenixDWrite.h"       // FULL DX11 gecis Faz T1/T2 — DirectWrite text motoru
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

//-----------------------------------------------------------------------------
// Utilities for IO.
//-----------------------------------------------------------------------------

//
// FL2��?? DC�� ???���� surface? ???��. ?��? ��?��?�� ?��? 
// surface??׻ݫ, offscreen surface��? � ? ??��.
//
// Unicorn edit line widget��? FL2�� ???���� ��?��? ? surface�� غ?�
// ��?��.
//
LPDIRECTDRAWSURFACE7	gpC_fl2_surface = NULL;
HDC gh_FL2_DC = NULL;

// 2026-05-16 Dalga 2 — M1: DC mode bayragi (release path yonlendirici).
// 0=none, 1=DDraw7, 2=D3D9 composition, 3=GDI memory DC. g_FL2_GetDC + frame
// cache temizleyici tarafindan okunur/yazilir. (Eski tanim daha asagidaydi —
// frame cache helper basta lazim oldugu icin yukari tasindi.)
static int g_eFL2_DC_Mode = 0;

// Fix #15-#17 (2026-05-14): D3D9 modunda gpC_fl2_surface NULL kaliyor (g_pLast'in
// DDraw7 m_pDDSurface NULL). Eski FL2 GetDC yolu `gpC_fl2_surface->GetDC()`
// abort firlatiyordu. Fix #17 uc-yolu fallback (DDraw7 / D3D9 composition / GDI
// memory DC) ile cozuldu — g_FL2_GetDC icinde g_eFL2_DC_Mode bayragi ile
// release path yonlendirilir. Eski g_bFL2_DC_FromGDI bayragi obsolete (artik
// kullanilmiyor) ama compile uyumu icin korunuyor.
static bool g_bFL2_DC_FromGDI = false;

static bool Phoenix_IsTurkishCP1254Byte(unsigned char ch)
{
	switch (ch)
	{
		case 0xC7: case 0xD0: case 0xD6: case 0xDC: case 0xDD: case 0xDE:
		case 0xE7: case 0xF0: case 0xF6: case 0xFC: case 0xFD: case 0xFE:
			return true;
		default:
			return false;
	}
}

static bool Phoenix_IsLegacyDBCSLeadByte(unsigned char ch)
{
	if (ch <= 128)
		return false;

	// CP1254 Turkish glyph bytes are single-byte characters, not DBCS lead bytes.
	if (Phoenix_IsTurkishCP1254Byte(ch))
		return false;

	return true;
}

enum
{
    PHOENIX_TEXT_STAGE_SELECTED_CREATURE = 1,
    PHOENIX_TEXT_STAGE_TEXT_LIST = 2,
    PHOENIX_TEXT_STAGE_HP_MODIFY = 3,
    PHOENIX_TEXT_STAGE_CREATURE_NAME_OVER_TEXT = 4,
    PHOENIX_TEXT_STAGE_ITEM_NAME_LIST = 5,
    PHOENIX_TEXT_STAGE_SYSTEM_TEXT = 6,
    PHOENIX_TEXT_STAGE_EVENT_TEXT = 7,
    PHOENIX_TEXT_STAGE_GAME_MESSAGE = 8,
    PHOENIX_TEXT_STAGE_UI_LABEL = 9
};

enum PhoenixTextPerfCategory
{
    PHOENIX_TEXT_CATEGORY_UNKNOWN = 0,
    PHOENIX_TEXT_CATEGORY_UI_LABEL,
    PHOENIX_TEXT_CATEGORY_TOOLTIP,
    PHOENIX_TEXT_CATEGORY_ITEM_SKILL_TOOLTIP,
    PHOENIX_TEXT_CATEGORY_CREATURE_NAME,
    PHOENIX_TEXT_CATEGORY_PLAYER_NAME,
    PHOENIX_TEXT_CATEGORY_NPC_NAME,
    PHOENIX_TEXT_CATEGORY_ITEM_NAME,
    PHOENIX_TEXT_CATEGORY_SELECTED_CREATURE,
    PHOENIX_TEXT_CATEGORY_HP_MODIFY,
    PHOENIX_TEXT_CATEGORY_FLOATING_DAMAGE,
    PHOENIX_TEXT_CATEGORY_TEXT_LIST,
    PHOENIX_TEXT_CATEGORY_SYSTEM_TEXT,
    PHOENIX_TEXT_CATEGORY_EVENT_TEXT,
    PHOENIX_TEXT_CATEGORY_GAME_MESSAGE,
    PHOENIX_TEXT_CATEGORY_DESCRIPTION_HEAVY,
    PHOENIX_TEXT_CATEGORY_DEBUG_TEXT,
    PHOENIX_TEXT_CATEGORY_COUNT
};

static const char* const kPhoenixTextPerfCategoryNames[PHOENIX_TEXT_CATEGORY_COUNT] =
{
    "Unknown",
    "UI_Label",
    "Tooltip",
    "ItemSkillTooltip",
    "CreatureName",
    "PlayerName",
    "NPCName",
    "ItemName",
    "SelectedCreature",
    "HPModify",
    "FloatingDamage",
    "TextList",
    "SystemText",
    "EventText",
    "GameMessage",
    "DescriptionHeavy",
    "DebugText"
};

struct PhoenixTextPerfCategoryFrame
{
    unsigned int calls;
    unsigned int textOutCalls;
    unsigned int drawTextCalls;
    unsigned int measureCalls;
    unsigned int getDcCalls;
    unsigned int realGetDcCalls;
    double drawMs;
    double measureMs;
    double scopeMs;
    unsigned int chars;
    unsigned int repeatedDraws;
};

struct PhoenixTextPerfFrame
{
    unsigned int frameIndex;
    DWORD tick;
    unsigned int printCalls;
    unsigned int textOutCalls;
    unsigned int drawTextCalls;
    unsigned int textMeasureCalls;
    unsigned int getDcLogicalCalls;
    unsigned int releaseDcLogicalCalls;
    unsigned int realGetDcCalls;
    unsigned int realReleaseDcCalls;
    unsigned int dcCacheHits;
    unsigned int dcCacheMisses;
    unsigned int dcCacheFlushes;
    unsigned int dcCacheSurfaceSwitches;
    unsigned int dcCacheActiveAtFrameEnd;
    unsigned int textFrameActiveAtFrameEnd;
    unsigned int textFrameBeginCalls;
    unsigned int textFrameEndCalls;
    unsigned int surfaceUnlockLockPairs;
    unsigned int measureCacheHits;
    unsigned int measureCacheMisses;
    unsigned int measureCacheBypasses;
    unsigned int measureCacheEvictions;
    unsigned int measureCacheEntries;
    double textDrawTotalMs;
    double textMeasureTotalMs;
    double textMeasureCachedMs;
    double textMeasureUncachedMs;
    double getDcAcquireTotalMs;
    double releaseDcTotalMs;
    double totalGdiTextMs;
    double maxSingleTextDrawMs;
    unsigned int charsDrawn;
    unsigned int uniqueStringEstimate;
    unsigned int repeatedStringDrawCount;
    PhoenixTextPerfCategoryFrame categories[PHOENIX_TEXT_CATEGORY_COUNT];
};

static PhoenixTextPerfFrame g_TextPerfFrame = {};
static PhoenixTextPerfFrame g_TextPerfLastFrame = {};
static PhoenixTextPerfFrame g_TextPerfRing[300] = {};
static int g_TextPerfRingHead = 0;
static int g_TextPerfRingCount = 0;
static bool g_TextPerfEnabled = false;
static LARGE_INTEGER g_TextPerfQpcFreq = {};
static unsigned int g_TextPerfFrameIndex = 0;
static int g_TextPerfCategoryStack[16] = {};
static int g_TextPerfCategoryDepth = 0;
static unsigned int g_TextPerfStringHashes[512] = {};

extern "C" bool Phoenix_D3D9_IsFrameInProgress();
static HDC  g_FrameCachedDC      = NULL;
static int  g_FrameCachedMode    = 0;     // 1=DDraw7, 2=D3D9, 3=GDI_Memory
static int  g_FrameAcquireDepth  = 0;     // ic ice cagri sayaci
static int  g_TextFrameDepth     = 0;     // FL2 text-only batch scope derinligi

enum
{
    PHOENIX_MEASURE_KIND_WIDTH = 1,
    PHOENIX_MEASURE_KIND_HEIGHT = 2,
    PHOENIX_MEASURE_KIND_WIDTH_INDEX = 3,
    PHOENIX_MEASURE_CACHE_SIZE = 4096,
    PHOENIX_MEASURE_CACHE_MAX_TEXT = 127
};

struct PhoenixTextMeasureCacheEntry
{
    unsigned int hash;
    HFONT hfont;
    unsigned short kind;
    short index;
    unsigned short len;
    SIZE size;
    char text[PHOENIX_MEASURE_CACHE_MAX_TEXT + 1];
};

static PhoenixTextMeasureCacheEntry g_TextMeasureCache[PHOENIX_MEASURE_CACHE_SIZE] = {};
static unsigned int g_TextMeasureCacheEntries = 0;
static void Phoenix_TextMeasureCache_Clear();

extern "C" bool Phoenix_TextPerf_IsEnabled()
{
    return g_TextPerfEnabled;
}

extern "C" void Phoenix_TextPerf_SetEnabled(bool enabled)
{
    g_TextPerfEnabled = enabled;
    ZeroMemory(&g_TextPerfFrame, sizeof(g_TextPerfFrame));
    ZeroMemory(&g_TextPerfLastFrame, sizeof(g_TextPerfLastFrame));
    ZeroMemory(g_TextPerfRing, sizeof(g_TextPerfRing));
    ZeroMemory(g_TextPerfStringHashes, sizeof(g_TextPerfStringHashes));
    Phoenix_TextMeasureCache_Clear();
    g_TextPerfRingHead = 0;
    g_TextPerfRingCount = 0;
    g_TextPerfCategoryDepth = 0;
}

static double Phoenix_TextPerf_TicksToMs(unsigned __int64 elapsedTicks)
{
    if (g_TextPerfQpcFreq.QuadPart == 0)
        QueryPerformanceFrequency(&g_TextPerfQpcFreq);

    if (g_TextPerfQpcFreq.QuadPart == 0)
        return 0.0;

    return (double)elapsedTicks * 1000.0 / (double)g_TextPerfQpcFreq.QuadPart;
}

extern "C" unsigned __int64 Phoenix_TextPerf_Begin()
{
    if (!g_TextPerfEnabled)
        return 0;

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (unsigned __int64)now.QuadPart;
}

static double Phoenix_TextPerf_EndToMs(unsigned __int64 beginTick)
{
    if (beginTick == 0 || !g_TextPerfEnabled)
        return 0.0;

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return Phoenix_TextPerf_TicksToMs((unsigned __int64)now.QuadPart - beginTick);
}

static int Phoenix_TextPerf_CurrentCategory()
{
    if (g_TextPerfCategoryDepth <= 0)
        return PHOENIX_TEXT_CATEGORY_UNKNOWN;

    int category = g_TextPerfCategoryStack[g_TextPerfCategoryDepth - 1];
    if (category < 0 || category >= PHOENIX_TEXT_CATEGORY_COUNT)
        return PHOENIX_TEXT_CATEGORY_UNKNOWN;

    return category;
}

static int Phoenix_TextPerf_CategoryFromStage(int stage)
{
    switch (stage)
    {
        case PHOENIX_TEXT_STAGE_SELECTED_CREATURE:
            return PHOENIX_TEXT_CATEGORY_SELECTED_CREATURE;
        case PHOENIX_TEXT_STAGE_TEXT_LIST:
            return PHOENIX_TEXT_CATEGORY_TEXT_LIST;
        case PHOENIX_TEXT_STAGE_HP_MODIFY:
            return PHOENIX_TEXT_CATEGORY_HP_MODIFY;
        case PHOENIX_TEXT_STAGE_CREATURE_NAME_OVER_TEXT:
            return PHOENIX_TEXT_CATEGORY_CREATURE_NAME;
        case PHOENIX_TEXT_STAGE_ITEM_NAME_LIST:
            return PHOENIX_TEXT_CATEGORY_ITEM_NAME;
        case PHOENIX_TEXT_STAGE_SYSTEM_TEXT:
            return PHOENIX_TEXT_CATEGORY_SYSTEM_TEXT;
        case PHOENIX_TEXT_STAGE_EVENT_TEXT:
            return PHOENIX_TEXT_CATEGORY_EVENT_TEXT;
        case PHOENIX_TEXT_STAGE_GAME_MESSAGE:
            return PHOENIX_TEXT_CATEGORY_GAME_MESSAGE;
        case PHOENIX_TEXT_STAGE_UI_LABEL:
            return PHOENIX_TEXT_CATEGORY_UI_LABEL;
        default:
            return PHOENIX_TEXT_CATEGORY_UNKNOWN;
    }
}

extern "C" void Phoenix_TextPerf_PushCategory(int stage)
{
    if (!g_TextPerfEnabled)
        return;

    if (g_TextPerfCategoryDepth >= (int)(sizeof(g_TextPerfCategoryStack) / sizeof(g_TextPerfCategoryStack[0])))
        return;

    g_TextPerfCategoryStack[g_TextPerfCategoryDepth++] =
        Phoenix_TextPerf_CategoryFromStage(stage);
}

extern "C" void Phoenix_TextPerf_PopCategory()
{
    if (!g_TextPerfEnabled)
        return;

    if (g_TextPerfCategoryDepth > 0)
        --g_TextPerfCategoryDepth;
}

static PhoenixTextPerfCategoryFrame& Phoenix_TextPerf_CurrentCategoryFrame()
{
    return g_TextPerfFrame.categories[Phoenix_TextPerf_CurrentCategory()];
}

static unsigned int Phoenix_TextPerf_HashString(const char* str, int len)
{
    unsigned int hash = 2166136261u;
    for (int i = 0; i < len; ++i)
    {
        hash ^= (unsigned char)str[i];
        hash *= 16777619u;
    }
    return hash ? hash : 1u;
}

static void Phoenix_TextMeasureCache_Clear()
{
    ZeroMemory(g_TextMeasureCache, sizeof(g_TextMeasureCache));
    g_TextMeasureCacheEntries = 0;
}

static unsigned int Phoenix_TextMeasureCache_Hash(const char* str, int len,
                                                  HFONT hfont, int kind, int index)
{
    unsigned int hash = Phoenix_TextPerf_HashString(str, len);
    hash ^= (unsigned int)(uintptr_t)hfont;
    hash *= 16777619u;
    hash ^= (unsigned int)kind;
    hash *= 16777619u;
    hash ^= (unsigned int)index;
    hash *= 16777619u;
    return hash ? hash : 1u;
}

static bool Phoenix_TextMeasureCache_CanUse(const char* str, int len)
{
    return str != NULL && len >= 0 && len <= PHOENIX_MEASURE_CACHE_MAX_TEXT;
}

static void Phoenix_TextMeasureCache_CountHit(const char* str, int len, double ms)
{
    if (!g_TextPerfEnabled)
        return;

    PhoenixTextPerfFrame& f = g_TextPerfFrame;
    PhoenixTextPerfCategoryFrame& c = Phoenix_TextPerf_CurrentCategoryFrame();

    ++f.textMeasureCalls;
    ++c.measureCalls;
    ++g_TextPerfFrame.measureCacheHits;
    f.textMeasureTotalMs += ms;
    g_TextPerfFrame.textMeasureCachedMs += ms;

    if (len < 0 && str != NULL)
        len = (int)strlen(str);
    if (len > 0)
        c.chars += (unsigned int)len;
}

static void Phoenix_TextMeasureCache_CountMiss()
{
    if (g_TextPerfEnabled)
        ++g_TextPerfFrame.measureCacheMisses;
}

static void Phoenix_TextMeasureCache_CountBypass()
{
    if (g_TextPerfEnabled)
        ++g_TextPerfFrame.measureCacheBypasses;
}

static bool Phoenix_TextMeasureCache_Get(const char* str, int len,
                                         HFONT hfont, int kind, int index,
                                         SIZE* outSize)
{
    if (!Phoenix_TextMeasureCache_CanUse(str, len) || outSize == NULL)
    {
        Phoenix_TextMeasureCache_CountBypass();
        return false;
    }

    const unsigned int hash = Phoenix_TextMeasureCache_Hash(str, len, hfont, kind, index);
    int slot = (int)(hash % PHOENIX_MEASURE_CACHE_SIZE);
    const unsigned __int64 tick = Phoenix_TextPerf_Begin();

    for (int probe = 0; probe < 4; ++probe)
    {
        const PhoenixTextMeasureCacheEntry& entry = g_TextMeasureCache[slot];
        if (entry.hash == 0)
            break;

        if (entry.hash == hash &&
            entry.hfont == hfont &&
            entry.kind == (unsigned short)kind &&
            entry.index == (short)index &&
            entry.len == (unsigned short)len &&
            memcmp(entry.text, str, len) == 0)
        {
            *outSize = entry.size;
            Phoenix_TextMeasureCache_CountHit(str, len, Phoenix_TextPerf_EndToMs(tick));
            return true;
        }

        slot = (slot + 1) % PHOENIX_MEASURE_CACHE_SIZE;
    }

    Phoenix_TextMeasureCache_CountMiss();
    return false;
}

static void Phoenix_TextMeasureCache_Put(const char* str, int len,
                                         HFONT hfont, int kind, int index,
                                         const SIZE& size)
{
    if (!Phoenix_TextMeasureCache_CanUse(str, len))
        return;

    const unsigned int hash = Phoenix_TextMeasureCache_Hash(str, len, hfont, kind, index);
    int slot = (int)(hash % PHOENIX_MEASURE_CACHE_SIZE);
    int target = slot;

    for (int probe = 0; probe < 4; ++probe)
    {
        PhoenixTextMeasureCacheEntry& entry = g_TextMeasureCache[slot];
        if (entry.hash == 0 ||
            (entry.hash == hash &&
             entry.hfont == hfont &&
             entry.kind == (unsigned short)kind &&
             entry.index == (short)index &&
             entry.len == (unsigned short)len &&
             memcmp(entry.text, str, len) == 0))
        {
            target = slot;
            break;
        }
        slot = (slot + 1) % PHOENIX_MEASURE_CACHE_SIZE;
    }

    PhoenixTextMeasureCacheEntry& entry = g_TextMeasureCache[target];
    const bool wasEmpty = (entry.hash == 0);
    if (!wasEmpty && g_TextPerfEnabled)
        ++g_TextPerfFrame.measureCacheEvictions;

    entry.hash = hash;
    entry.hfont = hfont;
    entry.kind = (unsigned short)kind;
    entry.index = (short)index;
    entry.len = (unsigned short)len;
    entry.size = size;
    memcpy(entry.text, str, len);
    entry.text[len] = '\0';

    if (wasEmpty && g_TextMeasureCacheEntries < PHOENIX_MEASURE_CACHE_SIZE)
        ++g_TextMeasureCacheEntries;
}

static void Phoenix_TextPerf_RecordMeasureCacheEntryCount()
{
    if (g_TextPerfEnabled)
        g_TextPerfFrame.measureCacheEntries = g_TextMeasureCacheEntries;
}

static bool Phoenix_TextPerf_RecordStringUse(const char* str, int len)
{
    if (!str || len <= 0)
        return false;

    const unsigned int hash = Phoenix_TextPerf_HashString(str, len);
    const int slotCount = (int)(sizeof(g_TextPerfStringHashes) / sizeof(g_TextPerfStringHashes[0]));
    int slot = (int)(hash % (unsigned int)slotCount);

    for (int probe = 0; probe < 8; ++probe)
    {
        unsigned int& stored = g_TextPerfStringHashes[slot];
        if (stored == hash)
            return true;
        if (stored == 0)
        {
            stored = hash;
            ++g_TextPerfFrame.uniqueStringEstimate;
            return false;
        }
        slot = (slot + 1) % slotCount;
    }

    return false;
}

static void Phoenix_TextPerf_RecordTextDraw(int drawKind, const char* str, int len,
                                            unsigned __int64 beginTick)
{
    if (!g_TextPerfEnabled)
        return;

    const double ms = Phoenix_TextPerf_EndToMs(beginTick);
    PhoenixTextPerfFrame& f = g_TextPerfFrame;
    PhoenixTextPerfCategoryFrame& c = Phoenix_TextPerf_CurrentCategoryFrame();

    ++c.calls;
    if (drawKind == 1)
    {
        ++f.textOutCalls;
        ++c.textOutCalls;
    }
    else
    {
        ++f.drawTextCalls;
        ++c.drawTextCalls;
    }

    if (len < 0 && str != NULL)
        len = (int)strlen(str);
    if (len > 0)
    {
        f.charsDrawn += (unsigned int)len;
        c.chars += (unsigned int)len;
        if (Phoenix_TextPerf_RecordStringUse(str, len))
        {
            ++f.repeatedStringDrawCount;
            ++c.repeatedDraws;
        }
    }

    f.textDrawTotalMs += ms;
    c.drawMs += ms;
    if (ms > f.maxSingleTextDrawMs)
        f.maxSingleTextDrawMs = ms;
}

static void Phoenix_TextPerf_RecordMeasure(const char* str, int len,
                                           unsigned __int64 beginTick)
{
    if (!g_TextPerfEnabled)
        return;

    const double ms = Phoenix_TextPerf_EndToMs(beginTick);
    PhoenixTextPerfFrame& f = g_TextPerfFrame;
    PhoenixTextPerfCategoryFrame& c = Phoenix_TextPerf_CurrentCategoryFrame();

    ++f.textMeasureCalls;
    ++c.measureCalls;
    f.textMeasureTotalMs += ms;
    f.textMeasureUncachedMs += ms;
    c.measureMs += ms;

    if (len < 0 && str != NULL)
        len = (int)strlen(str);
    if (len > 0)
        c.chars += (unsigned int)len;
}

static void Phoenix_TextPerf_AddAcquireTicks(unsigned __int64 beginTick)
{
    if (g_TextPerfEnabled)
        g_TextPerfFrame.getDcAcquireTotalMs += Phoenix_TextPerf_EndToMs(beginTick);
}

static void Phoenix_TextPerf_AddReleaseTicks(unsigned __int64 beginTick)
{
    if (g_TextPerfEnabled)
        g_TextPerfFrame.releaseDcTotalMs += Phoenix_TextPerf_EndToMs(beginTick);
}

// FULL DX11 gecis Faz T2 (2026-09-01): FL2'nin TUM GDI text cagrilari bu 3
// sarmalayicidan gecer — DWrite gecisi tam burada yapilir. text_engine.cfg
// "dwrite" ise once PhoenixDWrite denenir; motor kapali/fail ise ayni cagri
// icinde GDI'ye dusulur (gorsel kayip yok, API/PrintInfo akisi degismez).
static BOOL Phoenix_TextPerf_TextOut(HDC hdc, int x, int y, LPCSTR str, int len)
{
    const unsigned __int64 tick = Phoenix_TextPerf_Begin();
    BOOL result = FALSE;
    if (PhoenixDWrite_IsEnabled())
        result = PhoenixDWrite_TextOut(hdc, x, y, str, len);
    if (!result)
        result = TextOut(hdc, x, y, str, len);
    Phoenix_TextPerf_RecordTextDraw(1, str, len, tick);
    return result;
}

static int Phoenix_TextPerf_DrawText(HDC hdc, LPCSTR str, int len, LPRECT rect, UINT format)
{
    const unsigned __int64 tick = Phoenix_TextPerf_Begin();
    int result = -1;
    if (PhoenixDWrite_IsEnabled())
        result = PhoenixDWrite_DrawText(hdc, str, len, rect, format);
    if (result < 0)
        result = DrawText(hdc, str, len, rect, format);
    Phoenix_TextPerf_RecordTextDraw(2, str, len, tick);
    return result;
}

static BOOL Phoenix_TextPerf_GetTextExtentPoint32(HDC hdc, LPCSTR str, int len, LPSIZE size)
{
    const unsigned __int64 tick = Phoenix_TextPerf_Begin();
    BOOL result = FALSE;
    if (PhoenixDWrite_IsEnabled())
        result = PhoenixDWrite_GetTextExtent(hdc, str, len, size);
    if (!result)
        result = GetTextExtentPoint32(hdc, str, len, size);
    Phoenix_TextPerf_RecordMeasure(str, len, tick);
    return result;
}

extern "C" void Phoenix_TextPerf_CountPrintCall()
{
    if (g_TextPerfEnabled)
        ++g_TextPerfFrame.printCalls;
}

extern "C" void Phoenix_TextPerf_CountGetDCCall()
{
    if (g_TextPerfEnabled)
    {
        ++g_TextPerfFrame.getDcLogicalCalls;
        ++Phoenix_TextPerf_CurrentCategoryFrame().getDcCalls;
    }
}

extern "C" void Phoenix_TextPerf_CountReleaseDCCall()
{
    if (g_TextPerfEnabled)
        ++g_TextPerfFrame.releaseDcLogicalCalls;
}

extern "C" void Phoenix_TextPerf_CountHdcAcquire()
{
    if (g_TextPerfEnabled)
    {
        ++g_TextPerfFrame.realGetDcCalls;
        ++Phoenix_TextPerf_CurrentCategoryFrame().realGetDcCalls;
    }
}

extern "C" void Phoenix_TextPerf_CountHdcRelease()
{
    if (g_TextPerfEnabled)
        ++g_TextPerfFrame.realReleaseDcCalls;
}

extern "C" void Phoenix_TextPerf_CountSurfaceUnlockLock()
{
    if (g_TextPerfEnabled)
        ++g_TextPerfFrame.surfaceUnlockLockPairs;
}

extern "C" void Phoenix_TextPerf_AddTextDrawTicks(unsigned __int64 beginTick)
{
    (void)beginTick;
}

extern "C" void Phoenix_TextPerf_AddStageTicks(int stage, unsigned __int64 beginTick)
{
    const double ms = Phoenix_TextPerf_EndToMs(beginTick);
    if (ms <= 0.0)
        return;

    const int category = Phoenix_TextPerf_CategoryFromStage(stage);
    g_TextPerfFrame.categories[category].scopeMs += ms;
}

static void Phoenix_TextPerf_ResetFrame()
{
    ZeroMemory(&g_TextPerfFrame, sizeof(g_TextPerfFrame));
    ZeroMemory(g_TextPerfStringHashes, sizeof(g_TextPerfStringHashes));
    g_TextPerfCategoryDepth = 0;
}

static void Phoenix_TextPerf_EndFrame()
{
    if (!g_TextPerfEnabled)
    {
        Phoenix_TextPerf_ResetFrame();
        return;
    }

    g_TextPerfFrame.frameIndex = ++g_TextPerfFrameIndex;
    g_TextPerfFrame.tick = GetTickCount();
    if (g_TextPerfFrame.dcCacheActiveAtFrameEnd == 0)
        g_TextPerfFrame.dcCacheActiveAtFrameEnd = (g_FrameCachedDC != NULL) ? 1u : 0u;
    g_TextPerfFrame.textFrameActiveAtFrameEnd = (g_TextFrameDepth > 0) ? 1u : 0u;
    Phoenix_TextPerf_RecordMeasureCacheEntryCount();
    g_TextPerfFrame.totalGdiTextMs =
        g_TextPerfFrame.textDrawTotalMs +
        g_TextPerfFrame.textMeasureTotalMs +
        g_TextPerfFrame.getDcAcquireTotalMs +
        g_TextPerfFrame.releaseDcTotalMs;

    g_TextPerfLastFrame = g_TextPerfFrame;
    g_TextPerfRing[g_TextPerfRingHead] = g_TextPerfFrame;
    g_TextPerfRingHead = (g_TextPerfRingHead + 1) %
        (int)(sizeof(g_TextPerfRing) / sizeof(g_TextPerfRing[0]));
    if (g_TextPerfRingCount < (int)(sizeof(g_TextPerfRing) / sizeof(g_TextPerfRing[0])))
        ++g_TextPerfRingCount;

    Phoenix_TextPerf_ResetFrame();
}

static void Phoenix_TextPerf_Append(char* buffer, int bufferSize, int& used,
                                    const char* fmt, ...)
{
    if (buffer == NULL || bufferSize <= 0 || used >= bufferSize)
        return;

    va_list args;
    va_start(args, fmt);
    int written = _vsnprintf_s(buffer + used, bufferSize - used, _TRUNCATE, fmt, args);
    va_end(args);

    if (written < 0)
    {
        used = bufferSize - 1;
        buffer[used] = '\0';
        return;
    }

    used += written;
    if (used >= bufferSize)
        used = bufferSize - 1;
}

static void Phoenix_TextPerf_AddFrame(PhoenixTextPerfFrame& dst,
                                      const PhoenixTextPerfFrame& src)
{
    dst.printCalls += src.printCalls;
    dst.textOutCalls += src.textOutCalls;
    dst.drawTextCalls += src.drawTextCalls;
    dst.textMeasureCalls += src.textMeasureCalls;
    dst.getDcLogicalCalls += src.getDcLogicalCalls;
    dst.releaseDcLogicalCalls += src.releaseDcLogicalCalls;
    dst.realGetDcCalls += src.realGetDcCalls;
    dst.realReleaseDcCalls += src.realReleaseDcCalls;
    dst.dcCacheHits += src.dcCacheHits;
    dst.dcCacheMisses += src.dcCacheMisses;
    dst.dcCacheFlushes += src.dcCacheFlushes;
    dst.dcCacheSurfaceSwitches += src.dcCacheSurfaceSwitches;
    dst.surfaceUnlockLockPairs += src.surfaceUnlockLockPairs;
    dst.textFrameBeginCalls += src.textFrameBeginCalls;
    dst.textFrameEndCalls += src.textFrameEndCalls;
    dst.measureCacheHits += src.measureCacheHits;
    dst.measureCacheMisses += src.measureCacheMisses;
    dst.measureCacheBypasses += src.measureCacheBypasses;
    dst.measureCacheEvictions += src.measureCacheEvictions;
    dst.measureCacheEntries = src.measureCacheEntries;
    dst.textDrawTotalMs += src.textDrawTotalMs;
    dst.textMeasureTotalMs += src.textMeasureTotalMs;
    dst.textMeasureCachedMs += src.textMeasureCachedMs;
    dst.textMeasureUncachedMs += src.textMeasureUncachedMs;
    dst.getDcAcquireTotalMs += src.getDcAcquireTotalMs;
    dst.releaseDcTotalMs += src.releaseDcTotalMs;
    dst.totalGdiTextMs += src.totalGdiTextMs;
    if (src.maxSingleTextDrawMs > dst.maxSingleTextDrawMs)
        dst.maxSingleTextDrawMs = src.maxSingleTextDrawMs;
    dst.charsDrawn += src.charsDrawn;
    dst.uniqueStringEstimate += src.uniqueStringEstimate;
    dst.repeatedStringDrawCount += src.repeatedStringDrawCount;

    for (int i = 0; i < PHOENIX_TEXT_CATEGORY_COUNT; ++i)
    {
        PhoenixTextPerfCategoryFrame& d = dst.categories[i];
        const PhoenixTextPerfCategoryFrame& s = src.categories[i];
        d.calls += s.calls;
        d.textOutCalls += s.textOutCalls;
        d.drawTextCalls += s.drawTextCalls;
        d.measureCalls += s.measureCalls;
        d.getDcCalls += s.getDcCalls;
        d.realGetDcCalls += s.realGetDcCalls;
        d.drawMs += s.drawMs;
        d.measureMs += s.measureMs;
        d.scopeMs += s.scopeMs;
        d.chars += s.chars;
        d.repeatedDraws += s.repeatedDraws;
    }
}

static int Phoenix_TextPerf_BuildAverage(DWORD now, DWORD windowMs,
                                          PhoenixTextPerfFrame& out)
{
    ZeroMemory(&out, sizeof(out));

    int frames = 0;
    for (int i = 0; i < g_TextPerfRingCount; ++i)
    {
        int index = g_TextPerfRingHead - 1 - i;
        if (index < 0)
            index += (int)(sizeof(g_TextPerfRing) / sizeof(g_TextPerfRing[0]));

        const PhoenixTextPerfFrame& frame = g_TextPerfRing[index];
        if (frame.tick == 0 || now - frame.tick > windowMs)
            break;

        Phoenix_TextPerf_AddFrame(out, frame);
        ++frames;
    }

    if (frames > 0)
    {
        const double divisor = (double)frames;
        out.textDrawTotalMs /= divisor;
        out.textMeasureTotalMs /= divisor;
        out.textMeasureCachedMs /= divisor;
        out.textMeasureUncachedMs /= divisor;
        out.getDcAcquireTotalMs /= divisor;
        out.releaseDcTotalMs /= divisor;
        out.totalGdiTextMs /= divisor;
        for (int i = 0; i < PHOENIX_TEXT_CATEGORY_COUNT; ++i)
        {
            out.categories[i].drawMs /= divisor;
            out.categories[i].measureMs /= divisor;
            out.categories[i].scopeMs /= divisor;
        }
    }

    return frames;
}

static void Phoenix_TextPerf_FormatTopCategories(char* buffer, int bufferSize,
                                                 int& used,
                                                 const PhoenixTextPerfFrame& frame)
{
    int order[PHOENIX_TEXT_CATEGORY_COUNT];
    for (int i = 0; i < PHOENIX_TEXT_CATEGORY_COUNT; ++i)
        order[i] = i;

    for (int i = 0; i < PHOENIX_TEXT_CATEGORY_COUNT - 1; ++i)
    {
        for (int j = i + 1; j < PHOENIX_TEXT_CATEGORY_COUNT; ++j)
        {
            const PhoenixTextPerfCategoryFrame& a = frame.categories[order[i]];
            const PhoenixTextPerfCategoryFrame& b = frame.categories[order[j]];
            if (b.drawMs + b.measureMs > a.drawMs + a.measureMs)
            {
                int tmp = order[i];
                order[i] = order[j];
                order[j] = tmp;
            }
        }
    }

    Phoenix_TextPerf_Append(buffer, bufferSize, used, "  top categories by ms:\n");
    int shown = 0;
    for (int i = 0; i < PHOENIX_TEXT_CATEGORY_COUNT && shown < 5; ++i)
    {
        const int category = order[i];
        const PhoenixTextPerfCategoryFrame& c = frame.categories[category];
        const double ms = c.drawMs + c.measureMs;
        if (ms <= 0.0 && c.calls == 0 && c.measureCalls == 0)
            continue;

        Phoenix_TextPerf_Append(buffer, bufferSize, used,
            "    %-18s %.3f ms | calls %u | measure %u | chars %u | repeat %u\n",
            kPhoenixTextPerfCategoryNames[category], ms, c.calls,
            c.measureCalls, c.chars, c.repeatedDraws);
        ++shown;
    }

    if (shown == 0)
        Phoenix_TextPerf_Append(buffer, bufferSize, used, "    no category data yet\n");
}

extern "C" void Phoenix_TextPerf_FormatOverlay(char* buffer, int bufferSize)
{
    if (buffer == NULL || bufferSize <= 0)
        return;

    buffer[0] = '\0';
    int used = 0;

    Phoenix_TextPerf_Append(buffer, bufferSize, used,
        "TextPerf: %s (memory-only, no file output)\n",
        g_TextPerfEnabled ? "enabled" : "disabled");

    const PhoenixTextPerfFrame& f = g_TextPerfLastFrame;
    const unsigned int dcTotal = f.dcCacheHits + f.dcCacheMisses;
    const double hitRatio = dcTotal ? (100.0 * (double)f.dcCacheHits / (double)dcTotal) : 0.0;
    const unsigned int measureCacheTotal = f.measureCacheHits + f.measureCacheMisses;
    const double measureHitRatio = measureCacheTotal
        ? (100.0 * (double)f.measureCacheHits / (double)measureCacheTotal)
        : 0.0;

    Phoenix_TextPerf_Append(buffer, bufferSize, used,
        "  frame %u | total %.3f ms | draw %.3f | measure %.3f | getdc %.3f | releasedc %.3f\n",
        f.frameIndex, f.totalGdiTextMs, f.textDrawTotalMs, f.textMeasureTotalMs,
        f.getDcAcquireTotalMs, f.releaseDcTotalMs);
    Phoenix_TextPerf_Append(buffer, bufferSize, used,
        "  calls: TextOut %u | DrawText %u | Measure %u | Print %u\n",
        f.textOutCalls, f.drawTextCalls, f.textMeasureCalls, f.printCalls);
    Phoenix_TextPerf_Append(buffer, bufferSize, used,
        "  GetDC logical %u / real %u | Release logical %u / real %u | cache hit %.1f%% (%u/%u)\n",
        f.getDcLogicalCalls, f.realGetDcCalls, f.releaseDcLogicalCalls,
        f.realReleaseDcCalls, hitRatio, f.dcCacheHits, dcTotal);
    Phoenix_TextPerf_Append(buffer, bufferSize, used,
        "  text frame begin/end %u/%u | dc flush %u | surface switch %u | active end/now %u/%u\n",
        f.textFrameBeginCalls, f.textFrameEndCalls, f.dcCacheFlushes,
        f.dcCacheSurfaceSwitches, f.textFrameActiveAtFrameEnd,
        g_TextFrameDepth > 0 ? 1u : 0u);
    Phoenix_TextPerf_Append(buffer, bufferSize, used,
        "  measure cache %.1f%% (%u/%u) | entries %u | evict %u | bypass %u | cached %.3f | uncached %.3f\n",
        measureHitRatio, f.measureCacheHits, measureCacheTotal,
        f.measureCacheEntries, f.measureCacheEvictions, f.measureCacheBypasses,
        f.textMeasureCachedMs, f.textMeasureUncachedMs);
    Phoenix_TextPerf_Append(buffer, bufferSize, used,
        "  chars %u | unique estimate %u | repeated draws %u | cached DC at frame end %u | active now %u\n",
        f.charsDrawn, f.uniqueStringEstimate, f.repeatedStringDrawCount,
        f.dcCacheActiveAtFrameEnd, g_FrameCachedDC != NULL ? 1u : 0u);
    Phoenix_TextPerf_FormatTopCategories(buffer, bufferSize, used, f);

    PhoenixTextPerfFrame avg1s;
    PhoenixTextPerfFrame avg5s;
    DWORD now = GetTickCount();
    int frames1s = Phoenix_TextPerf_BuildAverage(now, 1000, avg1s);
    int frames5s = Phoenix_TextPerf_BuildAverage(now, 5000, avg5s);

    Phoenix_TextPerf_Append(buffer, bufferSize, used,
        "\n  avg 1s (%d frames): total %.3f | draw %.3f | measure %.3f | real GetDC/frame %.2f\n",
        frames1s, avg1s.totalGdiTextMs, avg1s.textDrawTotalMs,
        avg1s.textMeasureTotalMs, frames1s ? (double)avg1s.realGetDcCalls / (double)frames1s : 0.0);
    Phoenix_TextPerf_Append(buffer, bufferSize, used,
        "  avg 5s (%d frames): total %.3f | draw %.3f | measure %.3f | real GetDC/frame %.2f\n",
        frames5s, avg5s.totalGdiTextMs, avg5s.textDrawTotalMs,
        avg5s.textMeasureTotalMs, frames5s ? (double)avg5s.realGetDcCalls / (double)frames5s : 0.0);
}

// 2026-05-16 Dalga 2 — M1: frame-level DC cache.
//
// Problem: her UI Show/_Item_Description_Show vb. cagrisi g_FL2_GetDC +
// g_FL2_ReleaseDC pair'i acar (TextOut, GetTextExtentPoint32 metric, vs.).
// Description-heavy frame'lerde ~15k GetDC/sec hesaplandi → driver round-trip
// per acquire/release.
//
// Cozum: BeginFrame..EndFrame penceresi icinde gercek GetDC sadece ilk
// acquire'da yapilir; kalan acquire'lar cached HDC dondurur. Depth refcount
// 0'a inince ReleaseDC YAPILMAZ — frame sonuna kadar tutulur. EndFrame
// (Present sonu) cached DC'yi gercekten release eder.
//
// Backward compat: BeginFrame cagrilmadiysa (frame penceresi kapali — ornegin
// init oncesi splash / DDraw7 path) eski per-call GetDC/ReleaseDC davranisi
// otomatik korunur.
//
// Thread safety: render single thread varsayim — global statik state mutex'siz.
// Frame/batch sonu HDC temizleyici. TextPerf frame'i burada kapanmaz; frame
// snapshot'i yalniz Phoenix_FL2_FrameEnd_ReleaseCache tarafinda alinir.
static void Phoenix_FL2_ReleaseCachedDCOnly(bool frameEnd)
{
    if (g_FrameCachedDC == NULL)
        return;

    HDC hdc = g_FrameCachedDC;
    int mode = g_FrameCachedMode;
    if (g_TextPerfEnabled)
    {
        ++g_TextPerfFrame.dcCacheFlushes;
        if (!frameEnd)
            g_TextPerfFrame.dcCacheActiveAtFrameEnd = 0;
    }
    if (frameEnd && g_TextPerfEnabled)
        g_TextPerfFrame.dcCacheActiveAtFrameEnd = 1;

    g_FrameCachedDC = NULL;
    g_FrameCachedMode = 0;
    g_FrameAcquireDepth = 0;

    // gh_FL2_DC ile senkronize — DC artik tutulmuyor
    if (gh_FL2_DC == hdc)
        gh_FL2_DC = NULL;
    g_eFL2_DC_Mode = 0;

    switch (mode)
    {
        case 1:
            if (gpC_fl2_surface != NULL)
            {
                const unsigned __int64 releaseTick = Phoenix_TextPerf_Begin();
                gpC_fl2_surface->ReleaseDC(hdc);
                Phoenix_TextPerf_AddReleaseTicks(releaseTick);
                Phoenix_TextPerf_CountHdcRelease();
            }
            break;
        case 2:
        {
            const unsigned __int64 releaseTick = Phoenix_TextPerf_Begin();
            Phoenix_ReleaseD3D9DC(hdc);
            Phoenix_TextPerf_AddReleaseTicks(releaseTick);
            break;
        }
        case 3:
        {
            const unsigned __int64 releaseTick = Phoenix_TextPerf_Begin();
            DeleteDC(hdc);
            Phoenix_TextPerf_AddReleaseTicks(releaseTick);
            Phoenix_TextPerf_CountHdcRelease();
            break;
        }
        default:
            break;
    }
}

// Frame sonu temizleyici — D3D9Backend::EndFrame'den cagirilir.
extern "C" void Phoenix_FL2_FrameEnd_ReleaseCache()
{
    if (g_TextFrameDepth > 0 && g_TextPerfEnabled)
        g_TextPerfFrame.textFrameActiveAtFrameEnd = 1;

    g_TextFrameDepth = 0;
    Phoenix_FL2_ReleaseCachedDCOnly(true);
    Phoenix_TextPerf_EndFrame();
}

extern "C" void Phoenix_FL2_BeginTextFrame()
{
    ++g_TextFrameDepth;
    if (g_TextPerfEnabled)
        ++g_TextPerfFrame.textFrameBeginCalls;
}

extern "C" void Phoenix_FL2_EndTextFrame()
{
    if (g_TextPerfEnabled)
        ++g_TextPerfFrame.textFrameEndCalls;

    if (g_TextFrameDepth > 0)
        --g_TextFrameDepth;

    if (g_TextFrameDepth == 0)
        Phoenix_FL2_ReleaseCachedDCOnly(false);
}

//-----------------------------------------------------------------------------
// g_SetFL2Surface
//
// 
//-----------------------------------------------------------------------------
void g_SetFL2Surface(LPDIRECTDRAWSURFACE7 surface)
{
	if (gpC_fl2_surface != surface)
	{
		if (g_TextPerfEnabled)
			++g_TextPerfFrame.dcCacheSurfaceSwitches;
		Phoenix_FL2_ReleaseCachedDCOnly(false);
	}
	gpC_fl2_surface = surface;
}

//-----------------------------------------------------------------------------
// g_PossibleStringCut
//
// sz_str�� position(byte)? cut? ? ??? ?���� ?��ʵ��.
// sz_str? ʵޢ 2byte, ˬ? 1byte?��.
//-----------------------------------------------------------------------------
bool g_PossibleStringCut(const char * sz_str, int position)
{
	if (sz_str != NULL)
	{
		const int strLength = (int)strlen(sz_str);
		if (strLength <= position)
			return true;

		for (int i = 0; i < strLength; )
		{
			if (i == position)
				return true;

			const unsigned char ch = static_cast<unsigned char>(sz_str[i]);
			if (Phoenix_IsLegacyDBCSLeadByte(ch) && i + 1 < strLength)
			{
				if (i + 1 == position)
					return false;
				i += 2;
			}
			else
			{
				++i;
			}
		}

		return true;
	}

	return false;
}

int g_GetStringWidth2(const char * sz_str, int Index, HFONT hfont)
{
	if (sz_str == NULL)
		return 0;

	int iStrLen = strlen (sz_str);
	if (iStrLen <= 0)
		return 0;
	if(Index >= iStrLen)
		Index = iStrLen-1;
	if (Index <= 0)
		return 0;

	assert(!gpC_base->m_p_DDSurface_back->IsLock());
	// Fix #15 (2026-05-14): gpC_fl2_surface NULL D3D9 modunda kabul; g_FL2_GetDC
	// GDI memory DC fallback'i yapar, metric hesaplari hala calisir.
	// 2026-05-16 Dalga 2 — M1: metric path, _Cached fast path. Frame penceresi
	// acikken cache reuse.

	SIZE cachedSize;
	if (Phoenix_TextMeasureCache_Get(sz_str, Index, hfont,
		PHOENIX_MEASURE_KIND_WIDTH_INDEX, Index, &cachedSize))
		return cachedSize.cx;

	HDC hdc;

	bool bGetDC = g_FL2_GetDC_Cached();

	hdc = gh_FL2_DC;

	if (hfont != NULL)
		SelectObject(hdc, hfont);

	SIZE size;
	Phoenix_TextPerf_GetTextExtentPoint32(hdc, sz_str, Index, &size);
	Phoenix_TextMeasureCache_Put(sz_str, Index, hfont,
		PHOENIX_MEASURE_KIND_WIDTH_INDEX, Index, size);

	if(bGetDC)
		g_FL2_ReleaseDC_Cached();

	return size.cx;
}

// sz_str?��?��, Width?�� ��?? ? ?? ?��?�� Index (base 0)�� ��۪����. 
int g_GetStringIndexByWidth (const char *sz_str, int Width, HFONT hfont)
{
	int iStrWidth = g_GetStringWidth (sz_str, hfont);
	int resIndex;
	int Len = strlen(sz_str);

	if (iStrWidth <= Width)	//���� ��?����.
		resIndex = Len - 1;
	else
	{
		int Index = Len -1;
		while (--Index)
		{
			if (!g_PossibleStringCut (sz_str, Index))
				Index --;

			iStrWidth = g_GetStringWidth2 (sz_str, Index+1, hfont);
			if(iStrWidth <= Width)
				break;
		}
		//?�� Index??.. Width?? ��׻ Ͽ? ?��?�� ��׻ݽ ٻݯ���� ��ٹ.
		resIndex = Index;
	}

	return resIndex;
}

int g_PrintColorStr2 (int x, int y, const char * sz_str, PrintInfo &pi, COLORREF str_rgb, int LimitWidth)
{
	int iStrWidth = g_GetStringWidth ((const char*)sz_str, pi.hfont);
	if (iStrWidth <= LimitWidth)
		return g_PrintColorStr (x, y, sz_str, pi, str_rgb);
	else
	{
		// ��?� ?��?�� ?�� ܩ??, ��ʵ ܩ??�� ܩ��. ��ʵ ܩ?�� ��׻ Ͽ??.. ?��?? ������..
		int Len = strlen (sz_str);
		char *strTemp = new char [Len + 1];

		int Index = Len - 1;
		while (--Index)
		{
			if (!g_PossibleStringCut (sz_str, Index))
			{
				//���� ? ?? Index (ʵޢ�� 2ί?? ?��??��?ݫ .. ?��)�� ����.. ʵ? ? --
				Index--;
			}
			//?��? ܩ? ����..
			iStrWidth = g_GetStringWidth2 ((const char*)sz_str, Index-1, pi.hfont);
			if (iStrWidth <= LimitWidth)
				break;
		}
		// ?��?�� ???׻ ?��?.. ����ݤ? ���� ??׻ݫ.. ڶ?
		if (Index==0)
			return g_PrintColorStr (x, y, sz_str, pi, str_rgb);
		// ��?��?.. Index ݫ��ݫ.. ��ݹ��. ��?.. ��? ���� ��?��.. '..'? ??����.
		strncpy (strTemp, sz_str, Index-2);
		strTemp [Index-2] = '.';
		strTemp [Index-1] = '.';
		strTemp [Index] = '\0';
		int res = g_PrintColorStr (x, y, (const char*)strTemp, pi, str_rgb);
		delete [] strTemp;
		return res;
	}
}

//-----------------------------------------------------------------------------
// g_GetStringWidth
//
// p_str? null terminated string?��.
// hfont�� ?ί��? setting���� ?̳ʵ ?? ��ʯ��.
//
// ! Ϩ����? lock?? ϭ����.
//-----------------------------------------------------------------------------
int g_GetStringWidth(const char * sz_str, HFONT hfont)
{
	assert(!gpC_base->m_p_DDSurface_back->IsLock());
	// Fix #15 (2026-05-14): gpC_fl2_surface NULL D3D9 modunda kabul; g_FL2_GetDC
	// GDI memory DC fallback'i yapar, metric hesaplari hala calisir.
	// 2026-05-16 Dalga 2 — M1: metric path, _Cached fast path.

	if (sz_str == NULL)
		return 0;

	const int strLen = (int)strlen(sz_str);
	SIZE cachedSize;
	if (Phoenix_TextMeasureCache_Get(sz_str, strLen, hfont,
		PHOENIX_MEASURE_KIND_WIDTH, 0, &cachedSize))
		return cachedSize.cx;

	HDC hdc;

	bool bGetDC = g_FL2_GetDC_Cached();

	hdc = gh_FL2_DC;

	if (hfont != NULL)
		SelectObject(hdc, hfont);

	SIZE size;
	Phoenix_TextPerf_GetTextExtentPoint32(hdc, sz_str, strLen, &size);
	Phoenix_TextMeasureCache_Put(sz_str, strLen, hfont,
		PHOENIX_MEASURE_KIND_WIDTH, 0, size);

	if(bGetDC)
		g_FL2_ReleaseDC_Cached();

	return size.cx;
}

//-----------------------------------------------------------------------------
// g_GetStringHeight
//
// p_str? null terminated string?��.
// hfont�� ?ί��? setting���� ?̳ʵ ?? ��ʯ��.
//
// !Ϩ����? lock?? ϭ����.
//-----------------------------------------------------------------------------
int g_GetStringHeight(const char * sz_str, HFONT hfont)
{
	assert(!gpC_base->m_p_DDSurface_back->IsLock());
	// Fix #15 (2026-05-14): gpC_fl2_surface NULL D3D9 modunda kabul; g_FL2_GetDC
	// GDI memory DC fallback'i yapar, metric hesaplari hala calisir.
	// 2026-05-16 Dalga 2 — M1: metric path, _Cached fast path.

	if (sz_str == NULL)
		return 0;

	const int strLen = (int)strlen(sz_str);
	SIZE cachedSize;
	if (Phoenix_TextMeasureCache_Get(sz_str, strLen, hfont,
		PHOENIX_MEASURE_KIND_HEIGHT, 0, &cachedSize))
		return cachedSize.cy;

	HDC hdc;

	bool bGetDC = g_FL2_GetDC_Cached();

	hdc = gh_FL2_DC;


	if (hfont != NULL)
		SelectObject(hdc, hfont);

	SIZE size;
	Phoenix_TextPerf_GetTextExtentPoint32(hdc, sz_str, strLen, &size);
	Phoenix_TextMeasureCache_Put(sz_str, strLen, hfont,
		PHOENIX_MEASURE_KIND_HEIGHT, 0, size);

	if(bGetDC)
		g_FL2_ReleaseDC_Cached();

	return size.cy;
}

//-----------------------------------------------------------------------------
// g_PrintLen
//
// gpC_fl2_surface�� ��?ʵ��.
// p_str? null terminated string?��.
//
// !Ϩ����? lock?? ϭ����.
//-----------------------------------------------------------------------------
// Forward decl — inline color tag parser, asagida tanimli (g_DrawColoredText
// ile ayni mantik, RECT yerine (x,y) baslangic noktasi).
static void g_PrintColoredLen(int x, int y, const char* sz_str, int str_length, PrintInfo* p_print_info);

void g_PrintLen(int x, int y, const char * sz_str, int str_length, PrintInfo * p_print_info)
{
	assert(!gpC_base->m_p_DDSurface_back->IsLock());
	if (sz_str!=NULL)
	{
		Phoenix_TextPerf_CountPrintCall();
		const unsigned __int64 textPerfTick = Phoenix_TextPerf_Begin();

		// 2026-05-10: Inline color/style tag detect for g_PrintLen.
		// system message + chat + diger g_Print kullanan UI yerleri icin de
		// ^!Y/R/G/N/W/K/O/P + ^!B/I/U + ^!- destek aciliyor (g_DrawText paritesi).
		// Tag yoksa eski TextOut path'i (geriye donuk uyumlu).
		if (str_length > 0 && p_print_info != NULL)
		{
			// strchr ile hizli detect: '^' yoksa eski path
			bool hasTag = false;
			for (int i = 0; i < str_length; i++) {
				if (sz_str[i] == '^') { hasTag = true; break; }
			}
			if (hasTag) {
				g_PrintColoredLen(x, y, sz_str, str_length, p_print_info);
				Phoenix_TextPerf_AddTextDrawTicks(textPerfTick);
				return;
			}
		}

		//assert(sz_str != NULL);
		assert(gpC_fl2_surface != NULL);

		// no Release exception handling code for speed.

		HDC hdc;

		bool bGetDC = g_FL2_GetDC();

		hdc = gh_FL2_DC;

		if (p_print_info != NULL)
		{
			SetTextAlign(hdc, p_print_info->text_align);
			SetBkMode(hdc, p_print_info->bk_mode);
			SetBkColor(hdc, p_print_info->back_color);
			SetTextColor(hdc, p_print_info->text_color);
			SelectObject(hdc, p_print_info->hfont);
		}

		Phoenix_TextPerf_TextOut(hdc, x, y, sz_str, str_length);

		if(bGetDC)
			g_FL2_ReleaseDC();

		Phoenix_TextPerf_AddTextDrawTicks(textPerfTick);

	}
}

std::string g_MakeLinefeedString(char* sz_src, int linewidth, HFONT hfont)
{
	assert (sz_src != NULL);

	std::string resultString = sz_src;

	int strWidth = g_GetStringWidth(sz_src, hfont);
	int LinefeedCount = strWidth/linewidth;

	int srcIndex = 0;
	int destIndex = 0;
	int LinefeedIndex = 0;
	for (int i=0; i<LinefeedCount; i++)
	{
		LinefeedIndex = g_GetStringIndexByWidth (&sz_src[srcIndex], linewidth, hfont);
		destIndex += LinefeedIndex;
		resultString.insert(destIndex, "\n");
		destIndex ++;
		srcIndex += LinefeedIndex;
	}
	return resultString;
}

// =============================================================================
// 2026-05-09: Inline color + style tag support for g_DrawText.
//
// Syntax (tum tag'ler `^!` prefix + tek code karakteri):
//   ^!Y / ^!R / ^!G / ^!N / ^!W / ^!K / ^!O / ^!P  -> renkli segment baslat
//                                                     (yellow/red/green/navy/
//                                                      white/black/orange/purple)
//                                                     NOT: B Bold icin ayrildi,
//                                                     blue yerine N (navy).
//   ^!B                                            -> bold toggle (kalin)
//   ^!I                                            -> italic toggle
//   ^!U                                            -> underline toggle
//   ^!-                                             -> tum format reset
//                                                     (renk + style default'a)
//   ^^                                              -> literal '^' karakteri
//   ^!X (X tanimsiz code)                           -> tag tuketilir, hicbir sey
//                                                     yapmaz (geriye donuk
//                                                     uyumlu fallback)
//
// Kapsam: Phoenix'in TUM g_DrawText kullanan UI alanlari (NPC dialog,
// system message, item description, friend system, vb.) otomatik renkli/stil.
// Tag bulunmayan string'ler eski g_DrawText davranisini korur (transparent
// backward compat — giris noktasinda strchr ile detect).
//
// Style sinirlamalari:
// - GDI font synthesis: TTF Bold/Italic variant yoksa GDI sentezler.
//   orbitron-medium Regular icin Bold = stroke kalinlastirma, Italic = matrix
//   transform (yamuk). orbitron-medium zaten bold-display tipinde, sentetik bold
//   "cok kalin" gorunur. Italic estetik kalitesi dusuk olabilir.
//   Daha iyi sonuc icin orbitron-medium Variable veya ayri Bold/Italic TTF gerek.
// - Underline: GDI native (lfUnderline), sorunsuz.
//
// Sinirlama (ilk versiyon):
// - Single-line render; multi-line word wrap inline tag ile karisik kullanim
//   icin destek yok.
// - p_print_info->text_align bypass; tag-aware path daima TA_LEFT|TA_TOP.
// - Golge (1px black offset) g_DrawText pattern'iyle uyumlu korunur.
// - Font cache process boyunca leak (proc exit'te OS reclaim eder).
//
// Ornek string.inf entry:
//   2547 = You can receive basic weapons from ^!Y^!BJaques^!- at the Command Post.
//   ("Jaques" sari + bold, geri kalan default)
// =============================================================================

#include <map>
#include <array>

#define STY_BOLD       0x1
#define STY_ITALIC     0x2
#define STY_UNDERLINE  0x4

static COLORREF g_ColorCodeToRGB(char c, COLORREF defaultColor)
{
	switch (c) {
		case 'Y': case 'y': return RGB(255, 220,   0);   // yellow
		case 'R': case 'r': return RGB(255,  60,  60);   // red
		case 'G': case 'g': return RGB( 80, 220,  80);   // green
		case 'N': case 'n': return RGB( 80, 160, 255);   // navy/blue (B = Bold)
		case 'W': case 'w': return RGB(255, 255, 255);   // white
		case 'K': case 'k': return RGB(  0,   0,   0);   // black
		case 'O': case 'o': return RGB(255, 140,   0);   // orange
		case 'P': case 'p': return RGB(200, 100, 240);   // purple
		default:            return defaultColor;          // tanimsiz -> default
	}
}

// Font cache: base HFONT -> styled HFONT[8] (3 bit: bold/italic/underline)
// Static cache process omru kadar yasar (8 kombo * her base font). Phoenix
// PI font sayisi ~15-20, max ~120-160 HFONT. OS exit'te reclaim eder.
static std::map<HFONT, std::array<HFONT, 8> > g_styledFontCache;

static HFONT g_GetStyledFont(HFONT baseFont, int styleBits)
{
	if (!baseFont) return NULL;
	if (styleBits == 0) return baseFont;  // baseline, recreation gerek yok

	auto it = g_styledFontCache.find(baseFont);
	if (it == g_styledFontCache.end()) {
		std::array<HFONT, 8> arr;
		arr.fill(NULL);
		arr[0] = baseFont;
		auto res = g_styledFontCache.emplace(baseFont, arr);
		it = res.first;
	}
	if (it->second[styleBits] == NULL) {
		LOGFONT lf;
		if (GetObject(baseFont, sizeof(lf), &lf) > 0) {
			if (styleBits & STY_BOLD)      lf.lfWeight    = FW_BOLD;
			if (styleBits & STY_ITALIC)    lf.lfItalic    = TRUE;
			if (styleBits & STY_UNDERLINE) lf.lfUnderline = TRUE;
			it->second[styleBits] = CreateFontIndirect(&lf);
		}
		// CreateFontIndirect fail olursa NULL kalir; flushSegment'te baseFont fallback.
	}
	return it->second[styleBits] ? it->second[styleBits] : baseFont;
}

// Tag-aware text renderer. Single-line, golge dahil, X-offset kumulatif.
// 2026-05-10: Inline color/style tag parser for (x,y) point-based g_PrintLen.
// g_DrawColoredText'in RECT-based varyantinin (x,y) versiyonu — system message,
// chat, vs. icin. Ayni state machine: ^!Y/R/G/N/W/K/O/P + ^!B/I/U + ^!- + ^^.
static void g_PrintColoredLen(int x, int y, const char* sz_str, int str_length, PrintInfo* p_print_info)
{
	if (!sz_str || !p_print_info || str_length <= 0) return;

	bool bGetDC = g_FL2_GetDC();
	HDC hdc = gh_FL2_DC;

	SetBkMode(hdc, p_print_info->bk_mode);
	SetBkColor(hdc, p_print_info->back_color);
	// SetTextAlign default (TA_LEFT|TA_TOP) — tag-aware path tek satir varsayar

	HFONT baseFont = p_print_info->hfont;
	COLORREF defaultColor = p_print_info->text_color;
	COLORREF currentColor = defaultColor;
	int currentStyle = 0;

	char buf[1024];
	int bufLen = 0;
	int curX = x;
	int baseY = y;

	auto flushSegment = [&]() {
		if (bufLen <= 0) return;
		buf[bufLen] = '\0';
		HFONT hf = g_GetStyledFont(baseFont, currentStyle);
		SelectObject(hdc, hf);
		// Golge (1px black offset)
		SetTextColor(hdc, RGB(0, 0, 0));
		Phoenix_TextPerf_TextOut(hdc, curX + 1, baseY + 1, buf, bufLen);
		// Ana renk
		SetTextColor(hdc, currentColor);
		Phoenix_TextPerf_TextOut(hdc, curX, baseY, buf, bufLen);
		// X kumulatif advance
		SIZE sz;
		Phoenix_TextPerf_GetTextExtentPoint32(hdc, buf, bufLen, &sz);
		curX += sz.cx;
		bufLen = 0;
	};

	for (int i = 0; i < str_length; i++) {
		char c = sz_str[i];
		if (c == '^' && i + 1 < str_length) {
			char next = sz_str[i + 1];
			if (next == '^') {
				// ^^ -> literal '^'
				if (bufLen < (int)sizeof(buf) - 1) buf[bufLen++] = '^';
				i++;
				continue;
			}
			if (next == '!' && i + 2 < str_length) {
				char code = sz_str[i + 2];
				flushSegment();
				switch (code) {
					case 'Y': case 'y':
					case 'R': case 'r':
					case 'G': case 'g':
					case 'N': case 'n':
					case 'W': case 'w':
					case 'K': case 'k':
					case 'O': case 'o':
					case 'P': case 'p':
						currentColor = g_ColorCodeToRGB(code, defaultColor);
						break;
					case 'B': case 'b': currentStyle ^= STY_BOLD;      break;
					case 'I': case 'i': currentStyle ^= STY_ITALIC;    break;
					case 'U': case 'u': currentStyle ^= STY_UNDERLINE; break;
					case '-':
						currentColor = defaultColor;
						currentStyle = 0;
						break;
					default: break;  // Tanimsiz code: tag tuketilir
				}
				i += 2;
				continue;
			}
		}
		if (bufLen < (int)sizeof(buf) - 1) buf[bufLen++] = c;
	}
	flushSegment();

	if (bGetDC) g_FL2_ReleaseDC();
}

static void g_DrawColoredText(RECT* pRt, const char* sz_str, PrintInfo* p_print_info)
{
	if (!pRt || !sz_str || !p_print_info) return;

	bool bGetDC = g_FL2_GetDC();
	HDC hdc = gh_FL2_DC;

	SetBkMode(hdc, p_print_info->bk_mode);
	SetBkColor(hdc, p_print_info->back_color);
	// SetTextAlign default (TA_LEFT|TA_TOP) — tag-aware path tek satir varsayar

	HFONT baseFont = p_print_info->hfont;
	COLORREF defaultColor = p_print_info->text_color;
	COLORREF currentColor = defaultColor;
	int currentStyle = 0;  // bit flags: STY_BOLD | STY_ITALIC | STY_UNDERLINE

	char buf[1024];
	int bufLen = 0;
	int curX = pRt->left;
	int baseY = pRt->top;

	auto flushSegment = [&]() {
		if (bufLen <= 0) return;
		buf[bufLen] = '\0';
		HFONT hf = g_GetStyledFont(baseFont, currentStyle);
		SelectObject(hdc, hf);
		// Golge
		SetTextColor(hdc, RGB(0, 0, 0));
		Phoenix_TextPerf_TextOut(hdc, curX + 1, baseY + 1, buf, bufLen);
		// Ana renk
		SetTextColor(hdc, currentColor);
		Phoenix_TextPerf_TextOut(hdc, curX, baseY, buf, bufLen);
		// X kumulatif advance
		SIZE sz;
		Phoenix_TextPerf_GetTextExtentPoint32(hdc, buf, bufLen, &sz);
		curX += sz.cx;
		bufLen = 0;
	};

	int srcLen = (int)strlen(sz_str);
	for (int i = 0; i < srcLen; i++) {
		char c = sz_str[i];
		if (c == '^' && i + 1 < srcLen) {
			char next = sz_str[i + 1];
			if (next == '^') {
				// ^^ -> literal '^'
				if (bufLen < (int)sizeof(buf) - 1) buf[bufLen++] = '^';
				i++;
				continue;
			}
			if (next == '!' && i + 2 < srcLen) {
				// ^!X tag — flush + state guncelle + 2 char skip
				char code = sz_str[i + 2];
				flushSegment();
				switch (code) {
					// Color tags
					case 'Y': case 'y':
					case 'R': case 'r':
					case 'G': case 'g':
					case 'N': case 'n':
					case 'W': case 'w':
					case 'K': case 'k':
					case 'O': case 'o':
					case 'P': case 'p':
						currentColor = g_ColorCodeToRGB(code, defaultColor);
						break;
					// Style toggles
					case 'B': case 'b': currentStyle ^= STY_BOLD;      break;
					case 'I': case 'i': currentStyle ^= STY_ITALIC;    break;
					case 'U': case 'u': currentStyle ^= STY_UNDERLINE; break;
					// Reset all (renk + style -> default)
					case '-':
						currentColor = defaultColor;
						currentStyle = 0;
						break;
					// Tanimsiz code: tag tuketilir, hicbir sey yapmaz
					default: break;
				}
				i += 2;
				continue;
			}
			// '^' var ama ne '^^' ne '^!X' — literal tek '^' olarak yaz
		}
		if (bufLen < (int)sizeof(buf) - 1) buf[bufLen++] = c;
	}
	flushSegment();

	if (bGetDC) g_FL2_ReleaseDC();
}

void g_DrawText(RECT *pRt, const char* sz_str, PrintInfo * p_print_info)
{
	if(!pRt || !sz_str) return;

	// 2026-05-09: Inline color tag detect — '^' karakteri varsa colored path.
	// Tag yoksa mevcut davranis (geriye donuk uyumlu, breaking change yok).
	if (sz_str[0] != '\0' && p_print_info && strchr(sz_str, '^') != NULL)
	{
		g_DrawColoredText(pRt, sz_str, p_print_info);
		return;
	}

	assert(!gpC_base->m_p_DDSurface_back->IsLock());
	if (sz_str!=NULL)
	{
		//assert(sz_str != NULL);
		assert(gpC_fl2_surface != NULL);

		//sz_str�� ?��?�� ?���� pRt�� ?? ����??, ?��?? ����??? ? ??? �� ?ݤʵ��.

		int str_length = strlen(sz_str);
		int destWidth = pRt->right - pRt->left;
		int strWidth = g_GetStringWidth(sz_str, p_print_info->hfont);
		int LinefeedCount = strWidth / destWidth;
		HFONT hfont = NULL;
		if (p_print_info)
			hfont = p_print_info->hfont;
		
		std::string srcStr = g_MakeLinefeedString ((char*)sz_str, destWidth, hfont);//sz_str;

	
/*		int srcIndex = 0;
		int destIndex = 0;
		int LinefeedIndex = 0;
		for (int i=0; i<LinefeedCount; i++)
		{
			LinefeedIndex = g_GetStringIndexByWidth (&sz_str[srcIndex], destWidth, hfont);
			destIndex += LinefeedIndex;
			srcStr.insert(destIndex, "\n");
			destIndex ++;
			srcIndex += LinefeedIndex;
		}*/

		int strHeight = g_GetStringHeight(sz_str, hfont);
		int destHeight = pRt->bottom - pRt->top;
		if (strWidth%destWidth) LinefeedCount++;
		pRt->bottom = pRt->top + strHeight * LinefeedCount;

		// no Release exception handling code for speed.

		HDC hdc;

		bool bGetDC = g_FL2_GetDC();

		hdc = gh_FL2_DC;

		if (p_print_info != NULL)
		{
			SetTextAlign(hdc, p_print_info->text_align);
			SetBkMode(hdc, p_print_info->bk_mode);
			SetBkColor(hdc, p_print_info->back_color);
			SelectObject(hdc, p_print_info->hfont);
//������ ��?
			SetTextColor(hdc, 0);
			RECT sRt = {pRt->left+1, pRt->top+1, pRt->right+1, pRt->bottom+1};
			Phoenix_TextPerf_DrawText(hdc, srcStr.c_str(), srcStr.length(), &sRt, p_print_info->text_align);
//?��? ��?
			SetTextColor(hdc, p_print_info->text_color);
			Phoenix_TextPerf_DrawText(hdc, srcStr.c_str(), srcStr.length(), pRt, p_print_info->text_align);

		}
		else
			Phoenix_TextPerf_DrawText(hdc, srcStr.c_str(), srcStr.length(), pRt, DT_LEFT);

		if(bGetDC)
			g_FL2_ReleaseDC();

	}
}



//-----------------------------------------------------------------------------
// g_DBCSLen
//
// p_dbcs�� length�� ?��ʵ��.
//-----------------------------------------------------------------------------
int g_DBCSLen(const char_t * p_dbcs)
{
	if (p_dbcs == NULL)
		return 0;

	int len = 0;
	while (*p_dbcs++ != 0)
		len++;

	return len;
}
int	g_GetByteLenth(const char_t * p_dbcs, int dbcs_len)
{

	if (p_dbcs == NULL || dbcs_len <= 0)
		return 0;
	// convert
	int c = 0;
	for (int i=0; i < dbcs_len; i++)
	{
		// check high byte
		if ((p_dbcs[i]&0xFF00) != 0)
		{
			c+=2;
		}
		else
		{
			c++;
		}
	}
	return c;

}
//-----------------------------------------------------------------------------
// g_Convert_DBCS_Ascii2SingleByte
//
// DBCS Ascii code�� single byte�� ί����. ���� ?���? ������ ?׻??��.
// p_new_buf�� new�� ���ʵ��. ����? Ϩ����? p_new_buf�� delete��?�� ʵ��.
//
// ?���� buffer�� ܩ?(by byte)�� ?��ʵ��.
//-----------------------------------------------------------------------------
int g_Convert_DBCS_Ascii2SingleByte(const char_t * p_dbcs, int dbcs_len, char * &p_new_buf)
{
	//
	// (1) ignore 'p_new_buf'.
	//

	if (p_dbcs == NULL || dbcs_len <= 0)
		return 0;

	int max_size = dbcs_len*2+1;

	char * p_temp = new char[max_size];
	CheckMemAlloc(p_temp);
	memset(p_temp, 0, max_size);

	// convert
	for (int i=0, c=0; i < dbcs_len; i++)
	{
		// check high byte
		if ((p_dbcs[i]&0xFF00) != 0)
		{
			*(char_t *)(p_temp+i+c) = p_dbcs[i];
			c++;
		}
		else
		{
			p_temp[i+c] = (char)(p_dbcs[i]);
		}
	}

	int len = strlen(p_temp);
	p_new_buf = new char[len+1];
	CheckMemAlloc(p_new_buf);

	strcpy(p_new_buf, p_temp);

	DeleteNewArray(p_temp);

	return len;
}

//-----------------------------------------------------------------------------
// g_ConvertAscii2DBCS
//
// ascii code(single byte?׻ ��?)�� DBCS�� ����?? p_new_buf�� ���ʵ��.
// p_new_buf? Ϩ����? delete���� ʵ��.
//
//-----------------------------------------------------------------------------
// p_ascii:		single byte string
// ascii_len:	p_ascii length
// p_new_buf:	converted DBCS buf
//
// return:		p_new_buf length
//					zero is failed.
//-----------------------------------------------------------------------------
int g_ConvertAscii2DBCS(const char * p_ascii, int ascii_len, char_t * &p_new_buf)
{
	if (p_ascii == NULL || ascii_len <= 0)
		return 0;

	int size = ascii_len+1;
	char_t * p_temp = new char_t[size];

	int dbcs = 0;
	for (int i=0; i<ascii_len; i++, dbcs++)
	{
		const unsigned char ch = static_cast<unsigned char>(p_ascii[i]);
		if (ch < 128 || Phoenix_IsTurkishCP1254Byte(ch) || i + 1 >= ascii_len)
		{
			p_temp[dbcs] = (char_t)ch;
		}
		else
		{
			p_temp[dbcs] = *((char_t *)(p_ascii+i));
			i++;
		}
	}

	// ?̳ʵ size�� buffer�� p_new_buf? ?ݤ��?? ʵ��.
	// ʵޢ? ��?��? dbcs�� ascii_len? ������.
	p_new_buf = new char_t[dbcs+1];

	for (int m=0; m < dbcs; m++)
		p_new_buf[m] = p_temp[m];

	DeleteNewArray(p_temp);
	p_new_buf[dbcs] = 0;

	return dbcs;
}

// DC�� Getʵ��.
//
// Fix #17 (2026-05-14): Uc-yolu fallback chain.
//   1. DDraw7 path: gpC_fl2_surface set ise (legacy mode) onu kullan.
//   2. D3D9 path: composition buffer'in IDirect3DSurface9::GetDC()'si — text
//      ekrana cikar (Present pipeline composition buffer -> back buffer kopya).
//   3. GDI memory DC fallback: ikisi de fail ederse — metric calisir, render
//      gorunmez (init oncesi vs.).
// Bayrak `g_eFL2_DC_Mode` release path'i yonlendirir. (Eski tanim buradaydi —
// 2026-05-16 Dalga 2 — M1 ile dosya basina tasindi; frame-level DC cache
// helper'inin g_FL2_GetDC'den onceki noktada erismesi gerekiyor.)
// 2026-05-16 Dalga 2 — M1: opt-in fast path. Metric helper'lar
// (g_GetStringWidth/Height/Width2) sprite-Lock kritik degil — Get..Release
// arasinda DrawOutBox/DrawInBox cagrilmiyor. Bu yuzden frame penceresi
// acikken cache reuse guvenli. _Cached suffix'li akrobatik helper'lar metric
// hesaplari icin tasarlandi.
//
// Diger callsite'lar (LISTBOX outer-hoist release-reacquire-before-DrawOutBox,
// g_PrintLen / g_DrawText sprite Lock kontrati gerektiren render path) eski
// `g_FL2_GetDC` / `g_FL2_ReleaseDC` API'sini kullanir — bu API'lar cache'e
// dokunmaz, gercek GetDC/ReleaseDC yapar.
//
// Donus: cache hit ise true (caller _Cached release cagirmali); fresh acquire
// ise yine true (caller release cagirmali); fail ise false.
bool g_FL2_GetDC_Cached()
{
	assert(!gpC_base->m_p_DDSurface_back->IsLock());
	Phoenix_TextPerf_CountGetDCCall();

	// Cache hit (frame icinde onceki acquire'dan)
	if (g_FrameCachedDC != NULL)
	{
		if (g_TextPerfEnabled)
			++g_TextPerfFrame.dcCacheHits;
		gh_FL2_DC      = g_FrameCachedDC;
		g_eFL2_DC_Mode = g_FrameCachedMode;
		++g_FrameAcquireDepth;
		return true;
	}

	// Text-batch aktifse cache penceresi gercek text cizimini kapsar. Eski
	// D3D9 frame flag'i Present icinde kaldigi icin tek basina yeterli degil.
	const bool cacheAllowed = (g_TextFrameDepth > 0) || Phoenix_D3D9_IsFrameInProgress();

	if (gh_FL2_DC != NULL)
	{
		// Onceki caller hala DC tutuyor (frame disinda nested call) — cache'e
		// koyma, eski idempotent davranis (caller release etmesin).
		return false;
	}

	if (g_TextPerfEnabled)
		++g_TextPerfFrame.dcCacheMisses;

	// Yol 1: DDraw7 path
	if (gpC_fl2_surface != NULL)
	{
		const unsigned __int64 acquireTick = Phoenix_TextPerf_Begin();
		HRESULT hr = gpC_fl2_surface->GetDC(&gh_FL2_DC);
		Phoenix_TextPerf_AddAcquireTicks(acquireTick);
		if (SUCCEEDED(hr) && gh_FL2_DC != NULL)
		{
			Phoenix_TextPerf_CountHdcAcquire();
			g_eFL2_DC_Mode = 1;
			if (cacheAllowed)
			{
				g_FrameCachedDC     = gh_FL2_DC;
				g_FrameCachedMode   = 1;
				g_FrameAcquireDepth = 1;
			}
			return true;
		}
		gh_FL2_DC = NULL;
	}

	// Yol 2: D3D9 composition
	{
		const unsigned __int64 acquireTick = Phoenix_TextPerf_Begin();
		HDC hdc = Phoenix_AcquireD3D9DC();
		Phoenix_TextPerf_AddAcquireTicks(acquireTick);
		if (hdc != NULL)
		{
			gh_FL2_DC = hdc;
			g_eFL2_DC_Mode = 2;
			if (cacheAllowed)
			{
				g_FrameCachedDC     = hdc;
				g_FrameCachedMode   = 2;
				g_FrameAcquireDepth = 1;
			}
			return true;
		}
	}

	// Yol 3: GDI memory DC
	{
	const unsigned __int64 acquireTick = Phoenix_TextPerf_Begin();
	gh_FL2_DC = CreateCompatibleDC(NULL);
	Phoenix_TextPerf_AddAcquireTicks(acquireTick);
	if (gh_FL2_DC != NULL)
	{
		Phoenix_TextPerf_CountHdcAcquire();
		g_eFL2_DC_Mode = 3;
		if (cacheAllowed)
		{
			g_FrameCachedDC     = gh_FL2_DC;
			g_FrameCachedMode   = 3;
			g_FrameAcquireDepth = 1;
		}
		return true;
	}
	}

	g_eFL2_DC_Mode = 0;
	return false;
}

bool g_FL2_ReleaseDC_Cached()
{
	assert(!gpC_base->m_p_DDSurface_back->IsLock());
	Phoenix_TextPerf_CountReleaseDCCall();

	if (gh_FL2_DC == NULL)
		return false;

	// Cache aktif ise gercek release YAPMA, depth--. Frame sonu temizlenir
	// (Phoenix_FL2_FrameEnd_ReleaseCache).
	if (g_FrameCachedDC != NULL && gh_FL2_DC == g_FrameCachedDC)
	{
		if (g_FrameAcquireDepth > 0)
			--g_FrameAcquireDepth;
		// gh_FL2_DC kalsin — bir sonraki Get_Cached cache hit yapsin.
		// Diger callsite'lar gh_FL2_DC'nin non-NULL olmasini "outer-holding"
		// olarak okuyabilir; eski idempotent davranis korunur (onlar release
		// etmez).
		return true;
	}

	// Frame disinda — eski davranis (gercek release).
	HDC hdc = gh_FL2_DC;
	int mode = g_eFL2_DC_Mode;
	gh_FL2_DC = NULL;
	g_eFL2_DC_Mode = 0;

	switch (mode)
	{
		case 1:
			if (gpC_fl2_surface != NULL)
			{
				const unsigned __int64 releaseTick = Phoenix_TextPerf_Begin();
				gpC_fl2_surface->ReleaseDC(hdc);
				Phoenix_TextPerf_AddReleaseTicks(releaseTick);
				Phoenix_TextPerf_CountHdcRelease();
			}
			break;
		case 2:
		{
			const unsigned __int64 releaseTick = Phoenix_TextPerf_Begin();
			Phoenix_ReleaseD3D9DC(hdc);
			Phoenix_TextPerf_AddReleaseTicks(releaseTick);
			break;
		}
		case 3:
		{
			const unsigned __int64 releaseTick = Phoenix_TextPerf_Begin();
			DeleteDC(hdc);
			Phoenix_TextPerf_AddReleaseTicks(releaseTick);
			Phoenix_TextPerf_CountHdcRelease();
			break;
		}
		default:
			break;
	}
	return true;
}

bool	g_FL2_GetDC()
{
	assert(!gpC_base->m_p_DDSurface_back->IsLock());
	Phoenix_TextPerf_CountGetDCCall();

	// Text-only batch aktifse explicit render path de cached HDC kullanabilir.
	// Batch disinda eski semantik korunur: her caller kendi Get/Release'ini yapar.
	if (g_TextFrameDepth > 0 && g_FrameCachedDC != NULL)
	{
		if (g_TextPerfEnabled)
			++g_TextPerfFrame.dcCacheHits;
		gh_FL2_DC = g_FrameCachedDC;
		g_eFL2_DC_Mode = g_FrameCachedMode;
		++g_FrameAcquireDepth;
		return true;
	}

	if (gh_FL2_DC != NULL)
		return false;

	if (g_TextFrameDepth > 0 && g_TextPerfEnabled)
		++g_TextPerfFrame.dcCacheMisses;

	// Yol 1: legacy DDraw7 surface mevcutsa
	if (gpC_fl2_surface != NULL)
	{
		const unsigned __int64 acquireTick = Phoenix_TextPerf_Begin();
		HRESULT hr = gpC_fl2_surface->GetDC(&gh_FL2_DC);
		Phoenix_TextPerf_AddAcquireTicks(acquireTick);
		if (SUCCEEDED(hr) && gh_FL2_DC != NULL)
		{
			Phoenix_TextPerf_CountHdcAcquire();
			g_eFL2_DC_Mode = 1;
			if (g_TextFrameDepth > 0)
			{
				g_FrameCachedDC = gh_FL2_DC;
				g_FrameCachedMode = 1;
				g_FrameAcquireDepth = 1;
			}
			return true;
		}
		gh_FL2_DC = NULL;
	}

	// Yol 2: D3D9 composition buffer (text gorunur olur)
	{
		const unsigned __int64 acquireTick = Phoenix_TextPerf_Begin();
		HDC hdc = Phoenix_AcquireD3D9DC();
		Phoenix_TextPerf_AddAcquireTicks(acquireTick);
		if (hdc != NULL)
		{
			gh_FL2_DC = hdc;
			g_eFL2_DC_Mode = 2;
			if (g_TextFrameDepth > 0)
			{
				g_FrameCachedDC = hdc;
				g_FrameCachedMode = 2;
				g_FrameAcquireDepth = 1;
			}
			return true;
		}
	}

	// Yol 3: GDI memory DC (sadece metric — TextOut gorunmez)
	{
	const unsigned __int64 acquireTick = Phoenix_TextPerf_Begin();
	gh_FL2_DC = CreateCompatibleDC(NULL);
	Phoenix_TextPerf_AddAcquireTicks(acquireTick);
	if (gh_FL2_DC != NULL)
	{
		Phoenix_TextPerf_CountHdcAcquire();
		g_eFL2_DC_Mode = 3;
		if (g_TextFrameDepth > 0)
		{
			g_FrameCachedDC = gh_FL2_DC;
			g_FrameCachedMode = 3;
			g_FrameAcquireDepth = 1;
		}
		return true;
	}
	}

	g_eFL2_DC_Mode = 0;
	return false;
}

// DC�� Release ʵ��.
bool	g_FL2_ReleaseDC()
{
	assert(!gpC_base->m_p_DDSurface_back->IsLock());
	Phoenix_TextPerf_CountReleaseDCCall();

	if (gh_FL2_DC == NULL)
		return false;

	if (g_FrameCachedDC != NULL && gh_FL2_DC == g_FrameCachedDC)
	{
		if (g_FrameAcquireDepth > 0)
			--g_FrameAcquireDepth;
		return true;
	}

	HDC hdc = gh_FL2_DC;
	int mode = g_eFL2_DC_Mode;
	gh_FL2_DC = NULL;
	g_eFL2_DC_Mode = 0;

	switch (mode)
	{
		case 1:  // DDraw7 surface
			if (gpC_fl2_surface != NULL)
			{
				const unsigned __int64 releaseTick = Phoenix_TextPerf_Begin();
				gpC_fl2_surface->ReleaseDC(hdc);
				Phoenix_TextPerf_AddReleaseTicks(releaseTick);
				Phoenix_TextPerf_CountHdcRelease();
			}
			break;
		case 2:  // D3D9 composition
		{
			const unsigned __int64 releaseTick = Phoenix_TextPerf_Begin();
			Phoenix_ReleaseD3D9DC(hdc);
			Phoenix_TextPerf_AddReleaseTicks(releaseTick);
			break;
		}
		case 3:  // GDI memory DC
		{
			const unsigned __int64 releaseTick = Phoenix_TextPerf_Begin();
			DeleteDC(hdc);
			Phoenix_TextPerf_AddReleaseTicks(releaseTick);
			Phoenix_TextPerf_CountHdcRelease();
			break;
		}
		default:
			// Bilinmeyen state — sessiz temizle (DC leak riski yok cunku NULL'a aldik)
			break;
	}
	return true;
}


////////////////////////////////////////////////
// ?? ??��? ʫ? by sonee
//
// ? ?���� 40���� ??? ��?? ReduceString(str,40);
// str ��ڶ�� ����ʵ��.
// ����?׻ Ͽ? ݤ��??�� ����� ��׻ݽ ��?����? ?? ���� ݤ��???
////////////////////////////////////////////////

void ReduceString(char *str,int len)
{
	if(len<15) return;
	char name[100]="",*p_name;
	int lt=strlen(str),passlen,position,size;
	p_name=&name[0];
	//bool g_PossibleStringCut(const char * sz_str, int position)
	if(lt>len)
	{
		if(g_PossibleStringCut(str,5))
			passlen=5;
		else	
			passlen=6;
		memcpy(p_name,str,passlen);
		p_name+=passlen;
		*(p_name++)='.';
		*(p_name++)='.';
		*(p_name++)='.';
		passlen+=3;
		size=passlen;
		position=lt-(len-passlen);
		if(g_PossibleStringCut(str,position))
			{
			position=lt-len+passlen;
			passlen=len-passlen;
		}
		else
			{
			position=lt-len+passlen-1;
			passlen=len-passlen+1;
		}
		size+=passlen;
		memcpy(p_name,str+position,passlen);		
		memcpy(str,name,size);
		str[size]='\0';		
	}		
}

// ?������ ... ? ??����.				 by sonee
void ReduceString2(char *str,int len)
{
	if(len<15) return;
	char name[100]="",*p_name;
	int lt=strlen(str),passlen,position,size;
	p_name=&name[0];
	//bool g_PossibleStringCut(const char * sz_str, int position)
	
	int skip_len=len-14;
	if(lt>len)
	{
		if(g_PossibleStringCut(str,skip_len))
			passlen=skip_len;
		else	
			passlen=skip_len+1;
		memcpy(p_name,str,passlen);
		p_name+=passlen;
		*(p_name++)='.';
		*(p_name++)='.';
		*(p_name++)='.';
		passlen+=3;
		size=passlen;
		position=lt-(len-passlen);
		if(g_PossibleStringCut(str,position))
			{
			position=lt-len+passlen;
			passlen=len-passlen;
		}
		else
			{
			position=lt-len+passlen-1;
			passlen=len-passlen+1;
		}
		size+=passlen;
		memcpy(p_name,str+position,passlen);		
		memcpy(str,name,size);
		str[size]='\0';		
	}		
}
// 2004, 10, 26, sobeit add start
void ReduceString3(char *str,int len)
{
	if(str == NULL) return;
	if(len<15) return;
	int CurLen = strlen(str);
	if(CurLen<=len) return;
	
	if(!g_PossibleStringCut(str,len))
		str[len-3] = '.';
	str[len-2] = '.';
	str[len-1] = '.';
	str[len] = '\0';
}
// 2004, 10, 26, sobeit add end

//-----------------------------------------------------------------------------
// g_PrintColorStrLen
//
// str? ��?ʵ ?�� x�� ?��ʵ��.
//-----------------------------------------------------------------------------
int g_PrintColorStrLen(int x, int y, const char * sz_str, int str_length, PrintInfo &pi, COLORREF str_rgb)
{
	PrintInfo use_pi = pi;

	use_pi.text_color = str_rgb;

	bool bGetDC = g_FL2_GetDC();

	g_PrintLen(x, y, sz_str, str_length, &use_pi);

	int re = (x+g_GetStringWidth(sz_str, use_pi.hfont));

	if(bGetDC)
		g_FL2_ReleaseDC();

	return re;
}

void g_PrintColorStrLenNoMeasure(int x, int y, const char * sz_str, int str_length, PrintInfo &pi, COLORREF str_rgb)
{
	PrintInfo use_pi = pi;

	use_pi.text_color = str_rgb;

	g_PrintLen(x, y, sz_str, str_length, &use_pi);
}


//-----------------------------------------------------------------------------
// g_PrintColorStrOut
//
// str? ��?ʵ ?�� x�� ?��ʵ��.
//-----------------------------------------------------------------------------
int g_PrintColorStrOut(int x, int y, const char * sz_str, PrintInfo &pi, COLORREF str_rgb, COLORREF out_rgb)
{
	PrintInfo use_pi = pi;

	use_pi.text_color = out_rgb;
	bool bGetDC = g_FL2_GetDC();
	g_Print(x-1, y, sz_str, &use_pi);
	g_Print(x+1, y, sz_str, &use_pi);
	g_Print(x, y-1, sz_str, &use_pi);
	g_Print(x, y+1, sz_str, &use_pi);

	use_pi.text_color = str_rgb;
	g_Print(x, y, sz_str, &use_pi);
	
	int re = (x+g_GetStringWidth(sz_str, use_pi.hfont));

	if(bGetDC)
		g_FL2_ReleaseDC();

	return re;
}

//-----------------------------------------------------------------------------
// g_PrintColorStrShadow
//
// str? ��?ʵ ?�� x�� ?��ʵ��.
//-----------------------------------------------------------------------------
int g_PrintColorStrShadow(int x, int y, const char * sz_str, PrintInfo &pi, COLORREF str_rgb, COLORREF shadow_rgb)
{
	assert(!gpC_base->m_p_DDSurface_back->IsLock());
	PrintInfo use_pi = pi;

	if(shadow_rgb == 0xFFFFFFFF)
	{
		int r = (str_rgb&0xFF0000) >> 16;
		int g = (str_rgb&0xFF00) >> 8;
		int b = str_rgb&0xFF;
//		shadow_rgb = RGB(max(0, r-150), max(0, g-150), max(0, b-150));
		shadow_rgb = RGB(r>>2, g>>2, b>>2);
	}

	bool bGetDC = g_FL2_GetDC();

	use_pi.text_color = shadow_rgb;
	g_Print(x+1, y+1, sz_str, &use_pi);

	use_pi.text_color = str_rgb;
	g_Print(x, y, sz_str, &use_pi);
	
	int re = (x+g_GetStringWidth(sz_str, use_pi.hfont));

	if(bGetDC)
		g_FL2_ReleaseDC();

	return re;
}

std::string g_GetNumberString(int number)
{
	char sz_temp[20];
	wsprintf(sz_temp, "%d", number);
	std::string sstr = sz_temp;
	for(int i = 3; i <= 13; i += 4)
		if((int)sstr.size() > i)sstr.insert(sstr.size()-i, ",");

	return sstr;
}


std::string g_GetStringByMoney(DWORD dwMoney)
{
	char TempBuffer[32] = {0,};
	std::string sstr;
	DWORD TempMoney = 0;
	if(dwMoney>=100000000) // ?
	{
		TempMoney = dwMoney / 100000000;
		if(TempMoney)
		{
			wsprintf(TempBuffer, "%d��", TempMoney);
			sstr+= TempBuffer;
		}
	}
	if(dwMoney>=10000) // ݫ
	{
		TempMoney = (dwMoney%100000000) / 10000;
		if(TempMoney)
		{
			wsprintf(TempBuffer, "%d��", TempMoney);
			sstr+= TempBuffer;
		}
	}
	
	TempMoney = (dwMoney%10000);
	if(TempMoney || 0 == dwMoney)
	{
		wsprintf(TempBuffer, "%d", TempMoney);
		sstr+= TempBuffer;
	}

	return sstr;
}
