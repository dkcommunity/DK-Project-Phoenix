//----------------------------------------------------------------------
//
// Filename    : LCLoginOKHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "LCLoginOK.h"
#include "client_PCH.h"
#include "crash_trace_tool.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static void PhoenixLCLoginOKTrace(const char* fmt, ...)
{
#if !PHOENIX_CLIENT_LOGS_ENABLED
  (void)fmt;
  return;
#else
  char logPath[MAX_PATH] = {0};
  if (GetModuleFileNameA(NULL, logPath, MAX_PATH) > 0) {
    char* lastSlash = strrchr(logPath, '\\');
    if (lastSlash != NULL) {
      strcpy(lastSlash + 1, "login_handshake_trace.log");
    } else {
      strcpy(logPath, "login_handshake_trace.log");
    }
  } else {
    strcpy(logPath, "login_handshake_trace.log");
  }

  FILE* fp = fopen(logPath, "a");
  if (fp == NULL) return;

  time_t now = time(NULL);
  struct tm tmNow;
  localtime_s(&tmNow, &now);
  fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d ",
          tmNow.tm_year + 1900, tmNow.tm_mon + 1, tmNow.tm_mday,
          tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec);

  va_list args;
  va_start(args, fmt);
  vfprintf(fp, fmt, args);
  va_end(args);

  fputc('\n', fp);
  fclose(fp);
#endif
}


#ifdef __GAME_CLIENT__

#include "ClientPlayer.h"
// #include "Cpackets/CLGetPCList.h"
#include "Cpackets/CLGetWorldList.h"
#include "UserInformation.h"

#include "MGameStringTable.h"
#include "UIDialog.h"
#endif

#include "ClientDef.h"
#include "UIFunction.h"
// #include "DEServerInformation.h"

extern BOOL g_bNeedUpdate;
extern int g_LeftPremiumDays;
extern bool g_bFamily;
extern BYTE g_PayType;
// 20071214
#include "VS_UI.h"

#if __CONTENTS(__XTRAP)
#include "XTrap4Client.h"
#endif //__XTRAP

#if __CONTENTS(__TITLE_UI_RENWEAL)
#include "VS_UI_NewTitle.h"
#else //__TITLE_UI_RENWEAL
#include "VS_UI_Title.h"
#endif //__TITLE_UI_RENWEAL

bool ChecknProtectCRC(HINSTANCE dll, char *FilePath);

