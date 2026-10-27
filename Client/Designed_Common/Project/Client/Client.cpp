
#include "Client_PCH.h"
#include "crash_trace_tool.h"
#include "frame_perf_probe.h"
#include "ContentsFilter.h"
// 2026-05-18 ImGui Hibrit Admin Menu (Debug/PhoenixImGui.h)
#include "Debug/PhoenixImGui.h"
#if __CONTENTS(__XTRAP)
#include "XTrap4Client.h"
#endif //__XTRAP
// #define __NPROTECT__//by viva
// #undef __NPROTECT__
// #undef __NPROTECT_OLD_VERSION__
#define NO_GAMEGUARD
// #define __NPROTECT_OLD_VERSION__

//-----------------------------------------------------------------------------
// Client.cpp1
//-----------------------------------------------------------------------------

#pragma warning(disable : 4786)

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500 /* version 5.0 */
#endif                      /* !_WIN32_WINNT */

// #ifdef _DEBUG
// #define _CRTDBG_MAP_ALLOC
// #endif

#include "DebugInfo.h"
// #ifndef _DEBUG
// #include "CrashReport.h"
// #endif

#if defined(OUTPUT_DEBUG) || defined(_DEBUG) || defined(__DEBUG_OUTPUT__)
#undef __NPROTECT__
#define NO_GAMEGUARD

#endif

#ifndef _DEBUG
#define __VVPROTECT__ // by viva
#endif
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "AppendPatchInfo.h"
#include "Client.h"
#include "EffectSpriteTypeDef.h"
#include "GameObject.h"
#include "UIDialog.h"
#include "UpdateManager.h"
#include "VS_UI.h"
#include <Commctrl.h>
#include <Windows.h>
#include <cstdlib>
#include <cerrno>
#include <crtdbg.h>
#include <direct.h>
#include <exception>
#include <fstream>
#include <io.h>
#include <process.h>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <time.h>

// #include "MFileDef.h"
#include "APICheck.h" //by viva
#include "Defender.h"
#include "MFileDef.h"
#include "MTestDef.h"
#include "OperatorOption.h"
#include "Properties.h"
#include "Resource.h"
#include "ServerInfo.h"
#include "SoundSetting.h"
#include "ThemidaSDK.h"
#include "UserInformation.h"
#include "Win32Edite.h"

static DWORD PhoenixParseClientVersionBytes(const char* data, size_t length, DWORD fallback)
{
  if (data == NULL || length == 0) {
    return fallback;
  }

  bool textNumber = true;
  bool hasDigit = false;
  for (size_t i = 0; i < length; ++i) {
    const unsigned char c = static_cast<unsigned char>(data[i]);
    if (c >= '0' && c <= '9') {
      hasDigit = true;
      continue;
    }
    if (c == '\0' || c == '\r' || c == '\n' || c == ' ' || c == '\t') {
      continue;
    }
    textNumber = false;
    break;
  }

  if (textNumber && hasDigit) {
    char text[64] = {0};
    const size_t copyLength = (length < sizeof(text) - 1) ? length : sizeof(text) - 1;
    memcpy(text, data, copyLength);
    return static_cast<DWORD>(strtoul(text, NULL, 10));
  }

  if (length >= sizeof(DWORD)) {
    DWORD version = 0;
    memcpy(&version, data, sizeof(DWORD));
    return version;
  }

  return fallback;
}

static DWORD PhoenixReadClientVersionFile(const char* versionPath, DWORD fallback)
{
  if (versionPath == NULL || versionPath[0] == '\0') {
    return fallback;
  }

  std::ifstream versionFile(versionPath, std::ios::binary);
  if (!versionFile.is_open()) {
    return fallback;
  }

  char buffer[64] = {0};
  versionFile.read(buffer, sizeof(buffer));
  const std::streamsize bytesRead = versionFile.gcount();
  return PhoenixParseClientVersionBytes(buffer, bytesRead > 0 ? static_cast<size_t>(bytesRead) : 0, fallback);
}
#include <DbgHelp.h>
#include <strsafe.h>

#if !__CONTENTS(__INTERNATIONAL_UI)
#define __USE_CRASH_REPORT
#endif

#ifdef __USE_CRASH_REPORT
#ifdef __BUGTRAP
#include "CrashReport/CrashReport.h"
#endif //__BUGTRAP
#endif

// #ifdef __NPROTECT__
// #include "nProtect.h"
// #endif

#if __CONTENTS(__CRYPT)
#include "Crypt.h"
#endif

#ifdef _DEBUG
// #include <vld.h>
#endif

// #define	new			DEBUG_NEW
// #define	UPDATER_FILENAME		"Updater.exe"
#define DIRECTORY_UPDATE "Update"

#ifndef OUTPUT_DEBUG
#define USE_UPDATER
#endif

// GameStart ana dongu oncesi yeniden kurulum icin (DX/CRT filter calabilir).
static void Phoenix_InstallFatalHandlers();

#if defined(OUTPUT_DEBUG) && defined(__METROTECH_TEST__)
//	#define	__WM_OUTPUT_DEBUG__
#endif

#ifdef OUTPUT_DEBUG
extern CRITICAL_SECTION g_Lock;
#endif

#ifdef OUTPUT_DEBUG
//	int CLogFile = -1;
#endif
//-----------------------------------------------------------------------------
// Global
//-----------------------------------------------------------------------------

#ifdef __NPROTECT__
NPROTECT_STATUS g_nProtectSTATUS = NPROTECT_NORMAL;
#endif

// Added to fix C2065 and C3861 after removing duplicate definitions
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
                            LPARAM lParam);
void InitResolutionConfig();
void SaveResolutionConfig();
extern "C" void Phoenix_ClientLifecycleLogf(const char *format, ...);
extern "C" void Phoenix_ClientLifecycleLogStack(const char *label);
extern int g_Dimension;
extern DWORD g_dwVideoMemory;
#if 0 // Disable duplicate LNK2005
HWND g_hWnd;
#else
extern HWND g_hWnd;
#endif // Disable duplicate LNK2005

HINSTANCE g_hInstance;
int g_x = 400;
int g_y = 300;
// bool				g_bHALAvailable = false;
DWORD g_dwVideoMemory = 0;
char g_CWD[_MAX_PATH];

static const char* kPhoenixLaunchArg = "--phx-launch";
static const char* kPhoenixDevDirectLaunchArg = "--phoenix-dev-direct-launch";
static const char* kPhoenixLaunchSource = "updater";
static const DWORD kPhoenixLaunchTicketTtlSeconds = 60;

static std::string PhoenixTrimCommandLine(const std::string& value)
{
  size_t first = 0;
  while (first < value.size() && (value[first] == ' ' || value[first] == '\t')) {
    ++first;
  }

  size_t last = value.size();
  while (last > first && (value[last - 1] == ' ' || value[last - 1] == '\t')) {
    --last;
  }

  return value.substr(first, last - first);
}

static bool PhoenixIsArgBoundary(char c)
{
  return c == '\0' || c == ' ' || c == '\t';
}

static size_t PhoenixFindCommandArg(const std::string& commandLine, const char* arg)
{
  const size_t argLength = strlen(arg);
  size_t pos = commandLine.find(arg);
  while (pos != std::string::npos) {
    const bool leftOk = (pos == 0) || PhoenixIsArgBoundary(commandLine[pos - 1]);
    const size_t right = pos + argLength;
    const bool rightOk = (right >= commandLine.size()) || PhoenixIsArgBoundary(commandLine[right]);
    if (leftOk && rightOk) {
      return pos;
    }
    pos = commandLine.find(arg, pos + 1);
  }
  return std::string::npos;
}

static std::string PhoenixRemoveCommandArg(const std::string& commandLine,
                                           size_t argPos,
                                           size_t argLength)
{
  const std::string before = PhoenixTrimCommandLine(commandLine.substr(0, argPos));
  size_t afterStart = argPos + argLength;
  while (afterStart < commandLine.size() &&
         (commandLine[afterStart] == ' ' || commandLine[afterStart] == '\t')) {
    ++afterStart;
  }

  const std::string after = PhoenixTrimCommandLine(commandLine.substr(afterStart));
  if (before.empty()) {
    return after;
  }
  if (after.empty()) {
    return before;
  }
  return before + " " + after;
}

static bool PhoenixConsumeDevDirectLaunchArg(const char* rawCommandLine,
                                             std::string& clientArgs)
{
  clientArgs = PhoenixTrimCommandLine(rawCommandLine != NULL ? rawCommandLine : "");

  const size_t argPos = PhoenixFindCommandArg(clientArgs, kPhoenixDevDirectLaunchArg);
  if (argPos == std::string::npos) {
    return false;
  }

  clientArgs = PhoenixRemoveCommandArg(clientArgs, argPos, strlen(kPhoenixDevDirectLaunchArg));
  return true;
}

static bool PhoenixExtractLaunchTicket(const char* rawCommandLine,
                                       std::string& ticket,
                                       std::string& clientArgs)
{
  ticket.clear();
  clientArgs = PhoenixTrimCommandLine(rawCommandLine != NULL ? rawCommandLine : "");

  const size_t launchPos = PhoenixFindCommandArg(clientArgs, kPhoenixLaunchArg);
  if (launchPos == std::string::npos) {
    return false;
  }

  size_t valueStart = launchPos + strlen(kPhoenixLaunchArg);
  while (valueStart < clientArgs.size() &&
         (clientArgs[valueStart] == ' ' || clientArgs[valueStart] == '\t')) {
    ++valueStart;
  }

  if (valueStart >= clientArgs.size()) {
    clientArgs = PhoenixTrimCommandLine(clientArgs.substr(0, launchPos));
    return true;
  }

  size_t valueEnd = valueStart;
  if (clientArgs[valueStart] == '"') {
    ++valueStart;
    valueEnd = valueStart;
    while (valueEnd < clientArgs.size() && clientArgs[valueEnd] != '"') {
      ++valueEnd;
    }
  } else {
    while (valueEnd < clientArgs.size() && !PhoenixIsArgBoundary(clientArgs[valueEnd])) {
      ++valueEnd;
    }
  }

  ticket = clientArgs.substr(valueStart, valueEnd - valueStart);
  clientArgs = PhoenixTrimCommandLine(clientArgs.substr(0, launchPos));
  return true;
}

static bool PhoenixIsValidLaunchTicketId(const std::string& ticket)
{
  if (ticket.size() != 32) {
    return false;
  }

  for (size_t i = 0; i < ticket.size(); ++i) {
    const char c = ticket[i];
    const bool isHex = (c >= '0' && c <= '9') ||
                       (c >= 'a' && c <= 'f') ||
                       (c >= 'A' && c <= 'F');
    if (!isHex) {
      return false;
    }
  }
  return true;
}

static bool PhoenixBuildLaunchTicketPath(const std::string& ticket, std::string& path)
{
  char localAppData[MAX_PATH] = {0};
  const DWORD length = GetEnvironmentVariableA("LOCALAPPDATA", localAppData, MAX_PATH);
  if (length == 0 || length >= MAX_PATH) {
    CT_Done("UpdaterGate: LOCALAPPDATA is unavailable");
    return false;
  }

  path = localAppData;
  path += "\\ProjectPhoenix\\launch\\";
  path += ticket;
  path += ".ticket";
  return true;
}

static ULONGLONG PhoenixUnixTimeSeconds()
{
  FILETIME fileTime;
  GetSystemTimeAsFileTime(&fileTime);

  ULARGE_INTEGER ticks;
  ticks.LowPart = fileTime.dwLowDateTime;
  ticks.HighPart = fileTime.dwHighDateTime;

  static const ULONGLONG kUnixEpoch = 116444736000000000ULL;
  return (ticks.QuadPart - kUnixEpoch) / 10000000ULL;
}

static std::string PhoenixAnsiPathToUtf8(const char* path)
{
  if (path == NULL || path[0] == '\0') {
    return std::string();
  }

  const int wideCount = MultiByteToWideChar(CP_ACP, 0, path, -1, NULL, 0);
  if (wideCount <= 0) {
    return path;
  }

  std::wstring wide(static_cast<size_t>(wideCount - 1), L'\0');
  MultiByteToWideChar(CP_ACP, 0, path, -1, &wide[0], wideCount);

  const int utf8Count = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, NULL, 0, NULL, NULL);
  if (utf8Count <= 0) {
    return path;
  }

  std::string utf8(static_cast<size_t>(utf8Count - 1), '\0');
  WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, &utf8[0], utf8Count, NULL, NULL);
  return utf8;
}

static std::string PhoenixNormalizeLaunchPath(std::string path)
{
  for (size_t i = 0; i < path.size(); ++i) {
    if (path[i] == '/') {
      path[i] = '\\';
    } else if (path[i] >= 'A' && path[i] <= 'Z') {
      path[i] = static_cast<char>(path[i] - 'A' + 'a');
    }
  }

  while (path.size() > 3 && path[path.size() - 1] == '\\') {
    path.erase(path.size() - 1);
  }

  return path;
}

static bool PhoenixReadTicketValue(const std::string& line,
                                   const char* key,
                                   std::string& value)
{
  const size_t keyLength = strlen(key);
  if (line.compare(0, keyLength, key) != 0) {
    return false;
  }
  if (line.size() <= keyLength || line[keyLength] != '=') {
    return false;
  }
  value = line.substr(keyLength + 1);
  return true;
}

static bool PhoenixValidateLaunchTicket(const std::string& ticket)
{
  if (!PhoenixIsValidLaunchTicketId(ticket)) {
    CT_Done("UpdaterGate: invalid ticket id format");
    return false;
  }

  std::string ticketPath;
  if (!PhoenixBuildLaunchTicketPath(ticket, ticketPath)) {
    return false;
  }

  std::ifstream in(ticketPath.c_str(), std::ios::in | std::ios::binary);
  if (!in.is_open()) {
    CT_Done("UpdaterGate: ticket file missing: %s", ticketPath.c_str());
    return false;
  }

  std::string source;
  std::string root;
  std::string createdUtc;
  std::string fileTicket;
  std::string line;
  while (std::getline(in, line)) {
    if (!line.empty() && line[line.size() - 1] == '\r') {
      line.erase(line.size() - 1);
    }
    if (PhoenixReadTicketValue(line, "source", source)) continue;
    if (PhoenixReadTicketValue(line, "root", root)) continue;
    if (PhoenixReadTicketValue(line, "created_utc", createdUtc)) continue;
    if (PhoenixReadTicketValue(line, "ticket", fileTicket)) continue;
  }
  in.close();
  DeleteFileA(ticketPath.c_str());

  if (source != kPhoenixLaunchSource || fileTicket != ticket || root.empty() || createdUtc.empty()) {
    CT_Done("UpdaterGate: ticket content mismatch");
    return false;
  }

  const ULONGLONG created = _strtoui64(createdUtc.c_str(), NULL, 10);
  const ULONGLONG now = PhoenixUnixTimeSeconds();
  if (created == 0 || created > now + 5 || now - created > kPhoenixLaunchTicketTtlSeconds) {
    CT_Done("UpdaterGate: ticket expired or invalid time");
    return false;
  }

  const std::string currentRoot = PhoenixNormalizeLaunchPath(PhoenixAnsiPathToUtf8(g_CWD));
  const std::string ticketRoot = PhoenixNormalizeLaunchPath(root);
  if (currentRoot != ticketRoot) {
    CT_Done("UpdaterGate: root mismatch ticket=%s current=%s", ticketRoot.c_str(), currentRoot.c_str());
    return false;
  }

  CT_Done("UpdaterGate: valid updater handoff ticket");
  return true;
}

static bool PhoenixLaunchUpdater(const std::string& clientArgs)
{
  std::string updaterPath = g_CWD;
  updaterPath += "\\";
  updaterPath += UPDATER_FILENAME;

  if (_access(updaterPath.c_str(), 0) != 0) {
    MessageBoxA(NULL,
                "Updater.exe was not found. Please restore Updater.exe in the game folder.",
                "Project Phoenix",
                MB_ICONERROR | MB_OK);
    CT_Done("UpdaterGate: Updater.exe missing: %s", updaterPath.c_str());
    return false;
  }

  std::string commandLine = "\"";
  commandLine += updaterPath;
  commandLine += "\"";
  if (!clientArgs.empty()) {
    commandLine += " UPDATE ";
    commandLine += clientArgs;
  }
  commandLine.push_back('\0');

  STARTUPINFOA startupInfo;
  ZeroMemory(&startupInfo, sizeof(startupInfo));
  startupInfo.cb = sizeof(startupInfo);

  PROCESS_INFORMATION processInfo;
  ZeroMemory(&processInfo, sizeof(processInfo));

  const BOOL launched = CreateProcessA(NULL,
                                       &commandLine[0],
                                       NULL,
                                       NULL,
                                       FALSE,
                                       0,
                                       NULL,
                                       g_CWD,
                                       &startupInfo,
                                       &processInfo);
  if (!launched) {
    const DWORD error = GetLastError();
    char message[256];
    _snprintf(message, sizeof(message) - 1, "Could not start Updater.exe. Error code: %lu", error);
    message[sizeof(message) - 1] = '\0';
    MessageBoxA(NULL, message, "Project Phoenix", MB_ICONERROR | MB_OK);
    CT_Done("UpdaterGate: CreateProcess Updater.exe failed error=%lu", error);
    return false;
  }

  CloseHandle(processInfo.hThread);
  CloseHandle(processInfo.hProcess);
  CT_Done("UpdaterGate: started Updater.exe and exiting Client");
  return true;
}

static bool PhoenixEnsureUpdaterHandoff(const char* rawCommandLine,
                                        std::string& sanitizedCommandLine)
{
  if (PhoenixConsumeDevDirectLaunchArg(rawCommandLine, sanitizedCommandLine)) {
    CT_Done("UpdaterGate: --phoenix-dev-direct-launch development bypass accepted args='%s'",
            sanitizedCommandLine.c_str());
    return true;
  }

  std::string ticket;
  const bool hasTicket = PhoenixExtractLaunchTicket(rawCommandLine, ticket, sanitizedCommandLine);
  if (hasTicket && PhoenixValidateLaunchTicket(ticket)) {
    return true;
  }

  if (hasTicket) {
    CT_Done("UpdaterGate: invalid updater handoff, rerouting to updater");
  } else {
    CT_Done("UpdaterGate: direct Client launch, rerouting to updater");
  }

  PhoenixLaunchUpdater(sanitizedCommandLine);
  return false;
}

#if 0  // Disable duplicate LNK2005
int g_Dimension = 0;
#endif // Disable duplicate LNK2005
DWORD g_TimerNPMON = 0;

// FPS
DWORD g_CurrentTime = 0; // ‹œê°
#if 0                    // Disable duplicate LNK2005
DWORD g_CurrentFrame = 0; // frameˆ˜
#endif                   // Disable duplicate LNK2005

int g_FrameCount = 0;
int g_StartFrameCount = 0;
DWORD g_StartTime = timeGetTime();
DWORD g_EndTime = g_StartTime;
int g_FrameRate = 0;
DWORD g_PingSendTime = 0;
DWORD g_PingReceivTime = 0;
DWORD g_Ping = 0;
static void Phoenix_ClientSpeedGuardTick(DWORD dwGameTime)
{
  static BOOL s_initialized = FALSE;
  static BOOL s_qpcAvailable = FALSE;
  static LARGE_INTEGER s_frequency = {0};
  static LARGE_INTEGER s_startCounter = {0};
  static DWORD s_startGameTime = 0;
  static DWORD s_lastSampleGameTime = 0;
  static DWORD s_lastLogGameTime = 0;
  static int s_suspiciousSamples = 0;

  if (!s_initialized) {
    s_qpcAvailable = QueryPerformanceFrequency(&s_frequency);
    if (s_qpcAvailable) {
      QueryPerformanceCounter(&s_startCounter);
      s_startGameTime = dwGameTime;
      s_lastSampleGameTime = dwGameTime;
    }
    s_initialized = TRUE;
  }

  if (!s_qpcAvailable) {
    return;
  }

  if (dwGameTime >= s_lastSampleGameTime && dwGameTime - s_lastSampleGameTime < 1000) {
    return;
  }
  s_lastSampleGameTime = dwGameTime;

  LARGE_INTEGER nowCounter;
  QueryPerformanceCounter(&nowCounter);

  const double qpcMs = (double)(nowCounter.QuadPart - s_startCounter.QuadPart) * 1000.0 /
                       (double)s_frequency.QuadPart;
  const DWORD gameMs = dwGameTime - s_startGameTime;

  if (qpcMs < 5000.0) {
    return;
  }

  const double ratio = qpcMs > 0.0 ? ((double)gameMs / qpcMs) : 1.0;
  if (ratio > 1.20 || ratio < 0.80) {
    ++s_suspiciousSamples;
  } else if (s_suspiciousSamples > 0) {
    --s_suspiciousSamples;
  }

  if (s_suspiciousSamples < 3) {
    return;
  }

  if (s_lastLogGameTime != 0 && dwGameTime >= s_lastLogGameTime &&
      dwGameTime - s_lastLogGameTime < 10000) {
    return;
  }
  s_lastLogGameTime = dwGameTime;

  char message[512];
  _snprintf(message, sizeof(message) - 1,
            "[ClientSpeedGuard] timer drift ratio=%.3f gameMs=%lu qpcMs=%.0f samples=%d\r\n",
            ratio,
            (unsigned long)gameMs,
            qpcMs,
            s_suspiciousSamples);
  message[sizeof(message) - 1] = '\0';
  OutputDebugStringA(message);

// 2026-09-04 LIVE hazirlik: speedguard drift tespiti oyuncu makinesinde
// client_speedguard.log dosyasi olusturuyordu (Client.exe yaninda, oyuncu
// gorur). Tespit mantigi ve OutputDebugString AYNEN duruyor; yalniz dosya
// yazimi flag'e baglandi (default 0). Speedhack teshisi gerekirse 1 yap.
#ifndef PHOENIX_SPEEDGUARD_LOG_ENABLED
#define PHOENIX_SPEEDGUARD_LOG_ENABLED 0
#endif
#if PHOENIX_SPEEDGUARD_LOG_ENABLED
  char path[_MAX_PATH];
  if (g_CWD[0] != '\0') {
    _snprintf(path, sizeof(path) - 1, "%s\\client_speedguard.log", g_CWD);
  } else {
    _snprintf(path, sizeof(path) - 1, "client_speedguard.log");
  }
  path[sizeof(path) - 1] = '\0';

  FILE* fp = fopen(path, "a");
  if (fp != NULL) {
    fprintf(fp, "%lu,%s", (unsigned long)dwGameTime, message);
    fclose(fp);
  }
#endif
}


bool g_bGoodFPS = true;

const int g_FrameGood = 15;

LONG g_lGameRunBreakTime =
    0; // ê²Œìž„ ‹¤–‰´ ì¤‘ì   •Œ(Š¤¬ë¦ ƒ· ˜Š” Pause Break)

CWin32Edite *g_pWin32Edite = NULL;

// 2009.01.05 shootkj
// by viva
#ifdef __DESIGNED_KOREA
BOOL g_IsWow64 = FALSE;
#endif

typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);

LPFN_ISWOW64PROCESS fnIsWow64Process;

BOOL IsWow64() {
  BOOL bIsWow64 = FALSE;

  fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
      GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

  if (NULL != fnIsWow64Process) {
    if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64)) {
      // handle error
    }
  }
  return bIsWow64;
}
// end
//  Application Time
DWORD gGetApplcationRunTime(bool bReset) {
  // DWORD static time = timeGetTime();
  DWORD dwCurrentTime = timeGetTime();
  if (bReset)
    g_gameStartTime = dwCurrentTime;
  return dwCurrentTime - g_gameStartTime;
}
DWORD g_gameStartTime = timeGetTime();
#if __CONTENTS(__XTRAP)
DWORD WINAPI XTrap_Check_Alive(LPVOID temp) {
  DWORD dwTime = 20000;
  while (1) {
    Sleep(20000);
    XTrap_C_CallbackAlive(dwTime);
  }
}
#endif //__XTRAP
// Execute Program --> bActiveApp
// minimize | anotherWnd click--> !ActiveGame
BOOL g_bActiveApp = FALSE;  // Is application active?
BOOL g_bActiveGame = FALSE; // Is Game Active?
BOOL g_bNeedUpdate = FALSE; // update•´•¼˜‚˜?

DWORD g_double_click_time = 0;

bool g_bTestMode = false;
// BOOL				g_WindowsNT = FALSE;
bool g_bForceExitBynProtect = false;
bool g_bForceExitBynProtect2 = false;
DWORD g_nProtectErrorMessage = 0;
DWORD g_nProtectErrorMessage2 = 0;
extern int g_LeftPremiumDays;
#if 0  // Disable duplicate LNK2005
bool g_bFamily = false;
#endif // Disable duplicate LNK2005
BYTE g_PayType = 1;
// bool				g_bSuperUser			= false;

// void SizeOfObjects();
// #define				__WEB_BROWSER__
// IWebBrowser2*			g_pWebBrowser = NULL;
//  [Futecˆ˜ •]
char g_FutecIP[20] = {
    0,
};
unsigned int g_FutecPort = 0;
BYTE g_AdvanceVampireActionMaxCount[ACTION_ADVANCEMENT_MAX -
                                    ACTION_ADVANCEMENT_STOP];
BYTE g_AdvanceSlayerActionMaxCount[ACTION_ADVANCEMENT_SLAYER_MAX -
                                   ACTION_ADVANCEMENT_SLAYER_STOP_SWORD];
BYTE g_AdvanceOustersActionMaxCount[ACTION_ADVANCEMENT_OUSTERS_MAX -
                                    ACTION_ADVANCEMENT_OUSTERS_CHAKRAM_STOP];

enum DARKEDEN_LANGUAGE {
  DARKEDEN_KOREAN,
  DARKEDEN_CHINESE,
  DARKEDEN_JAPANESE,
  DARKEDEN_ENGLISH = 0,
  DARKEDEN_TAIWAN,

  DARKEDEN_LANGUAGE_MAX
};

struct NETMARBLE_INFO {
  MString ID;
  MString Password;
  int WorldID;
  int ServerID;
  bool bGore;
};

// 2004, 7, 14, sobeit add start
struct REALSERVER_INFO {
  REALSERVER_INFO() {
    bMode = false;
    WorldID = 0;
  }
  bool bMode;  // 0: ˆ˜™ ‘†, 1: •ƒ ‘†
  int WorldID; // 0: ë¸Œë, 1: Ž  ˆŠ¤
  MString ID;  // id
  MString Key; // Key
};
// 2004, 7, 14, sobeit add end

