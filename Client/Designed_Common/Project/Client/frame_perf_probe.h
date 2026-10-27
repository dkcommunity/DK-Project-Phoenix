#ifndef __PHOENIX_FRAME_PERF_PROBE_H__
#define __PHOENIX_FRAME_PERF_PROBE_H__

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <share.h>
#include <string.h>
#include <psapi.h>

// Low-overhead FPS/root-cause probe.
//
// Activation:
//   Create an empty file next to Client.exe:
//     frame_perf_probe.on
//
// Output:
//   frame_perf.csv next to Client.exe
//
// Disabled path checks the trigger file at most once per second. It does not
// use CrashTrace flags, so existing production CrashTrace policy stays intact.

#ifndef PHOENIX_FRAME_PROBE_ENABLED
#define PHOENIX_FRAME_PROBE_ENABLED 0
#endif

extern "C" unsigned int Phoenix_ItemOutputPerf_GetWarmupUniqueResourceCount();

static inline void Phoenix_FrameProbe_ModulePath(char* outPath, const char* fileName)
{
    outPath[0] = 0;
    if (GetModuleFileNameA(NULL, outPath, MAX_PATH) > 0)
    {
        char* lastSlash = strrchr(outPath, '\\');
        if (lastSlash)
        {
            strcpy(lastSlash + 1, fileName);
            return;
        }
    }
    strcpy(outPath, fileName);
}

static inline bool Phoenix_FrameProbe_Enabled()
{
#if !PHOENIX_FRAME_PROBE_ENABLED
    return false;
#else
    static DWORD s_lastCheck = 0;
    static bool s_enabled = false;

    DWORD now = GetTickCount();
    if (now - s_lastCheck < 1000)
        return s_enabled;

    s_lastCheck = now;

    char path[MAX_PATH];
    Phoenix_FrameProbe_ModulePath(path, "frame_perf_probe.on");
    s_enabled = (GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES);
    return s_enabled;
#endif
}

static inline LONGLONG Phoenix_FrameProbe_Now()
{
    LARGE_INTEGER value;
    QueryPerformanceCounter(&value);
    return value.QuadPart;
}

static inline double Phoenix_FrameProbe_Ms(LONGLONG start, LONGLONG end)
{
    static double s_tickMs = 0.0;
    if (s_tickMs == 0.0)
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        s_tickMs = 1000.0 / (double)freq.QuadPart;
    }
    return (double)(end - start) * s_tickMs;
}

static inline void Phoenix_FrameProbe_Log(const char* format, ...)
{
    // Perf probe production'da dosya uretmesin.
    (void)format;
    return;

    if (!Phoenix_FrameProbe_Enabled())
        return;

    char path[MAX_PATH];
    Phoenix_FrameProbe_ModulePath(path, "frame_perf.csv");

    FILE* fp = _fsopen(path, "a", _SH_DENYNO);
    if (!fp)
        return;

    fprintf(fp, "%lu,", (unsigned long)GetTickCount());

    char buffer[2048];
    va_list args;
    va_start(args, format);
    _vsnprintf(buffer, sizeof(buffer) - 1, format, args);
    va_end(args);
    buffer[sizeof(buffer) - 1] = 0;

    fprintf(fp, "%s\n", buffer);
    fclose(fp);
}

