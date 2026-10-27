#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <winhttp.h>
#include <bcrypt.h>
#include <shellapi.h>
#include <commctrl.h>
#include <gdiplus.h>
#include <objidl.h>

#include <algorithm>
#include <atomic>
#include <cctype>
#include <cwctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <mutex>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "PhoenixProtectedFile.h"

namespace fs = std::filesystem;
namespace gdip = Gdiplus;

static constexpr int kPhoenixUpdaterBackgroundResourceId = 101;
static constexpr int kPhoenixUpdaterCloseResourceId = 102;
static constexpr int kPhoenixUpdaterIconResourceId = 1;
static constexpr int kResolutionComboId = 2101;
static constexpr int kWindowedRadioId = 2102;
static constexpr int kFullScreenRadioId = 2103;

struct ManifestFile {
  std::string path;
  std::string url;
  std::string sha256;
  uint64_t size = 0;
  bool executable = false;
};

struct Manifest {
  uint32_t version = 0;
  uint32_t minClientVersion = 0;
  std::string baseUrl;
  std::vector<ManifestFile> files;
  std::vector<std::string> deletes;
  ManifestFile selfUpdate;
  bool hasSelfUpdate = false;
};

struct UpdaterOptions {
  bool requireSignature = true;
  bool launchClient = true;
  bool showGui = true;
};

struct RectDef {
  int x = 0;
  int y = 0;
  int w = 0;
  int h = 0;
};

struct ResolutionConfig {
  int x = 1280;
  int y = 960;
  int fullScreen = 0;
};

struct ResolutionOption {
  int x;
  int y;
  const wchar_t* label;
};

struct UpdaterGuiLayout {
  int windowWidth = 510;
  int windowHeight = 330;
  std::wstring backgroundImage = L"data\\updater\\phoenix_updater_bg.png";
  RectDef dragRect{0, 0, 510, 48};
  RectDef statusRect{130, 260, 276, 20};
  RectDef progressRect{129, 257, 278, 28};
  RectDef playButtonRect{183, 293, 145, 24};
  RectDef closeButtonRect{490, 8, 14, 14};
  ResolutionConfig defaultResolution;
  COLORREF progressFill = RGB(192, 82, 22);
  COLORREF progressBack = RGB(7, 7, 5);
  COLORREF statusText = RGB(230, 180, 96);
  COLORREF playText = RGB(255, 149, 32);
};

static const unsigned char kManifestPublicKeyX[32] = {
  0xf7, 0xa8, 0x25, 0xa7, 0x21, 0xde, 0xb4, 0xf3,
  0x0e, 0xbb, 0x66, 0xcc, 0xe5, 0x4b, 0x80, 0xcb,
  0x60, 0x16, 0x42, 0xd6, 0x86, 0x42, 0x77, 0x69,
  0x6a, 0x57, 0xf0, 0x1e, 0xbc, 0x21, 0x08, 0xae
};

static const unsigned char kManifestPublicKeyY[32] = {
  0xe4, 0x32, 0x8d, 0x8b, 0xf6, 0x9e, 0xed, 0xa4,
  0x82, 0xd7, 0x7e, 0xe6, 0x70, 0x56, 0x2f, 0x56,
  0xac, 0x64, 0x86, 0xfc, 0xac, 0x8b, 0x8f, 0x4b,
  0x25, 0x9b, 0x56, 0x4b, 0x4c, 0x7a, 0x24, 0xb2
};

static HWND g_hwndMain = NULL;
static std::atomic<int> g_updateExitCode{0};
static UpdaterGuiLayout g_guiLayout;
static std::unique_ptr<gdip::Image> g_guiBackground;
static std::unique_ptr<gdip::Image> g_guiCloseButton;
static ULONG_PTR g_gdiplusToken = 0;
static std::wstring g_guiStatusText = L"Preparing update...";
static int g_guiProgressCurrent = 0;
static int g_guiProgressTotal = 100;
static bool g_guiCanPlay = false;
static bool g_guiPlayDown = false;
static bool g_guiCloseDown = false;
static fs::path g_guiRoot;
static ResolutionConfig g_guiResolutionConfig;
static HWND g_hwndResolutionCombo = NULL;
static HWND g_hwndWindowedRadio = NULL;
static HWND g_hwndFullScreenRadio = NULL;
static HBRUSH g_guiControlBrush = NULL;
static std::mutex g_logMutex;
static std::unique_ptr<gdip::Bitmap> g_guiBackgroundCache;
static bool g_guiProgressRepaintTimerActive = false;

static constexpr UINT WM_PHOENIX_LOG = WM_APP + 101;
static constexpr UINT WM_PHOENIX_STATUS = WM_APP + 102;
static constexpr UINT WM_PHOENIX_PROGRESS = WM_APP + 103;
static constexpr UINT WM_PHOENIX_DONE = WM_APP + 104;
static constexpr UINT_PTR kGuiProgressRepaintTimerId = 2;
static constexpr UINT kGuiProgressRepaintIntervalMs = 50;

static const ResolutionOption kResolutionOptions[] = {
  {1280, 960, L"1280 x 960"},
  {1280, 1024, L"1280 x 1024"},
  {1440, 900, L"1440 x 900"},
  {1920, 1080, L"1920 x 1080"},
};

static void PostGuiText(UINT message, const std::wstring& text)
{
  if (g_hwndMain == NULL) return;
  PostMessageW(g_hwndMain, message, 0, reinterpret_cast<LPARAM>(new std::wstring(text)));
}

static void PostGuiProgress(int current, int total)
{
  if (g_hwndMain == NULL) return;
  PostMessageW(g_hwndMain, WM_PHOENIX_PROGRESS, static_cast<WPARAM>(current), static_cast<LPARAM>(total));
}

static std::string WideToUtf8(const std::wstring& text);

static fs::path GetUpdaterLogPath()
{
  wchar_t path[MAX_PATH] = {0};
  GetModuleFileNameW(NULL, path, _countof(path));
  if (path[0] == L'\0') return fs::path(L"data\\log\\updater.log");
  return fs::path(path).parent_path() / L"data\\log\\updater.log";
}

static void DeleteUpdaterLogFile()
{
  std::lock_guard<std::mutex> lock(g_logMutex);

  std::error_code ec;
  const fs::path logPath = GetUpdaterLogPath();
  fs::remove(logPath, ec);
  ec.clear();
  fs::remove(logPath.parent_path(), ec);
}

struct UpdaterLogCleanupGuard {
  ~UpdaterLogCleanupGuard()
  {
    DeleteUpdaterLogFile();
  }
};

static void Log(const wchar_t* fmt, ...)
{
  wchar_t buffer[2048] = {0};
  va_list args;
  va_start(args, fmt);
  _vsnwprintf_s(buffer, _TRUNCATE, fmt, args);
  va_end(args);

  std::lock_guard<std::mutex> lock(g_logMutex);
  OutputDebugStringW(buffer);
  OutputDebugStringW(L"\n");

  const fs::path logPath = GetUpdaterLogPath();
  std::error_code ec;
  fs::create_directories(logPath.parent_path(), ec);
  if (!ec) {
    std::ofstream out(logPath, std::ios::binary | std::ios::app);
    if (out.is_open()) {
      out << WideToUtf8(buffer) << "\r\n";
    }
  }
}

static std::wstring RedactUrlForLog(const std::wstring& url)
{
  URL_COMPONENTS parts;
  ZeroMemory(&parts, sizeof(parts));
  parts.dwStructSize = sizeof(parts);

  wchar_t scheme[16] = {0};
  wchar_t path[4096] = {0};
  parts.lpszScheme = scheme;
  parts.dwSchemeLength = _countof(scheme);
  parts.dwHostNameLength = 0;
  parts.lpszUrlPath = path;
  parts.dwUrlPathLength = _countof(path);

  if (!WinHttpCrackUrl(url.c_str(), 0, 0, &parts)) {
    return L"<redacted-url>";
  }

  std::wstring redacted;
  redacted.assign(parts.lpszScheme, parts.dwSchemeLength);
  redacted += L"://<redacted-host>";
  redacted.append(parts.lpszUrlPath, parts.dwUrlPathLength);
  return redacted;
}

static std::unique_ptr<gdip::Image> CloneImageToBitmap(gdip::Image* source)
{
  if (source == NULL || source->GetLastStatus() != gdip::Ok) return nullptr;

  const UINT width = source->GetWidth();
  const UINT height = source->GetHeight();
  if (width == 0 || height == 0) return nullptr;

  std::unique_ptr<gdip::Bitmap> bitmap(new gdip::Bitmap(width, height, PixelFormat32bppARGB));
  if (!bitmap || bitmap->GetLastStatus() != gdip::Ok) return nullptr;

  gdip::Graphics graphics(bitmap.get());
  graphics.SetCompositingMode(gdip::CompositingModeSourceCopy);
  if (graphics.DrawImage(source, 0, 0, static_cast<INT>(width), static_cast<INT>(height)) != gdip::Ok) {
    return nullptr;
  }

  return std::unique_ptr<gdip::Image>(bitmap.release());
}

static std::unique_ptr<gdip::Image> LoadImageFromResource(HINSTANCE instance, int resourceId)
{
  HRSRC resource = FindResourceW(instance, MAKEINTRESOURCEW(resourceId), RT_RCDATA);
  if (resource == NULL) return nullptr;

  const DWORD size = SizeofResource(instance, resource);
  if (size == 0) return nullptr;

  HGLOBAL loaded = LoadResource(instance, resource);
  if (loaded == NULL) return nullptr;

  const void* data = LockResource(loaded);
  if (data == NULL) return nullptr;

  HGLOBAL copy = GlobalAlloc(GMEM_MOVEABLE, size);
  if (copy == NULL) return nullptr;

  void* writable = GlobalLock(copy);
  if (writable == NULL) {
    GlobalFree(copy);
    return nullptr;
  }
  std::memcpy(writable, data, size);
  GlobalUnlock(copy);

  IStream* stream = NULL;
  if (FAILED(CreateStreamOnHGlobal(copy, TRUE, &stream)) || stream == NULL) {
    GlobalFree(copy);
    return nullptr;
  }

  std::unique_ptr<gdip::Image> loadedImage(gdip::Image::FromStream(stream, FALSE));
  std::unique_ptr<gdip::Image> clonedImage = CloneImageToBitmap(loadedImage.get());
  stream->Release();
  return clonedImage;
}

static std::wstring Utf8ToWide(const std::string& text)
{
  if (text.empty()) return std::wstring();
  int count = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, NULL, 0);
  if (count <= 0) {
    count = MultiByteToWideChar(CP_ACP, 0, text.c_str(), -1, NULL, 0);
    if (count <= 0) return std::wstring();
    std::wstring out(static_cast<size_t>(count - 1), L'\0');
    MultiByteToWideChar(CP_ACP, 0, text.c_str(), -1, out.data(), count);
    return out;
  }
  std::wstring out(static_cast<size_t>(count - 1), L'\0');
  MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, out.data(), count);
  return out;
}

static std::string WideToUtf8(const std::wstring& text)
{
  if (text.empty()) return std::string();
  int count = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, NULL, 0, NULL, NULL);
  if (count <= 0) return std::string();
  std::string out(static_cast<size_t>(count - 1), '\0');
  WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, out.data(), count, NULL, NULL);
  return out;
}

static uint64_t CurrentUnixTimeSeconds()
{
  FILETIME fileTime;
  GetSystemTimeAsFileTime(&fileTime);

  ULARGE_INTEGER ticks;
  ticks.LowPart = fileTime.dwLowDateTime;
  ticks.HighPart = fileTime.dwHighDateTime;

  static const uint64_t kUnixEpoch = 116444736000000000ULL;
  return (ticks.QuadPart - kUnixEpoch) / 10000000ULL;
}

