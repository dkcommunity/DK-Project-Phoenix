#include "Client_PCH.h"
#include "crash_trace_tool.h"
#ifndef OUTPUT_DEBUG
// #define __OLD_VERSION__
// #define __RUN_GAMEHACK__

// #define __TEST_SERVER__
// #define __REAL_SERVER__

#pragma warning(disable : 4786)

#include "DebugInfo.h"
#include "nProtect.h"
#include "npfgmsdk.h"
#include <process.h>
#include <windows.h>

bool g_bIsNT = false, g_bIsStartGameHack = false;
HINSTANCE g_nProtectInstance = NULL;
HINSTANCE g_NPFGMInstance = NULL;
HINSTANCE g_nPSICEInstance = NULL;
DWORD g_OsVersion[2];
MYCHECKNPMON *pCheckNPMON = NULL;

// #ifdef __REAL_SERVER__
// 20081223 각 국가 버젼 별로 .ini파일 이름을 다르게 설정할경우 아래 인자값을
// 변경 하여 이용한다.
#if __CONTENTS(__NPROTECT)
#ifdef __DESIGNED_KOREA
CNPGameLib npgl("DarkEden");
#elif __DESIGNED_ITEMMANIA
CNPGameLib npgl("DarkEden");
#elif __DESIGNED_JAPAN
CNPGameLib npgl("DarkEdenJP");
#else
CNPGameLib npgl("DarkEden"); // Default fallback
#endif
#else
// No npgl definition needed if __NPROTECT is off
#endif //__NPROTECT

DWORD g_delayTime_npmon = 60000; // 20초 후부터 체크 시작

extern HWND g_hWnd;
extern void Send_nProtect_Auth(DWORD dwVal);

BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg);

