//----------------------------------------------------------------------
// UpdateUtility.h
//----------------------------------------------------------------------

#ifndef	__UPDATEUTILITY_H__
#define	__UPDATEUTILITY_H__

// 한번에 카피하는 byte수
#define	SIZE_BUFFER			4096

//----------------------------------------------------------------------
// Update Utility Functions
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Permission 체크 - 다른 directory에는 접근을 못하도록
//----------------------------------------------------------------------
bool		UUFHasPermission(const char* filename);


//----------------------------------------------------------------------
// Directory 생성/삭제
//----------------------------------------------------------------------
bool		UUFCreateDirectory(const char* dirName);
bool		UUFCreateDirectoryRecursive(const char* dirName);
bool		UUFDeleteDirectory(const char* dirName);
void		UUFDeleteEmptyDirectoryRecursive(const char* dirName);


//----------------------------------------------------------------------
// File		Copy / Move / Delete / Rename(Directory도 가능)
//----------------------------------------------------------------------
bool		UUFCopyFile(const char* FilenameSource, const char* FilenameTarget);
bool		UUFMoveFile(const char* FilenameSource, const char* FilenameTarget);
bool		UUFDeleteFile(const char* FilenameTarget);
bool		UUFDeleteFiles(const char *path, const char *fileext);
bool		UUFRenameFile(const char* FilenameSource, const char* FilenameTarget);


//----------------------------------------------------------------------
// Append Pack (with Index)
//----------------------------------------------------------------------
// IndexFile들이 NULL이면 Index는 추가하지 않는다.
//----------------------------------------------------------------------
bool		UUFAppendPack(const char* FilenameAdd, const char* FilenameOriginal,
						const char* FilenameIndexAdd=0, const char* FilenameIndexOriginal=0);

//----------------------------------------------------------------------
// Delete SpritePack
//----------------------------------------------------------------------
// SpritePack에서 특정한 Sprite들을 삭제한다.
//----------------------------------------------------------------------
//bool		UUFDeleteSpritePack(const char* FilenameID, const char* FilenameOriginal);

//----------------------------------------------------------------------
// Update SpritePack
//----------------------------------------------------------------------
// SpritePack에서 특정한 sprite들만 교체한다.
//----------------------------------------------------------------------
//bool		UUFUpdateSpritePack(const char* FilenameNew, 
//								const char* FilenameID, 
//								const char* FilenameOriginal);

//----------------------------------------------------------------------
// Append Info
//----------------------------------------------------------------------
bool		UUFAppendInfo(const char* FilenameAdd, const char* FilenameOriginal);

//----------------------------------------------------------------------
// Make SpritePack Index
//----------------------------------------------------------------------
// SpritePack을 load해서 index를 생성한다.
//----------------------------------------------------------------------
//bool		UUFMakeSpritePackIndex(const char* Filename);


//----------------------------------------------------------------------
// etc.
//----------------------------------------------------------------------
void		UUFGetFileCount(const char* dirName, int* pFileCount);


//----------------------------------------------------------------------
// Append Pack Implement
//----------------------------------------------------------------------
// FilenameAdd		: 추가될 Information File
// FilenameOriginal : 원래의 Information File
//----------------------------------------------------------------------
// CDataTable::SaveToFile(), CTypePack::SaveToFile()로
// 만들어진 파일 두개를 합친다.
//----------------------------------------------------------------------
template<typename SIZE_TYPE>
bool
UUFAppendPackImpl(const char* FilenameAdd, const char* FilenameOriginal)
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
	//
	//					Pack을 추가한다.
	//
	//---------------------------------------------------------------
	// 추가할 수 있게 한다.
	std::ifstream addFile(FilenameAdd, std::ios::binary);
	std::fstream originalFile(FilenameOriginal, std::ios::in | std::ios::out | std::ios::binary);
	
	SIZE_TYPE	sourceCount, targetCount;
	
	//---------------------------------------------------------------
	// source의 개수를 저장해 둔다.
	//---------------------------------------------------------------
	addFile.read((char*)&sourceCount, sizeof(SIZE_TYPE));
	
	//---------------------------------------------------------------
	// target File Pointer를 끝으로..
	//---------------------------------------------------------------
	originalFile.seekp(0, std::ios::end);
	
	//---------------------------------------------------------------
	// Original File의 크기를 기억해둔다.
	//---------------------------------------------------------------
	long originalPackFileSize = originalFile.tellp();
	
	// 추가
	char buffer[SIZE_BUFFER];
	int n;
	//---------------------------------------------------------------
	// addFile을 읽어서 originalFile의 끝에 붙인다.
	//---------------------------------------------------------------
	while (1)
	{
		addFile.read(buffer, SIZE_BUFFER);
		
		n = addFile.gcount();
		
		if (n > 0)
		{		
			originalFile.write(buffer, n);
		}
		else
		{
			break;
		}
	}
	
	//---------------------------------------------------------------
	// 개수를 변경시켜준다. (originalFile + addFile)
	//---------------------------------------------------------------
	originalFile.seekg(0, std::ios::beg);
	originalFile.read((char*)&targetCount, sizeof(SIZE_TYPE));
	
	targetCount += sourceCount;
	
	originalFile.seekp(0, std::ios::beg);				
	originalFile.write((const char*)&targetCount, sizeof(SIZE_TYPE));
	
	// 끝
	addFile.close();
	originalFile.close();
	
	//---------------------------------------------------------------
	// 정해진 Filesize랑 같은가?
	//---------------------------------------------------------------
	//if (!IsFileSizeAfterOK())
	//{
	// Update를 했는데 원하는 Filesize가 아니면.. 심각하다!!
	// 심각한 경우이다.
	//	return false;
	//}

	return true;
}


#endif
