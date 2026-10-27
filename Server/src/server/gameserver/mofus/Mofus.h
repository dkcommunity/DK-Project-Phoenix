/////////////////////////////////////////////////////////////////////////////
// Filename : Mofus.h
// Desc		: 모퍼스 헤더
/////////////////////////////////////////////////////////////////////////////

#include "Types.h"

// 모퍼스 정의. 켜져있으면 모퍼스 모듈이 실행된다.
#if defined(__METRO_SERVER__)
// 2008.01.08 rappi76 파워짱 서비스 제거
//	#define __MOFUS__
#endif

// 로그용 파일
#define MOFUS_ERROR_FILE	"mofus_error.txt"
#define MOFUS_LOG_FILE		"mofus_log.txt"
#define MOFUS_PACKET_FILE	"mofus_packet.txt"

int loadPowerPoint( const string& name );
int savePowerPoint( const string& name, int amount );
void logPowerPoint( const string& name, int recvPoint, int savePoint );