BYTE g_macAddress[6];
extern bool CheckMacScreenMode();
extern BOOL GetMacAddressFromSock();
//-----------------------------------------------------------------------------
// define function
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// addFile„ ½–´„œ originalFile˜ — ë¶™ì¸‹¤.
//-----------------------------------------------------------------------------
#define WRITE_FROM_FILE(originalFile, patchFile)                               \
  {                                                                            \
    char buffer[4096];                                                         \
    int n;                                                                     \
                                                                               \
    while (1) {                                                                \
      patchFile.read(buffer, 4096);                                            \
      n = patchFile.gcount();                                                  \
                                                                               \
      if (n > 0) {                                                             \
        originalFile.write(buffer, n);                                         \
      } else {                                                                 \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  }

//-----------------------------------------------------------------------
// Get Futec Address [Futecˆ˜ •]
//-----------------------------------------------------------------------
// DarkEden.exe Futec(IP:Port)
//              01234567890123
//-----------------------------------------------------------------------
bool GetFutecAddress(const char *pStr) {
  const char *pFutecStr = strstr(pStr, "Futec");
  const char *pLeftParen = strchr(pStr, '(');
  const char *pSeperator = strchr(pStr, ':');
  const char *pRightParen = strchr(pStr, ')');

  if (pFutecStr == NULL || pLeftParen == NULL || pSeperator == NULL ||
      pRightParen == NULL ||
      !(pFutecStr < pLeftParen && pLeftParen < pSeperator &&
        pSeperator < pRightParen))
    return false;

  char str[20];

  memset(g_FutecIP, 0, 20); // by viva
  strncpy(g_FutecIP, pLeftParen + 1, pSeperator - (pLeftParen + 1));

  memset(str, 0, 20); // by viva
  strncpy(str, pSeperator + 1, pRightParen - (pSeperator + 1));
  g_FutecPort = atoi(str);

  return true;
}

bool ParsingNetmarble(const char *pCommandLine, NETMARBLE_INFO &info);
// bool ParsingRealServer(const char* pCommandLine,  int Dimention,
// REALSERVER_INFO &info);

bool ParsingRealServer(const char *pCommandLine, int Dimention,
                       REALSERVER_INFO &info) {
  if (pCommandLine == NULL)
    return false;

  char szTemp[4096];
  if (strlen(pCommandLine) < 16)
    return false;

  const char *pString = strchr(pCommandLine, ' ');
  if (pString == NULL) {
    return false;
  }

  strcpy(szTemp, pString + 1);

  int argcnt = 0;
  char *token = NULL;
  // •´‹¹ ë¬¸ìž—´„ 32ë°”ì´Š¸ë¡ Š˜ë¦¬ë©´„œ NULLë¬¸ìž ˆ„½´ ƒê¸°ì •Š„ë¡ +1 •´ì¤‹¤.
  char arg2[4][32 + 1];
  memset(arg2, 0, sizeof(arg2)); // DEFENSIVE: Clear array to prevent uninitialized access
  argcnt = 0;

  token = strtok(szTemp, "|");
  if (NULL == token)
    return false;
    
  while (token && argcnt < 4) {
    strncpy(arg2[argcnt], token, 32); // DEFENSIVE: Prevent buffer overrun on long tokens
    arg2[argcnt][32] = '\0';
    argcnt++;
    token = strtok(NULL, "|");
    //	if(NULL == token)
    //		return false;
  }
  
  // DEFENSIVE: Ensure we got all 4 required arguments
  if (argcnt < 4) {
    return false;
  }

  // mode|demention|ID|key
  info.bMode = atoi(arg2[0]) ? 1 : 0;
  info.WorldID = atoi(arg2[1]);
  info.ID = arg2[2];
  info.Key = arg2[3];

  return true;
}
//-----------------------------------------------------------------------------
// Name: InitFail()
// Desc: This function is called if an initialization function fails
//-----------------------------------------------------------------------------
HRESULT InitFail(LPCTSTR szError, ...) {
  // ”„ë¡œê·¸ž¨ ì¤‘ë‹¨..
  g_bActiveApp = FALSE;

  ShowCursor(TRUE);
  ShowWindow(g_hWnd, SW_HIDE);

  char szBuff[1024];
  va_list vl;

  va_start(vl, szError);
  vsprintf(szBuff, szError, vl);

  // ReleaseAllObjects();
  DEBUG_ADD(szBuff);

  MessageBox(g_hWnd, szBuff, PROGRAM_TITLE, MB_OK);
  DestroyWindow(g_hWnd);
  va_end(vl);

  return 0;
}

//-----------------------------------------------------------------------------
// ProgressBar
//-----------------------------------------------------------------------------
bool g_bUseProgressBar = true;
HWND g_hWndProgress = NULL;
const int progressBarWidth = 300;
const int progressBarHeight = 40;
const int g_numAppendFiles = 16; // AppendPatch.inf˜ file“¤ ê°œìˆ˜

std::map<DWORD, std::string> g_nProtectMessage;

static std::string BuildDataInfoPath(const std::string& fileName)
{
  if (fileName.find('\\') != std::string::npos ||
      fileName.find('/') != std::string::npos)
    return fileName;

  return std::string("Data\\Info\\") + fileName;
}

void RemoveProgressBar() {
  if (g_hWndProgress != NULL) {
    DestroyWindow(g_hWndProgress);
    g_hWndProgress = NULL;
  }
}

DARKEDEN_LANGUAGE CheckDarkEdenLanguage() {
  // Phoenix: Language.inf kullanimi kapali. Client sabit legacy CHS/Korean arayuzle acilir.
  return DARKEDEN_KOREAN;
}

void PrecalculateAdvancementClassCreatureFrames() {
  memset(g_AdvanceVampireActionMaxCount, 0,
         sizeof(g_AdvanceVampireActionMaxCount));
  memset(g_AdvanceSlayerActionMaxCount, 0,
         sizeof(g_AdvanceSlayerActionMaxCount));
  memset(g_AdvanceOustersActionMaxCount, 0,
         sizeof(g_AdvanceOustersActionMaxCount));

  int i;

  for (i = ACTION_ADVANCEMENT_SLAYER_STOP_SWORD;
       i < ACTION_ADVANCEMENT_SLAYER_MAX; ++i) {
    int actionindex = i - ADVANCEMENT_ACTION_START;

    char part = 0;
    if (i == ACTION_ADVANCEMENT_SLAYER_BIKE_MOVE) {
      g_AdvanceSlayerActionMaxCount[actionindex] = 4; // ì¹¡è¾œï¤³ë•¡
      continue;
    }

    // ||
    // i == ACTION_ADVANCEMENT_SLAYER_BIKE_STOP )
    // part = AC_BIKE_1;

    g_AdvanceSlayerActionMaxCount[actionindex] =
        g_pTopView->m_AdvancementSlayerManFPK[part][actionindex][2].GetSize();

    g_AdvanceSlayerActionMaxCount[36] = 23; //	›™žš˜»–‚ä»·ç©ºš‡ç«ŸéŒ¦ë¦
  }

  for (i = ACTION_ADVANCEMENT_STOP; i < ACTION_ADVANCEMENT_MAX; ++i) {
    int actionindex = i - ADVANCEMENT_ACTION_START;

    g_AdvanceVampireActionMaxCount[actionindex] =
        g_pTopView->m_AdvancementVampireManFPK[0][actionindex][2].GetSize();
  }

  for (i = ACTION_ADVANCEMENT_OUSTERS_CHAKRAM_STOP;
       i < ACTION_ADVANCEMENT_OUSTERS_MAX; ++i) {
    int actionindex = i - ADVANCEMENT_ACTION_START;

    g_AdvanceOustersActionMaxCount[actionindex] =
        g_pTopView->m_AdvancementOustersFPK[1][actionindex][2].GetSize();
  }
}

int GetPersnalShopUpdateTime() {
  CT_Done("GetPersnalShopUpdateTime ENTER");
  // Phoenix: UpdatePersnaltime.inf kullanimi kapali. Legacy default deger korunur.
  CT_Done("GetPersnalShopUpdateTime returning default 60000");
  return 60000;
}

void CreateProgressBar(const char *str) {
  if (g_bUseProgressBar) {
    RemoveProgressBar();

    InitCommonControls();

    int cyVScroll = GetSystemMetrics(SM_CYVSCROLL);
    g_hWndProgress = CreateWindowEx(
        WS_EX_TOPMOST, PROGRESS_CLASS, str, WS_VISIBLE | PBS_SMOOTH,
        GetSystemMetrics(SM_CXSCREEN) / 2 - progressBarWidth / 2,
        GetSystemMetrics(SM_CYSCREEN) / 2 - progressBarHeight / 2,
        progressBarWidth, progressBarHeight, NULL, (HMENU)0, g_hInstance, NULL);
  }
}

void SetProgressBarCount(int count) {
  if (!g_bUseProgressBar || g_hWndProgress == NULL) {
    return;
  }

  SendMessage(g_hWndProgress, PBM_SETRANGE, 0, MAKELPARAM(0, count));
  SendMessage(g_hWndProgress, PBM_SETSTEP, (WPARAM)1, 0);
  SendMessage(g_hWndProgress, PBM_SETPOS, (WPARAM)0, 0);
}

void SetProgressBarText(const char *str) {
  if (g_bUseProgressBar) {
    if (g_hWndProgress == NULL) {
      CreateProgressBar(str);
      SetProgressBarCount(g_numAppendFiles);
    } else {
      SetWindowText(g_hWndProgress, str);
    }
  }
}

void UpdateProgressBar() {
  if (!g_bUseProgressBar || g_hWndProgress == NULL) {
    return;
  }

  SendMessage(g_hWndProgress, PBM_STEPIT, 0, 0);
}

//-----------------------------------------------------------------------------
// Patch Log Window
//-----------------------------------------------------------------------------
HWND g_hPatchLogWnd = NULL;
HWND g_hPatchLogEdit = NULL;
char *g_pPatchLogBuffer = NULL; // –¼ë§ˆë‚˜ ´ì§ ëª°ë¼„œë¦ global— ‹¤.

LRESULT CALLBACK PatchLogWindowProc(HWND hWnd, UINT message, WPARAM wParam,
                                    LPARAM lParam) {
  switch (message) {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0L;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

bool ReadPatchLogFromFile() {
  char strBuffer[256];

  char *filename = "PatchLog.txt";

  if (access(filename, 0)) {
    return false;
  }

  std::ifstream file(filename); // text file´‹¤.

  file.seekg(0, std::ios::end);
  long fpEnd = file.tellg();

  if (g_pPatchLogBuffer != NULL) {
    delete[] g_pPatchLogBuffer;
  }
  g_pPatchLogBuffer = new char[fpEnd * 2];

  file.seekg(0);

  char *pBuffer = g_pPatchLogBuffer;

  while (!file.eof()) {
    file.getline(strBuffer, 254);
    int n = file.gcount();

    if (n == 0) {
      break;
    }

    if (strBuffer[n - 1] == '\0') {
      n -= 1;
    }

    // \r\n„ ë¶™ì—¬ì¤˜ì•¼ ‹¤Œì¤„ì´ œ‹¤.
    strBuffer[n] = '\r';
    strBuffer[n + 1] = '\n';

    strncpy(pBuffer, strBuffer, n + 2);
    pBuffer += n + 2;
  }
  *pBuffer = '\0';

  file.close();

  SetWindowText(g_hPatchLogEdit, g_pPatchLogBuffer);

  delete[] g_pPatchLogBuffer;
  g_pPatchLogBuffer = NULL;

  return true;
}

void OutPutClassSize() {
  // #ifdef __DEBUG_OUTPUT__
  //	SizeOfObjects();
  // #endif
}

void ShowPatchLogWindow() {
  /*	int width = 600;
          int height = 350;

          WNDCLASS               wc;

          wc.style			= CS_HREDRAW | CS_VREDRAW;
          wc.lpfnWndProc		= (WNDPROC)PatchLogWindowProc;
          wc.cbClsExtra		= 0;
          wc.cbWndExtra		= 0;
          wc.hInstance		= g_hInstance;
          wc.hIcon			= LoadIcon(g_hInstance,
  MAKEINTRESOURCE(IDI_DARKEDEN)); wc.hCursor			=
  LoadCursor(NULL, IDC_ARROW); wc.hbrBackground	= (HBRUSH
  )GetStockObject(BLACK_BRUSH); wc.lpszMenuName		= NULL; wc.lpszClassName
  = "PatchLog"; RegisterClass(&wc);

          DWORD style = WS_OVERLAPPEDWINDOW;
          style &= ~WS_MINIMIZEBOX;
          style &= ~WS_MAXIMIZEBOX;
          style &= ~WS_THICKFRAME;

          g_hPatchLogWnd = CreateWindow("PatchLog", "‹¤¬—´ Œ¨ì¹˜ë‚´š©",
                                                          style,
                                                          GetSystemMetrics(SM_CXSCREEN)/2
  - width/2, GetSystemMetrics(SM_CYSCREEN)/2 - height/2, width,	height, NULL,
  NULL, g_hInstance, NULL);

          if (!g_hPatchLogWnd)
          {
                  return;
          }

          RECT rect;
          GetClientRect(g_hPatchLogWnd, &rect);
          g_hPatchLogEdit = CreateWindow(TEXT("edit"), NULL,
                                                     WS_CHILD | WS_VISIBLE |
  WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE  |

  ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_READONLY,
                                                     0,0,
                                                     rect.right-rect.left,
  rect.bottom-rect.top, g_hPatchLogWnd, NULL, g_hInstance, NULL);

          if (!ReadPatchLogFromFile())
          {
                  DestroyWindow( g_hPatchLogWnd );

                  // ë°”ë¡œ return•˜ë© main windowê° ì£½ëŠ”‹¤. - -;
          }
          else
          {
                  ShowWindow(g_hPatchLogWnd, SW_SHOW);
                  UpdateWindow(g_hPatchLogWnd);
                  SetFocus(g_hPatchLogWnd);
          }

          MSG		msg;
          while (1)
          {
                  if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
                  {
                          if (!GetMessage(&msg, NULL, 0, 0))
                                  break;
                                  //return msg.wParam;
                          TranslateMessage(&msg);
                          DispatchMessage(&msg);
                  }
                  else
                  {
                          WaitMessage();
                  }
          }
  */
}

//-----------------------------------------------------------------------------
// Check DX Version
//-----------------------------------------------------------------------------
// DirectX Version„ check•œ‹¤.
//-----------------------------------------------------------------------------
/*
bool
CheckDXVersion()
{
        DWORD dwVer, dwPlatform;
        CDirectSetup::GetVersion( &dwVer, &dwPlatform );

        //------------------------------------------------------
        // Platform check
        //------------------------------------------------------
        //switch( dwPlatform )
    //{
      //  case DXVER_PLATFORM_WINDOWS:
            //strcpy( strBuff, "OS:\tWindows 9x\n" );
        //    break;
        //case DXVER_PLATFORM_NT:
            //strcpy( strBuff, "OS:\tWindows NT\n" );
          //  break;
        //default:
            //strcpy( strBuff, "Error!\n" );
          //  break;
    //}


        //------------------------------------------------------
        // DirectX Version check
        //------------------------------------------------------
        // DX 7.0 ´ƒ
    if (dwVer==DXVER_DX_7)
        {
                return true;
        }

        //InitFail("You need to install DirectX 7.0 or later version...");

        // •ˆ ê¹”ë ¸œ¼ë© •„˜ˆ ê¹”ì•„ë²„ë¦¬ž..
        char directory[_MAX_PATH];

        // ˜„ž¬ ”” ‰† ë¦¬ë –»–´„œ
        GetCurrentDirectory( _MAX_PATH, directory );

        // ‹¤–‰™”¼ ´ë¦„ì„ ë¶™ì¸‹¤.
        sprintf(directory, "%s\\%s", directory, PROGRAM_FILENAME);

        CDirectSetup::SetRestartProgram(directory);
        CDirectSetup::DirectXInstall(g_hWnd, g_hInstance, "DirectX7", true);

        // ƒˆë¡ ê¹”ê³  ‚˜„œ ë°”ë¡œ ‹¤–‰•´„ ˜Š”ê±¸ê¹Œ?
        // rebooting •´•¼•˜ì§ •Š„ê¹??

        return true;
        //return false;
}
*/

//-----------------------------------------------------------------------------
// Name: WindowProc()
// Desc: The Main Window Procedure
//-----------------------------------------------------------------------------
#if 1 // Enable WindowProc
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
                            LPARAM lParam) {
  // MINMAXINFO      *pMinMax;

  // 2026-05-18 ImGui Hibrit Admin Menu: input forwarding + CTRL+SHIFT+F12
  // toggle. ImGui mesaji yakaladiysa Phoenix kalanini atla. VS_UI menu
  // dokunulmaz — sadece ImGui penceresi uzerindeyken mouse/keyboard ImGui'ye.
  if (PhoenixImGui::WndProcHandler(hWnd, message, wParam, lParam))
    return DefWindowProc(hWnd, message, wParam, lParam);

  switch (message) {
    // case WM_CREATE:
    //	g_pWin32Edite = new CWin32Edite(hWnd, g_hInstance);//­Œ¡ë¹µë˜¥
    //	gC_vs_ui.SetWin32Edite( g_pWin32Edite );//­Œ¡ë¹µë˜¥
    //	break;

  case WM_CTLCOLOREDIT:
    if (g_pWin32Edite)
      return g_pWin32Edite->OnColor(hWnd, message, wParam, lParam);
    break;

#ifdef __NPROTECT__
#ifdef __NPROTECT_OLD_VERSION__
//	case WM_NPROTECT_SOFTICE :
//		DEBUG_ADD("[nProtect] GameClose By nProtect (Window Message :
// WM_NPROTECT_SOFTICE)"); 		SetMode( MODE_QUIT );
// g_nProtectSTATUS = NPROTECT_SOFTICE; 		break;
//
//	case WM_NPROTECT_EXIT_TWO :
//		DEBUG_ADD("[nProtect] GameClose By nProtect (Window Message :
// WM_NPROTECT_EXIT_TWO)"); 		SetMode( MODE_QUIT );
// g_nProtectSTATUS = NPROTECT_EXIT_TWO; 		break; 	case
// WM_NPROTECT_FORCEEXIT : 		DEBUG_ADD("[nProtect] GameClose By
// nProtect (Window Message : WM_NPROTECT_FORCEEXIT)"); 		SetMode(
// MODE_QUIT ); 		g_nProtectSTATUS = NPROTECT_FORCE_EXIT;
// break; 	case WM_NPROTECT_SH_ERROR : 		DEBUG_ADD("[nProtect]
// GameClose By nProtect (Window Message : WM_NPROTECT_SH_ERROR)");
// g_nProtectSTATUS = NPROTECT_SH_ERROR; 		break; 	case
// WM_NPROTECT_SPEEDHACK :
//		g_nProtectSTATUS = NPROTECT_SPEEDHACK;
//		break;
#endif
#endif
    // zzi	-  20071010 •œê¸ž… ¥ ê°•ì œ ì°¨ë‹¨.
    //			IME ëª¨ë“œ ë³™” ë©”ì„¸ì§ë¥ ë°›ì•„„œ~~~ ê°•ì œë¡.. NONCONVERSION
    // ‹œì¼œë³´•˜‹¤. 		ˆ˜ ••˜ê¸  „—Š” WM_IME_NOTIFY ì²˜ë¦¬ caseë¬¸ì´
    // ì£¼ì„œ¼ë¡ ì²˜ë¦¬˜–´ žˆ—ˆœ¼ë©, ‚´š© —†—ˆ‹¤.
#if !__CONTENTS(__LANGUAGE_CHANGE)
  case WM_IME_NOTIFY: {
    if ((DWORD)wParam == IMN_SETCONVERSIONMODE) {
      HIMC hImc = ImmGetContext(hWnd);
      if (hImc) {
        // Get current IME status
        DWORD dwConvMode, dwSentMode;
        ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);
        // notify ime mode change
        bool nativeMode = ((dwConvMode & IME_CMODE_NATIVE) != 0);

        //					if( nativeMode )
        //						dwConvMode |=
        // IME_CMODE_NATIVE; 					else
        // dwConvMode &= ~IME_CMODE_NATIVE;

        dwConvMode = IME_CMODE_NOCONVERSION;

        ImmSetConversionStatus(hImc, dwConvMode, dwSentMode);
        // Release input context
        ImmReleaseContext(hWnd, hImc);
      }
    }
  }
    return 0;
#endif
  //---------------------------------------------------------------
  //
  //						MM_MCINOTIFY
  //
  //---------------------------------------------------------------
  case MM_MCINOTIFY: {
    // Midê° ‹¤ —°ì£¼ë˜—ˆ‹¤Š” ë§ì´‹¤.
    if (wParam == MCI_NOTIFY_SUCCESSFUL) {
      // Opening ™˜ƒ´ ‚œ ê²½ìš°
      if (g_pAvi != NULL && (DWORD)lParam == g_pAvi->dwID) {
        g_pAvi->bEndFlag = true;
        g_pAvi->Close();

        // Login ™”ë©´ìœ¼ë¡...
        SetMode(MODE_MAINMENU);
      }
      // Œ•… —°ì£¼ê ‚œ ê²½ìš°
      else {
        /*
        // ë°˜ë³µ —°ì£ •ˆ•œ‹¤.
        if (g_pUserOption->PlayMusic)//g_Music.IsPause())	// ê³¼ì—°
        •„š”• ê¹.. - -;
        {
                if (g_pUserOption->PlayWaveMusic)
                {
                        g_pDXSoundStream->Play( FALSE );
                }
                else
                {
                        g_Music.RePlay();
                }
        }
        */
        // else
        //{
        //  ë°˜ë³µ•´„œ —°ì£¼í•œ‹¤.
        //	g_Music.RePlay();
        //}
        //					if(g_pMP3->IsLoop())
        //						g_pMP3->Play(true);
      }
    }
  }
    return 0L;

  //---------------------------------------------------------------
  //
  //	WM_SYSCOMMAND
  //
  //---------------------------------------------------------------
  case WM_SYSCOMMAND:
    // Prevent moving/sizing and power loss in fullscreen mode
    switch (wParam) {
    case SC_HOTKEY:
    case SC_KEYMENU:
    case SC_TASKLIST:
#if !defined(OUTPUT_DEBUG) && !defined(__DEBUG_OUTPUT__)
    case SC_PREVWINDOW:
    case SC_NEXTWINDOW:
#endif
      return 1;

    case SC_CLOSE:
    case SC_MOVE:
    case SC_SIZE:
    case SC_MAXIMIZE:
    case SC_MONITORPOWER:
    case SC_SCREENSAVE:
      if (CDirectDraw::IsFullscreen())
        return 1;
      break;
    }
    break; // return 0L;

    //---------------------------------------------------------------
    //
    //							WM_ACTIVATE
    //
    //---------------------------------------------------------------
    /*
    case WM_ACTIVATE :
    {
            #ifdef __WM_OUTPUT_DEBUG__
                            DEBUG_ADD("[WM_ACTIVATE]");
            #endif

                            /*
            //WORD fActive = LOWORD(wParam);           // activation flag
            //BOOL fMinimized = (BOOL) HIWORD(wParam); // minimized flag

            // activeê³ minimizedê° •„‹Œ ƒƒœê° ActiveGame´‹¹..
            //BOOL bActive = (fActive==WA_ACTIVE) || (fActive==WA_CLICKACTIVE);
            //				//&& !fMinimized;

            //g_double_click_time = GetDoubleClickTime();

            //if (g_bActiveApp)
            {
            //	CheckActivate(bActive);

            //	g_bActiveGame = bActive;
            }

            BOOL bActive = (wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE);

            if (g_pDXInput!=NULL)
            {
                    // acquire
                    g_pDXInput->SetAcquire(bActive);

                    // ž… ¥„ ì´ˆê¸°™”•œ‹¤.
                    g_pDXInput->Clear();
            }
    }
    break;//return 0L;
    */

    //---------------------------------------------------------------
    //
    //							WM_ACTIVATEAPP
    //
    //---------------------------------------------------------------
    ///*
    //		case WM_ACTIVATE :
  case WM_ACTIVATEAPP: {
#ifdef __WM_OUTPUT_DEBUG__
    DEBUG_ADD("[WM_ACTIVATEAPP]");
#endif

    // Pause if minimized or not the top window
    // WORD fActive = LOWORD(wParam);
    // WORD fMinimized = (BOOL) HIWORD(wParam);
    // g_bActiveGame = (fActive == WA_ACTIVE) || (fActive == WA_CLICKACTIVE);

    g_double_click_time = GetDoubleClickTime();

    BOOL bActiveGame = (BOOL)wParam;

    // if (g_bActiveApp)
    {
      CheckActivate(bActiveGame);
      //
      g_bActiveGame = bActiveGame;
    }

    // by svi	—Œ‡œAltˆ©¨æ¦´æª„
    //			if (g_bActiveApp)
    //			{
    //				if (g_pDXInput->KeyDown(DIK_LMENU) ||
    // g_pDXInput->KeyDown(DIK_RMENU))
    //				{
    //					g_pDXInput->Clear();
    //				}
    //			}
    // end

    gC_vs_ui.CloseGameMenu();
  } break; // return 0L;
           //*/

    /*
    case WM_SIZE :
    {
            BOOL bActive;

            // Check to see if we are losing our window...
if( SIZE_MAXHIDE==wParam || SIZE_MINIMIZED==wParam )
    bActive = FALSE;
else
    bActive = TRUE;

g_double_click_time = GetDoubleClickTime();

            if (g_bActiveApp)
            {
                    CheckActivate(bActive);

                    g_bActiveGame = bActive;
            }
    }
    break;
    */

    //---------------------------------------------------------------
    //
    //							WM_SIZE
    //
    //---------------------------------------------------------------
    // case WM_SIZE:
    // Check to see if we are losing our window...
    // if (SIZE_MAXHIDE==wParam || SIZE_MINIMIZED==wParam)
    //  g_bActiveGame = FALSE;
    // else
    //   g_bActiveGame = TRUE;
    // return 0L;

    //---------------------------------------------------------------
    //
    //							WM_DESTROY
    //
    //---------------------------------------------------------------
  case WM_DESTROY:
    // 2026-05-18 ImGui Hibrit Admin Menu: shutdown (context destroy + backend
    // shutdown). Initialize edilmediyse no-op (g_initialized=false).
    PhoenixImGui::Shutdown();
    // Clean up and close the app
    PostQuitMessage(0);
    return 0L;

  case WM_GETMINMAXINFO: {
    // Pencere boyutunu client == oyun cozunurlugu olacak sekilde kilitle.
    //
    // 2026-08-31 IZGARA ROOT CAUSE FIX: Eski VS6 hesabi
    // (SM_CXSIZEFRAME*2 / SM_CYSIZEFRAME*2 + SM_CYMENU) modern Windows
    // cercevesini eksik/yanlis hesapliyordu: padded border (8px) yok,
    // menusu olmayan pencereye SM_CYMENU ekleniyordu. Sonuc: dis boyut
    // 1288x988'e kilitleniyor, client 1272x949 kaliyordu (hedef 1280x960).
    // D3D9 Present her karede backbuffer'i kucuk client'a sikistirip 8
    // sutun + 11 satir atliyordu = ekranda seffaf izgara cizgileri.
    // Cozum: gercek pencere stiliyle AdjustWindowRectEx — tema/surum
    // bagimsiz dogru dis boyut. Cozunurluk resolution.inf'ten dinamik.
    MINMAXINFO *pMinMax = (MINMAXINFO *)lParam;

    const int nMinMaxW = (g_pUserInformation != NULL && g_pUserInformation->iResolution_x > 0)
                             ? g_pUserInformation->iResolution_x
                             : SURFACE_WIDTH;
    const int nMinMaxH = (g_pUserInformation != NULL && g_pUserInformation->iResolution_y > 0)
                             ? g_pUserInformation->iResolution_y
                             : SURFACE_HEIGHT;
    RECT rcMinMax = {0, 0, nMinMaxW, nMinMaxH};
    AdjustWindowRectEx(&rcMinMax,
                       (DWORD)GetWindowLong(hWnd, GWL_STYLE),
                       GetMenu(hWnd) != NULL,
                       (DWORD)GetWindowLong(hWnd, GWL_EXSTYLE));
    pMinMax->ptMinTrackSize.x = rcMinMax.right - rcMinMax.left;
    pMinMax->ptMinTrackSize.y = rcMinMax.bottom - rcMinMax.top;
    pMinMax->ptMaxTrackSize.x = pMinMax->ptMinTrackSize.x;
    pMinMax->ptMaxTrackSize.y = pMinMax->ptMinTrackSize.y;
  } break;

  case WM_PAINT:
    if (g_bActiveApp)
      CDirectDraw::Flip();
    return 0L;

  //---------------------------------------------------------------
  //
  //							WM_MOVE
  //
  //---------------------------------------------------------------
  case WM_MOVE:
    if (g_bActiveApp && !CDirectDraw::IsFullscreen())
      CDirectDraw::OnMove();
    break;

    /*
    case WM_MOUSEMOVE :
            //fwKeys = wParam;        // key flags
            //g_x = LOWORD(lParam);  // horizontal position of cursor
            //g_y = HIWORD(lParam);  // vertical position of cursor

            return 0L;
    */

    //---------------------------------------------------------------
    //
    //							WM_KEYDOWN
    //
    //---------------------------------------------------------------
  case WM_KEYDOWN:
#ifdef __WM_OUTPUT_DEBUG__
    DEBUG_ADD("[WM_KEYDOWN]");
#endif

    if (g_Mode == MODE_OPENING) {
      // Opening„ ‚¸‹¤.
      if (wParam == VK_ESCAPE || wParam == VK_RETURN || wParam == VK_SPACE) {
        if (g_pAvi != NULL) {
          g_pAvi->Close();
        }
        SetMode(MODE_MAINMENU);
      }
      return 0L;
    }

    gC_vs_ui.KeyboardControl(message, wParam, lParam);
    // Handle any non-accelerated key commands

    switch (wParam) {
    //-----------------------------------------------
    // Screen Shot
    //-----------------------------------------------
    case VK_SCROLL:

      if (!g_pOperatorOption->bSlideScreenShot) {
        MakeScreenShot();
      }

      // 					#ifdef OUTPUT_DEBUG
      // 						if
      // (!g_pOperatorOption->bSlideScreenShot)
      // 						{
      // 							MakeScreenShot();
      // 						}
      // 					#else
      // 						MakeScreenShot();
      // 					#endif

      return 0L;
// case VK_F11 :
//		g_UpdateDelay = (g_UpdateDelay==1)? DELAY_UPDATE_GAME : 1;
//		return 0L;

//-----------------------------------------------
// Toggle Music
//-----------------------------------------------
/*
case VK_F3 :
        //if (g_Music.IsPlay())
        if (g_DXMusic.IsPlay())
        {
                //g_Music.Pause();
                g_DXMusic.Pause();
        }
        else
        {
                //g_Music.Resume();
                g_DXMusic.Resume();
        }
return 0L;
*/

//-----------------------------------------------
// Volume ì¡°ì ˆ
//-----------------------------------------------
/*
case VK_F5 :
        {
                g_pMasterVolume->SetCurrentVolume(
                                g_pMasterVolume->GetCurrentVolume() - 0x1000
                        );
        }
        //if (g_pPlayer->IsExistAddon(MCreatureWear::ADDON_COAT))
        //	g_pPlayer->SetAddonNULL(MCreatureWear::ADDON_COAT);
        //else g_pPlayer->SetAddon(MCreatureWear::ADDON_COAT, 0);


return 0L;

case VK_F6 :
        {
                //g_DXMusic.AddVolume( 0xF000 );
                g_pMasterVolume->SetCurrentVolume(
                                g_pMasterVolume->GetCurrentVolume() + 0x1000
                        );
        }
        //if (g_pPlayer->IsExistAddon(MCreatureWear::ADDON_TROUSER))
        //	g_pPlayer->SetAddonNULL(MCreatureWear::ADDON_TROUSER);
        //else g_pPlayer->SetAddon(MCreatureWear::ADDON_TROUSER, 1);
return 0L;
*/

//-----------------------------------------------
//
//				Debug Mode š©
//
//-----------------------------------------------
#if defined(OUTPUT_DEBUG)

    case VK_PAUSE:
      g_bActiveApp = !g_bActiveApp;

      if (g_bActiveApp) {
        g_lGameRunBreakTime = timeGetTime() - g_CurrentTime;
      }

      return 0L;

    //-----------------------------------------------
    // Toggle ActiveMode
    //-----------------------------------------------
    // case VK_F1 :
    //	g_bActiveApp = !g_bActiveApp;
    //	return 0L;

    //-----------------------------------------------
    // ì§ì´Š” ë°©ë²• „¤ •
    //-----------------------------------------------
    /*
    case VK_F2 :
    {
            if (g_pPlayer->IsStop())
            {
                    // ì§ì´Š” ˜•ƒœ ë°”ê¾¸ê¸
                    //if
(g_pPlayer->GetMoveDevice()==MCreature::MOVE_DEVICE_NULL)
                    //{
                    //	g_pPlayer->SetMoveDevice( MCreature::MOVE_DEVICE_MOTOR1
);
                    //}/
                    //else
                    //{
                            //g_pPlayer->SetMoveDevice(
MCreature::MOVE_DEVICE_NULL );
                    //}

                    g_pPlayer->SetAction( ACTION_STAND );

                    int type = g_pPlayer->GetCreatureType();

                    type++;

                    if (type==g_CreatureTable.GetSize())
                    {
                            g_pPlayer->SetCreatureType( 0 );
                    }
                    else
                    {
                            g_pPlayer->SetCreatureType( type );
                    }

                    // ‚¨ž Slayer
                    if (g_pPlayer->GetCreatureType()==0)
                    {
                            g_pPlayer->RemoveAddon( ADDON_COAT );
                            g_pPlayer->RemoveAddon( ADDON_TROUSER );

                            MCoat		coat1;
                            MTrouser	trouser1;

                            coat1.SetItemType( 0 ); coat1.SetItemOption( 0 );
                            trouser1.SetItemType( 0 ); trouser1.SetItemOption( 0
);

                            g_pPlayer->SetAddonHair(ADDONID_HAIR1_MALE, 0);
// type, color g_pPlayer->SetAddonItem( &coat1 ); g_pPlayer->SetAddonItem(
&trouser1 );
                    }
                    else if (g_pPlayer->GetCreatureType()==1)
                    {
                            g_pPlayer->RemoveAddon( ADDON_COAT );
                            g_pPlayer->RemoveAddon( ADDON_TROUSER );

                            MCoat		coat1;
                            MTrouser	trouser1;

                            coat1.SetItemType( 0 ); coat1.SetItemOption( 0 );
                            trouser1.SetItemType( 0 ); trouser1.SetItemOption( 0
);

                            g_pPlayer->SetAddonHair(ADDONID_HAIR1_FEMALE, 0);
// type,

color
                            g_pPlayer->SetAddonItem( &coat1 );
                            g_pPlayer->SetAddonItem( &trouser1 );
                    }
                    else
                    {
                            g_pPlayer->RemoveAddon( ADDON_COAT );
                            g_pPlayer->RemoveAddon( ADDON_TROUSER );
                    }


                    // ë²ˆì©~~
                    g_pTopView->SetFadeStart(1, 31, 10,  31,0,0);
            }
    }
    return 0L;
    */

    /*
    case VK_F7 :
            //{
            //	pWaveVolume->SetCurrentVolume(
            //			pWaveVolume->GetCurrentVolume() - 0x1000
            //		);
            //}
            //if (g_pPlayer->IsExistAddon(MCreatureWear::ADDON_SHOES))
                    //g_pPlayer->SetAddonNULL(MCreatureWear::ADDON_SHOES);
            //else g_pPlayer->SetAddon(MCreatureWear::ADDON_SHOES, 2);

            // ê³µê²© ëª¨ë“œ  „™˜
            if (g_pPlayer->IsAttackModeAggress())
            {
                    g_pPlayer->SetAttackModePeace();
            }
            else if (g_pPlayer->IsAttackModePeace())
            {
                    g_pPlayer->SetAttackModeNormal();
            }
            else if (g_pPlayer->IsAttackModeNormal())
            {
                    g_pPlayer->SetAttackModeAggress();
            }

    return 0L;
    */

    /*
    case VK_F8 :
    {
            //{
                    ////g_DXMusic.AddVolume( 0xF000 );
                    //pWaveVolume->SetCurrentVolume(
                                    //pWaveVolume->GetCurrentVolume() + 0x1000
                            //);
            //}
            //if (g_pPlayer->IsExistAddon(MCreatureWear::ADDON_HELM))
            //	g_pPlayer->SetAddonNULL(MCreatureWear::ADDON_HELM);
            //else g_pPlayer->SetAddon(MCreatureWear::ADDON_HELM, 3);
            static int a = 0;

            if (a==0)
            {
                    gC_vs_ui.ChangeToVampireInterface();
                    a = 1;
            }
            else
            {
                    gC_vs_ui.ChangeToSlayerInterface();
                    a = 0;
            }
    }
    return 0L;
    */

    //-----------------------------------------------
    // Frame Fixing
    //-----------------------------------------------
    case VK_F11:
#if defined(_DEBUG)
      if (g_pDXInput->KeyDown(DIK_LCONTROL) ||
          g_pDXInput->KeyDown(DIK_RCONTROL)) {
        g_UpdateDelay = (g_UpdateDelay == 1) ? DELAY_UPDATE_GAME : 1;
      }
#endif
      return 0L;

    //-----------------------------------------------
    // debug ë©”ì„¸ì§ — VS6 dev stub (g_bPutMessage toggle) Phoenix modern
    // mimari icin GEREKSIZ. Memory entry doğrulamasi: "F1-F12 = skill atama
    // slot, VS6 dev stub'lari yanlis, dogru: gC_vs_ui.HotKey_FN()". Bu
    // WindowProc case g_bPutMessage'i true yapinca CGameUpdate.cpp:6860
    // debug print kod path tetikleniyor -> Release'de pPrintInfo / vector
    // access AV (F12 specific crash). F11 zaten `_DEBUG` guard'li Release'de
    // pasif — F12 da ayni davraniis olmali (kullanici karari 2026-05-18).
    // Disable comment-out: F12 mesaji WindowProc'ta consume edilmez,
    // DirectInput polling yakalar -> DXKeyboardEvent -> HotKey_F12 ->
    // m_pC_hotkey->HotKey_F12 (race-specific skill atama). F11 ile tutarli.
    // case VK_F12:
    //   g_bPutMessage = !g_bPutMessage;
    //   return 0L;

    //-----------------------------------------------
    // Debug Message ì²†Œ~•˜ê¸
    //-----------------------------------------------
    case VK_DELETE: {
#ifdef OUTPUT_DEBUG
      if (g_pDebugMessage != NULL)
        g_pDebugMessage->Clear();
#endif
    }
      return 0L;

#endif
    }

    return 0L;
    // 		case WM_TIMER :
    // 			//for (loop = 0; loop < STREAM_MAX; ++loop)
    // //					if (g_pOGG[loop] )
    // #ifndef __USE_MP3__
    // 			if( g_DXSound.IsInit() && g_pOGG != NULL )
    // 				g_pOGG->streamUpdate (g_pOGG);
    // #endif
    // 			break;

  //---------------------------------------------------------------
  //
  //							WM_CHAR
  //
  //---------------------------------------------------------------
  case WM_CHAR: {
#ifdef __WM_OUTPUT_DEBUG__
    DEBUG_ADD("[WM_CHAR]");
#endif

    /*
    {
            std::ofstream file("inputtest.txt", std::ios::app);
            file << "input!" << (char)wParam << std::endl;
            file.close();
    }
    */
    gC_vs_ui.KeyboardControl(message, wParam, lParam);

#ifdef __WM_OUTPUT_DEBUG__
    DEBUG_ADD("UI KeyCtrl OK");
#endif

    TCHAR ch = (TCHAR)wParam; // character code

    // switch (ch)
    //{
    //	case '`' : g_bDrawMinimap = !g_bDrawMinimap;
    //	return 0L;
    // }

#if defined(OUTPUT_DEBUG) && defined(_DEBUG)
    if (g_Mode == MODE_GAME)
      switch (ch) {
      ///*
      case '5': {
        /*
        for (int i=0; i<g_MaxNPC; i++)
        {
                MCreature* pCreature = g_pZone->GetCreature(1000+i);
                if (pCreature != NULL)
                {
                        if (pCreature->IsDead())
                        {
                                pCreature->SetAlive();
                        }
                        else
                        {
                                pCreature->SetDead();
                        }
                }
        }
        */
        if (g_pDXInput->KeyDown(DIK_LCONTROL) ||
            g_pDXInput->KeyDown(DIK_RCONTROL)) {
          if (g_pPlayer->IsDead()) {
            g_pPlayer->SetAlive();
            g_pTopView->SetFadeEnd();
          } else {
            // ì£½ì ê²½ìš°..
            g_pPlayer->SetDead();
          }
        }
      }
        return 0L;
      //*/

      // [ TEST CODE ]
      case '/': {
        // ƒƒœ ë³ê²
        /*
        int newHP = g_pPlayer->GetHP() - 5;
        int newMP = g_pPlayer->GetMP() - 3;

        if (newHP < 0) newHP = 0;
        if (newMP < 0) newMP = 0;

        MStatus* pStatus = new MStatus;
        pStatus->SetStatus(MODIFY_HP, newHP);
        pStatus->SetStatus(MODIFY_MP, newMP);

        // UIë³ê²
        gC_vs_ui.SetHP(newHP, 100);
        gC_vs_ui.SetMP(newMP, 100);

        // ê²°ê³¼— HPê° ë³•˜„ë¡ „¤ •
        MActionResult *pResult = new MActionResult;
        pResult->Add( new MActionResultNodeChangeStatus(g_pPlayer->GetID(),
        pStatus) );

        // ê²°ê³¼ Action “±ë¡
        //g_pPlayer->PacketSpecialActionResult( SKILL_ATTACK_MELEE );
        g_pPlayer->PacketSpecialActionResult( RESULT_VAMPIRE_DIE );

        // ê²°ê³¼ ‚´š© “±ë¡(effectIDê° žˆ–´•¼ ˜Š”°..)
        g_pPlayer->PacketAddActionResult(0, pResult);


        //switch (rand()%6)
        //{
        //	case 0 : g_pPlayer->SetChatString("huk!"); break;
        //	case 1 : g_pPlayer->SetChatString("kek!"); break;
        //	case 2 : g_pPlayer->SetChatString("uk!"); break;
        //	case 3 : g_pPlayer->SetChatString("-_-!"); break;
        //	case 4 : g_pPlayer->SetChatString("!!!"); break;
        //	case 5 : g_pPlayer->SetChatString("*_*;"); break;
        //}
        */
      }
        return 0L;
      //*/

      // ‚´ì§ ë¶‰ê²Œ ë³•˜ê¸
      case '6': {
        g_pTopView->SetFadeStart(25, 31, 2, 31, 0, 0);
      } break;

      // ë²ˆê°œ
      case '7': {
        SetLightning(rand() % 4 * 500 + 500);
      } break;

      case '.': {
        do {
          int newActionInfo;

          if (g_pPlayer->GetSpecialActionInfo() == ACTIONINFO_NULL) {
            newActionInfo = MAGIC_LIGHT;
          } else {
            newActionInfo = g_pPlayer->GetSpecialActionInfo() + 1;
          }

          if (newActionInfo >= (*g_pActionInfoTable).GetMinResultActionInfo())
            newActionInfo = 0;

          int maxPlayerAction = g_pPlayer->GetActionMax();
          int newActionInfoAction =
              (*g_pActionInfoTable)[newActionInfo].GetAction();

          g_pPlayer->SetSpecialActionInfo(newActionInfo);

          // player— žˆŠ” action¸ ê²½ìš°..
          if (newActionInfoAction <= maxPlayerAction)
            break;
        } while (1);
      }
        return 0L;

      case ',': {
        do {
          int newActionInfo;

          if (g_pPlayer->GetSpecialActionInfo() == ACTIONINFO_NULL) {
            newActionInfo = MAGIC_LIGHT;
          } else {
            newActionInfo = g_pPlayer->GetSpecialActionInfo() - 1;
          }

          if (newActionInfo < 0)
            newActionInfo = (*g_pActionInfoTable).GetMinResultActionInfo() - 1;

          int maxPlayerAction = g_pPlayer->GetActionMax();
          int newActionInfoAction =
              (*g_pActionInfoTable)[newActionInfo].GetAction();

          g_pPlayer->SetSpecialActionInfo(newActionInfo);

          // player— žˆŠ” action¸ ê²½ìš°..
          if (newActionInfoAction <= maxPlayerAction)
            break;
        } while (1);
      }
        return 0L;
      case 'L':
        g_pPlayer->AddEffectStatus(EFFECTSTATUS_SHRINE_SHIELD, 0xffff);
        return 0L;
      }
#endif
  }
    return 0L;

  /*
case WM_SETCURSOR:
// Turn off the cursor since this is a full-screen app
//if (CDirectDraw::IsActive() )//&& CDirectDraw::IsFullscreen())
          {
                  //SetCursor(LoadCursor(NULL, IDC_ARROW));
                  //SetCursor(LoadCursorFromFile("Cursor2.cur"));
                  //SetCursor(LoadCursor(g_hInstance,
MAKEINTRESOURCE(IDC_MYCROSS)));

                  //SetCursor(LoadCursorFromFile("Mono.cur"));
                  //SetCursor( LoadCursor(g_hInstance,
MAKEINTRESOURCE(IDC_TEST)) );

                  //SetCursor( LoadIcon(NULL, IDI_ERROR) );

                  SetCursor(NULL);

                  SetCapture(g_hWnd);
          }
return TRUE;
  */
  // `•œê¸ž… ¥‹œ IME‘œ‹œê° ‚˜˜¤ì§ ëª»í•˜„ë¡ •œ‹¤.
  //

  //---------------------------------------------------------------
  //
  //				WM_KEYUP
  //				WM_IME_COMPOSITION
  //				WM_IME_STARTCOMPOSITION
  //				WM_IME_ENDCOMPOSITION
  //
  //---------------------------------------------------------------
  case WM_KEYUP:
  case WM_IME_COMPOSITION:
  case WM_IME_STARTCOMPOSITION:
  case WM_IME_ENDCOMPOSITION:

#ifdef __WM_OUTPUT_DEBUG__
    switch (message) {
    case WM_KEYUP:
      DEBUG_ADD("[WM_KEYUP]");
      break;

    case WM_IME_COMPOSITION:
      DEBUG_ADD("[WM_IME_COMPOSITION]");
      break;

    case WM_IME_STARTCOMPOSITION:
      DEBUG_ADD("[WM_IME_STARTCOMPOSITION]");
      break;

    case WM_IME_ENDCOMPOSITION:
      DEBUG_ADD("[WM_IME_ENDCOMPOSITION]");
      break;
    }
#endif

    if (((wParam == 244 || wParam == 243 || wParam == 240) && message == 257) ||
        ((wParam == 256) && message == 257) ||
        ((wParam == 28) && message == 257 && !gC_ci->IsEngInput() &&
         gC_ci->IsJapanese())) {
      if (wParam == 240 || wParam == 28) {
        gC_ci->SetComposion(false);
      }
      bool bComposion = gC_ci->GetComposion();
      gC_ci->SetComposion(!bComposion);
    }

    gC_vs_ui.KeyboardControl(message, wParam, lParam);

#ifdef __WM_OUTPUT_DEBUG__
    DEBUG_ADD("[WM_IME_....] OK");
#endif

    if (message == WM_IME_STARTCOMPOSITION)
      return 0;

    break; // ! break!!!!
  }

#ifdef __WM_OUTPUT_DEBUG__
  DEBUG_ADD("[WM_....]");
#endif

  return DefWindowProc(hWnd, message, wParam, lParam);
}
#endif // Disable duplicate LNK2005

// 2026-08-31 IZGARA TANI: lifecycle log (her zaman yazar) — asagida tanimli.
static void Phoenix_WriteLifecycleLog(const char *message);

//-----------------------------------------------------------------------------
// Name: InitApp()
// Desc: Do work required for every instance of the application:
//          Create the window, initialize data
//-----------------------------------------------------------------------------
BOOL InitApp(int nCmdShow) {
  WNDCLASS wc;

  // Set up and register window class
  wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
  wc.lpfnWndProc = WindowProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = g_hInstance;
  wc.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_DARKEDEN));
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName = NULL; // NAME;
  wc.lpszClassName = PROGRAM_NAME;
  RegisterClass(&wc);

  // Calculate the proper size for the window given a client of 640x480
  int cx, cy;
  DWORD style = 0;
  DWORD exStyle = 0;

#ifdef __WEB_BROWSER__
  CoInitialize(NULL);
  if (HRESULT hr = FAILED(
          CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER,
                           IID_IWebBrowser2, (LPVOID *)&g_pWebBrowser))) {
    CT_Done("InitApp FAIL: IWebBrowser2 Create failed -> returning FALSE");
    return FALSE;
  }
