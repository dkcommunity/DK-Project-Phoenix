//----------------------------------------------------------------------
// UpdateUtility.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include <io.h>
#include <direct.h>

#include "PackageUpdateUtility.h"

#include "CFileIndexTable.h"
#include "VirtualFileSystem/vfstream.h"
typedef ivfstream ipfstream;
typedef ovfstream opfstream;

#include "PackagePath.h"
#include "PackageCache.h"

#include "FileIO.h"
#include "zzip/lib.h"


#define	NULL	0

//----------------------------------------------------------------------
// Pack의 Size의 byte 수
//----------------------------------------------------------------------
typedef	unsigned short		TYPE_PACKSIZE;
#define	SIZE_PACKSIZE		2

//----------------------------------------------------------------------
// Copy Package Inner File
//----------------------------------------------------------------------
// SourceFile : 원본
// TargetFile : 목적 filename
//----------------------------------------------------------------------
// SourceFile을 TargetFile로 copy해서 새로운 file을 생성한다.
//----------------------------------------------------------------------
bool
PUUFCopyFile(const char* FilenameSource, const char* FilenameTarget)
{				
	if (UUFHasPermission( FilenameTarget ) && UUFHasPermission( FilenameSource))				
	{
		std::ifstream fileSource(FilenameSource, std::ios_base::binary);
		opfstream fileTarget(FilenameTarget, std::ios_base::binary);
		
		// 추가
		char buffer[SIZE_BUFFER];
		int n;
		
		//---------------------------------------------------------------
		// addFile을 읽어서 originalFile의 끝에 붙인다.
		//---------------------------------------------------------------
		while (1)
		{
			fileSource.read(buffer, SIZE_BUFFER);
			
			n = fileSource.gcount();

			if (n > 0)
			{		
				fileTarget.write(buffer, n);
			}
			else
			{
				break;
			}
		}

		fileSource.close();
		fileTarget.close();		

		return true;
	}

	return false;
}
					
