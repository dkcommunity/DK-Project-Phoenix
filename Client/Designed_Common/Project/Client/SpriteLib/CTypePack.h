#ifndef __CTYPEPACK_H__
#define __CTYPEPACK_H__

#include "CDirectDraw.h"
#include "COrderedList.h"
#include "CSpriteSetManager.h"
#include "vfstream.h"
#include "../crash_trace_tool.h"
#include <algorithm>
#include <io.h>
#include <map>
#include <vector>
#include <windows.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" unsigned __int64 Phoenix_ItemOutputPerf_Begin();
extern "C" void Phoenix_ItemOutputPerf_RecordSpkLazyLoad(int spriteID, unsigned __int64 beginTick);
extern "C" int Phoenix_ItemOutputPerf_IsCurrentItemActive();
extern "C" int Phoenix_ResourceStreaming_ShouldDeferLazyLoad(int spriteID, const char* packName);
extern "C" void Phoenix_ResourceStreaming_RecordDeferredLazyLoad(int spriteID, const char* packName);
extern "C" void Phoenix_ResourceStreaming_RecordBlockingLazyLoad(int spriteID, const char* packName,
                                                                  unsigned __int64 beginTick);
extern "C" int Phoenix_VisualAudit_IsEnabled();
extern "C" void Phoenix_VisualAudit_Log(const char* category, const char* format, ...);

namespace {
    static int atoi_robust_next(const char*& p, const char* end) {
        // Skip non-digits
        while (p < end && !isdigit((unsigned char)*p)) p++;
        if (p >= end) return 0;
        
        // Convert to int
        int val = atoi(p);
        
        // Skip the digits we just read
        while (p < end && isdigit((unsigned char)*p)) p++;
        
        return val;
    }

    static void log_hex(const char* label, const char* str, int len, const char* filename) {
        char hexBuf[512] = {0};
        char* pOut = hexBuf;
        for(int i=0; i<len && i<128; i++) {
            pOut += sprintf(pOut, "%02X ", (unsigned char)str[i]);
            if (i > 0 && (i+1) % 16 == 0) pOut += sprintf(pOut, "| ");
        }
        CT_Done("FORENSIC [%s]: %s - Hex: %s", filename, label, hexBuf);
    }

    // Monolithic pack yolu (.spk vb.) dosya bulunamasa bile split klasor sayilmamali.
    // Aksi halde ...\Foo.spk\header.inf aranir ve UI sprite pack sessizce bos kalir.
    static bool Phoenix_PackPathLooksLikeSplitFolder(LPCTSTR lpszFilename, bool bIsDirectory) {
        if (bIsDirectory)
            return true;
        if (iovfs_base::get_vfs()->IsFileExist(lpszFilename))
            return false;

        const char* dot = strrchr(lpszFilename, '.');
        if (dot != NULL) {
            if (_stricmp(dot, ".spk") == 0 || _stricmp(dot, ".ispk") == 0 ||
                _stricmp(dot, ".aspk") == 0 || _stricmp(dot, ".sspk") == 0 ||
                _stricmp(dot, ".cfpk") == 0 || _stricmp(dot, ".fpk") == 0)
                return false;
        }
        return true;
    }

    static bool LoadLocalSplitIndex(const char* packFileName, std::vector<DWORD>& offsets) {
        offsets.clear();

        char indexFileName[MAX_PATH];
        sprintf(indexFileName, "%si", packFileName);

        ivfstream indexFile(indexFileName, std::ios::binary);
        if (!indexFile.is_open()) {
            return false;
        }

        WORD indexSize = 0;
        indexFile.read((char*)&indexSize, 2);
        if (indexFile.gcount() != 2 || indexSize == 0) {
            return false;
        }

        offsets.resize(indexSize);
        for (int i = 0; i < indexSize; ++i) {
            indexFile.read((char*)&offsets[i], 4);
            if (indexFile.gcount() != 4) {
                offsets.clear();
                return false;
            }
        }

        return true;
    }
}


class ITypePackBase {
public:
  ITypePackBase() {}
  virtual ~ITypePackBase() {}

  //--------------------------------------------------------
  // Init/Release
  //--------------------------------------------------------
  virtual void Init(WORD size) = 0;
  virtual void Release() = 0;

  //--------------------------------------------------------
  // Size
  //--------------------------------------------------------
  virtual DWORD GetSize() const = 0;

  //--------------------------------------------------------
  // file I/O
  //--------------------------------------------------------
  virtual bool SaveToFile(std::ofstream &dataFile,
                          std::ofstream &indexFile) = 0;

  virtual bool LoadFromFileRunning(LPCTSTR lpszFilename) = 0;
  virtual bool LoadFromFile(LPCTSTR lpszFilename) = 0;
  virtual bool LoadFromFilePart(int first, int last) = 0;
  virtual bool LoadFromFilePart(CSpriteSetManager &SSM) = 0;
  virtual bool LoadFromFileData(int dataID, int fileID, LPCTSTR packFilename,
                                LPCTSTR indexFilename) = 0;

  virtual bool ReleasePart(int first, int last) = 0;
  virtual bool ReleasePart(COrderedList<TYPE_SPRITEID> list) = 0;

  virtual bool SaveToFile(LPCTSTR lpszFilename) = 0;

  virtual void FinishRunningLoad() = 0;
};

template <class Type> class CTypePack : public ITypePackBase {
public:
  CTypePack();
  virtual ~CTypePack();

  typedef std::map<WORD, ivfstream *> FILECACHE_MAP;

  //--------------------------------------------------------
  // Init/Release
  //--------------------------------------------------------
  virtual void Init(WORD size);
  virtual void Release();

  //--------------------------------------------------------
  // Size
  //--------------------------------------------------------
  virtual DWORD GetSize() const { return m_Size; }

  //--------------------------------------------------------
  // operator
  //--------------------------------------------------------
  virtual Type &operator[](WORD n); //		{ return m_pSpritePals[n]; }
  virtual Type &Get(WORD n);
  virtual bool IsElementPrepared(WORD n) const;

  //--------------------------------------------------------
  // file I/O
  //--------------------------------------------------------
  virtual bool SaveToFile(std::ofstream &dataFile, std::ofstream &indexFile);

  virtual bool LoadFromFileRunning(LPCTSTR lpszFilename);
  virtual bool LoadFromFile(LPCTSTR lpszFilename);
  virtual bool LoadFromFilePart(int first, int last);
  virtual bool LoadFromFilePart(CSpriteSetManager &SSM);
  virtual bool LoadFromFileData(int dataID, int fileID, LPCTSTR packFilename,
                                LPCTSTR indexFilename);

  virtual bool ReleasePart(int first, int last);
  virtual bool ReleasePart(COrderedList<TYPE_SPRITEID> list);

  virtual bool SaveToFile(LPCTSTR lpszFilename);

  virtual void FinishRunningLoad();

  // Split pack: Get() ivfstream'i FileCacheMap'te tutar; PhoenixProtected
  // dosyada bu = tum chunk decrypt buffer. Sprite pixel zaten m_pData'da
  // olduktan sonra cache'i bosaltmak VA sismesini keser (m_bRunningLoad korunur).
  void ClearFileCache();
  size_t GetFileCacheCount() const { return m_FileCacheMap.size(); }

protected:
  static bool IsTwoMultiplier(int num);
  static int LogTwoMultiplier(int num); // 2의 승수만 사용할 수 있는 Math Log
  int GetSplitFile(int idx, char *szBuf = NULL);

protected:
  Type *m_pData;
  WORD m_Size;
  bool m_bRunningLoad;

  // runtime loading
  ivfstream *m_file;
  int *m_file_index;

  bool m_bSplitFile;
  int m_nUnitCount;
  int m_nUnitTwoMultiplier; // (!)m_nUnitCount를 2의 승수로 저장를
  std::string m_strFileName;
  std::string m_strFileExt;

  FILECACHE_MAP m_FileCacheMap;
};

template <class Type> CTypePack<Type>::CTypePack() {
  m_pData = NULL;
  m_Size = 0;
  m_bRunningLoad = false;
  m_file_index = NULL;
  m_file = NULL;

  m_bSplitFile = false;
  m_nUnitCount = 0;
  m_nUnitTwoMultiplier = 0;
}

template <class Type> CTypePack<Type>::~CTypePack() { Release(); }

template <class Type> void CTypePack<Type>::Release() {
  FinishRunningLoad();

  if (m_pData != NULL) {
    delete[] m_pData;
    m_pData = NULL;
    m_Size = 0;
  }
}

template <class Type> void CTypePack<Type>::Init(WORD size) {
  if (size == 0)
    return;

  Release();

  m_Size = size;

  m_pData = new Type[size];
}

template <class Type> Type &CTypePack<Type>::operator[](WORD n) {
  return Get(n);
}