#endif

  memset(g_macAddress, 0, 6 * sizeof(BYTE));
  // if(GetMacAddressFromSock())

  // MacAddress Operator Check
  if (GetMacAddressFromNetBIOS(g_macAddress)) {
    bool bMacAddressCheck = CheckMacScreenMode();
    g_pUserInformation->attrOperator.SetAttr(OPERATORTYPE_MACADDRESS,
                                             bMacAddressCheck);
  }

  // RelaseLog Operator Check
  // 2026-05-20 ROOT CAUSE FIX: OUTPUT_DEBUG Phoenix Designed_Chs CMakeLists.txt:2153
  // tarafindan Release build'de DE tanimli (DEBUG_ADD genel logging icin). Eski kod
  // OUTPUT_DEBUG ile RELEASELOG bit'i set ediyordu -> Release build'de bile her
  // player'da attrOperator.RELEASELOG=true -> admin menu (LSHIFT+RSHIFT + ImGui DEL)
  // tum karakterlerde acilir (CompetenceFix sonrasi PLAYERSHAPE false olsa bile).
  // Sadece _DEBUG (gercek Debug build, MSVC otomatik define) ile set et.
#if defined(_DEBUG)
  bool bReleaseLogCheck = true;
  g_pUserInformation->attrOperator.SetAttr(OPERATORTYPE_RELEASELOG, bReleaseLogCheck);
#endif

  // œˆ„š° ëª¨ë“œë¡  ‘†•´ BoAš”~
  // œˆ„š° ëª¨ë“œ ¼ë° ê³µê°œ
  // Windowed mode forced for verifying the fix
  bool bForceModeLog = true;
  if (bForceModeLog || g_pUserInformation->attrOperator.GetValue()) // by viva
  {
    int nAnswer = IDNO;
    CT_Done("DIAG: InitApp - Mode selection bypassed, g_bFullScreen from Resolution.inf preserved");

    if (nAnswer == IDCANCEL){
      CT_Done("InitApp FAIL: Display Mode selection CANCELLED -> returning FALSE");
      return FALSE;
    }

    // g_bFullScreen override removed - value from InitResolutionConfig (Resolution.inf) is preserved
  }

  // 2004, 8, 27, sobeit add end - mac address ì²´í¬ ë°, mac screen mode ì²´í¬
  if (g_bFullScreen) {
    exStyle = WS_EX_TOPMOST | WS_EX_APPWINDOW;
    style = WS_POPUP | WS_CLIPCHILDREN;
    // Exclusive D3D9: HWND client boyutu backbuffer ile AYNI olmali.
    // SM_CXSCREEN (ornegin 2560x1440) + oyun 1280x960 = Present gri/bos ekran.
    cx = (g_pUserInformation != NULL && g_pUserInformation->iResolution_x > 0)
             ? g_pUserInformation->iResolution_x
             : SURFACE_WIDTH;
    cy = (g_pUserInformation != NULL && g_pUserInformation->iResolution_y > 0)
             ? g_pUserInformation->iResolution_y
             : SURFACE_HEIGHT;
    CT_Done("DIAG: InitApp - Fullscreen HWND sized to game resolution %dx%d (not desktop)",
            cx, cy);
  } else {
    // Disabled desktop 16-bit force for modern Windows compatibility
    /*
    DEVMODE devModeScreen;
    memset(&devModeScreen, 0, sizeof(devModeScreen));
    devModeScreen.dmSize = sizeof(DEVMODE);
    devModeScreen.dmFields = DM_BITSPERPEL;
    devModeScreen.dmBitsPerPel = 16;

    if (ChangeDisplaySettings(&devModeScreen, CDS_TEST) ==
        DISP_CHANGE_SUCCESSFUL)
      ChangeDisplaySettings(&devModeScreen, 0);
    else
      CT_Done("");
    */

    exStyle = WS_EX_APPWINDOW;
    style = WS_OVERLAPPEDWINDOW | WS_VISIBLE |
            WS_CLIPCHILDREN; // WS_CLIPCHILDREN | WS_OVERLAPPED | WS_THICKFRAME
                             // | WS_MINIMIZEBOX;//WS_OVERLAPPEDWINDOW;
    style &= ~WS_MAXIMIZEBOX;
    
    // Dynamically use resolution from INF config instead of hardcoded SURFACE_WIDTH/HEIGHT
    int targetWidth = (g_pUserInformation != NULL) ? g_pUserInformation->iResolution_x : SURFACE_WIDTH;
    int targetHeight = (g_pUserInformation != NULL) ? g_pUserInformation->iResolution_y : SURFACE_HEIGHT;

    RECT rc = { 0, 0, targetWidth, targetHeight };
    AdjustWindowRectEx(&rc, style, FALSE, exStyle);
    cx = rc.right - rc.left;
    cy = rc.bottom - rc.top;

    CT_Done("DIAG: InitApp - Window rect adjusted: Width=%d, Height=%d (Client area target: %dx%d)", 
               cx, cy, targetWidth, targetHeight);
  }

  // Create a window at (0,0) to prevent off-screen spawning
  g_hWnd = CreateWindowEx(exStyle, // 0,
                          PROGRAM_NAME, PROGRAM_TITLE, style, 0,
                          0, cx, cy, NULL, NULL, g_hInstance, NULL);

  if (!g_hWnd) {
    DWORD err = GetLastError();
    char b[128];
    sprintf(b, "InitApp FAIL: CreateWindowEx returned NULL! GetLastError=%d", err);
    CT_Done("%s", b);
    MessageBox(NULL, b, "Debug", MB_OK);
    return FALSE;
  }
  CT_Done("DIAG: CreateWindowEx SUCCESS, hWnd=%p", g_hWnd);
  /*
  #ifdef __NPROTECT__

  #ifdef __NPROTECT_OLD_VERSION__
          int nProtectResult ;
          // ### nProtect ###
          if( (nProtectResult = RunNPROTECT()) != NPROTECT_OK )
          {
                  char szTemp[256];
                  wsprintf(szTemp,"nProtectê° ‹¤–‰˜ì§ •Š•˜Šµ‹ˆ‹¤.\në§¤í¬ë¡
  ”„ë¡œê·¸ž¨´‚˜ •´‚¹ ”„ë¡œê·¸ž¨„ ‚¬š©•œ ê²½ìš°ê° •„‹ˆ¼ë© \n bug@darkeden.com œ¼ë¡
  ë©”ì¼„ ë³´ë‚´ì£¼ì‹œê¸ ë°”ëž‹ˆ‹¤.[ErrorCode:%d]",nProtectResult); MessageBox(NULL,
  szTemp,"nProtect Error", MB_OK); CloseNPROTECT(); return FALSE;
          }

          if(g_bForceExitBynProtect2 )
          {
                  CloseNPROTECT();
                  return FALSE;
          }

  #elif !defined(__NPROTECT_OLD_VERSION__)
          if( RunNPROTECT() != NPROTECT_OK )
          {
                  CloseNPROTECT();
                  return FALSE;
          }

          if( g_bForceExitBynProtect == true )
          {
                  char szTemp[256];

                  sprintf(szTemp,"NProtect˜ %d —Ÿ¬ ž…‹ˆ‹¤. ‹¤¬—´„
  ì¢…ë£Œ•©‹ˆ‹¤.",g_nProtectErrorMessage); MessageBox(NULL,szTemp, "Error", MB_OK);
                  return FALSE;
          }
  #endif

  #endif*/

  //
  // ShowWindow(g_hWnd, nCmdShow);
  // UpdateWindow(g_hWnd);
  // SetFocus(g_hWnd);

  //---------------------------------------------------
  // DirectX Version check
  //---------------------------------------------------
  // if (!CheckDXVersion())
  //{
  //	return 0;
  //}

  // cursorë¥ —†• ì¤‹¤.
  SetCursor(NULL);
  ShowCursor(FALSE);

  // windowë¥ ë³´ì—¬ì¤‹¤.
  CT_Done("DIAG: InitApp - Calling ShowWindow/UpdateWindow");
  SetWindowPos(g_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW); 
  BOOL bShowRet = ShowWindow(g_hWnd, nCmdShow);
  BOOL bUpdateRet = UpdateWindow(g_hWnd);
  CT_Done("DIAG: InitApp - ShowWindow Ret:%d, UpdateWindow Ret:%d", bShowRet, bUpdateRet);

  // 2026-08-31 IZGARA FIX guvenlik agi: Asil kok neden WM_GETMINMAXINFO
  // handler'inin eski VS6 cerceve hesabiyla pencereyi kucuk kilitlemesiydi
  // (duzeltildi, WndProc'a bak). Bu blok savunma katmani: client hedeften
  // hala farkliysa fark kadar pencereyi buyutur (tema/DPI bagimsiz,
  // kendini duzelten yontem). Fullscreen'de gerek yok.
  if (!g_bFullScreen) {
    RECT rcClientFix = {0, 0, 0, 0};
    GetClientRect(g_hWnd, &rcClientFix);
    const int nFixTargetW = (g_pUserInformation != NULL && g_pUserInformation->iResolution_x > 0)
                                ? g_pUserInformation->iResolution_x
                                : SURFACE_WIDTH;
    const int nFixTargetH = (g_pUserInformation != NULL && g_pUserInformation->iResolution_y > 0)
                                ? g_pUserInformation->iResolution_y
                                : SURFACE_HEIGHT;
    const int nDeltaW = nFixTargetW - (int)(rcClientFix.right - rcClientFix.left);
    const int nDeltaH = nFixTargetH - (int)(rcClientFix.bottom - rcClientFix.top);
    if (nDeltaW != 0 || nDeltaH != 0) {
      RECT rcWindowFix = {0, 0, 0, 0};
      GetWindowRect(g_hWnd, &rcWindowFix);
      SetWindowPos(g_hWnd, NULL, 0, 0,
                   (int)(rcWindowFix.right - rcWindowFix.left) + nDeltaW,
                   (int)(rcWindowFix.bottom - rcWindowFix.top) + nDeltaH,
                   SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
  }

  // 2026-08-31 IZGARA TANI (kural 9.2): client alani backbuffer hedefinden
  // farkliysa D3D9 Present her karede goruntuyu nearest olcekler; duzenli
  // araliklarla satir/sutun atlanir = "seffaf izgara" cizgileri + yazi
  // bozulmasi. Tek seferlik satir, lifecycle loguna her zaman yazilir.
  // (Fix sonrasi client==target olmali — bu satir dogrulama gorevi gorur.)
  {
    RECT rcClientGeom = {0, 0, 0, 0};
    RECT rcWindowGeom = {0, 0, 0, 0};
    RECT rcWorkGeom = {0, 0, 0, 0};
    GetClientRect(g_hWnd, &rcClientGeom);
    GetWindowRect(g_hWnd, &rcWindowGeom);
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkGeom, 0);
    const int nGeomTargetW = (g_pUserInformation != NULL && g_pUserInformation->iResolution_x > 0)
                                 ? g_pUserInformation->iResolution_x
                                 : SURFACE_WIDTH;
    const int nGeomTargetH = (g_pUserInformation != NULL && g_pUserInformation->iResolution_y > 0)
                                 ? g_pUserInformation->iResolution_y
                                 : SURFACE_HEIGHT;
    char szGeomLine[512] = {0};
    sprintf(szGeomLine,
            "WINDOWGEOM client=%ldx%ld target=%dx%d window=(%ld,%ld)-(%ld,%ld) "
            "work=%ldx%ld screen=%dx%d fullscreen=%d",
            (long)(rcClientGeom.right - rcClientGeom.left),
            (long)(rcClientGeom.bottom - rcClientGeom.top),
            nGeomTargetW, nGeomTargetH,
            (long)rcWindowGeom.left, (long)rcWindowGeom.top,
            (long)rcWindowGeom.right, (long)rcWindowGeom.bottom,
            (long)(rcWorkGeom.right - rcWorkGeom.left),
            (long)(rcWorkGeom.bottom - rcWorkGeom.top),
            GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
            g_bFullScreen ? 1 : 0);
    Phoenix_WriteLifecycleLog(szGeomLine);
  }

  // Resolution & Coordinate Sync for Windowed Mode.
  // CDirectDraw is initialized later via InitGame()->InitDraw(); OnMove/Flip
  // now self-guard against pre-Init NULL state (see CDirectDraw.cpp).
  if (!g_bFullScreen) {
    CT_Done("DIAG: InitApp - Windowed mode, calling CDirectDraw::OnMove/Flip (guarded)");
    CDirectDraw::OnMove();
    CDirectDraw::Flip();
    CT_Done("DIAG: InitApp - CDirectDraw early sync complete");
  }

  CT_Done("DIAG: InitApp - Calling SetFocus");
  SetFocus(g_hWnd);
  CT_Done("DIAG: InitApp - SetFocus complete, proceeding to nProtect/InitGame");
// by viva
#ifdef __NPROTECT__

#ifdef __NPROTECT_OLD_VERSION__
  int nProtectResult;
  // ### nProtect ###
  if ((nProtectResult = RunNPROTECT()) != NPROTECT_OK) {
    char szTemp[256];
    wsprintf(szTemp,
             "nProtect has not been launched.\n Unless you have used a macro or hacking programme, \n please contact via Discord.[ErrorCode:%d]",
             nProtectResult);
    MessageBox(NULL, szTemp, "nProtect Error", MB_OK);
    CloseNPROTECT();
    return FALSE;
  }

  if (g_bForceExitBynProtect2) {
    CloseNPROTECT();
    return FALSE;
  }

#elif !defined(__NPROTECT_OLD_VERSION__)
#if __CONTENTS(__NPROTECT)
  if (RunNPROTECT() != NPROTECT_OK) {
    CT_Done("InitApp FAIL: RunNPROTECT() != NPROTECT_OK -> returning FALSE");
    CloseNPROTECT();
    return FALSE;
  }

  if (g_bForceExitBynProtect == true) {
    char szTemp[256];

    sprintf(szTemp, "This is an NProtect %d error. The Game is closing.",
            g_nProtectErrorMessage);
    MessageBox(NULL, szTemp, "Error", MB_OK);
    return FALSE;
  }
#endif //__NPROTECT
#endif

#endif

  //---------------------------------------------------
  // Init Game
  //---------------------------------------------------
  CT_Done("DIAG: InitApp calling InitGame()...");
  BOOL bRet = InitGame();
  CT_Done("DIAG: InitApp -> InitGame() returned %d", (int)bRet);
  return bRet;
}

