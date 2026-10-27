#include "FileCheckLib_Precompile.h"
#include "MD5/MD5Checksum.h"
#include "VFS_Macro.h"
#include "FileCheckInfo.h"

FileCheckInfo::FileCheckInfo()
	: m_FileSize(0)
	, m_PackingType(PT_NO_PACKING)
{
	memset(&m_LastWriteTime, 0, sizeof(FILETIME));
	memset(m_MD5CheckSum, 0, CHECKSUM_SIZE);
}

FileCheckInfo::FileCheckInfo(LPCTSTR fileName)
{
	GenerateFileInfo(fileName);
}

FileCheckInfo::FileCheckInfo(const FileCheckInfo& fileCheckInfo)
{
	m_FileSize		= fileCheckInfo.m_FileSize;
	m_LastWriteTime	= fileCheckInfo.m_LastWriteTime;
	m_PackingType	= fileCheckInfo.m_PackingType;

	memcpy(m_MD5CheckSum, fileCheckInfo.m_MD5CheckSum, CHECKSUM_SIZE);
}

FileCheckInfo::~FileCheckInfo()
{

}

void FileCheckInfo::GenerateFileInfo(LPCTSTR fileName)
{
	assert("GenerateFileInfo : file didn't exist" && !access(fileName, 0));

// 	char szUniqueNameBuf[MAX_PATH];
// 	strcpy(szUniqueNameBuf, fileName);
// 	make_unique_path(szUniqueNameBuf);
// 
// 	m_strFileName	= szUniqueNameBuf;
	m_PackingType	= PT_COMPRESS_PACKING;
	
	WIN32_FIND_DATA fd;

	HANDLE hFD = FindFirstFile(fileName, &fd);

	if(hFD != INVALID_HANDLE_VALUE)
	{
		m_LastWriteTime	= fd.ftLastWriteTime;
		m_FileSize		= fd.nFileSizeLow;
		FindClose(hFD);

		std::string checkData = CMD5Checksum::GetMD5(fileName);

		assert(checkData.size() == CHECKSUM_SIZE);

		memcpy(m_MD5CheckSum, checkData.c_str(), CHECKSUM_SIZE);
	}
}

bool FileCheckInfo::IsFileDataIdentical(const FileCheckInfo& fileCheckInfo) const
{
	return	m_FileSize == fileCheckInfo.m_FileSize &&
			m_LastWriteTime.dwHighDateTime == fileCheckInfo.m_LastWriteTime.dwHighDateTime &&
			m_LastWriteTime.dwLowDateTime == fileCheckInfo.m_LastWriteTime.dwLowDateTime &&
			memcmp(m_MD5CheckSum, fileCheckInfo.m_MD5CheckSum, CHECKSUM_SIZE) == 0;
}

void FileCheckInfo::LoadFromFile(std::ifstream &file)
{
	file.read((char *)&m_FileSize, sizeof(DWORD));

	file.read((char *)&m_LastWriteTime, sizeof(FILETIME));

	file.read((char *)&m_PackingType, sizeof(BYTE));

	file.read((char *)m_MD5CheckSum, CHECKSUM_SIZE);
}

void FileCheckInfo::SaveToFile(std::ofstream &file) const
{
	file.write((const char *)&m_FileSize, sizeof(DWORD));

	file.write((const char *)&m_LastWriteTime, sizeof(FILETIME));

	file.write((const char *)&m_PackingType, sizeof(BYTE));

	file.write((const char *)m_MD5CheckSum, CHECKSUM_SIZE);
}