template <class Type> bool CTypePack<Type>::IsElementPrepared(WORD n) const {
  if (m_pData == NULL || n >= m_Size)
    return false;
  return m_pData[n].IsInit();
}

template <class Type> Type &CTypePack<Type>::Get(WORD n) {
  // 2026-05-10: Comprehensive NULL/init guards — eksik .ispki yüzünden
  // uninitialized sprite döndürmeyi engeller. Phoenix'te creature4-7 ve
  // newitem packs index dosyası shipped olmamis. Caller render path'inde
  // 0xBC offset access violation kayniyordu.
  static Type s_safe_dummy_pack;  // tek static — birden fazla pack'in
                                  // failed-load case'leri için aynı dummy
  // 2026-09-01: CT_Done -> CT_OBJ rename (harita objesi teshisi, izole
  // kategori + 500ms throttle — bozuk pack her frame Get cagirinca log/FPS
  // sismesin; CT_ITEM rename emsali, kural 4.1 silme yok).
  static DWORD s_ctObjFailTick = 0;
  if (m_pData == NULL) {
    DWORD ctNow = GetTickCount();
    if (ctNow - s_ctObjFailTick > 500) {
      s_ctObjFailTick = ctNow;
      CT_OBJ("ERROR: CTypePack[%s]::Get(%d) - m_pData is NULL! returning dummy.", m_strFileName.c_str(), n);
    }
    return s_safe_dummy_pack;
  }
  if (n >= m_Size) {
    DWORD ctNow = GetTickCount();
    if (ctNow - s_ctObjFailTick > 500) {
      s_ctObjFailTick = ctNow;
      CT_OBJ("ERROR: CTypePack[%s]::Get(%d) - OUT OF BOUNDS (Size=%d)!", m_strFileName.c_str(), n, m_Size);
    }
    if (m_Size > 0 && m_pData[0].IsInit()) return m_pData[0];
    return s_safe_dummy_pack;
  }

  if (m_bRunningLoad && !m_pData[n].IsInit()) {
    if (Phoenix_ResourceStreaming_ShouldDeferLazyLoad((int)n, m_strFileName.c_str())) {
      Phoenix_ResourceStreaming_RecordDeferredLazyLoad((int)n, m_strFileName.c_str());
      return s_safe_dummy_pack;
    }

    unsigned __int64 phoenixItemSpkLazyBegin = 0;
    if (Phoenix_ItemOutputPerf_IsCurrentItemActive())
      phoenixItemSpkLazyBegin = Phoenix_ItemOutputPerf_Begin();

    // file에 있는 Sprite들을 Load
    if (m_bSplitFile) {
      int fileIdx = GetSplitFile(n);

      FILECACHE_MAP::iterator pos = m_FileCacheMap.find(fileIdx);

      ivfstream *pFile = NULL;

      // 파일이 캐시에 있으면 있는걸로 쓴다.
      if (pos != m_FileCacheMap.end()) {
        pFile = pos->second;
      }
      // 파일이 캐시에 없으면 불러온다.
      else {
        char fileName[MAX_PATH];
        GetSplitFile(n, fileName);
        pFile = new ivfstream(fileName, std::ios_base::binary);
      }

      WORD unitSize = 0;

      pFile->seekg(0);
      pFile->read((char *)&unitSize, 2);

      char splitFileName[MAX_PATH];
      GetSplitFile(n, splitFileName);

      std::vector<DWORD> localFileIndex;
      const bool bLoadLocalIndex = LoadLocalSplitIndex(splitFileName, localFileIndex);
      const int localIndex = n - fileIdx;

      // Split packlerde unit-local index dosyasi varsa onu tercih et.
      // Top-level .spki stale kalirsa yeni export edilen sprite'tan sonraki
      // offsetler kayiyor; unit index bu kaymayi dogrudan onler.
      if (bLoadLocalIndex && localIndex >= 0 && localIndex < (int)localFileIndex.size()) {
        pFile->seekg(localFileIndex[localIndex]);
        m_pData[n].LoadFromFile(*pFile);
      }
      else {
        bool bUseGlobalSplitIndex = false;

        if (m_file_index != NULL) {
          int actualChunkBytes = -1;
          std::streampos currentPos = pFile->tellg();
          pFile->seekg(0, std::ios_base::end);
          std::streampos endPos = pFile->tellg();
          if (endPos != std::streampos(-1))
            actualChunkBytes = (int)endPos;
          pFile->seekg(currentPos);

          const int nextChunkDataID = fileIdx + unitSize;
          if (actualChunkBytes > 0 && nextChunkDataID < m_Size) {
            const int expectedChunkBytes =
                m_file_index[nextChunkDataID] - m_file_index[fileIdx] + 2;
            bUseGlobalSplitIndex = (expectedChunkBytes == actualChunkBytes);

            if (!bUseGlobalSplitIndex) {
              CT_Done("CTypePack[%s] split index drift chunk=%d unit=%d expectedBytes=%d actualBytes=%d; sequential chunk load",
                      m_strFileName.c_str(), fileIdx, (int)unitSize,
                      expectedChunkBytes, actualChunkBytes);
            }
          }
        }

        // Unit-local index yoksa ve global index chunk boyutuyla tutarliyse
        // eski random-access davranisini koru. Drift varsa sequential parse
        // dogru kaynak olur; aksi halde shadow/imageobject gibi packlerde
        // yanlis offset bozuk sprite'i init kabul ettirip render crash yaratir.
        if (bUseGlobalSplitIndex) {
          int offset = m_file_index[n] - m_file_index[fileIdx];

          pFile->seekg(offset + 2);

          m_pData[n].LoadFromFile(*pFile);
        }
        else {
          pFile->seekg(2);

          for (int i = 0; i < unitSize && fileIdx + i < m_Size; ++i) {
            m_pData[fileIdx + i].LoadFromFile(*pFile);
          }
        }
      }

      if (!m_pData[n].IsInit()) {
        ivfstream fallbackFile(splitFileName, std::ios_base::binary);
        ivfstream* pSequentialFile = fallbackFile.is_open() ? &fallbackFile : pFile;

        if (fallbackFile.is_open()) {
          WORD fallbackUnitSize = 0;
          fallbackFile.seekg(0);
          fallbackFile.read((char *)&fallbackUnitSize, 2);
          if (fallbackUnitSize != 0)
            unitSize = fallbackUnitSize;
        }

        pSequentialFile->seekg(2);

        for (int i = 0; i < unitSize && fileIdx + i < m_Size; ++i) {
          m_pData[fileIdx + i].LoadFromFile(*pSequentialFile);
        }
      }

      bool bLoadComplete = true;

      for (int i = 0; i < unitSize && fileIdx + i < m_Size; ++i) {
        if (!m_pData[fileIdx + i].IsInit()) {
          bLoadComplete = false;
          break;
        }
      }

      if (bLoadComplete) {
        delete pFile;

        if (pos != m_FileCacheMap.end())
          m_FileCacheMap.erase(pos);
      } else {
        if (pos == m_FileCacheMap.end())
          m_FileCacheMap.insert(FILECACHE_MAP::value_type(fileIdx, pFile));
      }
    } else {
      if (m_file != NULL && m_file->is_open()) {
        if (m_file_index != NULL) {
          m_file->seekg(m_file_index[n]);
          m_pData[n].LoadFromFile(*m_file);
        } else {
          m_file->seekg(2);
          for (WORD walk = 0; walk <= n && walk < m_Size; ++walk) {
            if (!m_pData[walk].IsInit())
              m_pData[walk].LoadFromFile(*m_file);
          }
        }
      }
    }

    if (phoenixItemSpkLazyBegin != 0)
    {
      Phoenix_ItemOutputPerf_RecordSpkLazyLoad((int)n, phoenixItemSpkLazyBegin);
      Phoenix_ResourceStreaming_RecordBlockingLazyLoad((int)n, m_strFileName.c_str(),
                                                       phoenixItemSpkLazyBegin);
    }
  }

  // 2026-05-10: Final init check — load attempt sonrasi sprite hala
  // uninit ise (ispki eksik / chunk corrupt / Type::LoadFromFile fail vs.)
  // safe dummy don. Caller'i NULL-deref crash'inden korur.
  // 0xC0000005 access violation @ NULL+0xBC tipi crash'lerin ana sebebi
  // burasi: render kodu uninit sprite'in member field'larini okuyordu.
  if (!m_pData[n].IsInit()) {
    DWORD ctNow = GetTickCount();
    if (ctNow - s_ctObjFailTick > 500) {
      s_ctObjFailTick = ctNow;
      CT_OBJ("WARNING: CTypePack[%s]::Get(%d) - sprite NOT initialized after load attempt, returning safe dummy",
             m_strFileName.c_str(), (int)n);
    }
    return s_safe_dummy_pack;
  }

  return m_pData[n];
}

