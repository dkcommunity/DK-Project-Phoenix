#ifndef __INITINFO_H__
#define __INITINFO_H__

#include <fstream>
#include <climits>
#include "MString.h"

const char*	CSV_NULL_VALUE	= "(null)";
const char* CSV_TOKEN		= ",";

//--------------------------------------------------------------------------
// Save String To Text File
//--------------------------------------------------------------------------
void SaveStringToCSVFile(std::ofstream& file, const MString& value)
{
	int length = value.GetLength();
	if(length)
	{
		char* szCSVBuf = new char[length + 1];
		strcpy(szCSVBuf, value.GetString());

		for(int i = 0; i < length; ++i)
			if(szCSVBuf[i] == ',' || szCSVBuf[i] == '\n') szCSVBuf[i] = ' ';		// 콤마를 공백으로 변환

		file << szCSVBuf;	

		delete [] szCSVBuf;
	}
	else
	{
		file << CSV_NULL_VALUE;	
	}

	file << ',';
}


//--------------------------------------------------------------------------
// Save Number To Text File
//--------------------------------------------------------------------------

#undef max

template<typename TYPE>
void SaveNumberToCSVFile(std::ofstream& file, TYPE value,
						 TYPE null = std::numeric_limits<TYPE>::max())
{
	if(null == value)
		file << CSV_NULL_VALUE;
	else
		file << (int)value;

	file << ',';
}


//--------------------------------------------------------------------------
// Save Bool To Text File
//--------------------------------------------------------------------------
template<typename TYPE>
void SaveBoolToCSVFile(std::ofstream& file, TYPE value)
{
	if(value)	file << "true";
	else		file << "false";

	file << ',';
}

#if __CONTENTS(__ITEMINFO_TABLEATION_PROJECT)

//--------------------------------------------------------------------------
// Load Text File To String
//--------------------------------------------------------------------------
void LoadCSVFileToString(const std::ifstream& file, MString& value)
{
	int length = value.GetLength();
	if(length)
	{
		char* szCSVBuf = new char[length + 1];
		strcpy(szCSVBuf, value.GetString());

		for(int i = 0; i < length; ++i)
			if(szCSVBuf[i] == ',' || szCSVBuf[i] == '\n') szCSVBuf[i] = ' ';		// 콤마를 공백으로 변환

		szCSVBuf << file.getline()

		delete [] szCSVBuf;
	}
	else
	{
		file << CSV_NULL_VALUE;	
	}

	file << ',';
}


//--------------------------------------------------------------------------
// Load Text File To Number
//--------------------------------------------------------------------------

#undef max

template<typename TYPE>
void LoadCSVFileToNumber(std::ifstream& file, TYPE value,
						 TYPE null = std::numeric_limits<TYPE>::max())
{
	if(null == value)
		file << CSV_NULL_VALUE;
	else
		file << (int)value;

	file << ',';
}


//--------------------------------------------------------------------------
// Load Text File To Bool
//--------------------------------------------------------------------------
template<typename TYPE>
void LoadCSVFileToBool(std::ifstream& file, TYPE value)
{
	if(value)	file << "true";
	else		file << "false";

	file << ',';
}

#endif //__ITEMINFO_TABLEATION_PROJECT

#endif //__INITINFO_H__