// extern void OutputMemorySize();
// #include "MemoryCheck.cpp"
// #include "UIDialog.h"

//-----------------------------------------------------------------------------
// Check TerriblePatch
//-----------------------------------------------------------------------------
bool CheckTerriblePatch() {
  // Legacy bypass: This patch mechanism is no longer needed and causes invisible crashes.
  return true;
}

//-----------------------------------------------------------------------------
// Check TerriblePatch
//-----------------------------------------------------------------------------
// auto-patch˜ ‹¤ˆ˜ë¡ ¸•˜—¬
// –´ì© ˆ˜ —†´ ‹¤–‰™”¼—„œ ì²´í¬•´„œ Œ¨ì¹˜í•´•¼•˜Š” ê²½ìš°´‹¤.
// ê·¸ë¦¬•˜—¬.. •¨ˆ˜ ´ë¦„ì´ ´ ‡ê² ˜—ˆ‹¤.  --;;
//-----------------------------------------------------------------------------
/*
bool
CheckTerriblePatchOLD()
{
        bool bCrash = false;
        int errorCode = 0;

        //-----------------------------------------------------------------------
        //
        // Update SpritePack
        //
        //-----------------------------------------------------------------------
        // Data\\Image\\— New18ImageObjectSPK.spkê° žˆœ¼ë©..
        // ImageObject˜ ê°œìˆ˜Š”  „ì²´ê°œˆ˜ ê°™ì°.. š©Ÿ‰´ ‹¤ë¥´ë‹¤ë©
        // "Œ¨ì¹ ž‘—…„ •˜Š”° ‹œê°„ì´ ì¢ ê±¸ë¦½‹ˆ‹¤."¼ê³ ë©”ì‹œì§ë¥ „›Œì¤‹¤.
        //  „ì²´ê°œˆ˜ë§Œí¼ SPK ¬ê¸°ë ž¡ê³
        // ImageObjectSPKë¥ SPKë¡ Load•œ‹¤.
        // New18ImageObjectSPKë¥ SPKë¡ Load•œ‹¤.
        // SPKë¥ ImageObjectSPK— write•œ‹¤.
        // New18ImageObjectSPK.spkë¥ ì§š´‹¤.
        //
        // [22ë² Œ¨ì¹˜í• •Œë¶„°Š”..]
        // ê¸°ì¡´— ž˜ëª»ëœ ë¶ë¶„ì´ ‹¤ ®ë²„ë¦´ •„˜ Œ¨ì¹˜í™”¼´ ì¤ë¹„ë˜–´ žˆœ¼ë¯ë¡
        // 18Œ¨ì¹˜í™”¼´ žˆœ¼ë© 18ë² Œ¨ì¹˜ìœ„ì¹˜ë„° “°ê³
        // 22Œ¨ì¹˜í™”¼´ žˆœ¼ë© 22ë² Œ¨ì¹˜ìœ„ì¹˜ë„° “°ë©... œ‹¤.
        const int numWrite = 2;
        char newSpkFilename[numWrite][80] =
        {
                "Data\\Image\\New18ImageObject.spk",
                "Data\\Image\\New22ImageObject.spk"
        };
        const long writePosition[numWrite] =
        {
                47564158,	// 18ë² Œ¨ì¹  š©•  file position
                49532618	// 22ë² Œ¨ì¹  š©›„ file position
        };
        const WORD orgSpkSize[numWrite] =
        {
                0x054C,		// 18ë² Œ¨ì¹  š©›„ ê°œìˆ˜
                0x05D6		// 22ë² Œ¨ì¹  š©›„ ê°œìˆ˜
        };
        const long orgFileSize[numWrite] =
        {
                49532618,	// 18ë² Œ¨ì¹  š©›„ ™”¼‚¬´ì¦
                55239016	// 22ë² Œ¨ì¹  š©›„ ™”¼‚¬´ì¦
        };

        WORD spkSize;
        std::ifstream imageObjectFile;//(FILE_ISPRITEINDEX_CREATURE,
std::ios::binary); if (!FileOpenBinary(FILE_SPRITE_IMAGEOBJECT,
imageObjectFile)) return false; imageObjectFile.read((char*)&spkSize, 2);
// Sprite˜ ê°œìˆ˜ imageObjectFile.close();

        //-----------------------------------------------------------------------
        // š©Ÿ‰ ‹¤ë¥´ë©´ Œ¨ì¹
        //-----------------------------------------------------------------------
        bool bImageObjectPatch;
        if (spkSize==orgSpkSize[numWrite-1])
        {
                 bImageObjectPatch = false;
        }
        else
        {
                bImageObjectPatch = true;
        }

        //-----------------------------------------------------------------------
        // Œ¨ì¹˜ë  š©‹œì¼œì•¼ •˜Š” ê²½ìš°
        //-----------------------------------------------------------------------
        if (bImageObjectPatch)
        {
                //CT_Done("‹¤¬—´ °´ë¥  •ë¦¬í•˜ê³ ‹¤–‰• 
˜ˆ •ž…‹ˆ‹¤.\n OKë¥ ˆ„ë¥´ê³  ë°˜ì‘´ —†”¼„ ž™œ¼ë¡ ‹¤–‰˜‹ˆ ž ‹œë§ ê¸

‹¤ ¤ì£¼ì„¸š”.");

                for (int i=0; i<numWrite; i++)
                {
                        std::ifstream newfile(newSpkFilename[i],
std::ios::binary | std::ios::nocreate);

                        //-------------------------------------------------------------
                        // Œ¨ì¹ ™”¼´ —†Š” ê²½ìš°.. ‹¤Œêº ì²´í¬
                        //-------------------------------------------------------------
                        if (!newfile.is_open())
                        {
                                continue;
                        }
                        else
                        {
                                SetProgressBarText("Œ¨ì¹ ™”¼„  š©‹œ‚¤ê³
žˆŠµ‹ˆ‹¤."); UpdateProgressBar();

                                std::ofstream
imageObjectFile(FILE_SPRITE_IMAGEOBJECT, std::ios::binary | std::ios::ate);

                                newfile.seekg( 2 );	// sizeë¶ë¶  œ™¸
                                imageObjectFile.seekp( writePosition[i] );

                                WRITE_FROM_FILE( imageObjectFile, newfile );

                                newfile.close();

                                // ê°œìˆ˜ ë³ê²
                                spkSize = orgSpkSize[i];
                                imageObjectFile.seekp( 0, std::ios::beg );
                                imageObjectFile.write((const char*)&spkSize, 2);
                                imageObjectFile.close();

                                // Œ¨ì¹˜í™”¼„ ì§š´‹¤.
                                remove( newSpkFilename[i] );
                        }
                }
        }


        //-----------------------------------------------------------------------
        //
        // Append SpritePack
        //
        //-----------------------------------------------------------------------
        //
        //-----------------------------------------------------------------------
        // Data\\Image\\New18Creature.ispk - 2001.9.26
        // ›ž˜žˆ˜™”¼, ì¶”ê˜Š”™”¼, •©•œ›„ê°œìˆ˜
        // ´ ë°©ì‹—„œŠ” spkiŠ” ”°ë¡ ®–´•¼ •œ‹¤.
        if (!bCrash)
        {
                const int numAppend = 2;
                char orgFilename[numAppend][80] =
                {
                        "Data\\Image\\Creature.ispk",
                        "Data\\Image\\Creature.ispk"
                };
                char appFilename[numAppend][80] =
                {
                        "Data\\Image\\New18Creature.ispk",
                        "Data\\Image\\New22Creature.ispk"
                };

                int total[numAppend] =
                {
                        0x2E66,
                        0x35FE
                };

                // —¬Ÿ¬ë² append•´„œ total— „‹¬•˜ê² ˜Š” ê²½ìš°ê° žˆ‹¤.

                WORD orgNum, appNum;

                for (int i=0; i<numAppend; i++)
                {

                        class std::fstream orgFile(orgFilename[i], std::ios::in
| std::ios::out | std::ios::binary | std::ios::nocreate | std::ios::ate);
std::ifstream appFile(appFilename[i], std::ios::binary | std::ios::nocreate);

                        //-------------------------------------------------------------
                        // ›ž˜ ™”¼´ —†Š” ê²½ìš° - -;
                        //-------------------------------------------------------------
                        if (!orgFile.is_open())
                        {
                                bCrash = true;
                                errorCode = 1;
                                break;
                        }

                        orgFile.seekg( 0, std::ios::beg );
                        orgFile.read((char*)&orgNum, 2);	// Sprite˜ ê°œìˆ˜

                        if (orgNum>=total[i])
                        {
                                // ´ë¯ Œ¨ì¹˜ëœ ƒ™©´‹¤. ‹¤Œêº ì²´í¬.
                                orgFile.close();
                                appFile.close();
                                continue;
                        }

                        //-------------------------------------------------------------
                        // app• ê» —†Š” ê²½ìš°
                        //-------------------------------------------------------------
                        if (!appFile.is_open())
                        {
                                // ê°œìˆ˜Š” ‹¤ë¥¸ë° app• ê²ƒì´ —†Š” ê²½ìš°
                                bCrash = true;
                                errorCode = 2;
                                break;
                        }

                        // total sizeë¥ ì²´í¬•´„œ append•  •„š”ê° žˆŠ”ì§
ì²´í¬•œ‹¤. appFile.read((char*)&appNum, 2);	// Sprite˜ ê°œìˆ˜

                        //-------------------------------------------------------------
                        // ‘ê° •©ì³ì„œ total´ œ‹¤ë©..
                        //-------------------------------------------------------------
                        if (orgNum+appNum == total[i])
                        {
                                SetProgressBarText("Œ¨ì¹ ™”¼„  š©‹œ‚¤ê³
žˆŠµ‹ˆ‹¤."); UpdateProgressBar();

                                // orgFile˜ œ¼ë¡..
                                orgFile.seekp(0, std::ios::end);

                                WRITE_FROM_FILE( orgFile, appFile );

                                // ›ž˜ ™”¼˜ ê°œìˆ˜ë¥ ë°”ê¿”ì¤‹¤.
                                orgFile.seekp(0, std::ios::beg);
                                orgFile.write((const char*)&total[i], 2);

                                orgFile.close();
                                appFile.close();

                                //---------------------------------------------------------------
                                // App™”¼ ì§š´‹¤.
                                //---------------------------------------------------------------
                                remove( appFilename[i] );
                        }
                        //-------------------------------------------------------------
                        // size ì½©êë£.. - -;
                        //-------------------------------------------------------------
                        else
                        {
                                // ‘ ê° •©ì³ë„  œë¡œëœ ˆ«žê° ‚˜˜¤ì§ •ŠŠ” ê²½ìš°
                                // ‹¬ê°í•œ ƒ™©´‹¤.
                                bCrash = true;
                                errorCode = 3;
                                break;
                        }
                }
        }

        //-----------------------------------------------------------------------
        // ‹¬ê°í•œ ƒ™©´‹¤.
        //-----------------------------------------------------------------------
        if (bCrash)
        {
                char str[256];
                sprintf(str, "™”¼´ †ƒ˜—ˆŠµ‹ˆ‹¤. ‹¤¬—´ š´˜œ¼ë¡
—°½ì£¼ì„¸š” [—Ÿ¬ì½”ë“œ:%d]", errorCode); MessageBox(NULL, str, PROGRAM_TITLE,
MB_OK);

                return false;
        }

        return true;
}
*/

//-----------------------------------------------------------------------------
// ConvertScreenEffect
//-----------------------------------------------------------------------------
// ê¸°ì¡´˜ AlphaEffectë¥ ScreenEffect— ë§žê²Œ ë°”ê¾¼‹¤.
//-----------------------------------------------------------------------------
/*
bool
ConvertScreenEffect()
{
        //------------------------------------------------------------
        // Screen SpritePack ™•¸
        //------------------------------------------------------------

//	std::ifstream fileSPK2(FILE_SPRITE_SCREENEFFECT, std::ios::binary |
std::ios::nocreate);
//	if (fileSPK2.is_open())
//	{
//		TYPE_SPRITEID num;
//		fileSPK2.read((char*)&num, SIZE_SPRITEID);
//
//		// ¬ê¸°ê ê°™ìœ¼ë©.. convert•  •„š” —†‹¤.
//		if (num >= 0x06BE)
//		{
//			return false;
//		}
//
//		fileSPK2.close();
//	}

/*	std::ifstream fileSPK2("Data\\Info\\EffectScreenConvert.inf",
std::ios::binary | std::ios::nocreate); if (fileSPK2.is_open())
        {
                fileSPK2.close();
                return false;
        }


        const int MAX_EST = 135;
        const int est[MAX_EST] =
        {
                SCR_EFFECTSPRITETYPE_ACID_BALL_1,		// ‚ •„ê°ê¸
                SCR_EFFECTSPRITETYPE_ACID_BALL_2,		// „°ì§ê¸
                SCR_EFFECTSPRITETYPE_ACID_BOLT_1,		// ‚ •„ê°ê¸
                SCR_EFFECTSPRITETYPE_ACID_BOLT_2,		// „°ì§ê¸
                SCR_EFFECTSPRITETYPE_ACID_TOUCH,
                SCR_EFFECTSPRITETYPE_AURA_PRISM_SHIELD,	// ë°©ì–´ë§
                SCR_EFFECTSPRITETYPE_AURA_BALL_1,	// ìºìŠ¤Œ…
                SCR_EFFECTSPRITETYPE_AURA_BALL_2,	// ‚ •„ê°ê¸
                SCR_EFFECTSPRITETYPE_AURA_BALL_3,	// „°ì§ê¸
                SCR_EFFECTSPRITETYPE_AURA_PRISM_1,	// ìºìŠ¤Œ… - ë°”ë‹¥— ë¶™ëŠ”
                SCR_EFFECTSPRITETYPE_AURA_PRISM_2,	// ìºìŠ¤Œ… - ë°”ë‹¥—„œ
œ„ë¡ ˜¬¼ê°ê¸ SCR_EFFECTSPRITETYPE_AURA_PRISM_3,	// ë¶™ì–´„œ ‹œž‘
                SCR_EFFECTSPRITETYPE_AURA_PRISM_4,	// ë¶™ì–´„œ ë°˜ë³µ
                SCR_EFFECTSPRITETYPE_AURA_PRISM_5,	// ë¶™ì–´„œ 
                SCR_EFFECTSPRITETYPE_AURA_SHIELD_1,	// ìºìŠ¤Œ…
                SCR_EFFECTSPRITETYPE_AURA_SHIELD_2,	// ì§†
                SCR_EFFECTSPRITETYPE_AURA_SHIELD_3,	// ë§žì„ •Œ ë²ˆì©~
                SCR_EFFECTSPRITETYPE_AURA_RING,
                SCR_EFFECTSPRITETYPE_BLESS_GROUND_1,
                SCR_EFFECTSPRITETYPE_BLESS_GROUND_2,
                SCR_EFFECTSPRITETYPE_BLESS_GROUND_3,
                SCR_EFFECTSPRITETYPE_BLESS_GROUND_4,
                SCR_EFFECTSPRITETYPE_BLESS_ING,		// ë°˜ë³µ
                SCR_EFFECTSPRITETYPE_BLESS,			// •˜‚˜ ë°˜ë³µœ¼ë¡
ë°”ë //‹œž‘ SCR_EFFECTSPRITETYPE_CHAOS_COMBO,
                SCR_EFFECTSPRITETYPE_CONTINUAL_LIGHT_1,
                SCR_EFFECTSPRITETYPE_CONTINUAL_LIGHT_2,
                SCR_EFFECTSPRITETYPE_CONTINUAL_LIGHT_3,
                SCR_EFFECTSPRITETYPE_CREATE_HOLY_WATER_1,
                SCR_EFFECTSPRITETYPE_CREATE_HOLY_WATER_2,
                SCR_EFFECTSPRITETYPE_CREATE_HOLY_WATER_3,
                SCR_EFFECTSPRITETYPE_CURE_PARALYSIS,
                SCR_EFFECTSPRITETYPE_CURE_CRITICAL_WOUND_1,
                SCR_EFFECTSPRITETYPE_CURE_CRITICAL_WOUND_2,
                SCR_EFFECTSPRITETYPE_CURE_CRITICAL_WOUND_3,
                SCR_EFFECTSPRITETYPE_CURE_SERIOUS_WOUND_1,
                SCR_EFFECTSPRITETYPE_CURE_SERIOUS_WOUND_2,
                SCR_EFFECTSPRITETYPE_CURE_SERIOUS_WOUND_3,
                SCR_EFFECTSPRITETYPE_CURE_LIGHT_WOUND_1,
                SCR_EFFECTSPRITETYPE_CURE_LIGHT_WOUND_2,
                SCR_EFFECTSPRITETYPE_CURE_LIGHT_WOUND_3,
                SCR_EFFECTSPRITETYPE_CURE_POISON,
                SCR_EFFECTSPRITETYPE_DETECT_HIDDEN,
                SCR_EFFECTSPRITETYPE_DOUBLE_IMPACT,
                SCR_EFFECTSPRITETYPE_ENCHANT_CASTING1_1,
                SCR_EFFECTSPRITETYPE_ENCHANT_CASTING1_2,
                SCR_EFFECTSPRITETYPE_ENCHANT_CASTING1_3,
                SCR_EFFECTSPRITETYPE_ENCHANT_CASTING2_1,
                SCR_EFFECTSPRITETYPE_ENCHANT_CASTING2_2,
                SCR_EFFECTSPRITETYPE_ENCHANT_CASTING2_3,
                SCR_EFFECTSPRITETYPE_EXPLOSION_OLD,
                SCR_EFFECTSPRITETYPE_HEALING_CASTING_1,
                SCR_EFFECTSPRITETYPE_HEALING_CASTING_2,
                SCR_EFFECTSPRITETYPE_HEALING_CASTING_3,
                SCR_EFFECTSPRITETYPE_HOLY_SHOOTING_AR_MALE,
                SCR_EFFECTSPRITETYPE_HOLY_SHOOTING_SMG_MALE,
                SCR_EFFECTSPRITETYPE_HOLY_SHOOTING_TR_MALE,
                SCR_EFFECTSPRITETYPE_HOLY_SHOOTING_SG_MALE,
                SCR_EFFECTSPRITETYPE_HOLY_SHOOTING_AR_FEMALE,
                SCR_EFFECTSPRITETYPE_HOLY_SHOOTING_SMG_FEMALE,
                SCR_EFFECTSPRITETYPE_HOLY_SHOOTING_TR_FEMALE,
                SCR_EFFECTSPRITETYPE_HOLY_SHOOTING_SG_FEMALE,
                SCR_EFFECTSPRITETYPE_HOLY_SHOOTING_HIT,
                SCR_EFFECTSPRITETYPE_HOLY_WATER_1,	// ê¹¨ìŠ”ê±
                SCR_EFFECTSPRITETYPE_HOLY_WATER_2,	// ‹¤Œ¨? - -;
                SCR_EFFECTSPRITETYPE_HURRICANE_COMBO,
                SCR_EFFECTSPRITETYPE_LIGHT_1,
                SCR_EFFECTSPRITETYPE_LIGHT_2,
                SCR_EFFECTSPRITETYPE_LIGHT_3,
                SCR_EFFECTSPRITETYPE_POISONOUS_HAND,
                SCR_EFFECTSPRITETYPE_POTENTIAL,
                SCR_EFFECTSPRITETYPE_PROTECTION_FROM_CURSE,
                SCR_EFFECTSPRITETYPE_PROTECTION_FROM_PARALYSIS,
                SCR_EFFECTSPRITETYPE_PROTECTION_FROM_POISON,
                SCR_EFFECTSPRITETYPE_RAINBOW_SLASHER,
                SCR_EFFECTSPRITETYPE_REMOVE_CURSE,
                SCR_EFFECTSPRITETYPE_RESTORE_1,
                SCR_EFFECTSPRITETYPE_RESTORE_2,
                SCR_EFFECTSPRITETYPE_RESTORE_3,
                SCR_EFFECTSPRITETYPE_GUN_AR_MALE,
                SCR_EFFECTSPRITETYPE_GUN_SMG_MALE,
                SCR_EFFECTSPRITETYPE_GUN_AR_FEMALE,
                SCR_EFFECTSPRITETYPE_GUN_SMG_FEMALE,
                SCR_EFFECTSPRITETYPE_GUN_TR_MALE,
                SCR_EFFECTSPRITETYPE_GUN_SG_MALE,
                SCR_EFFECTSPRITETYPE_GUN_TR_FEMALE,
                SCR_EFFECTSPRITETYPE_GUN_SG_FEMALE,
                SCR_EFFECTSPRITETYPE_SCREW_SLASHER,
                SCR_EFFECTSPRITETYPE_SINGLE_BLOW,
                SCR_EFFECTSPRITETYPE_SMOKE,
                SCR_EFFECTSPRITETYPE_SMOKE_1,
                SCR_EFFECTSPRITETYPE_SMOKE_2,
                SCR_EFFECTSPRITETYPE_SMOKE_3,
                SCR_EFFECTSPRITETYPE_SMOKE_4,
                SCR_EFFECTSPRITETYPE_SNAKE_COMBO_OLD,
                SCR_EFFECTSPRITETYPE_SPIRAL_SLAY,
                SCR_EFFECTSPRITETYPE_STRIKING_ING_1,
                SCR_EFFECTSPRITETYPE_STRIKING_ING_2,
                SCR_EFFECTSPRITETYPE_STRIKING_ING_3,
                SCR_EFFECTSPRITETYPE_STRIKING_1,
                SCR_EFFECTSPRITETYPE_STRIKING_2,
                SCR_EFFECTSPRITETYPE_STRIKING_3,
                SCR_EFFECTSPRITETYPE_THUNDER_SPARK,
                SCR_EFFECTSPRITETYPE_TRIPLE_BREAK,
                SCR_EFFECTSPRITETYPE_TRIPLE_SLASHER,
                SCR_EFFECTSPRITETYPE_TWIN_BREAK,
                SCR_EFFECTSPRITETYPE_VAMPIRE_DEATH,
                SCR_EFFECTSPRITETYPE_WILD_SMASH,
                SCR_EFFECTSPRITETYPE_DETECT_INVISIBILITY,
                SCR_EFFECTSPRITETYPE_IDENTIFY_1x1,
                SCR_EFFECTSPRITETYPE_IDENTIFY_1x3,
                SCR_EFFECTSPRITETYPE_IDENTIFY_2x2,
                SCR_EFFECTSPRITETYPE_IDENTIFY_2x3,
                SCR_EFFECTSPRITETYPE_SACRIFICE_1,			//
casting SCR_EFFECTSPRITETYPE_SACRIFICE_2,			// ì§†
                SCR_EFFECTSPRITETYPE_SNAKE_COMBO,
                SCR_EFFECTSPRITETYPE_SWORD_WAVE_1,			//
frame ”°ë¡ —†‹¤. SCR_EFFECTSPRITETYPE_SWORD_WAVE_2,			//
frame ”°ë¡ —†‹¤. SCR_EFFECTSPRITETYPE_SWORD_WAVE_3,			//
frame ”°ë¡ —†‹¤. SCR_EFFECTSPRITETYPE_TORNADO_SEVER_1,		//
ë°”ë‹¥— ì°ížˆŠ”ê± (5”„ ˆž„ ›„— ì¶œë ¥) SCR_EFFECTSPRITETYPE_TORNADO_SEVER_2,
// ëª¸ì— ë¶™ëŠ”ê±

                SCR_EFFECTSPRITETYPE_EXPLOSION,

                // 2001.9.3
                SCR_EFFECTSPRITETYPE_LIGHTNING_HANDS_1,8
                SCR_EFFECTSPRITETYPE_LIGHTNING_HANDS_2,		// ë°˜ë³µ
                SCR_EFFECTSPRITETYPE_LIGHTNING_HANDS_3,

                // 2001.9.6
                SCR_EFFECTSPRITETYPE_PROTECTION_FROM_ACID_1,		// ‹œž‘
                SCR_EFFECTSPRITETYPE_PROTECTION_FROM_ACID_2,		// ì§†

                // 2001.10.8
                SCR_EFFECTSPRITETYPE_EXPLOSION_2,
                SCR_EFFECTSPRITETYPE_EXPLOSION_3,
                SCR_EFFECTSPRITETYPE_CURE_ALL_1, // ‹œž‘
                SCR_EFFECTSPRITETYPE_CURE_ALL_2,
// bless ë¿Œë ¤ì£¼ê¸° SCR_EFFECTSPRITETYPE_ENCHANT_1x1,
                SCR_EFFECTSPRITETYPE_ENCHANT_1x3,
                SCR_EFFECTSPRITETYPE_ENCHANT_2x2,
                SCR_EFFECTSPRITETYPE_ENCHANT_2x3
        };

        //------------------------------------------------------------
        // convert• •Œ.. progress.. ƒ ..
        //------------------------------------------------------------
        SetProgressBarText("°´ ™”¼„ ì²´í¬ì¤‘ìž…‹ˆ‹¤.");
        SetProgressBarCount( MAX_EST );

        int cx = GetSystemMetrics(SM_CXSCREEN);
        int cy = GetSystemMetrics(SM_CYSCREEN);

        ShowWindow(g_hWndProgress, SW_HIDE);
        CDirectDraw::Init(g_hWndProgress, cx, cy, CDirectDraw::FULLSCREEN);

        MoveWindow(g_hWndProgress,
                                (cx-progressBarWidth)/2,
                                (cy-progressBarHeight)/2,
                                progressBarWidth,
                                progressBarHeight,
                                TRUE);
        ShowWindow(g_hWndProgress, SW_SHOW);


        // 16 bit ëª¨ë“œë¡ ë°”ê¿”•¼ •œ‹¤.
        //DDSURFACEDESC2 ddsd;
        //CDirectDraw::GetDD()->GetDisplayMode( &ddsd );
        //CDirectDraw::GetDD()->SetDisplayMode(800, 600, 16, 0, 0);


        //------------------------------------------------------------
        // •„š”•œ Frame ë½‘ê¸°
        //------------------------------------------------------------
        COrderedList<int> intList;

        std::ifstream efpkFile(FILE_EFRAME_ALPHAEFFECT, std::ios::binary);
        CEffectFramePack	EFPK;
        EFPK.LoadFromFile( efpkFile );
        efpkFile.close();

        CEffectFramePack	NewEFPK;
        NewEFPK.Init( MAX_EST );

        //------------------------------------------------------------
        // •„š”•œ EFPKë¥ ƒ„±•˜ë©´ì„œ ‚¬š©œ SpriteID„ êµ¬í•œ‹¤.
        //------------------------------------------------------------
        for (int e=0; e<MAX_EST; e++)
        {
                int etype = est[e];

                DIRECTION_EFFECTFRAME_ARRAY& DEA = EFPK[etype];

                NewEFPK[e] = DEA;

                for (int d=0; d<8; d++)
                {
                        EFFECTFRAME_ARRAY& EA = DEA[d];

                        for (int f=0; f<EA.GetSize(); f++)
                        {
                                CEffectFrame& Frame = EA[f];

                                int spriteID = Frame.GetSpriteID();

                                intList.Add( spriteID );
                        }
                }

                UpdateProgressBar();
        }

        EFPK.Release();

        //------------------------------------------------------------
        // Alpha --> Normal ì¤ë¹..
        //------------------------------------------------------------
        CSpriteSurface	surface;
        surface.InitOffsurface( 640, 480, DDSCAPS_SYSTEMMEMORY );

        POINT point0 = { 0, 0 };


        CAlphaSprite565		ASPR;

        std::ifstream aspkiFile(FILE_ASPRITEINDEX_ALPHAEFFECT,
std::ios::binary); std::ifstream aspkFile(FILE_ASPRITE_ALPHAEFFECT,
std::ios::binary);

        TYPE_SPRITEID totalNum;
        int spriteNum = intList.GetSize();

        CSpritePack* pSPK = new CSpritePack;
        pSPK->Init( spriteNum, CDirectDraw::Is565() );

        aspkFile.read((char*)&totalNum, 2);

        int* pNewID = new int [totalNum];

        COrderedList<int>::DATA_LIST::const_iterator iID =
intList.GetIterator(); WORD *lpSurface, lPitch;

        //------------------------------------------------------------
        // convert• •Œ.. progress.. ƒ ..
        //------------------------------------------------------------
        SetProgressBarText("°´ ™”¼„ ë³ê²½ì¤‘ž…‹ˆ‹¤.");
        SetProgressBarCount( spriteNum/8 );


        //------------------------------------------------------------
        // Alpha --> Normal
        //------------------------------------------------------------
        for (int sid=0; sid<spriteNum; sid++)
        {
                int spriteID = *iID;

                aspkFile.seekg( 0 );
                aspkiFile.seekg( 0 );

                //--------------------------------------------------------
                // Load•  FilePointerë¥ ½–´˜¨‹¤.
                //--------------------------------------------------------
                long fp;
                aspkiFile.seekg( 2 + spriteID*4 );		// 2(num) +
spriteID * (4 bytes) aspkiFile.read((char*)&fp, 4); aspkFile.seekg( fp );

                //--------------------------------------------------------
                // Load
                //--------------------------------------------------------
                ASPR.LoadFromFile( aspkFile );

                //--------------------------------------------------------
                // ¬ê¸ ê³„ì‚°.. ê± - -
                //--------------------------------------------------------
                int width = ASPR.GetWidth();
                int height = ASPR.GetHeight();

                //--------------------------------------------------------
                // AlphaSpriteë¥ ì¶œë ¥•œ ›„—
                // Spriteë¥ ƒ„±•œ‹¤.
                //--------------------------------------------------------
                surface.FillSurface( 0 );

                surface.LockW(lpSurface, lPitch);
                surface.BltAlphaSprite( &point0, &ASPR );
                ((*pSPK)[sid]).SetPixel(lpSurface, lPitch, width, height);

                surface.Unlock();

                // ›ž˜Š” IDê° spriteID˜ê² sidë¡ ë°”ëŒê²Œ œ‹¤.
                pNewID[ spriteID ] = sid;

                if ((sid & 0x00000007)==0x00000007)
                {
                        UpdateProgressBar();
                }

                iID++;
        }

        aspkiFile.close();
        aspkFile.close();

        // ›ž˜ë¡
        //CDirectDraw::GetDD()->RestoreDisplayMode();

        //------------------------------------------------------------
        // SpriteIDë¥ ˆ˜ ••œ‹¤.
        //------------------------------------------------------------
        int numFPK = NewEFPK.GetSize();
        for (e=0; e<numFPK; e++)
        {
                DIRECTION_EFFECTFRAME_ARRAY& DEA = NewEFPK[e];

                for (int d=0; d<8; d++)
                {
                        EFFECTFRAME_ARRAY& EA = DEA[d];

                        for (int f=0; f<EA.GetSize(); f++)
                        {
                                CEffectFrame& Frame = EA[f];

                                int cx = Frame.GetCX();
                                int cy = Frame.GetCY();
                                int light = Frame.GetLight();
                                int spriteID = pNewID[ Frame.GetSpriteID() ];

                                Frame.Set( spriteID, cx, cy, light );
                        }
                }
        }

        delete [] pNewID;

        //------------------------------------------------------------
        // FramePackž¥
        //------------------------------------------------------------
        std::ofstream fileFPK(FILE_EFRAME_SCREENEFFECT, std::ios::binary);
        std::ofstream fileFPKI(FILE_EFRAMEINDEX_SCREENEFFECT, std::ios::binary);
        NewEFPK.SaveToFile( fileFPK, fileFPKI );
        fileFPK.close();
        fileFPKI.close();

        //------------------------------------------------------------
        // SpritePack ž¥
        //------------------------------------------------------------
        std::ofstream fileSPK(FILE_SPRITE_SCREENEFFECT, std::ios::binary);
        std::ofstream fileSPKI(FILE_SPRITEINDEX_SCREENEFFECT, std::ios::binary);
        pSPK->SaveToFile( fileSPK, fileSPKI );
        fileSPK.close();
        fileSPKI.close();

        delete pSPK;

        ShowWindow(g_hWndProgress, SW_HIDE);
        CDirectDraw::ReleaseAll();

        MoveWindow(g_hWndProgress,
                                (cx-progressBarWidth)/2,
                                (cy-progressBarHeight)/2,
                                progressBarWidth,
                                progressBarHeight,
                                TRUE);
        ShowWindow(g_hWndProgress, SW_SHOW);

        //------------------------------------------------------------
        // ë³™˜ ì²´í¬
        //------------------------------------------------------------
        std::ofstream fileCheck("Data\\Info\\EffectScreenConvert.inf",
std::ios::binary); int a = 1; fileCheck.write((const char*)&a, 4);
fileCheck.close();

        return true;
}
*/
//------------------------------------------------------------------------
// ApplyPatch
//------------------------------------------------------------------------
// ê°‘ìžê¸ ••ì¶•ì„ •˜ê² ˜Š” ë°”ëžŒ—... - -;
//------------------------------------------------------------------------
bool ApplyPatch() {
  //-----------------------------------------------------------------
  // ˜„ž¬ version
  //-----------------------------------------------------------------
  //	std::ifstream versionFile(FILE_INFO_VERSION, std::ios::binary);
  //	int version;
  //	versionFile.read((char*)&version, 4);
  //	versionFile.close();

  //-----------------------------------------------------------------
  // Œ¨ì¹˜ì••ì¶•í™”¼´ žˆ‚˜ ™•¸
  //-----------------------------------------------------------------
  //	char packFilename[256];
  //	char infoFilename[256];

  //	struct _finddata_t	FileData;
  //	long				hFile;

  //-----------------------------------------------------------------
  // *.mpk file„ ì°¾ëŠ”‹¤.
  //-----------------------------------------------------------------
  //	if ( (hFile = _findfirst( "*.mpk", &FileData )) != -1L )
  //	{
  //		int files = 1;
  //		do
  //		{
  //			strcpy(packFilename, FileData.name);
  //			files ++;
  //		}
  //		while (_findnext( hFile, &FileData ) == 0);
  //
  //		_findclose( hFile );

  //-----------------------------------------------------------------
  // Œ¨ì¹˜ì••ì¶•í™”¼˜ ••ì¶ •´ œ
  //-----------------------------------------------------------------
  //		_mkdir( "Update" );
  //
  //		SetProgressBarText("Œ¨ì¹ ™”¼˜ ••ì¶•ì„ •´ œ•˜ê³ žˆŠµ‹ˆ‹¤.");
  //
  //		MZLib mzlib;
  //
  //		// ••ì¶•í• •Œ Updaterê²½ë¡œë¥ ê°™ì´ ž¥–ˆœ¼ë¯ë¡ ê°™ì´ ë¦°ë‹¤.
  //		mzlib.Uncompress( packFilename );
  //
  //		//-----------------------------------------------------------------
  //		// infofile´ë¦ ê²°ì • - packFilename— ”°¼„œ..
  //		//-----------------------------------------------------------------
  //		int len = strlen(packFilename);
  //		char str[256];
  //		strncpy(str, packFilename, len-4);	// ê± .ê¹Œì ë¹¼ì‹¤.
  //		str[len-4] = '\0';
  //		sprintf(infoFilename, "Update\\%s.inf", str);
  //
  //	}
  //-----------------------------------------------------------------
  // mpk™”¼´ —†Š” ê²½ìš°Š” ˆ˜™Œ¨ì¹˜ì¸ ê²½ìš°ë¥ ì²´í¬•´ë´ì•¼•œ‹¤.
  // packFilename´ ëª‡ë²ˆ version—  š©˜Š”ê±´ì •Œ•„‚¸‹¤.
  //-----------------------------------------------------------------
  //	else
  //	{
  //		/*
  //		char* pUnder = strchr(packFilename+5, "_");
  //		if (pUnder!=NULL)
  //		{
  //			char strVersion[10];
  //			strncpy(strVersion, packFilename+5, pUnder -
  // packFilename+5); 			int versionFrom = atoi(strVersion);
  //		}
  //		*/
  //
  //		// ˜„ž¬ ë²„ì „— ë§žëŠ” Œ¨ì¹˜í™”¼„ ì°¾ëŠ”‹¤.
  //		sprintf(infoFilename, "Update\\Patch%d_*.inf", version);
  //
  //		if ( (hFile = _findfirst( infoFilename, &FileData )) != -1L )
  //		{
  //			int files = 1;
  //			do
  //			{
  //				sprintf(infoFilename, "Update\\%s",
  // FileData.name); 				files ++;
  //			}
  //			while (_findnext( hFile, &FileData ) == 0);
  //
  //			_findclose( hFile );
  //		}
  //		else
  //		{
  //			// ˆ˜™Œ¨ì¹˜ë„ —†Š” ê²½ìš° == ˜„ž¬ ë²„ì „
  //			return false;
  //		}
  //	}
  //
  //	//-----------------------------------------------------------------
  //	// info ½ê¸
  //	//-----------------------------------------------------------------
  //	CreateProgressBar("Œ¨ì¹  •ë³´ë ì²´í¬ ì¤‘ìž…‹ˆ‹¤.");
  //
  //	UpdateManager	UM;
  //	UM.load( infoFilename );
  //
  //	SetProgressBarCount( UM.getNum()+g_numAppendFiles );
  //
  //	SetProgressBarText("Œ¨ì¹ ™”¼„  š©‹œ‚¤ê³ žˆŠµ‹ˆ‹¤.");
  //
  //	//-----------------------------------------------------------------
  //	// Œ¨ì¹˜í™”¼  š©
  //	//-----------------------------------------------------------------
  //	int newVersion = 0;
  //	while (!UM.empty())
  //	{
  //		Update* pUpdate = UM.front();
  //		UM.pop_front();
  //
  //		if (!pUpdate->execute())
  //		{
  //			// -_-;;
  //		}
  //
  //		// version up¸ê°?
  //		if (pUpdate->getVersion() > newVersion)
  //		{
  //			newVersion = pUpdate->getVersion();
  //		}
  //
  //		UpdateProgressBar();
  //	}
  //
  //	//-----------------------------------------------------------------
  //	// ƒˆ versionž¥
  //	//-----------------------------------------------------------------
  //	std::ofstream versionFile2(FILE_INFO_VERSION, std::ios::binary);
  //	versionFile2.write((const char*)&newVersion, 4);
  //	versionFile2.close();
  //
  //	//-----------------------------------------------------------------
  //	// Œ¨ì¹˜ì••ì¶•í™”¼  œê±
  //	//-----------------------------------------------------------------
  //	remove( packFilename );
  //
  //	//-----------------------------------------------------------------
  //	// Œ¨ì¹˜í™”¼  œê± - Updater ”” ‰† ë¦ ì§š°ë©´ì„œ ˜ë¯ë¡ .. ‹ ê² •ˆ ¨„
  // œ‹¤.
  //	//-----------------------------------------------------------------
  //
  return true;
}