template <class Type> bool CTypePack<Type>::LoadFromFile(LPCTSTR lpszFilename) {
  if (iovfs_base::get_vfs()->IsFileExist(lpszFilename)) {
    ivfstream file(lpszFilename, std::ios_base::binary);
    file.read((char *)&m_Size, 2);

    Init(m_Size);

    int i = 0;

    for (; i < m_Size; i++) {
      m_pData[i].LoadFromFile(file);
    }
  } else {

    if (!LoadFromFileRunning(lpszFilename))
      return false;

    // Phoenix diag: Get(i) loop crash izleme
    CT_Done("CTypePack::LoadFromFile GET-LOOP-START %s m_Size=%d", lpszFilename, (int)m_Size);

    int i = 0;
    for (; i < m_Size; ++i) {
      Get(i);
    }

    CT_Done("CTypePack::LoadFromFile GET-LOOP-END %s", lpszFilename);

    if (m_bRunningLoad)
      return false;
  }

  return true;
}

template <class Type> bool CTypePack<Type>::SaveToFile(LPCTSTR lpszFilename) {
  char szIndexFilename[512];
  sprintf(szIndexFilename, "%si", lpszFilename);

  std::ofstream dataFile(lpszFilename, std::ios::binary);
  std::ofstream indexFile(szIndexFilename, std::ios::binary);

  bool re = SaveToFile(dataFile, indexFile);

  dataFile.close();
  indexFile.close();

  return re;
}

//----------------------------------------------------------------------
// Load From File Running
//----------------------------------------------------------------------
// 실시간 로딩
//----------------------------------------------------------------------
template <class Type>
bool CTypePack<Type>::LoadFromFileRunning(LPCTSTR lpszFilename) {
  Release();

  // 다음으로 인덱스 파일이 있다면 로드한다.
  std::string indexFileName = lpszFilename;
  indexFileName += 'i';

  WORD indexSize = 0;
  bool bLoadIndexFile = false;

  ivfstream indexFile(indexFileName.c_str(), std::ios::binary);

  if (indexFile.is_open()) {
    indexFile.read((char *)&indexSize, 2);

    Init(indexSize);

    m_file_index = new int[indexSize];

    int i = 0;

    for (; i < indexSize; ++i) {
      indexFile.read((char *)&m_file_index[i], 4);
    }

    indexFile.close();

    bLoadIndexFile = true;
  } else {
    CT_Done("WARNING: CTypePack::LoadFromFileRunning - Index file NOT FOUND: %s", indexFileName.c_str());
#ifdef OUTPUT_DEBUG
    char szBuf[256];
    sprintf(szBuf, "CTypePack Can Not Find Index File: %s\n",
            indexFileName.c_str());
    OutputDebugString(szBuf);
#endif
  }

  // 데이터 파일을 읽을 준비를 한다.
  // 이때 분할 데이터와 통데이터는 분기처리한다.
  // m_bSplitFile = !iovfs_base::get_vfs()->IsFileExist(lpszFilename);

  // Improved Directory detection for VFS Bypass environments
  DWORD dwAttrs = GetFileAttributesA(lpszFilename);
  bool bIsDirectory = (dwAttrs != INVALID_FILE_ATTRIBUTES && (dwAttrs & FILE_ATTRIBUTE_DIRECTORY));
  m_bSplitFile = Phoenix_PackPathLooksLikeSplitFolder(lpszFilename, bIsDirectory);

  CT_Done("CTypePack::LoadFromFileRunning - Path: %s, Attrs: 0x%X, isDir: %d, isFileExist: %d, res_bSplitFile: %d", 
             lpszFilename, dwAttrs, bIsDirectory, iovfs_base::get_vfs()->IsFileExist(lpszFilename), m_bSplitFile);

  if (m_bSplitFile) {
    m_strFileName = lpszFilename;

    std::string strHeaderPath = m_strFileName;
    strHeaderPath += "\\header.inf";
    ivfstream fileHeader(strHeaderPath.c_str(), 0);

    if (!fileHeader.is_open()) {
      Release();
      return false;
    }

    const int bufferSize = 128;
    char lineBuffer[bufferSize];
    memset(lineBuffer, 0, bufferSize);

    fileHeader.read(lineBuffer, bufferSize);
    int bytesRead = fileHeader.gcount();
    
    log_hex("header.inf", lineBuffer, bytesRead, lpszFilename);

    const char* ptr = lineBuffer;
    const char* end = lineBuffer + bytesRead;

    m_Size = static_cast<WORD>(atoi_robust_next(ptr, end));
    m_nUnitCount = static_cast<int>(atoi_robust_next(ptr, end));
    m_nUnitTwoMultiplier = LogTwoMultiplier(m_nUnitCount);

    if (m_Size == 0) {
        CT_Done("FATAL: CTypePack::LoadFromFileRunning - m_Size IS ZERO for %s (Parsed from buffer)", lpszFilename);
    }

    fileHeader.close();

    // 확장자 저장
    char szExt[_MAX_EXT];
    _splitpath(lpszFilename, NULL, NULL, NULL, szExt);

    m_strFileExt = szExt;
    if (m_strFileExt.empty())
      m_strFileExt = ".spk";

    if (!bLoadIndexFile || indexSize != m_Size) {
      Init(m_Size);

      // char szBuf[256];
      // sprintf(szBuf, "CTypePack Invaild Index File: %s = %d, %s = %d\n",
      // lpszFilename, m_Size, indexFileName.c_str(), indexSize);

      // std::ofstream file3("栗都쇱꿴.log", std::ios::out | std::ios::app);
      // file3 << szBuf << std::endl;
      // file3.close();
      // #ifdef OUTPUT_DEBUG
      //			char szBuf[256];
      //			sprintf(szBuf, "CTypePack Invaild Index File: %s
      //= %d, %s = %d\n", 				lpszFilename, m_Size,
      // indexFileName.c_str(), indexSize);
      // OutputDebugString(szBuf); #endif
    }
  } else {
    if (m_file == NULL) {
      m_file = new ivfstream;
    }

    // Phoenix: non-split spk dalı — crash noktası öncesi/sonrası trace
    CT_Done("CTypePack::LoadFromFileRunning[non-split] OPEN %s", lpszFilename);
    m_file->open(lpszFilename, std::ios::binary);

    if (!m_file->is_open()) {
      CT_Done("CTypePack::LoadFromFileRunning[non-split] OPEN-FAIL %s", lpszFilename);
      Release();
      return false;
    }

    m_file->read((char *)&m_Size, 2);
    CT_Done("CTypePack::LoadFromFileRunning[non-split] %s m_Size=%d indexSize=%d bLoadIndex=%d",
               lpszFilename, (int)m_Size, (int)indexSize, (int)bLoadIndexFile);

    if (!bLoadIndexFile || indexSize != m_Size) {
      if (!bLoadIndexFile) {
          CT_Done("WARNING: CTypePack Stabilization - Index file missing. Continuing with raw size %d", m_Size);
      } else {
          CT_Done("WARNING: CTypePack Stabilization - Index size mismatch (%d != %d)", indexSize, m_Size);
      }
      Init(m_Size);
    }
  }

  m_bRunningLoad = true;
  CT_Done("CTypePack::LoadFromFileRunning DONE %s (m_Size=%d bSplit=%d)",
             lpszFilename, (int)m_Size, (int)m_bSplitFile);

  return true;
}

template <class Type>
bool CTypePack<Type>::SaveToFile(std::ofstream &dataFile,
                                 std::ofstream &indexFile) {
  //--------------------------------------------------
  // index file을 생성하기 위한 정보
  //--------------------------------------------------
  //	long*	pIndex = new long [m_Size];
  std::vector<DWORD> vIndex;

  //--------------------------------------------------
  // Size 저장
  //--------------------------------------------------
  dataFile.write((const char *)&m_Size, 2);
  indexFile.write((const char *)&m_Size, 2);
  WORD realSize = m_Size;
  DWORD index = 0;

  for (int i = 0; i < m_Size; i++) {
    index = dataFile.tellp();
    if (m_pData[i].SaveToFile(dataFile) == false) {
      realSize--;
    } else {
      vIndex.push_back(index);
    }
  }

  if (realSize != m_Size) {
    char szTemp[512];
    sprintf(szTemp, "real size : %d size : %d", realSize, m_Size);
    MessageBox(NULL, szTemp, "CTypePack", MB_OK);
    dataFile.seekp(0);
    dataFile.write((const char *)&realSize, 2);
    indexFile.seekp(0);
    indexFile.write((const char *)&realSize, 2);
  }

  //--------------------------------------------------
  // index 저장
  //--------------------------------------------------
  for (int i = 0; i < (int)vIndex.size(); i++) {
    indexFile.write((const char *)&vIndex[i], 4);
  }

  //	delete [] pIndex;

  //	indexFile.close();
  //	dataFile.close();

  if (m_bRunningLoad) {
    FinishRunningLoad();
  }

  return true;
}

