//////////////////////////////////////////////////////////////////////
//
// Filename    : FileIO.inl
// Written By  : chyaya@darkeden.com
// Description : Inline Of *FileIO Class
//
//////////////////////////////////////////////////////////////////////

//#ifdef _MT
//#include "ACriticalSection.h"
//#define nfs_auto_lock	ACriticalSection::AutoLock _syncVar = *m_pCS;
//#else
#define nfs_auto_lock
//#endif

CNFS_INLINE CNFS::CNFS()
	: m_NFSHandle(NFS_HANDLE_NULL)
{
// #ifdef _MT
// 	m_pCS = new ACriticalSection;
// #endif
}


CNFS_INLINE CNFS::CNFS(char* filename, int flags)
{
// #ifdef _MT
// 	m_pCS = new ACriticalSection;
// #endif

	start(filename, flags);
}


CNFS_INLINE CNFS::~CNFS()
{
	end();

// #ifdef _MT
// 	delete m_pCS;
// #endif
}


CNFS_INLINE bool CNFS::start(char* filename, int flags)
{
	nfs_auto_lock;
	m_NFSHandle = nfs_start(filename, flags);
	return m_NFSHandle != NFS_HANDLE_NULL;
}


CNFS_INLINE void CNFS::end()
{
	nfs_auto_lock;
	if(m_NFSHandle)
	{
		nfs_end(m_NFSHandle, 0);
		m_NFSHandle = NFS_HANDLE_NULL;
	}
}


CNFS_INLINE bool CNFS::is_open() const
{
	nfs_auto_lock;
	return m_NFSHandle != NFS_HANDLE_NULL;
}


CNFS_INLINE int CNFS::file_create(const char* path, int mode)
{
	nfs_auto_lock;
	return nfs_file_create(m_NFSHandle, path, mode);
}


CNFS_INLINE NFS_FILEDES CNFS::file_open(const char* path, int oflag)
{
	nfs_auto_lock;
	return nfs_file_open(m_NFSHandle, path, oflag);
}


CNFS_INLINE NFS_FILEDES CNFS::file_close(int filedes)
{
	nfs_auto_lock;
	return nfs_file_close(m_NFSHandle, filedes);
}


CNFS_INLINE int CNFS::file_read(int filedes, void* buf, int nbyte)
{
	nfs_auto_lock;
	return nfs_file_read(m_NFSHandle, filedes, buf, nbyte);
}


CNFS_INLINE int CNFS::file_write(int filedes, const void* buf, int nbyte)
{
	nfs_auto_lock;
	return nfs_file_write(m_NFSHandle, filedes, buf, nbyte);
}


CNFS_INLINE int	CNFS::file_lseek(int filedes, int offset, int whence)
{
	nfs_auto_lock;
	return nfs_file_lseek(m_NFSHandle, filedes, offset, whence);
}


CNFS_INLINE int CNFS::file_exists(char* path) const
{
	nfs_auto_lock;
	return nfs_file_exists(m_NFSHandle, path);
}


CNFS_INLINE int CNFS::file_unlink(const char* path)
{
	nfs_auto_lock;
	return nfs_file_unlink(m_NFSHandle, path);
}


CNFS_INLINE int	CNFS::file_size(NFS_FILEDES filedes) const
{
	nfs_auto_lock;
	return nfs_nt_node_get_size(m_NFSHandle->nt, m_NFSHandle->handles[filedes]->nt_index);
}

CNFS_INLINE int	CNFS::glob(const char* pattern, int flags, int (*errfunc)(const char*, int), nfs_glob_t* pglob)
{
	nfs_auto_lock;
	return nfs_glob(m_NFSHandle, pattern, flags, errfunc, pglob);
}

CNFS_INLINE void CNFS::glob_free(nfs_glob_t* pglob)
{
	nfs_auto_lock;
	nfs_glob_free(m_NFSHandle, pglob);
}

CNFS_INLINE void CNFS::perror(const char* s) const
{
	nfs_auto_lock;
	nfs_perror(m_NFSHandle, s);
}

#undef nfs_auto_lock