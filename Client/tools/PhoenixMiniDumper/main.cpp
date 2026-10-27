// PhoenixMiniDumper — Client crash dump'ini AYRI process'ten yazar.
// In-process MiniDumpWriteDump heap kilidi yüzünden 0 bayt dump üretiyordu.
//
// Kullanım:
//   PhoenixMiniDumper.exe --version
//   PhoenixMiniDumper.exe <pid> <dumpPath> [inheritedProcessHandle] [--protocol N]
// inheritedProcessHandle: Client'in DuplicateHandle ile verdiği (opsiyonel).

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <DbgHelp.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#pragma comment(lib, "Dbghelp.lib")

static const unsigned kPhoenixMiniDumperProtocolVersion = 2U;

static MINIDUMP_TYPE Phoenix_GetPreferredMiniDumpType() {
  // Stack + PDB icin yeterli; FullMemory (GB) uretmez.
  return static_cast<MINIDUMP_TYPE>(MiniDumpNormal |
                                    MiniDumpWithIndirectlyReferencedMemory);
}

static bool Phoenix_WantFullMemoryDump() {
  char buf[8] = {};
  return GetEnvironmentVariableA("PHOENIX_FULL_MEMORY_DUMP", buf, sizeof(buf)) >
             0 &&
         buf[0] == '1';
}

static int Fail(const char *msg, const DWORD err = GetLastError()) {
  std::fprintf(stderr, "PhoenixMiniDumper: %s (err=%lu)\n", msg,
               static_cast<unsigned long>(err));
  return 1;
}

static const char *Phoenix_MiniDumpTypeName(MINIDUMP_TYPE type) {
  if (type == MiniDumpNormal)
    return "MiniDumpNormal";
  if (type == MiniDumpWithDataSegs)
    return "MiniDumpWithDataSegs";
  if (type == MiniDumpWithFullMemory)
    return "MiniDumpWithFullMemory";
  if (type ==
      static_cast<MINIDUMP_TYPE>(MiniDumpNormal |
                                 MiniDumpWithIndirectlyReferencedMemory))
    return "MiniDumpNormal|MiniDumpWithIndirectlyReferencedMemory";
  return "MiniDumpType";
}

static unsigned Phoenix_ParseProtocolVersion(int argc, char **argv) {
  for (int i = 1; i + 1 < argc; ++i) {
    if (std::strcmp(argv[i], "--protocol") == 0) {
      char *end = nullptr;
      const unsigned long value = std::strtoul(argv[i + 1], &end, 10);
      if (end != argv[i + 1] && *end == '\0' && value != 0UL)
        return static_cast<unsigned>(value);
    }
  }
  return 0U;
}

int main(int argc, char **argv) {
  if (argc >= 2 && std::strcmp(argv[1], "--version") == 0) {
    std::printf("%u\n", kPhoenixMiniDumperProtocolVersion);
    return 0;
  }

  if (argc < 3)
    return Fail(
        "usage: PhoenixMiniDumper.exe <pid> <dumpPath> [processHandle] "
        "[--protocol N]",
        0);

  const unsigned protocol = Phoenix_ParseProtocolVersion(argc, argv);
  if (protocol != 0U && protocol != kPhoenixMiniDumperProtocolVersion) {
    return Fail("unsupported --protocol version", 0);
  }

  char *end = nullptr;
  const unsigned long pid = std::strtoul(argv[1], &end, 10);
  if (end == argv[1] || *end != '\0' || pid == 0UL)
    return Fail("invalid pid", 0);

  const char *dumpPath = argv[2];
  HANDLE hProcess = NULL;

  if (argc >= 4 && argv[3][0] != '\0' && std::strcmp(argv[3], "--protocol") != 0) {
    char *hend = nullptr;
    const unsigned long handleValue = std::strtoul(argv[3], &hend, 10);
    if (hend != argv[3] && *hend == '\0' && handleValue != 0UL)
      hProcess = reinterpret_cast<HANDLE>(handleValue);
  }

  if (hProcess == NULL) {
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ |
                               PROCESS_DUP_HANDLE | PROCESS_SUSPEND_RESUME,
                           FALSE, static_cast<DWORD>(pid));
  }
  if (hProcess == NULL) {
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, static_cast<DWORD>(pid));
  }
  if (hProcess == NULL)
    return Fail("OpenProcess failed");

  HANDLE hFile =
      CreateFileA(dumpPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,
                  FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    CloseHandle(hProcess);
    return Fail("CreateFile dump failed");
  }

  MINIDUMP_TYPE types[4] = {
      Phoenix_GetPreferredMiniDumpType(),
      MiniDumpNormal,
      MiniDumpWithDataSegs,
      static_cast<MINIDUMP_TYPE>(0),
  };
  int typeCount = 3;
  if (Phoenix_WantFullMemoryDump()) {
    types[3] = MiniDumpWithFullMemory;
    typeCount = 4;
  }

  BOOL ok = FALSE;
  DWORD lastErr = 0;
  MINIDUMP_TYPE usedType = static_cast<MINIDUMP_TYPE>(0);
  for (int i = 0; i < typeCount; ++i) {
    const MINIDUMP_TYPE type = types[i];
    if (type == static_cast<MINIDUMP_TYPE>(0))
      continue;
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    SetEndOfFile(hFile);
    SetLastError(0);
    ok = MiniDumpWriteDump(hProcess, static_cast<DWORD>(pid), hFile, type, NULL,
                           NULL, NULL);
    lastErr = ok ? 0 : GetLastError();
    if (ok) {
      usedType = type;
      break;
    }
  }

  FlushFileBuffers(hFile);
  const DWORD size = GetFileSize(hFile, NULL);
  CloseHandle(hFile);
  CloseHandle(hProcess);

  if (!ok || size == INVALID_FILE_SIZE || size < 1024U) {
    DeleteFileA(dumpPath);
    return Fail("MiniDumpWriteDump failed or dump too small", lastErr);
  }

  std::fprintf(stderr, "PhoenixMiniDumper: protocol=%u type=%s size=%lu bytes\n",
               kPhoenixMiniDumperProtocolVersion,
               Phoenix_MiniDumpTypeName(usedType),
               static_cast<unsigned long>(size));
  std::printf("PhoenixMiniDumper: wrote %lu bytes to %s\n",
              static_cast<unsigned long>(size), dumpPath);
  return 0;
}