static inline void Phoenix_FrameProbe_RecordFrame(
    double frameMs,
    double updateMs,
    double drawMs,
    int activeApp,
    int activeGame,
    int foreground,
    int iconic,
    int visible)
{
    if (!Phoenix_FrameProbe_Enabled())
        return;

    static DWORD s_windowStart = 0;
    static unsigned long s_count = 0;
    static double s_frameSum = 0.0;
    static double s_updateSum = 0.0;
    static double s_drawSum = 0.0;
    static double s_frameMax = 0.0;
    static double s_updateMax = 0.0;
    static double s_drawMax = 0.0;

    DWORD now = GetTickCount();
    if (s_windowStart == 0)
        s_windowStart = now;

    ++s_count;
    s_frameSum += frameMs;
    s_updateSum += updateMs;
    s_drawSum += drawMs;
    if (frameMs > s_frameMax) s_frameMax = frameMs;
    if (updateMs > s_updateMax) s_updateMax = updateMs;
    if (drawMs > s_drawMax) s_drawMax = drawMs;

    if (frameMs >= 50.0)
    {
        Phoenix_FrameProbe_Log(
            "FRAME_SLOW,frame_ms=%.3f,update_ms=%.3f,draw_ms=%.3f,active_app=%d,active_game=%d,foreground=%d,iconic=%d,visible=%d",
            frameMs, updateMs, drawMs, activeApp, activeGame, foreground, iconic, visible);
    }

    if (now - s_windowStart >= 1000 && s_count > 0)
    {
        Phoenix_FrameProbe_Log(
            "FRAME_SUMMARY,count=%lu,avg_frame_ms=%.3f,max_frame_ms=%.3f,avg_update_ms=%.3f,max_update_ms=%.3f,avg_draw_ms=%.3f,max_draw_ms=%.3f,active_app=%d,active_game=%d,foreground=%d,iconic=%d,visible=%d",
            s_count,
            s_frameSum / (double)s_count, s_frameMax,
            s_updateSum / (double)s_count, s_updateMax,
            s_drawSum / (double)s_count, s_drawMax,
            activeApp, activeGame, foreground, iconic, visible);

        s_windowStart = now;
        s_count = 0;
        s_frameSum = 0.0;
        s_updateSum = 0.0;
        s_drawSum = 0.0;
        s_frameMax = 0.0;
        s_updateMax = 0.0;
        s_drawMax = 0.0;
    }
}

static inline void Phoenix_FrameProbe_RecordPresent(
    double totalMs,
    double stateMs,
    double uploadMs,
    double sceneMs,
    double imguiMs,
    double presentMs,
    HRESULT stateHr,
    HRESULT presentHr,
    int reason,
    int foreground,
    int iconic,
    int visible)
{
    if (!Phoenix_FrameProbe_Enabled())
        return;

    static DWORD s_windowStart = 0;
    static unsigned long s_count = 0;
    static unsigned long s_reasonCount[5] = {0, 0, 0, 0, 0};
    static double s_totalSum = 0.0;
    static double s_presentSum = 0.0;
    static double s_totalMax = 0.0;
    static double s_presentMax = 0.0;

    DWORD now = GetTickCount();
    if (s_windowStart == 0)
        s_windowStart = now;

    ++s_count;
    if (reason >= 0 && reason < 5)
        ++s_reasonCount[reason];
    s_totalSum += totalMs;
    s_presentSum += presentMs;
    if (totalMs > s_totalMax) s_totalMax = totalMs;
    if (presentMs > s_presentMax) s_presentMax = presentMs;

    if (totalMs >= 25.0 || presentMs >= 15.0 || reason != 0)
    {
        Phoenix_FrameProbe_Log(
            "PRESENT_EVENT,total_ms=%.3f,state_ms=%.3f,upload_ms=%.3f,scene_ms=%.3f,imgui_ms=%.3f,present_ms=%.3f,state_hr=0x%08X,present_hr=0x%08X,reason=%d,foreground=%d,iconic=%d,visible=%d",
            totalMs, stateMs, uploadMs, sceneMs, imguiMs, presentMs,
            (unsigned int)stateHr, (unsigned int)presentHr,
            reason, foreground, iconic, visible);
    }

    if (now - s_windowStart >= 1000 && s_count > 0)
    {
        Phoenix_FrameProbe_Log(
            "PRESENT_SUMMARY,count=%lu,avg_total_ms=%.3f,max_total_ms=%.3f,avg_present_ms=%.3f,max_present_ms=%.3f,normal=%lu,iconic=%lu,occluded=%lu,restore=%lu,missing=%lu,foreground=%d,iconic=%d,visible=%d",
            s_count,
            s_totalSum / (double)s_count, s_totalMax,
            s_presentSum / (double)s_count, s_presentMax,
            s_reasonCount[0], s_reasonCount[1], s_reasonCount[2],
            s_reasonCount[3], s_reasonCount[4],
            foreground, iconic, visible);

        s_windowStart = now;
        s_count = 0;
        s_reasonCount[0] = s_reasonCount[1] = s_reasonCount[2] = s_reasonCount[3] = s_reasonCount[4] = 0;
        s_totalSum = 0.0;
        s_presentSum = 0.0;
        s_totalMax = 0.0;
        s_presentMax = 0.0;
    }
}