template <class Type>
bool CTypePack<Type>::LoadFromFilePart(int first, int last) {
  if (!m_bRunningLoad)
    return true;

  last = min(last, 0xFFFE);

  int i = first;

  for (; i <= last; i++)
    Get(i);

  return true;
}

template <class Type>
bool CTypePack<Type>::LoadFromFilePart(CSpriteSetManager &SSM) {
  if (!m_bRunningLoad)
    return true;

  CSpriteSetManager::DATA_LIST::const_iterator iID = SSM.GetIterator();
  int t = 0;

  for (; t < SSM.GetSize(); ++t, ++iID) {
    if (*iID != 0xFFFF)
      Get(*iID);
  }

  return true;
}

template <class Type> bool CTypePack<Type>::ReleasePart(int first, int last) {
  last = min(last, 0xFFFE);

  if (m_bSplitFile) {
    int i = first;

    for (; i <= last; i++) {
      WORD fileIdx = GetSplitFile(i);

      FILECACHE_MAP::iterator pos = m_FileCacheMap.find(fileIdx);
      if (pos != m_FileCacheMap.end()) {
        delete pos->second;
        m_FileCacheMap.erase(pos);
      }

      for (int j = 0; j < m_nUnitCount && fileIdx < m_Size;
           ++j, ++i, ++fileIdx) {
        if (m_pData[fileIdx].IsInit())
          m_pData[fileIdx].Release();
      }
    }
  } else {
    int i = first;

    for (; i <= last; i++) {
      m_pData[i].Release();
    }
  }

  return true;
}

template <class Type>
bool CTypePack<Type>::ReleasePart(COrderedList<TYPE_SPRITEID> list) {
  COrderedList<TYPE_SPRITEID>::DATA_LIST::const_iterator iID =
      list.GetIterator();

  if (m_bSplitFile) {
    int t = 0;

    for (; t < list.GetSize(); t++, ++iID) {
      if (*iID == 0xFFFF)
        continue;

      WORD fileIdx = GetSplitFile(*iID);

      FILECACHE_MAP::iterator pos = m_FileCacheMap.find(fileIdx);
      if (pos != m_FileCacheMap.end()) {
        delete pos->second;
        m_FileCacheMap.erase(pos);
      }

      for (int i = 0; i < m_nUnitCount && fileIdx < m_Size; ++i, ++fileIdx) {
        if (m_pData[fileIdx].IsInit())
          m_pData[fileIdx].Release();
      }
    }
  } else {
    int t = 0;

    for (; t < list.GetSize(); t++, ++iID) {
      if (*iID == 0xFFFF)
        continue;
      m_pData[*iID].Release();
    }
  }

  return true;
}

template <class Type>
bool CTypePack<Type>::LoadFromFileData(int dataID, int fileID,
                                       LPCTSTR packFilename,
                                       LPCTSTR indexFilename) {
  if (dataID < 0 || dataID >= m_Size || fileID < 0) {
    return false;
  }

  //-------------------------------------------------------------------
  // load할 data의 file pointer를 읽는다.
  //-------------------------------------------------------------------
  bool bSplitFile = !iovfs_base::get_vfs()->IsFileExist(packFilename);

  if (bSplitFile) {
    m_strFileName = packFilename;

    std::string strHeaderPath = m_strFileName;
    strHeaderPath += "\\header.inf";
    ivfstream fileHeader(strHeaderPath.c_str(), 0);

    if (!fileHeader.is_open())
      return false;

    const int bufferSize = 8;
    char lineBuffer[bufferSize];

    fileHeader.getline(lineBuffer, bufferSize);
    int dataSize = static_cast<int>(atoi(lineBuffer));

    fileHeader.getline(lineBuffer, bufferSize);
    m_nUnitCount = static_cast<int>(atoi(lineBuffer));
    m_nUnitTwoMultiplier = LogTwoMultiplier(m_nUnitCount);

    fileHeader.close();

    if (fileID >= dataSize) {
      return false;
    }

    // 확장자 저장
    char szExt[_MAX_EXT];
    _splitpath(packFilename, NULL, NULL, NULL, szExt);

    m_strFileExt = szExt;
    if (m_strFileExt.empty())
      m_strFileExt = ".spk";

    char szFileName[MAX_PATH];

    int fileStartID = GetSplitFile(fileID, szFileName);

    ivfstream dataFile(szFileName, std::ios_base::binary);

    dataFile.seekg(2);

    do {
      m_pData[dataID].LoadFromFile(dataFile);
    } while (fileStartID++ < fileID);
  } else {
    // 	if (access(packFilename, 0) || access(indexFilename, 0))
    // 	{
    // 		return false;
    // 	}

    ivfstream dataFile(packFilename, std::ios::binary);

    if (!dataFile.is_open()) {
      return false;
    }

    ivfstream indexFile(indexFilename, std::ios::binary);

    if (!indexFile.is_open()) {
      return false;
    }

    //-------------------------------------------------------------------
    // index의 개수를 체크한다. fileID가 있는지..?
    //-------------------------------------------------------------------
    TYPE_SPRITEID num;
    indexFile.read((char *)&num, sizeof(WORD));

    if (fileID >= num) {
      return false;
    }

    long fp;
    indexFile.seekg(2 + fileID * 4); // 2(num) + spriteID * (4 bytes)
    indexFile.read((char *)&fp, 4);

    dataFile.seekg(fp);

    m_pData[dataID].LoadFromFile(dataFile);
  }

  return true;
}

template <class Type> void CTypePack<Type>::ClearFileCache() {
  FILECACHE_MAP::iterator pos = m_FileCacheMap.begin();
  for (; pos != m_FileCacheMap.end(); ++pos) {
    if (pos->second)
      delete pos->second;
  }
  m_FileCacheMap.clear();
}

template <class Type> void CTypePack<Type>::FinishRunningLoad() {
  m_bRunningLoad = false;

  ClearFileCache();

  if (m_file) {
    delete m_file;
    m_file = NULL;
  }

  if (m_file_index) {
    delete[] m_file_index;
    m_file_index = NULL;
  }
}

template <class Type> bool CTypePack<Type>::IsTwoMultiplier(int num) {
  int count = 0;

  for (int i = 0; i < 8; ++i) {

    if (num & 0x00000001) {
      if (count > 0)
        return false;
      ++count;
    }

    num >>= 1;
  }

  return count == 1;
}

template <class Type> int CTypePack<Type>::LogTwoMultiplier(int num) {
  if (num <= 0)
    return 0;

  for (int i = 0; i < 31; ++i) {
    if (num & 0x00000001)
      return i;
    num >>= 1;
  }

  return 0;
}

template <class Type> int CTypePack<Type>::GetSplitFile(int idx, char *szBuf) {
  WORD fileIdx = (idx >> m_nUnitTwoMultiplier) << m_nUnitTwoMultiplier;

  if (szBuf) {
    sprintf(szBuf, "%s\\%05d%s", m_strFileName.c_str(), fileIdx,
            m_strFileExt.c_str());
  }

  return fileIdx;
}

// CTypePack2
template <class TypeBase, class Type1, class Type2>
class CTypePack2 : public ITypePackBase {
public:
  CTypePack2();
  virtual ~CTypePack2();

  typedef std::map<WORD, ivfstream *> FILECACHE_MAP;

  //--------------------------------------------------------
  // Init/Release
  //--------------------------------------------------------
  virtual void Init(WORD size);
  virtual void Release();

  //--------------------------------------------------------
  // Size
  //--------------------------------------------------------
  virtual DWORD GetSize() const { return m_Size; }

  //--------------------------------------------------------
  // operator
  //--------------------------------------------------------
  virtual TypeBase &operator[](WORD n); //		{ return
                                        // m_pSpritePals[n]; }
  virtual TypeBase &Get(WORD n);
  virtual bool IsElementPrepared(WORD n) const;

  //--------------------------------------------------------
  // file I/O
  //--------------------------------------------------------
  virtual bool SaveToFile(std::ofstream &dataFile, std::ofstream &indexFile);

  virtual bool LoadFromFileRunning(LPCTSTR lpszFilename);
  virtual bool LoadFromFile(LPCTSTR lpszFilename);
  virtual bool LoadFromFilePart(int first, int last);
  virtual bool LoadFromFilePart(CSpriteSetManager &SSM);
  virtual bool LoadFromFileData(int dataID, int fileID, LPCTSTR packFilename,
                                LPCTSTR indexFilename);

  virtual bool ReleasePart(int first, int last);
  virtual bool ReleasePart(COrderedList<TYPE_SPRITEID> list);