#if defined(OUTPUT_DEBUG) && PHOENIX_CLIENT_LOGS_ENABLED
//-----------------------------------------------------------------------------
// Check LogFile
//-----------------------------------------------------------------------------
//	void
//	CheckCLogFile()
//	{
//		_chdir( g_CWD );
//
//		char filename[256] = { 0, };
//		char buffer[256] = { 0, };
//		char computerName[256] = { 0, };
//
//		struct _finddata_t	FileData;
//		long				hFile;
//
//		//-----------------------------------------------------------------
//		// *.spk file„ ì°¾ëŠ”‹¤.
//		//-----------------------------------------------------------------
//		if ( (hFile = _findfirst( "Data\\UI\\txt\\Log*.txt", &FileData
//)) != -1L )
//		{
//			do
//			{
//				sprintf(filename, "Data\\UI\\txt\\%s",
// FileData.name);
//
//				std::ifstream file( filename, std::ios::binary
//); 				file.seekg( 0, std::ios::end );
//
//				long fp = file.tellg();
//
//				file.close();
//				if (fp == 0)
//				{
//					_chmod( filename, _S_IREAD | _S_IWRITE
//); 					remove(filename);
//				}
//				else
//				{
//					//copy
//					if (computerName[0]=='\0')
//					{
//						unsigned long numSize = 255;
//						GetComputerName( computerName,
//&numSize );
//					}
//
//					sprintf(buffer, "\\\\‘¥ê°\\Log\\%s-%s",
// computerName, FileData.name);
//
//					_chmod( filename, _S_IREAD | _S_IWRITE
//); 					rename(filename, buffer);
//				}
//			}
//
//
//			while (_findnext( hFile, &FileData ) == 0);
//
//			_findclose( hFile );
//		}
//	}

void CheckLogFile() {
  _chdir(g_CWD);

  char filename[256] = {
      0,
  };
  char buffer[256] = {
      0,
  };
  char computerName[256] = {
      0,
  };

  struct _finddata_t FileData;
  long hFile;

  _mkdir("Log");

  //-----------------------------------------------------------------
  // *.spk file „ ì°¾ëŠ”‹¤.
  //-----------------------------------------------------------------
  if ((hFile = _findfirst("Log\\Log*.txt", &FileData)) != -1L) {
    do {
      sprintf(filename, "Log\\%s", FileData.name);

      std::ifstream file(filename, std::ios::binary);
      file.seekg(0, std::ios::end);

      long fp = file.tellg();

      if (fp < 39) {
        _chmod(filename, _S_IREAD | _S_IWRITE);
        remove(filename);
      } else {
        file.seekg(-41, std::ios::end);
        file.read((char *)buffer, 24);
        buffer[24] = '\0';
        file.close();

        if (strcmp(buffer, "Delete DebugMessageArray") == 0) {
          _chmod(filename, _S_IREAD | _S_IWRITE);
          remove(filename);
        } else {
          // copy
          //						if
          //(computerName[0]=='\0')
          //						{
          //							unsigned long
          // numSize = 255;
          // GetComputerName( computerName,  &numSize );
          //						}
          //
          //						sprintf(buffer,
          //"\\\\‘¥ê°\\Log\\%s-%s", computerName, FileData.name);
          //
          //						_chmod( filename,
          //_S_IREAD | _S_IWRITE );
          // rename(filename, buffer);
        }
      }
    }

    while (_findnext(hFile, &FileData) == 0);

    _findclose(hFile);
  }
}

//-----------------------------------------------------------------------------
// Check Flush LogFile
//-----------------------------------------------------------------------------
void CheckFlushLogFile() {
  // #ifdef OUTPUT_DEBUG_UPDATE_LOOP
  const DWORD flushDelay = 3 * 60 * 1000;
  static DWORD flushTime = g_CurrentTime + flushDelay;

  if (g_CurrentTime > flushTime) {
    if (g_pDebugMessage != NULL) {
      char logFile[128];
      strcpy(logFile, g_pDebugMessage->GetFilename());

      if (g_pDebugMessage != NULL) {
        DEBUG_ADD("--------------- Delete DebugMessageArray --------------");
        delete g_pDebugMessage;
        g_pDebugMessage = NULL;
      }
      _chmod(logFile, _S_IREAD | _S_IWRITE);
      remove(logFile);

      sprintf(logFile, "%s\\Log\\Log%d.txt", g_CWD, timeGetTime());
      g_pDebugMessage = new CMessageArray;
      g_pDebugMessage->Init(MAX_DEBUGMESSAGE, 256, logFile);

      DEBUG_ADD_FORMAT("[Time = %d]", g_CurrentTime);
    }

    // 3ë¶ ›„
    flushTime = g_CurrentTime + flushDelay;
  }
  // #endif
}
#endif

// HHOOK g_hHook = NULL;

// LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
//{
//     BOOL bSysKeyDown = FALSE;
//     BOOL bWinKeyDown = FALSE;
//
//     if(nCode == HC_ACTION)
//     {
//         KBDLLHOOKSTRUCT* kbhook = (KBDLLHOOKSTRUCT*)lParam;
//
//// •ŒŠ¸‚¤ê° ˆŒë¦°ê²ƒ¸ì§....
//        if(kbhook->flags & LLKHF_ALTDOWN)
//        {
//            switch(kbhook->vkCode)
//            {
//            case VK_TAB:
//            case VK_F4:
//			case VK_ESCAPE:
//                bSysKeyDown = TRUE;
//                break;
//            }
//        }
//// Control + ESC„ ë§‰ìŸˆ
//		else if(((GetAsyncKeyState( VK_CONTROL ) & 0x8000) || g_pDXInput
//!= NULL && (g_pDXInput->KeyDown(DIK_LCONTROL) ||
// g_pDXInput->KeyDown(DIK_RCONTROL))) && kbhook->vkCode == VK_ESCAPE)
//		{
//			SHORT control = GetAsyncKeyState( VK_CONTROL );
//			bSysKeyDown = TRUE;
//		}
//         else
//         {
//             switch(kbhook->vkCode)
//             {
//             case VK_LWIN:
//             case VK_RWIN:
//                 bWinKeyDown = TRUE;
//                 break;
//             }
//         }
//
//         if(bSysKeyDown || bWinKeyDown)
//             return 1;
//     }
//
//     return CallNextHookEx(g_hHook, nCode, wParam, lParam);
// }

//----------------------------------------------------------
// •´ƒ„  •ë³´ë ë¡œë“œ/„¸´ë¸Œí•œ‹¤.
//----------------------------------------------------------
namespace
{
struct PhoenixResolutionMode
{
  int x;
  int y;
};

static const PhoenixResolutionMode kPhoenixResolutionDefault = {1280, 960};
static const PhoenixResolutionMode kPhoenixResolutionModes[] = {
    {1280, 960},
    {1440, 900},
    {1920, 1080},
    {1280, 1024},
};

bool IsPhoenixResolutionSupported(int x, int y)
{
  for (int i = 0; i < static_cast<int>(sizeof(kPhoenixResolutionModes) / sizeof(kPhoenixResolutionModes[0])); ++i)
  {
    if (kPhoenixResolutionModes[i].x == x && kPhoenixResolutionModes[i].y == y)
      return true;
  }
  return false;
}

void NormalizePhoenixResolution(int& x, int& y)
{
  if (x == 800 && y == 600)
  {
    CT_Done("InitResolutionConfig MIGRATE: 800x600 -> 1280x960");
    x = 1280;
    y = 960;
    return;
  }
  if (x == 1024 && y == 768)
  {
    CT_Done("InitResolutionConfig MIGRATE: 1024x768 -> 1440x900");
    x = 1440;
    y = 900;
    return;
  }
  if (x == 1280 && y == 720)
  {
    CT_Done("InitResolutionConfig MIGRATE: 1280x720 -> 1920x1080");
    x = 1920;
    y = 1080;
    return;
  }
  if (IsPhoenixResolutionSupported(x, y))
    return;

  CT_Done("InitResolutionConfig UNSUPPORTED: %dx%d -> %dx%d",
          x, y, kPhoenixResolutionDefault.x, kPhoenixResolutionDefault.y);
  x = kPhoenixResolutionDefault.x;
  y = kPhoenixResolutionDefault.y;
}
}

void InitResolutionConfig() {
  CT_Done("InitResolutionConfig ENTER (g_pUserInformation=%p)", (void*)g_pUserInformation);
  int nResolutionX = kPhoenixResolutionDefault.x;
  int nResolutionY = kPhoenixResolutionDefault.y;
  int nFullScreen = 0;
  bool loaded = false;
  try {
    Properties ResolutionConfig;
    std::string str = g_pFileDef->getProperty("FILE_INFO_RESOLUTION");
    CT_Done("InitResolutionConfig path='%s'", str.c_str());
    ResolutionConfig.load(str.c_str());
    CT_Done("InitResolutionConfig Properties::load OK");
    nResolutionX = ResolutionConfig.getPropertyInt("ResolutionX");
    nResolutionY = ResolutionConfig.getPropertyInt("ResolutionY");
    nFullScreen = ResolutionConfig.getPropertyInt("FullScreen");
    loaded = true;
    CT_Done("InitResolutionConfig parsed: X=%d Y=%d FS=%d", nResolutionX, nResolutionY, nFullScreen);
  } catch (Throwable& t) {
    nResolutionX = kPhoenixResolutionDefault.x;
    nResolutionY = kPhoenixResolutionDefault.y;
    nFullScreen = 0;
    CT_Done("InitResolutionConfig ERROR: %s -- using defaults %dx%d",
            t.toString().c_str(), nResolutionX, nResolutionY);
  } catch (std::exception& e) {
    nResolutionX = kPhoenixResolutionDefault.x;
    nResolutionY = kPhoenixResolutionDefault.y;
    nFullScreen = 0;
    CT_Done("InitResolutionConfig STD ERROR: %s -- using defaults %dx%d",
            e.what(), nResolutionX, nResolutionY);
  }
  NormalizePhoenixResolution(nResolutionX, nResolutionY);
  if (g_pUserInformation == NULL) {
    CT_Done("InitResolutionConfig ERROR: g_pUserInformation NULL");
    return;
  }
  g_pUserInformation->IsResolution1024 = true;
  g_pUserInformation->iResolution_x = nResolutionX;
  g_pUserInformation->iResolution_y = nResolutionY;
  CT_Done("InitResolutionConfig ASSIGNED iResolution_x=%d iResolution_y=%d loaded=%d",
             g_pUserInformation->iResolution_x, g_pUserInformation->iResolution_y, loaded ? 1 : 0);
  CT_Done("InitResolutionConfig STRUCT sizeof(UserInformation)=%u offsetof(iResolution_x)=%u offsetof(iResolution_y)=%u",
             (unsigned)sizeof(UserInformation),
             (unsigned)((char*)&g_pUserInformation->iResolution_x - (char*)g_pUserInformation),
             (unsigned)((char*)&g_pUserInformation->iResolution_y - (char*)g_pUserInformation));
  if (nFullScreen == 1) {
    g_bFullScreen = true;
  } else {
    g_bFullScreen = false;
  }
}

void SaveResolutionConfig() {
  if (g_pUserOption == NULL || g_pUserInformation == NULL || g_pFileDef == NULL) {
    CT_Done("SaveResolutionConfig ERROR: missing global state option=%p info=%p filedef=%p",
            (void*)g_pUserOption, (void*)g_pUserInformation, (void*)g_pFileDef);
    return;
  }

  int nResolutionX = g_pUserInformation->iResolution_x;
  int nResolutionY = g_pUserInformation->iResolution_y;
  NormalizePhoenixResolution(nResolutionX, nResolutionY);
  g_pUserInformation->iResolution_x = nResolutionX;
  g_pUserInformation->iResolution_y = nResolutionY;
  g_pUserInformation->IsResolution1024 = true;

  std::string str = g_pFileDef->getProperty("FILE_INFO_RESOLUTION");
  std::ofstream file(str.c_str());
  if (file.is_open()) {
    file << "ResolutionX: " << g_pUserInformation->iResolution_x << std::endl;
    file << "ResolutionY: " << g_pUserInformation->iResolution_y << std::endl;
    file << "FullScreen: " << (g_bFullScreen ? "1" : "0") << '\n';
  } else {
    CT_Done("SaveResolutionConfig ERROR: cannot open '%s'", str.c_str());
  }
}