static inline void Phoenix_FrameProbe_RecordActivate(
    int activeGame,
    double totalMs,
    double inputMs,
    double gammaMs,
    double restore1Ms,
    double restore2Ms,
    double d3dRestoreMs,
    double topViewMs,
    double uiRestoreMs)
{
    if (!Phoenix_FrameProbe_Enabled())
        return;

    Phoenix_FrameProbe_Log(
        "ACTIVATE,active_game=%d,total_ms=%.3f,input_ms=%.3f,gamma_ms=%.3f,restore1_ms=%.3f,restore2_ms=%.3f,d3d_restore_ms=%.3f,topview_ms=%.3f,ui_restore_ms=%.3f",
        activeGame, totalMs, inputMs, gammaMs, restore1Ms, restore2Ms,
        d3dRestoreMs, topViewMs, uiRestoreMs);
}

static inline bool Phoenix_FrameProbe_QueryProcessMemory(
    SIZE_T* workingSet,
    SIZE_T* privateUsage)
{
    if (workingSet)
        *workingSet = 0;
    if (privateUsage)
        *privateUsage = 0;

    typedef BOOL (WINAPI *GetProcessMemoryInfoFn)(
        HANDLE, PPROCESS_MEMORY_COUNTERS, DWORD);

    static GetProcessMemoryInfoFn s_getProcessMemoryInfo = NULL;
    static bool s_resolved = false;

    if (!s_resolved)
    {
        HMODULE psapi = GetModuleHandleA("psapi.dll");
        if (psapi == NULL)
            psapi = LoadLibraryA("psapi.dll");
        if (psapi != NULL)
        {
            s_getProcessMemoryInfo =
                (GetProcessMemoryInfoFn)GetProcAddress(psapi, "GetProcessMemoryInfo");
        }
        s_resolved = true;
    }

    if (s_getProcessMemoryInfo == NULL)
        return false;

    PROCESS_MEMORY_COUNTERS_EX counters;
    ZeroMemory(&counters, sizeof(counters));
    counters.cb = sizeof(counters);

    if (!s_getProcessMemoryInfo(
            GetCurrentProcess(),
            (PPROCESS_MEMORY_COUNTERS)&counters,
            sizeof(counters)))
    {
        return false;
    }

    if (workingSet)
        *workingSet = counters.WorkingSetSize;
    if (privateUsage)
        *privateUsage = counters.PrivateUsage;
    return true;
}

static inline void Phoenix_FrameProbe_LogResource(const char* format, ...)
{
    // Resource perf probe production'da dosya uretmesin.
    (void)format;
    return;

    if (!Phoenix_FrameProbe_Enabled())
        return;

    char path[MAX_PATH];
    Phoenix_FrameProbe_ModulePath(path, "frame_resource_after_warmup_3.csv");

    FILE* fp = _fsopen(path, "a", _SH_DENYNO);
    if (!fp)
        return;

    fprintf(fp, "%lu,", (unsigned long)GetTickCount());

    char buffer[2048];
    va_list args;
    va_start(args, format);
    _vsnprintf(buffer, sizeof(buffer) - 1, format, args);
    va_end(args);
    buffer[sizeof(buffer) - 1] = 0;

    fprintf(fp, "%s\n", buffer);
    fclose(fp);
}

