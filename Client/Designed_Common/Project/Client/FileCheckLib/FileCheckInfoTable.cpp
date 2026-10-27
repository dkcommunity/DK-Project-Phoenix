#include "FileCheckLib_Precompile.h"
#include "FileCheckInfoTable.h"

#include "VFS_Macro.h"

FileCheckInfoTable::FileCheckInfoTable()
{

}

FileCheckInfoTable::~FileCheckInfoTable()
{


}

void FileCheckInfoTable::LoadFromFile(std::ifstream &file)
{
	int dataSize = 0;
	file.read((char *)&dataSize, sizeof(int));

	clear();

	char nameBuffer[MAX_PATH];

	for(int i = 0; i < dataSize; ++i)
	{
		int		nameLength = 0;
		file.read((char *)&nameLength, sizeof(int));
		file.read((char *)&nameBuffer, nameLength);
		nameBuffer[nameLength] = '\0';
		
		assert((int)strlen(nameBuffer) == nameLength);

		FileCheckInfo fileCheckInfo;
		fileCheckInfo.LoadFromFile(file);

		insert(FILECHECKINFO_MAP::value_type(
			nameBuffer, fileCheckInfo));
	}
}

void FileCheckInfoTable::SaveToFile(std::ofstream &file) const
{
	int dataSize = static_cast<int>(size());
	file.write((const char *)&dataSize, sizeof(int));

	for(const_iterator pos = begin(); pos != end(); ++pos)
	{
		const std::string	&fileName = pos->first;
		const FileCheckInfo	&fileCheckInfo = pos->second;

		int nameLength = static_cast<int>(fileName.size());
		file.write((const char *)&nameLength, sizeof(int));
		file.write((const char *)fileName.c_str(), nameLength);
		
		fileCheckInfo.SaveToFile(file);
	}
}


void LoadCompressFileCheckInfoTable(FileCheckInfoTable& FileCheckInfoTable, LPCTSTR fileName)
{
	std::ifstream compressFile(fileName, std::ios_base::binary);
//	assert(compressFile.is_open());

	DWORD	UncompressSize;
	DWORD	CompressSize;
	BYTE	Incompressible;
	
	std::streamsize rd;

	compressFile.read((char *)&UncompressSize, sizeof(DWORD));
	compressFile.read((char *)&CompressSize, sizeof(DWORD));
	compressFile.read((char *)&Incompressible, sizeof(BYTE));

	rd = compressFile.gcount();
//	assert(rd == PATCHDATAHEADER_SIZE);

	char *pBuffer		= new char[UncompressSize];
	char *pCompressBuf	= new char[CompressSize];

	compressFile.read(pCompressBuf, CompressSize);
	rd = compressFile.gcount();
//	assert(rd == CompressSize);

	compressFile.close();

	DWORD uncompressSize = UncompressSize;

	int result = uncompress(
		(Bytef*)pBuffer, &uncompressSize,
		(Bytef*)pCompressBuf, CompressSize);
//	assert(result == Z_OK);
//	assert(uncompressSize == UncompressSize);

	std::string strTempFileName = fileName;
	strTempFileName += ".tmp";

	std::ofstream fileOut(strTempFileName.c_str(), std::ios_base::binary);
//	assert(fileOut.is_open());

	fileOut.write(pBuffer, uncompressSize);
	fileOut.close();

	delete [] pCompressBuf;
	delete [] pBuffer;

	std::ifstream fileIn(strTempFileName.c_str(), std::ios_base::binary);
//	assert(fileIn.is_open());

	FileCheckInfoTable.LoadFromFile(fileIn);
	fileIn.close();

	DeleteFile(strTempFileName.c_str());
}


void SaveCompressFileCheckInfoTable(const FileCheckInfoTable& FileCheckInfoTable, LPCTSTR fileName)
{
	std::string strTempFileName = fileName;
	strTempFileName += ".tmp";

	std::ofstream uncompressfileOut(strTempFileName.c_str(), std::ios_base::binary);
//	assert(uncompressfileOut.is_open());

	FileCheckInfoTable.SaveToFile(uncompressfileOut);
	uncompressfileOut.close();

	DWORD	UncompressSize	= 0;
	DWORD	CompressSize	= 0;
	BYTE	Incompressible	= 0;

	std::ifstream uncompressfileIn(strTempFileName.c_str(), std::ios_base::binary);
//	assert(uncompressfileIn.is_open());

	uncompressfileIn.seekg(0, std::ios_base::end);
	UncompressSize = static_cast<DWORD>(uncompressfileIn.tellg());
	uncompressfileIn.seekg(0, std::ios_base::beg);

	CompressSize = calc_compress_buffer_size(UncompressSize);

	char *pBuffer		= new char[UncompressSize];
	char *pCompressBuf	= new char[CompressSize];

	uncompressfileIn.read(pBuffer, UncompressSize);

	std::streamsize rd = uncompressfileIn.gcount();
//	assert(rd == UncompressSize);

	uncompressfileIn.close();

	int result = compress2(
		(Bytef*)pCompressBuf, &CompressSize,
		(Bytef*)pBuffer, UncompressSize, Z_BEST_COMPRESSION);
//	assert(result == Z_OK);

	std::ofstream compressFile(fileName, std::ios_base::binary);
//	assert(compressFile.is_open());

	compressFile.write((const char *)&UncompressSize, sizeof(DWORD));
	compressFile.write((const char *)&CompressSize, sizeof(DWORD));
	compressFile.write((const char *)&Incompressible, sizeof(BYTE));

	compressFile.write((const char *)pCompressBuf, CompressSize);

	compressFile.close();

	DeleteFile(strTempFileName.c_str());
}