  virtual bool SaveToFile(LPCTSTR lpszFilename);

  virtual bool Is565() { return m_bSecond; }

  virtual void FinishRunningLoad();

  // Split pack FileCacheMap = decrypt buffer birikimi; running load acik kalir.
  void ClearFileCache();
  size_t GetFileCacheCount() const { return m_FileCacheMap.size(); }

protected:
  static bool IsTwoMultiplier(int num);
  static int LogTwoMultiplier(int num); // Math Log, which only allows powers of 2
  int GetSplitFile(int idx, char *szBuf = NULL);
  void BuildDetectedSplitFileStarts();

protected:
  TypeBase *m_pData;
  WORD m_Size;
  bool m_bRunningLoad;

  // runtime loading

  ivfstream *m_file;
  int *m_file_index;

  bool m_bSplitFile;
  int m_nUnitCount;
  int m_nUnitTwoMultiplier; // (!) Store m_nUnitCount as a power of 2
  std::string m_strFileName;
  std::string m_strFileExt;
  std::vector<int> m_splitFileStarts;

  FILECACHE_MAP m_FileCacheMap;

  bool m_bSecond;
  bool m_bReleased = false;  // Double-free guard
};

template <class TypeBase, class Type1, class Type2>
CTypePack2<TypeBase, Type1, Type2>::CTypePack2() {
  m_pData = NULL;
  m_Size = 0;
  m_bRunningLoad = false;
  m_file_index = NULL;
  m_file = NULL;

  m_bSplitFile = false;
  m_nUnitCount = 0;
  m_nUnitTwoMultiplier = 0;
  
  m_bReleased = false;  // Double-free guard
}

template <class TypeBase, class Type1, class Type2>
CTypePack2<TypeBase, Type1, Type2>::~CTypePack2() {
  Release();
}

template <class TypeBase, class Type1, class Type2>
void CTypePack2<TypeBase, Type1, Type2>::Release() {
  if (m_bReleased) return;  // Guard
  m_bReleased = true;       // Set flag
  
  FinishRunningLoad();

  if (m_pData != NULL) {
    delete[] m_pData;
    m_pData = NULL;
    m_Size = 0;
  }

  m_splitFileStarts.clear();
}

template <class TypeBase, class Type1, class Type2>
void CTypePack2<TypeBase, Type1, Type2>::Init(WORD size) {
  if (size == 0)
    return;

  Release();

  m_Size = size;
  m_bSecond = CDirectDraw::Is565();

  if (m_bSecond == true)
    m_pData = new Type2[size];
  else
    m_pData = new Type1[size];
}

template <class TypeBase, class Type1, class Type2>
TypeBase &CTypePack2<TypeBase, Type1, Type2>::operator[](WORD n) {
  return Get(n);
}

template <class TypeBase, class Type1, class Type2>
bool CTypePack2<TypeBase, Type1, Type2>::IsElementPrepared(WORD n) const {
  if (m_pData == NULL || n >= m_Size)
    return false;
  return m_pData[n].IsInit();
}

template <class TypeBase, class Type1, class Type2>
TypeBase &CTypePack2<TypeBase, Type1, Type2>::Get(WORD n) {
  // 2026-05-10: Comprehensive NULL/init guards (CTypePack2 variant) —
  // CTypePack ile ayni gerekce: eksik .ispki yuzunden uninit sprite
  // dondurmek render path'inde NULL+0xBC access violation'a sebep oluyor.
  static Type1 s_safe_dummy_pack2;
  // 2026-09-01: CT_Done -> CT_OBJ rename + 500ms throttle (bkz. CTypePack::Get
  // aciklamasi — harita objesi teshisi, kural 4.1 silme yok).
  static DWORD s_ctObjFailTick2 = 0;
  if (m_pData == NULL) {
    DWORD ctNow = GetTickCount();
    if (ctNow - s_ctObjFailTick2 > 500) {
      s_ctObjFailTick2 = ctNow;
      CT_OBJ("ERROR: CTypePack2[%s]::Get(%d) - m_pData is NULL!", m_strFileName.c_str(), (int)n);
    }
    return s_safe_dummy_pack2;
  }
  if (n >= m_Size) {
    DWORD ctNow = GetTickCount();
    if (ctNow - s_ctObjFailTick2 > 500) {
      s_ctObjFailTick2 = ctNow;
      CT_OBJ("ERROR: CTypePack2[%s]::Get(%d) - OUT OF BOUNDS (Size=%d)!", m_strFileName.c_str(), (int)n, (int)m_Size);
    }
    if (m_Size > 0 && m_pData[0].IsInit()) return m_pData[0];
    return s_safe_dummy_pack2;
  }

  if (m_bRunningLoad && !m_pData[n].IsInit()) {
    if (Phoenix_ResourceStreaming_ShouldDeferLazyLoad((int)n, m_strFileName.c_str())) {
      Phoenix_ResourceStreaming_RecordDeferredLazyLoad((int)n, m_strFileName.c_str());
      return s_safe_dummy_pack2;
    }

    unsigned __int64 phoenixItemSpkLazyBegin = 0;
    if (Phoenix_ItemOutputPerf_IsCurrentItemActive())
      phoenixItemSpkLazyBegin = Phoenix_ItemOutputPerf_Begin();

    // file에 있는 Sprite들을 Load
    if (m_bSplitFile) {
      int fileIdx = GetSplitFile(n);

      FILECACHE_MAP::iterator pos = m_FileCacheMap.find(fileIdx);

      ivfstream *pFile = NULL;
      bool bCachedFile = false;

      // 파일이 캐시에 있으면 있는걸로 쓴다.
      if (pos != m_FileCacheMap.end()) {
        pFile = pos->second;
        bCachedFile = true;
      }
      // 파일이 캐시에 없으면 불러온다.
      else {
        char fileName[MAX_PATH];
        GetSplitFile(n, fileName);
        pFile = new ivfstream(fileName, std::ios_base::binary);
      }

      if (pFile == NULL || !pFile->is_open()) {
        char splitFileName[MAX_PATH];
        GetSplitFile(n, splitFileName);
        {
          DWORD ctNow = GetTickCount();
          if (ctNow - s_ctObjFailTick2 > 500) {
            s_ctObjFailTick2 = ctNow;
            CT_OBJ("ERROR: CTypePack2[%s]::Get(%d) split open failed fileIdx=%d file=%s",
                   m_strFileName.c_str(), (int)n, fileIdx, splitFileName);
          }
        }
        if (Phoenix_VisualAudit_IsEnabled()) {
          static DWORD s_lastSplitOpenAuditTick = 0;
          DWORD now = GetTickCount();
          if (now - s_lastSplitOpenAuditTick > 1000) {
            Phoenix_VisualAudit_Log("SPK_SPLIT_OPEN_FAIL",
                                    "pack=%s sprite=%d fileIdx=%d splitFile=%s size=%d unit=%d index=%s",
                                    m_strFileName.c_str(), (int)n, fileIdx, splitFileName,
                                    (int)m_Size, m_nUnitCount,
                                    m_file_index != NULL ? "yes" : "no");
            s_lastSplitOpenAuditTick = now;
          }
        }

        if (bCachedFile) {
          delete pFile;
          m_FileCacheMap.erase(pos);
        } else {
          delete pFile;
        }

        return s_safe_dummy_pack2;
      }

      WORD unitSize = 0;

      pFile->seekg(0);
      pFile->read((char *)&unitSize, 2);

      char splitFileName[MAX_PATH];
      GetSplitFile(n, splitFileName);

      std::vector<DWORD> localFileIndex;
      const bool bLoadLocalIndex = LoadLocalSplitIndex(splitFileName, localFileIndex);
      const int localIndex = n - fileIdx;

      // Split packlerde unit-local index dosyasi varsa onu tercih et.
      if (bLoadLocalIndex && localIndex >= 0 && localIndex < (int)localFileIndex.size()) {
        pFile->seekg(localFileIndex[localIndex]);
        m_pData[n].LoadFromFile(*pFile);
      }
      // Unit-local index yoksa eski global index davranisini koru.
      else if (m_file_index) {
        int offset = m_file_index[n] - m_file_index[fileIdx];

        pFile->seekg(offset + 2);

        m_pData[n].LoadFromFile(*pFile);
      }
      else {
        pFile->seekg(2);

        for (int i = 0; i < unitSize && fileIdx + i < m_Size; ++i) {
          m_pData[fileIdx + i].LoadFromFile(*pFile);
        }
      }

      if (!m_pData[n].IsInit()) {
        ivfstream fallbackFile(splitFileName, std::ios_base::binary);
        ivfstream* pSequentialFile = fallbackFile.is_open() ? &fallbackFile : pFile;

        if (fallbackFile.is_open()) {
          WORD fallbackUnitSize = 0;
          fallbackFile.seekg(0);
          fallbackFile.read((char *)&fallbackUnitSize, 2);
          if (fallbackUnitSize != 0)
            unitSize = fallbackUnitSize;
        }

        pSequentialFile->seekg(2);

        for (int i = 0; i < unitSize && fileIdx + i < m_Size; ++i) {
          m_pData[fileIdx + i].LoadFromFile(*pSequentialFile);
        }
      }

      bool bLoadComplete = true;

      for (int i = 0; i < unitSize && fileIdx + i < m_Size; ++i) {
        if (!m_pData[fileIdx + i].IsInit()) {
          bLoadComplete = false;
          break;
        }
      }

      if (bLoadComplete) {
        delete pFile;

        if (pos != m_FileCacheMap.end())
          m_FileCacheMap.erase(pos);
      } else {
        if (pos == m_FileCacheMap.end())
          m_FileCacheMap.insert(FILECACHE_MAP::value_type(fileIdx, pFile));
      }
    } else {
      if (m_file != NULL && m_file->is_open()) {
        if (m_file_index != NULL) {
          m_file->seekg(m_file_index[n]);
          m_pData[n].LoadFromFile(*m_file);
        } else {
          m_file->seekg(2);
          for (WORD walk = 0; walk <= n && walk < m_Size; ++walk) {
            if (!m_pData[walk].IsInit())
              m_pData[walk].LoadFromFile(*m_file);
          }
        }
      }
    }
    if (phoenixItemSpkLazyBegin != 0)
    {
      Phoenix_ItemOutputPerf_RecordSpkLazyLoad((int)n, phoenixItemSpkLazyBegin);
      Phoenix_ResourceStreaming_RecordBlockingLazyLoad((int)n, m_strFileName.c_str(),
                                                       phoenixItemSpkLazyBegin);
    }
  }

  // 2026-05-10: Final init check (CTypePack2 variant) — CTypePack ile ayni gerekce.
  if (!m_pData[n].IsInit()) {
    {
      DWORD ctNow = GetTickCount();
      if (ctNow - s_ctObjFailTick2 > 500) {
        s_ctObjFailTick2 = ctNow;
        CT_OBJ("WARNING: CTypePack2[%s]::Get(%d) - sprite NOT initialized after load attempt, returning safe dummy",
               m_strFileName.c_str(), (int)n);
      }
    }
    if (Phoenix_VisualAudit_IsEnabled()) {
      static DWORD s_lastUninitAuditTick = 0;
      DWORD now = GetTickCount();
      if (now - s_lastUninitAuditTick > 1000) {
        int fileIdx = m_bSplitFile ? GetSplitFile(n) : -1;
        char splitFileName[MAX_PATH] = "";
        if (m_bSplitFile)
          GetSplitFile(n, splitFileName);
        Phoenix_VisualAudit_Log("SPK_DUMMY_SPRITE",
                                "pack=%s sprite=%d split=%d fileIdx=%d splitFile=%s size=%d unit=%d index=%s",
                                m_strFileName.c_str(), (int)n, (int)m_bSplitFile,
                                fileIdx, splitFileName, (int)m_Size, m_nUnitCount,
                                m_file_index != NULL ? "yes" : "no");
        s_lastUninitAuditTick = now;
      }
    }
    return s_safe_dummy_pack2;
  }

  return m_pData[n];
}

