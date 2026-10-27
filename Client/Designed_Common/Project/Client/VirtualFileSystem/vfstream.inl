//////////////////////////////////////////////////////////////////////////////
/// @file vfstream.inl
/// @author chyaya
/// @date 2007.06.26
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// iovfs_base

PACKAGEFILE_INLINE iovfs_base::iovfs_base()
{
	Init();
}

PACKAGEFILE_INLINE iovfs_base::iovfs_base(LPCTSTR szName, int nMode)
{
	Init();
	open(szName, nMode);
}

PACKAGEFILE_INLINE iovfs_base::~iovfs_base()
{
	close();
}

PACKAGEFILE_INLINE void iovfs_base::Init()
{
	m_pFileIO = NULL;

	m_strExceptList[0] = "Data\\\\Info\\\\ClientConfig.inf";
	m_strExceptList[1] = "Data\\\\Info\\\\GuildMapper.inf";
	m_strExceptList[2] = "Data\\\\Info\\\\GuildVersion.inf";
	m_strExceptList[3] = "Data\\\\Info\\\\Player.inf";
	m_strExceptList[4] = "Data\\\\Info\\\\Resolution.inf";
	m_strExceptList[5] = "Data\\\\Info\\\\SelectedChannel.inf";
	
}

PACKAGEFILE_INLINE void iovfs_base::Destory()
{
	delete m_pFileIO;
	Init();
}

PACKAGEFILE_INLINE bool	iovfs_base::is_open() const
{
	return m_pFileIO && m_pFileIO->is_open();
}

PACKAGEFILE_INLINE int iovfs_base::gcount() const
{
	if(m_pFileIO) return m_pFileIO->gcount(); return 0;
}


//////////////////////////////////////////////////////////////////////
// ivfstream

PACKAGEFILE_INLINE ivfstream::ivfstream()
{

}

PACKAGEFILE_INLINE ivfstream::ivfstream(LPCTSTR szName, int nMode)
{
	open(szName, nMode);
}

PACKAGEFILE_INLINE ivfstream::~ivfstream()
{

}

PACKAGEFILE_INLINE void ivfstream::open(LPCTSTR szName, int nMode)
{
	iovfs_base::open(szName, nMode | std::ios_base::in);
}

PACKAGEFILE_INLINE void ivfstream::read(char* buf, unsigned len)
{
	if(m_pFileIO) m_pFileIO->read(buf, len);
}

PACKAGEFILE_INLINE void	ivfstream::seekg(long off, int dir)
{
	if(m_pFileIO) m_pFileIO->seekg(off, dir);
}

PACKAGEFILE_INLINE int ivfstream::tellg()
{
	if(m_pFileIO) return m_pFileIO->tellg(); return 0;
}	

PACKAGEFILE_INLINE bool	ivfstream::eof() const
{
	return m_pFileIO && m_pFileIO->eof();
}

PACKAGEFILE_INLINE void ivfstream::get(char& ch)
{
	if(m_pFileIO)
		m_pFileIO->get(ch);
}

PACKAGEFILE_INLINE void* ivfstream::getline(char* psch, int nCount, char delim)
{
	if(m_pFileIO)
		return m_pFileIO->getline(psch, nCount, delim);
	return NULL;
}


//////////////////////////////////////////////////////////////////////
// ovfstream

PACKAGEFILE_INLINE ovfstream::ovfstream()
{

}

PACKAGEFILE_INLINE ovfstream::ovfstream(LPCTSTR szName, int nMode)
{
	open(szName, nMode);
}

PACKAGEFILE_INLINE ovfstream::~ovfstream()
{

}

PACKAGEFILE_INLINE void ovfstream::open(LPCTSTR szName, int nMode)
{
	iovfs_base::open(szName, nMode | std::ios_base::out);
}

PACKAGEFILE_INLINE void ovfstream::write(const char* src, int len)
{
	if(m_pFileIO) m_pFileIO->write(src, len);
}

PACKAGEFILE_INLINE void ovfstream::seekg(long off, int dir)
{
	if(m_pFileIO) m_pFileIO->seekg(off, dir);
}