static std::wstring QuoteProcessArg(const std::wstring& arg)
{
  if (arg.empty()) return L"\"\"";

  bool needsQuote = false;
  for (wchar_t ch : arg) {
    if (std::iswspace(ch) || ch == L'"') {
      needsQuote = true;
      break;
    }
  }
  if (!needsQuote) return arg;

  std::wstring result = L"\"";
  size_t slashCount = 0;
  for (wchar_t ch : arg) {
    if (ch == L'\\') {
      ++slashCount;
      continue;
    }
    if (ch == L'"') {
      result.append(slashCount * 2 + 1, L'\\');
      result.push_back(ch);
      slashCount = 0;
      continue;
    }
    result.append(slashCount, L'\\');
    slashCount = 0;
    result.push_back(ch);
  }
  result.append(slashCount * 2, L'\\');
  result.push_back(L'"');
  return result;
}

static bool GenerateLaunchTicketId(std::wstring& ticketId)
{
  unsigned char bytes[16] = {0};
  const NTSTATUS status = BCryptGenRandom(NULL, bytes, sizeof(bytes), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
  if (status < 0) {
    Log(L"Updater: BCryptGenRandom failed for launch ticket: 0x%08lx", static_cast<unsigned long>(status));
    return false;
  }

  static const wchar_t kHex[] = L"0123456789abcdef";
  ticketId.clear();
  ticketId.reserve(sizeof(bytes) * 2);
  for (unsigned char byte : bytes) {
    ticketId.push_back(kHex[(byte >> 4) & 0x0f]);
    ticketId.push_back(kHex[byte & 0x0f]);
  }
  return true;
}

static bool GetLaunchTicketDirectory(fs::path& ticketDir)
{
  const DWORD required = GetEnvironmentVariableW(L"LOCALAPPDATA", NULL, 0);
  if (required == 0) {
    Log(L"Updater: LOCALAPPDATA is unavailable for launch ticket");
    return false;
  }

  std::vector<wchar_t> buffer(required);
  const DWORD written = GetEnvironmentVariableW(L"LOCALAPPDATA", buffer.data(), required);
  if (written == 0 || written >= required) {
    Log(L"Updater: failed to read LOCALAPPDATA for launch ticket");
    return false;
  }

  ticketDir = fs::path(buffer.data()) / L"ProjectPhoenix" / L"launch";
  std::error_code ec;
  fs::create_directories(ticketDir, ec);
  if (ec) {
    Log(L"Updater: failed to create launch ticket directory: %s error=%S",
        ticketDir.c_str(), ec.message().c_str());
    return false;
  }
  return true;
}

static bool CreateClientLaunchTicket(const fs::path& root, std::wstring& ticketId)
{
  if (!GenerateLaunchTicketId(ticketId)) {
    return false;
  }

  fs::path ticketDir;
  if (!GetLaunchTicketDirectory(ticketDir)) {
    return false;
  }

  const fs::path ticketPath = ticketDir / (ticketId + L".ticket");
  std::ofstream out(ticketPath, std::ios::binary | std::ios::trunc);
  if (!out.is_open()) {
    Log(L"Updater: failed to open launch ticket: %s", ticketPath.c_str());
    return false;
  }

  out << "source=updater\n";
  out << "root=" << WideToUtf8(root.lexically_normal().wstring()) << "\n";
  out << "created_utc=" << CurrentUnixTimeSeconds() << "\n";
  out << "ticket=" << WideToUtf8(ticketId) << "\n";
  if (!out.good()) {
    Log(L"Updater: failed to write launch ticket: %s", ticketPath.c_str());
    return false;
  }

  Log(L"Updater: launch ticket created: %s", ticketId.c_str());
  return true;
}

static bool ReadPlainTextFile(const fs::path& path, std::string& text)
{
  text.clear();
  std::ifstream in(path, std::ios::binary);
  if (!in.is_open()) return false;
  std::ostringstream buffer;
  buffer << in.rdbuf();
  text = buffer.str();
  return in.good() || in.eof();
}

static fs::path BuildProtectedSiblingPath(const fs::path& path)
{
  fs::path protectedPath = path;
  protectedPath += Utf8ToWide(PhoenixProtectedFile::Extension());
  return protectedPath;
}

static void AddUniqueOriginalPathCandidate(std::vector<std::string>& candidates,
                                           const fs::path& candidate)
{
  const std::string text = WideToUtf8(candidate.wstring());
  if (text.empty()) return;
  if (std::find(candidates.begin(), candidates.end(), text) == candidates.end()) {
    candidates.push_back(text);
  }
}

static bool IsUsableRelativeCandidate(const fs::path& relative)
{
  if (relative.empty() || relative.is_absolute()) return false;
  for (const fs::path& part : relative) {
    if (part == L"..") return false;
  }
  return true;
}

static std::vector<std::string> BuildProtectedOriginalPathCandidates(const fs::path& root,
                                                                     const fs::path& path)
{
  std::vector<std::string> candidates;
  const fs::path normalizedRoot = root.lexically_normal();
  const fs::path normalizedPath = path.lexically_normal();

  AddUniqueOriginalPathCandidate(candidates, normalizedPath);

  const fs::path relative = normalizedPath.lexically_relative(normalizedRoot);
  if (IsUsableRelativeCandidate(relative)) {
    AddUniqueOriginalPathCandidate(candidates, relative);
  }

  // Backward compatibility for PHX files generated before the patch tool's
  // GameData-relative root was selected correctly.
  AddUniqueOriginalPathCandidate(candidates, normalizedPath.filename());
  return candidates;
}

static bool ReadTextFileWithProtectedFallback(const fs::path& root,
                                              const fs::path& path,
                                              std::string& text)
{
  text.clear();

  const fs::path protectedPath = BuildProtectedSiblingPath(path);
  if (fs::exists(protectedPath)) {
    const std::string protectedPathText = WideToUtf8(protectedPath.wstring());
    const std::vector<std::string> originalCandidates =
        BuildProtectedOriginalPathCandidates(root, path);
    for (const std::string& originalPathText : originalCandidates) {
      std::vector<char> data;
      if (PhoenixProtectedFile::LoadWithOriginalPath(protectedPathText.c_str(),
                                                     originalPathText.c_str(),
                                                     data)) {
        text.assign(data.begin(), data.end());
        Log(L"Updater: protected file loaded %s logical=%s",
            protectedPath.c_str(), Utf8ToWide(originalPathText).c_str());
        return true;
      }
    }
    Log(L"Updater: protected file failed for all path identities, trying plaintext %s",
        protectedPath.c_str());
  }

  if (ReadPlainTextFile(path, text)) {
    if (fs::exists(protectedPath)) {
      Log(L"Updater: plaintext fallback loaded %s", path.c_str());
    }
    return true;
  }

  if (fs::exists(protectedPath)) {
    Log(L"Updater: plaintext fallback missing or unreadable %s", path.c_str());
  }
  return false;
}

static std::string Trim(const std::string& text)
{
  size_t begin = 0;
  while (begin < text.size() && std::isspace(static_cast<unsigned char>(text[begin]))) ++begin;
  size_t end = text.size();
  while (end > begin && std::isspace(static_cast<unsigned char>(text[end - 1]))) --end;
  return text.substr(begin, end - begin);
}

static std::string ToLowerAscii(std::string text)
{
  std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });
  return text;
}

static std::string JsonUnescape(const std::string& text)
{
  std::string out;
  out.reserve(text.size());
  for (size_t i = 0; i < text.size(); ++i) {
    if (text[i] != '\\' || i + 1 >= text.size()) {
      out.push_back(text[i]);
      continue;
    }
    const char c = text[++i];
    switch (c) {
      case '"': out.push_back('"'); break;
      case '\\': out.push_back('\\'); break;
      case '/': out.push_back('/'); break;
      case 'b': out.push_back('\b'); break;
      case 'f': out.push_back('\f'); break;
      case 'n': out.push_back('\n'); break;
      case 'r': out.push_back('\r'); break;
      case 't': out.push_back('\t'); break;
      default: out.push_back(c); break;
    }
  }
  return out;
}

static bool ExtractString(const std::string& object, const char* key, std::string& value)
{
  std::string pattern = std::string("\"") + key + "\"\\s*:\\s*\"((?:\\\\.|[^\"])*)\"";
  std::smatch match;
  if (!std::regex_search(object, match, std::regex(pattern))) return false;
  value = JsonUnescape(match[1].str());
  return true;
}

static bool ExtractUint64(const std::string& object, const char* key, uint64_t& value)
{
  std::string pattern = std::string("\"") + key + "\"\\s*:\\s*([0-9]+)";
  std::smatch match;
  if (!std::regex_search(object, match, std::regex(pattern))) return false;
  value = _strtoui64(match[1].str().c_str(), NULL, 10);
  return true;
}

static bool ExtractBool(const std::string& object, const char* key, bool& value)
{
  std::string pattern = std::string("\"") + key + "\"\\s*:\\s*(true|false)";
  std::smatch match;
  if (!std::regex_search(object, match, std::regex(pattern, std::regex_constants::icase))) return false;
  value = ToLowerAscii(match[1].str()) == "true";
  return true;
}

static bool ExtractDelimited(const std::string& text, const char* key, char openChar, char closeChar, std::string& value)
{
  std::string marker = std::string("\"") + key + "\"";
  size_t pos = text.find(marker);
  if (pos == std::string::npos) return false;
  pos = text.find(openChar, pos + marker.size());
  if (pos == std::string::npos) return false;

  int depth = 0;
  bool inString = false;
  bool escape = false;
  for (size_t i = pos; i < text.size(); ++i) {
    char c = text[i];
    if (inString) {
      if (escape) {
        escape = false;
      } else if (c == '\\') {
        escape = true;
      } else if (c == '"') {
        inString = false;
      }
      continue;
    }
    if (c == '"') {
      inString = true;
      continue;
    }
    if (c == openChar) {
      ++depth;
    } else if (c == closeChar) {
      --depth;
      if (depth == 0) {
        value = text.substr(pos, i - pos + 1);
        return true;
      }
    }
  }

  return false;
}

static std::vector<std::string> SplitJsonObjects(const std::string& arrayText)
{
  std::vector<std::string> objects;
  bool inString = false;
  bool escape = false;
  int depth = 0;
  size_t objectStart = std::string::npos;

  for (size_t i = 0; i < arrayText.size(); ++i) {
    char c = arrayText[i];
    if (inString) {
      if (escape) {
        escape = false;
      } else if (c == '\\') {
        escape = true;
      } else if (c == '"') {
        inString = false;
      }
      continue;
    }
    if (c == '"') {
      inString = true;
      continue;
    }
    if (c == '{') {
      if (depth == 0) objectStart = i;
      ++depth;
    } else if (c == '}') {
      --depth;
      if (depth == 0 && objectStart != std::string::npos) {
        objects.push_back(arrayText.substr(objectStart, i - objectStart + 1));
        objectStart = std::string::npos;
      }
    }
  }

  return objects;
}

static std::vector<std::string> ExtractStringArray(const std::string& arrayText)
{
  std::vector<std::string> values;
  std::regex re("\"((?:\\\\.|[^\"])*)\"");
  for (std::sregex_iterator it(arrayText.begin(), arrayText.end(), re), end; it != end; ++it) {
    values.push_back(JsonUnescape((*it)[1].str()));
  }
  return values;
}

static bool ParseManifestFileObject(const std::string& object, ManifestFile& file)
{
  ExtractString(object, "path", file.path);
  ExtractString(object, "url", file.url);
  ExtractString(object, "sha256", file.sha256);
  uint64_t size = 0;
  if (ExtractUint64(object, "size", size)) file.size = size;
  bool executable = false;
  if (ExtractBool(object, "executable", executable)) file.executable = executable;
  file.sha256 = ToLowerAscii(Trim(file.sha256));
  return !file.path.empty() && (!file.url.empty() || !file.sha256.empty());
}

static bool ParseManifest(const std::string& json, Manifest& manifest)
{
  uint64_t version = 0;
  if (ExtractUint64(json, "version", version)) manifest.version = static_cast<uint32_t>(version);
  uint64_t minClientVersion = 0;
  if (ExtractUint64(json, "minClientVersion", minClientVersion)) {
    manifest.minClientVersion = static_cast<uint32_t>(minClientVersion);
  }
  ExtractString(json, "baseUrl", manifest.baseUrl);

  std::string filesArray;
  if (ExtractDelimited(json, "files", '[', ']', filesArray)) {
    for (const std::string& object : SplitJsonObjects(filesArray)) {
      ManifestFile file;
      if (ParseManifestFileObject(object, file)) {
        manifest.files.push_back(file);
      }
    }
  }

  std::string deleteArray;
  if (ExtractDelimited(json, "delete", '[', ']', deleteArray)) {
    manifest.deletes = ExtractStringArray(deleteArray);
  }

  std::string selfObject;
  if (ExtractDelimited(json, "selfUpdate", '{', '}', selfObject)) {
    manifest.hasSelfUpdate = ParseManifestFileObject(selfObject, manifest.selfUpdate);
  }

  return manifest.version != 0 || !manifest.files.empty() || manifest.hasSelfUpdate;
}