//----------------------------------------------------------------------
//
// �α��� �����κ��� ���̵�� �н������� ������ �޾Ҵٴ�
// α κ ̵ н  ޾Ҵٴ
// Ҹ.
//
//  α  PC  Ʈ ûϴ Ŷ ϸ ȴ.
//
//----------------------------------------------------------------------
void LCLoginOKHandler::execute(LCLoginOK *pPacket, Player *pPlayer)
{
  PhoenixLCLoginOKTrace("LCLoginOK entry packet=%p player=%p needUpdate=%d",
                        (void*)pPacket, (void*)pPlayer, (int)g_bNeedUpdate);
  CT_Done("DIAG: LCLoginOKHandler::execute() hit");
  __BEGIN_TRY

#ifdef __GAME_CLIENT__

#if __CONTENTS(__XTRAP)
//	XTrap_C_SetUserInfoEx("darkeden","run","test1","vampire",100);
#endif //__XTRAP
  if (g_pUserInformation->UserID.GetLength() >= 12) {
    UI_BackupLoginID("DarkEden");
  } else
    UI_BackupLoginID(g_pUserInformation->UserID);

  if (!g_bNeedUpdate) {
    // 2004, 07, 20 sobeit add start
    // ���� ó�� �Ȱ� ���� ��
    //			ClientPlayer * pClientPlayer =
    //dynamic_cast<ClientPlayer*>(pPlayer);
    //
    //			CLGetWorldList clGetWorldList;
    //
    //			pClientPlayer->sendPacket( &clGetWorldList );
    //
    //			SetMode( MODE_WAIT_WORLD_LIST );

    g_pUserInformation->IsUnderFifthteen =
        pPacket->isUnderFifthteen();              // 15�� �̸��ΰ�?
    g_pUserInformation->IsUnderFifthteen = false; // by svi, �����û�ѡ���Ƿ���Ѫ

    //		if(g_pUserInformation->IsUnderFifthteen)		// 15��
    //�̸��̸� �ǰ� �ȶ������� 			g_pUserOption->BloodDrop = false ;

    g_pUserInformation->pLogInClientPlayer =
        dynamic_cast<ClientPlayer *>(pPlayer);
#if __CONTENTS(!__IMI_INTERFACE)
    if (false == g_pUserInformation->IsAutoLogIn)
#endif //__IMI_INTERFACE
    {
      CLGetWorldList clGetWorldList;

      ClientPlayer *pClientPlayer =
          (ClientPlayer *)(g_pUserInformation->pLogInClientPlayer);
      PhoenixLCLoginOKTrace("LCLoginOK send worldlist begin clientPlayer=%p autoLogin=%d",
                            (void*)pClientPlayer,
                            (int)g_pUserInformation->IsAutoLogIn);
      CT_Done("DIAG: LCLoginOK - Sending CLGetWorldList packet");
      pClientPlayer->sendPacket(&clGetWorldList);
      PhoenixLCLoginOKTrace("LCLoginOK sendPacket CLGetWorldList done");
      // Login handshake paketi ayni handler icinde cikmali; mode loop flush'una kalmasin.
      pClientPlayer->processOutput();
      PhoenixLCLoginOKTrace("LCLoginOK processOutput done");

      SetMode(MODE_WAIT_WORLD_LIST);
      PhoenixLCLoginOKTrace("LCLoginOK SetMode MODE_WAIT_WORLD_LIST done");
    }
    // else
    //	SetMode( MODE_MAINMENU );
    //  2004, 07, 20 sobeit add end

    /*
    // ������
    ClientPlayer * pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);

    CLGetPCList clGetPCList;

    pClientPlayer->sendPacket( &clGetPCList );

    // �÷��̾��� ���¸� �ٲ۴�.
    pClientPlayer->setPlayerStatus( CPS_AFTER_SENDING_CL_GET_PC_LIST );

    //------------------------------------------------------------
    // ���� Server ������ �����صд�.
    //------------------------------------------------------------
    SetServerGroupName( pPacket->getGroupName().c_str() );
    SetServerGroupStatus( (int)pPacket->getStat() );
    //SetServerName( pPacket->getServerName().c_str() );

    //------------------------------------------------------------
    // Client�� PC List�� ��ٷ��� �Ѵ�.
    //------------------------------------------------------------
    SetMode( MODE_WAIT_PCLIST );
    */

    //------------------------------------------------------------
    // Gore Level�� �ٲ۴�.
    //		-
    //------------------------------------------------------------
    bool bGoreLevel = pPacket->isAdult();
    bGoreLevel = true; // by viva
    g_pUserInformation->bNetmarbleGoreLevel =
        bGoreLevel; // bNetmarbelGoreLevel���� ���� ���Ǵ� ���� ���� (20071218)

    if (!bGoreLevel) // Login�� ������ û�ҳ��̴�.
      g_pUserOption->UseTeenVersion =
          TRUE; // ������, UseTeenVersion���� �����Ѵ�.
    else if (g_pUserOption
                 ->UseTeenVersion) // Login�� ������ û�ҳ��� �ƴ�����, �α���â����
                                   // UseTeenVersion�� �����ߴٸ�!!
      bGoreLevel = FALSE; // bGoreLevel�� FALSE�� �����, �����ʱ�ȭ�� �ݿ��Ѵ�.
                          // (SetGoreLevel)

    SetGoreLevel(bGoreLevel);
  }

#endif

  // #if !defined(OUTPUT_DEBUG) && !defined(_DEBUG) &&
  // !defined(__DEBUG_OUTPUT__) 	if( g_nProtectInstance == NULL )
  //	{
  //		SendMessage(g_hWnd, WM_NPROTECT_FORCEEXIT, 0, 0 );
  //		return;
  //	}
  //
  //	char szBuffer[256] = {0,};
  //	if( GetSystemDirectory( szBuffer, 256) == 0)
  //	{
  //		SendMessage( g_hWnd, WM_NPROTECT_FORCEEXIT, 0, 0 );
  //		return;
  //
  //	}
  //
  //	strcat( szBuffer, "\\NPMON.EXE");
  //
  //	bool	Result;
  //	Result  = ChecknProtectCRC(g_nProtectInstance, "FindHack.exe");
  //	Result |= ChecknProtectCRC(g_nProtectInstance, szBuffer );
  //
  //	if(!Result)
  //		SendMessage(g_hWnd, WM_NPROTECT_FORCEEXIT, 0, 0 );
  // #endif

  g_bFamily = pPacket->isFamily();
#if __CONTENTS(__PAYZONE_PASS_TICKET)
  g_PayType = pPacket->getPayType();
#endif //__PAYZONE_PASS_TICKET
  if (g_pUserInformation != NULL && !g_pUserInformation->IsNetmarble) {
    // 2005, 1, 24, sobeit add start
    if (0xFFFD == pPacket->getLastDays()) {
      g_LeftPremiumDays = 15; // �޸� ���� �̺�Ʈ�� 15�Ϸ� �ٲ����.
                              // (2008.04.29)
      //			g_LeftPremiumDays = 7; // �޸� ���� ������ �ִ�
      //�̺�Ʈ �� ù �����϶� fffd�� ����´�. -> ���� �����̾� 7�� ����
      g_pUIDialog->PopupFreeMessageDlg(
          (*g_pGameStringTable)[UI_STRING_MESSAGE_GET_EVENT_ITEM_NOTICE]
              .GetString());
    } else
      // 2005, 1, 24, sobeit add end
      g_LeftPremiumDays = pPacket->getLastDays();
  } else
    g_LeftPremiumDays = 0xFFFF;

  DEBUG_ADD_FORMAT("[LCLoginOK] g_LeftPremiumDays : %x pPacket->getLastDays() "
                   ": %x g_bFamily : %s",
                   g_LeftPremiumDays, pPacket->getLastDays(),
                   g_bFamily ? "true" : "false");

// ##??#if !defined(__DEBUG_OUTPUT__) && !defined(OUTPUT_DEBUG)
#if 0
	if( g_pUserInformation!=NULL && !g_pUserInformation->bChinese && !g_pUserInformation->IsNetmarble)
		SendUserIDToGameMon(g_pUserInformation->UserID.GetString());
#endif
  __END_CATCH
}
