//////////////////////////////////////////////////////////////////////
//
// Filename    : VirtualFileAccess.h
// Written By  : chyaya@darkeden.com
// Description : Header Of VirtualFileAccess Class
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "VFA_Base.h"


class VirtualFileAccess : public VFA_Base
{
public:
	VirtualFileAccess(VirtualFileIO* vfio);
	virtual ~VirtualFileAccess();

public:
	int		read(char* buf, int len);
	int		write(const char* src, int len);

	int		lseek(long off, int dir);

	void	flush()	{}
};


#ifndef _DEBUG
#define V_FILE_ACCESS_INLINE inline
#include "VirtualFileAccess.inl"
#endif