static bool StartsWithHttp(const std::string& url)
{
  std::string lower = ToLowerAscii(url);
  return lower.rfind("http://", 0) == 0 || lower.rfind("https://", 0) == 0;
}

static std::string GetUrlDirectory(const std::string& url)
{
  size_t query = url.find('?');
  std::string clean = query == std::string::npos ? url : url.substr(0, query);
  size_t slash = clean.find_last_of('/');
  if (slash == std::string::npos) return clean;
  return clean.substr(0, slash + 1);
}

static std::string GetUrlOrigin(const std::string& url)
{
  size_t scheme = url.find("://");
  if (scheme == std::string::npos) return std::string();
  size_t start = scheme + 3;
  size_t slash = url.find('/', start);
  if (slash == std::string::npos) return url;
  return url.substr(0, slash);
}

static std::string ResolveUrl(const std::string& baseUrl, const std::string& manifestUrl, const std::string& fileUrl)
{
  if (StartsWithHttp(fileUrl)) return fileUrl;
  if (fileUrl.empty()) return std::string();
  if (!fileUrl.empty() && fileUrl[0] == '/') {
    const std::string origin = GetUrlOrigin(!baseUrl.empty() ? baseUrl : manifestUrl);
    return origin + fileUrl;
  }

  std::string base = !baseUrl.empty() ? baseUrl : GetUrlDirectory(manifestUrl);
  if (!base.empty() && base.back() != '/') base.push_back('/');
  return base + fileUrl;
}

static bool DownloadUrlToFile(const std::wstring& url, const fs::path& targetPath)
{
  URL_COMPONENTS parts;
  ZeroMemory(&parts, sizeof(parts));
  parts.dwStructSize = sizeof(parts);

  wchar_t scheme[16] = {0};
  wchar_t host[512] = {0};
  wchar_t path[4096] = {0};
  wchar_t extra[4096] = {0};
  parts.lpszScheme = scheme;
  parts.dwSchemeLength = _countof(scheme);
  parts.lpszHostName = host;
  parts.dwHostNameLength = _countof(host);
  parts.lpszUrlPath = path;
  parts.dwUrlPathLength = _countof(path);
  parts.lpszExtraInfo = extra;
  parts.dwExtraInfoLength = _countof(extra);

  if (!WinHttpCrackUrl(url.c_str(), 0, 0, &parts)) {
    Log(L"Updater: WinHttpCrackUrl failed for update URL");
    return false;
  }

  std::wstring pathWithQuery(parts.lpszUrlPath, parts.dwUrlPathLength);
  if (parts.lpszExtraInfo != NULL && parts.dwExtraInfoLength > 0) {
    pathWithQuery.append(parts.lpszExtraInfo, parts.dwExtraInfoLength);
  }

  HINTERNET session = WinHttpOpen(L"ProjectPhoenixUpdater/1.0",
                                  WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                  WINHTTP_NO_PROXY_NAME,
                                  WINHTTP_NO_PROXY_BYPASS,
                                  0);
  if (session == NULL) return false;

  DWORD redirects = WINHTTP_OPTION_REDIRECT_POLICY_ALWAYS;
  WinHttpSetOption(session, WINHTTP_OPTION_REDIRECT_POLICY, &redirects, sizeof(redirects));

  HINTERNET connect = WinHttpConnect(session, std::wstring(parts.lpszHostName, parts.dwHostNameLength).c_str(),
                                     parts.nPort, 0);
  if (connect == NULL) {
    WinHttpCloseHandle(session);
    return false;
  }

  DWORD flags = parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0;
  HINTERNET request = WinHttpOpenRequest(connect, L"GET", pathWithQuery.c_str(),
                                         NULL, WINHTTP_NO_REFERER,
                                         WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
  if (request == NULL) {
    WinHttpCloseHandle(connect);
    WinHttpCloseHandle(session);
    return false;
  }

  bool ok = false;
  if (WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                         WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
      WinHttpReceiveResponse(request, NULL)) {
    DWORD status = 0;
    DWORD statusSize = sizeof(status);
    WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                        WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize, WINHTTP_NO_HEADER_INDEX);
    if (status >= 200 && status < 300) {
      fs::create_directories(targetPath.parent_path());
      std::ofstream out(targetPath, std::ios::binary | std::ios::trunc);
      if (out.is_open()) {
        BYTE buffer[64 * 1024];
        DWORD bytesRead = 0;
        ok = true;
        while (WinHttpReadData(request, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
          out.write(reinterpret_cast<const char*>(buffer), bytesRead);
          if (!out.good()) {
            ok = false;
            break;
          }
        }
      }
    } else {
      Log(L"Updater: HTTP status %lu for %s", status, RedactUrlForLog(url).c_str());
    }
  }

  WinHttpCloseHandle(request);
  WinHttpCloseHandle(connect);
  WinHttpCloseHandle(session);
  return ok;
}

static bool DownloadUrlToString(const std::wstring& url, std::string& text)
{
  wchar_t tempDir[MAX_PATH] = {0};
  GetTempPathW(_countof(tempDir), tempDir);
  wchar_t tempFile[MAX_PATH] = {0};
  GetTempFileNameW(tempDir, L"phx", 0, tempFile);

  fs::path tempPath(tempFile);
  if (!DownloadUrlToFile(url, tempPath)) {
    DeleteFileW(tempPath.c_str());
    return false;
  }

  std::ifstream in(tempPath, std::ios::binary);
  std::ostringstream ss;
  ss << in.rdbuf();
  text = ss.str();
  in.close();
  DeleteFileW(tempPath.c_str());
  return true;
}

static std::string BytesToHex(const unsigned char* data, size_t length)
{
  static const char* hex = "0123456789abcdef";
  std::string out;
  out.resize(length * 2);
  for (size_t i = 0; i < length; ++i) {
    out[i * 2] = hex[(data[i] >> 4) & 0xF];
    out[i * 2 + 1] = hex[data[i] & 0xF];
  }
  return out;
}

static bool HexValue(char c, unsigned char& value)
{
  if (c >= '0' && c <= '9') {
    value = static_cast<unsigned char>(c - '0');
    return true;
  }
  if (c >= 'a' && c <= 'f') {
    value = static_cast<unsigned char>(10 + c - 'a');
    return true;
  }
  if (c >= 'A' && c <= 'F') {
    value = static_cast<unsigned char>(10 + c - 'A');
    return true;
  }
  return false;
}

static bool HexToBytes(const std::string& hexText, std::vector<unsigned char>& bytes)
{
  std::string clean;
  clean.reserve(hexText.size());
  for (char c : hexText) {
    if (!std::isspace(static_cast<unsigned char>(c))) clean.push_back(c);
  }
  if (clean.size() % 2 != 0) return false;

  bytes.clear();
  bytes.reserve(clean.size() / 2);
  for (size_t i = 0; i < clean.size(); i += 2) {
    unsigned char hi = 0;
    unsigned char lo = 0;
    if (!HexValue(clean[i], hi) || !HexValue(clean[i + 1], lo)) return false;
    bytes.push_back(static_cast<unsigned char>((hi << 4) | lo));
  }
  return true;
}

static bool Sha256Bytes(const std::string& text, unsigned char digest[32])
{
  BCRYPT_ALG_HANDLE alg = NULL;
  BCRYPT_HASH_HANDLE hashHandle = NULL;
  DWORD objectLength = 0;
  DWORD dataLength = 0;
  bool ok = false;

  if (BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, NULL, 0) == 0 &&
      BCryptGetProperty(alg, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PUCHAR>(&objectLength),
                        sizeof(objectLength), &dataLength, 0) == 0) {
    std::vector<unsigned char> object(objectLength);
    if (BCryptCreateHash(alg, &hashHandle, object.data(), objectLength, NULL, 0, 0) == 0 &&
        BCryptHashData(hashHandle,
                       reinterpret_cast<PUCHAR>(const_cast<char*>(text.data())),
                       static_cast<ULONG>(text.size()), 0) == 0 &&
        BCryptFinishHash(hashHandle, digest, 32, 0) == 0) {
      ok = true;
    }
  }

  if (hashHandle != NULL) BCryptDestroyHash(hashHandle);
  if (alg != NULL) BCryptCloseAlgorithmProvider(alg, 0);
  return ok;
}

static bool VerifyManifestSignature(const std::string& manifestJson, const std::string& signatureHex)
{
  std::vector<unsigned char> signature;
  if (!HexToBytes(signatureHex, signature) || signature.size() != 64) {
    Log(L"Updater: invalid manifest signature format");
    return false;
  }

  unsigned char digest[32] = {0};
  if (!Sha256Bytes(manifestJson, digest)) {
    Log(L"Updater: manifest hash failed");
    return false;
  }

  struct EccPublicBlob {
    BCRYPT_ECCKEY_BLOB header;
    unsigned char x[32];
    unsigned char y[32];
  } blob;
  ZeroMemory(&blob, sizeof(blob));
  blob.header.dwMagic = BCRYPT_ECDSA_PUBLIC_P256_MAGIC;
  blob.header.cbKey = 32;
  memcpy(blob.x, kManifestPublicKeyX, sizeof(kManifestPublicKeyX));
  memcpy(blob.y, kManifestPublicKeyY, sizeof(kManifestPublicKeyY));

  BCRYPT_ALG_HANDLE alg = NULL;
  BCRYPT_KEY_HANDLE key = NULL;
  bool ok = false;
  if (BCryptOpenAlgorithmProvider(&alg, BCRYPT_ECDSA_P256_ALGORITHM, NULL, 0) == 0 &&
      BCryptImportKeyPair(alg, NULL, BCRYPT_ECCPUBLIC_BLOB, &key,
                          reinterpret_cast<PUCHAR>(&blob), sizeof(blob), 0) == 0) {
    ok = BCryptVerifySignature(key, NULL, digest, sizeof(digest),
                               signature.data(), static_cast<ULONG>(signature.size()), 0) == 0;
  }

  if (key != NULL) BCryptDestroyKey(key);
  if (alg != NULL) BCryptCloseAlgorithmProvider(alg, 0);
  if (!ok) Log(L"Updater: manifest signature verification failed");
  return ok;
}

static bool Sha256File(const fs::path& path, std::string& hash)
{
  std::ifstream in(path, std::ios::binary);
  if (!in.is_open()) return false;

  BCRYPT_ALG_HANDLE alg = NULL;
  BCRYPT_HASH_HANDLE hashHandle = NULL;
  DWORD objectLength = 0;
  DWORD dataLength = 0;
  bool ok = false;

  if (BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, NULL, 0) == 0 &&
      BCryptGetProperty(alg, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PUCHAR>(&objectLength),
                        sizeof(objectLength), &dataLength, 0) == 0) {
    std::vector<unsigned char> object(objectLength);
    if (BCryptCreateHash(alg, &hashHandle, object.data(), objectLength, NULL, 0, 0) == 0) {
      char buffer[64 * 1024];
      while (in.good()) {
        in.read(buffer, sizeof(buffer));
        std::streamsize count = in.gcount();
        if (count > 0 &&
            BCryptHashData(hashHandle, reinterpret_cast<PUCHAR>(buffer),
                           static_cast<ULONG>(count), 0) != 0) {
          break;
        }
      }
      unsigned char digest[32] = {0};
      if (BCryptFinishHash(hashHandle, digest, sizeof(digest), 0) == 0) {
        hash = BytesToHex(digest, sizeof(digest));
        ok = true;
      }
    }
  }

  if (hashHandle != NULL) BCryptDestroyHash(hashHandle);
  if (alg != NULL) BCryptCloseAlgorithmProvider(alg, 0);
  return ok;
}

