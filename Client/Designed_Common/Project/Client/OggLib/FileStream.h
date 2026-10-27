#ifndef __FILESTREAM_H__
#define __FILESTREAM_H__
//-----------------------------------------------------------------------------

//#include <d3d9.h>
#include <windows.h>
#include <string>
#include "Define.h"

class FileStream
{
public:
	FileStream();
	virtual ~FileStream();
	void Clear();

	HRESULT __stdcall Open(std::string sFileName);
	void Close();

	// IStream Interface
	HRESULT __stdcall Seek( 
		 LARGE_INTEGER dlibMove,
		 DWORD dwOrigin,
		 ULARGE_INTEGER *plibNewPosition);

	// ISequentialStream Interface
	HRESULT __stdcall Read( 
		 void *pv,
		 ULONG cb,
		 ULONG *pcbRead);        
private:

	INT32			m_iRefCount;
	FILE*			m_pFile;
	UINT32			m_nSize;
	std::string		m_sFileName;

	UINT8*			m_pBuffer;
	UINT32			m_nReadPtr;
	bool			m_bBuffered;
};

#endif __FILESTREAM_H__


