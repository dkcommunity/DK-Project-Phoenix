#pragma once

#include <stddef.h>
#include <vector>
#include <string>

// Phoenix RPK bridge — RARLAB UnRAR 7.x (RAR5 + legacy RAR) bellek icinden extract.
// UnRAR source code may be used in any software to handle RAR archives without
// limitations free of charge, but cannot be used to develop a RAR (WinRAR)
// compatible archiver. See thirdparty/unrar/unrar/license.txt.

bool PhoenixUnrarExtractMember(
    const char *archivePath,
    const char *memberName,
    const char *password,
    char **outData,
    int *outSize);

bool PhoenixUnrarListMembers(
    const char *archivePath,
    const char *password,
    const char *nameFilter,
    std::vector<std::string> *outNames);
