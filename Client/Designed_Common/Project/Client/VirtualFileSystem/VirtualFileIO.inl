//////////////////////////////////////////////////////////////////////
//
// Filename    : FileIO.inl
// Written By  : chyaya@darkeden.com
// Description : Inline Of *FileIO Class
//
//////////////////////////////////////////////////////////////////////


V_FILEIO_INLINE VirtualFileIO::~VirtualFileIO()
{

// #ifdef _DEBUG
// 	--s_nReferenceCount;
// 	char szBuf[128];
// 	sprintf(szBuf, "Destory VFileIO: RefCount = %d\n", s_nReferenceCount);
// 	OutputDebugString(szBuf);
// #endif

	close();
}


V_FILEIO_INLINE bool VirtualFileIO::is_open() const
{
	return m_NFSFiledes	>= 0;
}


V_FILEIO_INLINE int VirtualFileIO::gcount() const
{
	return m_dwGetCount;
}