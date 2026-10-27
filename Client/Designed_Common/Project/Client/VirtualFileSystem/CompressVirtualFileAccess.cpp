//////////////////////////////////////////////////////////////////////
//
// Filename    : VirtualFileAccess.cpp
// Written By  : chyaya@darkeden.com
// Description : Source Of VirtualFileAccess Class
//
//////////////////////////////////////////////////////////////////////

#include "VFS_Precompile.h"
#include "VFS_Macro.h"
#include "CompressVirtualFileAccess.h"

// ���� �˰����� ����
//#define COMPRESS_ALGORITHM_LZO

#ifdef COMPRESS_ALGORITHM_LZO
	#include "MMiniLZO.h"
#else
extern "C" {
	#include "MZLib/zlib.h"
}
#endif

#undef max

CompressedVirtualFileAccess::CompressedVirtualFileAccess(VirtualFileIO* vfio)
	: VFA_Base(vfio), m_pGetBuffer(NULL), m_GetPosition(0), m_PutPosition(0)  
{
	int flags = vfio->m_nFlags;
		
	if((flags & O_WRONLY) == false)
	{
		DWORD readSize;

		vfio->m_pNFS->file_lseek(vfio->m_NFSFiledes,
			VF_HEADER_SIZE, SEEK_SET);

		readSize = vfio->m_pNFS->file_read(vfio->m_NFSFiledes,
			&m_CVFHeader, CVF_HEADER_SIZE);

		assert(readSize == CVF_HEADER_SIZE);
		assert(m_CVFHeader.Incompressible == 0 || m_CVFHeader.Incompressible == 1);
		assert(m_CVFHeader.CompressSize >= 0);

		m_pGetBuffer = new char[vfio->m_VFHeader.FileSize];

		if(m_CVFHeader.Incompressible)
		{
			readSize = vfio->m_pNFS->file_read(vfio->m_NFSFiledes,
				m_pGetBuffer, m_CVFHeader.CompressSize);

			assert(readSize == m_CVFHeader.CompressSize);
		}
		else
		{
			char* pCompressBuf = new char[m_CVFHeader.CompressSize];

// 			char* pReadPos		= pCompressBuf;
// 			//register DWORD readOnce		= 524288;
// 			register DWORD readOnce		= 10240;
// 			register DWORD readRemain	= m_CVFHeader.CompressSize;
// 			register DWORD readSize;
// 
// 			while(readRemain > readOnce)
// 			{
// 				readSize = vfio->m_pNFS->file_read(
// 					vfio->m_NFSFiledes, pReadPos, readOnce);
// 				readRemain	-= readSize;
// 				pReadPos	+= readSize; 
// 			}
// 
// 			readRemain -= vfio->m_pNFS->file_read(
// 					vfio->m_NFSFiledes, pReadPos, readRemain);
// 
// 			assert(readRemain == 0);

			readSize = vfio->m_pNFS->file_read(vfio->m_NFSFiledes,
				pCompressBuf, m_CVFHeader.CompressSize);
			assert(readSize == m_CVFHeader.CompressSize);
			
			// ��ȣȭ
			decode_data(pCompressBuf, m_CVFHeader.CompressSize,
				(int)hash_func_path_string_stlport()(m_pVirtualFileIO->m_strFileName.c_str()));

			DWORD	uncompressSize = m_pVirtualFileIO->m_VFHeader.FileSize;
			int		result;	
			
#ifdef COMPRESS_ALGORITHM_LZO
			result = MMiniLZO::Decompress(
				(lzo_bytep)pCompressBuf, m_CVFHeader.CompressSize,
				(lzo_bytep)m_pGetBuffer, &uncompressSize);
			assert(result == LZO_E_OK);
#else
			result = uncompress(
				(Bytef*)m_pGetBuffer, &uncompressSize,
				(Bytef*)pCompressBuf, m_CVFHeader.CompressSize);
			assert(result == Z_OK);
#endif	

			delete [] pCompressBuf;
		}
	}
}


CompressedVirtualFileAccess::~CompressedVirtualFileAccess()
{
	// Delete Put Buffer
	PUTBUFFER_VEC::iterator delpos = m_PutBufferVec.begin();
	
	for(; delpos != m_PutBufferVec.end(); ++delpos)
		delete [] delpos->buffer;
	
	m_PutBufferVec.clear();
	
	m_PutPosition = 0;
	
	// Delete Get Buffer
	delete [] m_pGetBuffer;
	m_pGetBuffer = NULL;
	
	m_GetPosition = 0;
}


int CompressedVirtualFileAccess::read(char* buf, int len)
{
	assert(m_pGetBuffer);

	int bufferRemain	= m_pVirtualFileIO->m_VFHeader.FileSize - m_GetPosition;
	int readCount		= min(len, bufferRemain);
	
	memcpy(buf, m_pGetBuffer + m_GetPosition, readCount);

	m_GetPosition += readCount;

	return readCount;
}