static inline void Phoenix_FrameProbe_RecordRuntimeCounters(
    int fps,
    int creatureCount,
    int fakeCreatureCount,
    int itemCount,
    int effectCount,
    int groundEffectCount)
{
    if (!Phoenix_FrameProbe_Enabled())
        return;

    static DWORD s_lastLog = 0;
    DWORD now = GetTickCount();
    if (now - s_lastLog < 5000)
        return;
    s_lastLog = now;

    DWORD handleCount = 0;
    GetProcessHandleCount(GetCurrentProcess(), &handleCount);

    DWORD gdiObjects = 0;
    DWORD userObjects = 0;
    HMODULE user32 = GetModuleHandleA("user32.dll");
    typedef DWORD (WINAPI *GetGuiResourcesFn)(HANDLE, DWORD);
    GetGuiResourcesFn getGuiResources = NULL;
    if (user32 != NULL)
        getGuiResources = (GetGuiResourcesFn)GetProcAddress(user32, "GetGuiResources");
    if (getGuiResources != NULL)
    {
        gdiObjects = getGuiResources(GetCurrentProcess(), 0);
        userObjects = getGuiResources(GetCurrentProcess(), 1);
    }

    SIZE_T workingSet = 0;
    SIZE_T privateUsage = 0;
    Phoenix_FrameProbe_QueryProcessMemory(&workingSet, &privateUsage);

    Phoenix_FrameProbe_LogResource(
        "RESOURCE_SUMMARY,fps=%d,working_set_kb=%lu,private_kb=%lu,handles=%lu,gdi=%lu,user=%lu,item_warmup_unique_resource_count=%lu,creatures=%d,fake_creatures=%d,items=%d,effects=%d,ground_effects=%d",
        fps,
        (unsigned long)(workingSet / 1024),
        (unsigned long)(privateUsage / 1024),
        (unsigned long)handleCount,
        (unsigned long)gdiObjects,
        (unsigned long)userObjects,
        (unsigned long)Phoenix_ItemOutputPerf_GetWarmupUniqueResourceCount(),
        creatureCount,
        fakeCreatureCount,
        itemCount,
        effectCount,
        groundEffectCount);
}

static inline void Phoenix_FrameProbe_LogStage(const char* format, ...)
{
    // Stage probe artik frame_stage.csv dosyasi uretmesin.
    (void)format;
    return;

    if (!Phoenix_FrameProbe_Enabled())
        return;

    char path[MAX_PATH];
    Phoenix_FrameProbe_ModulePath(path, "frame_stage.csv");

    FILE* fp = _fsopen(path, "a", _SH_DENYNO);
    if (!fp)
        return;

    fprintf(fp, "%lu,", (unsigned long)GetTickCount());

    char buffer[2048];
    va_list args;
    va_start(args, format);
    _vsnprintf(buffer, sizeof(buffer) - 1, format, args);
    va_end(args);
    buffer[sizeof(buffer) - 1] = 0;

    fprintf(fp, "%s\n", buffer);
    fclose(fp);
}

struct Phoenix_FrameProbeStageSlot
{
    const char* name;
    DWORD windowStart;
    unsigned long count;
    double sumMs;
    double maxMs;
};

