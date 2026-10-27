//////////////////////////////////////////////////////////////////////
//
// Filename    : VirtualFileIO.cpp
// Written By  : chyaya@darkeden.com
// Description : Source Of VirtualFileIO Class
//
//////////////////////////////////////////////////////////////////////

#include "VFS_Precompile.h"

#include "VirtualFileSystem.h"
#include "VirtualFileIO.h"

#include "VFA_Base.h"
#include "VirtualFileAccess.h"
#include "CompressVirtualFileAccess.h"

#ifdef _DEBUG
#define V_FILEIO_INLINE
#include "VirtualFileIO.inl"
#endif

// #ifdef _DEBUG
// int VirtualFileIO::s_nReferenceCount = 0;
// #endif

VirtualFileIO::VirtualFileIO(VirtualFileSystem* pVFS)
	: m_pNFS(pVFS->GetNFS())
{
	
// #ifdef _DEBUG
// 	++s_nReferenceCount;
// 	char szBuf[128];
// 	sprintf(szBuf, "Create VFileIO: RefCount = %d\n", s_nReferenceCount);
// 	OutputDebugString(szBuf);
// #endif

	init();
}


void VirtualFileIO::init()
{
	m_pVFAccess		= NULL;
	m_NFSFiledes	= NFS_FILEDES_NULL;

	m_dwGetCount	= 0;

	m_dwGetPosition	= 0;
	m_dwPutPosition	= 0;

	m_VFHeader.Init();
}


void VirtualFileIO::destory()
{
	delete m_pVFAccess;
	init();
}


void VirtualFileIO::open(LPCTSTR szName, int nMode)
{
	assert(m_pVFAccess == NULL);
	assert(strlen(szName) < nfs_NAME_LEN);

	close();

	if(!m_pNFS->is_open()) return;

	m_strFileName = szName;

	int nNFSFlags = 0;
	
	// c++ fstream ���� �ɼ��� filedes ���� �ɼ����� ����
	BOOL bRead		= nMode & std::ios_base::in;
	BOOL bWrite		= nMode & std::ios_base::out;
	BOOL bAppend	= nMode & std::ios_base::app;
	BOOL bBinary	= nMode & std::ios_base::binary;
	BOOL bCompress	= nMode & iovs_ex::compress;
	
	if(bRead && bWrite)	nNFSFlags |= O_RDWR;
	else if(bRead)		nNFSFlags |= O_RDONLY;
	else if(bWrite)		nNFSFlags |= O_WRONLY;
	
	// O_APPEND�� ���������� ó�����ֹǷ� �ɼǿ��� �����Ѵ�.
	//if(bAppend)		nNFSFlags |= O_APPEND;

	if(bBinary)			nNFSFlags |= O_BINARY;

	m_nFlags = nNFSFlags;

	// ���� Append ���� ������
	assert(bAppend == false);

	// nfs file�� ����.
	if(bRead)
	{
		// VF�� ����. 
  		m_NFSFiledes = m_pNFS->file_open(szName, nNFSFlags);
		//assert("nfs file open error" && m_NFSFiledes >= 0);

		if(m_NFSFiledes < 0) return;

		int rd = m_pNFS->file_read(m_NFSFiledes, &m_VFHeader, VF_HEADER_SIZE);
		assert("virtual file header read error" && rd == VF_HEADER_SIZE);

		bCompress = m_VFHeader.IsCompressed;
	}
	else
	{
		int bFileExists = m_pNFS->file_exists(const_cast<char*>(szName));

		m_VFHeader.Init();
		m_VFHeader.IsCompressed = bCompress ? 1 : 0;

		// ������ �����ϰ� Append��� �� ���� ������ ������ ����
		if(bFileExists && bAppend)
		{
			m_NFSFiledes = m_pNFS->file_open(szName, nNFSFlags);
			assert("nfs file open error" && m_NFSFiledes >= 0);

			m_pNFS->file_lseek(m_NFSFiledes, VF_HEADER_SIZE, SEEK_CUR);	
		}
		else
		{
			// Append�� �ƴϰų� ���� ���� �����ϴ� ��� ������ ������ �ִٸ� ����
			if(bFileExists)
			{
				if(m_pNFS->file_unlink(szName) < 0)
				{
					assert("nfs file unlink error" && FALSE);
//					return;
				}
			}

			m_NFSFiledes = m_pNFS->file_create(szName, nNFSFlags);
			assert("nfs file create error" && m_NFSFiledes >= 0);

			m_pNFS->file_write(m_NFSFiledes, &m_VFHeader, VF_HEADER_SIZE);
		}
	}

	// ���� ���ο� ���� File Access ����� �б�
	if(m_VFHeader.IsCompressed)
	{
		m_pVFAccess = new CompressedVirtualFileAccess(this);
	}
	else
	{
		m_pVFAccess = new VirtualFileAccess(this);
	}

	if(bAppend)
	{
		m_pVFAccess->lseek(0, SEEK_END);
	}

// #ifdef _DEBUG
// 	char szBuf[256];
// 	sprintf(szBuf, "Open VFileIO: RefCount = %d, FileName = %s\n", s_nReferenceCount, szName);
// 	OutputDebugString(szBuf);
// #endif
}


