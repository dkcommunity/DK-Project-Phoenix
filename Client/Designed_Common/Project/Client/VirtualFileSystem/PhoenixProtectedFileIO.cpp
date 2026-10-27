#include "VFS_Precompile.h"

#include "PhoenixProtectedFileIO.h"
#include "PhoenixProtectedFile.h"

#include <algorithm>
#include <cstring>

PhoenixProtectedFileIO::PhoenixProtectedFileIO()
	: m_Position(0)
	, m_LastCount(0)
	, m_Open(false)
{
}

PhoenixProtectedFileIO::~PhoenixProtectedFileIO()
{
	close();
}

void PhoenixProtectedFileIO::open(LPCTSTR szName, int nMode)
{
	openWithOriginalPath(szName, szName, nMode);
}

void PhoenixProtectedFileIO::openWithOriginalPath(LPCTSTR protectedPath, LPCTSTR originalPath, int nMode)
{
	close();

	if(!(nMode & std::ios_base::in))
		return;

	m_Open = PhoenixProtectedFile::LoadWithOriginalPath(
		protectedPath,
		originalPath ? originalPath : protectedPath,
		m_Buffer);
	m_Position = 0;
	m_LastCount = 0;
}

void PhoenixProtectedFileIO::close()
{
	m_Buffer.clear();
	m_Position = 0;
	m_LastCount = 0;
	m_Open = false;
}

bool PhoenixProtectedFileIO::is_open() const
{
	return m_Open;
}

void PhoenixProtectedFileIO::read(char* buf, int len)
{
	m_LastCount = 0;

	if(!m_Open || !buf || len <= 0)
		return;

	const size_t remain = m_Position < m_Buffer.size() ? m_Buffer.size() - m_Position : 0;
	const size_t requested = static_cast<size_t>(len);
	const size_t readCount = remain < requested ? remain : requested;

	if(readCount > 0)
	{
		memcpy(buf, &m_Buffer[m_Position], readCount);
		m_Position += readCount;
		m_LastCount = static_cast<int>(readCount);
	}
}

int PhoenixProtectedFileIO::gcount() const
{
	return m_LastCount;
}

void PhoenixProtectedFileIO::seekg(long off, int dir)
{
	if(!m_Open)
		return;

	long base = 0;
	switch(static_cast<std::ios_base::seekdir>(dir))
	{
	case std::ios_base::cur:
		base = static_cast<long>(m_Position);
		break;
	case std::ios_base::end:
		base = static_cast<long>(m_Buffer.size());
		break;
	case std::ios_base::beg:
	default:
		base = 0;
		break;
	}

	long next = base + off;
	if(next < 0)
		next = 0;
	if(static_cast<size_t>(next) > m_Buffer.size())
		next = static_cast<long>(m_Buffer.size());

	m_Position = static_cast<size_t>(next);
	m_LastCount = 0;
}

int PhoenixProtectedFileIO::tellg()
{
	return static_cast<int>(m_Position);
}

void PhoenixProtectedFileIO::write(const char* src, int len)
{
	(void)src;
	(void)len;
}

bool PhoenixProtectedFileIO::eof() const
{
	return !m_Open || m_Position >= m_Buffer.size();
}

void PhoenixProtectedFileIO::get(char& ch)
{
	m_LastCount = 0;
	if(!m_Open || m_Position >= m_Buffer.size())
		return;

	ch = m_Buffer[m_Position++];
	m_LastCount = 1;
}

void* PhoenixProtectedFileIO::getline(char* psch, int nCount, char delim)
{
	m_LastCount = 0;

	if(!m_Open || !psch || nCount <= 0)
		return NULL;
	if(m_Position >= m_Buffer.size())
	{
		psch[0] = '\0';
		return NULL;
	}

	int written = 0;
	while(written < nCount - 1 && m_Position < m_Buffer.size())
	{
		char ch = m_Buffer[m_Position++];
		if(ch == delim)
			break;

		psch[written++] = ch;
	}

	psch[written] = '\0';
	m_LastCount = written;
	return psch;
}