static inline void Phoenix_FrameProbe_RecordStage(
    const char* name,
    double ms,
    int activeApp,
    int activeGame,
    int foreground,
    int iconic,
    int visible)
{
    if (!Phoenix_FrameProbe_Enabled() || !name)
        return;

    static Phoenix_FrameProbeStageSlot s_slots[48] = {0};
    Phoenix_FrameProbeStageSlot* slot = NULL;

    for (int i = 0; i < 48; ++i)
    {
        if (s_slots[i].name && strcmp(s_slots[i].name, name) == 0)
        {
            slot = &s_slots[i];
            break;
        }
        if (!s_slots[i].name && slot == NULL)
        {
            slot = &s_slots[i];
            slot->name = name;
            slot->windowStart = GetTickCount();
            break;
        }
    }

    if (!slot)
        return;

    DWORD now = GetTickCount();
    if (slot->windowStart == 0)
        slot->windowStart = now;

    ++slot->count;
    slot->sumMs += ms;
    if (ms > slot->maxMs)
        slot->maxMs = ms;

    if (ms >= 10.0)
    {
        Phoenix_FrameProbe_LogStage(
            "STAGE_SLOW,name=%s,ms=%.3f,active_app=%d,active_game=%d,foreground=%d,iconic=%d,visible=%d",
            name, ms, activeApp, activeGame, foreground, iconic, visible);
    }

    if (now - slot->windowStart >= 1000 && slot->count > 0)
    {
        Phoenix_FrameProbe_LogStage(
            "STAGE_SUMMARY,name=%s,count=%lu,avg_ms=%.3f,max_ms=%.3f,active_app=%d,active_game=%d,foreground=%d,iconic=%d,visible=%d",
            name,
            slot->count,
            slot->sumMs / (double)slot->count,
            slot->maxMs,
            activeApp,
            activeGame,
            foreground,
            iconic,
            visible);

        slot->windowStart = now;
        slot->count = 0;
        slot->sumMs = 0.0;
        slot->maxMs = 0.0;
    }
}

struct Phoenix_FrameProbeNamedStageSlot
{
    char name[128];
    DWORD windowStart;
    unsigned long count;
    double sumMs;
    double maxMs;
};

static inline void Phoenix_FrameProbe_CopyClean(char* dst, int dstSize, const char* src)
{
    if (!dst || dstSize <= 0)
        return;

    dst[0] = 0;
    if (!src || !src[0])
        src = "(unnamed)";

    int out = 0;
    while (*src && out < dstSize - 1)
    {
        char c = *src++;
        if (c == ',' || c == '\r' || c == '\n')
            c = '_';
        dst[out++] = c;
    }
    dst[out] = 0;
}

static inline void Phoenix_FrameProbe_BuildNamedStageKey(
    char* dst,
    int dstSize,
    const char* prefix,
    const char* name)
{
    if (!dst || dstSize <= 0)
        return;

    dst[0] = 0;

    char cleanPrefix[48];
    char cleanName[96];
    Phoenix_FrameProbe_CopyClean(cleanPrefix, sizeof(cleanPrefix), prefix);
    Phoenix_FrameProbe_CopyClean(cleanName, sizeof(cleanName), name);

    _snprintf(dst, dstSize - 1, "%s:%s", cleanPrefix, cleanName);
    dst[dstSize - 1] = 0;
}

