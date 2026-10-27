//////////////////////////////////////////////////////////////////////
//
// Filename    : FileIO.inl
// Written By  : chyaya@darkeden.com
// Description : Inline Of *FileIO Class
//
//////////////////////////////////////////////////////////////////////

#include <Assert.h>

VT_FILEIO_INLINE VirtualTempFileIO::~VirtualTempFileIO()
{
	close();
}

VT_FILEIO_INLINE bool VirtualTempFileIO::is_open() const
{
	return m_bOpen;
}

VT_FILEIO_INLINE void VirtualTempFileIO::read(char* buf, int len)
{
	//assert(FALSE);
}

VT_FILEIO_INLINE int VirtualTempFileIO::gcount() const
{
	//assert(FALSE);
	return 0;
}

VT_FILEIO_INLINE void VirtualTempFileIO::seekg(long off, int dir)
{
	//assert(FALSE);
}

VT_FILEIO_INLINE int VirtualTempFileIO::tellg()
{
	//assert(FALSE);
	return 0;
}

VT_FILEIO_INLINE void VirtualTempFileIO::write(const char* src, int len)
{
	//assert(FALSE);
}

VT_FILEIO_INLINE void VirtualTempFileIO::seekp(long off, int dir)
{
	//assert(FALSE);
}

VT_FILEIO_INLINE int VirtualTempFileIO::tellp()
{
	//assert(FALSE);
	return 0;
}

VT_FILEIO_INLINE bool VirtualTempFileIO::eof() const
{
	//assert(FALSE);
	return true;
}

VT_FILEIO_INLINE void VirtualTempFileIO::get(char& ch)
{

}

VT_FILEIO_INLINE void* VirtualTempFileIO::getline(char* psch, int nCount, char delim)
{
	//assert(FALSE);
	return NULL;
}

VT_FILEIO_INLINE void VirtualTempFileIO::close()
{
    m_bOpen = false;
}