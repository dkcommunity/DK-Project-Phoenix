#pragma once

#include <map>
#include <string>

#include "FileCheckInfo.h"

typedef std::map<std::string, FileCheckInfo> FILECHECKINFO_MAP;

class FileCheckInfoTable : public FILECHECKINFO_MAP
{
public:
	FileCheckInfoTable();
	~FileCheckInfoTable();

public:
	void LoadFromFile(std::ifstream &file);
	void SaveToFile(std::ofstream &file) const;
};

void LoadCompressFileCheckInfoTable(FileCheckInfoTable& FileCheckInfoTable, LPCTSTR fileName);
void SaveCompressFileCheckInfoTable(const FileCheckInfoTable& FileCheckInfoTable, LPCTSTR fileName);
