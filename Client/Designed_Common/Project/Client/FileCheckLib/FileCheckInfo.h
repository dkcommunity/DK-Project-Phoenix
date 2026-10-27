#pragma once

enum PACKING_TYPE {
	PT_NO_PACKING,
	PT_PACKING,
	PT_COMPRESS_PACKING,
	PT_MAX,
};

class FileCheckInfo
{
public:
	enum { CHECKSUM_SIZE = 32	};

	FileCheckInfo();
	FileCheckInfo(LPCTSTR fileName);
	FileCheckInfo(const FileCheckInfo& fileCheckInfo);
	~FileCheckInfo();

public:
	void		GenerateFileInfo(LPCTSTR fileName);
	bool		IsFileDataIdentical(const FileCheckInfo& fileCheckInfo) const;

	void		LoadFromFile(std::ifstream &file);
	void		SaveToFile(std::ofstream &file) const;

// Get / Set Function
public:
	DWORD		GetFileSize() const						{ return m_FileSize;							}
	void		SetFileSize(DWORD fileSize)				{ m_FileSize = fileSize;						}

	BYTE		GetPackingType() const					{ return m_PackingType;							}
	void		SetPackingType(BYTE packingType)		{ m_PackingType = packingType;					}

	const char*	GetMD5CheckSum() const					{ return m_MD5CheckSum;							}
	void		SetMD5CheckSum(char *md5)				{ memcpy(m_MD5CheckSum, md5, CHECKSUM_SIZE);	}

	const FILETIME& GetLastWriteTime() const			{ return m_LastWriteTime;						}
	void		SetLastWriteTime(const FILETIME& ft)	{ m_LastWriteTime = ft;							}

private:
	DWORD		m_FileSize;						///< 파일 크기

	FILETIME	m_LastWriteTime;				///< 마지막 업데이트 시간

	BYTE		m_PackingType;					///< Packing 종류 (0 = No Packing, 1 = Packing, 2 = Compress Packing)
	char		m_MD5CheckSum[CHECKSUM_SIZE];	///< MD5 Check Sum 알고리즘을 이용한 Hash 데이터
};