template <class TypeBase, class Type1, class Type2>
bool CTypePack2<TypeBase, Type1, Type2>::LoadFromFile(LPCTSTR lpszFilename) {
  if (iovfs_base::get_vfs()->IsFileExist(lpszFilename)) {
    ivfstream file(lpszFilename, std::ios_base::binary);
    file.read((char *)&m_Size, 2);

    Init(m_Size);

    int i = 0;

    for (; i < m_Size; i++) {
      m_pData[i].LoadFromFile(file);
    }
  } else {

    if (!LoadFromFileRunning(lpszFilename))
      return false;

    // Phoenix diag: Get(i) loop crash izleme
    CT_Done("CTypePack::LoadFromFile GET-LOOP-START %s m_Size=%d", lpszFilename, (int)m_Size);

    int i = 0;
    for (; i < m_Size; ++i) {
      Get(i);
    }

    CT_Done("CTypePack::LoadFromFile GET-LOOP-END %s", lpszFilename);

    if (m_bRunningLoad)
      return false;
  }

  return true;
}

template <class TypeBase, class Type1, class Type2>
bool CTypePack2<TypeBase, Type1, Type2>::SaveToFile(LPCTSTR lpszFilename) {
  char szIndexFilename[512];
  sprintf(szIndexFilename, "%si", lpszFilename);

  std::ofstream dataFile(lpszFilename, std::ios::binary);
  std::ofstream indexFile(szIndexFilename, std::ios::binary);

  bool re = SaveToFile(dataFile, indexFile);

  dataFile.close();
  indexFile.close();

  return re;
}

//----------------------------------------------------------------------
// Load From File Running
//----------------------------------------------------------------------
// 실시간 로딩
//----------------------------------------------------------------------
template <class TypeBase, class Type1, class Type2>
bool CTypePack2<TypeBase, Type1, Type2>::LoadFromFileRunning(
    LPCTSTR lpszFilename) {
  Release();

  // 다음으로 인덱스 파일이 있다면 로드한다.
  std::string indexFileName = lpszFilename;
  indexFileName += 'i';

  WORD indexSize = 0;
  bool bLoadIndexFile = false;

  ivfstream indexFile(indexFileName.c_str(), std::ios::binary);

  if (indexFile.is_open()) {
    indexFile.read((char *)&indexSize, 2);

    Init(indexSize);

    m_file_index = new int[indexSize];

    int i = 0;

    for (; i < indexSize; ++i) {
      // if ( i == 1599 )
      //	int a = 0;
      indexFile.read((char *)&m_file_index[i], 4);
    }

    indexFile.close();

    bLoadIndexFile = true;
  } else {
#ifdef OUTPUT_DEBUG
    char szBuf[256];
    sprintf(szBuf, "CTypePack Can Not Find Index File: %s\n",
            indexFileName.c_str());
    OutputDebugString(szBuf);
#endif
  }

  // 데이터 파일을 읽을 준비를 한다.
  // 이때 분할 데이터와 통데이터는 분기처리한다.
  // m_bSplitFile = !iovfs_base::get_vfs()->IsFileExist(lpszFilename);

  // Improved Directory detection for VFS Bypass environments
  DWORD dwAttrs2 = GetFileAttributesA(lpszFilename);
  bool bIsDirectory2 = (dwAttrs2 != INVALID_FILE_ATTRIBUTES && (dwAttrs2 & FILE_ATTRIBUTE_DIRECTORY));
  m_bSplitFile = Phoenix_PackPathLooksLikeSplitFolder(lpszFilename, bIsDirectory2);

  CT_Done("CTypePack2::LoadFromFileRunning - Path: %s, Attrs: 0x%X, isDir: %d, isFileExist: %d, res_bSplitFile: %d", 
             lpszFilename, dwAttrs2, bIsDirectory2, iovfs_base::get_vfs()->IsFileExist(lpszFilename), m_bSplitFile);

  if (m_bSplitFile) {
    m_strFileName = lpszFilename;

    std::string strHeaderPath = m_strFileName + "\\header.inf";
    ivfstream fileHeader(strHeaderPath.c_str(), 0);

    if (!fileHeader.is_open()) {
      Release();
      return false;
    }

    const int bufferSize = 128;
    char lineBuffer[bufferSize];
    memset(lineBuffer, 0, bufferSize);

    fileHeader.read(lineBuffer, bufferSize);
    int bytesRead = fileHeader.gcount();
    
    log_hex("header.inf", lineBuffer, bytesRead, lpszFilename);

    const char* ptr = lineBuffer;
    const char* end = lineBuffer + bytesRead;

    m_Size = static_cast<WORD>(atoi_robust_next(ptr, end));
    m_nUnitCount = static_cast<int>(atoi_robust_next(ptr, end));
    m_nUnitTwoMultiplier = LogTwoMultiplier(m_nUnitCount);

    if (m_Size == 0) {
        CT_Done("FATAL: CTypePack2::LoadFromFileRunning - m_Size IS ZERO for %s (Parsed from buffer)", lpszFilename);
    }

    fileHeader.close();

    // 확장자 저장
    char szExt[_MAX_EXT];
    _splitpath(lpszFilename, NULL, NULL, NULL, szExt);

    m_strFileExt = szExt;
    if (m_strFileExt.empty())
      m_strFileExt = ".spk";

    if (!bLoadIndexFile || indexSize != m_Size) {
      if (Phoenix_VisualAudit_IsEnabled()) {
        Phoenix_VisualAudit_Log("SPK_INDEX_CONTRACT",
                                "pack=%s split=1 headerSize=%d indexLoaded=%d indexSize=%d headerUnit=%d",
                                m_strFileName.c_str(), (int)m_Size,
                                (int)bLoadIndexFile, (int)indexSize, m_nUnitCount);
      }

      if (m_file_index) {
        delete m_file_index;
        m_file_index = NULL;
      }

      Init(m_Size);

      // char szBuf[256];
      // sprintf(szBuf, "CTypePack Invaild Index File: %s = %d, %s = %d\n",
      // lpszFilename, m_Size, indexFileName.c_str(), indexSize);
      //
      // std::ofstream file3("栗都쇱꿴2.log", std::ios::out | std::ios::app);
      // file3 << szBuf << std::endl;
      // file3.close();
      // #ifdef OUTPUT_DEBUG
      //			char szBuf[256];
      //			sprintf(szBuf, "CTypePack Invaild Index File: %s
      //= %d, %s = %d\n", 				lpszFilename, m_Size,
      // indexFileName.c_str(), indexSize);
      // OutputDebugString(szBuf); #endif
    }
    BuildDetectedSplitFileStarts();
  } else {
    if (m_file == NULL) {
      m_file = new ivfstream;
    }

    // Phoenix: non-split spk dalı — crash noktası öncesi/sonrası trace
    CT_Done("CTypePack::LoadFromFileRunning[non-split] OPEN %s", lpszFilename);
    m_file->open(lpszFilename, std::ios::binary);

    if (!m_file->is_open()) {
      CT_Done("CTypePack::LoadFromFileRunning[non-split] OPEN-FAIL %s", lpszFilename);
      Release();
      return false;
    }

    m_file->read((char *)&m_Size, 2);
    CT_Done("CTypePack::LoadFromFileRunning[non-split] %s m_Size=%d indexSize=%d bLoadIndex=%d",
               lpszFilename, (int)m_Size, (int)indexSize, (int)bLoadIndexFile);

    if (!bLoadIndexFile || indexSize != m_Size) {
      if (!bLoadIndexFile) {
          CT_Done("WARNING: CTypePack Stabilization - Index file missing. Continuing with raw size %d", m_Size);
      } else {
          CT_Done("WARNING: CTypePack Stabilization - Index size mismatch (%d != %d)", indexSize, m_Size);
      }
      Init(m_Size);
    }
  }

  m_bRunningLoad = true;
  CT_Done("CTypePack::LoadFromFileRunning DONE %s (m_Size=%d bSplit=%d)",
             lpszFilename, (int)m_Size, (int)m_bSplitFile);

  return true;
}