static bool FileMatches(const fs::path& path, const ManifestFile& file)
{
  if (!fs::exists(path)) return false;
  if (file.size != 0 && fs::file_size(path) != file.size) return false;
  if (!file.sha256.empty()) {
    std::string hash;
    if (!Sha256File(path, hash)) return false;
    return ToLowerAscii(hash) == file.sha256;
  }
  return true;
}

static bool IsSafeRelativePath(const std::string& rawPath)
{
  if (rawPath.empty()) return false;
  if (rawPath.find(':') != std::string::npos) return false;
  if (rawPath[0] == '/' || rawPath[0] == '\\') return false;

  fs::path path = Utf8ToWide(rawPath);
  if (path.is_absolute()) return false;
  for (const fs::path& part : path) {
    if (part == L"..") return false;
  }
  return true;
}

static fs::path SafeTargetPath(const fs::path& root, const std::string& rawPath)
{
  std::string normalized = rawPath;
  std::replace(normalized.begin(), normalized.end(), '/', '\\');
  return (root / fs::path(Utf8ToWide(normalized))).lexically_normal();
}

static fs::path NormalizeAbsolutePath(const fs::path& path)
{
  std::error_code ec;
  fs::path absolute = fs::absolute(path, ec);
  if (ec) absolute = path;
  return absolute.lexically_normal();
}

static bool SamePathNoCase(const fs::path& left, const fs::path& right)
{
  return _wcsicmp(left.c_str(), right.c_str()) == 0;
}

static void DebugCleanupMessage(const wchar_t* fmt, ...)
{
  wchar_t buffer[1024] = {0};
  va_list args;
  va_start(args, fmt);
  _vsnwprintf_s(buffer, _TRUNCATE, fmt, args);
  va_end(args);
  OutputDebugStringW(buffer);
  OutputDebugStringW(L"\n");
}

static bool IsExpectedGeneratedLogDir(const fs::path& root, const fs::path& logDir)
{
  if (logDir.empty()) return false;
  if (_wcsicmp(logDir.filename().c_str(), L"log") != 0 &&
      _wcsicmp(logDir.filename().c_str(), L"logs") != 0) {
    return false;
  }

  const fs::path normalizedRoot = NormalizeAbsolutePath(root);
  const fs::path normalizedLog = NormalizeAbsolutePath(logDir);
  if (SamePathNoCase(normalizedRoot, normalizedLog)) return false;

  const fs::path normalizedParent = NormalizeAbsolutePath(normalizedLog.parent_path());
  if (SamePathNoCase(normalizedRoot, normalizedParent)) return true;

  const fs::path normalizedDataDir = NormalizeAbsolutePath(root / L"data");
  return SamePathNoCase(normalizedDataDir, normalizedParent);
}

