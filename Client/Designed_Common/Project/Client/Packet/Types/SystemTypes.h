//////////////////////////////////////////////////////////////////////////////
// Filename    : SystemTypes.h
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __SYSTEM_TYPES_H__
#define __SYSTEM_TYPES_H__

#if defined(__WINDOWS__) || defined(WIN32) || defined(_WIN32)
// 4786 - STL의 심볼명의 길이가 255를 넘어갈 때
#pragma warning(disable : 4786)

#include <Windows.h>
#elif defined(__LINUX__)
#include <sys/types.h>
#endif

#include <fstream>
#include <iostream>
#include <string>

// using namespace std;
using std::string;

//////////////////////////////////////////////////////////////////////////////
// built-in type redefinition
//////////////////////////////////////////////////////////////////////////////
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

#if defined(__LINUX__) || defined(__WIN_CONSOLE__) || defined(WIN_CONSOLE)
#if !defined(__WINDOWS__) && !defined(WIN32) && !defined(_WIN32)
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
#endif
typedef unsigned long long ulonglong;
#else
typedef unsigned __int64 ulonglong;
#endif

// 2026-05-19 ODR VIOLATION FIX — `const std::string separator` namespace-scope
// const is INTERNAL LINKAGE by default. Bu header'i include eden HER .cpp TU
// kendi bagimsiz `separator` instance'ini + atexit destructor registration'unu
// uretiyordu (100+ TU -> 100+ atexit thunk -> ODR-like state corruption +
// `_Deallocate(NULL, 512)` AV crash atexit chain'inde). Plan agent 2026-05-19
// audit. C++17 `inline const` -> external linkage, tek instance, tek atexit.
// Phoenix CMake cxx_std_17 zaten kullanir.
#if defined(__LINUX__)
inline const char separatorChar = '/';
inline const std::string separator = "/";
#elif defined(__WINDOWS__) || defined(WIN32) || defined(_WIN32)
inline const char separatorChar = '\\';
inline const std::string separator = "\\";
#endif

//////////////////////////////////////////////////////////////////////////////
// built-in type size
//////////////////////////////////////////////////////////////////////////////
const unsigned int szbool = sizeof(bool);
const unsigned int szchar = sizeof(char);
const unsigned int szshort = sizeof(short);
const unsigned int szint = sizeof(int);
const unsigned int szlong = sizeof(long);
const unsigned int szuchar = sizeof(unsigned char);
const unsigned int szushort = sizeof(unsigned short);
const unsigned int szuint = sizeof(unsigned int);
const unsigned int szulong = sizeof(unsigned long);
const unsigned int szBYTE = sizeof(BYTE);
const unsigned int szWORD = sizeof(WORD);
const unsigned int szDWORD = sizeof(DWORD);

//////////////////////////////////////////////////////////////////////////////
// ServerGroupInfo
//////////////////////////////////////////////////////////////////////////////
typedef BYTE ServerGroupID_t;
const uint szServerGroupID = sizeof(ServerGroupID_t);

//////////////////////////////////////////////////////////////////////////////
// SubServerInfo
//////////////////////////////////////////////////////////////////////////////
typedef WORD ServerID_t;
const uint szServerID = sizeof(ServerID_t);

typedef WORD UserNum_t;
const uint szUserNum = sizeof(UserNum_t);

//////////////////////////////////////////////////////////////////////////////
// SubServerInfo
//////////////////////////////////////////////////////////////////////////////
enum ServerStatus {
  SERVER_FREE,
  SERVER_NORMAL,
  SERVER_BUSY,
  SERVER_VERY_BUSY,
  SERVER_FULL,
  SERVER_DOWN
};

enum WorldStatus { WORLD_OPEN, WORLD_CLOSE };

typedef unsigned long IP_t;
const uint szIP = sizeof(IP_t);

typedef BYTE WorldID_t;
const uint szWorldID = sizeof(WorldID_t);

#endif
