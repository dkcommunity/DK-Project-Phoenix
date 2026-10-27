//////////////////////////////////////////////////////////////////////
//
// Filename    : CNFS.h
// Written By  : chyaya@darkeden.com
// Description : class for nfs
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "nFS/nfs.h"

typedef int			NFS_FILEDES;
typedef nfs_Handle*	NFS_HANDLE;

#define NFS_HANDLE_NULL		(NULL)
#define NFS_FILEDES_NULL	(-1)

class ACriticalSection;

//////////////////////////////////////////////////////////////////////
//
// CNFS
//
//////////////////////////////////////////////////////////// chyaya //
class CNFS
{
public:
	CNFS();
	CNFS(char* filename, int flags);
	~CNFS();

public:
	bool			start(char* filename, int flags);
	void			end();

	bool			is_open() const;
	
	int				file_create(const char* path, int mode);
	NFS_FILEDES		file_open(const char* path, int oflag);
	int				file_close(NFS_FILEDES filedes);
	
	int				file_read(NFS_FILEDES filedes, void* buf, int nbyte);
	int				file_write(NFS_FILEDES filedes, const void* buf, int nbyte);

	int				file_lseek(NFS_FILEDES filedes, int offset, int whence);

	int				file_exists(char* path) const;
	int				file_unlink(const char* path);

	int				file_size(NFS_FILEDES filedes) const;
	
	int             glob(const char* pattern, int flags, int (*errfunc)(const char*, int), nfs_glob_t* pglob);
	void			glob_free(nfs_glob_t* pglob);

	void			perror(const char* s) const;

private:
	NFS_HANDLE		m_NFSHandle;

/*	ACriticalSection *m_pCS;*/
};


#ifndef _DEBUG
#define CNFS_INLINE inline
#include "CNFS.inl"
#endif
