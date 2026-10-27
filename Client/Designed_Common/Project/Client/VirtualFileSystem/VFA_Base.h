//////////////////////////////////////////////////////////////////////
//
// Filename    : VirtualFileAccess.h
// Written By  : chyaya@darkeden.com
// Description : Header Of CVirtualFileAccess Class
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "VirtualFileSystem.h"
#include "VirtualFileIO.h"

class VFA_Base
{
public:
	VFA_Base(VirtualFileIO* vfio) : m_pVirtualFileIO(vfio) {}
	virtual ~VFA_Base() {}

public:
	virtual int	read(char* buf, int len) = 0;
	virtual int	write(const char* src, int len) = 0;

	virtual int	lseek(long off, int dir) = 0;

	virtual void flush() = 0;

protected:
	VirtualFileIO* m_pVirtualFileIO;
};