static inline void Phoenix_FrameProbe_RecordNamedStage(
    const char* prefix,
    const char* name,
    double ms,
    int activeApp,
    int activeGame,
    int foreground,
    int iconic,
    int visible)
{
    if (!Phoenix_FrameProbe_Enabled())
        return;

    char key[128];
    Phoenix_FrameProbe_BuildNamedStageKey(key, sizeof(key), prefix, name);

    static Phoenix_FrameProbeNamedStageSlot s_slots[256] = {0};
    Phoenix_FrameProbeNamedStageSlot* slot = NULL;

    for (int i = 0; i < 256; ++i)
    {
        if (s_slots[i].name[0] && strcmp(s_slots[i].name, key) == 0)
        {
            slot = &s_slots[i];
            break;
        }
        if (!s_slots[i].name[0] && slot == NULL)
        {
            slot = &s_slots[i];
            Phoenix_FrameProbe_CopyClean(slot->name, sizeof(slot->name), key);
            slot->windowStart = GetTickCount();
            break;
        }
    }

    if (!slot)
        return;

    DWORD now = GetTickCount();
    if (slot->windowStart == 0)
        slot->windowStart = now;

    ++slot->count;
    slot->sumMs += ms;
    if (ms > slot->maxMs)
        slot->maxMs = ms;

    if (ms >= 2.0)
    {
        Phoenix_FrameProbe_LogStage(
            "WINDOW_SLOW,name=%s,ms=%.3f,active_app=%d,active_game=%d,foreground=%d,iconic=%d,visible=%d",
            slot->name, ms, activeApp, activeGame, foreground, iconic, visible);
    }

    if (now - slot->windowStart >= 1000 && slot->count > 0)
    {
        Phoenix_FrameProbe_LogStage(
            "WINDOW_SUMMARY,name=%s,count=%lu,avg_ms=%.3f,max_ms=%.3f,active_app=%d,active_game=%d,foreground=%d,iconic=%d,visible=%d",
            slot->name,
            slot->count,
            slot->sumMs / (double)slot->count,
            slot->maxMs,
            activeApp,
            activeGame,
            foreground,
            iconic,
            visible);

        slot->windowStart = now;
        slot->count = 0;
        slot->sumMs = 0.0;
        slot->maxMs = 0.0;
    }
}

class Phoenix_FrameProbeStageScope
{
public:
    Phoenix_FrameProbeStageScope(const char* name, HWND hWnd, int activeApp, int activeGame)
        : m_name(name)
        , m_hWnd(hWnd)
        , m_activeApp(activeApp)
        , m_activeGame(activeGame)
        , m_enabled(Phoenix_FrameProbe_Enabled())
        , m_start(0)
    {
        if (m_enabled)
            m_start = Phoenix_FrameProbe_Now();
    }

    ~Phoenix_FrameProbeStageScope()
    {
        if (!m_enabled)
            return;

        LONGLONG end = Phoenix_FrameProbe_Now();
        Phoenix_FrameProbe_RecordStage(
            m_name,
            Phoenix_FrameProbe_Ms(m_start, end),
            m_activeApp,
            m_activeGame,
            m_hWnd ? (int)(GetForegroundWindow() == m_hWnd) : 0,
            m_hWnd ? (int)IsIconic(m_hWnd) : 0,
            m_hWnd ? (int)IsWindowVisible(m_hWnd) : 0);
    }

private:
    const char* m_name;
    HWND m_hWnd;
    int m_activeApp;
    int m_activeGame;
    bool m_enabled;
    LONGLONG m_start;
};

class Phoenix_FrameProbeNamedStageScope
{
public:
    Phoenix_FrameProbeNamedStageScope(
        const char* prefix,
        const char* name,
        HWND hWnd,
        int activeApp,
        int activeGame)
        : m_prefix(prefix)
        , m_name(name)
        , m_hWnd(hWnd)
        , m_activeApp(activeApp)
        , m_activeGame(activeGame)
        , m_enabled(Phoenix_FrameProbe_Enabled())
        , m_start(0)
    {
        if (m_enabled)
            m_start = Phoenix_FrameProbe_Now();
    }

    ~Phoenix_FrameProbeNamedStageScope()
    {
        if (!m_enabled)
            return;

        LONGLONG end = Phoenix_FrameProbe_Now();
        Phoenix_FrameProbe_RecordNamedStage(
            m_prefix,
            m_name,
            Phoenix_FrameProbe_Ms(m_start, end),
            m_activeApp,
            m_activeGame,
            m_hWnd ? (int)(GetForegroundWindow() == m_hWnd) : 0,
            m_hWnd ? (int)IsIconic(m_hWnd) : 0,
            m_hWnd ? (int)IsWindowVisible(m_hWnd) : 0);
    }

private:
    const char* m_prefix;
    const char* m_name;
    HWND m_hWnd;
    int m_activeApp;
    int m_activeGame;
    bool m_enabled;
    LONGLONG m_start;
};

#endif // __PHOENIX_FRAME_PERF_PROBE_H__