template <class TypeBase, class Type1, class Type2>
bool CTypePack2<TypeBase, Type1, Type2>::SaveToFile(std::ofstream &dataFile,
                                                    std::ofstream &indexFile) {
  //--------------------------------------------------
  // index file을 생성하기 위한 정보
  //--------------------------------------------------
  //	long*	pIndex = new long [m_Size];
  std::vector<DWORD> vIndex;

  //--------------------------------------------------
  // Size 저장
  //--------------------------------------------------
  dataFile.write((const char *)&m_Size, 2);
  indexFile.write((const char *)&m_Size, 2);
  WORD realSize = m_Size;
  DWORD index = 0;

  for (int i = 0; i < m_Size; i++) {
    index = dataFile.tellp();
    if (m_pData[i].SaveToFile(dataFile) == false) {
      realSize--;
    } else {
      vIndex.push_back(index);
    }
  }

  if (realSize != m_Size) {
    char szTemp[512];
    sprintf(szTemp, "real size : %d size : %d", realSize, m_Size);
    MessageBox(NULL, szTemp, "CTypePack2", MB_OK);
    dataFile.seekp(0);
    dataFile.write((const char *)&realSize, 2);
    indexFile.seekp(0);
    indexFile.write((const char *)&realSize, 2);
  }

  //--------------------------------------------------
  // index 저장
  //--------------------------------------------------
  for (int i = 0; i < (int)vIndex.size(); i++) {
    indexFile.write((const char *)&vIndex[i], 4);
  }

  //	delete [] pIndex;

  //	indexFile.close();
  //	dataFile.close();

  if (m_bRunningLoad) {
    FinishRunningLoad();
  }

  return true;
}

template <class TypeBase, class Type1, class Type2>
bool CTypePack2<TypeBase, Type1, Type2>::LoadFromFilePart(int first, int last) {
  bool bTemporarySplitLoad = false;
  if (!m_bRunningLoad) {
    if (!m_bSplitFile || m_pData == NULL || m_Size == 0 ||
        m_strFileName.empty() || m_nUnitCount <= 0) {
      return true;
    }

    m_bRunningLoad = true;
    bTemporarySplitLoad = true;
  }

  if (first < 0 || first >= m_Size) {
    if (bTemporarySplitLoad)
      m_bRunningLoad = false;
    return true;
  }

  last = min(last, 0xFFFE);
  last = min(last, (int)m_Size - 1);

  int i = first;

  for (; i <= last; i++)
    Get(i);

  if (bTemporarySplitLoad) {
    FILECACHE_MAP::iterator pos = m_FileCacheMap.begin();
    for (; pos != m_FileCacheMap.end(); ++pos) {
      ivfstream *pVF = pos->second;
      if (pVF)
        delete pVF;
    }

    m_FileCacheMap.clear();
    m_bRunningLoad = false;
  }

  return true;
}

template <class TypeBase, class Type1, class Type2>
bool CTypePack2<TypeBase, Type1, Type2>::LoadFromFilePart(
    CSpriteSetManager &SSM) {
  if (!m_bRunningLoad)
    return true;

  CSpriteSetManager::DATA_LIST::const_iterator iID = SSM.GetIterator();
  int t = 0;

  for (; t < SSM.GetSize(); ++t, ++iID) {
    if (*iID != 0xFFFF)
      Get(*iID);
  }

  return true;
}

/*
template <class TypeBase, class Type1, class Type2>
bool CTypePack2<TypeBase, Type1, Type2>::LoadFromFilePart(int first, int last)
{
        last = min(last, 0xFFFE);
        for(int i = first; i <= last; i++)
                operator[](i);

        return true;
}

template <class TypeBase, class Type1, class Type2>
bool CTypePack2<TypeBase, Type1, Type2>::LoadFromFilePart(const
CSpriteSetManager& SSM)
{
        CSpriteSetManager::DATA_LIST::const_iterator iID = SSM.GetIterator();
        for (int t=0; t<SSM.GetSize(); t++)
        {
                if(*iID != 0xFFFF)
                        Get(*iID);
        }

        return true;
}
*/

template <class TypeBase, class Type1, class Type2>
bool CTypePack2<TypeBase, Type1, Type2>::ReleasePart(int first, int last) {
  last = min(last, 0xFFFE);

  if (m_bSplitFile) {
    int i = first;

    for (; i <= last; i++) {
      WORD fileIdx = GetSplitFile(i);

      FILECACHE_MAP::iterator pos = m_FileCacheMap.find(fileIdx);
      if (pos != m_FileCacheMap.end()) {
        delete pos->second;
        m_FileCacheMap.erase(pos);
      }

      for (int j = 0; j < m_nUnitCount && fileIdx < m_Size;
           ++j, ++i, ++fileIdx) {
        if (m_pData[fileIdx].IsInit())
          m_pData[fileIdx].Release();
      }
    }
  } else {
    int i = first;

    for (; i <= last; i++) {
      m_pData[i].Release();
    }
  }

  return true;
}

template <class TypeBase, class Type1, class Type2>
bool CTypePack2<TypeBase, Type1, Type2>::ReleasePart(
    COrderedList<TYPE_SPRITEID> list) {
  COrderedList<TYPE_SPRITEID>::DATA_LIST::const_iterator iID =
      list.GetIterator();

  if (m_bSplitFile) {
    int t = 0;

    for (; t < list.GetSize(); t++, ++iID) {
      if (*iID == 0xFFFF)
        continue;

      WORD fileIdx = GetSplitFile(*iID);

      FILECACHE_MAP::iterator pos = m_FileCacheMap.find(fileIdx);
      if (pos != m_FileCacheMap.end()) {
        delete pos->second;
        m_FileCacheMap.erase(pos);
      }

      for (int i = 0; i < m_nUnitCount && fileIdx < m_Size; ++i, ++fileIdx) {
        if (m_pData[fileIdx].IsInit())
          m_pData[fileIdx].Release();
      }
    }
  } else {
    int t = 0;

    for (; t < list.GetSize(); t++, ++iID) {
      if (*iID == 0xFFFF)
        continue;
      m_pData[*iID].Release();
    }
  }

  return true;
}