int GameStart(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
              int nCmdShow) {
#if __CONTENTS(__XTRAP)
  // 2009 04 21 •˜ê¸ ì£¼ì„ ì¤‘êµ­ìª ë²ˆí˜¸ ˜„ž¬ „œë¹„ìŠ¤ì¤ •„‹˜..
  // http://patch.wiselogic.co.kr/DarkEden
  // 660970B478E9CD6790316D9844CFE862EEE63E3F2BAD301E5F3D81C30324E0EBAAE8D9458994D60E58688AACDA8FE61389ECBD25D6B246C09E4B18F747C51F420F7D04245246E403AE73194E9554761FEBE0CB630673E9ADAFB81AF9B7ECAC
  XTrap_C_Start(
      "660970B47809CDE555336D9844CFE86287416167DEC365483208B760E262AA7D84A0660B"
      "AA6116039CE2CBB0C332B21FC86DAE63C50560522AD880E5933D746E0F7D04245246E417"
      "A4610E5695477E1EF47455B309945CB0D27B7C7D3821396080E71928F0",
      NULL);
#endif //__XTRAP

  //	std::string tttt = "NEWSTART ";
  //	tttt += lpCmdLine;
  //	lpCmdLine = (char *)tttt.c_str();



  // opfstream testTemp("Data/Test/File.txt", std::ios_base::binary);
  // testTemp.write("9", 1);
  // testTemp.close();

  // CCompressedTempFileIO::flush_temp_file();
#ifdef __DESIGNED_KOREA
  g_IsWow64 = IsWow64(); // by viva
  // g_IsWow64 = TRUE;
#endif

#ifdef OUTPUT_DEBUG
  InitializeCriticalSection(&g_Lock);
#endif

  //----------------------------------------------------------
  // Œ¨‚¤ì§  •ë³´ë ë¡œë“œ
  //----------------------------------------------------------
  // ´ê± ì£¼ì„ ì²˜ë¦¬•˜ë© Œ© ŒŒ¼  „ë¶êº¼ì§
  // iovfs_base::start_vfs("data/phoenix", O_RDONLY); // Bypassed by user request to load from local folders
  GetModuleFileName(NULL, g_CWD, _MAX_PATH);
  char *tempCut = strrchr(g_CWD, '\\');
  if (tempCut == NULL)
    return FALSE;

  *tempCut = '\0';

  //	GetCurrentDirectory( _MAX_PATH, g_CWD );

  SetCurrentDirectory(g_CWD);
  std::string phoenixSanitizedCommandLine;
  if (!PhoenixEnsureUpdaterHandoff(lpCmdLine, phoenixSanitizedCommandLine)) {
    return 0;
  }
  lpCmdLine = const_cast<char*>(phoenixSanitizedCommandLine.c_str());
	CT_Done("Client::InitApp - VFS start_vfs(data/phoenix) [AUTO-SUFFIX] activated");

  //----------------------------------------------------------
  // ŒŒ¼ ê²½ë¡œ  •ë³ ë¡œë“œ
  //----------------------------------------------------------
  g_pFileDef = new Properties;
  CT_Done("DIAG: Loading FILE_INFO_FILEDEF: %s", FILE_INFO_FILEDEF);
  g_pFileDef->load(FILE_INFO_FILEDEF);
  CT_Done("DIAG: FILE_INFO_FILEDEF loaded");

  //----------------------------------------------------------
  // œ   •ë³ ì´ˆê¸°™”
  //----------------------------------------------------------
  if (g_pUserInformation == NULL) {
    CT_Done("DIAG: Creating UserInformation");
    g_pUserInformation = new UserInformation;
  }
  CT_Done("DIAG: UserInformation created/ready");

  //----------------------------------------------------------
  // ë²„ì „  •ë³ ë¡œë“œ
  //----------------------------------------------------------
  int version = 0;
  // 2026-05-18: Dangling pointer fix (ayni MSkillManager.cpp pattern).
  // getProperty() std::string by-value returns -> .c_str() temporary pointer
  // SEMICOLON sonrasi dangling -> sonraki FileOpenBinary yanlis dosya okuyabilir.
  // Fix: std::string ile sakla, scope sonuna kadar yasasin.
  std::string versionPath = g_pFileDef->getProperty("FILE_INFO_VERSION");
  CT_Done("DIAG: Loading version from: %s", versionPath.c_str());
  version = static_cast<int>(PhoenixReadClientVersionFile(versionPath.c_str(), 0));
  if (version == 0) {
    ivfstream versionFile;
    if (FileOpenBinary(versionPath.c_str(), versionFile)) {
      versionFile.read((char *)&version, 4);
      versionFile.close();
    }
  }

  if (version > 0) {
    CT_Done("DIAG: Version loaded: %d", version);
  }
  else {
    CT_Done("DIAG: FAILED to open version file");
  }

  g_pUserInformation->GameVersion = version;

#ifdef __USE_CRASH_REPORT
  //----------------------------------------------------------
  // Crash Report
  //----------------------------------------------------------
#ifdef __BUGTRAP
  if (CrashReport::LoadOptionFile(
          g_pFileDef->getProperty("FILE_INFO_CRASH_REPORT").c_str())) {
    CrashReport::SetAppVersion(g_pUserInformation->GameVersion);
    CrashReport::Install();
  }
#endif //__BUGTRAP
#endif

  //----------------------------------------------------------
  // •´ƒ„  •ë³´ë ë¡œë“œ
  //----------------------------------------------------------
  CT_Done("DIAG: Calling InitInitResolutionConfig()"); InitResolutionConfig(); CT_Done("DIAG: Returned from InitResolutionConfig()");

  // by viva	hMutex
  //----------------------------------------------------------
  // •˜‚˜˜ applicationë§ ‹¤–‰‹œ‚¨‹¤.
  //----------------------------------------------------------
  // ê° ì¶ „£ê¸.. --;
  /*//by viva
          SECURITY_ATTRIBUTES sa;
          sa.nLength = sizeof(sa);
      sa.lpSecurityDescriptor = NULL;
      sa.bInheritHandle = FALSE;


  #ifndef OUTPUT_DEBUG
          HANDLE hMutex = CreateMutex(&sa, FALSE, "<<<dklegend>>>");


          if (GetLastError()==ERROR_ALREADY_EXISTS)
          {
                  #ifdef OUTPUT_DEBUG
                          if (g_pDebugMessage!=NULL)
                          {
                                  DEBUG_ADD("DKLegend is Already Running...");
                                  DEBUG_ADD("--------------- Delete
  DebugMessageArray --------------"); delete g_pDebugMessage; g_pDebugMessage =
  NULL;
                          }
                  #endif

                  ReleaseMutex( hMutex );

                  #ifdef OUTPUT_DEBUG
                          DeleteCriticalSection(&g_Lock);
                  #endif

                  #ifndef _DEBUG
                          return -1;
                  #endif
          }


          if( FindWindow( "DKLegend", "DKLegend" ) != NULL )
          {
                  return -1;
          }
  #endif
          */
  // ˜„ž¬ directoryë¥ ž¥•´‘”‹¤.
  //	strcpy(g_CWD, __argv[0]);
  

  // 	//	// ƒˆë¡œìš´ updater‹¤–‰™”¼´ ì¡´ìž¬•˜ë©..
  //	if (_access(UPDATER_NEW_FILENAME, 0) == 0/* && _access(UPDATER_FILENAME,
  // 0)*/)//updaterNewFile)
  //	{
  //		// ˜ˆ „ Patcherë¥ ì§š°ê³..
  //		if (remove( UPDATER_FILENAME )==0)
  //		{
  //			// ƒˆê²ƒì„ ˜ˆ „ê±¸ë¡œ rename
  //			if (rename( UPDATER_NEW_FILENAME, UPDATER_FILENAME )==0)
  //			{
  //				DEBUG_ADD("Update Updater.exe OK");
  //			}
  //		}
  ////		// ˜ˆ „ updaterë¥ ì§š°ê³..
  ////		//remove( UPDATER_FILENAME );
  ////		// ƒˆê²ƒì„ ˜ˆ „ê±¸ë¡œ rename
  ////		if (rename( UPDATER_NEW_FILENAME, UPDATER_FILENAME )==0)
  ////		{
  ////			DEBUG_ADD("Update Updater.exe OK");
  ////		}
  //	}

#if defined(__METROTECH_TEST__) && PHOENIX_CLIENT_LOGS_ENABLED
  CheckLogFile();
#endif

  /*
#include "packet\ServerSocket.h"

  // •œ ClientŠ” ™‹œ— ‹¤ë¥ •œ Client †µ‹ •œ‹¤.
  // —¬Ÿ¬êµ°ë°˜ resource(ê¸¸ë“œë§ˆí¬, profile...)ê° •„š”•œ ê²½ìš°
  //		ResourceQueue— ž¥–ˆ‹¤ê° ‹ˆ‹ˆžˆ ë°›ë„ë¡ •œ‹¤.
  // while(1)ë¶ë¶„ì ‹¤ë¥ threadë¡ ë¹¼ì•¼œ‹¤.
  // RequestManager  ‘ê·¼í• •ŒŠ” mutex“°“ ì§ •´•¼œ‹¤.
  // RequestManager˜ Update()Š” main thread—„œ •œ‹¤.
  //
  ServerSocket* pServerSocket = new ServerSocket( 9650 );

  char strClient[256];
  uint port;

  while (1)
  {
          Socket* pSocket = pServerSocket->accept();

          // request— “±ë¡
          RequestServerPlayer* pRequestServerPlayer = new RequestServerPlayer(
pSocket ); g_pRequestManager->AddPlayer( pRequestServerPlayer );

          // g_pRequestManagerŠ” “±ë¡ëœ RequestServerPlayer— •´„œ
processInput/Command/Output ì²˜ë¦¬

          strcpy(strClient, pSocket->getHost().c_str());
          port = pSocket->getPort();
  }
  */

  /*
  std::ifstream file("ImageObject.spk", std::ios::binary);
  std::ofstream file2("ImageObject-2.spk", std::ios::binary);
  std::ofstream file2index("ImageObject-2.spki", std::ios::binary);
  CSpritePack SPK;
  SPK.LoadFromFile(file);
  SPK.SaveToFile(file2, file2index);
  file.close();
  file2.close();
  file2index.close();

  return 0;
  */

  /*
  OSVERSIONINFO        osVer;

// First get the windows platform
osVer.dwOSVersionInfoSize = sizeof(osVer);

if( !GetVersionEx( &osVer ) )
{
  return 0;
}

if (osVer.dwPlatformId == VER_PLATFORM_WIN32_NT )
{
          g_WindowsNT = TRUE;
  }
  else
  {
          g_WindowsNT = FALSE;
  }
  */

  // OutputMemorySize();

  //	return 0;



  //-----------------------------------------------------------------------
  // Client ‹œž‘..
  //-----------------------------------------------------------------------
  g_bNeedUpdate = FALSE;
  //	OutPutClassSize();
  //	return 0;

#if defined(OUTPUT_DEBUG) && PHOENIX_CLIENT_LOGS_ENABLED

  OutPutClassSize();
  if (g_pDebugMessage != NULL) {
    delete g_pDebugMessage;
  }
  g_pDebugMessage = new CMessageArray;

  //---------------------------------------------------
  // Debug Log File Init...
  //---------------------------------------------------
  if (g_pDebugMessage != NULL) {
    char logFile[128];
    sprintf(logFile, "%s\\Log\\Log%d.txt", g_CWD, timeGetTime());

    //			#include <fcntl.h>
    //			char clogFile[128];
    //			sprintf(clogFile, "%s\\Data\\UI\\txt\\Logc%d.txt",
    // g_CWD, timeGetTime()); 			CLogFile = _open(clogFile,
    // _O_WRONLY | _O_TEXT | _O_CREAT | _O_TRUNC);

    g_pDebugMessage->Init(MAX_DEBUGMESSAGE, 256, logFile);

    DEBUG_ADD_FORMAT("[ Compile Time : %s ]", __TIMESTAMP__);
  }
#endif

  // return 0;

  // g_MemLogFile = _open("memlog.txt", _O_WRONLY | _O_TEXT | _O_CREAT |
  // _O_TRUNC);

#ifdef _DEBUG
  int tmpDbgFlag;
  tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
  tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
  tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
  //_CrtSetDbgFlag(tmpDbgFlag);
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | tmpDbgFlag);

  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
#endif

  g_hInstance = hInstance;
  // random
  srand(time(NULL));

  //---------------------------------------------------
  // SpritePack Loading Test
  //---------------------------------------------------
  /*
  CSpritePack tileSPK;
  std::ifstream	fileSPK;//(FILE_ISPRITE_ADDON, std::ios::binary);
  if (!FileOpenBinary(FILE_SPRITE_TILE, fileSPK))
          return false;
  tileSPK.LoadFromFile(fileSPK);
  fileSPK.close();
  */

  // „·ë§ˆë¸”š©
  bool bNetmarble = false;
  bool bNetmarbleLogin = false;
  Properties NetmarbleConfig;
  try {
    NetmarbleConfig.load(
        g_pFileDef->getProperty("FILE_INFO_NETMARBLE").c_str());
    bNetmarble = NetmarbleConfig.getPropertyInt("Netmarble") != 0;
  } catch (...) // ŒŒ¼´ —†ê±°ë‚˜ ë­”ê ´ƒ´ žˆ‹¤.
  {
    bNetmarble = false;
  }
  try {
    // „·ë§ˆë¸” ë¡œê·¸¸  •ë³ „¸Œ…
    bNetmarbleLogin = NetmarbleConfig.getPropertyInt("NetmarbleLogin") != 0;
  } catch (...) // ŒŒ¼´ —†ê±°ë‚˜ NetmarbleLogin´  •˜˜ì§ •Š•˜„ ê²½ìš°
  {
    bNetmarbleLogin = bNetmarble; // ””´Š¸ë¡ „¸Œ…
  }

#ifdef __DESIGNED_JAPAN
  // ¼ë³ „·ë§ˆë¸”„ œ„•œ  •ë³.
  bool bNetmarbleJapan = false;
  try {
    // „·ë§ˆë¸” ë¡œê·¸¸  •ë³ „¸Œ…
    bNetmarbleJapan = NetmarbleConfig.getPropertyInt("NetmarbleJapan") != 0;
  } catch (...) // ŒŒ¼´ —†ê±°ë‚˜ NetmarbleLogin´  •˜˜ì§ •Š•˜„ ê²½ìš°
  {
    bNetmarbleJapan = false; // ””´Š¸ë¡ „¸Œ…
  }
#endif

  CT_Done("DIAG: Reached bNetmarble check (GameStart)");
      if (bNetmarble) {
    // -------------------------------------------------
    // read registry...
    // -------------------------------------------------
    HKEY newKey = 0;
    LONG ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                            "SOFTWARE\\Netmarble\\NetmarbleDarkEden", NULL,
                            KEY_ALL_ACCESS, &newKey);
    DWORD dwSize = MAX_PATH;
    char filepath[MAX_PATH];

    if (ret == ERROR_SUCCESS) {
      RegQueryValueEx(newKey, "RunFileName", NULL, NULL,
                      (unsigned char *)filepath, &dwSize);

      char *pFilename = strstr(filepath, "fk.exe");
      if (pFilename != NULL) {
        strcpy(pFilename, "Updater.exe");
        RegSetValueEx(newKey, "RunFileName", NULL, REG_SZ,
                      (unsigned char *)&filepath, strlen(filepath));
      }
    }
    RegCloseKey(newKey);
  }
  //	if( bNetmarble == false )
  //		return -1;

  // MessageBox(g_hWnd, lpCmdLine, NULL, MB_OK);

  //------------------------------------------------------
  // Server IPê° „¤ •˜–´ žˆì§ •Š ê²½ìš° --> Launcher‹¤–‰
  //------------------------------------------------------

  if (strlen(lpCmdLine) == 0) {
#if 0 // defined(USE_UPDATER) disabled for testing
    // ...
    return FALSE;
#else
    g_MaxNPC = 0; // MAX_NPC;
    g_bHAL = true;
    g_bMusicSW = true;
#endif
  }
  else {
    // Updater handoff daha once dogrulandi; kalan argumanlar gercek
    // client/login argumanlaridir.
    g_MaxNPC = 0; // MAX_NPC;
    g_bHAL = true;
    g_bMusicSW = true;
  }

  // [Futecˆ˜ •]
  GetFutecAddress(lpCmdLine);

  //	bool bPatched = ApplyPatch();

  //------------------------------------------------------------------------
  // ScreenEffect ƒ„±
  //------------------------------------------------------------------------
  //	ConvertScreenEffect();

  //------------------------------------------------------------------------
  // ´ œë¶„° EffectScreen— Append˜Š” SPKŠ”
  // ´ ‹œ —„œ Append•´•¼ •œ‹¤.
  // CheckTerriblePatch— ê°™ì´ „£–´„ œ‹¤. - -;;
  //------------------------------------------------------------------------
  // AppendScreenEffect();

  CT_Done("DIAG: Entering CheckTerriblePatch bypass block");
  if (!CheckTerriblePatch()) {
    CT_Done("GameStart: CheckTerriblePatch returned false, exiting!");
    ReleaseAllObjects();

#if defined(OUTPUT_DEBUG) && PHOENIX_CLIENT_LOGS_ENABLED
    if (g_pDebugMessage != NULL) {
      delete g_pDebugMessage;
      g_pDebugMessage = NULL;
    }
#endif
    // by viva hMutex
    /*
    #ifndef OUTPUT_DEBUG
                    ReleaseMutex( hMutex );
    #endif
    */

#ifdef OUTPUT_DEBUG
    DeleteCriticalSection(&g_Lock);
#endif

    return 0;
  }

  //----------------------------------------------------------------
  // Updater.exeë¥ update ‹œ‚¨‹¤.
  //----------------------------------------------------------------
  //	std::ifstream updaterNewFile(UPDATER_NEW_FILENAME, std::ios::binary |
  // std::ios::nocreate);

  // ƒˆë¡œìš´ updater‹¤–‰™”¼´ ì¡´ìž¬•˜ë©..
  if (!_access(UPDATER_NEW_FILENAME, 0)) // updaterNewFile)
  {
    //		updaterNewFile.close();

    // ˜ˆ „ updaterë¥ ì§š°ê³..
    if (remove(UPDATER_FILENAME) == 0) {
      // ƒˆê²ƒì„ ˜ˆ „ê±¸ë¡œ rename
      if (rename(UPDATER_NEW_FILENAME, UPDATER_FILENAME) == 0) {
        DEBUG_ADD("Update Updater.exe OK");
      }
    }
  }

  // strcpy(g_ServerIP, SERVER_IP);
  //---------------------------------------------------
  //  Update ”” ‰† ë¦¬ë ì§›Œì¤‹¤.
  //---------------------------------------------------
  //  updater—„œ ì§›Œ•¼ •˜Š”°..ë¹¼ë¨¹Š” ë°”ëžŒ—.. T_T;;
  //---------------------------------------------------
  char CWD[_MAX_PATH];

  // ˜„ž¬ path½ê¸
  if (_getcwd(CWD, _MAX_PATH) != NULL) {
    char UpdateDir[_MAX_PATH];

    int lenCWD = strlen(CWD);
    if (lenCWD > 1) {
      if (CWD[lenCWD - 1] == '\\') {
        sprintf(UpdateDir, "%s%s", CWD, DIRECTORY_UPDATE);
      } else {
        sprintf(UpdateDir, "%s\\%s", CWD, DIRECTORY_UPDATE);
      }
    }

    if (_rmdir(UpdateDir) != 0) {
      // PATHê° ž˜ëª»ëœ ê²½ìš°
      if (errno == ENOENT) {
      }
      // ë­”ê žˆ–´„œ •ˆ ì§›Œì§ ê²½ìš°..
      else // if (errno==ENOTEMPTY)
      {
        // ˜„ž¬ directoryë¥ ê¸°ì–µ•´‘”‹¤.
        if (_chdir(UpdateDir) == 0) {
          //---------------------------------------------------
          // file•˜‚˜•˜‚˜ë¥ ì§›Œì¤‹¤. T_T;
          //---------------------------------------------------
          struct _finddata_t FileData;
          long hFile;

          // ëª¨ë“  ™”¼„ ½–´˜¨‹¤.
          if ((hFile = _findfirst("*.*", &FileData)) != -1L) {
            while (_findnext(hFile, &FileData) == 0) {
              // .œ¼ë¡ ‹œž‘•˜Š” ê± ì§š¸ •„š” —†‹¹..
              if (FileData.name[0] != '.') {
                remove(FileData.name);
              }
            }

            // 
            _findclose(hFile);
          }

          _chdir(CWD);

          if (_rmdir(UpdateDir) == 0) {
            // ž˜ ì§›Œì¡Œë‹¤.
          }
        }
      }
    }
  }

  RemoveProgressBar();

  //-----------------------------------------------------------------
  // Œ¨ì¹ ‚´š© ì°ì–´ì¤‹¤.
  //-----------------------------------------------------------------
  //	if (bPatched)
  //	{
  //		ShowPatchLogWindow();
  //	}

  BOOL bBadTimer = FALSE;

  MSG msg;

  NETMARBLE_INFO NetmarbleInfo;
  REALSERVER_INFO RealServerInfo;
  if (bNetmarble && bNetmarbleLogin) {
    if (ParsingNetmarble(lpCmdLine, NetmarbleInfo) == false) {
      if (NetmarbleConfig.getPropertyInt("Netmarble") == 1) {
        CT_Done("„·ë§ˆë¸”š© ‹¤¬—´ „·ë§ˆë¸” ™ˆŽ˜´ì§ë¥ †µ•´„œë§ ‹¤–‰´ ê°Š¥•©‹ˆ‹¤.");
        return -1;
      }

      bNetmarble = false;
    }
  } else {
    //		 //test  mode|demention|ID|key
    //		char szBuf[1024];
    //		sprintf(szBuf, "%s", lpCmdLine);

    if (ParsingRealServer(lpCmdLine, g_Dimension, RealServerInfo) == false) {
      // #ifndef OUTPUT_DEBUG
      //			CT_Done("›¹ Ž˜´ì§ —„œ ‹¤‹œ
      // ‹¤–‰•´ì£¼ì‹œê¸ ë°”ëž‹ˆ‹¤.");
      // ShellExecute(NULL, NULL, "www.darkeden.com", NULL, NULL, SW_SHOW);
      // return -1;
      //
      // #endif
    }
  }
  // User Information— Netmarble •ë³ ž¥
  g_pUserInformation->IsNetmarble = bNetmarble;
  g_pUserInformation->IsNetmarbleLogin = bNetmarbleLogin;

  bool bTestServer = false;
  Properties TestServerConfig;
  try {

    TestServerConfig.load(
        g_pFileDef->getProperty("FILE_INFO_TESTSERVER").c_str());
    bTestServer = TestServerConfig.getPropertyInt("TestServer") != 0;
  } catch (...) {
  }
  //
  //	FILE *fp = fopen("Data\\Info\\SuperUser.inf","rt");
  //	MessageBox(NULL,__TIME__,__TIME__,MB_OK);
  //	if(fp != NULL)
  //	{
  //		char szLine[512];
  //		fgets(szLine,512,fp);
  //		char szCompare[512];
  //		bool	superUser = true;
  //		wsprintf(szCompare,"%d%s",__LINE__,__TIME__);
  //		MessageBox(NULL,szCompare,szCompare,MB_OK);
  //		if(strcmp(szLine,szCompare))
  //			superUser = false;
  //		fgets(szLine,512,fp);
  //		fclose(fp);
  //
  //		int Code=0;
  //		char *tempCode = new char[strlen(szCompare)];
  //		memset(tempCode, 0, strlen(szCompare) );
  //		for(int i=0;i<strlen(szCompare);i++)
  //		{
  //			if( i != 0 )
  //				tempCode[i] = szCompare[1] + szCompare[i];
  //			else
  //				tempCode[i] = szCompare[0] + szCompare[i];
  //		}
  //		MessageBox(NULL, tempCode, tempCode, MB_OK);
  //		if(strcmp(tempCode,szLine))
  //			superUser = false;
  //		delete []tempCode;
  //
  //		std::string myIP;
  //
  //		WORD wVersionRequested;
  //		WSADATA wsaData;
  //		char name[255];
  //		PHOSTENT hostinfo;
  //		wVersionRequested = MAKEWORD( 2, 0 );
  //		if ( WSAStartup( wVersionRequested, &wsaData ) == 0 )
  //		{
  //			if( gethostname ( name, sizeof(name)) == 0)
  //			{
  //				if((hostinfo = gethostbyname(name)) != NULL)
  //					myIP= inet_ntoa (*(struct in_addr
  //*)*hostinfo->h_addr_list); 				else
  // superUser = false; 			} else superUser = false;
  // WSACleanup( ); 		} else 			superUser = false;
  //		if(strstr(myIP.c_str(),"220.117.154")==NULL)
  //			superUser = false;
  //
  //		MessageBox(NULL, myIP.c_str(), myIP.c_str(), MB_OK);
  //		if(superUser)
  //		{
  //			g_bSuperUser = true;
  //		} else
  //			g_bSuperUser = false;
  //	}

  DARKEDEN_LANGUAGE Language = CheckDarkEdenLanguage();

  switch (Language) {
  case DARKEDEN_CHINESE:
    gC_ci = new CI_CHINESE;
    break;
  case DARKEDEN_JAPANESE: {
    gC_ci = new CI_JAPAN;
    break;
  }
  default:
    gC_ci = new CI_KOREAN;
    break;
  }

  // BugTrapœ¼ë¡ êµì²´
  // #ifndef _DEBUG
  // 	if( gC_ci->IsKorean() == true )
  // 		InitCrashReport();
  // #endif
  /*#ifdef __VVPROTECT__
  #ifndef _DEBUG //by viva
          VM_START_WITHLEVEL(20)
  #endif
          Defender* _pDefender = NULL;
  #ifdef __DESIGNED_KOREA
          if(!g_IsWow64)
          {
  #endif
                  _pDefender = new Defender();
                  if(!_pDefender->init()) return FALSE;
  #ifdef __DESIGNED_KOREA
          }
  #endif
  #ifndef _DEBUG
          VM_END
  #endif
  #endif */

  CT_Done("DIAG: GameStart -> calling InitApp(%d)", nCmdShow);
  if (InitApp(nCmdShow)) {
    CT_Done("DIAG: InitApp returned TRUE, entering main start block");
#if __CONTENTS(__XTRAP)
    XTrap_C_KeepAlive();
#endif //__XTRAP
    DWORD dwTotal;
    DWORD dwFree;
    HRESULT hr = CDirectDraw::QueryAvailableVideoMemory(
        DDSCAPS_VIDEOMEMORY, &dwTotal, &dwFree);

    DDCAPS driverCaps;
    ZeroMemory(&driverCaps, sizeof(driverCaps));
    driverCaps.dwSize = sizeof(driverCaps);

    hr = CDirectDraw::QueryDeviceCaps(&driverCaps);
#ifndef _DEBUG
    VM_START_WITHLEVEL(20)
#endif
    // by viva
    APICheck _ApiCheck;
    CT_Done("DIAG: Before _ApiCheck.init()");
      bool apRet = _ApiCheck.init();
      CT_Done("DIAG: After _ApiCheck.init(), result: %d", (int)apRet);
      if (!apRet)
      return FALSE;

    // end
#ifndef _DEBUG
    VM_END
#endif
    g_dwVideoMemory = driverCaps.dwVidMemTotal;
    //		g_dwVideoMemory = driverCaps.dwVidMemFree;

    // „·ë§ˆë¸”š©
    CT_Done("DIAG: Reached bNetmarble check (GameStart)");
      if (bNetmarble) {
      g_pUserInformation->NetmarbleID = NetmarbleInfo.ID;
      g_pUserInformation->NetmarblePassword = NetmarbleInfo.Password;
      g_pUserInformation->WorldID = NetmarbleInfo.WorldID;
      g_pUserInformation->ServerID = NetmarbleInfo.ServerID;
      //			g_pUserInformation->GoreLevel =
      // NetmarbleInfo.bGore;	//zzi - „·ë§ˆë¸” ´š© œ•œ UI ì²˜ë¦¬
      //			g_pUserInformation->bNetmarbleGoreLevel =
      // NetmarbleInfo.bGore;

#ifdef __DESIGNED_JAPAN
      g_pUserInformation->IsNetmarbleJapan = bNetmarbleJapan;
#endif

    } else {
      // mode|demention|ID|key
      g_pUserInformation->IsAutoLogIn = RealServerInfo.bMode;
      if (true == g_pUserInformation->IsAutoLogIn) {
        g_pUserInformation->UserID = RealServerInfo.ID;
        g_pUserInformation->AutoLogInKeyValue = RealServerInfo.Key;

        g_Dimension = RealServerInfo.WorldID;

#ifdef OUTPUT_DEBUG
        char szBuf[1024];
        sprintf(szBuf, "mode = %d, id = %d,world = %s,key = %s",
                g_pUserInformation->IsAutoLogIn,

                g_Dimension,

                g_pUserInformation->UserID.GetString(),

                g_pUserInformation->AutoLogInKeyValue.GetString());
        DEBUG_ADD(szBuf);
#endif
      }
    }
    /*#ifdef __VVPROTECT__
    #ifndef _DEBUG//by viva
                    VM_START_WITHLEVEL(20)
    #endif
    #ifdef __DESIGNED_KOREA
                    if(!g_IsWow64)
    #endif
                            if(!_pDefender->setProtectProcessPid())
                                    return FALSE;

                    int checkDriverTime = g_CurrentTime;
    #ifndef _DEBUG
                    VM_END
    #endif
    #endif*/

    CT_Done("DIAG: GameStart -> Check IsKorean");
    if (gC_ci->IsKorean())
      g_pUserInformation->SetKorean();
    else if (gC_ci->IsChinese())
      g_pUserInformation->SetChinese();
    else if (gC_ci->IsJapanese())
      g_pUserInformation->SetJapanese();

    //		#if !defined(OUTPUT_DEBUG) && !defined(__DEBUG_OUTPUT__)
    //		g_hHook =
    // SetWindowsHookEx(WH_KEYBOARD_LL,(HOOKPROC)KeyboardHook, hInstance, 0);
    //
    //		SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, TRUE, NULL,
    // NULL); 		#endif

    // static DWORD lastTime=1;
    //------------------------------
    //  ì´ˆê¸°™” ˜ì§ •Š•˜œ¼ë© ì´ˆê¸°™”•œ‹¤.
    //------------------------------
    CT_Done("DIAG: GameStart -> g_pTopView init");
    if (g_pTopView && !g_pTopView->IsInit()) {
      CT_Done("DIAG: Calling g_pTopView->Init()");
      g_pTopView->Init();
      CT_Done("DIAG: RETURNED FROM g_pTopView->Init()");
    } else {
      CT_Done("DIAG: SKIPPED g_pTopView->Init() (NULL or already init)");
    }

    CT_Done("DIAG: Setting g_bActiveApp = TRUE");
    g_bActiveApp = TRUE;
    // g_bActiveGame = TRUE;
    // DX / CRT init sonrasi UEF tekrar — dump filter'i korunur.
    Phoenix_InstallFatalHandlers();
    DEBUG_CMD(MIN_CLRSCR, "‹œž‘");
    DEBUG_CMD(MIN_SHOWWND, "------------------------------");

    int iGameRunTime = 0;   // ê²Œìž„
    int iGameBreakTime = 0; // ê²Œìž„ ë©ˆì¶¤ ‹œê°

    DWORD dwthread = 0;
#if __CONTENTS(__XTRAP)
    CT_Done("DIAG: Creating XTrap_Check_Alive thread");
    CreateThread(NULL, 0, XTrap_Check_Alive, NULL, 0, &dwthread);
#endif //__XTRAP

    // 2026-08-31 PERF FIX (FPS analiz 1.5 — tick jitter): timeGetTime default
    // cozunurlugu ~15.6ms; 62ms logic tick ve g_CurrentTime tabanli tum
    // zamanlayicilar bu granularitede seker (jitter). timeBeginPeriod(1) ile
    // 1ms cozunurluk — legacy oyunlarin standart pratigi, oyun mekanigini
    // degistirmez (ayni saat, sadece daha hassas okuma). Dongu cikisinda
    // timeEndPeriod(1) ile iade edilir.
    timeBeginPeriod(1);

    CT_Done("DIAG: Entering main while(TRUE) loop");

    while (TRUE) {
      static DWORD lastLoopDiag = 0;
      DWORD heartbeatTime = timeGetTime();
      if (heartbeatTime - lastLoopDiag > 3000) {
          CT_Done("DIAG: LOOP HEARTBEAT - ActiveApp:%d ActiveGame:%d", g_bActiveApp, g_bActiveGame);
          lastLoopDiag = heartbeatTime;
      }
      g_bActiveGame = TRUE; // ž„‹œë¡ ™œ„±™” ƒƒœë¡ „¸Œ…•œ‹¤.

      if (_ApiCheck.CheckApi()) {
        CT_Done("DIAG: Main loop break - CheckApi failed");
        Phoenix_ClientLifecycleLogf("main loop break: CheckApi failed mode=%d update=%p",
                                    (int)g_Mode, (void*)g_pUpdate);
        break;
      }

      if (g_Mode == MODE_QUIT && g_pUpdate == NULL && !g_bActiveApp) {
        CT_Done("DIAG: Main loop break - MODE_QUIT inactive app");
        Phoenix_ClientLifecycleLogf("main loop break: MODE_QUIT inactive app update=%p",
                                    (void*)g_pUpdate);
        break;
      }
      /*#ifdef __VVPROTECT__
                              if(g_CurrentTime - checkDriverTime > 1000)
                              {
      #ifndef _DEBUG//by viva
              VM_START_WITHLEVEL(20)
      #endif
      #ifdef __DESIGNED_KOREA
                                      if(!g_IsWow64)
      #endif
                                              if(_pDefender->getCheck() !=
      ::GetCurrentProcessId()) break; checkDriverTime = g_CurrentTime; #ifndef
      _DEBUG VM_END #endif
                              }
      #ifdef __DESIGNED_KOREA
                              if(!g_IsWow64)
      #endif
                                      if(!_pDefender->getCheckLive())	break;
      #endif*/

      if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
        if (!GetMessage(&msg, NULL, 0, 0)) {
          CT_Done("DIAG: Main loop break - GetMessage returned 0 (WM_QUIT)");
          Phoenix_ClientLifecycleLogf("main loop break: WM_QUIT mode=%d update=%p activeApp=%d",
                                      (int)g_Mode, (void*)g_pUpdate, (int)g_bActiveApp);
          break;
        }
        // return msg.wParam;
        // by viva
        if (msg.message == WM_SYSCOMMAND && msg.wParam == SC_CLOSE)
          // gpC_base->SendMessage(UI_QUITEXIT, 0, 0);
          continue;
        // end
        else {
          TranslateMessage(&msg);
          DispatchMessage(&msg);

          if (msg.message == WM_KEYDOWN) {
            switch (LOWORD(msg.wParam)) {
            case VK_RETURN:
              if (g_pWin32Edite)
                g_pWin32Edite->OnReturn();
              break;
            }
          }

          if (msg.message == WM_LBUTTONDBLCLK ||
              msg.message == WM_LBUTTONDOWN ||
              msg.message == WM_RBUTTONDBLCLK ||
              msg.message == WM_RBUTTONDOWN) {
            POINT cursorPoint;
            GetCursorPos(&cursorPoint);
            ScreenToClient(g_hWnd, &cursorPoint);

            if (g_pWin32Edite) {
              if (g_pWin32Edite->PtInRect(cursorPoint))
                g_pWin32Edite->SetFocus(true);
              else
                g_pWin32Edite->SetFocus(false);
            }
          }
        }
      }
      if (g_bActiveApp
#ifdef OUTPUT_DEBUG
                 || g_bTestMode
#endif
      ) {
#ifdef OUTPUT_DEBUG
//	DEBUG_ADD("u-");
#endif
        // if ( )
        {
          // UpdateFrame();
          //  timer
          g_CurrentTime = timeGetTime();
          Phoenix_ClientSpeedGuardTick(g_CurrentTime);

          // if (g_CurrentTime - lastTime > g_UpdateDelay)
          {
            // CDirectDrawê° ž‘™ì¤‘ì´ì§ •Š„ ê²½ìš°—Š” return
            // if (g_bActiveApp)// && CDirectDraw::IsActive())
            if (g_pUpdate != NULL) {
              // …¸ŒŒ‹¬.. œ¼ .. --;;
              CWinUpdate *pCurrentUpdate = g_pUpdate;

              static DWORD lastUpdateLog = 0;
              if (g_CurrentTime - lastUpdateLog > 1000) {
                  // Removed flood log
                  lastUpdateLog = g_CurrentTime;
              }

              const bool fpEnabled = Phoenix_FrameProbe_Enabled();
              LONGLONG fpFrameStart = 0;
              LONGLONG fpUpdateStart = 0;
              LONGLONG fpDrawStart = 0;
              double fpUpdateMs = 0.0;
              double fpDrawMs = 0.0;
              if (fpEnabled)
              {
                fpFrameStart = Phoenix_FrameProbe_Now();
                fpUpdateStart = fpFrameStart;
              }

              pCurrentUpdate->Update();
              if (g_Mode == MODE_QUIT || g_pUpdate == NULL || !g_bActiveApp) {
                CT_Done("DIAG: Main loop break - update requested quit");
                Phoenix_ClientLifecycleLogf(
                    "main loop break: update requested quit mode=%d update=%p activeApp=%d currentUpdate=%p",
                    (int)g_Mode, (void*)g_pUpdate, (int)g_bActiveApp,
                    (void*)pCurrentUpdate);
                break;
              }
              if (fpEnabled)
              {
                LONGLONG fpAfterUpdate = Phoenix_FrameProbe_Now();
                fpUpdateMs = Phoenix_FrameProbe_Ms(fpUpdateStart, fpAfterUpdate);
                fpDrawStart = fpAfterUpdate;
              }
              // 2026-08-31 PERF FIX (FPS analiz 1.1 — cift Present):
              // Referans client ana dongusu (ClientSRC_Referans Client.cpp:4373)
              // yalnizca Update() cagirir; her update sinifi cizimini kendi
              // Update()'i icinde yapar (CGameUpdate.cpp draw dali + Flip,
              // CWaitUIUpdate.cpp:284). Buradaki ek UpdateDraw() cagrisi Phoenix
              // portuna ilk snapshot oncesi eklenmisti ve iterasyon basina
              // IKINCI tam cizim + Flip uretiyordu — VSync (INTERVAL_ONE) ile
              // her Flip bir vblank bekledigi icin FPS tavani monitorun
              // YARISINA dusuyordu (60Hz->~30). Referans parity'ye donuldu:
              // tek cizim/tek Present. COpeningUpdate/CWaitPacketUpdate icin
              // bu cagri zaten no-op idi (base CWinUpdate::UpdateDraw bos).
              // (Kural 4.1 geregi silinmedi, aciklamali devre disi.)
              // {
              //   static DWORD s_lastUpdateDrawDiag = 0;
              //   DWORD nowUD = timeGetTime();
              //   if (nowUD - s_lastUpdateDrawDiag > 3000) {
              //     CT_Done("DIAG: Main loop calling UpdateDraw on ptr=%p", (void*)pCurrentUpdate);
              //     s_lastUpdateDrawDiag = nowUD;
              //   }
              // }
              // pCurrentUpdate->UpdateDraw();
              if (fpEnabled)
              {
                LONGLONG fpFrameEnd = Phoenix_FrameProbe_Now();
                fpDrawMs = Phoenix_FrameProbe_Ms(fpDrawStart, fpFrameEnd);
                Phoenix_FrameProbe_RecordFrame(
                    Phoenix_FrameProbe_Ms(fpFrameStart, fpFrameEnd),
                    fpUpdateMs,
                    fpDrawMs,
                    (int)g_bActiveApp,
                    (int)g_bActiveGame,
                    (int)(GetForegroundWindow() == g_hWnd),
                    g_hWnd ? (int)IsIconic(g_hWnd) : 0,
                    g_hWnd ? (int)IsWindowVisible(g_hWnd) : 0);
              }

#if defined(__METROTECH_TEST__) && PHOENIX_CLIENT_LOGS_ENABLED
              CheckFlushLogFile();
#endif
            }

            // lastTime = g_CurrentTime;
          }
#ifdef __NPROTECT__
#ifndef __DESIGNED_INTERNATION
          // 20080902 ‹¤‹œê° ì²´í¬ ’¤Š¦ê²Œë¼„ ë§Œë“¤–´„œ ‹¤–‰ ˜•œ ˜„ž¬
          // closeNPROTECTŠ” •„ë¬ ê¸°ëŠ¥„ —†‹¤.
#if __CONTENTS(__NPROTECT)
          if (CheckNPROTECT() != NPROTECT_OK) {
            CT_Done("DIAG: Main loop break - CheckNPROTECT failed");
            Phoenix_ClientLifecycleLogf("main loop break: CheckNPROTECT failed");
            g_nProtectSTATUS = NPROTECT_CHECK_ERROR;
            break;
          }
#endif //__NPROTECT
#endif //__DESIGNED_INTERNATION
          if (g_nProtectSTATUS != NPROTECT_NORMAL) {
            CT_Done("DIAG: Main loop break - nProtectSTATUS (status:%d)", g_nProtectSTATUS);
            Phoenix_ClientLifecycleLogf("main loop break: nProtect status=%d", g_nProtectSTATUS);
            bBadTimer = TRUE;
            break;
          }
#endif

          if (g_bForceExitBynProtect) {
            CT_Done("DIAG: Main loop break - g_bForceExitBynProtect is TRUE");
            Phoenix_ClientLifecycleLogf("main loop break: force exit by nProtect");
            bBadTimer = TRUE;
            break;
          }

          // #ifdef OUTPUT_DEBUG
          DWORD timeGap = g_CurrentTime - g_StartTime;

          if (timeGap > 1000) {
            // UpdateFrame();

            g_FrameRate = (g_FrameCount - g_StartFrameCount) * 1000 / timeGap;

            // 15 fps ´ƒ
            g_bGoodFPS = (g_FrameRate >= g_FrameGood);

            g_StartTime = g_CurrentTime;
            g_StartFrameCount = g_FrameCount;
          }
        }

#ifdef OUTPUT_DEBUG
//	DEBUG_ADD("-u");
#endif
      } else {
        // Make sure we go to sleep if we have nothing else to do
        WaitMessage();
      }
    }

    // 2026-08-31 PERF FIX (FPS analiz 1.5): timeBeginPeriod(1) iadesi.
    timeEndPeriod(1);