int CompressedVirtualFileAccess::write(const char* src, int len)
{
	PutBuffer putBuffer;

	putBuffer.buffer	= new char[len];
	putBuffer.size		= len;

	memcpy(putBuffer.buffer, src, len);

	m_PutPosition += len;

	m_PutBufferVec.push_back(putBuffer);

	return len;
}


int CompressedVirtualFileAccess::lseek(long off, int dir)
{
	assert("CompressedVirtualFileAccess::lseek() : invalid direction" &&
		(dir == SEEK_SET || dir == SEEK_CUR || dir == SEEK_END));

	DWORD fileSize = m_pVirtualFileIO->m_VFHeader.FileSize;

	switch(dir)
	{
	case SEEK_CUR:
		if(off == 0) return m_GetPosition;
		m_GetPosition = m_GetPosition + off;
		break;
	case SEEK_SET:
		if(off < 0) return m_GetPosition;
		m_GetPosition = off;
		break;
	case SEEK_END:	
		if(off > 0) return m_GetPosition;
		m_GetPosition = fileSize + off;
		break;
	}

	if(m_GetPosition < 0)
		m_GetPosition = 0;

	if(m_GetPosition > fileSize)
		m_GetPosition = fileSize;

	return m_GetPosition;
}


void CompressedVirtualFileAccess::flush()
{
	// Write put buffer
	if(m_pVirtualFileIO->m_nFlags & O_RDWR || m_pVirtualFileIO->m_nFlags & O_WRONLY)
	{
		if(m_PutPosition > 0)
		{
			const int compressBufSize = calc_compress_buffer_size(m_PutPosition);

			char* pUncompressBuf	= new char[m_PutPosition];
			char* pCompressBuf		= new char[compressBufSize];

			char* pCopyPos = pUncompressBuf;
			PUTBUFFER_VEC::iterator writepos = m_PutBufferVec.begin();

			for(; writepos != m_PutBufferVec.end(); ++writepos)
			{
				memcpy(pCopyPos, writepos->buffer, writepos->size);
				pCopyPos += writepos->size;
			}

			DWORD	compressSize = compressBufSize;
			int		result;

#ifdef COMPRESS_ALGORITHM_LZO
			result = MMiniLZO::Compress(
				(lzo_bytep)pUncompressBuf, m_PutPosition,
				(lzo_bytep)pCompressBuf, &compressSize);
			assert(result == LZO_E_OK);
#else	
			result = compress2(
				(Bytef*)pCompressBuf, &compressSize,
				(Bytef*)pUncompressBuf, m_PutPosition, Z_BEST_SPEED );
			assert(result == Z_OK);
#endif

			// ��ȣȭ
			encrypt_data(pCompressBuf, compressSize,
				(int)hash_func_path_string_stlport()(m_pVirtualFileIO->m_strFileName.c_str()));

			m_pVirtualFileIO->m_pNFS->file_lseek(m_pVirtualFileIO->m_NFSFiledes,
				VF_HEADER_SIZE + CVF_HEADER_SIZE, SEEK_SET);

			if(compressSize < m_PutPosition)
			{
				m_CVFHeader.CompressSize	= compressSize;
				m_CVFHeader.Incompressible	= 0;

				m_pVirtualFileIO->m_pNFS->file_write(m_pVirtualFileIO->m_NFSFiledes,
					pCompressBuf, compressSize);
			}
			else
			{
				m_CVFHeader.CompressSize	= m_PutPosition;
				m_CVFHeader.Incompressible	= 1;

				m_pVirtualFileIO->m_pNFS->file_write(m_pVirtualFileIO->m_NFSFiledes,
					pUncompressBuf, m_PutPosition);
			}	

			delete [] pCompressBuf;
			delete [] pUncompressBuf;
		}
		else
		{
			m_CVFHeader.CompressSize	= 0;
			m_CVFHeader.Incompressible	= 1;
		}

		m_pVirtualFileIO->m_pNFS->file_lseek(m_pVirtualFileIO->m_NFSFiledes,
			VF_HEADER_SIZE, SEEK_SET);

		m_pVirtualFileIO->m_pNFS->file_write(m_pVirtualFileIO->m_NFSFiledes,
			&m_CVFHeader, CVF_HEADER_SIZE);
	}

	// Delete Put Buffer
	PUTBUFFER_VEC::iterator delpos = m_PutBufferVec.begin();
	
	for(; delpos != m_PutBufferVec.end(); ++delpos)
		delete [] delpos->buffer;
	
	m_PutBufferVec.clear();
	
	m_PutPosition = 0;
}