//----------------------------------------------------------------------
// Move Package Inner File
//----------------------------------------------------------------------
// SourceFile : 원본
// TargetFile : 목적 filename
//----------------------------------------------------------------------
// SourceFile을 TargetFile로 rename해서 새로운 file을 생성한다.
//----------------------------------------------------------------------
bool
PUUFMoveFile(const char* FilenameSource, const char* FilenameTarget)
{	
	//---------------------------------------------------------------
	// PPD 파일에 정보가 없다면 일반 파일이다.
	//---------------------------------------------------------------
	if(!PUUFIsPackageInnerFile(FilenameTarget))
		return UUFMoveFile(FilenameSource, FilenameTarget);
	
	if (UUFHasPermission( FilenameTarget ) && UUFHasPermission( FilenameSource))				
	{	
		std::ifstream fileSource(FilenameSource, std::ios_base::binary);
		opfstream fileTarget(FilenameTarget, std::ios_base::binary);
		
		// 추가
		char buffer[SIZE_BUFFER];
		int n;
		
		//---------------------------------------------------------------
		// addFile을 읽어서 originalFile에 복사한다.
		//---------------------------------------------------------------
		while (1)
		{
			fileSource.read(buffer, SIZE_BUFFER);
			
			n = fileSource.gcount();

			if (n > 0)
			{		
				fileTarget.write(buffer, n);
			}
			else
			{
				break;
			}
		}

		fileSource.close();
		fileTarget.close();		
		
		DeleteFile(FilenameTarget);
		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Delete Package Inner File
//----------------------------------------------------------------------
// TargetFile : 삭제할 filename
//----------------------------------------------------------------------
// TargetFile을 지운다.
//----------------------------------------------------------------------
bool
PUUFDeleteFile(const char* FilenameTarget)
{
	//---------------------------------------------------------------
	// PPD 파일에 정보가 없다면 일반 파일이다.
	//---------------------------------------------------------------
	const PACKAGEINNERPATH_DATA* pPPD = CPackagePath::Instance()->GetPackageInnerPathData(FilenameTarget);

	if(!pPPD)
		return !remove( FilenameTarget );

	// file을 지운다.
	if (UUFHasPermission( FilenameTarget ))
	{
		LPCTSTR packagePath = CPackagePath::Instance()->GetPackagePath(pPPD->nPackageIdx);
		LPCTSTR innerPath	= pPPD->strInnerPath.c_str();

		//---------------------------------------------------------------
		// 이 파일을 바뀐것으로 체크해두면 패키지에
		// 업데이트되지 않으므로 자동으로 삭제된다.
		//---------------------------------------------------------------
		CCompressedRepackingFileIO::set_change_package(packagePath, innerPath);
	}

	return false;
}



//----------------------------------------------------------------------
// Append Package Inner Pack
//----------------------------------------------------------------------
// SourceFile		: Pack(to add)
// TargetFile		: Pack(original)
// SourceIndexFile	: PackIndex(to add)
// TargetIndexFile	: PackIndex(original)
//----------------------------------------------------------------------
// TargetFile의 끝에 SourceFile을 추가한다.
// 단, TargetFile의 전체 개수를 추가된 것 만큼 증가해야 한다.    
//----------------------------------------------------------------------
bool
PUUFAppendPack(const char* FilenameAdd, const char* FilenameOriginal,
			const char* FilenameIndexAdd, const char* FilenameIndexOriginal)
{	
	//---------------------------------------------------------------
	// PPD 파일에 정보가 없다면 일반 파일이다.
	//---------------------------------------------------------------
	if(!PUUFIsPackageInnerFile(FilenameOriginal))
		return UUFAppendPack(FilenameAdd, FilenameOriginal, FilenameIndexAdd, FilenameIndexOriginal);


	//---------------------------------------------------------------
	//
	//					Pack을 추가한다.
	//
	//---------------------------------------------------------------
	if(!PUUFAppendPackImpl<TYPE_PACKSIZE>(FilenameAdd, FilenameOriginal))
		return false;

	//--------------------------------------------------
	// Index도 추가하는가?
	//--------------------------------------------------
	bool bAppendIndex = FilenameIndexAdd != NULL || FilenameIndexOriginal != NULL;

	//--------------------------------------------------
	// permission 체크
	//--------------------------------------------------
	if(!bAppendIndex || UUFHasPermission( FilenameIndexOriginal ) && UUFHasPermission( FilenameIndexAdd ))		
	{
		//---------------------------------------------------------------
		//
		//					Pack Index를 추가한다.
		//
		//---------------------------------------------------------------	
		
		// Index를 추가할려는 경우에만..
		if (bAppendIndex)
		{
			//---------------------------------------------------------------
			// 정해진 Filesize랑 같은가?
			//---------------------------------------------------------------
			//if (!IsFileSizeBeforeOK())
			//{
				// Update하려고 하는데 원하는 Filesize가 아니면.. 심각하다!!						
			//	return false;
			//}

			//---------------------------------------------------------------
			// original 파일 사이즈를 얻는다.
			//---------------------------------------------------------------
			long originalPackFileSize = PUUFGetFileSize(FilenameOriginal);

			//---------------------------------------------------------------
			// original index의 크기를 읽는다.
			//---------------------------------------------------------------
			TYPE_PACKSIZE	targetCount;

			ipfstream originalIndexFile(FilenameIndexOriginal, std::ios_base::binary);
			
			originalIndexFile.read((char*)&targetCount, SIZE_PACKSIZE);

			//---------------------------------------------------------------
			// source index를 load한다.
			//---------------------------------------------------------------
			ipfstream addIndexFile(FilenameIndexAdd, std::ios_base::binary);

			CFileIndexTable	sourceIndexFile;
			sourceIndexFile.LoadFromFile( addIndexFile );

			//---------------------------------------------------------------
			// target index를 연다.
			//---------------------------------------------------------------
			opfstream targetIndexFile(FilenameOriginal, std::ios_base::binary);
			
			//---------------------------------------------------------------
			// OriginalFile의 크기에서부터 file pointer가 시작되므로
			//---------------------------------------------------------------
			targetCount += sourceIndexFile.GetSize();

			long targetEnd = originalPackFileSize - SIZE_PACKSIZE;

			//---------------------------------------------------------------
			// target index에 크기를 기록한다.
			//---------------------------------------------------------------
			targetIndexFile.write((const char*)&targetCount, SIZE_PACKSIZE);

			//---------------------------------------------------------------
			// original index를 target index에 복사한다.
			//---------------------------------------------------------------
			char buffer[SIZE_BUFFER];
			int n;
			
			while (1)
			{
				originalIndexFile.read(buffer, SIZE_BUFFER);
				
				n = originalIndexFile.gcount();
				
				if (n > 0)
				{		
					targetIndexFile.write(buffer, n);
				}
				else
				{
					break;
				}
			}

			//---------------------------------------------------------------
			// Source의 각 file position을 
			// targetEnd만큼 증가시켜서 originalIndexFile에 추가한다.
			//---------------------------------------------------------------
			long sourceIndex;
			for (int i=0; i<sourceIndexFile.GetSize(); i++)
			{
				sourceIndex = targetEnd + sourceIndexFile[i];
				targetIndexFile.write((const char*)&sourceIndex, 4);
			}								

			// 끝
			addIndexFile.close();
			originalIndexFile.close();
			targetIndexFile.close();

			//---------------------------------------------------------------
			// 정해진 Filesize랑 같은가?
			//---------------------------------------------------------------
			//if (!IsFileSizeAfterOK())
			//{
				// Update를 했는데 원하는 Filesize가 아니면.. 심각하다!!
				// 심각한 경우이다.
			//	return false;
			//}	
		}

		return true;
	}

	return false;
}


//----------------------------------------------------------------------
// Append Package Inner Info
//----------------------------------------------------------------------
// FilenameAdd		: 추가될 Information File
// FilenameOriginal : 원래의 Information File
//----------------------------------------------------------------------
// TargetFile의 끝에 SourceFile을 추가한다.
// 단, TargetFile의 전체 개수를 추가된 것 만큼 증가해야 한다.    
//----------------------------------------------------------------------
bool
PUUFAppendInfo(const char* FilenameAdd, const char* FilenameOriginal)
{
	//---------------------------------------------------------------
	// PPD 파일에 정보가 없다면 일반 파일이다.
	//---------------------------------------------------------------
	if(!PUUFIsPackageInnerFile(FilenameOriginal))
		return PUUFAppendInfo(FilenameAdd, FilenameOriginal);

	return PUUFAppendPackImpl<int>(FilenameAdd, FilenameOriginal);
}


//----------------------------------------------------------------------
// Get Package Inner File Size
//----------------------------------------------------------------------
// Filename : Package 내부 파일 이름
//----------------------------------------------------------------------
// CPackageCache에서 Package 내부 파일의 크기를 얻는다.
//----------------------------------------------------------------------
long
PUUFGetFileSize(const char* Filename)
{
	const PACKAGEINNERPATH_DATA* pPPD = CPackagePath::Instance()->GetPackageInnerPathData(Filename);
	if(!pPPD) return -1;

	LPCTSTR	packagePath	= CPackagePath::Instance()->GetPackagePath(pPPD->nPackageIdx);
	if(!packagePath) return -1;

	CPackageCacheData* pPackageData	= CPackageCache::Instance()->GetCacheData(packagePath);
	if(!pPackageData) return -1;

	zzip_dir_hdr* pHDR = pPackageData->GetFileInfo(pPPD->strInnerPath.c_str());
	if(!pHDR) return -1;
	
	return pHDR->d_usize;
}



//----------------------------------------------------------------------
// Is Package Inner File Exist
//----------------------------------------------------------------------
// Filename : Package 내부 파일 이름
//----------------------------------------------------------------------
// Package 내부에 파일이 있는지 검사한다.
//----------------------------------------------------------------------
bool
PUUFIsFileExist(const char* Filename)
{
	return PUUFGetFileSize(Filename) != -1;
}


//----------------------------------------------------------------------
// Is Package Inner File
//----------------------------------------------------------------------
// Filename : Package 내부 파일 이름
//----------------------------------------------------------------------
// Package 내부에 파일이 있는지 검사한다.
//----------------------------------------------------------------------
bool
PUUFIsPackageInnerFile(const char* Filename)
{
	return CPackagePath::Instance()->GetPackageInnerPathData(Filename) != NULL;
}