bool CheckWindowVersion() {
  OSVERSIONINFO VersionInfo;

  VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

  if (GetVersionEx(&VersionInfo) == FALSE) {
    CT_Done("지원되지 않는 윈도우 버전입니다.");
    return false;
  }

  if (VersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
    g_bIsNT = true;
  else
    g_bIsNT = false;

  g_OsVersion[0] = VersionInfo.dwPlatformId;
  g_OsVersion[1] = VersionInfo.dwMajorVersion;

  return true;
}
//
// bool	ChecknProtectCRC(HINSTANCE dll, char* FilePath)
//{
//	CheckCRC		MyCheckCRC;
//
//	DEBUG_ADD_FORMAT("[nProtect] CheckCRC %s",FilePath);
//	MyCheckCRC = (CheckCRC)GetProcAddress( dll, "CheckCRC" );
//	return MyCheckCRC( (LPTSTR)(LPCTSTR) FilePath ) != FALSE;
//}

int CheckNPROTECT(int CurTime) {
#ifdef __OLD_VERSION__
  static DWORD time = 0;
  HWND hWnd;

  time++;
  if (time >= 512)
    time = 0;

  if (time == 4) {
    hWnd = FindWindow("TfrmNPMON", "WWW.NPROTECT.COM");

    if (hWnd != 0)
      return NPROTECT_OK;

    DEBUG_ADD("[nProtect] Cannot Find nProtect");
    return NPROTECT_ERROR;
  } // else
#endif

//	if(time == 15)
//	if( TimerCheckNPMON() )
//	{
//		BOOL Result = pCheckNPMON();
//
//		if(!Result)
//			return NPROTECT_ERROR;
//
//		return NPROTECT_OK;
//
// #ifdef __RUN_GAMEHACK__
//		if ( !pnPFindSICE())
//			return NPROTECT_OK;
//
//		DEBUG_ADD("[nProtect] Find SoftICE");
//		return NPROTECT_ERROR;
// #endif
//	}
#if __CONTENTS(__NPROTECT)
  if (npgl.Check() != NPGAMEMON_SUCCESS) {
    CT_Done("게임가드가 실행 중 이지 않습니다. 비정상적으로 종료 됩니다.");
    return NPROTECT_ERROR;
  }
#endif

  return NPROTECT_OK;
}

int RunNPROTECT() {
  // zzi - 클로벌 서비스에는 nProtect를 안 쓴다고 한다. 무조건.. OK....
#ifdef __DESIGNED_INTERNATION
  return NPROTECT_OK;
#endif
  //-
#ifdef __OLD_VERSION__
  int ExecuteResult;

  g_nProtectMessage[NPROTECT_NORMAL] =
      "알 수 없는 오류 입니다. 다크에덴을 종료합니다.";
  g_nProtectMessage[NPROTECT_EXIT_TWO] =
      "nProtect 에 의해 다크에덴이 종료됩니다.(1)";
  g_nProtectMessage[NPROTECT_FORCE_EXIT] =
      "nProtect 에 의해 다크에덴이 종료됩니다.(2)";
  g_nProtectMessage[NPROTECT_SPEEDHACK] =
      "스피드핵이 발견되어 다크에덴이 종료됩니다.";
  g_nProtectMessage[NPROTECT_SOFTICE] =
      "게임핵이 발견되어 다크에덴이 종료됩니다.";
  g_nProtectMessage[NPROTECT_SH_ERROR] =
      "nProtect 에 의해 다크에덴이 종료됩니다(SHERROR)";
  g_nProtectMessage[NPROTECT_CHECK_ERROR] =
      "nProtect Check 에러에 의해 다크에덴이 종료됩니다.";

  DEBUG_ADD("[nProtect] Execute nProtect");
  ExecuteResult = spawnl(P_WAIT, "findhack.exe", "findhack.exe", NULL);

  switch (ExecuteResult) {
  case 1024: // 메모리상에 해킹툴이 존재하지 않을 경우
    DEBUG_ADD("[nProtect] Cannot Find Hacking program");
    break;
  case 1025: // 메모리상에 해킹툴이 존재하나 정상적으로 치료를 했을 경우
    DEBUG_ADD("[nProtect] Find Hacking program. - fixed it.");
    break;
  case 1026: // 메모리상의 해킹툴을 감지했으나 사용자가 치료를 선택하지 않거나
             // 프로그램에서 정상적으로 치료를 하지 못했을 경우
    DEBUG_ADD("[nProtect] did not Fix Hacking program.");
    return NPROTECT_EXECUTE_ERROR_DID_NOT_FIX;
  case 1027: // 해킹툴 진단 프로그램이 정상적으로 다운로드 되지 않았을
             // 경우(URL이 잘못되었거나 서버가 정상적으로 동작하지 않을 경우)
    DEBUG_ADD("[nProtect] Can not download check program");
    return NPROTECT_EXECUTE_ERROR_CANNOT_DOWNLOAD;
  case 1028: // NPX.DLL 등록 에러 및 nProtect 구동에 필요한 파일이 없을 경우
    DEBUG_ADD("[nProtect] Cannot Find DLL File");
    return NPROTECT_EXECUTE_ERROR_CANNOT_FIND_DLL;
  case 1029: // 프로그램내에서 예외사항이 발생했을 경우
    DEBUG_ADD("[nProtect] Occured Exception in nProtect");
    return NPROTECT_EXECUTE_ERROR_EXCEPTION;
  case 1030: // 사용자가 종료 버튼을 클랙했을 경우의 처리값
    DEBUG_ADD("[nProtect] Quit nProtect by user");
    return NPROTECT_EXECUTE_ERROR;
  case 1031: // 업데이트 서버 접속을 실패한 경우
    DEBUG_ADD("[nProtect] Cannot connect updateserver");
    return NPROTECT_CANNOT_UPDATE;
  }

  DEBUG_ADD("[nProtect] Ok");

  if ((g_nProtectInstance = LoadLibrary(FILE_NPROTECT_DLL)) == NULL)
    return NPROTECT_CANNOT_FIND_DLL;
  /*
          char szBuffer[256] = {0,};
          if( GetSystemDirectory( szBuffer, 256) == 0)
                  return NPROTECT_ERROR_GET_WINDOWS_DIRECTORY;
          strcat( szBuffer, "\\NPMON.EXE");
          bool	Result;
          Result = ChecknProtectCRC(g_nProtectInstance, "FindHack.exe");
          Result |=ChecknProtectCRC(g_nProtectInstance, szBuffer );

          if(!Result)
          {
                  FreeLibrary( g_nProtectInstance );
                  g_nProtectInstance = NULL;
                  return NPROTECT_ERROR_CRC_CHECK;
          }
  */
  if ((pCheckNPMON = (MYCHECKNPMON *)GetProcAddress(g_nProtectInstance,
                                                    "CheckNPMON")) == NULL)
    return NPROTECT_ERROR_NOT_EXIST_CHECK_NPMON;

#ifdef __RUN_GAMEHACK__
  if (LoadLibraryNPFGM() == false)
    return NPROTECT_ERROR_LOAD_DLL_GAMEHACK;

  EnterGameHack();
#endif

#endif // __OLD_VERSION__

#ifndef __OLD_VERSION__
  //	g_nProtectMessage[NPGAMEMON_COMM_ERROR] = "[NPGAMEMON_COMM_ERROR]
  // nProtect와의 통신에 에러가 발생하여 다크에덴을 종료합니다.";
  //	g_nProtectMessage[NPGAMEMON_COMM_CLOSE] = "[NPGAMEMON_COMM_CLOSE]
  // nProtect와의 통신이 닫혀 다크에덴을 종료합니다.";
  //	g_nProtectMessage[NPGAMEMON_INIT_ERROR] = "[NPGAMEMON_INIT_ERROR] 초기화
  // 에러입니다. 다크에덴을 종료합니다.";
  // g_nProtectMessage[NPGAMEMON_SPEEDHACK] = "[NPGAMEMON_SPEEDHACK] 스피드핵이
  //발견되어 다크에덴을 종료합니다.";
  //	g_nProtectMessage[NPGAMEMON_GAMEHACK_KILLED] =
  //"[NPGAMEMON_GAMEHACK_KILLED] 게임핵이 발견되었습니다. 다크에덴을
  // 종료합니다."; 	g_nProtectMessage[NPGAMEMON_GAMEHACK_DETECT] =
  //"[NPGAMEMON_GAMEHACK_DETECT] 게임핵이 감지되어 다크에덴을 종료합니다.";
  //	g_nProtectMessage[NPGAMEMON_UNDEFINED] = "[NPGAMEMON_UNDEFINED] nProtect
  // 관련 문제가 발생하여 다크에덴이 종료됩니다.";

  g_nProtectMessage[NPGAMEMON_COMM_ERROR] = "[NPGAMEMON_COMM_ERROR] ";
  g_nProtectMessage[NPGAMEMON_COMM_CLOSE] = "[NPGAMEMON_COMM_CLOSE] ";
  g_nProtectMessage[NPGAMEMON_INIT_ERROR] = "[NPGAMEMON_INIT_ERROR] ";
  g_nProtectMessage[NPGAMEMON_SPEEDHACK] = "[NPGAMEMON_SPEEDHACK] ";
  g_nProtectMessage[NPGAMEMON_GAMEHACK_KILLED] = "[NPGAMEMON_GAMEHACK_KILLED] ";
  g_nProtectMessage[NPGAMEMON_GAMEHACK_DETECT] = "[NPGAMEMON_GAMEHACK_DETECT] ";
  g_nProtectMessage[NPGAMEMON_UNDEFINED] = "[NPGAMEMON_UNDEFINED] nProtect ";

  // DWORD dwResult = npgl.Init(g_hWnd);
#if __CONTENTS(__NPROTECT)
  DWORD dwResult = npgl.Init();
  if (dwResult != NPGAMEMON_SUCCESS) {
    // 적절한 종료 메시지 출력
    TCHAR msg[512];

    wsprintf(msg, "An error has occurred whilst GameGuard is running. Inside the game folder, "
                  "The *.erl files in the GameGuard folder Game1@inca.co.kr "
                  "Please send it as an attachment.");

    MessageBox(g_hWnd, msg, "Error", MB_OK | MB_ICONERROR);
    return NPROTECT_ERROR;
  } else {
    npgl.SetHwnd(g_hWnd);
    SetCallbackToGameMon(NPGameMonCallback);
  }
#else
  DWORD dwResult = NPGAMEMON_SUCCESS;
#endif

#endif

  return NPROTECT_OK;
}

void CloseNPROTECT() {
#ifdef __OLD_VERSION__
  HWND nProtecthWnd;

  nProtecthWnd = FindWindow(NULL, "WWW.NPROTECT.COM");

#ifdef __RUN_GAMEHACK__
  ReleaseGameHack();
#endif

  if (nProtecthWnd != 0) {
    DEBUG_ADD("[nProtect] Close nProtect");
    SendMessage(nProtecthWnd, WM_NPROTECT_FORCEEXIT, NULL, NULL);
  }
  if (g_nProtectInstance != NULL) {
    FreeLibrary(g_nProtectInstance);
    g_nProtectInstance = NULL;
  }
  if (g_NPFGMInstance != NULL) {
    FreeLibrary(g_NPFGMInstance);
    g_NPFGMInstance = NULL;
  }
#ifdef __RUN_GAMEHACK__
  if (g_nPSICEInstance != NULL) {
    FreeLibrary(g_nPSICEInstance);
    g_nPSICEInstance = NULL;
  }
#endif

#endif // __OLD_VERSION__
  //	npgl.Close();
}

// bool	LoadLibraryNPFGM()
//{
//	if( CheckWindowVersion() == false)
//		return false;
//
// #ifdef __RUN_GAMEHACK__
//
//	if(g_bIsNT)
//		g_NPFGMInstance = LoadLibrary( NPFGM_NT );
//	else
//		g_NPFGMInstance = LoadLibrary( NPFGM_9X );
//
//	g_nPSICEInstance = LoadLibrary( NPFGM_NT );
// #endif
//
//	if(g_NPFGMInstance == NULL
// #ifdef __RUN_GAMEHACK__
//		|| g_nPSICEInstance == NULL
// #endif
//		)
//		return false;
//
//	if( ((pEnterProtect = (EnterProtect) GetProcAddress(g_NPFGMInstance,
//"EnterProtect")) == NULL) ||
//		((pReleaseProtect = (ReleaseProtect) GetProcAddress(
// g_NPFGMInstance, "ReleaseProtect")) == NULL) #ifdef __RUN_GAMEHACK__
//		||((pnPFindSICE = (nPFindSICE) GetProcAddress( g_nPSICEInstance,
//"nPFindSICE")) == NULL) #endif
//		)
//		return false;
//	else
//		return true;
// }
//
// void	EnterGameHack()
//{
// #ifdef __RUN_GAMEHACK__
//	if(g_bIsStartGameHack == true)
//		return;
//
//	pEnterProtect();
//	g_bIsStartGameHack = true;
// #endif
//
// }
//
// void	ReleaseGameHack()
//{
// #ifdef __RUN_GAMEHACK__
//	if(g_bIsStartGameHack == false)
//		return;
//
//	pReleaseProtect();
//	g_bIsStartGameHack = false;
// #endif
// }

bool TimerCheckNPMON(int CurTime) {
  DWORD nCurrentTime;
  if (CurTime)
    nCurrentTime = CurTime;
  else
    nCurrentTime = timeGetTime();

  if ((nCurrentTime - g_TimerNPMON) > g_delayTime_npmon) {
    g_TimerNPMON = nCurrentTime;
    g_delayTime_npmon = 30000;
    return true;
  }
  return false;
}

BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg) {
  //	TCHAR msg[128];

  g_nProtectErrorMessage = dwMsg;
  g_nProtectErrorMessage2 = dwArg;
  switch (dwMsg) {
  case NPGAMEMON_COMM_ERROR:
  case NPGAMEMON_COMM_CLOSE:
    g_bForceExitBynProtect = true;
    return false; // 반드시 종료

  case NPGAMEMON_INIT_ERROR:
    //		wsprintf(msg, "nProtect GameGuard 초기화 에러 : %lu", dwArg);
    //		MessageBox(hWnd, msg, "Error", MB_OK);
    g_bForceExitBynProtect = true;
    return false; // 반드시 종료

  case NPGAMEMON_SPEEDHACK:
    //		CT_Done("스피드핵이 감지되었습니다.");
    g_bForceExitBynProtect = true;
    return false; // 종료

  case NPGAMEMON_GAMEHACK_KILLED:
    //		CT_Done("게임핵이 발견되었습니다.");
    return true; // 계속 진행

  case NPGAMEMON_GAMEHACK_DETECT:
    //		CT_Done("게임핵이 감지되었습니다.");
    g_bForceExitBynProtect = true;
    return false; // 종료
  case NPGAMEMON_CHECK_CSAUTH:
    // CT_Done("NPGAMEMON_CHECK_CSAUTH받음");
    Send_nProtect_Auth(dwArg);
    return true;
  }
  return true;
}
#if __CONTENTS(__NPROTECT)
void Set_Auth(DWORD dwVal) { npgl.Auth(dwVal); }
#endif
#endif
