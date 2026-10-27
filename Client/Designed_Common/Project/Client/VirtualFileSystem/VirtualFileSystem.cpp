//////////////////////////////////////////////////////////////////////////////
/// @file VirtualFileSystem.cpp
/// @author chyaya
/// @date 2007.06.26
//////////////////////////////////////////////////////////////////////////////

#include "VFS_Precompile.h"
#include "VFS_Macro.h"

#include "VirtualFileSystem.h"
#include "NormalFileIO.h"
#include "VirtualFileIO.h"
#include "..\crash_trace_tool.h"

#include <direct.h>

#ifdef SPECIALIZE_DARKEDEN
#include "CTypePack.h"
#endif // SPECIALIZE_DARKEDEN


VirtualFileSystem::VirtualFileSystem()
{
	Init();
}


VirtualFileSystem::VirtualFileSystem(LPCTSTR filename, int flags)
{
	Start(filename, flags);
}


VirtualFileSystem::~VirtualFileSystem()
{
	End();
}


void VirtualFileSystem::Init()
{
	m_bBypassVFS = false;
	m_strBypassPath = "";
}


bool VirtualFileSystem::Start(LPCTSTR filename, int flags) 
{
	if(m_NFS.is_open())
	{
		assert("VirtualFileSystem Start Error" && false);
		return false;
	}

	bool bWriteMode = flags & O_WRONLY || flags & O_RDWR; 

	char szVFile[MAX_PATH];

	if(!bWriteMode)
	{
		// ش̸ dpk ִ üũ
		strcpy(szVFile, filename);
		strcat(szVFile, nfs_DATA_SUFFIX);
		if(access(szVFile, 0)) return false;
	}

	//     dpl   ȴ.
	strcpy(szVFile, filename);
	strcat(szVFile, nfs_LOCK_SUFFIX);
	remove(szVFile);

	CT_Done("VFS: Attempting to connect DPK: %s", filename);
    m_NFS.start(const_cast<char*>(filename), flags);
	
	if(!m_NFS.is_open()) 
	{
		m_NFS.perror("start error: ");
		assert("VirtualFileSystem Start Error" && false);
		return false;
	}

	return true;
}


void VirtualFileSystem::End()
{
	m_NFS.end();
}


void VirtualFileSystem::AddFile(LPCTSTR srcname, bool compress)
{
	char szUniquePath[MAX_PATH];
	strcpy(szUniquePath, srcname);
	make_unique_path(szUniquePath);

	AddFileRename(srcname, szUniquePath, compress);
}


void VirtualFileSystem::AddFileRename(LPCTSTR srcname, LPCTSTR destname, bool compress)
{
	NormalFileIO NormalFile;
	NormalFile.open(srcname, std::ios_base::in | std::ios_base::binary);
	
	NormalFile.seekg(0, std::ios_base::end);
	int fileSize = NormalFile.tellg();

	NormalFile.seekg(0, std::ios_base::beg);

	int addFlag = 0;
	if(compress) addFlag |= iovs_ex::compress;


	VirtualFileIO VirtualFile(this);
	VirtualFile.open(destname, std::ios_base::out | std::ios_base::binary | addFlag);

	char	buffer[FILE_COPY_BUFFER_SIZE];
	int		readCount = 0;

	for(;;)
	{

		NormalFile.read(buffer, FILE_COPY_BUFFER_SIZE);
		readCount = NormalFile.gcount();
		if(readCount == 0) break;
		VirtualFile.write(buffer, readCount);
	}

	VirtualFile.close();

	NormalFile.close();
}


#ifdef SPECIALIZE_DARKEDEN

void VirtualFileSystem::AddTypePackFile(LPCTSTR srcname, bool compress)
{
	AddTypePackFileRename(srcname, srcname, compress);
}


void VirtualFileSystem::AddTypePackFileRename(LPCTSTR srcname, LPCTSTR destname, bool compress)
{
	// Open TypePack
	NormalFileIO TypePackFile;
	TypePackFile.open(srcname, std::ios_base::in | std::ios_base::binary);

	// Get TypePack File Size
	TypePackFile.seekg(0, std::ios_base::end);
	int FileSize = TypePackFile.tellg();
	TypePackFile.seekg(0, std::ios_base::beg);

	// Get TypePack Data Count
	WORD DataCount = 0;
	TypePackFile.read((char *)&DataCount, 2);

    // TypePack Index Load
	bool bIndexLoad = false;

	WORD IndexCount = 0;
	NormalFileIO TypePackIndexFile;
	TypePackIndexFile.open((std::string(srcname) + "i").c_str(), std::ios_base::in | std::ios_base::binary);
	TypePackIndexFile.read((char *)&IndexCount, 2);

	int* pIndex = NULL;

	if(TypePackIndexFile.is_open() && IndexCount == DataCount)
	{
		bool readError = false;

		pIndex = new int[DataCount + 1];
		for (int i = 0; i < DataCount; ++i)
		{
			TypePackIndexFile.read((char *)&pIndex[i], 4);
			if(TypePackIndexFile.gcount() < 4) { readError = true; break; }
		}

		TypePackIndexFile.close();

		if(!readError)
		{
			pIndex[DataCount] = FileSize;
			bIndexLoad = true;
		}
	}

	// Index Load ϸ Ϲ  Ѵ.
	if(!bIndexLoad)
	{
        delete [] pIndex;
		AddFileRename(srcname, destname, compress);
		return;
	}

	// Generate Data Size
	int* pDataSize = new int[DataCount];
	int DataSizeMax = 0;
	int DataSizeSum = 0;

	for (int i = 0; i < DataCount; ++i)
	{
		int size = pIndex[i + 1] - pIndex[i];
		if(size > DataSizeMax) DataSizeMax = size;
		pDataSize[i] = size;
		DataSizeSum += size;
	}

	assert(DataSizeSum == FileSize - 2);

	delete [] pIndex;

	// Open Virtual File
	int addFlag = 0;
	if(compress) addFlag |= iovs_ex::compress;

	VirtualFileIO VirtualFile(this);
	VirtualFile.open(destname, std::ios_base::out | std::ios_base::binary | addFlag);

	VirtualFile.write((char *)&DataCount, 2);

	char* pBuffer = new char[DataSizeMax];

	for(int j = 0; j < DataCount; ++j)
	{
		TypePackFile.read(pBuffer, pDataSize[j]);
		VirtualFile.write(pBuffer, pDataSize[j]);
	}

	VirtualFile.close();

	TypePackFile.close();

	delete [] pBuffer;
	delete [] pDataSize;
}