template <class TypeBase, class Type1, class Type2>
bool CTypePack2<TypeBase, Type1, Type2>::LoadFromFileData(
    int dataID, int fileID, LPCTSTR packFilename, LPCTSTR indexFilename) {
  if (dataID < 0 || dataID >= m_Size || fileID < 0) {
    return false;
  }

  //-------------------------------------------------------------------
  // load할 data의 file pointer를 읽는다.
  //-------------------------------------------------------------------
  bool bSplitFile = !iovfs_base::get_vfs()->IsFileExist(packFilename);

  if (bSplitFile) {
    m_strFileName = packFilename;

    std::string strHeaderPath = m_strFileName + "\\header.inf";
    ivfstream fileHeader(strHeaderPath.c_str(), 0);

    if (!fileHeader.is_open())
      return false;

    const int bufferSize = 8;
    char lineBuffer[bufferSize];

    fileHeader.getline(lineBuffer, bufferSize);
    int dataSize = static_cast<int>(atoi(lineBuffer));

    fileHeader.getline(lineBuffer, bufferSize);
    m_nUnitCount = static_cast<int>(atoi(lineBuffer));
    m_nUnitTwoMultiplier = LogTwoMultiplier(m_nUnitCount);

    fileHeader.close();

    if (fileID >= dataSize) {
      return false;
    }

    // 확장자 저장
    char szExt[_MAX_EXT];
    _splitpath(packFilename, NULL, NULL, NULL, szExt);

    m_strFileExt = szExt;
    if (m_strFileExt.empty())
      m_strFileExt = ".spk";
    BuildDetectedSplitFileStarts();

    char szFileName[MAX_PATH];

    int fileStartID = GetSplitFile(fileID, szFileName);

    ivfstream dataFile(szFileName, std::ios_base::binary);

    dataFile.seekg(2);

    do {
      m_pData[dataID].LoadFromFile(dataFile);
    } while (fileStartID++ < fileID);
  } else {
    // 	if (access(packFilename, 0) || access(indexFilename, 0))
    // 	{
    // 		return false;
    // 	}

    ivfstream dataFile(packFilename, std::ios::binary);

    if (!dataFile.is_open()) {
      return false;
    }

    ivfstream indexFile(indexFilename, std::ios::binary);

    if (!indexFile.is_open()) {
      return false;
    }

    //-------------------------------------------------------------------
    // index의 개수를 체크한다. fileID가 있는지..?
    //-------------------------------------------------------------------
    TYPE_SPRITEID num;
    indexFile.read((char *)&num, sizeof(WORD));

    if (fileID >= num) {
      return false;
    }

    long fp;
    indexFile.seekg(2 + fileID * 4); // 2(num) + spriteID * (4 bytes)
    indexFile.read((char *)&fp, 4);

    dataFile.seekg(fp);

    m_pData[dataID].LoadFromFile(dataFile);
  }

  return true;
}

template <class TypeBase, class Type1, class Type2>
void CTypePack2<TypeBase, Type1, Type2>::ClearFileCache() {
  FILECACHE_MAP::iterator pos = m_FileCacheMap.begin();
  for (; pos != m_FileCacheMap.end(); ++pos) {
    if (pos->second)
      delete pos->second;
  }
  m_FileCacheMap.clear();
}

template <class TypeBase, class Type1, class Type2>
void CTypePack2<TypeBase, Type1, Type2>::FinishRunningLoad() {
  if (m_bReleased) return;  // Guard
  m_bRunningLoad = false;

  ClearFileCache();

  if (m_file) {
    delete m_file;
    m_file = NULL;
  }

  if (m_file_index) {
    delete[] m_file_index;
    m_file_index = NULL;
  }
}

template <class TypeBase, class Type1, class Type2>
bool CTypePack2<TypeBase, Type1, Type2>::IsTwoMultiplier(int num) {
  int count = 0;

  for (int i = 0; i < 8; ++i) {

    if (num & 0x00000001) {
      if (count > 0)
        return false;
      ++count;
    }

    num >>= 1;
  }

  return count == 1;
}

template <class TypeBase, class Type1, class Type2>
int CTypePack2<TypeBase, Type1, Type2>::LogTwoMultiplier(int num) {
  if (num <= 0)
    return 0;

  for (int i = 0; i < 31; ++i) {
    if (num & 0x00000001)
      return i;
    num >>= 1;
  }

  return 0;
}

template <class TypeBase, class Type1, class Type2>
void CTypePack2<TypeBase, Type1, Type2>::BuildDetectedSplitFileStarts() {
  m_splitFileStarts.clear();

  if (m_strFileName.empty() || m_strFileExt.empty() || m_Size == 0)
    return;

  // Mixed 32/16 chunk layout is confirmed for skillminirenewal2 only.
  // Keep every other split pack on the legacy header-based path.
  if (m_strFileName.find("skillminirenewal2.spk") == std::string::npos &&
      m_strFileName.find("skillminirenewal2.SPK") == std::string::npos)
    return;

  char pattern[MAX_PATH];
  sprintf(pattern, "%s\\*%s", m_strFileName.c_str(), m_strFileExt.c_str());

  WIN32_FIND_DATAA findData;
  HANDLE hFind = FindFirstFileA(pattern, &findData);
  if (hFind == INVALID_HANDLE_VALUE) {
    CT_Done("WARNING: CTypePack2[%s] split scan found no chunk files pattern=%s",
            m_strFileName.c_str(), pattern);
    return;
  }

  do {
    if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      continue;

    const char* fileName = findData.cFileName;
    const char* dot = strrchr(fileName, '.');
    if (dot == NULL || _stricmp(dot, m_strFileExt.c_str()) != 0)
      continue;

    bool bNumericName = true;
    for (const char* p = fileName; p < dot; ++p) {
      if (!isdigit((unsigned char)*p)) {
        bNumericName = false;
        break;
      }
    }

    if (!bNumericName || dot == fileName)
      continue;

    const int fileStart = atoi(fileName);
    if (fileStart < 0 || fileStart >= m_Size)
      continue;

    m_splitFileStarts.push_back(fileStart);
  } while (FindNextFileA(hFind, &findData));

  FindClose(hFind);

  if (m_splitFileStarts.empty())
    return;

  std::sort(m_splitFileStarts.begin(), m_splitFileStarts.end());
  m_splitFileStarts.erase(
      std::unique(m_splitFileStarts.begin(), m_splitFileStarts.end()),
      m_splitFileStarts.end());

  bool bHeaderUnitMismatch = false;
  int firstMismatchStart = -1;
  WORD firstMismatchUnit = 0;

  for (size_t i = 0; i < m_splitFileStarts.size(); ++i) {
    char chunkName[MAX_PATH];
    sprintf(chunkName, "%s\\%05d%s", m_strFileName.c_str(), m_splitFileStarts[i],
            m_strFileExt.c_str());

    ivfstream chunkFile(chunkName, std::ios_base::binary);
    if (!chunkFile.is_open())
      continue;

    WORD unitSize = 0;
    chunkFile.read((char*)&unitSize, 2);
    if (chunkFile.gcount() == 2 && unitSize != 0 && unitSize != m_nUnitCount) {
      bHeaderUnitMismatch = true;
      firstMismatchStart = m_splitFileStarts[i];
      firstMismatchUnit = unitSize;
      break;
    }
  }

  if (bHeaderUnitMismatch) {
    CT_Done("CTypePack2[%s] split layout differs from header: headerUnit=%d firstMismatchStart=%d chunkUnit=%d detectedChunks=%d",
            m_strFileName.c_str(), m_nUnitCount, firstMismatchStart,
            (int)firstMismatchUnit, (int)m_splitFileStarts.size());
    if (Phoenix_VisualAudit_IsEnabled()) {
      Phoenix_VisualAudit_Log("SPK_SPLIT_LAYOUT",
                              "pack=%s headerUnit=%d firstMismatchStart=%d chunkUnit=%d detectedChunks=%d",
                              m_strFileName.c_str(), m_nUnitCount, firstMismatchStart,
                              (int)firstMismatchUnit, (int)m_splitFileStarts.size());
    }
  }
}

template <class TypeBase, class Type1, class Type2>
int CTypePack2<TypeBase, Type1, Type2>::GetSplitFile(int idx, char *szBuf) {
  int fileIdx = (idx >> m_nUnitTwoMultiplier) << m_nUnitTwoMultiplier;

  if (!m_splitFileStarts.empty()) {
    std::vector<int>::const_iterator it =
        std::upper_bound(m_splitFileStarts.begin(), m_splitFileStarts.end(), idx);
    if (it != m_splitFileStarts.begin()) {
      --it;
      fileIdx = *it;
    } else {
      fileIdx = m_splitFileStarts.front();
    }
  }

  if (szBuf) {
    sprintf(szBuf, "%s\\%05d%s", m_strFileName.c_str(), fileIdx,
            m_strFileExt.c_str());
  }

  return fileIdx;
}

#endif
