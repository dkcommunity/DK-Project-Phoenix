//////////////////////////////////////////////////////////////////////////////
// Filename    : SystemTypes.h
// Written By  : Reiot
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SYSTEM_TYPES_H__
#define __SYSTEM_TYPES_H__

/* �׼�/���� ��ġ�� ���� ���� ������ �Ʒ� BUILD_NUMBER �� BUILD_INFO�� ä���ִ´�.
 * BUILD_NUMBER �����
 * BUILD_INFO  +Add (�߰��� ��� ���� ���)
 *             -Delete (������ ��� ���� ���)
 *             *Fix (������ġ/���� ���� ���)
 * BUILD_INFO �� �����ϰ� �������� ���
 */
#define BUILD_NUMBER 40518
#define BUILD_INFO   "<Version Information>\
+Add ----- \
-Delete -----\
Fix -----\
"


#if defined(__WINDOWS__)
	// 4786 - STL�� �ɺ����� ���̰� 255�� �Ѿ ��
	#pragma warning ( disable : 4786 )
	#include <Windows.h>
#elif defined(__LINUX__)
	#include <sys/types.h>
#endif
 
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

//////////////////////////////////////////////////////////////////////////////
// built-in type redefinition
//////////////////////////////////////////////////////////////////////////////
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

#if defined(__LINUX__) || defined(__WIN_CONSOLE__)
	typedef unsigned char  BYTE;
	typedef unsigned short WORD;
	// 2026-05-10 KRITIK FIX: Linux LP64 ABI uyumsuzluk
	// Eski: `typedef unsigned long DWORD`
	//   - Linux x86_64: unsigned long = 8 byte
	//   - Windows x86 (Client.exe): unsigned long = 4 byte
	//   - Wire format'a yazilan DWORD/ObjectID_t/HP_t/Fame_t/RankExp_t/SkillExp_t
	//     vb. degerlerin server tarafinda 8 byte olmasi → client 4 byte okuyunca
	//     packet boyutu kaymasi → InsufficientDataException → crash.
	// Cozum: DWORD'u explicit uint32_t (her zaman 4 byte) yap. Phoenix Win
	// client'i ile wire-compatible olur.
	// Memory: project_dword_32bit_decision.md (Direct typedef fix kararı). Bu
	// fix Apr ortasinda alinmis ama bu .h dosyasina uygulanmamis (ya da
	// reverted edilmis). Bugun ModifyInfo packet 60 (GCAttackMeleeOK1) crash'i
	// ile yeniden yuzeye cikti.
	typedef unsigned int   DWORD;       // 32-bit on both Linux and Windows
	typedef unsigned long long ulonglong;
#else
	typedef unsigned __int64 ulonglong;
#endif

#if defined(__LINUX__)
	const char separatorChar = '/';
	const string separator = "/";
#elif defined(__WINDOWS__)
	const char separatorChar = '\\';
	const string separator = "\\";
#endif


//////////////////////////////////////////////////////////////////////////////
// built-in type size
//////////////////////////////////////////////////////////////////////////////
const unsigned int szbool   = sizeof(bool);
const unsigned int szchar   = sizeof(char);
const unsigned int szshort  = sizeof(short);
const unsigned int szint    = sizeof(int);
const unsigned int szlong   = sizeof(long);
const unsigned int szuchar  = sizeof(unsigned char);
const unsigned int szushort = sizeof(unsigned short);
const unsigned int szuint   = sizeof(unsigned int);
const unsigned int szulong  = sizeof(unsigned long);
const unsigned int szBYTE   = sizeof(BYTE);
const unsigned int szWORD   = sizeof(WORD);
const unsigned int szDWORD  = sizeof(DWORD);


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
enum ServerStatus 
{
	SERVER_FREE = 0,
	SERVER_NORMAL,
	SERVER_BUSY,
	SERVER_VERY_BUSY,
	SERVER_FULL,
	SERVER_DOWN
};

enum WorldStatus
{
	WORLD_OPEN,
	WORLD_CLOSE	
};

typedef unsigned long IP_t;
const uint szIP = sizeof(IP_t);

typedef BYTE WorldID_t;
const uint szWorldID = sizeof(WorldID_t);

#endif
