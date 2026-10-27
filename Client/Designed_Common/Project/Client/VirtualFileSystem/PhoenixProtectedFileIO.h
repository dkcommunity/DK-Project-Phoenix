#pragma once

#include "IFileIO.h"

#include <vector>

class PhoenixProtectedFileIO : public IFileIO
{
public:
	PhoenixProtectedFileIO();
	virtual ~PhoenixProtectedFileIO();

	void open(LPCTSTR szName, int nMode);
	void openWithOriginalPath(LPCTSTR protectedPath, LPCTSTR originalPath, int nMode);
	void close();

	bool is_open() const;

	void read(char* buf, int len);
	int gcount() const;

	void seekg(long off, int dir = std::ios_base::beg);
	int tellg();

	void write(const char* src, int len);

	bool eof() const;

	void get(char& ch);
	void* getline(char* psch, int nCount, char delim = '\n');

private:
	std::vector<char> m_Buffer;
	size_t m_Position;
	int m_LastCount;
	bool m_Open;
};
