//////////////////////////////////////////////////////////////////////
//
// Filename    : VirtualFileAccess.h
// Written By  : chyaya@darkeden.com
// Description : Header Of CVirtualFileAccess Class
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "VFA_Base.h"
#include <vector>

struct CVFHeader
{
	DWORD	CompressSize;
	BYTE	Incompressible;
};

#define CVF_HEADER_SIZE		5

class VirtualFileIO;

class CompressedVirtualFileAccess : public VFA_Base
{
public:
	CompressedVirtualFileAccess(VirtualFileIO* vfio);
	virtual ~CompressedVirtualFileAccess();

	struct PutBuffer {
		char*	buffer;
		DWORD	size;
	};

	typedef std::vector<PutBuffer>	PUTBUFFER_VEC;

public:
	int		read(char* buf, int len);
	int		write(const char* src, int len);

	int		lseek(long off, int dir);

	void	flush();

private:
	char*	m_pGetBuffer;
	DWORD	m_GetPosition;

	PUTBUFFER_VEC	m_PutBufferVec;
	DWORD			m_PutPosition;
	CVFHeader		m_CVFHeader;
};