#ifndef __OUTPUT_DEBUG__
    SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, FALSE, NULL, NULL);
#endif
  } else {
    //	InitFail("Initialize Failed!");
  }

  //-----------------------------------------------------------------------------
  // ´ë¨ ´ƒœ¼ë¡ ì¢…ë£Œ•˜Š” ê²½ìš°
  //-----------------------------------------------------------------------------
  if (false && bBadTimer) {
    if (g_pUIDialog != NULL) {
#if defined(__NPROTECT_OLD_VERSION__) && defined(__NPROTECT__)

      if (g_nProtectSTATUS != NPROTECT_NORMAL &&
          g_nProtectSTATUS != NPROTECT_MAX) {
        g_pUIDialog->PopupFreeMessageDlg(
            g_nProtectMessage[g_nProtectSTATUS].c_str());
      } else
        g_pUIDialog->PopupFreeMessageDlg(
            "ë§¤í¬ë¡œë‚˜ ë°”ì´Ÿ¬Š¤ê° ë°œê²¬˜–´ ‹¤¬—´´ ì¢…ë£Œ©‹ˆ‹¤. ë°”ì´Ÿ¬Š¤ "
            "ê²‚¬ë¥ •´ë³´ì‹œê³,  •ƒ œ¼ë¡ ê²Œìž„ ì§„í–‰„ •˜Š”° ´Ÿ¬•œ ë¬¸ì œê° ˜ "
            "ë°œìƒœ‹¤ë© bug@darkeden.comœ¼ë¡ ë©”ì¼„ ë³´ë‚´ì£¼ì„¸š”.",
            -1, -1, 0);
#endif

#if defined(__NPROTECT__) && !defined(__NPROTECT_OLD_VERSION__)
      if (g_bForceExitBynProtect) {
        if (g_nProtectMessage.find(g_nProtectErrorMessage) !=
            g_nProtectMessage.end()) {
          char szTemp[256];
          wsprintf(szTemp, "%s [%d]",
                   g_nProtectMessage[g_nProtectErrorMessage].c_str(),
                   g_nProtectErrorMessage2);
          g_pUIDialog->PopupFreeMessageDlg(szTemp);
        } else {
          char szTemp[256];
          wsprintf(szTemp,
                   "nProtect˜ •Œ ˆ˜ —†Š” ˜¤ë¥ —Ÿ¬ì½”ë“œ[%d][%d]ž…‹ˆ‹¤. "
                   "‹¤¬—´„ ì¢…ë£Œ•©‹ˆ‹¤.",
                   g_nProtectErrorMessage, g_nProtectErrorMessage2);
          g_pUIDialog->PopupFreeMessageDlg(szTemp);
        }
      }
#endif

      //			if(g_bForceExitBynProtect2)
      //			{
      //				g_pUIDialog->PopupFreeMessageDlg(
      //"nProtect ê´ ¨ ŒŒ¼ ì´ˆê¸°™” —Ÿ¬ê° ë°œìƒ–ˆŠµ‹ˆ‹¤. ê´ë¦¬ìž ê³„ì •œ¼ë¡
      // ë¡œê·¸¸•˜—¬ ê²Œìž„„ ‹¤–‰•˜‹œê¸ ë°”ëž‹ˆ‹¤.", -1, -1, 0 );
      // }else
      //			{
      //				if(g_bForceExitBynProtect)
      //					g_pUIDialog->PopupFreeMessageDlg(
      //"ë°”ì´Ÿ¬Š¤ ë° •´‚¹ ì°¨ë‹¨ê¸°ì— ˜•´ ‹¤¬—´´ ì¢…ë£Œ©‹ˆ‹¤. ë°”ì´Ÿ¬Š¤ ê²‚¬ë¥
      // •´ë³´ì‹œê³,  •ƒ œ¼ë¡ ê²Œìž„ ì§„í–‰„ •˜Š”° ´Ÿ¬•œ ë¬¸ì œê° ˜ ë°œìƒœ‹¤ë©,
      // bug@darkeden.com œ¼ë¡ ë©”ì¼„ ë³´ë‚´ì£¼ì„¸š”.", -1, -1, 0 );
      // else
      // g_pUIDialog->PopupFreeMessageDlg( "ë§¤í¬ë¡œë‚˜ ë°”ì´Ÿ¬Š¤ê° ë°œê²¬˜–´
      // ‹¤¬—´´ ì¢…ë£Œ©‹ˆ‹¤. ë°”ì´Ÿ¬Š¤ ê²‚¬ë¥ •´ë³´ì‹œê³,  •ƒ œ¼ë¡ ê²Œìž„ ì§„í–‰„
      // •˜Š”° ´Ÿ¬•œ ë¬¸ì œê° ˜ ë°œìƒœ‹¤ë© bug@darkeden.comœ¼ë¡ ë©”ì¼„
      // ë³´ë‚´ì£¼ì„¸š”.", -1, -1, 0 );
      //			}
      //			//CT_Done("Timer Error!");

      while (1) {
        UpdateInput();
        UpdateMouse();

        //				if (CDirect3D::IsHAL())
        //				{
        //					gC_vs_ui.Show();
        //				}
        //				else
        {
          gC_vs_ui.Show();

          POINT point = {0, 0};
          RECT rect = {0, 0, SURFACE_WIDTH, SURFACE_HEIGHT};

          if (g_pBack != NULL && g_pLast != NULL) {
            g_pBack->BltNoColorkey(&point, g_pLast, &rect);
          }
        }
        CDirectDraw::Flip();

        if (g_pDXInput->KeyDown(DIK_ESCAPE) ||
            g_pDXInput->KeyDown(DIK_NUMPADENTER) ||
            g_pDXInput->KeyDown(DIK_RETURN) || g_pDXInput->KeyDown(DIK_SPACE)) {
          break;
        }
      }
    }
  }

  // •´ƒ„  •ë³ ž¥
  SaveResolutionConfig();

  //-----------------------------------------------------------------------------
  // ëª¨ë“  object“¤  œê±
  //-----------------------------------------------------------------------------
  Phoenix_ClientLifecycleLogf("GameStart cleanup begin mode=%d update=%p activeApp=%d needUpdate=%d",
                              (int)g_Mode, (void*)g_pUpdate, (int)g_bActiveApp,
                              (int)g_bNeedUpdate);
  ReleaseAllObjects();
#ifdef __WEB_BROWSER__
  if (NULL != g_pWebBrowser) {
    g_pWebBrowser->Quit();
    g_pWebBrowser->Release();
    DeleteObject(g_pWebBrowser);
    g_pWebBrowser = NULL;
    CoUninitialize();
  }
#endif
  // #if defined(OUTPUT_DEBUG) && !defined(_DEBUG)
  delete gC_ci;
#ifdef OUTPUT_DEBUG
//	_close(CLogFile);
//	CheckCLogFile();
#endif
#ifdef __VVPROTECT__
#ifndef _DEBUG // by viva
  VM_START_WITHLEVEL(20)
#endif
#ifdef __DESIGNED_KOREA
  if (!g_IsWow64)
#endif
  //		if(_pDefender->openVVProtect()!=INVALID_HANDLE_VALUE)
  //			_pDefender->UnLoadDriver();

#ifndef _DEBUG
    VM_END
#endif
#endif
  // •´ƒ„ ë³µêµ¬//by viva
  HWND hWnd = ::FindWindow("fk", "ë£©ìšºì¡°æŽ‰");
  if (hWnd == NULL)
    ChangeDisplaySettings(NULL, 0);

#if defined(OUTPUT_DEBUG) && PHOENIX_CLIENT_LOGS_ENABLED
  if (g_bNeedUpdate) {
    DEBUG_ADD("Need to Update! Run Updater.exe");
  }

  DEBUG_ADD("---------------[  End ReleaseAllObjects  ]---------------");

  if (g_pDebugMessage != NULL) {
    DEBUG_ADD("--------------- Delete DebugMessageArray --------------");
    delete g_pDebugMessage;
    g_pDebugMessage = NULL;
  }
#endif
  DEBUG_CMD(MIN_HIDEWND, "------------------------------");

  if (g_pClientConfig != NULL) {
    delete g_pClientConfig;
    g_pClientConfig = NULL;
  }

#ifdef __NPROTECT__
  CloseNPROTECT();
#endif

  // by viva hMutex
  /*
  #ifndef OUTPUT_DEBUG
          ReleaseMutex( hMutex );
  #endif
  */

#ifdef OUTPUT_DEBUG
  DeleteCriticalSection(&g_Lock);
#endif

  //----------------------------------------------------------------
  // Updaterë¥ ‹¤–‰‹œ‚¨‹¤.
  //----------------------------------------------------------------
  if (g_bNeedUpdate) {
    //_spawnl(_P_NOWAIT, UPDATER_FILENAME, UPDATER_FILENAME, "UPDATE", NULL);
    char szTemp[2048];
    sprintf(szTemp, "UPDATE %s", lpCmdLine);
    _chdir(g_CWD);
    _spawnl(_P_OVERLAY, UPDATER_FILENAME, UPDATER_FILENAME, szTemp, NULL);
    //		ShellExecute(g_hWnd, NULL, UPDATER_FILENAME, szTemp, NULL,
    // SW_SHOW);
  }

  return 0;
}

static LONG WINAPI Phoenix_UnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionPointers);

typedef BOOL(WINAPI *PhoenixMiniDumpWriteDumpFn)(
    HANDLE, DWORD, HANDLE, MINIDUMP_TYPE, PMINIDUMP_EXCEPTION_INFORMATION,
    PMINIDUMP_USER_STREAM_INFORMATION, PMINIDUMP_CALLBACK_INFORMATION);

static volatile LONG g_PhoenixFatalHandlerEntered = 0;
static HMODULE g_PhoenixDbgHelpModule = NULL;
static PhoenixMiniDumpWriteDumpFn g_PhoenixMiniDumpWriteDump = NULL;
static PVOID g_PhoenixVectoredContinueHandle = NULL;
static LPTOP_LEVEL_EXCEPTION_FILTER(WINAPI *g_RealSetUnhandledExceptionFilter)(
    LPTOP_LEVEL_EXCEPTION_FILTER) = NULL;

static void Phoenix_GetClientDirectoryA(char *outDir, size_t outChars) {
  if (outDir == NULL || outChars == 0)
    return;
  outDir[0] = '\0';
  char modulePath[MAX_PATH] = {0};
  if (GetModuleFileNameA(NULL, modulePath, MAX_PATH) == 0)
    return;
  char *pSlash = strrchr(modulePath, '\\');
  if (pSlash != NULL)
    *(pSlash + 1) = '\0';
  else
    modulePath[0] = '\0';
  StringCchCopyA(outDir, outChars, modulePath);
}

static void Phoenix_EnsureDumpDirectoryA(char *outDumpDir, size_t outChars) {
  if (outDumpDir == NULL || outChars == 0)
    return;
  outDumpDir[0] = '\0';
  char moduleDir[MAX_PATH] = {0};
  Phoenix_GetClientDirectoryA(moduleDir, MAX_PATH);
  StringCchPrintfA(outDumpDir, outChars, "%sDump", moduleDir);
  CreateDirectoryA(outDumpDir, NULL);
}

static void Phoenix_AppendFatalLogLine(const char *line) {
  if (line == NULL)
    return;

  char moduleDir[MAX_PATH] = {0};
  Phoenix_GetClientDirectoryA(moduleDir, MAX_PATH);

  char dataDir[MAX_PATH] = {0};
  char logDir[MAX_PATH] = {0};
  char logPath[MAX_PATH] = {0};
  StringCchPrintfA(dataDir, MAX_PATH, "%sdata", moduleDir);
  CreateDirectoryA(dataDir, NULL);
  StringCchPrintfA(logDir, MAX_PATH, "%sdata\\log", moduleDir);
  CreateDirectoryA(logDir, NULL);
  StringCchPrintfA(logPath, MAX_PATH, "%sdata\\log\\client_fatal_exception.log",
                   moduleDir);

  // CRT bozuk olsa bile dump teşhis satırı kalsın.
  HANDLE hLog = CreateFileA(logPath, FILE_APPEND_DATA,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hLog != INVALID_HANDLE_VALUE) {
    DWORD written = 0;
    const DWORD len = static_cast<DWORD>(strlen(line));
    WriteFile(hLog, line, len, &written, NULL);
    WriteFile(hLog, "\r\n", 2, &written, NULL);
    FlushFileBuffers(hLog);
    CloseHandle(hLog);
    return;
  }

  FILE *fp = fopen(logPath, "a");
  if (fp == NULL)
    return;
  fputs(line, fp);
  fputc('\n', fp);
  fflush(fp);
  fclose(fp);
}

static void Phoenix_WriteFatalExceptionLog(PEXCEPTION_POINTERS pExceptionPointers,
                                           const char *pDumpPath,
                                           BOOL dumpWritten,
                                           DWORD dumpCreateError,
                                           DWORD dumpWriteError) {
  SYSTEMTIME stLocalTime;
  GetLocalTime(&stLocalTime);

  DWORD exceptionCode = 0;
  void *exceptionAddress = NULL;
  if (pExceptionPointers != NULL && pExceptionPointers->ExceptionRecord != NULL) {
    exceptionCode = pExceptionPointers->ExceptionRecord->ExceptionCode;
    exceptionAddress = pExceptionPointers->ExceptionRecord->ExceptionAddress;
  }

  char line[1024] = {0};
  StringCchPrintfA(
      line, 1024,
      "%04d-%02d-%02d %02d:%02d:%02d fatal exception code=0x%08lX address=%p "
      "pid=%lu tid=%lu dump=%s written=%d createErr=%lu writeErr=%lu",
      stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
      stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
      exceptionCode, exceptionAddress,
      GetCurrentProcessId(), GetCurrentThreadId(),
      (pDumpPath != NULL && pDumpPath[0] != '\0') ? pDumpPath : "<none>",
      (int)dumpWritten, (unsigned long)dumpCreateError,
      (unsigned long)dumpWriteError);
  Phoenix_AppendFatalLogLine(line);
}

// 2026-09-04 LIVE hazirlik: lifecycle breadcrumb'lari (GameStart enter,
// fatal handlers installed, WINDOWGEOM, main loop break vb.) her acilista
// data\log\client_fatal_exception.log'a satir yaziyordu. Oyuncu makinesinde
// normal kosuda hicbir log dosyasi olusmamali — flag default 0. Gercek crash
// metadata satiri (Phoenix_WriteFatalExceptionLog -> Phoenix_AppendFatalLogLine)
// ve MiniDump uretimi ETKILENMEZ, sadece crash aninda yazar. Debug'da 1 yap.
#ifndef PHOENIX_LIFECYCLE_LOG_ENABLED
#define PHOENIX_LIFECYCLE_LOG_ENABLED 0
#endif

static void Phoenix_WriteLifecycleLog(const char *message) {
#if !PHOENIX_LIFECYCLE_LOG_ENABLED
  (void)message;
  return;
#else
  SYSTEMTIME stLocalTime;
  GetLocalTime(&stLocalTime);
  char line[768] = {0};
  StringCchPrintfA(line, 768,
                   "%04d-%02d-%02d %02d:%02d:%02d tid=%lu lifecycle %s",
                   stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
                   stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
                   GetCurrentThreadId(),
                   message != NULL ? message : "<null>");
  Phoenix_AppendFatalLogLine(line);
#endif
}

extern "C" void Phoenix_ClientLifecycleLogf(const char *format, ...) {
  static const bool kPhoenixClientLifecycleLogEnabled = false;
  if (!kPhoenixClientLifecycleLogEnabled)
    return;

  char message[1024] = {0};
  va_list args;
  va_start(args, format);
  _vsnprintf_s(message, sizeof(message), _TRUNCATE,
               format != NULL ? format : "<null>", args);
  va_end(args);
  Phoenix_WriteLifecycleLog(message);
}

// Zone OOM / LoadZone mem / GCUI stage teshis satirlari.
// Production: 0 (hop basina FlushFileBuffers → gereksiz I/O).
// Crash/OOM debug: 1 yap, rebuild. Cagri yerleri silinmez.
#ifndef PHOENIX_FATAL_DIAG_LOG_ENABLED
#define PHOENIX_FATAL_DIAG_LOG_ENABLED 0
#endif

// SEH disi kontrollu cikislar + mem teshis — flag kapaliyken no-op.
// Gercek fatal exception satiri (Phoenix_WriteFatalExceptionLog) her zaman yazar.
extern "C" void Phoenix_FatalDiagLogf(const char *format, ...) {
#if !PHOENIX_FATAL_DIAG_LOG_ENABLED
  (void)format;
  return;
#else
  if (format == NULL)
    return;
  char message[1024] = {0};
  va_list args;
  va_start(args, format);
  _vsnprintf_s(message, sizeof(message), _TRUNCATE, format, args);
  va_end(args);
  Phoenix_AppendFatalLogLine(message);
#endif
}

extern "C" void Phoenix_ClientLifecycleLogStack(const char *label) {
  static const bool kPhoenixClientLifecycleLogEnabled = false;
  if (!kPhoenixClientLifecycleLogEnabled)
    return;

  Phoenix_ClientLifecycleLogf("%s", label != NULL ? label : "stack");

  void *frames[16] = {0};
  const USHORT frameCount = CaptureStackBackTrace(0, 16, frames, NULL);

  for (USHORT i = 0; i < frameCount; ++i) {
    Phoenix_ClientLifecycleLogf("  stack[%u]=%p", (unsigned)i, frames[i]);
  }
}

static MINIDUMP_TYPE Phoenix_GetPreferredMiniDumpType() {
  return static_cast<MINIDUMP_TYPE>(MiniDumpNormal |
                                    MiniDumpWithIndirectlyReferencedMemory);
}

// PhoenixMiniDumper protokol surumu (2026-08-28 kucuk dump duzeni).
static const unsigned kPhoenixMiniDumperProtocolVersion = 2U;

static bool Phoenix_WantFullMemoryDump() {
  char buf[8] = {};
  return GetEnvironmentVariableA("PHOENIX_FULL_MEMORY_DUMP", buf, sizeof(buf)) >
             0 &&
         buf[0] == '1';
}

// Tipik mini dump birkaç MB; 128MB ustu genelde MiniDumpWithFullMemory / eski helper.
static ULONGLONG Phoenix_GetMaxDumpBytes() {
  const ULONGLONG kDefaultMaxBytes = 128ULL * 1024ULL * 1024ULL;
  char buf[32] = {};
  if (GetEnvironmentVariableA("PHOENIX_MAX_DUMP_MB", buf, sizeof(buf)) == 0 ||
      buf[0] == '\0')
    return kDefaultMaxBytes;

  char *end = NULL;
  const unsigned long mb = strtoul(buf, &end, 10);
  if (end == buf || mb == 0UL)
    return kDefaultMaxBytes;
  return static_cast<ULONGLONG>(mb) * 1024ULL * 1024ULL;
}

