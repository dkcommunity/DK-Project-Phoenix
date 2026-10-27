//----------------------------------------------------------------------
// PackageUpdateUtility.h
//----------------------------------------------------------------------

#ifndef	__PACKAGEUPDATEUTILITY_H__
#define	__PACKAGEUPDATEUTILITY_H__

#include "UpdateUtility.h"

//----------------------------------------------------------------------
// Package Update Utility Functions
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// File		Copy / Move / Delete / Rename(Directory도 가능) / Append
//----------------------------------------------------------------------
bool		PUUFCopyFile(const char* FilenameSource, const char* FilenameTarget);
bool		PUUFMoveFile(const char* FilenameSource, const char* FilenameTarget);
bool		PUUFDeleteFile(const char* FilenameTarget);

//----------------------------------------------------------------------
// Append Pack (with Index)
//----------------------------------------------------------------------
// IndexFile들이 NULL이면 Index는 추가하지 않는다.
//----------------------------------------------------------------------
bool		PUUFAppendPack(const char* FilenameAdd, const char* FilenameOriginal,
						const char* FilenameIndexAdd=0, const char* FilenameIndexOriginal=0);

//----------------------------------------------------------------------
// Append Info
//----------------------------------------------------------------------
bool		PUUFAppendInfo(const char* FilenameAdd, const char* FilenameOriginal);

//----------------------------------------------------------------------
// etc.
//----------------------------------------------------------------------
long		PUUFGetFileSize(const char* Filename);
bool		PUUFIsFileExist(const char* Filename);
bool		PUUFIsPackageInnerFile(const char* Filename);


//----------------------------------------------------------------------
// Append Package Inner Pack Implement
//----------------------------------------------------------------------
// FilenameAdd		: 추가될 Information File
// FilenameOriginal : 원래의 Information File
//----------------------------------------------------------------------
// CDataTable::SaveToFile(), CTypePack::SaveToFile()로
// 만들어진 파일 두개를 합친다.
//----------------------------------------------------------------------
template<typename SIZE_TYPE>
bool
PUUFAppendPackImpl(const char* FilenameAdd, const char* FilenameOriginal)
{
	/*
	if(!PUUFIsFileExist(FilenameOriginal))
		return UUFAppendPackImpl<SIZE_TYPE>(FilenameAdd, FilenameOriginal);

	if(UUFHasPermission(FilenameOriginal) && UUFHasPermission(FilenameAdd))
	{
		//------------------------------------------
		// 정해진 Filesize랑 같은가?
		//------------------------------------------
		//if (!IsFileSizeBeforeOK())
		//{
			// Update하려고 하는데 원하는 Filesize가 아니면.. 심각하다!!						
		//	return false;
		//}

		SIZE_TYPE sourceCount, targetCount;

		//---------------------------------------------------------------
		// target의 개수를 읽는다.
		//---------------------------------------------------------------
		ivfstream originalFile(FilenameOriginal, std::ios_base::binary);
		
		originalFile.read((char*)&targetCount, sizeof(SIZE_TYPE));

		//---------------------------------------------------------------
		// source의 개수를 target의 개수와 더한다.
		//---------------------------------------------------------------
		std::ifstream addFile(FilenameAdd, std::ios_base::binary);
			
		addFile.read((char*)&sourceCount, sizeof(SIZE_TYPE));

		targetCount += sourceCount;


		//---------------------------------------------------------------
		// original과 source를 합쳐 target를 만든다.
		//---------------------------------------------------------------
		opfstream targetFile(FilenameOriginal, std::ios_base::binary);

		// 개수를 쓴다.
		targetFile.write((char *)&targetCount, sizeof(SIZE_TYPE));

		//---------------------------------------------------------------
		// originalFile를 targetFile에 복사한다.
		//---------------------------------------------------------------
		char buffer[SIZE_BUFFER];
		int n;

		while (1)
		{
			originalFile.read(buffer, SIZE_BUFFER);
			
			n = originalFile.gcount();

			if (n > 0)
			{		
				targetFile.write(buffer, n);
			}
			else
			{
				break;
			}
		}

		//---------------------------------------------------------------
		// addFile을 targetFile에 복사한다.
		//---------------------------------------------------------------
		while (1)
		{
			addFile.read(buffer, SIZE_BUFFER);
			
			n = addFile.gcount();

			if (n > 0)
			{		
				targetFile.write(buffer, n);
			}
			else
			{
				break;
			}
		}

		addFile.close();
		originalFile.close();
		targetFile.close();

		//------------------------------------------
		// 정해진 Filesize랑 같은가?
		//------------------------------------------
		//if (!IsFileSizeAfterOK())
		//{
			// Update를 했는데 원하는 Filesize가 아니면.. 심각하다!!
			// 심각한 경우이다.
		//	return false;
		//}
	}
	*/

	return true;
}

#endif
