//////////////////////////////////////////////////////////////////////
//
// Filename    : VirtaulFileIO.h
// Written By  : chyaya@darkeden.com
// Description : Header Of *FileIO Class
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "VirtualFileSystem.h"
#include "IFileIO.h"

class VFA_Base;

//////////////////////////////////////////////////////////////////////
//
// VirtualFileIO
//
//////////////////////////////////////////////////////////// chyaya //
class VirtualFileIO : public IFileIO
{
friend class VirtualFileAccess;
friend class CompressedVirtualFileAccess;

public:
	VirtualFileIO(VirtualFileSystem* pVFS);
	virtual ~VirtualFileIO();

public:
	void		init();
	void		destory();

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
	CNFS		*m_pNFS;
	NFS_FILEDES m_NFSFiledes;

	DWORD		m_dwGetCount;

	DWORD		m_dwGetPosition;
	DWORD		m_dwPutPosition;

	int			m_nFlags;

	std::string	m_strFileName;

	VirtualFileHeader		m_VFHeader;
	VFA_Base*				m_pVFAccess;
	
// #ifdef _DEBUG
// 	static int	s_nReferenceCount;
// #endif
};


#ifndef _DEBUG
#define V_FILEIO_INLINE inline
#include "VirtualFileIO.inl"
#endif
