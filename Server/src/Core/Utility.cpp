//////////////////////////////////////////////////////////////////////////////
// Filename    : Utility.cpp
// Written by  : �輺��
// Description : 
// ���� ������ ���Ǵ� ��ũ�� �� ������ �Լ��� ������ �����̴�.
//////////////////////////////////////////////////////////////////////////////

#include "Utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "Assert.h"
#include "VSDateTime.h"
#include <stdarg.h>	// va_start, va_list
#include <sys/stat.h>
#include <sys/types.h>

static bool isBareLogFilename(const char* szFilename)
{
	if (szFilename == NULL || szFilename[0] == '\0') return false;
	if (strchr(szFilename, '/') != NULL) return false;
	if (strchr(szFilename, '\\') != NULL) return false;
	if (strchr(szFilename, ':') != NULL) return false;

	const char* dot = strrchr(szFilename, '.');
	if (dot == NULL) return false;

	return strcasecmp(dot, ".log") == 0 || strcasecmp(dot, ".txt") == 0;
}


static bool copySafeLogFilename(const char* szFilename, char* outFilename, size_t outSize)
	throw()
{
	if (outFilename == NULL || outSize == 0) return false;
	outFilename[0] = '\0';

	if (szFilename == NULL) return false;

	size_t i = 0;
	for (; i + 1 < outSize; ++i)
	{
		unsigned char ch = (unsigned char)szFilename[i];
		if (ch == '\0') break;
		if (ch < 32) return false;

		outFilename[i] = (char)ch;
	}

	if (i == 0 || i + 1 >= outSize) return false;
	outFilename[i] = '\0';

	if (outFilename[0] == '/') return false;
	if (strchr(outFilename, ':') != NULL) return false;
	if (strstr(outFilename, "..") != NULL) return false;

	for (size_t j = 0; outFilename[j] != '\0'; ++j)
	{
		if (outFilename[j] == '\\') outFilename[j] = '/';
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////////
// �簢�� Ŭ����
//////////////////////////////////////////////////////////////////////////////
/*
VSRect::VSRect(int l, int t, int r, int b)
{
	Assert(l <= r && t <= b);

	left   = l;
	top    = t;
	right  = r;
	bottom = b;
}

bool VSRect::ptInRect(const POINT& pt) const
{
	if(left <= pt.x && pt.x <= right && top <= pt.y && pt.y <= bottom) return true;
	return false;
}

bool VSRect::ptInRect(const int x, const int y) const
{
	if(left <= x && x <= right && top <= y && y <= bottom) return true;
	return false;
}

void VSRect::set(int l, int t, int r, int b)
{
	Assert(l <= r && t <= b);

	left   = l;
	top    = t;
	right  = r;
	bottom = b;
}
*/

//////////////////////////////////////////////////////////////////////////////
// ���ڸ� ���ڿ��� �ٲٱ�
//////////////////////////////////////////////////////////////////////////////
string itos(int value)
{
	char buf[100] = {0, };
	sprintf(buf, "%d", value);
	return string(buf);
}

//////////////////////////////////////////////////////////////////////////////
// ���� ���ϱ�
// origin���� dest�� ������ Ÿ��ũ�⿡ �°� ���Ѵ�. client���� �����Դ�.
//////////////////////////////////////////////////////////////////////////////
#define BASIS_DIRECTION_HIGH    2.0f
#define BASIS_DIRECTION_LOW     0.5f
Dir_t computeDirection(int originX, int originY, int destX, int destY)
{
	int   stepX = destX - originX;
	int   stepY = destY - originY;
	float k     =(stepX == 0) ? 0 :(float)(stepY) / stepX;

	if(stepY == 0)
	{
		if(stepX == 0)     return DOWN;
		else if(stepX > 0) return RIGHT;
		else                return LEFT;
	}
	else if(stepY < 0)
	{
		if(stepX == 0)
		{
			return UP;
		}
		else if(stepX > 0)
		{
			if(k < -BASIS_DIRECTION_HIGH)     return UP;
			else if(k < -BASIS_DIRECTION_LOW) return RIGHTUP;
			else                               return RIGHT;
		}
		else
		{
			if(k > BASIS_DIRECTION_HIGH)     return UP;
			else if(k > BASIS_DIRECTION_LOW) return LEFTUP;
			else                              return LEFT;
		}
	}
	else
	{
		if(stepX == 0)
		{
			return DOWN;
		}
		else if(stepX > 0)
		{
			if(k > BASIS_DIRECTION_HIGH)     return DOWN;
			else if(k > BASIS_DIRECTION_LOW) return RIGHTDOWN;
			else                              return RIGHT;
		}
		else
		{
			if(k < -BASIS_DIRECTION_HIGH)     return DOWN;
			else if(k < -BASIS_DIRECTION_LOW) return LEFTDOWN;
			else                               return LEFT;
		}
	}

	return DIR_NONE;
}

/*
Dir_t calcDirection(int originX, int originY, int destX, int destY)
{
	return computeDirection(originX, originY, destX, destY);
}

Dir_t getDirection(int originX, int originY, int destX, int destY)
{
	return computeDirection(originX, originY, destX, destY);
}

//////////////////////////////////////////////////////////////////////////////
// ���� ���� �̾Ƴ���
//////////////////////////////////////////////////////////////////////////////
int Random(int Min, int Max)
{
	if(Max == 0 || Min > Max) return  0;
	return ((rand() %(int)((Max) -(Min) + 1)) +(Min));
}

//////////////////////////////////////////////////////////////////////////////
// �ݿø��ϱ�
//////////////////////////////////////////////////////////////////////////////
int Round(float f) { return f - 0.5 >(int)f ?(int)f + 1 :(int)f; }

//////////////////////////////////////////////////////////////////////////////
// ���̽� �Լ�
//////////////////////////////////////////////////////////////////////////////
uint Dice(uint num , uint dice)
{
	uint result = 0;
	for(uint i = 0 ; i < num ; i ++)
	result += rand() % dice + 1;
	return result;
}
*/

//////////////////////////////////////////////////////////////////////////////
// �� ���ڿ����� �� ���� ��������
// pos ���� \n �� �ε������� �ȴ�.
//////////////////////////////////////////////////////////////////////////////
string getline(const string & str , uint & pos)
	throw()
{
	if(pos >= str.size()) return "";

	// FAZ 4 - 64-bit migration fix: find_first_of npos (size_t) → uint truncate edilince
	// guard atlatiliyor, pos+1 overflow olunca caller'in m_Index 0'a duser → infinite loop.
	// size_t ile bul, npos check'i 64-bit'te yap, sonra uint'e güvenli atama.
	uint oldpos = pos;
	size_t found = str.find_first_of('\n', oldpos);

	uint newPos;
	if (found == string::npos) newPos = (uint)str.size() + 1;
	else                         newPos = (uint)found + 1;

	pos = newPos;
	return str.substr(oldpos, (newPos - 1) - oldpos);

	/*
	uint oldpos = pos;
	// 1 �� �����ָ�, 
	pos = str.find_first_of('\n',oldpos) + 1;
	return str.substr(oldpos ,(pos - 1) - oldpos);
	*/
}

//////////////////////////////////////////////////////////////////////////////
// ���ڿ� �� ���� ���� �����ϱ�
//////////////////////////////////////////////////////////////////////////////
string trim(const string & str)
	throw()
{
	if(str.size() == 0) return "";

	static const char * WhiteSpaces = " \t\n\r";
	// FAZ 4 - 64-bit migration fix: uint string::npos truncate ediyor (0xFFFFFFFF),
	// `begin == string::npos` her zaman false donduruyor -> garbage substr.
	size_t begin = str.find_first_not_of(WhiteSpaces);
	size_t end = str.find_last_not_of(WhiteSpaces);

	if(begin == string::npos) begin = 0;
	if(end   == string::npos) end   = str.size();

	return str.substr(begin , end - begin + 1);
}

string replace(const string& strSrc, const string& strFind, const string& strReplace)
{
	string strResult = strSrc;
	string::size_type offset = 0;

	while(true)
	{
		offset = strResult.find(strFind, offset);
		if ( string::npos == offset )
		{
			break;
		}

		strResult.replace(offset, strFind.length(), strReplace);

		offset += strReplace.length();
	}

	return strResult;
}

//////////////////////////////////////////////////////////////////////////////
// ���� �ð� ����
//////////////////////////////////////////////////////////////////////////////
void getCurrentTimeEx(int& year, int& month, int& day, int& hour, int& minute, int& sec) 
	throw()
{
	time_t cur_time = time(NULL);
	tm     cur_tm;
	localtime_r( &cur_time, &cur_tm );
	//tm*    cur_tm   = localtime(&cur_time);

	year   = cur_tm.tm_year + 1900;
	month  = cur_tm.tm_mon  + 1;
	day    = cur_tm.tm_mday;
	hour   = cur_tm.tm_hour;
	minute = cur_tm.tm_min;
	sec    = cur_tm.tm_sec;
}

string getCurrentTimeStringEx(void) 
	throw()
{
	int year, month, day, hour, minute, second;

	getCurrentTimeEx(year, month, day, hour, minute, second);

	string rValue;
	rValue += itos(year)   + ". ";
	rValue += itos(month)  + ". ";
	rValue += itos(day)    + ". ";
	rValue += itos(hour)   + ". ";
	rValue += itos(minute) + ". ";
	rValue += itos(second) + ". ";
	return rValue;
}

//////////////////////////////////////////////////////////////////////////////
// ���� ���� ����Ʈ�� ���� ����Ʈ�� �и���Ű��
//////////////////////////////////////////////////////////////////////////////
/*
void splitWord(WORD value, int& high, int& low) 
	throw()
{
	high = value >> 8;
	low  = value & 0xFF;
}

//////////////////////////////////////////////////////////////////////////////
// 0���� �ʱ�ȭ��Ű��
//////////////////////////////////////////////////////////////////////////////
void zerofill(void* pointer, size_t size)
	throw()
{
	memset(pointer, 0, size);
}

//////////////////////////////////////////////////////////////////////////////
// range ���� �ִ��� �˻��ϱ�
//////////////////////////////////////////////////////////////////////////////
bool isInRange(int value, int min, int max)
	throw()
{
	if(min <= value && value <= max) return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////////
// �־��� ������ %�� �˾Ƴ���
//////////////////////////////////////////////////////////////////////////////
int getPercentValue(int value, int percent)
{
	return(int)((float)value *(float)percent / 100.0);
}
*/

//////////////////////////////////////////////////////////////////////////////
// �־��� ������ %�� �˾Ƴ���  -  �Ҽ� ù���ڸ� ����
//////////////////////////////////////////////////////////////////////////////
int getPercentValueEx( int value, int percent )
{
	static int tick = 0;
	
	int thousand = getPercentValue( value, percent * 10 );
	int ret = ( thousand + tick ) / 10;

	tick++;
	if ( tick > 9 ) tick = 0;

	return ret;
}

//////////////////////////////////////////////////////////////////////////////
// ���Ͽ��� �α��ϱ�
//////////////////////////////////////////////////////////////////////////////
void filelog(const char* szFilename, const char* fmt, ...)
    throw()
{
    __BEGIN_TRY

	if (fmt == NULL) fmt = "(null log format)";

    va_list valist;

    va_start(valist, fmt);

    char buffer[30000];

    int nchars = vsnprintf(buffer, 30000, fmt, valist);
    va_end(valist);

    if(nchars < 0)
	{
		snprintf(buffer, sizeof(buffer), "[filelog] format failed");
	}
	else if((size_t)nchars >= sizeof(buffer))
	{
		buffer[sizeof(buffer) - 1] = '\0';
	}

	VSDateTime current = VSDateTime::currentDateTime();

	char safeFilename[256];
	const bool validFilename = copySafeLogFilename(szFilename, safeFilename, sizeof(safeFilename));
	const char* targetFilename = validFilename ? safeFilename : "log/filelog_invalid.log";
	char redirectedFilename[512];
	if (isBareLogFilename(targetFilename))
	{
		mkdir("log", 0755);
		snprintf(redirectedFilename, sizeof(redirectedFilename), "log/%s", targetFilename);
		redirectedFilename[sizeof(redirectedFilename) - 1] = '\0';
		targetFilename = redirectedFilename;
	}
	else if (strncmp(targetFilename, "log/", 4) == 0)
	{
		mkdir("log", 0755);
	}

    ofstream file(targetFilename, ios::out | ios::app);
	if (!file)
	{
		cerr << current.toString() << " : [filelog] cannot open log file: " << targetFilename << endl;
		return;
	}

	if (!validFilename)
	{
		file << current.toString() << " : [filelog] invalid log filename rejected" << endl;
	}

    file << current.toString() << " : " << buffer << endl;
    file.close();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 2���� WORD �� DWORD �� �ٲ��ش�. HIWORD, LOWORD
//////////////////////////////////////////////////////////////////////////////
DWORD makeDWORD( WORD hiWord, WORD loWord )
{
	DWORD dwResult = ( (DWORD)hiWord << 16 ) | loWord;

	return dwResult;
}

//////////////////////////////////////////////////////////////////////////////
// DWORD �� HIWORD, LOWORD ��������
//////////////////////////////////////////////////////////////////////////////
WORD getHIWORD( DWORD dwValue )
{
	WORD wResult = (WORD)( dwValue >> 16 );

	return wResult;
}

WORD getLOWORD( DWORD dwValue )
{
	DWORD dwMask = 0x00FF;

	WORD wResult = (WORD)( dwMask & dwValue );

	return wResult;
}

bool CheckSQLInjectionString(string& str)
{
	// SQL Injection ���
    unsigned char ch;
    int iNumOfInvalidChar = 0;
    
    for ( unsigned int i=0; i < str.size(); ++i)
    {
        ch = str[i];
        
        if ( ch == '\'' || ch == '\"' || ch == ';' )
        {
        	++iNumOfInvalidChar; 
        }
    }
    
    int iIndex = str.find("1=1");
    
    if ( str.find("1=1") != std::string::npos )
    {
    	++iNumOfInvalidChar;
    }
    
    if ( iNumOfInvalidChar > 0 )
    {
    	return true;
    }
    
    return false;
}

string ReplaceDBString(const string& s)
{
	static string strResult;
	
	strResult = replace(s, "\\", "\\\\");
	strResult = replace(strResult, "'", "\\'");
	strResult = replace(strResult, "\"", "\\\"");
	
	return strResult;
}