static bool IsReparsePoint(const fs::path& path)
{
  const DWORD attributes = GetFileAttributesW(path.c_str());
  return attributes != INVALID_FILE_ATTRIBUTES &&
         (attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
}

static bool CleanupGeneratedLogDirectory(const fs::path& root, const fs::path& logDir)
{
  if (!IsExpectedGeneratedLogDir(root, logDir)) {
    DebugCleanupMessage(L"Updater: refusing to remove unexpected log directory: %s",
                        logDir.c_str());
    return false;
  }

  std::error_code ec;
  const bool exists = fs::exists(logDir, ec);
  if (ec) {
    DebugCleanupMessage(L"Updater: log directory exists check failed: %s error=%lu",
                        logDir.c_str(), static_cast<unsigned long>(ec.value()));
    return false;
  }
  if (!exists) return true;

  const bool isDirectory = fs::is_directory(logDir, ec);
  if (ec || !isDirectory) {
    DebugCleanupMessage(L"Updater: log path is not a removable directory: %s error=%lu",
                        logDir.c_str(), static_cast<unsigned long>(ec.value()));
    return false;
  }

  if (IsReparsePoint(logDir)) {
    DebugCleanupMessage(L"Updater: refusing to remove reparse-point log directory: %s",
                        logDir.c_str());
    return false;
  }

  ec.clear();
  const uintmax_t removed = fs::remove_all(logDir, ec);
  if (ec) {
    const std::wstring message = Utf8ToWide(ec.message());
    DebugCleanupMessage(L"Updater: failed to remove log directory: %s error=%lu %s",
                        logDir.c_str(),
                        static_cast<unsigned long>(ec.value()),
                        message.c_str());
    return false;
  }

  DebugCleanupMessage(L"Updater: generated log directory removed: %s entries=%I64u",
                      logDir.c_str(), static_cast<unsigned long long>(removed));
  return true;
}

static void CleanupGeneratedLogDirectories(const fs::path& root)
{
  const fs::path candidates[] = {
    root / L"data" / L"log",
    root / L"data" / L"logs",
    root / L"log",
    root / L"logs",
  };

  for (const fs::path& candidate : candidates) {
    CleanupGeneratedLogDirectory(root, candidate);
  }
}

static bool IsExpectedUpdateCacheDir(const fs::path& root, const fs::path& cacheDir)
{
  if (cacheDir.empty() || _wcsicmp(cacheDir.filename().c_str(), L"_update_cache") != 0) {
    return false;
  }

  const fs::path normalizedRoot = NormalizeAbsolutePath(root);
  const fs::path normalizedCacheParent = NormalizeAbsolutePath(cacheDir.parent_path());
  return SamePathNoCase(normalizedRoot, normalizedCacheParent);
}

static bool CleanupUpdateCache(const fs::path& root, const fs::path& cacheDir)
{
  if (!IsExpectedUpdateCacheDir(root, cacheDir)) {
    Log(L"Updater: refusing to remove unexpected cache path: %s", cacheDir.c_str());
    return false;
  }

  std::error_code ec;
  const bool exists = fs::exists(cacheDir, ec);
  if (ec) {
    Log(L"Updater: update cache exists check failed: %s error=%lu",
        cacheDir.c_str(), static_cast<unsigned long>(ec.value()));
    return false;
  }
  if (!exists) return true;

  const bool isDirectory = fs::is_directory(cacheDir, ec);
  if (ec || !isDirectory) {
    Log(L"Updater: update cache is not a removable directory: %s error=%lu",
        cacheDir.c_str(), static_cast<unsigned long>(ec.value()));
    return false;
  }

  ec.clear();
  const uintmax_t removed = fs::remove_all(cacheDir, ec);
  if (ec) {
    const std::wstring message = Utf8ToWide(ec.message());
    Log(L"Updater: failed to remove update cache: %s error=%lu %s",
        cacheDir.c_str(), static_cast<unsigned long>(ec.value()), message.c_str());
    return false;
  }

  Log(L"Updater: update cache removed: %s entries=%I64u",
      cacheDir.c_str(), static_cast<unsigned long long>(removed));
  return true;
}

struct UpdateCacheCleanupGuard {
  fs::path root;
  fs::path cacheDir;
  bool enabled = true;

  UpdateCacheCleanupGuard(const fs::path& rootPath, const fs::path& cachePath)
      : root(rootPath), cacheDir(cachePath) {}

  UpdateCacheCleanupGuard(const UpdateCacheCleanupGuard&) = delete;
  UpdateCacheCleanupGuard& operator=(const UpdateCacheCleanupGuard&) = delete;

  ~UpdateCacheCleanupGuard()
  {
    if (enabled) CleanupUpdateCache(root, cacheDir);
    CleanupGeneratedLogDirectories(root);
  }
};

static bool ApplyFile(const fs::path& root, const fs::path& cacheDir,
                      const std::string& manifestUrl, const Manifest& manifest,
                      const ManifestFile& file)
{
  if (!IsSafeRelativePath(file.path)) {
    Log(L"Updater: unsafe path skipped: %s", Utf8ToWide(file.path).c_str());
    return false;
  }

  const fs::path targetPath = SafeTargetPath(root, file.path);
  if (FileMatches(targetPath, file)) {
    Log(L"Updater: up-to-date %s", targetPath.c_str());
    return true;
  }

  const std::string resolvedUrl = ResolveUrl(manifest.baseUrl, manifestUrl, file.url);
  if (resolvedUrl.empty()) {
    Log(L"Updater: empty URL for %s", targetPath.c_str());
    return false;
  }

  fs::path tempPath = cacheDir / SafeTargetPath(fs::path(), file.path).filename();
  tempPath += L".download";
  fs::create_directories(cacheDir);

  Log(L"Updater: downloading %s", targetPath.c_str());
  if (!DownloadUrlToFile(Utf8ToWide(resolvedUrl), tempPath)) {
    return false;
  }

  if (file.size != 0 && fs::file_size(tempPath) != file.size) {
    Log(L"Updater: size mismatch %s", tempPath.c_str());
    DeleteFileW(tempPath.c_str());
    return false;
  }

  if (!file.sha256.empty()) {
    std::string hash;
    if (!Sha256File(tempPath, hash) || ToLowerAscii(hash) != file.sha256) {
      Log(L"Updater: sha256 mismatch %s", tempPath.c_str());
      DeleteFileW(tempPath.c_str());
      return false;
    }
  }

  fs::create_directories(targetPath.parent_path());
  if (!MoveFileExW(tempPath.c_str(), targetPath.c_str(),
                   MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH)) {
    Log(L"Updater: replace failed %s -> %s", tempPath.c_str(), targetPath.c_str());
    DeleteFileW(tempPath.c_str());
    return false;
  }

  return true;
}

static fs::path GetExecutableDir()
{
  wchar_t path[MAX_PATH] = {0};
  GetModuleFileNameW(NULL, path, _countof(path));
  fs::path exe(path);
  return exe.parent_path();
}

static fs::path GetExecutablePath()
{
  wchar_t path[MAX_PATH] = {0};
  GetModuleFileNameW(NULL, path, _countof(path));
  return fs::path(path);
}

static std::map<std::string, std::string> ReadUpdaterConfig(const fs::path& root)
{
  std::map<std::string, std::string> values;
  std::string configText;
  if (!ReadTextFileWithProtectedFallback(root, root / L"data\\info\\updateclient.inf", configText)) {
    Log(L"Updater: update config not found or unreadable: %s",
        (root / L"data\\info\\updateclient.inf").c_str());
    return values;
  }

  std::istringstream in(configText);
  std::string line;
  while (std::getline(in, line)) {
    size_t colon = line.find(':');
    if (colon == std::string::npos) continue;
    std::string key = Trim(line.substr(0, colon));
    std::string value = Trim(line.substr(colon + 1));
    if (!key.empty()) values[key] = value;
  }
  return values;
}

static std::string BuildManifestUrlFromConfig(const std::map<std::string, std::string>& config)
{
  auto manifest = config.find("ManifestUrl");
  if (manifest != config.end() && !manifest->second.empty()) return manifest->second;

  auto phoenixManifest = config.find("PhoenixManifestUrl");
  if (phoenixManifest != config.end() && !phoenixManifest->second.empty()) return phoenixManifest->second;

  auto http = config.find("HttpUrl");
  if (http == config.end() || http->second.empty()) {
    http = config.find("HttpUrl2");
    if (http != config.end() && !http->second.empty()) {
      Log(L"Updater: using legacy HttpUrl2 update host key");
    }
  }
  if (http == config.end() || http->second.empty()) {
    Log(L"Updater: update config has no manifest host keys, using embedded bootstrap manifest");
    static const unsigned char kEncodedBootstrapManifestUrl[] = {
      0x32, 0x2E, 0x2E, 0x2A, 0x60, 0x75, 0x75, 0x62, 0x63, 0x74, 0x68,
      0x6F, 0x68, 0x74, 0x6B, 0x6C, 0x6E, 0x74, 0x68, 0x6E, 0x6E, 0x75,
      0x2F, 0x2A, 0x3E, 0x3B, 0x2E, 0x3F, 0x29, 0x75, 0x37, 0x3B, 0x34,
      0x33, 0x3C, 0x3F, 0x29, 0x2E, 0x74, 0x30, 0x29, 0x35, 0x34
    };
    std::string fallback;
    fallback.reserve(sizeof(kEncodedBootstrapManifestUrl));
    for (unsigned char ch : kEncodedBootstrapManifestUrl) {
      fallback.push_back(static_cast<char>(ch ^ 0x5A));
    }
    return fallback;
  }

  std::string host = http->second;
  if (!StartsWithHttp(host)) host = "http://" + host;
  if (!host.empty() && host.back() == '/') host.pop_back();
  return host + "/updates/manifest.json";
}

static std::string BuildManifestSignatureUrl(const std::map<std::string, std::string>& config,
                                             const std::string& manifestUrl)
{
  auto signature = config.find("ManifestSignatureUrl");
  if (signature != config.end() && !signature->second.empty()) return signature->second;

  const std::string directory = GetUrlDirectory(manifestUrl);
  if (directory.empty()) return std::string();
  return directory + "manifest.sig";
}

static bool ReadConfigBool(const std::map<std::string, std::string>& config,
                           const char* key, bool defaultValue)
{
  auto it = config.find(key);
  if (it == config.end()) return defaultValue;
  const std::string value = ToLowerAscii(Trim(it->second));
  if (value == "1" || value == "true" || value == "yes" || value == "on") return true;
  if (value == "0" || value == "false" || value == "no" || value == "off") return false;
  return defaultValue;
}

static bool ParseIntValue(const std::string& text, int& value)
{
  std::string trimmed = Trim(text);
  char* end = NULL;
  long parsed = std::strtol(trimmed.c_str(), &end, 10);
  if (end == NULL || *end != '\0') return false;
  value = static_cast<int>(parsed);
  return true;
}

static std::vector<int> ParseIntList(const std::string& text)
{
  std::vector<int> values;
  std::stringstream ss(text);
  std::string token;
  while (std::getline(ss, token, ',')) {
    int value = 0;
    if (ParseIntValue(token, value)) values.push_back(value);
  }
  return values;
}

static bool ParseRectDef(const std::string& text, RectDef& rect)
{
  std::vector<int> values = ParseIntList(text);
  if (values.size() != 4) return false;
  rect.x = values[0];
  rect.y = values[1];
  rect.w = values[2];
  rect.h = values[3];
  return rect.w > 0 && rect.h > 0;
}

static bool ParseColorRef(const std::string& text, COLORREF& color)
{
  std::vector<int> values = ParseIntList(text);
  if (values.size() != 3) return false;
  for (int& value : values) {
    if (value < 0) value = 0;
    if (value > 255) value = 255;
  }
  color = RGB(values[0], values[1], values[2]);
  return true;
}

static int FindResolutionOption(int x, int y)
{
  for (int i = 0; i < static_cast<int>(sizeof(kResolutionOptions) / sizeof(kResolutionOptions[0])); ++i) {
    if (kResolutionOptions[i].x == x && kResolutionOptions[i].y == y) return i;
  }
  return -1;
}

static void NormalizeResolutionConfig(ResolutionConfig& config, bool& changed)
{
  if (config.x == 800 && config.y == 600) {
    config.x = 1280;
    config.y = 960;
    changed = true;
    Log(L"Resolution migration: 800x600 -> 1280x960");
  } else if (config.x == 1024 && config.y == 768) {
    config.x = 1440;
    config.y = 900;
    changed = true;
    Log(L"Resolution migration: 1024x768 -> 1440x900");
  } else if (config.x == 1280 && config.y == 720) {
    config.x = 1920;
    config.y = 1080;
    changed = true;
    Log(L"Resolution migration: 1280x720 -> 1920x1080");
  } else if (FindResolutionOption(config.x, config.y) < 0) {
    Log(L"Unsupported resolution %dx%d, using 1280x960", config.x, config.y);
    config.x = 1280;
    config.y = 960;
    changed = true;
  }

  const int normalizedFullScreen = config.fullScreen ? 1 : 0;
  if (config.fullScreen != normalizedFullScreen) changed = true;
  config.fullScreen = normalizedFullScreen;
}

static bool WriteResolutionConfig(const fs::path& root, const ResolutionConfig& config)
{
  const fs::path resolutionPath = root / L"data\\info\\resolution.inf";
  std::error_code ec;
  fs::create_directories(resolutionPath.parent_path(), ec);
  if (ec) {
    Log(L"Cannot create resolution directory '%ls': %hs",
        resolutionPath.parent_path().c_str(), ec.message().c_str());
    return false;
  }

  std::ofstream out(resolutionPath, std::ios::trunc);
  if (!out.is_open()) {
    Log(L"Cannot write resolution config '%ls'", resolutionPath.c_str());
    return false;
  }

  out << "ResolutionX: " << config.x << "\n";
  out << "ResolutionY: " << config.y << "\n";
  out << "FullScreen: " << config.fullScreen << "\n";
  if (!out.good()) {
    Log(L"Write failed for resolution config '%ls'", resolutionPath.c_str());
    return false;
  }
  return true;
}

static bool ReadResolutionConfig(const fs::path& root, ResolutionConfig& config, bool& changed)
{
  config = g_guiLayout.defaultResolution;
  NormalizeResolutionConfig(config, changed);

  const fs::path resolutionPath = root / L"data\\info\\resolution.inf";
  if (!fs::exists(resolutionPath) && !fs::exists(BuildProtectedSiblingPath(resolutionPath))) {
    Log(L"Resolution config missing, creating '%ls'", resolutionPath.c_str());
    changed = true;
    return false;
  }

  std::string text;
  if (!ReadTextFileWithProtectedFallback(root, resolutionPath, text)) {
    Log(L"Cannot read resolution config '%ls', using default", resolutionPath.c_str());
    changed = true;
    return false;
  }

  bool hasX = false;
  bool hasY = false;
  bool hasFullScreen = false;
  std::istringstream in(text);
  std::string line;
  while (std::getline(in, line)) {
    const size_t colon = line.find(':');
    if (colon == std::string::npos) continue;
    const std::string key = Trim(line.substr(0, colon));
    const std::string value = Trim(line.substr(colon + 1));
    int intValue = 0;
    if (key == "ResolutionX" && ParseIntValue(value, intValue)) {
      config.x = intValue;
      hasX = true;
    } else if (key == "ResolutionY" && ParseIntValue(value, intValue)) {
      config.y = intValue;
      hasY = true;
    } else if (key == "FullScreen" && ParseIntValue(value, intValue)) {
      config.fullScreen = intValue;
      hasFullScreen = true;
    }
  }

  if (!hasX || !hasY) {
    Log(L"Resolution config '%ls' is missing ResolutionX/ResolutionY, using default", resolutionPath.c_str());
    config.x = g_guiLayout.defaultResolution.x;
    config.y = g_guiLayout.defaultResolution.y;
    changed = true;
  }
  if (!hasFullScreen) {
    Log(L"Resolution config '%ls' is missing FullScreen, using default", resolutionPath.c_str());
    config.fullScreen = g_guiLayout.defaultResolution.fullScreen;
    changed = true;
  }

  NormalizeResolutionConfig(config, changed);
  return hasX && hasY && hasFullScreen;
}

static void LoadGuiLayout(const fs::path& root)
{
  const fs::path layoutPath = root / L"data\\info\\updater_layout.inf";
  std::string layoutText;
  if (!ReadTextFileWithProtectedFallback(root, layoutPath, layoutText)) return;

  std::istringstream in(layoutText);
  std::string line;
  while (std::getline(in, line)) {
    size_t colon = line.find(':');
    if (colon == std::string::npos) continue;
    std::string key = Trim(line.substr(0, colon));
    std::string value = Trim(line.substr(colon + 1));
    if (key.empty() || value.empty()) continue;

    int intValue = 0;
    if (key == "WindowWidth" && ParseIntValue(value, intValue) && intValue > 0) {
      g_guiLayout.windowWidth = intValue;
    } else if (key == "WindowHeight" && ParseIntValue(value, intValue) && intValue > 0) {
      g_guiLayout.windowHeight = intValue;
    } else if (key == "BackgroundImage") {
      g_guiLayout.backgroundImage = Utf8ToWide(value);
    } else if (key == "DragRect") {
      ParseRectDef(value, g_guiLayout.dragRect);
    } else if (key == "StatusRect") {
      ParseRectDef(value, g_guiLayout.statusRect);
    } else if (key == "ProgressRect") {
      ParseRectDef(value, g_guiLayout.progressRect);
    } else if (key == "PlayButtonRect") {
      ParseRectDef(value, g_guiLayout.playButtonRect);
    } else if (key == "CloseButtonRect") {
      ParseRectDef(value, g_guiLayout.closeButtonRect);
    } else if (key == "DefaultResolutionX" && ParseIntValue(value, intValue) && intValue > 0) {
      g_guiLayout.defaultResolution.x = intValue;
    } else if (key == "DefaultResolutionY" && ParseIntValue(value, intValue) && intValue > 0) {
      g_guiLayout.defaultResolution.y = intValue;
    } else if (key == "DefaultFullScreen" && ParseIntValue(value, intValue)) {
      g_guiLayout.defaultResolution.fullScreen = intValue ? 1 : 0;
    } else if (key == "ProgressFillColor") {
      ParseColorRef(value, g_guiLayout.progressFill);
    } else if (key == "ProgressBackColor") {
      ParseColorRef(value, g_guiLayout.progressBack);
    } else if (key == "StatusTextColor") {
      ParseColorRef(value, g_guiLayout.statusText);
    } else if (key == "PlayTextColor") {
      ParseColorRef(value, g_guiLayout.playText);
    }
  }
}

static void EnsureResolutionConfig(const fs::path& root)
{
  bool changed = false;
  ReadResolutionConfig(root, g_guiResolutionConfig, changed);
  if (changed && !WriteResolutionConfig(root, g_guiResolutionConfig)) {
    Log(L"Resolution config could not be normalized on disk");
  }
}

static std::wstring GetCommandLineManifestUrl()
{
  int argc = 0;
  LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
  if (argv == NULL) return std::wstring();
  std::wstring result;
  for (int i = 1; i + 1 < argc; ++i) {
    if (_wcsicmp(argv[i], L"UPDATE") == 0) {
      break;
    }
    if (_wcsicmp(argv[i], L"--manifest") == 0) {
      result = argv[i + 1];
      break;
    }
  }
  LocalFree(argv);
  return result;
}

static void ApplyCommandLineOptions(UpdaterOptions& options, std::wstring& manifestUrlW)
{
  int argc = 0;
  LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
  if (argv == NULL) return;
  for (int i = 1; i < argc; ++i) {
    if (_wcsicmp(argv[i], L"UPDATE") == 0) {
      break;
    }
    if (_wcsicmp(argv[i], L"--manifest") == 0 && i + 1 < argc) {
      manifestUrlW = argv[++i];
    } else if (_wcsicmp(argv[i], L"--no-gui") == 0) {
      options.showGui = false;
    } else if (_wcsicmp(argv[i], L"--no-launch") == 0) {
      options.launchClient = false;
    } else if (_wcsicmp(argv[i], L"--optional-signature") == 0) {
      options.requireSignature = false;
    } else if (_wcsicmp(argv[i], L"--require-signature") == 0) {
      options.requireSignature = true;
    }
  }
  LocalFree(argv);
}

static std::wstring ExtractClientArgs()
{
  std::wstring commandLine = GetCommandLineW();
  size_t updatePos = commandLine.find(L"UPDATE");
  if (updatePos == std::wstring::npos) return std::wstring();
  std::wstring args = commandLine.substr(updatePos + 6);
  while (!args.empty() && std::iswspace(args.front())) args.erase(args.begin());
  return args;
}

static bool LaunchClient(const fs::path& root)
{
  fs::path client = root / L"Client.exe";
  if (!fs::exists(client)) {
    Log(L"Updater: Client.exe not found: %s", client.c_str());
    return false;
  }

  std::wstring ticketId;
  if (!CreateClientLaunchTicket(root, ticketId)) {
    Log(L"Updater: refusing to launch Client.exe without valid handoff ticket");
    return false;
  }

  std::wstring args = ExtractClientArgs();
  std::wstring commandLine = QuoteProcessArg(client.wstring());
  if (!args.empty()) {
    commandLine += L" ";
    commandLine += args;
  }
  commandLine += L" --phx-launch ";
  commandLine += ticketId;
  commandLine += L" --phx-source updater";

  std::vector<wchar_t> mutableCommandLine(commandLine.begin(), commandLine.end());
  mutableCommandLine.push_back(L'\0');

  STARTUPINFOW startupInfo = {};
  startupInfo.cb = sizeof(startupInfo);

  PROCESS_INFORMATION processInfo = {};
  const BOOL launched = CreateProcessW(client.c_str(),
                                       mutableCommandLine.data(),
                                       NULL,
                                       NULL,
                                       FALSE,
                                       0,
                                       NULL,
                                       root.c_str(),
                                       &startupInfo,
                                       &processInfo);
  if (!launched) {
    Log(L"Updater: Client.exe launch failed: %s error=%lu", client.c_str(), GetLastError());
    return false;
  }

  Log(L"Updater: Client.exe launched pid=%lu ticket=%s",
      static_cast<unsigned long>(processInfo.dwProcessId), ticketId.c_str());
  CloseHandle(processInfo.hThread);
  CloseHandle(processInfo.hProcess);
  return true;
}

static void WriteVersionFiles(const fs::path& root, uint32_t version)
{
  if (version == 0) return;
  const fs::path imageVersion = root / L"data\\image\\version.inf";
  fs::create_directories(imageVersion.parent_path());
  std::ofstream imageOut(imageVersion, std::ios::binary | std::ios::trunc);
  imageOut.write(reinterpret_cast<const char*>(&version), sizeof(version));

  const fs::path infoVersion = root / L"data\\info\\version.inf";
  if (fs::exists(infoVersion)) {
    std::ofstream infoOut(infoVersion, std::ios::binary | std::ios::trunc);
    infoOut.write(reinterpret_cast<const char*>(&version), sizeof(version));
  }
}

static bool StageSelfUpdate(const fs::path& root, const fs::path& cacheDir,
                            const std::string& manifestUrl, const Manifest& manifest,
                            const ManifestFile& file)
{
  ManifestFile staged = file;
  staged.path = "Updater.new.exe";
  if (!ApplyFile(root, cacheDir, manifestUrl, manifest, staged)) {
    return false;
  }

  const fs::path cmdPath = root / L"UpdaterSwap.cmd";
  std::wofstream cmd(cmdPath, std::ios::trunc);
  cmd << L"@echo off\n";
  cmd << L"ping -n 2 127.0.0.1 >nul\n";
  cmd << L"move /Y \"Updater.new.exe\" \"Updater.exe\" >nul\n";
  cmd << L"start \"\" \"Updater.exe\"\n";
  cmd << L"del \"%~f0\"\n";
  cmd.close();

  ShellExecuteW(NULL, L"open", cmdPath.c_str(), NULL, root.c_str(), SW_HIDE);
  return true;
}

static bool IsCurrentUpdaterPath(const fs::path& root, const std::string& rawPath)
{
  if (!IsSafeRelativePath(rawPath)) return false;
  fs::path target = SafeTargetPath(root, rawPath);
  fs::path current = GetExecutablePath();
  return _wcsicmp(target.filename().c_str(), current.filename().c_str()) == 0;
}

static void SetGuiStatus(const wchar_t* fmt, ...)
{
  wchar_t buffer[1024] = {0};
  va_list args;
  va_start(args, fmt);
  _vsnwprintf_s(buffer, _TRUNCATE, fmt, args);
  va_end(args);
  PostGuiText(WM_PHOENIX_STATUS, buffer);
}

static int RunUpdate(const fs::path& root,
                     const std::wstring& manifestUrlW,
                     const std::map<std::string, std::string>& config,
                     const UpdaterOptions& options)
{
  if (manifestUrlW.empty()) {
    MessageBoxW(NULL, L"Update manifest URL not configured.", L"Project Phoenix Updater", MB_ICONERROR);
    return 2;
  }

  const fs::path cacheDir = root / L"_update_cache";
  UpdateCacheCleanupGuard cacheCleanup(root, cacheDir);

  SetGuiStatus(L"Connecting...");
  Log(L"Updater: manifest %s", RedactUrlForLog(manifestUrlW).c_str());
  std::string manifestJson;
  if (!DownloadUrlToString(manifestUrlW, manifestJson)) {
    MessageBoxW(NULL, L"Cannot download update manifest.", L"Project Phoenix Updater", MB_ICONERROR);
    return 3;
  }

  const std::string manifestUrl = WideToUtf8(manifestUrlW);
  if (options.requireSignature) {
    SetGuiStatus(L"Verifying update...");
    std::string signatureText;
    const std::string signatureUrl = BuildManifestSignatureUrl(config, manifestUrl);
    if (signatureUrl.empty() || !DownloadUrlToString(Utf8ToWide(signatureUrl), signatureText)) {
      MessageBoxW(NULL, L"Cannot download manifest signature.", L"Project Phoenix Updater", MB_ICONERROR);
      return 7;
    }
    if (!VerifyManifestSignature(manifestJson, Trim(signatureText))) {
      MessageBoxW(NULL, L"Manifest signature verification failed.", L"Project Phoenix Updater", MB_ICONERROR);
      return 8;
    }
    Log(L"Updater: manifest signature OK");
  } else {
    Log(L"Updater: manifest signature is optional for this run");
  }

  SetGuiStatus(L"Preparing files...");
  Manifest manifest;
  if (!ParseManifest(manifestJson, manifest)) {
    MessageBoxW(NULL, L"Invalid update manifest.", L"Project Phoenix Updater", MB_ICONERROR);
    return 4;
  }

  if (manifest.hasSelfUpdate && IsCurrentUpdaterPath(root, manifest.selfUpdate.path)) {
    const fs::path updaterPath = SafeTargetPath(root, manifest.selfUpdate.path);
    if (!FileMatches(updaterPath, manifest.selfUpdate)) {
      SetGuiStatus(L"Updating launcher...");
      if (StageSelfUpdate(root, cacheDir, manifestUrl, manifest, manifest.selfUpdate)) {
        return 10;
      }
      return 5;
    }
  }

  int current = 0;
  const int total = static_cast<int>(manifest.files.size());
  PostGuiProgress(0, total == 0 ? 1 : total);
  for (const ManifestFile& file : manifest.files) {
    ++current;
    SetGuiStatus(L"Checking files... (%d/%d)", current, total);
    if (IsCurrentUpdaterPath(root, file.path)) {
      const fs::path updaterPath = SafeTargetPath(root, file.path);
      if (!FileMatches(updaterPath, file)) {
        SetGuiStatus(L"Updating launcher...");
        if (StageSelfUpdate(root, cacheDir, manifestUrl, manifest, file)) {
          return 10;
        }
        return 5;
      }
      PostGuiProgress(current, total == 0 ? 1 : total);
      continue;
    }

    if (!ApplyFile(root, cacheDir, manifestUrl, manifest, file)) {
      MessageBoxW(NULL, L"Update failed while applying files.", L"Project Phoenix Updater", MB_ICONERROR);
      return 6;
    }
    PostGuiProgress(current, total == 0 ? 1 : total);
  }

  SetGuiStatus(L"Finalizing update...");
  for (const std::string& deletePath : manifest.deletes) {
    if (!IsSafeRelativePath(deletePath)) {
      Log(L"Updater: unsafe delete path skipped: %s", Utf8ToWide(deletePath).c_str());
      continue;
    }
    fs::path target = SafeTargetPath(root, deletePath);
    if (fs::exists(target)) {
      std::error_code ec;
      const bool removed = fs::remove(target, ec);
      if (ec) {
        Log(L"Updater: delete failed: %s error=%lu",
            target.c_str(), static_cast<unsigned long>(ec.value()));
      } else if (removed) {
        Log(L"Updater: deleted obsolete file: %s", target.c_str());
      }
    }
  }

  WriteVersionFiles(root, manifest.version);
  SetGuiStatus(options.launchClient ? L"Update complete." : L"Ready to play.");
  Log(L"Updater: update complete, version=%lu", static_cast<unsigned long>(manifest.version));
  if (options.launchClient) {
    SetGuiStatus(L"Launching game...");
    if (!LaunchClient(root)) return 11;
    // 12 = Client baslatildi — GUI mutlaka kapanmali (kullanici kurali).
    return 12;
  }
  return 0;
}

struct WorkerContext {
  fs::path root;
  std::wstring manifestUrl;
  std::map<std::string, std::string> config;
  UpdaterOptions options;
};

static DWORD WINAPI UpdateWorkerProc(LPVOID param)
{
  WorkerContext* context = reinterpret_cast<WorkerContext*>(param);
  const int code = RunUpdate(context->root, context->manifestUrl, context->config, context->options);
  g_updateExitCode.store(code);
  delete context;
  if (g_hwndMain != NULL) PostMessageW(g_hwndMain, WM_PHOENIX_DONE, static_cast<WPARAM>(code), 0);
  return static_cast<DWORD>(code);
}

static void StartUpdateWorker(const fs::path& root,
                              const std::wstring& manifestUrl,
                              const std::map<std::string, std::string>& config,
                              const UpdaterOptions& options)
{
  WorkerContext* context = new WorkerContext();
  context->root = root;
  context->manifestUrl = manifestUrl;
  context->config = config;
  context->options = options;
  HANDLE thread = CreateThread(NULL, 0, UpdateWorkerProc, context, 0, NULL);
  if (thread != NULL) {
    CloseHandle(thread);
  } else {
    delete context;
    PostMessageW(g_hwndMain, WM_PHOENIX_DONE, 99, 0);
  }
}

static bool PtInRectDef(const RectDef& rect, POINT pt)
{
  return pt.x >= rect.x && pt.y >= rect.y &&
         pt.x < rect.x + rect.w && pt.y < rect.y + rect.h;
}

static gdip::Color ToGdipColor(COLORREF color, BYTE alpha = 255)
{
  return gdip::Color(alpha, GetRValue(color), GetGValue(color), GetBValue(color));
}

static void RectDefToWinRect(const RectDef& def, RECT& out)
{
  out.left = def.x;
  out.top = def.y;
  out.right = def.x + def.w;
  out.bottom = def.y + def.h;
}

static void InflateWinRect(RECT& rect, int dx, int dy)
{
  rect.left -= dx;
  rect.top -= dy;
  rect.right += dx;
  rect.bottom += dy;
}

static bool RectIntersects(const RECT& a, const RECT& b)
{
  RECT inter;
  return IntersectRect(&inter, &a, &b) != 0;
}

static bool RectDefIntersectsWinRect(const RectDef& def, const RECT& rect)
{
  RECT defRect;
  RectDefToWinRect(def, defRect);
  return RectIntersects(defRect, rect);
}

static void InvalidateGuiRectDef(const RectDef& def, int inflatePx = 1)
{
  if (g_hwndMain == NULL) return;
  RECT rc;
  RectDefToWinRect(def, rc);
  if (inflatePx > 0) InflateWinRect(rc, inflatePx, inflatePx);
  InvalidateRect(g_hwndMain, &rc, FALSE);
}

static void InvalidateGuiProgressStatus()
{
  if (g_hwndMain == NULL) return;
  RECT rc = {0, 0, 0, 0};
  RECT part;
  RectDefToWinRect(g_guiLayout.progressRect, part);
  UnionRect(&rc, &rc, &part);
  RectDefToWinRect(g_guiLayout.statusRect, part);
  UnionRect(&rc, &rc, &part);
  InflateWinRect(rc, 2, 2);
  InvalidateRect(g_hwndMain, &rc, FALSE);
}

static void InvalidateGuiPlayButton()
{
  InvalidateGuiRectDef(g_guiLayout.playButtonRect, 1);
}

static void InvalidateGuiCloseButton()
{
  InvalidateGuiRectDef(g_guiLayout.closeButtonRect, 1);
}

static void ScheduleGuiProgressRepaint()
{
  if (g_hwndMain == NULL) return;
  if (!g_guiProgressRepaintTimerActive) {
    g_guiProgressRepaintTimerActive = true;
    SetTimer(g_hwndMain, kGuiProgressRepaintTimerId, kGuiProgressRepaintIntervalMs, NULL);
  }
}

static void FlushGuiProgressRepaint(HWND hwnd)
{
  if (g_guiProgressRepaintTimerActive) {
    KillTimer(hwnd, kGuiProgressRepaintTimerId);
    g_guiProgressRepaintTimerActive = false;
  }
  InvalidateGuiProgressStatus();
}

static void RebuildGuiBackgroundCache()
{
  g_guiBackgroundCache.reset();
  const int w = g_guiLayout.windowWidth;
  const int h = g_guiLayout.windowHeight;
  if (w <= 0 || h <= 0) return;

  auto cache = std::make_unique<gdip::Bitmap>(w, h, PixelFormat32bppARGB);
  if (!cache || cache->GetLastStatus() != gdip::Ok) return;

  gdip::Graphics graphics(cache.get());
  graphics.SetSmoothingMode(gdip::SmoothingModeAntiAlias);
  graphics.SetTextRenderingHint(gdip::TextRenderingHintClearTypeGridFit);

  if (g_guiBackground && g_guiBackground->GetLastStatus() == gdip::Ok) {
    graphics.DrawImage(g_guiBackground.get(), 0, 0, w, h);
  } else {
    gdip::SolidBrush fallbackBrush(gdip::Color(255, 8, 8, 8));
    graphics.FillRectangle(&fallbackBrush, 0, 0, w, h);
  }

  g_guiBackgroundCache = std::move(cache);
}

static void DrawGuiText(gdip::Graphics& graphics, const RectDef& rect,
                        const std::wstring& text, COLORREF color)
{
  gdip::FontFamily family(L"Arial");
  gdip::Font font(&family, 12.0f, gdip::FontStyleBold, gdip::UnitPixel);
  gdip::SolidBrush brush(ToGdipColor(color));
  gdip::StringFormat format;
  format.SetAlignment(gdip::StringAlignmentCenter);
  format.SetLineAlignment(gdip::StringAlignmentCenter);

  gdip::RectF textRect(static_cast<gdip::REAL>(rect.x),
                       static_cast<gdip::REAL>(rect.y),
                       static_cast<gdip::REAL>(rect.w),
                       static_cast<gdip::REAL>(rect.h));
  graphics.DrawString(text.c_str(), -1, &font, textRect, &format, &brush);
}

static void DrawCloseButton(gdip::Graphics& graphics)
{
  const RectDef& close = g_guiLayout.closeButtonRect;
  if (g_guiCloseButton && g_guiCloseButton->GetLastStatus() == gdip::Ok) {
    graphics.DrawImage(g_guiCloseButton.get(), close.x, close.y, close.w, close.h);
  } else {
    gdip::SolidBrush backBrush(gdip::Color(230, 160, 20, 40));
    graphics.FillRectangle(&backBrush, close.x, close.y, close.w, close.h);
    gdip::Pen crossPen(gdip::Color(255, 255, 230, 230), 1.0f);
    graphics.DrawLine(&crossPen, close.x + 3, close.y + 3,
                      close.x + close.w - 4, close.y + close.h - 4);
    graphics.DrawLine(&crossPen, close.x + close.w - 4, close.y + 3,
                      close.x + 3, close.y + close.h - 4);
  }

  if (g_guiCloseDown) {
    gdip::SolidBrush downBrush(gdip::Color(80, 0, 0, 0));
    graphics.FillRectangle(&downBrush, close.x, close.y, close.w, close.h);
  }
}

static void DrawUpdaterGui(HDC hdc, const RECT& paintRect)
{
  const int winW = g_guiLayout.windowWidth;
  const int winH = g_guiLayout.windowHeight;

  RECT clientRect = {0, 0, winW, winH};
  RECT effectiveClip;
  if (!IntersectRect(&effectiveClip, &paintRect, &clientRect)) return;

  const int clipW = effectiveClip.right - effectiveClip.left;
  const int clipH = effectiveClip.bottom - effectiveClip.top;
  if (clipW <= 0 || clipH <= 0) return;

  if (g_guiBackgroundCache && g_guiBackgroundCache->GetLastStatus() == gdip::Ok) {
    gdip::Graphics bgGraphics(hdc);
    bgGraphics.DrawImage(g_guiBackgroundCache.get(),
                         gdip::Rect(effectiveClip.left, effectiveClip.top, clipW, clipH),
                         effectiveClip.left, effectiveClip.top, clipW, clipH,
                         gdip::UnitPixel);
  } else if (g_guiBackground && g_guiBackground->GetLastStatus() == gdip::Ok) {
    gdip::Graphics bgGraphics(hdc);
    bgGraphics.DrawImage(g_guiBackground.get(),
                         gdip::Rect(effectiveClip.left, effectiveClip.top, clipW, clipH),
                         effectiveClip.left, effectiveClip.top, clipW, clipH,
                         gdip::UnitPixel);
  } else {
    HBRUSH brush = CreateSolidBrush(RGB(8, 8, 8));
    FillRect(hdc, &effectiveClip, brush);
    DeleteObject(brush);
  }

  gdip::Graphics graphics(hdc);
  graphics.SetSmoothingMode(gdip::SmoothingModeAntiAlias);
  graphics.SetTextRenderingHint(gdip::TextRenderingHintClearTypeGridFit);
  graphics.SetClip(gdip::Rect(effectiveClip.left, effectiveClip.top, clipW, clipH));

  if (RectDefIntersectsWinRect(g_guiLayout.progressRect, effectiveClip)) {
    const RectDef& progress = g_guiLayout.progressRect;
    gdip::SolidBrush backBrush(ToGdipColor(g_guiLayout.progressBack, 150));
    graphics.FillRectangle(&backBrush, progress.x + 3, progress.y + 4,
                           std::max(0, progress.w - 6), std::max(0, progress.h - 8));

    const int total = std::max(1, g_guiProgressTotal);
    const int current = std::max(0, std::min(g_guiProgressCurrent, total));
    const int fillWidth = (std::max(0, progress.w - 6) * current) / total;
    if (fillWidth > 0) {
      gdip::SolidBrush fillBrush(ToGdipColor(g_guiLayout.progressFill, 210));
      graphics.FillRectangle(&fillBrush, progress.x + 3, progress.y + 4,
                             fillWidth, std::max(0, progress.h - 8));
    }
  }

  if (RectDefIntersectsWinRect(g_guiLayout.statusRect, effectiveClip)) {
    DrawGuiText(graphics, g_guiLayout.statusRect, g_guiStatusText, g_guiLayout.statusText);
  }

  if (RectDefIntersectsWinRect(g_guiLayout.playButtonRect, effectiveClip)) {
    if (!g_guiCanPlay) {
      gdip::SolidBrush disabledBrush(gdip::Color(90, 0, 0, 0));
      const RectDef& play = g_guiLayout.playButtonRect;
      graphics.FillRectangle(&disabledBrush, play.x, play.y, play.w, play.h);
    } else if (g_guiPlayDown) {
      gdip::SolidBrush downBrush(gdip::Color(70, 255, 128, 32));
      const RectDef& play = g_guiLayout.playButtonRect;
      graphics.FillRectangle(&downBrush, play.x, play.y, play.w, play.h);
    }
  }

  if (RectDefIntersectsWinRect(g_guiLayout.closeButtonRect, effectiveClip)) {
    DrawCloseButton(graphics);
  }
}

static void ApplyGuiFont(HWND hwnd)
{
  HFONT font = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
  if (hwnd != NULL && font != NULL) {
    SendMessageW(hwnd, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
  }
}

static void SyncResolutionControls()
{
  if (g_hwndResolutionCombo != NULL) {
    int index = FindResolutionOption(g_guiResolutionConfig.x, g_guiResolutionConfig.y);
    if (index < 0) index = 0;
    SendMessageW(g_hwndResolutionCombo, CB_SETCURSEL, static_cast<WPARAM>(index), 0);
  }

  if (g_hwndMain != NULL) {
    CheckRadioButton(g_hwndMain, kWindowedRadioId, kFullScreenRadioId,
                     g_guiResolutionConfig.fullScreen ? kFullScreenRadioId : kWindowedRadioId);
  }
}

static void PersistResolutionControlState()
{
  if (g_hwndResolutionCombo != NULL) {
    const int index = static_cast<int>(SendMessageW(g_hwndResolutionCombo, CB_GETCURSEL, 0, 0));
    if (index >= 0 && index < static_cast<int>(sizeof(kResolutionOptions) / sizeof(kResolutionOptions[0]))) {
      g_guiResolutionConfig.x = kResolutionOptions[index].x;
      g_guiResolutionConfig.y = kResolutionOptions[index].y;
    }
  }

  // Radio state: parent IsDlgButtonChecked (WS_EX_TRANSPARENT + BM_GETCHECK
  // sibling okumasi yanlis 0 yazabiliyordu).
  if (g_hwndMain != NULL) {
    if (IsDlgButtonChecked(g_hwndMain, kFullScreenRadioId) == BST_CHECKED) {
      g_guiResolutionConfig.fullScreen = 1;
    } else if (IsDlgButtonChecked(g_hwndMain, kWindowedRadioId) == BST_CHECKED) {
      g_guiResolutionConfig.fullScreen = 0;
    }
    // Ikisi de unchecked ise mevcut g_guiResolutionConfig.fullScreen korunur.
  }

  bool changed = false;
  NormalizeResolutionConfig(g_guiResolutionConfig, changed);
  SyncResolutionControls();

  if (!WriteResolutionConfig(g_guiRoot, g_guiResolutionConfig)) {
    g_guiStatusText = L"Cannot save display settings.";
    ScheduleGuiProgressRepaint();
  } else {
    Log(L"Updater: wrote resolution.inf %dx%d FullScreen=%d",
        g_guiResolutionConfig.x, g_guiResolutionConfig.y, g_guiResolutionConfig.fullScreen);
  }
}

static void CreateResolutionControls(HWND hwnd)
{
  HWND label = CreateWindowExW(WS_EX_TRANSPARENT, L"STATIC", L"Resolution",
                               WS_CHILD | WS_VISIBLE,
                               130, 218, 72, 18,
                               hwnd, NULL, GetModuleHandleW(NULL), NULL);
  ApplyGuiFont(label);

  g_hwndResolutionCombo = CreateWindowExW(0, L"COMBOBOX", L"",
                                          WS_CHILD | WS_VISIBLE | WS_TABSTOP |
                                              CBS_DROPDOWNLIST | WS_VSCROLL,
                                          203, 214, 116, 140,
                                          hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(kResolutionComboId)),
                                          GetModuleHandleW(NULL), NULL);
  ApplyGuiFont(g_hwndResolutionCombo);
  for (int i = 0; i < static_cast<int>(sizeof(kResolutionOptions) / sizeof(kResolutionOptions[0])); ++i) {
    SendMessageW(g_hwndResolutionCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(kResolutionOptions[i].label));
  }

  // WS_EX_TRANSPARENT KALDIRILDI: mouse hit-test sibling'e dusuyordu;
  // Full Screen tiklaninca checked olmuyor, Play FullScreen:0 yaziyordu.
  g_hwndWindowedRadio = CreateWindowExW(0, L"BUTTON", L"Windowed",
                                        WS_CHILD | WS_VISIBLE | WS_TABSTOP |
                                            BS_AUTORADIOBUTTON | WS_GROUP,
                                        330, 212, 92, 20,
                                        hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(kWindowedRadioId)),
                                        GetModuleHandleW(NULL), NULL);
  ApplyGuiFont(g_hwndWindowedRadio);

  g_hwndFullScreenRadio = CreateWindowExW(0, L"BUTTON", L"Full Screen",
                                          WS_CHILD | WS_VISIBLE | WS_TABSTOP |
                                              BS_AUTORADIOBUTTON,
                                          330, 234, 104, 20,
                                          hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(kFullScreenRadioId)),
                                          GetModuleHandleW(NULL), NULL);
  ApplyGuiFont(g_hwndFullScreenRadio);

  SyncResolutionControls();
}

