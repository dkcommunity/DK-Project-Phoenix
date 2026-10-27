//////////////////////////////////////////////////////////////////////
//
// Filename    : FileIO.inl
// Written By  : chyaya@darkeden.com
// Description : Inline Of *FileIO Class
//
//////////////////////////////////////////////////////////////////////


N_FILEIO_INLINE NormalFileIO::NormalFileIO()
{

}

N_FILEIO_INLINE NormalFileIO::~NormalFileIO()
{
	close();
}

N_FILEIO_INLINE void NormalFileIO::open(LPCTSTR szName, int nMode)
{
	m_NormalFile.open(szName, nMode);
}

N_FILEIO_INLINE void NormalFileIO::close()
{
	m_NormalFile.close();
}

N_FILEIO_INLINE bool NormalFileIO::is_open() const
{
	return const_cast<NormalFileIO*>(this)->m_NormalFile.is_open();
}

N_FILEIO_INLINE void NormalFileIO::read(char* buf, int len)
{
	m_NormalFile.read(buf, len);
}

N_FILEIO_INLINE int NormalFileIO::gcount() const
{
	return static_cast<int>(m_NormalFile.gcount());
}

N_FILEIO_INLINE void NormalFileIO::seekg(long off, int dir)
{
	m_NormalFile.seekg(off, static_cast<std::ios_base::seekdir>(dir));
}

N_FILEIO_INLINE int NormalFileIO::tellg()
{
	return static_cast<int>(m_NormalFile.tellg());
}

N_FILEIO_INLINE void NormalFileIO::write(const char* src, int len)
{
	m_NormalFile.write(src, len);
}

// N_FILEIO_INLINE void NormalFileIO::seekp(long off, int dir)
// {
// 	m_NormalFile.seekp(off, dir);
// }
// 
// N_FILEIO_INLINE int NormalFileIO::tellp()
// {
// 	return static_cast<int>(m_NormalFile.tellp());
// }

N_FILEIO_INLINE bool NormalFileIO::eof() const
{
	return m_NormalFile.eof();
}

N_FILEIO_INLINE void NormalFileIO::get(char& ch)
{
	m_NormalFile.get(ch);
}

N_FILEIO_INLINE void* NormalFileIO::getline(char* psch, int nCount, char delim)
{
	m_NormalFile.getline(psch, nCount, delim);
	return m_NormalFile ? &m_NormalFile : NULL;
}
