#include "UnRarBridge.h"

#include <windows.h>

#include "dll.hpp"

#include <cctype>
#include <cstring>
#include <cstdlib>

namespace {

struct ExtractContext {
  const char *password;
  std::vector<char> buffer;
};

// urarlib stricomp ile ayni: \ ve / yok sayilir, buyuk/kucuk harf duyarsiz.
bool RpkNamesMatch(const char *requested, const char *archiveMember) {
  if (!requested || !archiveMember)
    return false;

  char left[512];
  char right[512];
  strncpy(left, requested, sizeof(left) - 1);
  left[sizeof(left) - 1] = '\0';
  strncpy(right, archiveMember, sizeof(right) - 1);
  right[sizeof(right) - 1] = '\0';

  for (char *p = left; (p = strchr(p, '\\')) != nullptr; ++p)
    *p = '_';
  for (char *p = right; (p = strchr(p, '\\')) != nullptr; ++p)
    *p = '_';
  for (char *p = left; (p = strchr(p, '/')) != nullptr; ++p)
    *p = '_';
  for (char *p = right; (p = strchr(p, '/')) != nullptr; ++p)
    *p = '_';

  for (char *p = left; *p; ++p)
    *p = static_cast<char>(toupper(static_cast<unsigned char>(*p)));
  for (char *p = right; *p; ++p)
    *p = static_cast<char>(toupper(static_cast<unsigned char>(*p)));

  return strcmp(left, right) == 0;
}

int CALLBACK UnrarBridgeCallback(UINT msg, LPARAM userData, LPARAM p1, LPARAM p2) {
  auto *ctx = reinterpret_cast<ExtractContext *>(userData);
  if (msg == UCM_PROCESSDATA) {
    const auto *bytes = reinterpret_cast<const unsigned char *>(p1);
    const int size = static_cast<int>(p2);
    if (bytes != nullptr && size > 0)
      ctx->buffer.insert(ctx->buffer.end(), bytes, bytes + size);
    return 1;
  }
  if (msg == UCM_NEEDPASSWORD) {
    if (ctx->password == nullptr || ctx->password[0] == '\0')
      return -1;
    strncpy(reinterpret_cast<char *>(p1), ctx->password, static_cast<size_t>(p2) - 1);
    reinterpret_cast<char *>(p1)[static_cast<size_t>(p2) - 1] = '\0';
    return 1;
  }
  return 1;
}

bool CopyBufferToMalloc(const std::vector<char> &src, char **outData, int *outSize) {
  if (outData == nullptr || outSize == nullptr)
    return false;

  *outData = nullptr;
  *outSize = 0;

  if (src.empty())
    return false;

  char *buffer = static_cast<char *>(malloc(src.size() + 1));
  if (buffer == nullptr)
    return false;

  memcpy(buffer, src.data(), src.size());
  buffer[src.size()] = '\0';
  *outData = buffer;
  *outSize = static_cast<int>(src.size());
  return true;
}

} // namespace

bool PhoenixUnrarExtractMember(
    const char *archivePath,
    const char *memberName,
    const char *password,
    char **outData,
    int *outSize) {
  if (archivePath == nullptr || archivePath[0] == '\0' ||
      memberName == nullptr || memberName[0] == '\0' ||
      outData == nullptr || outSize == nullptr) {
    return false;
  }

  *outData = nullptr;
  *outSize = 0;

  RAROpenArchiveDataEx openData{};
  openData.ArcName = const_cast<char *>(archivePath);
  openData.OpenMode = RAR_OM_EXTRACT;

  ExtractContext ctx{};
  ctx.password = password;
  openData.Callback = UnrarBridgeCallback;
  openData.UserData = reinterpret_cast<LPARAM>(&ctx);

  HANDLE archive = RAROpenArchiveEx(&openData);
  if (archive == nullptr || openData.OpenResult != ERAR_SUCCESS)
    return false;

  if (password != nullptr && password[0] != '\0')
    RARSetPassword(archive, const_cast<char *>(password));

  bool found = false;
  RARHeaderDataEx header{};

  while (true) {
    const int readCode = RARReadHeaderEx(archive, &header);
    if (readCode == ERAR_END_ARCHIVE)
      break;
    if (readCode != ERAR_SUCCESS)
      break;

    if ((header.Flags & RHDF_DIRECTORY) != 0) {
      if (RARProcessFile(archive, RAR_SKIP, nullptr, nullptr) != ERAR_SUCCESS)
        break;
      continue;
    }

    if (!RpkNamesMatch(memberName, header.FileName)) {
      if (RARProcessFile(archive, RAR_SKIP, nullptr, nullptr) != ERAR_SUCCESS)
        break;
      continue;
    }

    ctx.buffer.clear();
    // RAR_EXTRACT + DestPath=NULL CWD'ye (Client.exe yani) uye yaziyordu.
    // UCM_PROCESSDATA zaten bellege kopyaliyor; RAR_TEST ayni unpack + callback,
    // disk yok. CRarFile disk bypass loose dosyayi tercih ederdi — artik
    // sadece RPK'dan okunur.
    const int processCode =
        RARProcessFile(archive, RAR_TEST, nullptr, nullptr);
    if (processCode == ERAR_SUCCESS && !ctx.buffer.empty())
      found = CopyBufferToMalloc(ctx.buffer, outData, outSize);
    break;
  }

  RARCloseArchive(archive);
  return found;
}

bool PhoenixUnrarListMembers(
    const char *archivePath,
    const char *password,
    const char *nameFilter,
    std::vector<std::string> *outNames) {
  if (archivePath == nullptr || archivePath[0] == '\0' || outNames == nullptr)
    return false;

  outNames->clear();

  RAROpenArchiveDataEx openData{};
  openData.ArcName = const_cast<char *>(archivePath);
  openData.OpenMode = RAR_OM_LIST;

  HANDLE archive = RAROpenArchiveEx(&openData);
  if (archive == nullptr || openData.OpenResult != ERAR_SUCCESS)
    return false;

  if (password != nullptr && password[0] != '\0')
    RARSetPassword(archive, const_cast<char *>(password));

  RARHeaderDataEx header{};
  while (true) {
    const int readCode = RARReadHeaderEx(archive, &header);
    if (readCode == ERAR_END_ARCHIVE)
      break;
    if (readCode != ERAR_SUCCESS)
      break;

    if ((header.Flags & RHDF_DIRECTORY) == 0) {
      if (nameFilter == nullptr || strstr(header.FileName, nameFilter) != nullptr)
        outNames->push_back(header.FileName);
    }

    if (RARProcessFile(archive, RAR_SKIP, nullptr, nullptr) != ERAR_SUCCESS)
      break;
  }

  RARCloseArchive(archive);
  return !outNames->empty();
}