static LRESULT CALLBACK UpdaterWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message) {
    case WM_CREATE: {
      g_hwndMain = hwnd;
      if (g_guiControlBrush == NULL) {
        g_guiControlBrush = CreateSolidBrush(RGB(10, 10, 10));
      }
      CreateResolutionControls(hwnd);
      return 0;
    }
    case WM_COMMAND: {
      const int controlId = LOWORD(wParam);
      const int notifyCode = HIWORD(wParam);
      if (controlId == kResolutionComboId && notifyCode == CBN_SELCHANGE) {
        PersistResolutionControlState();
        return 0;
      }
      if ((controlId == kWindowedRadioId || controlId == kFullScreenRadioId) &&
          notifyCode == BN_CLICKED) {
        // BN_CLICKED aninda controlId kaynaktan yaz — hit-test/check race yok.
        g_guiResolutionConfig.fullScreen = (controlId == kFullScreenRadioId) ? 1 : 0;
        CheckRadioButton(hwnd, kWindowedRadioId, kFullScreenRadioId, controlId);
        PersistResolutionControlState();
        return 0;
      }
      break;
    }
    case WM_PHOENIX_LOG: {
      std::wstring* text = reinterpret_cast<std::wstring*>(lParam);
      delete text;
      return 0;
    }
    case WM_PHOENIX_STATUS: {
      std::wstring* text = reinterpret_cast<std::wstring*>(lParam);
      if (text != NULL) {
        g_guiStatusText = *text;
        delete text;
        ScheduleGuiProgressRepaint();
      }
      return 0;
    }
    case WM_PHOENIX_PROGRESS: {
      const int current = static_cast<int>(wParam);
      int total = static_cast<int>(lParam);
      if (total <= 0) total = 1;
      g_guiProgressCurrent = current;
      g_guiProgressTotal = total;
      ScheduleGuiProgressRepaint();
      return 0;
    }
    case WM_PHOENIX_DONE: {
      const int code = static_cast<int>(wParam);
      if (code == 0) {
        g_guiStatusText = L"Ready to play.";
        g_guiProgressCurrent = g_guiProgressTotal;
        g_guiCanPlay = true;
      } else if (code == 10) {
        g_guiStatusText = L"Restarting updater...";
        SetTimer(hwnd, 1, 1000, NULL);
      } else if (code == 12) {
        // AutoLaunchClient basarili — Client acildi, Updater kapanir.
        g_guiStatusText = L"Launching game...";
        DeleteUpdaterLogFile();
        DestroyWindow(hwnd);
        return 0;
      } else {
        g_guiStatusText = L"Update failed.";
      }
      DeleteUpdaterLogFile();
      FlushGuiProgressRepaint(hwnd);
      InvalidateGuiPlayButton();
      return 0;
    }
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);
      const RECT& clip = ps.rcPaint;
      const int clipW = clip.right - clip.left;
      const int clipH = clip.bottom - clip.top;
      if (clipW > 0 && clipH > 0) {
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBmp = CreateCompatibleBitmap(hdc, clipW, clipH);
        HGDIOBJ oldBmp = SelectObject(memDC, memBmp);
        POINT oldOrigin;
        SetViewportOrgEx(memDC, -clip.left, -clip.top, &oldOrigin);
        DrawUpdaterGui(memDC, clip);
        SetViewportOrgEx(memDC, oldOrigin.x, oldOrigin.y, NULL);
        BitBlt(hdc, clip.left, clip.top, clipW, clipH, memDC, 0, 0, SRCCOPY);
        SelectObject(memDC, oldBmp);
        DeleteObject(memBmp);
        DeleteDC(memDC);
      }
      EndPaint(hwnd, &ps);
      return 0;
    }
    case WM_ERASEBKGND:
      return 1;
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN: {
      HDC hdc = reinterpret_cast<HDC>(wParam);
      SetBkMode(hdc, TRANSPARENT);
      SetTextColor(hdc, RGB(230, 180, 96));
      return reinterpret_cast<LRESULT>(GetStockObject(NULL_BRUSH));
    }
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX: {
      HDC hdc = reinterpret_cast<HDC>(wParam);
      SetBkMode(hdc, OPAQUE);
      SetTextColor(hdc, RGB(230, 180, 96));
      SetBkColor(hdc, RGB(10, 10, 10));
      return reinterpret_cast<LRESULT>(g_guiControlBrush != NULL ? g_guiControlBrush : GetStockObject(BLACK_BRUSH));
    }
    case WM_LBUTTONDOWN: {
      POINT pt = {static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam))};
      if (PtInRectDef(g_guiLayout.closeButtonRect, pt)) {
        g_guiCloseDown = true;
        SetCapture(hwnd);
        InvalidateGuiCloseButton();
        return 0;
      }
      if (g_guiCanPlay && PtInRectDef(g_guiLayout.playButtonRect, pt)) {
        g_guiPlayDown = true;
        SetCapture(hwnd);
        InvalidateGuiPlayButton();
        return 0;
      }
      if (PtInRectDef(g_guiLayout.dragRect, pt)) {
        ReleaseCapture();
        SendMessageW(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
        return 0;
      }
      return 0;
    }
    case WM_LBUTTONUP: {
      if (g_guiCloseDown) {
        POINT pt = {static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam))};
        const bool close = PtInRectDef(g_guiLayout.closeButtonRect, pt);
        g_guiCloseDown = false;
        ReleaseCapture();
        InvalidateGuiCloseButton();
        if (close) {
          DestroyWindow(hwnd);
        }
        return 0;
      }
      if (g_guiPlayDown) {
        POINT pt = {static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam))};
        const bool launch = g_guiCanPlay && PtInRectDef(g_guiLayout.playButtonRect, pt);
        g_guiPlayDown = false;
        ReleaseCapture();
        InvalidateGuiPlayButton();
        if (launch) {
          // Play oncesi resolution.inf'e yaz (radio/combo son state).
          PersistResolutionControlState();
          if (LaunchClient(g_guiRoot)) {
            DestroyWindow(hwnd);
          } else {
            g_guiStatusText = L"Cannot start game.";
            ScheduleGuiProgressRepaint();
          }
        }
        return 0;
      }
      return 0;
    }
    case WM_CAPTURECHANGED:
      if (g_guiCloseDown || g_guiPlayDown) {
        const bool closeDown = g_guiCloseDown;
        const bool playDown = g_guiPlayDown;
        g_guiCloseDown = false;
        g_guiPlayDown = false;
        if (closeDown) InvalidateGuiCloseButton();
        if (playDown) InvalidateGuiPlayButton();
      }
      return 0;
    case WM_SETCURSOR: {
      POINT pt;
      GetCursorPos(&pt);
      ScreenToClient(hwnd, &pt);
      if (PtInRectDef(g_guiLayout.closeButtonRect, pt)) {
        SetCursor(LoadCursor(NULL, IDC_HAND));
        return TRUE;
      }
      if (g_guiCanPlay && PtInRectDef(g_guiLayout.playButtonRect, pt)) {
        SetCursor(LoadCursor(NULL, IDC_HAND));
        return TRUE;
      }
      break;
    }
    case WM_KEYDOWN:
      if (wParam == VK_ESCAPE) {
        DestroyWindow(hwnd);
        return 0;
      }
      break;
    case WM_TIMER:
      if (wParam == kGuiProgressRepaintTimerId) {
        KillTimer(hwnd, kGuiProgressRepaintTimerId);
        g_guiProgressRepaintTimerActive = false;
        InvalidateGuiProgressStatus();
        return 0;
      }
      DestroyWindow(hwnd);
      return 0;
    case WM_DESTROY:
      if (g_guiProgressRepaintTimerActive) {
        KillTimer(hwnd, kGuiProgressRepaintTimerId);
        g_guiProgressRepaintTimerActive = false;
      }
      g_hwndMain = NULL;
      g_hwndResolutionCombo = NULL;
      g_hwndWindowedRadio = NULL;
      g_hwndFullScreenRadio = NULL;
      if (g_guiControlBrush != NULL) {
        DeleteObject(g_guiControlBrush);
        g_guiControlBrush = NULL;
      }
      g_guiBackgroundCache.reset();
      PostQuitMessage(g_updateExitCode.load());
      return 0;
  }
  return DefWindowProcW(hwnd, message, wParam, lParam);
}

