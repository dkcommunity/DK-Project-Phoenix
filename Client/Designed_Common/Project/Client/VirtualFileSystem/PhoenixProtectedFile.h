#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace PhoenixProtectedFile
{
	const char* Extension();

	bool IsInfPath(const char* path);
	bool BuildProtectedPath(const char* sourcePath, std::string& protectedPath);

	bool Load(const char* protectedPath, std::vector<char>& outData);
	bool LoadWithOriginalPath(const char* protectedPath, const char* originalPath, std::vector<char>& outData);
	bool Save(const char* originalPath, const char* data, size_t dataSize, const char* protectedPath);
}