void VirtualFileIO::close()
{
	if(m_pNFS->is_open() && m_NFSFiledes != NFS_FILEDES_NULL)
	{
		// VFAccess�� ���۸� ����.
		m_pVFAccess->flush();

		// ��� ���̿��ٸ� ������ ���ۿ� VFHeader�� ����.
		if(m_nFlags & O_RDWR || m_nFlags & O_WRONLY)
		{
			m_VFHeader.Version	= VFS_VERSION;
			m_VFHeader.FileSize	= m_dwPutPosition;

			m_pNFS->file_lseek(m_NFSFiledes, 0, SEEK_SET);
			int wr = m_pNFS->file_write(m_NFSFiledes, &m_VFHeader, VF_HEADER_SIZE);
			assert(wr == VF_HEADER_SIZE);
		}

		m_pNFS->file_close(m_NFSFiledes);
	}

	destory();
}


void VirtualFileIO::read(char* buf, int len)
{
	if(m_dwGetPosition + len > m_VFHeader.FileSize)
		len = m_VFHeader.FileSize - m_dwGetPosition;

	m_dwGetCount = m_pVFAccess->read(buf, len);
	assert(m_dwGetCount >= 0);

	m_dwGetPosition += m_dwGetCount;
}


void VirtualFileIO::write(const char* src, int len)
{
	int PutCount = m_pVFAccess->write(src, len);
	assert(PutCount == len);

	m_dwPutPosition += PutCount;
}


void VirtualFileIO::seekg(long off, int dir)
{
	switch(dir)	{
	case std::ios_base::beg: dir = SEEK_SET; break;
	case std::ios_base::cur: dir = SEEK_CUR; break;
	case std::ios_base::end: dir = SEEK_END; break;
	}

// 	if(dir == SEEK_SET)
// 		off += VF_HEADER_SIZE;
	
	m_dwGetPosition = m_pVFAccess->lseek(off, dir);
}


int VirtualFileIO::tellg()
{
	return m_dwGetPosition;
}


// void VirtualFileIO::seekp(long off, int dir)
// {
// 	switch(dir)	{
// 	case std::ios_base::beg: dir = SEEK_SET; break;
// 	case std::ios_base::cur: dir = SEEK_CUR; break;
// 	case std::ios_base::end: dir = SEEK_END; break;
// 	}
// 
// 	if(dir == SEEK_SET)
// 		off += VF_HEADER_SIZE;
// 
// 	m_pVFAccess->lseek(off, dir);
// }
// 
// 
// int VirtualFileIO::tellp()
// {
// 	return m_nPutPosition;
// }


bool VirtualFileIO::eof() const
{
	/*
	char ch;
	if(!m_pVFAccess->read(&ch, 1)) return true;
	m_pVFAccess->lseek(-1, SEEK_CUR);
	return ch == EOF;
	*/
	return m_dwGetPosition == m_VFHeader.FileSize;
}


void VirtualFileIO::get(char& ch)
{
	read(&ch, 1);
}


void* VirtualFileIO::getline(char* psch, int nCount, char delim)
{
	void* rtn	= this;
	char* p		= psch;

	if(psch == NULL || nCount <= 0)
		return NULL;

	for(int i = 0; i < nCount - 1; ++i)
	{
		char ch;
		read(&ch, 1);
		if(!gcount())
		{
			rtn = NULL;
			break;
		}

		if(ch == '\r') continue;
		else if(ch == delim)
		{
			//if(i) m_pVFAccess->lseek(-1, SEEK_CUR);
			break;
		}

		*p++ = ch;
	}

	*p = '\0';

	return rtn;
}


