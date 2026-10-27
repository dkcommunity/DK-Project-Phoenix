//////////////////////////////////////////////////////////////////////
//
// Filename    : FileIO.cpp
// Written By  : chyaya@darkeden.com
// Description : Source Of *FileIO Class
//
//////////////////////////////////////////////////////////////////////

#include "VFS_Precompile.h"
#include "VFS_Macro.h"

#include "VirtualFileSystem.h"
#include "VirtualTempFileIO.h"

#include "TSingleton.h"

#include <set>

#ifdef _DEBUG
#define VT_FILEIO_INLINE
#include "VirtualTempFileIO.inl"
#endif


//////////////////////////////////////////////////////////////////////
//
// VirtualTempFileManager
//
//////////////////////////////////////////////////////////// chyaya //
class VirtualTempFileManager : public TSingleton<VirtualTempFileManager>
{
public:
	VirtualTempFileManager();
	~VirtualTempFileManager();

	typedef std::set<std::string>	TEMPFILENAME_SET;

public:
	bool	CreateTempFile(LPVFSYSTEM pVFS, LPCTSTR szVFName, LPCTSTR szTempName);
	bool	IsAlreadyCreated(LPCTSTR szName);


private:
	TEMPFILENAME_SET	m_TempFileNameSet;
};


// Instance
namespace
{
	VirtualTempFileManager	s_VirtualTempFileManagerInstance;
}


VirtualTempFileManager::VirtualTempFileManager()
{
}


VirtualTempFileManager::~VirtualTempFileManager()
{
	TEMPFILENAME_SET::iterator pos = m_TempFileNameSet.begin();

	for(; pos != m_TempFileNameSet.end(); ++pos)
	{
		remove(pos->c_str());

#ifdef _DEBUG
		static cnt = m_TempFileNameSet.size();
		char szBuf[128];
		sprintf(szBuf, "Delete Temp File: szTempName = %s, Size = %d\n",
			pos->c_str(), --cnt);
		OutputDebugString(szBuf);
#endif
	}
}


bool VirtualTempFileManager::CreateTempFile(LPVFSYSTEM pVFS, LPCTSTR szVFName, LPCTSTR szTempName)
{
	if(!IsAlreadyCreated(szTempName))
	{
		if(pVFS->CreateRealFile(szVFName, szTempName))
		{
			m_TempFileNameSet.insert(szTempName);

#ifdef _DEBUG
			char szBuf[128];
			sprintf(szBuf, "Create Temp File: szTempName = %s, Size = %d\n",
				szTempName, m_TempFileNameSet.size());
			OutputDebugString(szBuf);
#endif
			return true;
		}
		else
		{
			return false;
		}
	}

	return true;
}


bool VirtualTempFileManager::IsAlreadyCreated(LPCTSTR szName)
{
	TEMPFILENAME_SET::iterator pos = m_TempFileNameSet.find(szName);
	return pos != m_TempFileNameSet.end();
}


//////////////////////////////////////////////////////////////////////
//
// VirtualTempFileIO
//
//////////////////////////////////////////////////////////// chyaya //

VirtualTempFileIO::VirtualTempFileIO(VirtualFileSystem* pVFS)
	: m_pVFS(pVFS)
{
	m_bOpen = false;
}


void VirtualTempFileIO::open(LPCTSTR szName, int nMode)
{
    char szTempPath[MAX_PATH];
    make_temp_filepath(szName, szTempPath);
    make_unique_path(szTempPath);

    // Extended path prefix for >260 char support
    std::string extendedPath = "\\\\?\\" + std::string(szTempPath);

    if (VirtualTempFileManager::Instance()->CreateTempFile(m_pVFS, szName, extendedPath.c_str()))
    {
        m_strTempPath = extendedPath;  // Store extended path
        m_bOpen = true;
    }
}