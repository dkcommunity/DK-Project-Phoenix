//////////////////////////////////////////////////////////////////////////////
/// @file vfstream.cpp
/// @author chyaya
/// @date 2007.06.26
//////////////////////////////////////////////////////////////////////////////

#include "VFS_Macro.h"
#include "VFS_Precompile.h"


#include "vfstream.h"

#include "NormalFileIO.h"
#include "PhoenixProtectedFile.h"
#include "PhoenixProtectedFileIO.h"
#include "VirtualFileIO.h"

#ifdef _DEBUG
#define PACKAGEFILE_INLINE
#include "vfstream.inl"
#endif

VFSYSTEM iovfs_base::s_VFSystem;

void iovfs_base::open(LPCTSTR szName, int nMode) {
  if (!szName)
    return;

  if ((nMode & std::ios_base::in) && !(nMode & std::ios_base::out) &&
      PhoenixProtectedFile::IsInfPath(szName))
  {
    std::string protectedPath;
    if (PhoenixProtectedFile::BuildProtectedPath(szName, protectedPath) &&
        access(protectedPath.c_str(), 0) == 0)
    {
      m_pFileIO = new PhoenixProtectedFileIO;
      static_cast<PhoenixProtectedFileIO*>(m_pFileIO)->openWithOriginalPath(protectedPath.c_str(), szName, nMode);

      if (m_pFileIO->is_open())
        return;

      delete m_pFileIO;
      m_pFileIO = NULL;
    }
  }

  BOOL bVirtualFile = nMode & iovs_ex::virtualfile;

  if (!bVirtualFile && nMode & std::ios_base::in)
  {
      if (access(szName, 0) == 0)
      {
          bVirtualFile = false;
      }
      else
      {
          // IF FILE NOT FOUND ON DISK: Only use VirtualFile if VFS is actually started!
          // This prevents NULL pointer crash in VirtualFileIO constructor.
          if (s_VFSystem.GetNFS() != NULL && s_VFSystem.GetNFS()->is_open())
              bVirtualFile = true;
          else
              bVirtualFile = false;
      }
  }

  std::string strName = szName;

  //	 if ( strName.find(".inf") != -1 ) //NIFļ	ASPKʱע͵
  //	 {
  //		 bVirtualFile = 1; //Ӱ
  //		for (int i=0; i<EXCEPT_FILE_MAX; i++)
  //		{
  //			if ( strName.find( m_strExceptList[i].c_str() ) != -1 )
  ////ҵ 				bVirtualFile = 0; //ļ
  //		}
  //	 }

  if (bVirtualFile) {
    // VF ҹڸ   Unique Path ش.
    char szUniquePath[MAX_PATH];
    strcpy(szUniquePath, szName);
    make_unique_path(szUniquePath);

    m_pFileIO = new VirtualFileIO(&s_VFSystem);
    m_pFileIO->open(szUniquePath, nMode);
  } else {
    // Ϲ  Ȯ ɼ   .
    nMode &= 0x0FFFFFFF;

    m_pFileIO = new NormalFileIO;
    m_pFileIO->open(szName, nMode);
  }

  if (!m_pFileIO->is_open())
    close();
}

void iovfs_base::close() {
  if (m_pFileIO)
    m_pFileIO->close();
  Destory();
}

void iovfs_base::start_vfs(LPCTSTR filename, int flags) {
  s_VFSystem.Start(filename, flags);
}

void iovfs_base::end_vfs() { s_VFSystem.End(); }

bool iovfs_base::is_virtualfile() const {
  return dynamic_cast<VirtualFileIO *>(m_pFileIO) != NULL;
}
