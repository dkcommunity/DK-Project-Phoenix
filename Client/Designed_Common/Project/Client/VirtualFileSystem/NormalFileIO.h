//////////////////////////////////////////////////////////////////////
//
// Filename    : FileIO.h
// Written By  : chyaya@darkeden.com
// Description : Header Of NormalFileIO Class
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "IFileIO.h"

//////////////////////////////////////////////////////////////////////
//
// NormalFileIO
//
// : �Ϲ����� ���� ����� �������̽��̴�.
//
//////////////////////////////////////////////////////////// chyaya //
class NormalFileIO : public IFileIO
{
public:
	NormalFileIO();
	virtual ~NormalFileIO();

public:
	void		open(LPCTSTR szName, int nMode);
	void		close();

	bool		is_open() const;

	void		read(char* buf, int len);
	
	int			gcount() const;

	void		seekg(long off, int dir = std::ios_base::beg);
	int			tellg();

	void		write(const char* src, int len);

// 	void		seekp(long off, int dir = std::ios_base::beg);
// 	int			tellp();

	bool		eof() const;

	void		get(char& ch);
	void*		getline(char* psch, int nCount, char delim = '\n');

private:
	mutable std::fstream m_NormalFile;
};


#ifndef _DEBUG
#define N_FILEIO_INLINE inline
#include "NormalFileIO.inl"
#endif
