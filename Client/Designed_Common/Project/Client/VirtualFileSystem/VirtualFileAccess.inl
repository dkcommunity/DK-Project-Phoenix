//////////////////////////////////////////////////////////////////////
//
// Filename    : VirtualFileAccess.inl
// Written By  : chyaya@darkeden.com
// Description : Inline Of *FileIO Class
//
//////////////////////////////////////////////////////////////////////


V_FILE_ACCESS_INLINE
VirtualFileAccess::VirtualFileAccess(VirtualFileIO* vfio)
	: VFA_Base(vfio)
{

}


V_FILE_ACCESS_INLINE
VirtualFileAccess::~VirtualFileAccess()
{

}


V_FILE_ACCESS_INLINE
int VirtualFileAccess::read(char* buf, int len)
{
	return m_pVirtualFileIO->m_pNFS->file_read(m_pVirtualFileIO->m_NFSFiledes, buf, len);
}


V_FILE_ACCESS_INLINE
int VirtualFileAccess::write(const char* src, int len)
{
	return m_pVirtualFileIO->m_pNFS->file_write(m_pVirtualFileIO->m_NFSFiledes, src, len);
}

V_FILE_ACCESS_INLINE
int VirtualFileAccess::lseek(long off, int dir)
{
	if(dir == SEEK_SET) off += VF_HEADER_SIZE;
	return m_pVirtualFileIO->m_pNFS->file_lseek(m_pVirtualFileIO->m_NFSFiledes, off, dir);
}