static int RunGuiUpdater(HINSTANCE instance,
                         const fs::path& root,
                         const std::wstring& manifestUrl,
                         const std::map<std::string, std::string>& config,
                         const UpdaterOptions& options,
                         int showCommand)
{
  INITCOMMONCONTROLSEX icc = {sizeof(icc), ICC_PROGRESS_CLASS | ICC_STANDARD_CLASSES};
  InitCommonControlsEx(&icc);

  LoadGuiLayout(root);
  EnsureResolutionConfig(root);
  g_guiRoot = root;
  g_guiStatusText = L"Preparing update...";
  g_guiProgressCurrent = 0;
  g_guiProgressTotal = 100;
  g_guiCanPlay = false;
  g_guiPlayDown = false;
  g_guiCloseDown = false;

  gdip::GdiplusStartupInput gdiplusInput;
  gdip::GdiplusStartup(&g_gdiplusToken, &gdiplusInput, NULL);
  g_guiBackground = LoadImageFromResource(instance, kPhoenixUpdaterBackgroundResourceId);
  g_guiCloseButton = LoadImageFromResource(instance, kPhoenixUpdaterCloseResourceId);
  if (!g_guiBackground) {
    fs::path backgroundPath = fs::path(g_guiLayout.backgroundImage);
    if (backgroundPath.is_relative()) backgroundPath = root / backgroundPath;
    g_guiBackground.reset(gdip::Image::FromFile(backgroundPath.c_str(), FALSE));
    if (!g_guiBackground || g_guiBackground->GetLastStatus() != gdip::Ok) {
      g_guiBackground.reset();
    }
  }
  RebuildGuiBackgroundCache();

  WNDCLASSW wc;
  ZeroMemory(&wc, sizeof(wc));
  wc.lpfnWndProc = UpdaterWndProc;
  wc.hInstance = instance;
  wc.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(kPhoenixUpdaterIconResourceId));
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = NULL;
  wc.lpszClassName = L"ProjectPhoenixUpdaterWindow";
  RegisterClassW(&wc);

  const int screenW = GetSystemMetrics(SM_CXSCREEN);
  const int screenH = GetSystemMetrics(SM_CYSCREEN);
  const int x = std::max(0, (screenW - g_guiLayout.windowWidth) / 2);
  const int y = std::max(0, (screenH - g_guiLayout.windowHeight) / 2);

  HWND hwnd = CreateWindowExW(WS_EX_APPWINDOW, wc.lpszClassName, L"Project Phoenix Updater",
                              WS_POPUP | WS_CLIPCHILDREN,
                              x, y, g_guiLayout.windowWidth, g_guiLayout.windowHeight,
                              NULL, NULL, instance, NULL);
  if (hwnd == NULL) {
    g_guiBackground.reset();
    g_guiCloseButton.reset();
    if (g_gdiplusToken != 0) gdip::GdiplusShutdown(g_gdiplusToken);
    g_gdiplusToken = 0;
    return 98;
  }

  ShowWindow(hwnd, showCommand);
  UpdateWindow(hwnd);
  StartUpdateWorker(root, manifestUrl, config, options);

  MSG msg;
  while (GetMessageW(&msg, NULL, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }
  g_guiBackground.reset();
  g_guiCloseButton.reset();
  g_guiBackgroundCache.reset();
  if (g_gdiplusToken != 0) gdip::GdiplusShutdown(g_gdiplusToken);
  g_gdiplusToken = 0;
  return static_cast<int>(msg.wParam);
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR, int showCommand)
{
  const fs::path root = GetExecutableDir();
  SetCurrentDirectoryW(root.c_str());
  CleanupGeneratedLogDirectories(root);
  DeleteUpdaterLogFile();
  UpdaterLogCleanupGuard updaterLogCleanup;

  std::map<std::string, std::string> config = ReadUpdaterConfig(root);
  UpdaterOptions options;
  options.requireSignature = ReadConfigBool(config, "RequireManifestSignature", true);
  options.launchClient = ReadConfigBool(config, "AutoLaunchClient", true);

  std::wstring manifestUrlW = Utf8ToWide(BuildManifestUrlFromConfig(config));
  ApplyCommandLineOptions(options, manifestUrlW);

  if (options.showGui) {
    UpdaterOptions guiOptions = options;
    guiOptions.launchClient = false;
    return RunGuiUpdater(instance, root, manifestUrlW, config, guiOptions, showCommand);
  }
  return RunUpdate(root, manifestUrlW, config, options);
}