#endif	// SPECIALIZE_DARKEDEN



void VirtualFileSystem::DeleteFile(LPCTSTR filename)
{
	m_NFS.file_unlink(filename);
}


bool VirtualFileSystem::IsFileExist(LPCTSTR filename) const
{
	DWORD dwFileAttri = GetFileAttributes(filename);

	// Ϲ  ٸ Ű ˻
	if(dwFileAttri == 0xFFFFFFFF ||
	   dwFileAttri & FILE_ATTRIBUTE_DIRECTORY)
	{
		if(m_NFS.is_open())
		{
			char szUniqueName[MAX_PATH];
			
			strcpy(szUniqueName, filename);
			make_unique_path(szUniqueName);

			bool bExists = m_NFS.file_exists(szUniqueName) != 0;
            if (bExists) CT_Done("VFS: File FOUND in DPK: %s", szUniqueName);
            return bExists;
		}
		else
		{
			return false;
		}
	}

	return true;
}


int VirtualFileSystem::GetFileSize(LPCTSTR filename)
{
	int fileSize = -1;

	if(access(filename, 0))
	{
		NFS_FILEDES des = m_NFS.file_open(filename, O_RDONLY);

		if(des >= 0)
		{
			int size = m_NFS.file_size(des);
			
			fileSize = size - VF_HEADER_SIZE;
			assert(fileSize >= 0);

			m_NFS.file_close(des);
		}	
	}
	else
	{
		std::ifstream file(filename);

		file.seekg(0, std::ios_base::end);

		fileSize = static_cast<int>(file.tellg());
		assert(fileSize >= 0);
	}

	return fileSize;
}


bool VirtualFileSystem::CreateTempFile(LPCTSTR srcname)
{
	char szTempName[MAX_PATH];
	make_temp_filepath(srcname, szTempName);

	return CreateRealFile(srcname, szTempName);
}


bool VirtualFileSystem::CreateRealFile(LPCTSTR srcname, LPCTSTR destname)
{
    VirtualFileIO VirtualFile(this);

    std::string uniquePath(srcname);
    make_unique_path(&uniquePath[0]);

    // Extended path prefix for >260 char support
    std::string extendedSrc = "\\\\?\\" + uniquePath;
    std::string extendedDest = "\\\\?\\" + std::string(destname);

    VirtualFile.open(extendedSrc.c_str(), std::ios_base::in | std::ios_base::binary);
    if (!VirtualFile.is_open()) return false;

    BuildFullPathDirectory(destname);

    NormalFileIO NormalFile;
    NormalFile.open(extendedDest.c_str(), std::ios_base::out | std::ios_base::binary);
    if (!NormalFile.is_open()) return false;

    char buffer[FILE_COPY_BUFFER_SIZE];
    int readCount = 0;

    for (;;)
    {
        VirtualFile.read(buffer, FILE_COPY_BUFFER_SIZE);
        readCount = VirtualFile.gcount();
        if (readCount == 0) break;
        NormalFile.write(buffer, readCount);
    }

    NormalFile.close();
    VirtualFile.close();
    return true;
}


bool VirtualFileSystem::BuildFullPathDirectory(LPCTSTR szFullPath)
{
    // Convert to wide string for CreateDirectoryW
    std::wstring wpath;
    {
        int len = MultiByteToWideChar(CP_ACP, 0, szFullPath, -1, nullptr, 0);
        if (len <= 0) return false;
        wpath.resize(len - 1);
        MultiByteToWideChar(CP_ACP, 0, szFullPath, -1, &wpath[0], len);
    }

    // Extended path prefix for >260 support
    std::wstring extended = L"\\\\?\\" + wpath;

    // Parse and create each directory level
    std::wstring current;
    for (wchar_t ch : extended) {
        if (ch == L'\\' || ch == L'/') {
            if (!current.empty() && current.back() != L':') {
                if (!CreateDirectoryW(current.c_str(), nullptr)) {
                    DWORD err = GetLastError();
                    if (err != ERROR_ALREADY_EXISTS) return false;
                }
            }
        }
        current += ch;
    }
    return true;
}