// Helper --version stdout (ornek: "2\n"). Eski FullMemory binary --version bilmez.
static bool Phoenix_QueryMiniDumperProtocol(const char *helperPath,
                                           unsigned *outProtocol) {
  if (helperPath == NULL || helperPath[0] == '\0' || outProtocol == NULL)
    return false;
  *outProtocol = 0;

  SECURITY_ATTRIBUTES sa{};
  sa.nLength = sizeof(sa);
  sa.bInheritHandle = TRUE;
  HANDLE readPipe = NULL;
  HANDLE writePipe = NULL;
  if (!CreatePipe(&readPipe, &writePipe, &sa, 0))
    return false;
  SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0);

  char cmdLine[MAX_PATH + 32] = {0};
  StringCchPrintfA(cmdLine, MAX_PATH + 32, "\"%s\" --version", helperPath);

  STARTUPINFOA si{};
  si.cb = sizeof(si);
  si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  si.hStdOutput = writePipe;
  si.hStdError = writePipe;
  si.wShowWindow = SW_HIDE;

  PROCESS_INFORMATION pi{};
  const BOOL created =
      CreateProcessA(NULL, cmdLine, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL,
                     NULL, &si, &pi);
  CloseHandle(writePipe);
  if (!created) {
    CloseHandle(readPipe);
    return false;
  }

  char buf[64] = {0};
  DWORD nRead = 0;
  ReadFile(readPipe, buf, sizeof(buf) - 1, &nRead, NULL);
  WaitForSingleObject(pi.hProcess, 2000);
  CloseHandle(readPipe);
  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);

  char *end = NULL;
  const unsigned long parsed = strtoul(buf, &end, 10);
  if (end == buf || parsed == 0UL)
    return false;
  *outProtocol = static_cast<unsigned>(parsed);
  return true;
}

static void Phoenix_WarnIfMiniDumperStale() {
  char moduleDir[MAX_PATH] = {0};
  Phoenix_GetClientDirectoryA(moduleDir, MAX_PATH);
  char helperPath[MAX_PATH] = {0};
  StringCchPrintfA(helperPath, MAX_PATH, "%sPhoenixMiniDumper.exe", moduleDir);

  if (GetFileAttributesA(helperPath) == INVALID_FILE_ATTRIBUTES) {
    Phoenix_WriteLifecycleLog(
        "WARN PhoenixMiniDumper.exe missing next to Client.exe");
    return;
  }

  // Dosya tarihi degil protokol: Client her rebuild'de yeni, MiniDumper
  // ayni protokolde kalabilir. Eski heuristic surekli false WARN yaziyordu.
  unsigned helperProtocol = 0;
  if (!Phoenix_QueryMiniDumperProtocol(helperPath, &helperProtocol) ||
      helperProtocol < kPhoenixMiniDumperProtocolVersion) {
    char line[192] = {0};
    StringCchPrintfA(line, 192,
                     "WARN PhoenixMiniDumper.exe protocol=%u expected=%u — "
                     "copy helper from GameData or dumps may be huge",
                     helperProtocol, kPhoenixMiniDumperProtocolVersion);
    Phoenix_WriteLifecycleLog(line);
  }
}

static BOOL Phoenix_TryWriteMiniDump(PhoenixMiniDumpWriteDumpFn pfnMiniDumpWriteDump,
                                     HANDLE hDumpFile,
                                     PEXCEPTION_POINTERS pExceptionPointers,
                                     MINIDUMP_TYPE dumpType,
                                     DWORD *pWriteError) {
  if (pfnMiniDumpWriteDump == NULL || hDumpFile == NULL ||
      hDumpFile == INVALID_HANDLE_VALUE) {
    if (pWriteError != NULL)
      *pWriteError = ERROR_INVALID_HANDLE;
    return FALSE;
  }

  MINIDUMP_EXCEPTION_INFORMATION ExpParam;
  ExpParam.ThreadId = GetCurrentThreadId();
  ExpParam.ExceptionPointers = pExceptionPointers;
  ExpParam.ClientPointers = FALSE;

  SetLastError(0);
  const BOOL ok = pfnMiniDumpWriteDump(
      GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, dumpType,
      (pExceptionPointers ? &ExpParam : NULL), NULL, NULL);
  if (pWriteError != NULL)
    *pWriteError = ok ? 0 : GetLastError();
  if (ok)
    FlushFileBuffers(hDumpFile);
  return ok;
}

static BOOL Phoenix_CreateDumpFileAt(const char *dumpPath, HANDLE *pOutHandle,
                                     DWORD *pCreateError) {
  if (pOutHandle == NULL)
    return FALSE;
  *pOutHandle = INVALID_HANDLE_VALUE;
  SetLastError(0);
  HANDLE hDumpFile = CreateFileA(dumpPath, GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_WRITE | FILE_SHARE_READ, 0,
                                 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
  if (pCreateError != NULL)
    *pCreateError = (hDumpFile == INVALID_HANDLE_VALUE) ? GetLastError() : 0;
  if (hDumpFile == INVALID_HANDLE_VALUE)
    return FALSE;
  *pOutHandle = hDumpFile;
  return TRUE;
}

static void Phoenix_PreloadDbgHelp() {
  if (g_PhoenixMiniDumpWriteDump != NULL)
    return;
  if (g_PhoenixDbgHelpModule == NULL)
    g_PhoenixDbgHelpModule = LoadLibraryA("DbgHelp.dll");
  if (g_PhoenixDbgHelpModule == NULL)
    return;
  g_PhoenixMiniDumpWriteDump = reinterpret_cast<PhoenixMiniDumpWriteDumpFn>(
      GetProcAddress(g_PhoenixDbgHelpModule, "MiniDumpWriteDump"));
}

static BOOL Phoenix_TryWriteMiniDumpOutOfProcess(const char *dumpPath,
                                                 DWORD *pWriteError) {
  if (dumpPath == NULL || dumpPath[0] == '\0') {
    if (pWriteError != NULL)
      *pWriteError = ERROR_INVALID_PARAMETER;
    return FALSE;
  }

  char moduleDir[MAX_PATH] = {0};
  Phoenix_GetClientDirectoryA(moduleDir, MAX_PATH);
  char helperPath[MAX_PATH] = {0};
  StringCchPrintfA(helperPath, MAX_PATH, "%sPhoenixMiniDumper.exe", moduleDir);
  if (GetFileAttributesA(helperPath) == INVALID_FILE_ATTRIBUTES) {
    if (pWriteError != NULL)
      *pWriteError = ERROR_FILE_NOT_FOUND;
    return FALSE;
  }

  // Inherit edilebilir gercek process handle — OpenProcess izin sorununu asar.
  HANDLE hInherit = NULL;
  DuplicateHandle(GetCurrentProcess(), GetCurrentProcess(), GetCurrentProcess(),
                  &hInherit, 0, TRUE, DUPLICATE_SAME_ACCESS);

  char cmdLine[MAX_PATH * 2 + 128] = {0};
  if (hInherit != NULL) {
    StringCchPrintfA(cmdLine, MAX_PATH * 2 + 128,
                     "\"%s\" %lu \"%s\" %lu --protocol %u", helperPath,
                     GetCurrentProcessId(), dumpPath,
                     static_cast<unsigned long>(
                         reinterpret_cast<ULONG_PTR>(hInherit)),
                     kPhoenixMiniDumperProtocolVersion);
  } else {
    StringCchPrintfA(cmdLine, MAX_PATH * 2 + 128, "\"%s\" %lu \"%s\" --protocol %u",
                     helperPath, GetCurrentProcessId(), dumpPath,
                     kPhoenixMiniDumperProtocolVersion);
  }

  STARTUPINFOA si{};
  si.cb = sizeof(si);
  PROCESS_INFORMATION pi{};
  SetLastError(0);
  const BOOL created =
      CreateProcessA(NULL, cmdLine, NULL, NULL, TRUE /* inherit handles */,
                     CREATE_NO_WINDOW, NULL,
                     moduleDir[0] != '\0' ? moduleDir : NULL, &si, &pi);
  if (!created) {
    if (pWriteError != NULL)
      *pWriteError = GetLastError();
    if (hInherit != NULL)
      CloseHandle(hInherit);
    return FALSE;
  }

  const DWORD wait = WaitForSingleObject(pi.hProcess, 60000);
  DWORD exitCode = 1;
  if (wait == WAIT_OBJECT_0)
    GetExitCodeProcess(pi.hProcess, &exitCode);
  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
  if (hInherit != NULL)
    CloseHandle(hInherit);

  WIN32_FILE_ATTRIBUTE_DATA fad{};
  const BOOL exists = GetFileAttributesExA(dumpPath, GetFileExInfoStandard, &fad);
  const ULONGLONG size =
      (static_cast<ULONGLONG>(fad.nFileSizeHigh) << 32) | fad.nFileSizeLow;
  if (wait != WAIT_OBJECT_0 || exitCode != 0 || !exists || size < 1024ULL) {
    if (pWriteError != NULL)
      *pWriteError = (wait != WAIT_OBJECT_0) ? ERROR_TIMEOUT : exitCode;
    if (exists && size == 0)
      DeleteFileA(dumpPath);
    return FALSE;
  }

  if (!Phoenix_WantFullMemoryDump() && size > Phoenix_GetMaxDumpBytes()) {
    char line[256] = {0};
    StringCchPrintfA(
        line, 256,
        "dump rejected oversized %lu MB (max %lu MB) — stale "
        "PhoenixMiniDumper.exe? fallback in-process mini",
        static_cast<unsigned long>(size / (1024UL * 1024UL)),
        static_cast<unsigned long>(Phoenix_GetMaxDumpBytes() / (1024UL * 1024UL)));
    Phoenix_AppendFatalLogLine(line);
    DeleteFileA(dumpPath);
    if (pWriteError != NULL)
      *pWriteError = ERROR_FILE_TOO_LARGE;
    return FALSE;
  }

  if (pWriteError != NULL)
    *pWriteError = 0;
  return TRUE;
}

static void Phoenix_LogCppExceptionDetails(PEXCEPTION_POINTERS pExceptionPointers) {
  if (pExceptionPointers == NULL || pExceptionPointers->ExceptionRecord == NULL)
    return;
  EXCEPTION_RECORD *rec = pExceptionPointers->ExceptionRecord;
  if (rec->ExceptionCode != 0xE06D7363UL || rec->NumberParameters < 3)
    return;

  char line[512] = {0};
  StringCchPrintfA(
      line, 512,
      "cxx exception params=%lu magic=0x%p object=%p throwInfo=%p",
      rec->NumberParameters,
      reinterpret_cast<void *>(rec->ExceptionInformation[0]),
      reinterpret_cast<void *>(rec->ExceptionInformation[1]),
      reinterpret_cast<void *>(rec->ExceptionInformation[2]));
  Phoenix_AppendFatalLogLine(line);

  // MSVC ThrowInfo -> CatchableType -> TypeDescriptor name (best-effort).
  __try {
    const ULONG_PTR throwInfo = rec->ExceptionInformation[2];
    if (throwInfo == 0)
      return;
    // ThrowInfo::pCatchableTypeArray at offset 0x0C (x86)
    const DWORD *pCatchableTypes =
        *reinterpret_cast<DWORD **>(throwInfo + 0x0C);
    if (pCatchableTypes == NULL || pCatchableTypes[0] == 0)
      return;
    const DWORD catchableTypeRva = pCatchableTypes[1];
    if (catchableTypeRva == 0)
      return;
    const BYTE *catchableType =
        reinterpret_cast<const BYTE *>(catchableTypeRva);
    // CatchableType::pType at offset 0x04 (x86)
    const DWORD typeDescRva = *reinterpret_cast<const DWORD *>(catchableType + 4);
    if (typeDescRva == 0)
      return;
    const char *typeName =
        reinterpret_cast<const char *>(typeDescRva + 8); // TypeDescriptor::name
    if (typeName != NULL && typeName[0] != '\0') {
      char typeLine[384] = {0};
      StringCchPrintfA(typeLine, 384, "cxx exception type=%s", typeName);
      Phoenix_AppendFatalLogLine(typeLine);
    }
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    Phoenix_AppendFatalLogLine("cxx exception type decode failed");
  }
}

int GenerateMiniDump(HANDLE hFile, PEXCEPTION_POINTERS pExceptionPointers) {
  // Re-entrant koruma (VEH + UEF / WinMain).
  if (InterlockedCompareExchange(&g_PhoenixFatalHandlerEntered, 1, 0) != 0)
    return EXCEPTION_CONTINUE_SEARCH;

  BOOL bOwnDumpFile = FALSE;
  BOOL bDumpWritten = FALSE;
  HANDLE hDumpFile = hFile;
  char dumpPath[MAX_PATH] = {0};
  DWORD dumpCreateError = 0;
  DWORD dumpWriteError = 0;

  Phoenix_LogCppExceptionDetails(pExceptionPointers);
  Phoenix_PreloadDbgHelp();
  PhoenixMiniDumpWriteDumpFn pfnMiniDumpWriteDump = g_PhoenixMiniDumpWriteDump;
  HMODULE hDbgHelpLocal = NULL;
  if (pfnMiniDumpWriteDump == NULL) {
    hDbgHelpLocal = LoadLibraryA("DbgHelp.dll");
    if (hDbgHelpLocal)
      pfnMiniDumpWriteDump = reinterpret_cast<PhoenixMiniDumpWriteDumpFn>(
          GetProcAddress(hDbgHelpLocal, "MiniDumpWriteDump"));
  }

  if (hDumpFile == NULL || hDumpFile == INVALID_HANDLE_VALUE) {
    SYSTEMTIME stLocalTime;
    GetLocalTime(&stLocalTime);

    char dumpDir[MAX_PATH] = {0};
    Phoenix_EnsureDumpDirectoryA(dumpDir, MAX_PATH);
    StringCchPrintfA(dumpPath, MAX_PATH,
                     "%s\\v2.0-%04d%02d%02d-%02d%02d%02d-%03d-p%lu.dmp", dumpDir,
                     stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
                     stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
                     stLocalTime.wMilliseconds, GetCurrentProcessId());

    // 1) Out-of-process dumper — in-process 0 bayt / heap deadlock fix.
    bDumpWritten =
        Phoenix_TryWriteMiniDumpOutOfProcess(dumpPath, &dumpWriteError);
    if (bDumpWritten) {
      Phoenix_AppendFatalLogLine("dump via PhoenixMiniDumper.exe OK");
    } else {
      // 2) Fallback: in-process (eski yol; basarisiz olabilir).
      if (!Phoenix_CreateDumpFileAt(dumpPath, &hDumpFile, &dumpCreateError)) {
        char tempDir[MAX_PATH] = {0};
        GetTempPathA(MAX_PATH, tempDir);
        StringCchPrintfA(dumpPath, MAX_PATH,
                         "%sClient-v2.0-%04d%02d%02d-%02d%02d%02d-%03d-p%lu.dmp",
                         tempDir, stLocalTime.wYear, stLocalTime.wMonth,
                         stLocalTime.wDay, stLocalTime.wHour, stLocalTime.wMinute,
                         stLocalTime.wSecond, stLocalTime.wMilliseconds,
                         GetCurrentProcessId());
        Phoenix_CreateDumpFileAt(dumpPath, &hDumpFile, &dumpCreateError);
      }
      bOwnDumpFile = TRUE;
    }
  }

  if (!bDumpWritten && pfnMiniDumpWriteDump &&
      hDumpFile != INVALID_HANDLE_VALUE && hDumpFile != NULL) {
    const MINIDUMP_TYPE dumpTypes[] = {
        Phoenix_GetPreferredMiniDumpType(),
        MiniDumpNormal,
        MiniDumpWithDataSegs,
    };
    for (size_t i = 0; i < sizeof(dumpTypes) / sizeof(dumpTypes[0]); ++i) {
      if (i > 0) {
        SetFilePointer(hDumpFile, 0, NULL, FILE_BEGIN);
        SetEndOfFile(hDumpFile);
      }
      bDumpWritten = Phoenix_TryWriteMiniDump(
          pfnMiniDumpWriteDump, hDumpFile, pExceptionPointers, dumpTypes[i],
          &dumpWriteError);
      if (bDumpWritten)
        break;
    }
  } else if (!bDumpWritten && pfnMiniDumpWriteDump == NULL) {
    dumpWriteError = ERROR_PROC_NOT_FOUND;
  }

  if (bOwnDumpFile && hDumpFile != NULL && hDumpFile != INVALID_HANDLE_VALUE) {
    // 0 bayt dosyayi birakma.
    if (!bDumpWritten) {
      CloseHandle(hDumpFile);
      DeleteFileA(dumpPath);
      hDumpFile = INVALID_HANDLE_VALUE;
    } else {
      CloseHandle(hDumpFile);
    }
  }

  if (hDbgHelpLocal != NULL)
    FreeLibrary(hDbgHelpLocal);

  Phoenix_WriteFatalExceptionLog(pExceptionPointers, dumpPath, bDumpWritten,
                                 dumpCreateError, dumpWriteError);

  // Basarisizsa WER LocalDumps'a sans ver (WinMain __except yutmasin).
  if (!bDumpWritten) {
    InterlockedExchange(&g_PhoenixFatalHandlerEntered, 0);
    Phoenix_AppendFatalLogLine(
        "dump FAILED — returning CONTINUE_SEARCH for WER LocalDumps");
    return EXCEPTION_CONTINUE_SEARCH;
  }
  return EXCEPTION_EXECUTE_HANDLER;
}

static LPTOP_LEVEL_EXCEPTION_FILTER WINAPI
Phoenix_HookedSetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER filter) {
  // CRT/DX/dgVoodoo filter'imizi NULL/başka filter ile silmesin.
  (void)filter;
  LPTOP_LEVEL_EXCEPTION_FILTER(WINAPI * real)(LPTOP_LEVEL_EXCEPTION_FILTER) =
      g_RealSetUnhandledExceptionFilter;
  if (real == NULL) {
    HMODULE k32 = GetModuleHandleA("kernel32.dll");
    if (k32 != NULL) {
      real = reinterpret_cast<LPTOP_LEVEL_EXCEPTION_FILTER(WINAPI *)(
          LPTOP_LEVEL_EXCEPTION_FILTER)>(
          GetProcAddress(k32, "SetUnhandledExceptionFilter"));
    }
  }
  if (real != NULL)
    return real(Phoenix_UnhandledExceptionFilter);
  return Phoenix_UnhandledExceptionFilter;
}

static bool Phoenix_PatchSetUnhandledExceptionFilterIAT() {
  HMODULE hModule = GetModuleHandleA(NULL);
  if (hModule == NULL)
    return false;

  // Gerçek API'yi IAT patch öncesi al — hook içinden recursive Set* çağırma.
  if (g_RealSetUnhandledExceptionFilter == NULL) {
    HMODULE k32 = GetModuleHandleA("kernel32.dll");
    if (k32 != NULL) {
      g_RealSetUnhandledExceptionFilter =
          reinterpret_cast<LPTOP_LEVEL_EXCEPTION_FILTER(WINAPI *)(
              LPTOP_LEVEL_EXCEPTION_FILTER)>(
              GetProcAddress(k32, "SetUnhandledExceptionFilter"));
    }
  }

  auto *dos = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);
  if (dos->e_magic != IMAGE_DOS_SIGNATURE)
    return false;
  auto *nt = reinterpret_cast<PIMAGE_NT_HEADERS>(
      reinterpret_cast<BYTE *>(hModule) + dos->e_lfanew);
  if (nt->Signature != IMAGE_NT_SIGNATURE)
    return false;

  IMAGE_DATA_DIRECTORY importDir =
      nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
  if (importDir.VirtualAddress == 0 || importDir.Size == 0)
    return false;

  auto *importDesc = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(
      reinterpret_cast<BYTE *>(hModule) + importDir.VirtualAddress);
  for (; importDesc->Name != 0; ++importDesc) {
    const char *dllName = reinterpret_cast<const char *>(
        reinterpret_cast<BYTE *>(hModule) + importDesc->Name);
    if (_stricmp(dllName, "KERNEL32.dll") != 0 &&
        _stricmp(dllName, "api-ms-win-core-errorhandling-l1-1-0.dll") != 0)
      continue;

    auto *thunk = reinterpret_cast<PIMAGE_THUNK_DATA>(
        reinterpret_cast<BYTE *>(hModule) + importDesc->FirstThunk);
    auto *origThunk = reinterpret_cast<PIMAGE_THUNK_DATA>(
        reinterpret_cast<BYTE *>(hModule) +
        (importDesc->OriginalFirstThunk ? importDesc->OriginalFirstThunk
                                        : importDesc->FirstThunk));
    for (; origThunk->u1.AddressOfData != 0; ++thunk, ++origThunk) {
      if (IMAGE_SNAP_BY_ORDINAL(origThunk->u1.Ordinal))
        continue;
      auto *importByName = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(
          reinterpret_cast<BYTE *>(hModule) + origThunk->u1.AddressOfData);
      if (strcmp(reinterpret_cast<const char *>(importByName->Name),
                 "SetUnhandledExceptionFilter") != 0)
        continue;

      DWORD oldProtect = 0;
      if (!VirtualProtect(&thunk->u1.Function, sizeof(ULONG_PTR), PAGE_READWRITE,
                          &oldProtect))
        return false;

      thunk->u1.Function = reinterpret_cast<ULONG_PTR>(
          &Phoenix_HookedSetUnhandledExceptionFilter);
      VirtualProtect(&thunk->u1.Function, sizeof(ULONG_PTR), oldProtect,
                     &oldProtect);
      return true;
    }
  }
  return false;
}

static void Phoenix_RegisterWerLocalDumps() {
  char dumpDir[MAX_PATH] = {0};
  Phoenix_EnsureDumpDirectoryA(dumpDir, MAX_PATH);

  HKEY hKey = NULL;
  if (RegCreateKeyExA(HKEY_CURRENT_USER,
                      "SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\"
                      "LocalDumps\\Client.exe",
                      0, NULL, 0, KEY_SET_VALUE | KEY_QUERY_VALUE, NULL, &hKey,
                      NULL) != ERROR_SUCCESS)
    return;

  const DWORD dumpCount = 20;
  const DWORD dumpType = 1; // Mini dump — Full (2) GB boyut uretir
  RegSetValueExA(hKey, "DumpFolder", 0, REG_SZ,
                 reinterpret_cast<const BYTE *>(dumpDir),
                 static_cast<DWORD>(strlen(dumpDir) + 1));
  RegSetValueExA(hKey, "DumpCount", 0, REG_DWORD,
                 reinterpret_cast<const BYTE *>(&dumpCount), sizeof(dumpCount));
  RegSetValueExA(hKey, "DumpType", 0, REG_DWORD,
                 reinterpret_cast<const BYTE *>(&dumpType), sizeof(dumpType));
  RegCloseKey(hKey);

  // Global LocalDumps default (exe alt anahtari yoksa kullanilir).
  HKEY hGlobal = NULL;
  if (RegCreateKeyExA(HKEY_CURRENT_USER,
                      "SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\"
                      "LocalDumps",
                      0, NULL, 0, KEY_SET_VALUE, NULL, &hGlobal,
                      NULL) == ERROR_SUCCESS) {
    RegSetValueExA(hGlobal, "DumpFolder", 0, REG_SZ,
                   reinterpret_cast<const BYTE *>(dumpDir),
                   static_cast<DWORD>(strlen(dumpDir) + 1));
    RegSetValueExA(hGlobal, "DumpCount", 0, REG_DWORD,
                   reinterpret_cast<const BYTE *>(&dumpCount),
                   sizeof(dumpCount));
    RegSetValueExA(hGlobal, "DumpType", 0, REG_DWORD,
                   reinterpret_cast<const BYTE *>(&dumpType), sizeof(dumpType));
    RegCloseKey(hGlobal);
  }
}

static LONG CALLBACK
Phoenix_VectoredContinueHandler(PEXCEPTION_POINTERS pExceptionPointers) {
  // Frame handler'lar yutmazsa UEF oncesi yedek dump.
  if (pExceptionPointers != NULL &&
      pExceptionPointers->ExceptionRecord != NULL) {
    const DWORD code = pExceptionPointers->ExceptionRecord->ExceptionCode;
    // Breakpoint / single-step dump uretmesin.
    if (code == EXCEPTION_BREAKPOINT || code == EXCEPTION_SINGLE_STEP)
      return EXCEPTION_CONTINUE_SEARCH;
    // 2026-09-02 dump slot fix: 0x406D1388 = MS "SetThreadName" konvansiyonu
    // (thread isimlendirme, kasitli raise + her zaman continue). Her acilista
    // VCH'ye dusuyor, gereksiz dump uretiyordu VE one-shot
    // g_PhoenixFatalHandlerEntered slotunu tuketiyordu — sonraki GERCEK
    // crash dump'siz kaliyordu (22:36 raporunda yasandi).
    if (code == 0x406D1388)
      return EXCEPTION_CONTINUE_SEARCH;
  }
  Phoenix_WriteLifecycleLog("fatal vectored continue handler");
  GenerateMiniDump(NULL, pExceptionPointers);
  return EXCEPTION_CONTINUE_SEARCH;
}

static LONG WINAPI Phoenix_UnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionPointers) {
  Phoenix_WriteLifecycleLog("fatal unhandled exception filter");
  const int action = GenerateMiniDump(NULL, pExceptionPointers);
  if (action == EXCEPTION_EXECUTE_HANDLER) {
    ExitProcess(1);
  }
  // Dump yok — WER LocalDumps yazabilsin.
  return EXCEPTION_CONTINUE_SEARCH;
}

static void Phoenix_FailFastAfterDump(const char *reason,
                                      PEXCEPTION_POINTERS pExceptionPointers) {
  Phoenix_WriteLifecycleLog(reason);
  GenerateMiniDump(NULL, pExceptionPointers);
  ExitProcess(1);
}

static void Phoenix_PurecallHandler() {
  Phoenix_FailFastAfterDump("fatal pure virtual function call", NULL);
}

static void Phoenix_InvalidParameterHandler(const wchar_t *expression,
                                            const wchar_t *function,
                                            const wchar_t *file,
                                            unsigned int line,
                                            uintptr_t reserved) {
  (void)expression;
  (void)function;
  (void)file;
  (void)line;
  (void)reserved;
  Phoenix_FailFastAfterDump("fatal invalid parameter", NULL);
}

static void Phoenix_TerminateHandler() {
  Phoenix_FailFastAfterDump("fatal std::terminate", NULL);
}

static void Phoenix_InstallFatalHandlers() {
  Phoenix_PreloadDbgHelp();
  {
    char dumpDir[MAX_PATH] = {0};
    Phoenix_EnsureDumpDirectoryA(dumpDir, MAX_PATH);
  }

  if (g_RealSetUnhandledExceptionFilter == NULL) {
    HMODULE k32 = GetModuleHandleA("kernel32.dll");
    if (k32 != NULL) {
      g_RealSetUnhandledExceptionFilter =
          reinterpret_cast<LPTOP_LEVEL_EXCEPTION_FILTER(WINAPI *)(
              LPTOP_LEVEL_EXCEPTION_FILTER)>(
              GetProcAddress(k32, "SetUnhandledExceptionFilter"));
    }
  }

  if (g_RealSetUnhandledExceptionFilter != NULL)
    g_RealSetUnhandledExceptionFilter(Phoenix_UnhandledExceptionFilter);
  else
    SetUnhandledExceptionFilter(Phoenix_UnhandledExceptionFilter);

  Phoenix_PatchSetUnhandledExceptionFilterIAT();

  if (g_RealSetUnhandledExceptionFilter != NULL)
    g_RealSetUnhandledExceptionFilter(Phoenix_UnhandledExceptionFilter);
  else
    SetUnhandledExceptionFilter(Phoenix_UnhandledExceptionFilter);

  if (g_PhoenixVectoredContinueHandle == NULL) {
    g_PhoenixVectoredContinueHandle =
        AddVectoredContinueHandler(1, Phoenix_VectoredContinueHandler);
  }

  Phoenix_RegisterWerLocalDumps();
  Phoenix_WarnIfMiniDumperStale();
  _set_purecall_handler(Phoenix_PurecallHandler);
  _set_invalid_parameter_handler(Phoenix_InvalidParameterHandler);
  std::set_terminate(Phoenix_TerminateHandler);
  _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);

  Phoenix_WriteLifecycleLog("fatal handlers installed");
}

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Initialization, message loop
//-----------------------------------------------------------------------------
#if 1

int PASCAL
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow)
{
  HANDLE hFile = 0;
  Phoenix_InstallFatalHandlers();

  __try
  {
    Phoenix_WriteLifecycleLog("GameStart enter");
    int a = GameStart(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    (void)a;
    Phoenix_WriteLifecycleLog("GameStart returned normally");
  }
  __except(GenerateMiniDump(hFile, GetExceptionInformation()))
  {
    ExitProcess(1);
  }

  return 0;
}
#endif // #if 1 (WinMain block)
