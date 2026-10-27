//-----------------------------------------------------------------------------
// GameMain.cpp
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "Client.h"
#include "crash_trace_tool.h"
#include "frame_perf_probe.h"
#include "login_socket_trace.h"
#include "UIFunction.h"
#include "client_PCH.h"
#include <stdarg.h>
#include <MMSystem.h>
#include <ctype.h>
#include <direct.h>
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
#include <new>
#include <process.h>
#include <psapi.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
// #include "MFileDef.h"
#include "AddonDef.h"
#include "MCreatureTable.h"
#include "DEServerInformation.h"
#include "DebugInfo.h"
#include "GameObject.h"
#include "MPortal.h"
#include "MusicDef.h"
#include "PacketDef.h"
#include "Properties.h"
#include "ServerInfo.h"
#include "SoundNode.h"
#include "TempInformation.h"
#include "UIDialog.h"
#include "UserInformation.h"

#include "BGMManager.h"
#include "CDirectSound.h"
#include "CDirectSoundStream.h"
#include "CGameUpdate.h"
#include "CMP3.h"
#include "COpeningUpdate.h"
#include "COrderedList.h"
#include "CSoundPartManager.h"
#include "CWaitPacketUpdate.h"
#include "CWaitUIUpdate.h"
#include "ClientCommunicationManager.h"
#include "MChatManager.h"
#include "MEventManager.h"
#include "MGameStringTable.h"
#include "MHelpManager.h"
#include "MJusticeAttackManager.h"
#include "MMusic.h"
#include "MNpc.h"
#include "MParty.h"
#include "MZoneSound.h"
#include "MZoneSoundManager.h"
#include "PCConfigTable.h"
#include "Profiler.h"
#include "RequestClientPlayerManager.h"
#include "RequestFileManager.h"
#include "RequestServerPlayerManager.h"
#include "RequestUserManager.h"
#include "SoundDef.h"
#include "TitleMusic.h"  // Title music random pool (2026-05-14)
#include "CSprite565.h"
#include "SpriteIDDef.h"
#include "UtilityFunction.h"
#include "WavePackFileManager.h"
#include "WhisperManager.h"
#include "ZoneFileHeader.h"
#include "packet\Cpackets\CGVerifyTime.h"
#include "WebView2Host.h"

extern "C" int Phoenix_CreatureDrawAudit_ShouldLog(int creatureType);
extern "C" void Phoenix_CreatureDrawAudit_Log(const char* format, ...);
extern "C" void Phoenix_ClientLifecycleLogf(const char* format, ...);
extern "C" void Phoenix_FatalDiagLogf(const char* format, ...);
extern "C" void Phoenix_ClientLifecycleLogStack(const char* label);
extern char g_CWD[_MAX_PATH];

static const char* Phoenix_SafeMString(const MString& value) {
  const char* text = value.GetString();
  return text != NULL ? text : "";
}

static bool Phoenix_HasText(const MString& value) {
  const char* text = value.GetString();
  return text != NULL && text[0] != '\0';
}

static int Phoenix_CountUsableMapShapes(const ZONETABLE_INFO* pZoneInfo) {
  if (pZoneInfo == NULL) {
    return 0;
  }

  int count = 0;
  ZONETABLE_INFO::MAPSHAPE_VEC::const_iterator pos =
      pZoneInfo->MapShapeList.begin();
  for (; pos != pZoneInfo->MapShapeList.end(); ++pos) {
    if (Phoenix_HasText(*pos)) {
      ++count;
    }
  }

  return count;
}

static const MString* Phoenix_GetUsableMapShape(const ZONETABLE_INFO* pZoneInfo,
                                                int shapeIndex) {
  if (pZoneInfo == NULL || shapeIndex < 0) {
    return NULL;
  }

  int usableIndex = 0;
  ZONETABLE_INFO::MAPSHAPE_VEC::const_iterator pos =
      pZoneInfo->MapShapeList.begin();
  for (; pos != pZoneInfo->MapShapeList.end(); ++pos) {
    if (!Phoenix_HasText(*pos)) {
      continue;
    }

    if (usableIndex == shapeIndex) {
      return &(*pos);
    }

    ++usableIndex;
  }

  return NULL;
}

// 2002.6.28 [UDP수정]
#include "packet\Cpackets\CGPortCheck.h"

// #include "MZoneInfo.h"

#include "MTimeItemManager.h"
#include "MWarManager.h"

#include "VS_UI.h"

#include "MFakeCreature.h"
#include "MGuildType.h"

#include "MTestDef.h"
#include "SoundSetting.h"

#include "SXml.h"

#include "VS_UI_WorldMap.h"

#include "MWorkThreadGlobal.h"

#if defined(OUTPUT_DEBUG) && defined(__METROTECH_TEST__)
//	#define OUTPUT_DEBUG_UPDATE_LOOP
#endif

extern BOOL g_bSetHotKey;
extern CSoundPartManager *g_pSoundManager;

extern bool g_bGoodFPS;

// 헬기장 프로펠러 소리.. - -;
extern BOOL g_bPlayPropeller;

extern bool g_bSaveSlideScreenShot;
extern RECT g_rectScreenShot;

// #define	new			DEBUG_NEW
// #define	delete		DEBUG_DELETE

//-----------------------------------------------------------------------------
// Global
//-----------------------------------------------------------------------------
// mp3
// CMySound*			g_pMP3 = NULL;

// Avi
CAVI *g_pAvi = NULL;

// Volume 조절
IVolume *g_pMasterVolume = NULL;
IVolume *g_pWaveVolume = NULL;
DWORD g_MasterVolumeOriginal = 0;
DWORD g_WaveVolumeOriginal = 0;

int g_previousSoundID = SOUNDID_NULL;

// Sound PartManager
CSoundPartManager *g_pSoundManager = NULL;

// Cursor Surface
CStorageSurface *g_pCursorSurface = NULL;

// mouse cursor
POINT g_SelectSector = {SECTORPOSITION_NULL, SECTORPOSITION_NULL};
extern BOOL g_bLButtonDown;
BOOL g_bRButtonDown = FALSE;
BOOL g_bCButtonDown = FALSE;
BOOL g_bUIInput = FALSE;

int g_UpdateDelay = DELAY_UPDATE_GAME;

// Zone
WORD g_ZoneCreatureColorSet = 0xFFFF;
bool g_bZoneSafe = false;
bool g_bHolyLand = false;
int g_nZoneLarge = ZONEID_NULL;
int g_nZoneSmall = ZONEID_NULL;
bool g_bZoneLargeLoadImage = false;
bool g_bZoneSmallLoadImage = false;
MZone *g_pZoneLarge = NULL;
MZone *g_pZoneSmall = NULL;
bool g_bZonePlayerInLarge = true;
bool g_bLastLoadZoneWasOOM = false;
DWORD g_ZoneRandomSoundTime = 0;
bool g_bWatchMode = false;
int g_nZoneShape = 0;

// effectManager
MScreenEffectManager *g_pInventoryEffectManager = NULL;

// DirectDraw
CSpriteSurface *g_pBack = NULL;
CSpriteSurface *g_pLast = NULL;
int g_ScreenShotNumber = 0;

CDirectSoundStream *g_pDXSoundStream = NULL;
CMP3 *g_pMP3 = NULL;
int g_SoundPerSecond = 0;

// Chat string
#ifdef OUTPUT_DEBUG
CMessageArray *g_pDebugMessage = NULL;
#endif

bool g_bPutMessage = false; // 화면에 debug메세지를 출력할까?

bool g_bNewDraw = false;      // 화면을 다시 그렸는가? (커서 제외)
bool g_bSmoothCursor = false; // 부드러운(?) cursor를 출력할 수 있나?
bool g_bNetStatusGood = true; // 네트웍 상태가 좋은가?

CMessageArray *g_pSystemMessage = NULL;
CMessageArray *g_pNoticeMessage = NULL;
CMessageArray *g_pGameMessage = NULL;
CMessageArray *g_pHelpMessage = NULL;
CMessageArray *g_pGMMessage = NULL; // 진정한 운영자 말씀
CMessageArray *g_pEventMessage =
    NULL; // 특수문자열 .. 화면 중상단에 출력된다. 오직 하나의 문자열만
          // 출력한다. 로그아웃을 하거나 빈 문자열이 들어오기 전까지 유지된다.

CInfomationBord *g_pInfomationBord = NULL;
#if __CONTENTS(__WORLDCHATING)
CMessageArray *g_pWorldChatingMessage = NULL;
#endif //__WORLDCHATING

// socket
ClientPlayer *g_pSocket = NULL;

// Thread
// MWorkThread*		g_pLoadingThread = NULL;
// HANDLE				g_hFileThread = 0;
// HANDLE				g_hFileEvent;
// THREADJOB			g_ThreadJob;

// Server IP
// char				g_ServerIP[80];
bool g_bFullScreen = false;
int g_MaxNPC = MAX_NPC;
bool g_bHAL = true;
bool g_bMusicSW = true;
extern int g_MorphCreatureType;
extern LONG g_lGameRunBreakTime; // 게임 실행이 중지 될 때(스크린 샷 또는 Pause
                                 // Break) 2009.01.05 shootkj

//--------------------------------------------
// 현재의 Client Mode에 대한 설정..
//--------------------------------------------
enum CLIENT_MODE g_Mode = MODE_NULL;
enum CLIENT_MODE g_ModeNext = MODE_NULL;
extern bool g_PhoenixRegistrationResultActive;

CWinUpdate *g_pUpdate = NULL;

extern MCreature *AddClientCreature();
extern void Add_GDR_Effect(int nEffect, bool bAppearBossMonster);
extern void Add_GDR_Potal_Effect(int nMapID);

static const char* PhoenixClientModeName(enum CLIENT_MODE mode)
{
  switch (mode)
  {
  case MODE_NULL: return "MODE_NULL";
  case MODE_OPENING: return "MODE_OPENING";
  case MODE_MAINMENU: return "MODE_MAINMENU";
  case MODE_CHANGE_OPTION: return "MODE_CHANGE_OPTION";
  case MODE_NEWUSER: return "MODE_NEWUSER";
  case MODE_CREATECHARACTER: return "MODE_CREATECHARACTER";
  case MODE_WAIT_CREATEPCOK: return "MODE_WAIT_CREATEPCOK";
  case MODE_WAIT_DELETEPCOK: return "MODE_WAIT_DELETEPCOK";
  case MODE_SELECTCHARACTER: return "MODE_SELECTCHARACTER";
  case MODE_WAIT_LOGINOK: return "MODE_WAIT_LOGINOK";
  case MODE_WAIT_WORLD_LIST: return "MODE_WAIT_WORLD_LIST";
  case MODE_WAIT_SELECT_WORLD: return "MODE_WAIT_SELECT_WORLD";
  case MODE_WAIT_SERVER_LIST: return "MODE_WAIT_SERVER_LIST";
  case MODE_WAIT_SELECT_SERVER: return "MODE_WAIT_SELECT_SERVER";
  case MODE_LOGIN_WRONG: return "MODE_LOGIN_WRONG";
  case MODE_WAIT_PCLIST: return "MODE_WAIT_PCLIST";
  case MODE_WAIT_SELECTPC: return "MODE_WAIT_SELECTPC";
  case MODE_WAIT_RECONNECT: return "MODE_WAIT_RECONNECT";
  case MODE_WAIT_UPDATEINFO: return "MODE_WAIT_UPDATEINFO";
  case MODE_WAIT_SETPOSITION: return "MODE_WAIT_SETPOSITION";
  case MODE_WAIT_REGISTERPLAYEROK: return "MODE_WAIT_REGISTERPLAYEROK";
  case MODE_WAIT_RECONNECT_LOGIN: return "MODE_WAIT_RECONNECT_LOGIN";
  case MODE_GAME: return "MODE_GAME";
  case MODE_QUIT: return "MODE_QUIT";
  default: return "MODE_UNKNOWN";
  }
}

static void PhoenixSocketUpdateTrace(const char* fmt, ...)
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

static bool Phoenix_IsLoginServerSocket(const ClientPlayer* pPlayer)
{
  if (pPlayer == NULL) return false;

  const PlayerStatus status = pPlayer->getPlayerStatus();
  return status == CPS_NONE ||
         status == CPS_BEGIN_SESSION ||
         status == CPS_AFTER_SENDING_CL_LOGIN ||
         status == CPS_AFTER_SENDING_CL_QUERY_PLAYER_ID ||
         status == CPS_AFTER_SENDING_CL_REGISTER_PLAYER ||
         status == CPS_AFTER_SENDING_CL_GET_PC_LIST ||
         status == CPS_AFTER_SENDING_CL_CREATE_PC ||
         status == CPS_AFTER_SENDING_CL_DELETE_PC ||
         status == CPS_AFTER_SENDING_CL_SELECT_PC;
}

static bool Phoenix_TryDrainRegistrationResultAfterInputException()
{
  if (g_Mode != MODE_WAIT_REGISTERPLAYEROK || g_pSocket == NULL)
    return false;

  PhoenixSocketUpdateTrace("UpdateSocketInput registration drain begin socket=%p",
                           (void*)g_pSocket);

  try {
    g_pSocket->processCommand();
  } catch (Throwable &queuedError) {
    PhoenixSocketUpdateTrace("UpdateSocketInput registration drain failed error=%s",
                             queuedError.toString().c_str());
    PhoenixLoginSocketTrace("UpdateSocketInput: registration drain failed error=%s",
                            queuedError.toString().c_str());
    if (!g_PhoenixRegistrationResultActive)
      return false;
  }

  if (!g_PhoenixRegistrationResultActive) {
    PhoenixSocketUpdateTrace("UpdateSocketInput registration drain found no result");
    return false;
  }

  if (g_ModeNext == MODE_NULL)
    g_ModeNext = MODE_NEWUSER;

  PhoenixSocketUpdateTrace("UpdateSocketInput registration drain result active modeNext=%d",
                           (int)g_ModeNext);
  return true;
}

static bool Phoenix_TryDrainReconnectLoginAfterInputException()
{
  if (g_Mode != MODE_WAIT_RECONNECT_LOGIN || g_pSocket == NULL)
    return false;

  if (g_pSocket->getPlayerStatus() != CPS_WAITING_FOR_GC_RECONNECT_LOGIN)
    return false;

  PhoenixSocketUpdateTrace("UpdateSocketInput reconnect drain begin socket=%p",
                           (void*)g_pSocket);
  PhoenixLoginSocketTrace("UpdateSocketInput: reconnect drain begin socket=%p",
                          (void*)g_pSocket);

  try {
    g_pSocket->processCommand();
  } catch (Throwable &queuedError) {
    PhoenixSocketUpdateTrace("UpdateSocketInput reconnect drain failed error=%s",
                             queuedError.toString().c_str());
    PhoenixLoginSocketTrace("UpdateSocketInput: reconnect drain failed error=%s",
                            queuedError.toString().c_str());
    return false;
  }

  if (g_Mode != MODE_WAIT_RECONNECT_LOGIN ||
      g_pSocket->getPlayerStatus() != CPS_WAITING_FOR_GC_RECONNECT_LOGIN ||
      g_ModeNext != MODE_NULL) {
    PhoenixSocketUpdateTrace(
        "UpdateSocketInput reconnect drain consumed packet mode=%d status=%d modeNext=%d",
        (int)g_Mode, (int)g_pSocket->getPlayerStatus(), (int)g_ModeNext);
    PhoenixLoginSocketTrace(
        "UpdateSocketInput: reconnect drain consumed packet mode=%d status=%d modeNext=%d",
        (int)g_Mode, (int)g_pSocket->getPlayerStatus(), (int)g_ModeNext);
    return true;
  }

  PhoenixSocketUpdateTrace("UpdateSocketInput reconnect drain found no packet");
  PhoenixLoginSocketTrace("UpdateSocketInput: reconnect drain found no packet");
  return false;
}
//---------------------------------------------------------------------------
// Update Socket Input
//---------------------------------------------------------------------------
bool UpdateSocketInput() {

  // DEBUG_ADD("-PS-");

  if (g_Mode == MODE_NEWUSER && g_PhoenixRegistrationResultActive) {
    PhoenixSocketUpdateTrace("UpdateSocketInput skipped for active NewIDCreate result socket=%p",
                             (void*)g_pSocket);
    return true;
  }

  try {

    if (g_pSocket != NULL) {
      PhoenixSocketUpdateTrace("UpdateSocketInput begin mode=%d socket=%p",
                               (int)g_Mode, (void*)g_pSocket);
      g_pSocket->processInput();
      PhoenixSocketUpdateTrace("UpdateSocketInput after processInput mode=%d socket=%p",
                               (int)g_Mode, (void*)g_pSocket);
      g_pSocket->processCommand();
      PhoenixSocketUpdateTrace("UpdateSocketInput after processCommand mode=%d socket=%p",
                               (int)g_Mode, (void*)g_pSocket);
      // g_pSocket->processOutput();
    } else if (g_Mode != MODE_MAINMENU) {
      PhoenixSocketUpdateTrace("UpdateSocketInput socket null mode=%d", (int)g_Mode);
    }

  } catch (Throwable &t) {

    CT_BD("[SOCK] UpdateSocketInput exception mode=%d socket=%p error=%s",
          (int)g_Mode, (void*)g_pSocket, t.toString().c_str());
    PhoenixSocketUpdateTrace("UpdateSocketInput exception mode=%d socket=%p error=%s",
                             (int)g_Mode, (void*)g_pSocket, t.toString().c_str());
    CT_Done("DIAG: UpdateSocketInput CAUGHT exception: %s", t.toString().c_str());
    PhoenixLoginSocketTrace("UpdateSocketInput: exception mode=%d socket=%p error=%s",
                            (int)g_Mode, (void*)g_pSocket, t.toString().c_str());

    if (strstr(t.toString().c_str(), "InvalidProtocolException") != NULL) {
      if (!strstr(t.toString().c_str(), "(datagram)") == NULL)
        SendBugReport(t.toString().c_str());
    }

    DEBUG_ADD(t.toString().c_str());
    DEBUG_ADD_ERR("[Error] UpdateSocketInput");
    DEBUG_ADD(t.toString().c_str());

    if (Phoenix_TryDrainRegistrationResultAfterInputException()) {
      PhoenixLoginSocketTrace("UpdateSocketInput: preserved NewIDCreate result after input exception");
      return true;
    }

    if (Phoenix_TryDrainReconnectLoginAfterInputException()) {
      PhoenixLoginSocketTrace("UpdateSocketInput: preserved logout reconnect after input exception");
      return true;
    }

    // InitFail("Server와의 접속이 끊어졐습니다.");
    CT_BD("[SOCK] UpdateSocketInput -> UpdateDisconnected mode=%d error=%s",
          (int)g_Mode, t.toString().c_str());
    CT_Done("DIAG: UpdateSocketInput pre SetMode(MAINMENU) + UpdateDisconnected");
    PhoenixLoginSocketTrace("UpdateSocketInput: before SetMode MAINMENU + UpdateDisconnected");
    SetMode(MODE_MAINMENU);
    UpdateDisconnected();

    return false;
  }

  //----------------------------------------------------------------
  // RequestServerPlayerManager도 처리한다.
  //----------------------------------------------------------------
  static DWORD nextTime = g_CurrentTime;

  if (nextTime <= g_CurrentTime) {
#ifdef OUTPUT_DEBUG_UPDATE_LOOP
    DEBUG_ADD("RequestUpdate");
#endif

    if (g_pRequestUserManager != NULL) {
#ifdef OUTPUT_DEBUG_UPDATE_LOOP
      DEBUG_ADD("RUM");
#endif

      g_pRequestUserManager->Update();

#ifdef OUTPUT_DEBUG_UPDATE_LOOP
      DEBUG_ADD("RUM ok");
#endif
    }

    if (g_pRequestServerPlayerManager != NULL) {
// int numRequest = g_pRequestServerPlayerManager->GetSize();
#ifdef OUTPUT_DEBUG_UPDATE_LOOP
      DEBUG_ADD("RSPM");
#endif

      g_pRequestServerPlayerManager->Update();

#ifdef OUTPUT_DEBUG_UPDATE_LOOP
      DEBUG_ADD("RSPM ok");
#endif
    }

    if (g_pRequestClientPlayerManager != NULL) {
#ifdef OUTPUT_DEBUG_UPDATE_LOOP
      DEBUG_ADD("RCPM");
#endif

      g_pRequestClientPlayerManager->Update();

#ifdef OUTPUT_DEBUG_UPDATE_LOOP
      DEBUG_ADD("RCPM ok");
#endif
    }

    if (g_pClientCommunicationManager != NULL) {
#ifdef OUTPUT_DEBUG_UPDATE_LOOP
      DEBUG_ADD("CCMu");
#endif

      g_pClientCommunicationManager->Update();

#ifdef OUTPUT_DEBUG_UPDATE_LOOP
      DEBUG_ADD("CCMu ok");
#endif
    }

    if (g_pWhisperManager != NULL) {
#ifdef OUTPUT_DEBUG_UPDATE_LOOP
      DEBUG_ADD("WMu");
#endif

      g_pWhisperManager->Update();

#ifdef OUTPUT_DEBUG_UPDATE_LOOP
      DEBUG_ADD("WMu ok");
#endif
    }

    if (g_pRequestFileManager != NULL) {
#ifdef OUTPUT_DEBUG_UPDATE_LOOP
      DEBUG_ADD("RFM");
#endif

      g_pRequestFileManager->Update();

#ifdef OUTPUT_DEBUG_UPDATE_LOOP
      DEBUG_ADD("RFM ok");
#endif
    }

    if (g_pProfileManager != NULL) {
#ifdef OUTPUT_DEBUG_UPDATE_LOOP
      DEBUG_ADD("PfM");
#endif

      g_pProfileManager->Update();

#ifdef OUTPUT_DEBUG_UPDATE_LOOP
      DEBUG_ADD("PfM ok");
#endif
    }

    // 초당 3번 update한다.
    nextTime = g_CurrentTime + 330;
  }

  // DEBUG_ADD("-OK-");

  return true;
}

//---------------------------------------------------------------------------
// Update Socket Output
//---------------------------------------------------------------------------
bool UpdateSocketOutput() {

  // DEBUG_ADD("-PS-");

  if (g_Mode == MODE_NEWUSER && g_PhoenixRegistrationResultActive) {
    PhoenixSocketUpdateTrace("UpdateSocketOutput skipped for active NewIDCreate result socket=%p",
                             (void*)g_pSocket);
    return true;
  }

  try {

    if (g_pSocket != NULL) {
      PhoenixSocketUpdateTrace("UpdateSocketOutput begin mode=%d socket=%p",
                               (int)g_Mode, (void*)g_pSocket);
      // g_pSocket->processInput();
      // g_pSocket->processCommand();
      g_pSocket->processOutput();
      PhoenixSocketUpdateTrace("UpdateSocketOutput done mode=%d socket=%p",
                               (int)g_Mode, (void*)g_pSocket);
    } else if (g_Mode != MODE_MAINMENU) {
      PhoenixSocketUpdateTrace("UpdateSocketOutput socket null mode=%d", (int)g_Mode);
    }

  } catch (Throwable &t) {
    CT_BD("[SOCK] UpdateSocketOutput exception mode=%d socket=%p error=%s",
          (int)g_Mode, (void*)g_pSocket, t.toString().c_str());
    PhoenixSocketUpdateTrace("UpdateSocketOutput exception mode=%d socket=%p error=%s",
                             (int)g_Mode, (void*)g_pSocket, t.toString().c_str());
    PhoenixLoginSocketTrace("UpdateSocketOutput: exception mode=%d socket=%p error=%s",
                            (int)g_Mode, (void*)g_pSocket, t.toString().c_str());

    if (strstr(t.toString().c_str(), "InvalidProtocolException") != NULL)
      if (!strstr(t.toString().c_str(), "(datagram)") == NULL)
        SendBugReport(t.toString().c_str());

    DEBUG_ADD_ERR("[Error] UpdateSocketInput");
    DEBUG_ADD(t.toString().c_str());

    // InitFail("Server와의 접속이 끊어졌습니다.");
    SetMode(MODE_MAINMENU);
    UpdateDisconnected();

    return false;
  }

  // DEBUG_ADD("-OK-");

  return true;
}

//-----------------------------------------------------------------------------
// Client의 실행 Mode를 설정한다.
//-----------------------------------------------------------------------------
// game 상태 변경과
// UI의 mode변경을 처리한다.
//
// (!)다른 곳~에서는 game상태변경이나 UI mode변경을 처리하지 않도록 해야한다.
//-----------------------------------------------------------------------------
void SetMode(enum CLIENT_MODE mode) {
  const enum CLIENT_MODE previousMode = g_Mode;
  Phoenix_ClientLifecycleLogf(
      "SetMode request prev=%d(%s) new=%d(%s) update=%p socket=%p activeApp=%d activeGame=%d needUpdate=%d",
      (int)previousMode,
      PhoenixClientModeName(previousMode),
      (int)mode,
      PhoenixClientModeName(mode),
      (void*)g_pUpdate,
      (void*)g_pSocket,
      (int)g_bActiveApp,
      (int)g_bActiveGame,
      (int)g_bNeedUpdate);
  if (mode == MODE_QUIT)
  {
    Phoenix_ClientLifecycleLogStack("SetMode MODE_QUIT stack");
  }
  g_Mode = mode;
  CT_Done("DIAG: SetMode called (mode:%d)", (int)mode);

  // 입력을 초기화한다.
  if (g_pDXInput != NULL) {
    g_pDXInput->UpdateInput();
    g_pDXInput->Clear();

    // acquire
    g_pDXInput->SetAcquire(true);

    // 입력을 초기화한다.
    g_pDXInput->Clear();
  }

  g_bLButtonDown = FALSE;
  g_bRButtonDown = FALSE;
  g_bCButtonDown = FALSE;

  switch (g_Mode) {
  //------------------------------------------------------
  // 초기 화면
  //------------------------------------------------------
  case MODE_OPENING:
    // Debug Message
    DEBUG_ADD("[ SetMode ]  OPENING");

    UnInitSound(); // 잠시 Sound 중지

    g_pCOpeningUpdate->PlayMPG("test.mpg");

    //----------------------------------------------
    // update 함수 설정
    //----------------------------------------------
    g_pUpdate = g_pCOpeningUpdate;
    CT_Done("DIAG: g_pUpdate set to g_pCOpeningUpdate (MODE_OPENING)");
    g_pCOpeningUpdate->Init();
    break;

  //------------------------------------------------------
  // 뭔가 option을 바꾼다. - -;
  //------------------------------------------------------
  case MODE_CHANGE_OPTION:
    if ( // 3D가속 중인데.. 가속 끄는 경우
        CDirect3D::IsHAL() && !g_pUserOption->Use3DHAL
        // 3D가속 아닌데.. 가속 하는 경우
        || !CDirect3D::IsHAL() && g_pUserOption->Use3DHAL) {
      // if (g_pTopView!=NULL)
      {
        //	delete g_pTopView;//->Release();
        //	g_pTopView = NULL;
      }
      //--------------------------------------------------
      // 음악 멈춘다.
      //--------------------------------------------------
      if (g_pUserOption->PlayWaveMusic) {
#ifdef __USE_MP3__
        DEBUG_ADD("MP3 STOP1");
        g_pMP3->Stop();
        DEBUG_ADD("MP3 STOP1 OK");
//					g_pDXSoundStream->Stop();
#else
        if (g_DXSound.IsInit())
          BGMManager::Instance()->StopMusic();
        // g_pOGG->streamClose();
#endif
      } else {
        g_Music.Stop();
      }

      if (CDirect3D::IsHAL()) {
        CDirect3D::Release();
      }

      CDirectDraw::RestoreGammaRamp();
      CDirectDraw::ReleaseAll();

      InitDraw();
      InitSurface();

      //-----------------------------------------------------------------
      // 잠시 기다려 달라고 출력..
      //-----------------------------------------------------------------
      g_pUIDialog->PopupFreeMessageDlg(
          (*g_pGameStringTable)[STRING_MESSAGE_WAIT].GetString(), -1, -1, 0);

      gC_vs_ui.Process();
      gC_vs_ui.Show();

      //				if (CDirect3D::IsHAL())
      //				{
      //				}
      //				else
      {
        //-----------------------------------------------------------------
        // Last를 Back으로 copy - 3D HAL이 아닌 경우만..
        //-----------------------------------------------------------------
        POINT point = {0, 0};
        RECT rect = {0, 0, SURFACE_WIDTH, SURFACE_HEIGHT};

        g_pBack->BltNoColorkey(&point, g_pLast, &rect);
      }

      CDirectDraw::Flip();

      //-----------------------------------------------------------------
      // g_pTopView : 2D <--> 3D 바뀔때..
      //-----------------------------------------------------------------
      if (g_pTopView != NULL) {
        g_pTopView->InitChanges();
      }

      g_pUIDialog->CloseMessageDlg();
    }

    // 바로 다음에 MAINMENU로.. - -;
    SetMode(MODE_MAINMENU);
    break;

  //------------------------------------------------------
  // Login 상태 설정..
  //------------------------------------------------------
  case MODE_MAINMENU:
    // Debug Message
    DEBUG_ADD("[ SetMode ]  MAINMENU");

    // acquire
    g_pDXInput->SetAcquire(g_bActiveGame == TRUE);

    //------------------------------------------------------
    // Socket 해제..
    //------------------------------------------------------
    if (NULL == g_pUserInformation->pLogInClientPlayer &&
        true == g_pUserInformation
                    ->IsAutoLogIn) // 웹 로긴이고 한번도 로그인 하지 않았을경우
      ReleaseSocket();

    //------------------------------------------------------
    // [ TEST CODE ]
    // 여기서 sound를 초기화해도 되남??
    //------------------------------------------------------
    InitSound();

    g_CurrentFrame = 0;

    g_bSetHotKey = FALSE;

    //------------------------------------------------------
    // ui dialog
    //------------------------------------------------------
    g_pUIDialog->HidePCTalkDlg();

    //------------------------------------------------------
    // 모두 대화 허용
    //------------------------------------------------------
    g_pChatManager->ClearID();
    g_pChatManager->SetAcceptMode();

    // Title화면 UI시작
    // gC_vs_ui.EndTitle();
    gC_vs_ui.StartTitle();

    gC_vs_ui.DropItem();

    g_bUIInput = FALSE;

    // 귓속말 안 받을려고... 냠 -- ;
    if (g_pUserInformation != NULL) {
      // 귓속말 대상을 지워준다.
      g_pUserInformation->WhisperID.Release();

      g_pUserInformation->CharacterID.Release();
    }

    //----------------------------------------------
    // message 제거
    //----------------------------------------------
    g_pSystemMessage->Clear();
    g_pGameMessage->Clear();
    g_pNoticeMessage->Clear();

    // UpdateDisconnected();

    //------------------------------------------------------
    // 음악 시작
    //------------------------------------------------------
    if (g_pUserOption->PlayWaveMusic) {
      //				g_pDXSoundStream->Stop();
      DEBUG_ADD("MP3 STOP2");
#ifdef __USE_MP3__
      g_pMP3->Stop();
#else

      if (g_DXSound.IsInit()) {
        // 					if( g_pSoundBufferForOGG == NULL
        // ) 						g_pSoundBufferForOGG =
        // new CDirectSoundBuffer(g_hWnd, SOUND_STEREO, SOUND_44K, SOUND_16BIT);
        //
        // 					if( g_pOGG == NULL )
        // #ifdef _MT
        // 						g_pOGG = new
        // COGGSTREAM(g_hWnd, g_pSoundBufferForOGG, 44100, 11025, 8800); #else
        // 						g_pOGG = new
        // COGGSTREAM(g_hWnd, g_pSoundBufferForOGG, 44100, 11025, 8800,1);
        // #endif
        //
        // 					g_pOGG->streamClose();

        BGMManager::Instance()->StopMusic();
      }
#endif
      DEBUG_ADD("MP3 STOP2 OK");

      if (g_pUserOption->PlayMusic) {
        // Title muzik random pool (TitleMusic.h) - 2026-05-14
        int musicID = GetTitleMusicID();

        if (musicID != MUSICID_NULL) {
#ifdef __USE_MP3__
          DEBUG_ADD("MP3 OPEM");
          g_pMP3->Open((*g_pMusicTable)[musicID].FilenameWav);
          DEBUG_ADD("MP3 OPEM OK");
          DEBUG_ADD("MP3 PLAY1");
          g_pMP3->Play(false);
          DEBUG_ADD("MP3 PLAY1 OK");
//						g_pDXSoundStream->Load(
//(*g_pMusicTable)[ musicID ].FilenameWav );
//g_pDXSoundStream->Play( FALSE );
#else
          // 						if( g_oggfile != NULL )
          // 							fclose(g_oggfile);
          //
          // 						g_oggfile = NULL;
          //
          // 						if( g_DXSound.IsInit() )
          // 						{
          // 							g_oggfile =
          // fopen( (*g_pMusicTable)[ musicID ].FilenameWav ,"rb");
          // 							g_pOGG->streamLoad(
          // g_oggfile, NULL );
          // g_pOGG->streamPlay(SOUND_PLAY_ONCE);
          // int volume = (g_pUserOption->VolumeMusic - 15) * 250;
          // g_pOGG->streamVolume( max( -10000, min( -1, volume ) ) );
          // 						}

          BGMManager::Instance()->PlayMusic(
              (*g_pMusicTable)[musicID].FilenameWav);
          BGMManager::Instance()->SetVolumeFromSlider(g_pUserOption->VolumeMusic);

#endif
        }
      }
    } else {
      g_Music.Stop();

      if (g_pUserOption->PlayMusic) {
        // Title muzik random pool (TitleMusic.h) - 2026-05-14
        int musicID = GetTitleMusicID();

        if (musicID != MUSICID_NULL) {
          g_Music.Play((*g_pMusicTable)[musicID].Filename);
        }
      }
    }

    // g_pDXSoundStream->Load( "music.wav" );//(*g_pMusicTable)[
    // g_pClientConfig->MUSIC_THEME ].Filename ); g_pDXSoundStream->Play(TRUE);

    // g_DXMusic.SetOriginalTempo();
    // g_DXMusic.Stop();

    // g_DXMusic.Play( (*g_pMusicTable)[ g_pClientConfig->MUSIC_THEME ].Filename
    // );

    //----------------------------------------------
    // update 함수 설정
    //----------------------------------------------
    g_UpdateDelay = DELAY_UPDATE_LOGIN;
    g_pUpdate = g_pCWaitUIUpdate;
    CT_Done("DIAG: g_pUpdate set to g_pCWaitUIUpdate (MODE_MAINMENU)");
    g_pCWaitUIUpdate->Init();
    break;

  //------------------------------------------------------
  // PC를 선택하기 위한 정보를 기다린다.
  //------------------------------------------------------
  case MODE_NEWUSER: {
    // Debug Message
    DEBUG_ADD("[ SetMode ]  MODE_NEWUSER");

    //----------------------------------------------
    // Player의 정보 초기화
    //----------------------------------------------
    // g_pPlayer->SetCreatureType( 0 );

    //----------------------------------------------
    // update 함수 설정
    //----------------------------------------------
    g_UpdateDelay = DELAY_UPDATE_LOGIN;
    g_pUpdate = g_pCWaitUIUpdate;
    g_pCWaitUIUpdate->Init();
  } break;

  //------------------------------------------------------
  // Login이 성공되기를 기다린다.
  //------------------------------------------------------
  // (ID, Password)를 보내고
  // Login이 성공 되기를 기다린다.
  //------------------------------------------------------
  case MODE_WAIT_LOGINOK: {
    // Debug Message
    DEBUG_ADD("[ SetMode ]  MODE_WAIT_LOGINOK");

    //------------------------------------------------------
    // Loading 중지
    //------------------------------------------------------
    // StopFileThread();

    //----------------------------------------------
    // update 함수 설정
    //----------------------------------------------
    g_UpdateDelay = DELAY_UPDATE_WAITING;
    g_pUpdate = g_pCWaitPacketUpdate;
    g_pCWaitPacketUpdate->SetDelay(g_pClientConfig->MAX_WAIT_PACKET);
    g_pCWaitPacketUpdate->Init();
  } break;

  //------------------------------------------------------
  // MODE_WAIT_WORLD_LIST
  //------------------------------------------------------
  case MODE_WAIT_WORLD_LIST: {
    // Debug Message
    DEBUG_ADD("[ SetMode ]  MODE_WAIT_WORLD_LIST");

    //----------------------------------------------
    // update 함수 설정
    //----------------------------------------------
    g_UpdateDelay = DELAY_UPDATE_WAITING;
    g_pUpdate = g_pCWaitPacketUpdate;
    g_pCWaitPacketUpdate->SetDelay(g_pClientConfig->MAX_WAIT_PACKET);
    g_pCWaitPacketUpdate->Init();
  } break;

  //------------------------------------------------------
  // MODE_WAIT_SELECT_WORLD
  //------------------------------------------------------
  case MODE_WAIT_SELECT_WORLD: {
    // Debug Message
    DEBUG_ADD("[ SetMode ]  MODE_WAIT_SELECT_WORLD");

    g_pUserInformation->KeepConnection = FALSE;

    //----------------------------------------------
    // update 함수 설정
    //----------------------------------------------
    g_UpdateDelay = DELAY_UPDATE_LOGIN;
    g_pUpdate = g_pCWaitUIUpdate;
    g_pCWaitUIUpdate->Init();
  } break;

  //------------------------------------------------------
  // MODE_WAIT_SERVER_LIST
  //------------------------------------------------------
  case MODE_WAIT_SERVER_LIST: {
    // Debug Message
    DEBUG_ADD("[ SetMode ]  MODE_WAIT_SERVER_LIST");

    //----------------------------------------------
    // update 함수 설정
    //----------------------------------------------
    g_UpdateDelay = DELAY_UPDATE_WAITING;
    g_pUpdate = g_pCWaitPacketUpdate;
    g_pCWaitPacketUpdate->SetDelay(g_pClientConfig->MAX_WAIT_PACKET);
    g_pCWaitPacketUpdate->Init();
  } break;

  //------------------------------------------------------
  // MODE_WAIT_SELECT_SERVER
  //------------------------------------------------------
  case MODE_WAIT_SELECT_SERVER: {
    // Debug Message
    DEBUG_ADD("[ SetMode ]  MODE_WAIT_SELECT_SERVER");

    g_pUserInformation->KeepConnection = FALSE;

    //----------------------------------------------
    // update 함수 설정
    //----------------------------------------------
    g_UpdateDelay = DELAY_UPDATE_LOGIN;
    g_pUpdate = g_pCWaitUIUpdate;
    g_pCWaitUIUpdate->Init();
  } break;

  //------------------------------------------------------
  // 잘못 입력한 경우이다.
  //------------------------------------------------------
  case MODE_LOGIN_WRONG: {
    // gC_vs_ui.InvalidIdPasswordMessage();

    /*
    //InitFail("Server가 응답하지 않습니다.");
    //InitFail("Server와의 접속이 끊어졌습니다.");
    g_pBack->GDI_Text(101,201, "ID나 Password가 틀렸습니다.", RGB(0,0,0));
    g_pBack->GDI_Text(100,200, "ID나 Password가 틀렸습니다.", RGB(220,220,220));

    g_pBack->GDI_Text(101,221, "[ESC]를 누르세요.", RGB(0,0,0));
    g_pBack->GDI_Text(100,220, "[ESC]를 누르세요.", RGB(220,220,220));

    CDirectDraw::Flip();

    // return을 누를 때까지...
    while (1)
    {
            UpdateInput();

            if (g_pDXInput->KeyDown(DIK_ESCAPE))
            {
                    break;
            }
    }
    */

    // 다시 mainmenu로..

    g_Mode = MODE_MAINMENU;

    //-----------------------
    // Socket 해제..
    //-----------------------
    ReleaseSocket();

    //----------------------------------------------
    // update 함수 설정
    //----------------------------------------------
    g_UpdateDelay = DELAY_UPDATE_LOGIN;
    g_pUpdate = g_pCWaitUIUpdate;
    g_pCWaitUIUpdate->Init();
  } break;

  //------------------------------------------------------
  // Register되기를 기다린다.
  //------------------------------------------------------
  case MODE_WAIT_REGISTERPLAYEROK: {
    // Debug Message
    DEBUG_ADD("[ SetMode ]  MODE_WAIT_REGISTERPLAYEROK");

    //----------------------------------------------
    // update 함수 설정
    //----------------------------------------------
    g_UpdateDelay = DELAY_UPDATE_WAITING;
    g_pUpdate = g_pCWaitPacketUpdate;
    g_pCWaitPacketUpdate->SetDelay(g_pClientConfig->MAX_WAIT_PACKET);
    g_pCWaitPacketUpdate->Init();
  } break;

  //------------------------------------------------------
  // PC를 선택하기 위한 정보를 기다린다.
  //------------------------------------------------------
  case MODE_WAIT_PCLIST: {
    // Debug Message
    DEBUG_ADD("[ SetMode ]  MODE_WAIT_PCLIST");

    //------------------------------------------------------------
    // Characer 선택 창을 띄워야 한다.
    //------------------------------------------------------------
    // LCPCListHandler에서 하게 했다.
    // UI_StartCharacterManager();

    g_ZoneRandomSoundTime = g_CurrentTime;

    //------------------------------------------------------------
    // server name 설정
    //------------------------------------------------------------
    /*
    int serverID = g_pServerInformation->GetServerGroupID();
    int status = g_pServerInformation->GetServerGroupStatus();

    char str[80];

    if (g_pServerInformation->GetServerGroupName()!=NULL)
    {
            strcpy(str, g_pServerInformation->GetServerGroupName());

            //gC_vs_ui.SetServerName( str );
            DEBUG_ADD_FORMAT("[ SetMode ]  MODE_WAIT_PCLIST -
ServerName[%d]=%s", serverID, str);
    }

//			gC_vs_ui.SetServerDefault( str, serverID,
(C_VS_UI_CHAR_MANAGER::SERVER_STATUS)status );

//			gC_vs_ui.CharManagerDisable();
    */

    //----------------------------------------------
    // update 함수 설정
    //----------------------------------------------
    g_UpdateDelay = DELAY_UPDATE_WAITING;
    g_pUpdate = g_pCWaitPacketUpdate;
    g_pCWaitPacketUpdate->SetDelay(g_pClientConfig->MAX_WAIT_PACKET);
    g_pCWaitPacketUpdate->Init();
  } break;

  //------------------------------------------------------
  // PC를 선택하기 위한 정보를 기다린다.
  //------------------------------------------------------
  case MODE_WAIT_SELECTPC: {
    // Debug Message
    DEBUG_ADD("[ SetMode ]  MODE_WAIT_SELECTPC");

    //----------------------------------------------
    // Player의 정보 초기화
    //----------------------------------------------
    // g_pPlayer->SetCreatureType( 0 );
    g_pUserInformation->KeepConnection = FALSE;

    //			gC_vs_ui.CharManagerEnable();

    SelectLastSelectedCharacter();

    //----------------------------------------------
    // update 함수 설정
    //----------------------------------------------
    g_MorphCreatureType = 0;
    g_UpdateDelay = DELAY_UPDATE_LOGIN;
    g_pUpdate = g_pCWaitUIUpdate;
    g_pCWaitUIUpdate->Init();
  } break;

  //------------------------------------------------------
  // PC가 제대로 생성되었나?
  //------------------------------------------------------
  case MODE_WAIT_CREATEPCOK: {
    // Debug Message
    DEBUG_ADD("[ SetMode ]  MODE_WAIT_CREATEPCOK");

    //----------------------------------------------
    // update 함수 설정
    //----------------------------------------------
    g_UpdateDelay = DELAY_UPDATE_WAITING;
    g_pUpdate = g_pCWaitPacketUpdate;
    g_pCWaitPacketUpdate->SetDelay(g_pClientConfig->MAX_WAIT_PACKET);
    g_pCWaitPacketUpdate->Init();
  } break;

  //------------------------------------------------------
  // PC가 제대로 생성되었나?
  //------------------------------------------------------
  case MODE_WAIT_DELETEPCOK: {
    // Debug Message
    DEBUG_ADD("[ SetMode ]  MODE_WAIT_DELETEPCOK");

    //----------------------------------------------
    // update 함수 설정
    //----------------------------------------------
    g_UpdateDelay = DELAY_UPDATE_WAITING;
    g_pUpdate = g_pCWaitPacketUpdate;
    g_pCWaitPacketUpdate->SetDelay(g_pClientConfig->MAX_WAIT_PACKET);
    g_pCWaitPacketUpdate->Init();
  } break;

  //------------------------------------------------------
  // GameServer로 접속하기 위해서...
  //------------------------------------------------------
  case MODE_WAIT_RECONNECT: {
    // Debug Message
    DEBUG_ADD("[ SetMode ]  MODE_WAIT_RECONNECT");

    //----------------------------------------------
    // update 함수 설정
    //----------------------------------------------
    g_UpdateDelay = DELAY_UPDATE_WAITING;
    g_pUpdate = g_pCWaitPacketUpdate;
    g_pCWaitPacketUpdate->SetDelay(g_pClientConfig->MAX_WAIT_PACKET);
    g_pCWaitPacketUpdate->Init();
  } break;

  //------------------------------------------------------
  // GameServer에서 LoginServer로 재접속하기 위해서...
  //------------------------------------------------------
  case MODE_WAIT_RECONNECT_LOGIN: {
    // Debug Message
    DEBUG_ADD("[ SetMode ]  MODE_WAIT_RECONNECT_LOGIN");

    // 귓속말 안 받을려고... 냠 -- ;
    if (g_pUserInformation != NULL) {
      g_pUserInformation->CharacterID.Release();
    }

    //----------------------------------------------
    // update 함수 설정
    //----------------------------------------------
    g_UpdateDelay = DELAY_UPDATE_WAITING;
    g_pUpdate = g_pCWaitPacketUpdate;
    g_pCWaitPacketUpdate->SetDelay(g_pClientConfig->MAX_WAIT_PACKET);
    g_pCWaitPacketUpdate->Init();
  } break;

  //------------------------------------------------------
  // UpdateInfo를 받는다.
  //------------------------------------------------------
  case MODE_WAIT_UPDATEINFO: {
    //--------------------------------------------------
    // Player 초기화
    //--------------------------------------------------
    if (g_pPlayer != NULL) {
      if (g_pZone != NULL) {
        g_pZone->RemoveCreature(g_pPlayer->GetID());
      }

      delete g_pPlayer; //->SetSpecialActionInfoNULL();
      g_pPlayer = NULL;
    }

    //--------------------------------------------------
    // 정당방위 초기화
    //--------------------------------------------------
    g_pJusticeAttackManager->Release();

    //--------------------------------------------------
    //
    // Skill Tree 초기화 - 임시로.. - -;;
    //
    //--------------------------------------------------
    g_pSkillManager->Init();

    //------------------------------
    // 게임 UI 를 시작한다.
    //------------------------------
    UI_SetCharInfoName(g_pUserInformation->CharacterID);
    DEBUG_ADD("[ SetMode ] SET CHARINFO");
    UI_StartGame();

    DEBUG_ADD("[ SetMode ] UI FINISH");

    g_ZoneRandomSoundTime = g_CurrentTime;

    // Debug Message
    DEBUG_ADD("[ SetMode ]  MODE_WAIT_UPDATEINFO");

    //----------------------------------------------
    // update 함수 설정
    //----------------------------------------------
    g_UpdateDelay = DELAY_UPDATE_WAITING;
    g_pUpdate = g_pCWaitPacketUpdate;
    g_pCWaitPacketUpdate->SetDelay(g_pClientConfig->MAX_WAIT_PACKET);
    g_pCWaitPacketUpdate->Init();
  } break;

  //------------------------------------------------------
  // Player의 좌표를 받기 위해서 기다린다.
  //------------------------------------------------------
  case MODE_WAIT_SETPOSITION: {
    // Debug Message
    DEBUG_ADD("[ SetMode ]  MODE_WAIT_SETPOSITION");

    //--------------------------------------------------
    // Option 저장
    //--------------------------------------------------
    if (g_pUserOption != NULL) {
      g_pUserOption->SaveToFile(
          g_pFileDef->getProperty("FILE_INFO_USEROPTION").c_str());
    }

    // 정지..
    g_pPlayer->SetStop();
    g_pPlayer->SetAction(ACTION_STAND);

    // 키 입력 제거
    g_pDXInput->Clear();

    g_bLButtonDown = FALSE;
    g_bRButtonDown = FALSE;
    g_bCButtonDown = FALSE;

    // [ TEST CODE ]
    InitSound();

    static int first = 1;
    if (first) {
      g_pSystemMessage->Add(
          (*g_pGameStringTable)[STRING_MESSAGE_HELP_KEY].GetString());
      first = 0;
    }

    //--------------------------------------------------
    // 게임 서버로 CGReady 패킷을 보낸다.
    //--------------------------------------------------
    CGReady cgReady;
    g_pSocket->sendPacket(&cgReady);
    /*				CGSetVampireHotKey _CGSetVampireHotKey;
                                    for (int i=0; i < 8; i++)
                                    {
                                            int id = 268;

                                            if (id != NOT_SELECTED)
                                            {
                                                     //save(i, id)
                                                     //선택 안된 것이라도..
       그대로 저장시켰다가 받아오면 된다. _CGSetVampireHotKey.setHotKey( i, id
       );
                                            }
                                    }
                                    g_pSocket->sendPacket(
       &_CGSetVampireHotKey); g_pSocket->sendPacket( &cgReady );
    */
    g_pSocket->setPlayerStatus(CPS_WAITING_FOR_GC_SET_POSITION);

    // 바로 보낸다.
    UpdateSocketOutput();

    // 2002.6.28 [UDP수정]
    // 서버에 UDP port를 알려주기 위해서
    //				CGPortCheck cgPortCheck;
    //				cgPortCheck.setPCName(
    // g_pUserInformation->CharacterID.GetString() );
    //
    //				string ServerAddress =
    // g_pConfig->getProperty("LoginServerAddress");
    // uint ServerPort = g_pConfig->getPropertyInt("LoginServerCheckPort");
    //
    //				g_pClientCommunicationManager->sendPacket(
    // ServerAddress,
    // ServerPort,
    // &cgPortCheck );

    //
    g_SelectSector.x = 0;
    g_SelectSector.y = 0;

    if (g_pTopView != NULL) {
      g_pTopView->ClearShadowManager();

      g_pTopView->SetSelectedNULL();
    }

    // update 함수
    g_UpdateDelay = DELAY_UPDATE_WAITING;
    g_pUpdate = g_pCWaitPacketUpdate;
    g_pCWaitPacketUpdate->SetDelay(g_pClientConfig->MAX_WAIT_PACKET);
    g_pCWaitPacketUpdate->Init();
  } break;

  //------------------------------------------------------
  // 게임을 시작한다.
  //------------------------------------------------------
  case MODE_GAME:
    DEBUG_ADD("---------- Start Game ---------- ");

    DEBUG_ADD("CDirectDraw::RestoreAllSurfaces()");
    CDirectDraw::RestoreAllSurfaces();

    DEBUG_ADD("CDirect3D::Restore()");
    CDirect3D::Restore();

    DEBUG_ADD("TempInformation");

    //-----------------------------------------------------------
    // temp information제거
    //-----------------------------------------------------------
    g_pTempInformation->SetMode(TempInformation::MODE_NULL);

    DEBUG_ADD("UserInformation");
    //-----------------------------------------------------------
    // Logout 시간 제거
    //-----------------------------------------------------------
    g_pUserInformation->LogoutTime = 0;

#if __CONTENTS(__080405_FIREST_UI_UPDATE)
    g_pUserInformation->QuitExitTime = 0;
#endif //__080405_FIREST_UI_UPDATE

    //-----------------------------------------------------------
    // hot key를 저장한다.
    //-----------------------------------------------------------
    DEBUG_ADD("SaveHotKey");
    UI_SaveHotKeyToServer();

    //-----------------------------------------
    // Button 초기화
    //-----------------------------------------
    g_bLButtonDown = FALSE;
    g_bRButtonDown = FALSE;
    g_bCButtonDown = FALSE;

    //-----------------------------------------
    // UI Dialog에서 입력 막은거 풀어줌..
    //-----------------------------------------
    DEBUG_ADD("UIDialog");
    UIDialog::UnSetLockInputPCTalk();
    UIDialog::UnSetLockInputMessage();

    // 부활 버튼 없애기
    // 이거는 GCUpdateInfoHandler에서 해야하는데.. 일단. - -;
    DEBUG_ADD("FinishRequestDie");
    gC_vs_ui.FinishRequestDie();
    gC_vs_ui.FinishRequestResurrect();
    gC_vs_ui.CloseRequestShrineMinimap();

    //			//-----------------------------------------
    //			// 자크 도움말..
    //			//-----------------------------------------
    //			if (g_pPlayer!=NULL
    //				&& g_pPlayer->IsSlayer()
    //				&& g_pInventory!=NULL
    //				&& g_pSlayerGear!=NULL
    //				&&
    // g_pPlayer->GetSTR()+g_pPlayer->GetDEX()+g_pPlayer->GetINT()==30
    //				&& g_pInventory->GetItemNum()==0
    //				&& g_pSlayerGear->GetItemNum()==0)
    //			{
    //				gC_vs_ui.AddChatToHistory((*g_pGameStringTable)[STRING_MESSAGE_HOW_TO_GET_BASIC_WEAPON].GetString(),
    //											(*g_pGameStringTable)[UI_STRING_MESSAGE_HELP_MESSAGE].GetString(),
    //											CLD_INFO,
    // RGB(130, 230, 230));
    //			}

    g_UpdateDelay = DELAY_UPDATE_GAME;

    DEBUG_ADD("SetUpdate");
    g_pUpdate = g_pCGameUpdate;
    g_pCGameUpdate->Init();

    ExecuteHelpEvent(HELP_EVENT_INTERFACE);
    //			if(!g_pUserInformation->IsNetmarble)
    //			{
    //				ExecuteHelpEvent( HELP_EVENT_CAMPAIGN );
    //				ExecuteHelpEvent( HELP_EVENT_EVENT );
    //			}

    //--------------------------------------------------------
    // 플레이 화면으로 바뀔 때 배경음악 재생
    //--------------------------------------------------------
    PlayMusicCurrentZone();
    gGetApplcationRunTime(true);

    break;

  //------------------------------------------------------
  // Program을 끝낸다.
  //------------------------------------------------------
  case MODE_QUIT:
    // Logout을 보낸다.
    if (g_pSocket != NULL) {
      //--------------------------------------------------
      // update할려고 종료하는 경우는 CLLogout을 보낸다.
      //--------------------------------------------------
      if (g_bNeedUpdate || Phoenix_IsLoginServerSocket(g_pSocket)) {
        //--------------------------------------------------
        // Login 서버로 CLLogout 패킷을 보낸다.
        //--------------------------------------------------
        CLLogout clLogout;

        g_pSocket->sendPacket(&clLogout);
        // g_pSocket->setPlayerStatus( CPS_END_SESSION );

      }
      //--------------------------------------------------
      // game중에 종료하는 경우.. CG Logout
      //--------------------------------------------------
      else {
        CGLogout cgLogout;

        g_pSocket->sendPacket(&cgLogout);

#ifdef __DEBUG_OUTPUT__
        DEBUG_ADD(cgLogout.toString().c_str());
#endif
      }
    }

    //--------------------------------------------------
    // Thread Loading 종료..
    //--------------------------------------------------
    //			StopLoadingThread();

    // Update함수 설정
    g_pUpdate = NULL;

    g_bUIInput = FALSE;

    // window 닫기
    g_bActiveApp = FALSE;
    if (g_hWnd != NULL && IsWindow(g_hWnd)) {
      if (!PostMessage(g_hWnd, WM_CLOSE, 0, 0)) {
        CT_Done("DIAG: MODE_QUIT PostMessage(WM_CLOSE) failed, posting WM_QUIT");
        PostQuitMessage(0);
      }
    } else {
      CT_Done("DIAG: MODE_QUIT without a valid window, posting WM_QUIT");
      PostQuitMessage(0);
    }
    break;
  }

  // 입력을 초기화한다.
  g_pDXInput->Clear();
}

//-----------------------------------------------------------------------------
// Check Activate
//-----------------------------------------------------------------------------
// 프로그램이 실행중인지 아닌지에 따라서 처리..
// ALT + TAB 과 관련이 깊다. - -;
//-----------------------------------------------------------------------------
void CheckActivate(BOOL bActiveGame) {
  // static BOOL musicPause = FALSE; //g_Music.IsPause();
  const bool fpEnabled = Phoenix_FrameProbe_Enabled();
  LONGLONG fpActivateStart = 0;
  double fpInputMs = 0.0;
  double fpGammaMs = 0.0;
  double fpRestore1Ms = 0.0;
  double fpRestore2Ms = 0.0;
  double fpD3DRestoreMs = 0.0;
  double fpTopViewMs = 0.0;
  double fpUIRestoreMs = 0.0;
  if (fpEnabled)
    fpActivateStart = Phoenix_FrameProbe_Now();

#ifdef OUTPUT_DEBUG
  if (g_pDebugMessage != NULL) {
    if (bActiveGame) {
      if (g_bActiveApp) {
        DEBUG_ADD("[CheckActivate] ActiveGame, ActiveApp");
      } else {
        DEBUG_ADD("[CheckActivate] ActiveGame, DeactiveApp");
      }
    } else {
      if (g_bActiveApp) {
        DEBUG_ADD("[CheckActivate] DeactiveGame, ActiveApp");
      } else {
        DEBUG_ADD("[CheckActivate] DeactiveGame, DeactiveApp");
      }
    }
  }
#endif

  // FAZ 4 (2026-05-05): Tehlikeli explicit destructor + Init pattern kaldırıldı.
  // `g_pDXInput->~CDirectInput()` + `Init()` placement-new olmadan UB —
  // vtable corrupt potansiyeli, focus regain crash kaynaği.
  // Standart DirectInput pattern: SetAcquire(false) → focus loss,
  // SetAcquire(true) → focus regain. Re-init yalnızca cihaz değişiminde gerekir.
  CT_Done("CHKACT: enter bActiveGame=%d g_bActiveApp=%d g_Mode=%d g_pDXInput=%p g_pTopView=%p g_pUserOption=%p g_pPlayer=%p",
             (int)bActiveGame, (int)g_bActiveApp, (int)g_Mode,
             (void*)g_pDXInput, (void*)g_pTopView, (void*)g_pUserOption, (void*)g_pPlayer);
  if (g_pDXInput != NULL) {
    CT_Done("CHKACT: pre DXInput SetAcquire/Clear");
    LONGLONG fpInputStart = 0;
    if (fpEnabled)
      fpInputStart = Phoenix_FrameProbe_Now();
    g_pDXInput->SetAcquire(bActiveGame == TRUE);
    g_pDXInput->Clear();
    if (fpEnabled)
      fpInputMs = Phoenix_FrameProbe_Ms(fpInputStart, Phoenix_FrameProbe_Now());
    CT_Done("CHKACT: post DXInput SetAcquire/Clear");
  }

  //----------------------------------------------------
  // 프로그램 진행..
  //----------------------------------------------------
  g_bActiveGame = FALSE;

  extern bool g_bTestMode;
  if (g_bActiveApp
#ifdef OUTPUT_DEBUG
      || g_bTestMode
#endif
  ) {
    // CDirectDraw::RestoreAllSurfaces();

    CT_Done("CHKACT: pre RestoreGammaRamp");
    LONGLONG fpGammaStart = 0;
    if (fpEnabled)
      fpGammaStart = Phoenix_FrameProbe_Now();
    CDirectDraw::RestoreGammaRamp();
    if (fpEnabled)
      fpGammaMs = Phoenix_FrameProbe_Ms(fpGammaStart, Phoenix_FrameProbe_Now());
    CT_Done("CHKACT: post RestoreGammaRamp");

    if (bActiveGame) {
      CT_Done("CHKACT: REGAIN branch — pre 1st RestoreAllSurfaces");
      LONGLONG fpRestore1Start = 0;
      if (fpEnabled)
        fpRestore1Start = Phoenix_FrameProbe_Now();
      CDirectDraw::RestoreAllSurfaces();
      if (fpEnabled)
        fpRestore1Ms = Phoenix_FrameProbe_Ms(fpRestore1Start, Phoenix_FrameProbe_Now());
      CT_Done("CHKACT: REGAIN — post 1st RestoreAllSurfaces");

      DEBUG_ADD("WM_ACTIVATEAPP : Restore Surfaces");

      //----------------------------------------------------
      // global setting
      //----------------------------------------------------
      g_bActiveGame = TRUE;

      if (CDirect3D::IsHAL()) {
        // FAZ 4 (2026-05-06): Release + Init re-cycle KALDIRILDI.
        // ROOT CAUSE: focus regain sırasında CDirect3D::Init() crash
        // ediyordu (CHKACT trace ile kanıtlandı: pre Init marker var,
        // post yok). Modern Windows + dgVoodoo HAL emülasyonunda DX7
        // cihaz "persistent" yapı; orijinal VS6 dönemi gerçek hardware
        // GPU'da Release+Init focus regain için gerekiyordu, dgVoodoo'da
        // ise re-init = crash. Sadece Restore() çağrısı yeterli; surface
        // restore'u zaten yukarıdaki 1st + aşağıdaki 2nd RestoreAllSurfaces
        // hallediyor.
        DEBUG_ADD("IsHAL : Skip Release/Init re-cycle (dgVoodoo crash) → Restore only");
        CT_Done("CHKACT: REGAIN — HAL Restore only (Release/Init kaldırıldı)");
        LONGLONG fpD3DStart = 0;
        if (fpEnabled)
          fpD3DStart = Phoenix_FrameProbe_Now();
        CDirect3D::Restore();
        if (fpEnabled)
          fpD3DRestoreMs += Phoenix_FrameProbe_Ms(fpD3DStart, Phoenix_FrameProbe_Now());
        CT_Done("CHKACT: REGAIN — post CDirect3D::Restore (HAL)");
      }

      if (g_bFullScreen) {
        DEBUG_ADD("FullScreen : Before DD::SetDisplayMode()");
        CT_Done("CHKACT: REGAIN — pre SetDisplayMode (FullScreen)");

        CDirectDraw::SetDisplayModeCompat(SURFACE_WIDTH, SURFACE_HEIGHT, 16);
        CT_Done("CHKACT: REGAIN — post SetDisplayMode");
      }

      DEBUG_ADD("Before Restore All Surfaces");
      CT_Done("CHKACT: REGAIN — pre 2nd RestoreAllSurfaces");

      LONGLONG fpRestore2Start = 0;
      if (fpEnabled)
        fpRestore2Start = Phoenix_FrameProbe_Now();
      CDirectDraw::RestoreAllSurfaces();
      if (fpEnabled)
        fpRestore2Ms = Phoenix_FrameProbe_Ms(fpRestore2Start, Phoenix_FrameProbe_Now());
      CT_Done("CHKACT: REGAIN — post 2nd RestoreAllSurfaces");

      DEBUG_ADD("Before Restore");
      CT_Done("CHKACT: REGAIN — pre CDirect3D::Restore");

      LONGLONG fpD3DStart = 0;
      if (fpEnabled)
        fpD3DStart = Phoenix_FrameProbe_Now();
      CDirect3D::Restore();
      if (fpEnabled)
        fpD3DRestoreMs += Phoenix_FrameProbe_Ms(fpD3DStart, Phoenix_FrameProbe_Now());
      CT_Done("CHKACT: REGAIN — post CDirect3D::Restore");

      DEBUG_ADD("if g_pTopView");
      CT_Done("CHKACT: REGAIN — pre g_pTopView check (g_pTopView=%p)", (void*)g_pTopView);

      if (g_pTopView != NULL && g_pTopView->IsInit()) {
        DEBUG_ADD("Before g_pTopView->RestoreSurface");
        CT_Done("CHKACT: REGAIN — pre g_pTopView->RestoreSurface");

        LONGLONG fpTopViewStart = 0;
        if (fpEnabled)
          fpTopViewStart = Phoenix_FrameProbe_Now();
        g_pTopView->RestoreSurface();

        DEBUG_ADD("After g_pTopView->RestoreSurface");
        CT_Done("CHKACT: REGAIN — post RestoreSurface, pre SetFirstDraw");

        g_pTopView->SetFirstDraw();
        if (fpEnabled)
          fpTopViewMs = Phoenix_FrameProbe_Ms(fpTopViewStart, Phoenix_FrameProbe_Now());
        CT_Done("CHKACT: REGAIN — post SetFirstDraw");
      }

      //--------------------------------------------------------
      // 감마값 재 설정
      //--------------------------------------------------------
      if (g_pUserOption != NULL && g_pClientConfig != NULL) {
        if (g_pUserOption->UseGammaControl &&
            g_pUserOption->GammaValue != 100) {
          CDirectDraw::SetGammaRamp(g_pUserOption->GammaValue);
        }

        //------------------------------------
        // 연주중이면.. 중단..
        //------------------------------------
        if (g_pUserOption->PlayMusic) {
          if (g_pUserOption->PlayWaveMusic) {
            if (g_Mode == MODE_GAME) {
              PlayMusicCurrentZone();
            } else {
              // Title muzik random pool (TitleMusic.h) - 2026-05-14
              int musicID = GetTitleMusicID();

              if (musicID != MUSICID_NULL
#ifdef __USE_MP3__
                  && g_pMP3 != NULL
#else
//								&& g_pOGG !=
// NULL
#endif
                  //								&&
                  // g_pDXSoundStream!=NULL
              ) {
#ifdef __USE_MP3__
                DEBUG_ADD("MP3 OPEM");
                g_pMP3->Open((*g_pMusicTable)[musicID].FilenameWav);
                DEBUG_ADD("MP3 OPEM OK");
                DEBUG_ADD("MP3 PLAY2");
                g_pMP3->Play(false);
                DEBUG_ADD("MP3 PLAY2 OK");
//								g_pDXSoundStream->Load(
//(*g_pMusicTable)[ musicID ].FilenameWav );
//g_pDXSoundStream->Play( FALSE );
#else
                if (g_DXSound.IsInit()) {
                  // 									g_pOGG->streamClose();
                  // 									if(
                  // g_oggfile != NULL )
                  // fclose(g_oggfile);
                  // g_oggfile = fopen(
                  // (*g_pMusicTable)[ musicID ].FilenameWav ,"rb");
                  // if( g_oggfile != NULL )
                  // 									{
                  // 										g_pOGG->streamLoad(
                  // g_oggfile, NULL );
                  // g_pOGG->streamPlay(SOUND_PLAY_ONCE);
                  // int volume = (g_pUserOption->VolumeMusic - 15) * 250;
                  // 										g_pOGG->streamVolume(
                  // max( -10000, min( -1, volume ) ) );
                  // 									}
                  BGMManager::Instance()->PlayMusic(
                      (*g_pMusicTable)[musicID].FilenameWav);

                  BGMManager::Instance()->SetVolumeFromSlider(
                      g_pUserOption->VolumeMusic);
                }
#endif
              }
            }
          } else {
            if (g_Music.IsPlay()) {
              g_Music.Resume();
            } else if (g_Mode == MODE_GAME) {
              PlayMusicCurrentZone();
            } else {
              // Title muzik random pool (TitleMusic.h) - 2026-05-14
              int musicID = GetTitleMusicID();

              if (musicID != MUSICID_NULL && g_pMusicTable != NULL) {
                g_Music.Play((*g_pMusicTable)[musicID].Filename);
              }
            }
          }
        }
        //------------------------------------
        // 연주중이 아닌 경우
        //------------------------------------
        else {
          // nothing - -;
        }
      }

      if (g_Mode != MODE_WAIT_UPDATEINFO) {
        g_DXSound.UnSetMute();
      }

      //--------------------------------
      // mouse 버튼 눌린 상태 제거
      //--------------------------------
      g_bLButtonDown = FALSE;
      g_bRButtonDown = FALSE;
      g_bCButtonDown = FALSE;

      if (g_pPlayer != NULL) {
        g_pPlayer->UnSetLockMode();
      }

      // UI에서 alt+tab처lı
      DEBUG_ADD("UI_RestoreWhenActivateGame");
      CT_Done("CHKACT: REGAIN — pre gC_vs_ui.RestoreWhenActivateGame");

      LONGLONG fpUIRestoreStart = 0;
      if (fpEnabled)
        fpUIRestoreStart = Phoenix_FrameProbe_Now();
      gC_vs_ui.RestoreWhenActivateGame();
      if (fpEnabled)
        fpUIRestoreMs = Phoenix_FrameProbe_Ms(fpUIRestoreStart, Phoenix_FrameProbe_Now());
      CT_Done("CHKACT: REGAIN — post RestoreWhenActivateGame");

      DEBUG_ADD("UI_Restore_ok");
      CT_Done("CHKACT: REGAIN branch DONE");
    }
    //----------------------------------------------------
    // 멈춤..
    //----------------------------------------------------
    else {
      CT_Done("CHKACT: LOSS branch — pre FullScreen check g_bFullScreen=%d", (int)g_bFullScreen);
      if (g_bFullScreen) {
        // CDirectDraw::RestoreGammaRamp();
        CT_Done("CHKACT: LOSS — pre RestoreDisplayMode");
        CDirectDraw::RestoreDisplayModeCompat();
        CT_Done("CHKACT: LOSS — post RestoreDisplayMode");
        // CDirectDraw::RestoreGammaRamp();
      }

#ifdef OUTPUT_DEBUG
      if (g_pDebugMessage != NULL)
        DEBUG_ADD("WM_ACTIVATEAPP : Stop Music");
#endif
      // g_DXMusic.Pause();
      // musicPause = g_Music.IsPause();

      // FAZ 4 (2026-05-06): g_pUserOption NULL guard — eski kod focus loss
      // sırasında g_pUserOption henüz init olmadıysa NULL deref ediyordu.
      // Regain branch'inde guard zaten vardı, loss branch'ine de eklendi.
      CT_Done("CHKACT: LOSS — pre Music branch g_pUserOption=%p", (void*)g_pUserOption);
      if (g_pUserOption != NULL && g_pUserOption->PlayWaveMusic) {
#ifdef __USE_MP3__
        if (g_pMP3 != NULL) {
          DEBUG_ADD("MP3 STOP3");
          g_pMP3->Stop();
          DEBUG_ADD("MP3 STOP3 OK");
        }
//				if (g_pDXSoundStream!=NULL)
//				{
//					g_pDXSoundStream->Stop();
//				}
#else
        //				if( g_pOGG != NULL )
        //				{
        if (g_DXSound.IsInit())
          BGMManager::Instance()->StopMusic();
//						g_pOGG->streamClose();
//				}
#endif
      } else if (g_pUserOption != NULL) {
        g_Music.Stop();
      }
      CT_Done("CHKACT: LOSS — post Music branch");

      if (g_Mode != MODE_WAIT_UPDATEINFO) {
        g_DXSound.SetMute();
      }

      // 반복 동작 중지
      if (g_pPlayer != NULL) {
        g_pPlayer->UnSetRepeatAction();
      }
      CT_Done("CHKACT: LOSS branch DONE");
    }
  }
  if (fpEnabled)
  {
    Phoenix_FrameProbe_RecordActivate(
        (int)bActiveGame,
        Phoenix_FrameProbe_Ms(fpActivateStart, Phoenix_FrameProbe_Now()),
        fpInputMs,
        fpGammaMs,
        fpRestore1Ms,
        fpRestore2Ms,
        fpD3DRestoreMs,
        fpTopViewMs,
        fpUIRestoreMs);
  }
  CT_Done("CHKACT: exit bActiveGame=%d", (int)bActiveGame);

  // Cash Charge WebView2 / ShellExecute: OS imlecini overlay yasamina bagla.
  // Hold flag yoksa no-op — normal alt-tab oyun imlecini bozmaz.
  NotifyCashChargeGameActivate(bActiveGame);
}

//-----------------------------------------------------------------------------
// File Thread Proc
//-----------------------------------------------------------------------------
/*
LONG
FileThreadProc(LPVOID lpParameter)
{
        std::ifstream file;

        while ( 1 )
        {
                WaitForSingleObject(g_hFileEvent, INFINITE);

                #ifdef OUTPUT_DEBUG
                        //		DEBUG_ADD("[Thread Job] Start");
                #endif

                bool bLoad = true;

                switch (g_ThreadJob)
                {
                        //--------------------------------------------------------
                        // Large Zone의 ImageObject를 Loading한다.
                        //--------------------------------------------------------
                        case THREADJOB_LOAD_IMAGEOBJECT_LARGEZONE :
                                g_bZoneLargeLoadImage = false;
                                file.open((*g_pZoneTable).Get(g_nZoneLarge)->Filename,
ios::binary);

                                // Tile Loading
                                if
(g_ThreadJob==THREADJOB_LOAD_IMAGEOBJECT_LARGEZONE)
                                {
                                        #ifdef OUTPUT_DEBUG
                                                //DEBUG_ADD("[Thread Job] Load
Tile LargeZone"); #endif file.seekg(g_pZone->GetTileFilePosition(), ios::beg);
                                        if (!g_pTopView->LoadFromFileTileSPKLargeZone( file ))	// tile만 load
                                        {
                                                #ifdef OUTPUT_DEBUG
                                                //
DEBUG_ADD("[Thread Job] Stop Loading Tile"); #endif file.close(); break;
                                        }
                                }

                                // ImageObject Loading
                                if (bLoad &&
g_ThreadJob==THREADJOB_LOAD_IMAGEOBJECT_LARGEZONE)
                                {
                                        #ifdef OUTPUT_DEBUG
                                                //	DEBUG_ADD("[Thread Job]
Load Imageobject LargeZone"); #endif
                                        file.seekg(g_pZone->GetImageObjectFilePosition(),
ios::beg);
                                        // 모두 load한 경우이면..
                                        if
(g_pTopView->LoadFromFileImageObjectSPKLargeZone( file ))
                                        {
                                                #ifdef OUTPUT_DEBUG
                                                        //
DEBUG_ADD("[Thread Job] Stop Loading ImageObject"); #endif file.close(); break;
                                        }
                                }

                                if (bLoad &&
g_ThreadJob==THREADJOB_LOAD_IMAGEOBJECT_LARGEZONE)
                                {
                                        #ifdef OUTPUT_DEBUG
                                                //	DEBUG_ADD("[Thread Job]
Complete Load"); #endif g_bZoneLargeLoadImage = true;
                                }

                                file.close();
                        break;

                        //--------------------------------------------------------
                        // Small Zone의 ImageObject를 Loading한다.
                        //--------------------------------------------------------
                        case THREADJOB_LOAD_IMAGEOBJECT_SMALLZONE :
                                g_bZoneSmallLoadImage = false;
                                file.open((*g_pZoneTable).Get(g_nZoneSmall)->Filename,
ios::binary);

                                // Tile Loading
                                if
(g_ThreadJob==THREADJOB_LOAD_IMAGEOBJECT_SMALLZONE)
                                {
                                        #ifdef OUTPUT_DEBUG
                                        //		DEBUG_ADD("[Thread Job]
Load Tile SmallZone"); #endif

                                        file.seekg(g_pZone->GetTileFilePosition(),
ios::beg); if (!g_pTopView->LoadFromFileTileSPKLargeZone( file ))	//
tile만 load
                                        {
                                                #ifdef OUTPUT_DEBUG
                                                                //DEBUG_ADD("[Thread
Job] Stop Loading Tile"); #endif file.close(); break;
                                        }
                                }


                                // ImageObject Loading
                                if (bLoad &&
g_ThreadJob==THREADJOB_LOAD_IMAGEOBJECT_SMALLZONE)
                                {
                                        #ifdef OUTPUT_DEBUG
                                                //	DEBUG_ADD("[Thread Job]
Load ImageObject SmallZone"); #endif
                                        file.seekg(g_pZone->GetImageObjectFilePosition(),
ios::beg); if (!g_pTopView->LoadFromFileImageObjectSPKSmallZone( file ))
                                        {
                                                #ifdef OUTPUT_DEBUG
                                                        //
DEBUG_ADD("[Thread Job] Stop Loading ImageObject"); #endif file.close(); break;
                                        }
                                }

                                if (bLoad &&
g_ThreadJob==THREADJOB_LOAD_IMAGEOBJECT_SMALLZONE)
                                {
                                        g_bZoneSmallLoadImage = true;
                                }

                                file.close();
                        break;

                        //--------------------------------------------------------
                        // Creature를 Loading한다.
                        //--------------------------------------------------------
                        case THREADJOB_LOAD_CREATURE :
                                g_pTopView->LoadFromFileCreatureSPK( 0 );
                                g_pTopView->LoadFromFileCreatureSPK( 1 );
                        break;
                }

                #ifdef OUTPUT_DEBUG
                        //	DEBUG_ADD("[Thread Job] End");
                #endif

                ResetEvent(g_hFileEvent);
        }

        return 0L;
}
*/

//-----------------------------------------------------------------------------
// Stop File Thread
//-----------------------------------------------------------------------------
/*
void
StopFileThread()
{
        #ifdef OUTPUT_DEBUG
        //		DEBUG_ADD("[Thread Job] StopFileThread");
        #endif

        switch (g_ThreadJob)
        {
                case THREADJOB_LOAD_IMAGEOBJECT_LARGEZONE :
                        //g_nZoneLarge = ZONEID_NULL;

                        // ImageObject Loading을 중단한다.
                        g_pTopView->StopLoadTileSPK();
                        g_pTopView->StopLoadImageObjectSPK();
                        g_bZoneLargeLoadImage = false;

                        // 우선 순위를 높게 잡고...
                        SetThreadPriority(g_hFileThread,
                                                                THREAD_PRIORITY_HIGHEST);
                break;

                case THREADJOB_LOAD_IMAGEOBJECT_SMALLZONE :
                        //g_nZoneSmall = ZONEID_NULL;

                        // ImageObject Loading을 중단한다.
                        g_pTopView->StopLoadTileSPK();
                        g_pTopView->StopLoadImageObjectSPK();
                        g_bZoneSmallLoadImage = false;

                        // 우선 순위를 높게 잡고...
                        SetThreadPriority(g_hFileThread,
                                                                THREAD_PRIORITY_HIGHEST);
                break;

                case THREADJOB_LOAD_CREATURE :

                        //g_pTopView->StopLoadCreature();

                        // 캐릭터 그림 Load중인가?
                        // 우선 순위를 높게 잡고...
                        SetThreadPriority(g_hFileThread,
                                                                THREAD_PRIORITY_HIGHEST);
                break;
        }

        g_ThreadJob = THREADJOB_NONE;

        #ifdef OUTPUT_DEBUG
        //		DEBUG_ADD("[Thread Job] Wait for Stopping thread");
        #endif

        while (WaitForSingleObject(g_hFileEvent, 0) == WAIT_OBJECT_0);

                // Thread가 하는 Loading이 끝날때까지 기다린다.
        #ifdef OUTPUT_DEBUG
        //		DEBUG_ADD("[Thread Job] Stop File Thread");
        #endif


        // 순위를 좀 낮춘다.
        SetThreadPriority(g_hFileThread,
                                                THREAD_PRIORITY_BELOW_NORMAL);
}
*/

//-----------------------------------------------------------------------------
// Load Creature
//-----------------------------------------------------------------------------
// n번째 Creature Type ID의 Sprite를 load한다.
//-----------------------------------------------------------------------------
BOOL LoadCreature(int spriteType) {
  // Debug Message
  DEBUG_ADD_FORMAT("LoadCreature : %d", spriteType);

  g_pTopView->LoadFromFileCreatureSPK(spriteType);

  return TRUE;
}

//-----------------------------------------------------------------------------
// Load CreatureType
//-----------------------------------------------------------------------------
// creature type을 보고 sprite를 load한다.
//-----------------------------------------------------------------------------
BOOL LoadCreatureType(int creatureType) {
  // Debug Message
  DEBUG_ADD_FORMAT("LoadCreatureType : %d", creatureType);

  // FAZ 4 (2026-05-05): SafeSpriteType0 — boş SpriteTypes guard
  int spriteType = SafeSpriteType0(creatureType);
  const bool auditCreature2221 = (Phoenix_CreatureDrawAudit_ShouldLog(creatureType) != 0);
  if (auditCreature2221) {
    Phoenix_CreatureDrawAudit_Log(
        "LoadCreatureType entry creatureType=%d creatureTableSize=%d spriteTableSize=%d spriteType=%d",
        creatureType,
        (g_pCreatureTable != NULL) ? (int)g_pCreatureTable->GetSize() : -1,
        (g_pCreatureSpriteTable != NULL) ? (int)g_pCreatureSpriteTable->GetSize() : -1,
        spriteType);
  }
  if (spriteType <= 0) {
    DEBUG_ADD_FORMAT("LoadCreatureType: empty SpriteTypes for creatureType=%d, skip",
                     creatureType);
    if (auditCreature2221) {
      Phoenix_CreatureDrawAudit_Log(
          "LoadCreatureType stop creatureType=%d reason=empty_or_invalid_sprite_type",
          creatureType);
    }
    return FALSE;
  }

  if (g_pCreatureSpriteTable == NULL ||
      spriteType >= (int)g_pCreatureSpriteTable->GetSize()) {
    DEBUG_ADD_FORMAT("LoadCreatureType: spriteType out of range creatureType=%d spriteType=%d",
                     creatureType, spriteType);
    if (auditCreature2221) {
      Phoenix_CreatureDrawAudit_Log(
          "LoadCreatureType stop creatureType=%d reason=sprite_table_oob spriteType=%d spriteTableSize=%d",
          creatureType,
          spriteType,
          (g_pCreatureSpriteTable != NULL) ? (int)g_pCreatureSpriteTable->GetSize() : -1);
    }
    return FALSE;
  }

  if (auditCreature2221) {
    const CREATURESPRITETABLE_INFO& spriteInfo = (*g_pCreatureSpriteTable)[spriteType];
    Phoenix_CreatureDrawAudit_Log(
        "LoadCreatureType state creatureType=%d spriteType=%d bLoad=%d frameID=%d first=%d last=%d shadowFirst=%d shadowLast=%d preparedFirst=%d preparedLast=%d",
        creatureType,
        spriteType,
        (int)spriteInfo.bLoad,
        (int)spriteInfo.FrameID,
        (int)spriteInfo.FirstSpriteID,
        (int)spriteInfo.LastSpriteID,
        (int)spriteInfo.FirstShadowSpriteID,
        (int)spriteInfo.LastShadowSpriteID,
        (g_pTopView != NULL) ? (int)g_pTopView->IsCreatureSpriteResourcePrepared(spriteInfo.FrameID, spriteInfo.FirstSpriteID) : -1,
        (g_pTopView != NULL) ? (int)g_pTopView->IsCreatureSpriteResourcePrepared(spriteInfo.FrameID, spriteInfo.LastSpriteID) : -1);
  }

  CREATURESPRITETABLE_INFO& spriteInfo = (*g_pCreatureSpriteTable)[spriteType];
  const bool normalSpriteRangeValid =
      spriteInfo.FirstSpriteID != 0xFFFF &&
      spriteInfo.LastSpriteID != 0xFFFF &&
      spriteInfo.FirstSpriteID <= spriteInfo.LastSpriteID;
  const bool normalSpriteRangePrepared =
      normalSpriteRangeValid && g_pTopView != NULL &&
      g_pTopView->IsCreatureSpriteResourcePrepared(spriteInfo.FrameID,
                                                   spriteInfo.FirstSpriteID) &&
      g_pTopView->IsCreatureSpriteResourcePrepared(spriteInfo.FrameID,
                                                   spriteInfo.LastSpriteID);

  if (spriteInfo.bLoad && normalSpriteRangeValid && !normalSpriteRangePrepared) {
    spriteInfo.bLoad = FALSE;

    if (auditCreature2221) {
      Phoenix_CreatureDrawAudit_Log(
          "LoadCreatureType retry creatureType=%d spriteType=%d reason=stale_bLoad frameID=%d first=%d last=%d",
          creatureType,
          spriteType,
          (int)spriteInfo.FrameID,
          (int)spriteInfo.FirstSpriteID,
          (int)spriteInfo.LastSpriteID);
    }
  }

  if (!(*g_pCreatureSpriteTable)[spriteType].bLoad) {
    // if (spriteType==9)
    //{
    //	int a = 9;
    // }

    LoadCreature(spriteType);

    if (auditCreature2221) {
      const CREATURESPRITETABLE_INFO& spriteInfo = (*g_pCreatureSpriteTable)[spriteType];
      Phoenix_CreatureDrawAudit_Log(
          "LoadCreatureType loaded creatureType=%d spriteType=%d bLoad=%d preparedFirst=%d preparedLast=%d",
          creatureType,
          spriteType,
          (int)spriteInfo.bLoad,
          (g_pTopView != NULL) ? (int)g_pTopView->IsCreatureSpriteResourcePrepared(spriteInfo.FrameID, spriteInfo.FirstSpriteID) : -1,
          (g_pTopView != NULL) ? (int)g_pTopView->IsCreatureSpriteResourcePrepared(spriteInfo.FrameID, spriteInfo.LastSpriteID) : -1);
    }

    return TRUE;
  }

  DEBUG_ADD_FORMAT("Don't need to LoadCreatureType : %d", creatureType);
  if (auditCreature2221) {
    const CREATURESPRITETABLE_INFO& spriteInfo = (*g_pCreatureSpriteTable)[spriteType];
    Phoenix_CreatureDrawAudit_Log(
        "LoadCreatureType skipped creatureType=%d reason=already_bLoad spriteType=%d preparedFirst=%d preparedLast=%d",
        creatureType,
        spriteType,
        (g_pTopView != NULL) ? (int)g_pTopView->IsCreatureSpriteResourcePrepared(spriteInfo.FrameID, spriteInfo.FirstSpriteID) : -1,
        (g_pTopView != NULL) ? (int)g_pTopView->IsCreatureSpriteResourcePrepared(spriteInfo.FrameID, spriteInfo.LastSpriteID) : -1);
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
// ReleaseUselessCreatureSPKExcept
//-----------------------------------------------------------------------------
void ReleaseUselessCreatureSPKExcept(const COrderedList<int> &listUse) {
  DEBUG_ADD("ReleaseUselessCreatureSPKExcept");

  g_pTopView->ReleaseUselessCreatureSPKExcept(listUse);
}

//-----------------------------------------------------------------------------
// Release GameObject
//-----------------------------------------------------------------------------
// game에 관련된 object들을 제거시킨다.
//-----------------------------------------------------------------------------
void ReleaseGameObject() {
  //------------------------------------------------------
  // 날씨 제거
  //------------------------------------------------------
  g_pWeather->Release();

  g_pParty->Release();

  g_pEventManager->RemoveAllEvent();
  g_pWarManager->ClearWar();
  g_pTimeItemManager->clear();

  //------------------------------------------------------
  // Zone에서 Player제거 / Zone 제거
  //------------------------------------------------------
  if (g_pZone != NULL) {
    DEBUG_ADD("[ SetZone NULL ] Remove Player & Set Zone to NULL");
    g_pZone->RemovePlayer();
    g_pZone->ReleaseObject();
    g_pZone = NULL;

    if (g_pZoneSoundTable) {
      delete g_pZoneSoundTable;
      g_pZoneSoundTable = NULL;
    }

    if (g_pZoneSoundManager) {
      delete g_pZoneSoundManager;
      g_pZoneSoundManager = NULL;
    }
  }

  //------------------------------------------------------
  // Small Zone 제거
  //------------------------------------------------------
  /*
  if (g_pZoneSmall != NULL)
  {
          DEBUG_ADD("[ delete SmallZone ]");
          delete g_pZoneSmall;
          g_pZoneSmall = NULL;
          g_nZoneSmall = ZONEID_NULL;
          g_bZoneSmallLoadImage = false;
  }
  */

  //------------------------------------------------------
  // Large Zone 제거
  //------------------------------------------------------
  /*
  if (g_pZoneLarge != NULL)
  {
          DEBUG_ADD("[ delete LargeZone ]");
          delete g_pZoneLarge;
          g_pZoneLarge = NULL;
          g_nZoneLarge = ZONEID_NULL;
          g_bZoneLargeLoadImage = false;
  }
  */

  //------------------------------------------------------
  // player제거
  //------------------------------------------------------
  if (g_pPlayer != NULL) {
    DEBUG_ADD("[ delete Player ]");

    if (g_pZone != NULL) {
      g_pZone->RemoveCreature(g_pPlayer->GetID());
    }

    delete g_pPlayer;
    g_pPlayer = NULL;
  }

  if (g_pTradeManager != NULL) {
    delete g_pTradeManager;
    g_pTradeManager = NULL;
  }

  if (g_pStorage != NULL) {
    delete g_pStorage;
    g_pStorage = NULL;
  }

  if (g_pStorage2 != NULL) {
    delete g_pStorage2;
    g_pStorage2 = NULL;
  }

  DEBUG_ADD("[ delete GameObject OK ]");
}

// Zone silindikten / Release sonrasi Large+Small tile/image SPK'larini yeni map'e gore dusur.
static void Phoenix_ReleaseZoneSPKAfterReclaim(const char *mapPath) {
  if (mapPath == NULL || mapPath[0] == '\0' || g_pTopView == NULL)
    return;

  ivfstream file;
  if (!FileOpenBinary(mapPath, file))
    return;

  FILEINFO_ZONE_HEADER header;
  header.LoadFromFile(file);
  if (header.ZoneVersion != MAP_VERSION_2000_05_10)
    return;

  long fpTile = 0;
  long fpImageObject = 0;
  file.read(reinterpret_cast<char *>(&fpTile), 4);
  file.read(reinterpret_cast<char *>(&fpImageObject), 4);
  if (fpTile <= 0)
    return;

  file.seekg(fpTile, std::ios::beg);
  g_pTopView->ReleaseTileSPKLargeZone(file);
  file.seekg(fpTile, std::ios::beg);
  g_pTopView->ReleaseTileSPKSmallZone(file);

  if (fpImageObject > 0) {
    file.seekg(fpImageObject, std::ios::beg);
    g_pTopView->ReleaseImageObjectSPKLargeZone(file);
    file.seekg(fpImageObject, std::ios::beg);
    g_pTopView->ReleaseImageObjectSPKSmallZone(file);
  }
}

static void Phoenix_ReclaimAllZoneCaches(MZone **pLoadZoneSlot) {
  if (g_pZoneLarge != NULL) {
    if (g_pZone == g_pZoneLarge)
      g_pZone = NULL;
    delete g_pZoneLarge;
    g_pZoneLarge = NULL;
    g_nZoneLarge = ZONEID_NULL;
    g_bZoneLargeLoadImage = false;
  }
  if (g_pZoneSmall != NULL) {
    if (g_pZone == g_pZoneSmall)
      g_pZone = NULL;
    delete g_pZoneSmall;
    g_pZoneSmall = NULL;
    g_nZoneSmall = ZONEID_NULL;
    g_bZoneSmallLoadImage = false;
  }
  if (pLoadZoneSlot != NULL)
    *pLoadZoneSlot = NULL;
}

static bool Phoenix_PeekMapSectorSize(const char *mapPath, WORD *outW,
                                      WORD *outH) {
  if (outW == NULL || outH == NULL || mapPath == NULL || mapPath[0] == '\0')
    return false;
  *outW = 0;
  *outH = 0;

  ivfstream file;
  if (!FileOpenBinary(mapPath, file))
    return false;

  FILEINFO_ZONE_HEADER header;
  header.LoadFromFile(file);
  if (header.ZoneVersion != MAP_VERSION_2000_05_10)
    return false;

  long fpTile = 0;
  long fpImageObject = 0;
  file.read(reinterpret_cast<char *>(&fpTile), 4);
  file.read(reinterpret_cast<char *>(&fpImageObject), 4);
  (void)fpTile;
  (void)fpImageObject;

  WORD w = 0;
  WORD h = 0;
  file.read(reinterpret_cast<char *>(&w), 2);
  file.read(reinterpret_cast<char *>(&h), 2);
  if (w == 0 || h == 0)
    return false;
  *outW = w;
  *outH = h;
  return true;
}

static unsigned long long Phoenix_EstimateSectorGridBytes(WORD w, WORD h) {
  const unsigned long long cells =
      static_cast<unsigned long long>(w) * static_cast<unsigned long long>(h);
  const unsigned long long sectorBytes =
      cells * static_cast<unsigned long long>(sizeof(MSector));
  const unsigned long long rowBytes =
      static_cast<unsigned long long>(h) * sizeof(MSector *);
  // STL constructor overhead / heap metadata payi
  return sectorBytes + rowBytes + (sectorBytes / 8ULL) + (2ULL * 1024ULL * 1024ULL);
}

static bool Phoenix_QueryProcessMem(unsigned long long *outWs,
                                    unsigned long long *outPf) {
  if (outWs != NULL)
    *outWs = 0;
  if (outPf != NULL)
    *outPf = 0;
  HMODULE psapi = GetModuleHandleA("psapi.dll");
  if (psapi == NULL)
    psapi = LoadLibraryA("psapi.dll");
  if (psapi == NULL)
    return false;
  typedef BOOL(WINAPI * GetProcessMemoryInfoFn)(HANDLE, PPROCESS_MEMORY_COUNTERS,
                                                DWORD);
  auto fn = reinterpret_cast<GetProcessMemoryInfoFn>(
      GetProcAddress(psapi, "GetProcessMemoryInfo"));
  if (fn == NULL)
    return false;
  PROCESS_MEMORY_COUNTERS pmc{};
  pmc.cb = sizeof(pmc);
  if (!fn(GetCurrentProcess(), &pmc, sizeof(pmc)))
    return false;
  if (outWs != NULL)
    *outWs = static_cast<unsigned long long>(pmc.WorkingSetSize);
  if (outPf != NULL)
    *outPf = static_cast<unsigned long long>(pmc.PagefileUsage);
  return true;
}

static void Phoenix_LogZoneMem(int zoneId, unsigned long long needBytes,
                               const char *tag) {
  MEMORYSTATUSEX ms{};
  ms.dwLength = sizeof(ms);
  GlobalMemoryStatusEx(&ms);

  unsigned long long ws = 0;
  unsigned long long pf = 0;
  Phoenix_QueryProcessMem(&ws, &pf);

  Phoenix_FatalDiagLogf(
      "mem %s zone=%d need=%llu availVirt=%llu totalVirt=%llu ws=%llu pf=%llu",
      tag != NULL ? tag : "?", zoneId, needBytes,
      static_cast<unsigned long long>(ms.ullAvailVirtual),
      static_cast<unsigned long long>(ms.ullTotalVirtual), ws, pf);
}

// need*1.25 yetmez — log'da need~3MB iken WS 1.7GB'a cikti, reclaim hic tetiklenmedi.
static bool Phoenix_IsVirtualBudgetTight(unsigned long long needBytes) {
  MEMORYSTATUSEX ms{};
  ms.dwLength = sizeof(ms);
  if (!GlobalMemoryStatusEx(&ms))
    return false;

  const unsigned long long kLowWater = 128ULL * 1024ULL * 1024ULL;   // 128 MB
  const unsigned long long kNeedFloor = 48ULL * 1024ULL * 1024ULL;  // min headroom
  const unsigned long long kWsHigh = 1200ULL * 1024ULL * 1024ULL;   // 1.2 GB

  unsigned long long need = needBytes;
  if (need < kNeedFloor)
    need = kNeedFloor;
  const unsigned long long budget = need + (need / 4ULL);

  unsigned long long ws = 0;
  Phoenix_QueryProcessMem(&ws, NULL);

  return ms.ullAvailVirtual < budget || ms.ullAvailVirtual < kLowWater ||
         ws >= kWsHigh;
}

static void Phoenix_BudgetReclaimForZoneLoad(const char *mapPath, int zoneId,
                                             unsigned long long needBytes) {
  Phoenix_LogZoneMem(zoneId, needBytes, "budget-check");
  if (!Phoenix_IsVirtualBudgetTight(needBytes)) {
    Phoenix_LogZoneMem(zoneId, needBytes, "budget-ok");
    return;
  }

  Phoenix_FatalDiagLogf(
      "LoadZone budget-low: purge map SPK+creature zoneID=%d need=%llu", zoneId,
      needBytes);
  if (g_pTopView != NULL)
    g_pTopView->PurgeCachedZoneMapSprites();
  else if (mapPath != NULL && mapPath[0] != '\0')
    Phoenix_ReleaseZoneSPKAfterReclaim(mapPath);
  COrderedList<int> emptyUse;
  ReleaseUselessCreatureSPKExcept(emptyUse);
  Phoenix_LogZoneMem(zoneId, needBytes, "budget-after-reclaim");
}

// GCUpdateInfo SkillManager::Init oncesi — sprite baskisini dusur.
// Zone grid SILINMEZ: contiguous MSector blogu kaybedilince fragmanli
// heap'te yeniden new → bad_alloc (10:04 log: emergency sonrasi availVirt~8MB).
void Phoenix_EmergencyMemoryReclaim(const char *optionalMapPath) {
  Phoenix_LogZoneMem(-1, 0, "emergency-reclaim-enter");

  // ImageObject / runtime object'leri dusur, sector grid'i tut.
  if (g_pZoneLarge != NULL)
    g_pZoneLarge->ReleaseKeepSectorGrid();
  if (g_pZoneSmall != NULL)
    g_pZoneSmall->ReleaseKeepSectorGrid();

  if (optionalMapPath != NULL && optionalMapPath[0] != '\0')
    Phoenix_ReleaseZoneSPKAfterReclaim(optionalMapPath);
  if (g_pTopView != NULL)
    g_pTopView->PurgeCachedZoneMapSprites();

  COrderedList<int> emptyUse;
  ReleaseUselessCreatureSPKExcept(emptyUse);
  Phoenix_LogZoneMem(-1, 0, "emergency-reclaim-done");
}

//-----------------------------------------------------------------------------
// Load Zone
//-----------------------------------------------------------------------------
// n번째 zone을 load한다.
//-----------------------------------------------------------------------------
BOOL LoadZone(int n) {
  CT_Done("[LoadZone] ENTER zoneID=%d", n);
  g_bLastLoadZoneWasOOM = false;
  //------------------------------------------------
  // Zone의 종류(size)에 따른 처	리
  //------------------------------------------------
  //
  // 같은 종류(size)에 따른 Zone에 Load를 해야한다.
  //
  //------------------------------------------------
  ZONETABLE_INFO *pZoneInfo = g_pZoneTable->Get(n);

  if (pZoneInfo == NULL) {
    DEBUG_ADD_FORMAT("[Error] Wrong Zone ID=%d", n);
    CT_Done("[LoadZone] FAIL pZoneInfo NULL zoneID=%d", n);

    return FALSE;
  }

  //------------------------------------------------
  // 모든 사운드를 정지해버린다.
  //------------------------------------------------
  if (g_pSoundManager != NULL) {
    g_pSoundManager->Stop();
    g_bPlayPropeller = FALSE;
  }
  g_DXSound.ReleaseDuplicateBuffer();

  // Debug Message
  DEBUG_ADD_FORMAT("LoadZone : ID=%d, Filename=%s", pZoneInfo->ID,
                   pZoneInfo->Filename.GetString());

  //------------------------------------------------
  // Music 정지...
  //------------------------------------------------
  // BOOL bMusicPause = g_DXMusic.IsPause();
  // g_DXMusic.Stop();

  // BOOL bMusicPause = g_Music.IsPause();

  if (g_pUserOption->PlayWaveMusic) {
#ifdef __USE_MP3__
    if (g_pMP3 != NULL) {
      DEBUG_ADD("MP3 STOP4");
      g_pMP3->Stop();
      DEBUG_ADD("MP3 STOP4 OK");
    }
#else
    //		if( g_pOGG != NULL )
    //		{
    if (g_DXSound.IsInit())
      BGMManager::Instance()->StopMusic();
//				g_pOGG->streamClose();
//		}
#endif
    //		if (g_pDXSoundStream!=NULL)
    //		{
    //			g_pDXSoundStream->Stop();
    //		}
    //		g_pDXSoundStream->Update();
  } else {
    g_Music.Stop();
  }

  //------------------------------------------------
  // Fade Out 적용
  //------------------------------------------------
  /*
if (g_pZone!=NULL && g_Mode==MODE_GAME )
{
  g_pTopView->SetFadeOut(6);

  while(g_pTopView->IsFade())
  {
          g_CurrentTime = timeGetTime();
          g_pUpdate->Update();
  }

  // 음악 중지
  //g_Music.Stop();
}
*/

  //------------------------------------------------
  // Fade In 설정
  //------------------------------------------------
  // g_pTopView->SetFadeIn(10);

  //------------------------------------------------
  // 아직 다른 Zone을 Loading중이라면
  //------------------------------------------------
  /*
  if  (WaitForSingleObject(g_hFileEvent, 0) == WAIT_OBJECT_0)
  {
          StopFileThread();
  }
  */
  // g_pLoadingThread->Remove( 1 );

  //-------------------------------------------------------------
  // Loading 중이던것 모두 제거
  //-------------------------------------------------------------
  //	StopLoadingThread();

  // priority를 최대한 낮춘다.
  // 	if (g_pLoadingThread!=NULL)
  // 	{
  // 		g_pLoadingThread->SetPriority( THREAD_PRIORITY_IDLE );
  // 	}

  //	MWorkThreadGlobal::Instance()->StopThread();
  //	MWorkThreadGlobal::Instance()->SetPriority(WORKTHREAD_PRIORITY_MAIN_LOADING);

  //----------------------------------------------------------------------
  // 현재 Zone의 object들 제거
  //----------------------------------------------------------------------
  if (g_pZone != NULL) {
    g_pZone->ReleaseObject();
  }

  bool bChangeMapShape = false;

  //----------------------------------------------------------------------
  // 이전 Zone과 현재 Zone이 같고 Shape List를 가지고 있다면 Index 증가
  //----------------------------------------------------------------------
  if (g_pZone != NULL) {
    if (g_pZone->GetID() == n) {
      int listSize = Phoenix_CountUsableMapShapes(pZoneInfo);

      if (listSize > 0) {
        g_nZoneShape = ++g_nZoneShape % listSize;

        bChangeMapShape = true;
      }
    } else {
      g_nZoneShape = 0;
    }
  }

  //----------------------------------------------------------------------
  // 맵 파일 이름
  //----------------------------------------------------------------------
  MString mapFileName = pZoneInfo->Filename;
  int usableMapShapeCount = Phoenix_CountUsableMapShapes(pZoneInfo);

  if (usableMapShapeCount > 0) {
    const MString* pShapeName = Phoenix_GetUsableMapShape(pZoneInfo, g_nZoneShape);

    if (pShapeName != NULL) {
      mapFileName = *pShapeName;
    }
  } else if (g_pUserInformation->GoreLevel == false) {
    if (pZoneInfo->TeenFilename.GetLength() > 0)
      mapFileName = pZoneInfo->TeenFilename;
  }

  if (!pZoneInfo->MapShapeList.empty() && usableMapShapeCount == 0) {
    Phoenix_ClientLifecycleLogf(
        "LoadZone ignored empty MapShapeList zoneID=%d name=%s rawShapeCount=%d primary=%s",
        n, Phoenix_SafeMString(pZoneInfo->Name),
        (int)pZoneInfo->MapShapeList.size(),
        Phoenix_SafeMString(pZoneInfo->Filename));
    DEBUG_ADD_FORMAT(
        "[Warn] LoadZone ignored empty MapShapeList. zoneID=%d name=%s rawShapeCount=%d primary=%s",
        n, Phoenix_SafeMString(pZoneInfo->Name),
        (int)pZoneInfo->MapShapeList.size(),
        Phoenix_SafeMString(pZoneInfo->Filename));
  }

  if (!Phoenix_HasText(mapFileName)) {
    Phoenix_ClientLifecycleLogf(
        "LoadZone invalid empty map filename zoneID=%d name=%s primary=%s rawShapeCount=%d usableShapeCount=%d",
        n, Phoenix_SafeMString(pZoneInfo->Name),
        Phoenix_SafeMString(pZoneInfo->Filename),
        (int)pZoneInfo->MapShapeList.size(), usableMapShapeCount);
    DEBUG_ADD_FORMAT(
        "[Error] LoadZone invalid empty map filename. zoneID=%d name=%s rawShapeCount=%d usableShapeCount=%d",
        n, Phoenix_SafeMString(pZoneInfo->Name),
        (int)pZoneInfo->MapShapeList.size(), usableMapShapeCount);
    SetMode(MODE_QUIT);
    return FALSE;
  }

  //----------------------------------------------------------------------
  // 바로 전에 있던 zone의 정보
  //----------------------------------------------------------------------
  ZONETABLE_INFO *pPreviousZoneInfo =
      (*g_pZoneTable).Get((g_bZonePlayerInLarge ? g_nZoneLarge : g_nZoneSmall));

  bool bLoadLargeZone = !(pZoneInfo->Property & FLAG_ZONESIZE_SMALL);

  MZone **pLoadZone = NULL;
  int *pLoadZoneNum = NULL;

  if (bLoadLargeZone) {
    DEBUG_ADD("Begin Load ZoneLarge");

    pLoadZone = &g_pZoneLarge;
    pLoadZoneNum = &g_nZoneLarge;
  } else {
    DEBUG_ADD("Begin Load ZoneSmall");

    pLoadZone = &g_pZoneSmall;
    pLoadZoneNum = &g_nZoneSmall;
  }

  // *붙이기 귀찮은 관계로다가
  MZone *&rLoadZone = *pLoadZone;
  int &rLoadZoneNum = *pLoadZoneNum;

  //----------------------------------------------------------------------
  //
  //					Load Zone
  //
  //----------------------------------------------------------------------

  //-------------------------------------------
  // 이전에 같은 크기의 존이 아니였다면
  // 그림자 다시 생성
  //-------------------------------------------
  if (g_bZonePlayerInLarge != bLoadLargeZone) {
    g_pTopView->ClearShadowManager();
  }

  //-------------------------------------------
  // 다시 load할 필요가 없는 경우
  //-------------------------------------------
  if (rLoadZoneNum == n && rLoadZone != NULL && !bChangeMapShape) {
    DEBUG_ADD("ReleaseObject in SmallZone");

    // object들 제거
    rLoadZone->ReleaseObject();

    DEBUG_ADD("OK");

    // 그림이 Load되지 않은 상태면..
    /*
    if (!g_bZoneSmallLoadImage)
    {
    // Thread가 할일을 지정
    g_ThreadJob = THREADJOB_LOAD_IMAGEOBJECT_SMALLZONE;
    SetEvent(g_hFileEvent);
    }
    */

    // SmallZone의 pointer를 대입
    g_pZone = rLoadZone;

    UI_DrawProgress(85);
  }
  //-------------------------------------------
  // 다시 Load해야될 경우
  //-------------------------------------------
  else {
    // Load할 Zone의 종류(size)를 기억


    DEBUG_ADD("[Load Zone] Release Old Tile&ImageObject SPK");

    /* chyaya delete
    // 이전 Zone에 있던 Sprite들을 지워준다.
    if(bLoadLargeZone)
    {
            g_pTopView->ReleaseTileSPKLargeZone();
            g_pTopView->ReleaseImageObjectSPKLargeZone();
    }
    else
    {
            g_pTopView->ReleaseTileSPKSmallZone();
            g_pTopView->ReleaseImageObjectSPKSmallZone();
    }
    //*/

    // Hızlı Large↔Small geçişte karşı boyut cache'i bellekte kalır;
    // peş peşe MSector grid alloc OOM (std::bad_alloc / MZone::Init).
    // Yeni zone yüklemeden önce aktif olmayan cache'i boşalt.
    if (bLoadLargeZone) {
      if (g_pZoneSmall != NULL) {
        DEBUG_ADD("Reclaim inactive ZoneSmall before reload");
        if (g_pZone == g_pZoneSmall) {
          g_pZone = NULL;
        }
        delete g_pZoneSmall;
        g_pZoneSmall = NULL;
        g_nZoneSmall = ZONEID_NULL;
        g_bZoneSmallLoadImage = false;
      }
    } else {
      if (g_pZoneLarge != NULL) {
        DEBUG_ADD("Reclaim inactive ZoneLarge before reload");
        if (g_pZone == g_pZoneLarge) {
          g_pZone = NULL;
        }
        delete g_pZoneLarge;
        g_pZoneLarge = NULL;
        g_nZoneLarge = ZONEID_NULL;
        g_bZoneLargeLoadImage = false;
      }
    }

    // Differential ReleaseTileSPK* benzer map'lerde (1311↔1321) eski near
    // sprite'lari biriktiriyor (~160MB/hop). Her gercek reload'da once
    // tum tile/image pixel'lerini dusur — sonra near set yeniden yuklenir.
    if (g_pTopView != NULL) {
      g_pTopView->PurgeCachedZoneMapSprites();
      Phoenix_FatalDiagLogf(
          "LoadZone pre-reload purge map sprites zoneID=%d", n);
    }

    // Zone initialize — ayni MZone* uzerinde LoadFromFile (sector reuse).
    // Budget dusukse once grid'i bosalt + SPK/creature reclaim (canli sector yok).
    WORD peekW = 0;
    WORD peekH = 0;
    unsigned long long needBytes = 0;
    if (Phoenix_PeekMapSectorSize(mapFileName.GetString(), &peekW, &peekH)) {
      needBytes = Phoenix_EstimateSectorGridBytes(peekW, peekH);
      Phoenix_FatalDiagLogf("LoadZone peek zoneID=%d size=%u x %u need=%llu", n,
                            (unsigned)peekW, (unsigned)peekH, needBytes);
    } else {
      Phoenix_FatalDiagLogf("LoadZone peek FAILED zoneID=%d file=%s", n,
                            mapFileName.GetString());
    }

    if (rLoadZone == NULL) {
      DEBUG_ADD("New Zone");
      rLoadZone = new MZone;
    } else {
      DEBUG_ADD("Reuse existing Zone pointer");
      // Budget tight olsa bile sector grid'i Release() etme — contiguous
      // blogu koru (fragmanli heap'te 3MB yeniden new bad_alloc).
      // Object/image temizligi LoadFromFile → ReleaseKeepSectorGrid.
      if (Phoenix_IsVirtualBudgetTight(needBytes != 0 ? needBytes : 1)) {
        Phoenix_BudgetReclaimForZoneLoad(mapFileName.GetString(), n, needBytes);
      }
    }

    g_pZone = rLoadZone;

    if (Phoenix_IsVirtualBudgetTight(needBytes != 0 ? needBytes : 1)) {
      Phoenix_BudgetReclaimForZoneLoad(mapFileName.GetString(), n, needBytes);
    } else if (needBytes != 0) {
      Phoenix_LogZoneMem(n, needBytes, "pre-load");
    }

    ivfstream file;
    if (!FileOpenBinary(mapFileName.GetString(), file)) {
      CT_Done("[LoadZone] FAIL FileOpenBinary mapFile zoneID=%d", n);
      DEBUG_ADD_FORMAT("[Error] LoadZone failed to open map file. zoneID=%d file=%s",
                       n, mapFileName.GetString());
      delete rLoadZone;
      rLoadZone = NULL;
      rLoadZoneNum = ZONEID_NULL;
      g_pZone = NULL;
      SetMode(MODE_QUIT);

      return FALSE;
    }

    DEBUG_ADD("[Load Zone] Load Zone Data");

    UI_DrawProgress(75);

    bool loadOk = false;
    bool hitBadAlloc = false;
    try {
      loadOk = (g_pZone->LoadFromFile(file) != FALSE);
    } catch (const std::bad_alloc &) {
      hitBadAlloc = true;
      loadOk = false;
      Phoenix_FatalDiagLogf(
          "LoadZone bad_alloc first try zoneID=%d", n);
      DEBUG_ADD_FORMAT(
          "[Error] LoadZone bad_alloc on first LoadFromFile zoneID=%d", n);
    }

    // OOM: stream kısmen tüketilmiş — sprite reclaim + AYNI MZone reuse ile dene.
    // Eski yol tum zone'lari delete edip new MZone yapiyordu → fragmanli
    // heap'te sector grid yeniden alloc bad_alloc (10:04 emergency sonrasi).
    if (hitBadAlloc) {
      file.close();

      Phoenix_FatalDiagLogf(
          "LoadZone OOM retry: keep zone grid + purge SPK+creature zoneID=%d",
          n);
      if (rLoadZone != NULL)
        rLoadZone->ReleaseKeepSectorGrid();
      if (g_pTopView != NULL)
        g_pTopView->PurgeCachedZoneMapSprites();
      else
        Phoenix_ReleaseZoneSPKAfterReclaim(mapFileName.GetString());
      {
        COrderedList<int> emptyUse;
        ReleaseUselessCreatureSPKExcept(emptyUse);
      }
      HeapCompact(GetProcessHeap(), 0);
#ifdef _MSC_VER
      _heapmin();
#endif
      if (needBytes != 0)
        Phoenix_LogZoneMem(n, needBytes, "oom-retry-after-reclaim");

      if (rLoadZone == NULL) {
        rLoadZone = new MZone;
        if (bLoadLargeZone) {
          g_pZoneLarge = rLoadZone;
          g_nZoneLarge = n;
        } else {
          g_pZoneSmall = rLoadZone;
          g_nZoneSmall = n;
        }
      }
      g_pZone = rLoadZone;

      if (!FileOpenBinary(mapFileName.GetString(), file)) {
        CT_Done("[LoadZone] FAIL FileOpenBinary retry after OOM zoneID=%d", n);
        DEBUG_ADD_FORMAT(
            "[Error] LoadZone reopen failed after bad_alloc. zoneID=%d file=%s",
            n, mapFileName.GetString());
        g_bLastLoadZoneWasOOM = true;
        Phoenix_FatalDiagLogf(
            "soft fail: LoadZone reopen after bad_alloc failed zoneID=%d -> MODE_QUIT",
            n);
        SetMode(MODE_QUIT);
        return FALSE;
      }

      try {
        loadOk = (g_pZone->LoadFromFile(file) != FALSE);
      } catch (const std::bad_alloc &) {
        loadOk = false;
        Phoenix_FatalDiagLogf(
            "LoadZone bad_alloc retry zoneID=%d", n);
        DEBUG_ADD_FORMAT(
            "[Error] LoadZone bad_alloc on retry LoadFromFile zoneID=%d", n);
      }
    }

    // Zone loading 실패?
    if (!loadOk) {
      Phoenix_FatalDiagLogf(
          "LoadZone LoadFromFile failed zoneID=%d hitBadAlloc=%d", n,
          hitBadAlloc ? 1 : 0);
      CT_Done("[LoadZone] FAIL LoadFromFile zoneID=%d hitBadAlloc=%d", n,
              hitBadAlloc ? 1 : 0);
      DEBUG_ADD_FORMAT("[Error] LoadZone failed to parse map file. zoneID=%d file=%s",
                       n, mapFileName.GetString());
      delete rLoadZone;
      rLoadZone = NULL;
      rLoadZoneNum = ZONEID_NULL;
      g_pZone = NULL;

      if (hitBadAlloc) {
        g_bLastLoadZoneWasOOM = true;
        Phoenix_FatalDiagLogf(
            "soft fail: LoadZone OOM zoneID=%d -> MODE_QUIT", n);
        SetMode(MODE_QUIT);
        return FALSE;
      }

      SetMode(MODE_QUIT);
      return FALSE;
    }

    rLoadZoneNum = n;
    g_pZone->SetCurrentZoneID(n);

    // g_pTopView->LoadFromFileTileSPKLargeZone( file );	// tile만 load
    // g_pTopView->LoadFromFileImageObjectSPKLargeZone( file );
    file.close();
    if (needBytes != 0)
      Phoenix_LogZoneMem(n, needBytes, "post-LoadFromFile");
  }

  //* chyaya add
  //-------------------------------------------------------
  // Tile & ImageObject SPK를 Thread로 로드한다.
  //-------------------------------------------------------
  ivfstream fileSPKPos;
  fileSPKPos.open(mapFileName.GetString(), std::ios::binary);

  fileSPKPos.seekg(g_pZone->GetTileFilePosition(), std::ios::beg);

  if (bLoadLargeZone) {
    g_pTopView->ReleaseTileSPKLargeZone(fileSPKPos);

    UI_DrawProgress(85);

    fileSPKPos.seekg(g_pZone->GetImageObjectFilePosition(), std::ios::beg);

    g_pTopView->ReleaseImageObjectSPKLargeZone(fileSPKPos);

    UI_DrawProgress(90);
  } else {
    g_pTopView->ReleaseTileSPKSmallZone(fileSPKPos);

    UI_DrawProgress(85);

    fileSPKPos.seekg(g_pZone->GetImageObjectFilePosition(), std::ios::beg);

    g_pTopView->ReleaseImageObjectSPKSmallZone(fileSPKPos);

    UI_DrawProgress(90);
  }

  fileSPKPos.close();
  //*/

  //-------------------------------------------------------
  // Player의 좌표 근처의 Sprite들을 Load한다.
  //-------------------------------------------------------
  DEBUG_ADD("[Load Zone] Find Tile&ImageObject ID");

  CSpriteSetManager NearTileSSM;
  CSpriteSetManager NearImageObjectSSM;

  // 플레이어의 시야의 3배까지는 Main Thread에서 로드한다.
  g_pZone->GetNearSpriteSet(NearTileSSM, NearImageObjectSSM, g_pPlayer->GetX(),
                            g_pPlayer->GetY());

  g_pTopView->LoadFromFileTileAndImageObjectSet(NearTileSSM,
                                                NearImageObjectSSM);

  Phoenix_LogZoneMem(n, 0, "post-LoadNearSprites");

  DEBUG_ADD("[Load Zone] Find Tile&ImageObject ID.. OK");

  // 	CSpriteSetManager NearOrderAllTileSSM;
  // 	CSpriteSetManager NearOrderAllImageObjectSSM;
  //
  // 	// 플레이어의 시야의 3배를 제외한 나머지 부분들을 가까운 순서대로
  // WorkThread로 로드한다. 	g_pZone->GetNearOrderAllSpriteSet(
  // 		NearOrderAllTileSSM, NearOrderAllImageObjectSSM,
  // 		g_pPlayer->GetX(), g_pPlayer->GetY(),
  // 		SECTOR_WIDTH*3/2, SECTOR_HEIGHT*3/2);
  //
  // 	g_pTopView->LoadFromFileTileAndImageObjectSetThread(NearOrderAllTileSSM,
  // NearOrderAllImageObjectSSM);

  g_bZonePlayerInLarge = bLoadLargeZone;

  // 	//----------------------------------------------------------------------
  // 	//
  // 	//					Small Zone
  // 	//
  // 	//----------------------------------------------------------------------
  // 	if(!bLoadLargeZone)
  // 	{
  // 		//ZONETABLE_INFO* pOldZoneInfo = (*g_pZoneTable).Get(
  // g_nZoneSmall );
  //
  // 		//-------------------------------------------
  // 		// 다시 load할 필요가 없는 경우
  // 		//-------------------------------------------
  // 		if (g_nZoneSmall==n && g_pZoneSmall!=NULL && !bChangeMapShape)
  // 		{
  // 			DEBUG_ADD("ReleaseObject in SmallZone");
  //
  // 			// object들 제거
  // 			g_pZoneSmall->ReleaseObject();
  //
  // 			DEBUG_ADD("OK");
  //
  // 			// 그림이 Load되지 않은 상태면..
  // 			/*
  // 			if (!g_bZoneSmallLoadImage)
  // 			{
  // 				// Thread가 할일을 지정
  // 				g_ThreadJob =
  // THREADJOB_LOAD_IMAGEOBJECT_SMALLZONE;
  // SetEvent(g_hFileEvent);
  // 			}
  // 			*/
  //
  // 			// SmallZone의 pointer를 대입
  // 			g_pZone	= g_pZoneSmall;
  //
  // 			UI_DrawProgress( 95 );
  // 		}
  // 		//-------------------------------------------
  // 		// 다시 Load해야될 경우
  // 		//-------------------------------------------
  // 		else
  // 		{
  // 			// Load할 Zone의 종류(size)를 기억
  // 			g_nZoneSmall = n;
  //
  // 			DEBUG_ADD("[Load Zone] Release Old Tile&ImageObject
  // SPK");
  //
  // 			// 이전 Zone에 있던 Sprite들을 지워준다.
  // 			g_pTopView->ReleaseTileSPKSmallZone();
  // 			g_pTopView->ReleaseImageObjectSPKSmallZone();
  //
  // 			// SmallZone을 초기화한다.
  // 			if (g_pZoneSmall != NULL)
  // 			{
  // 				DEBUG_ADD( "Delete old ZoneSmall");
  //
  // 				delete g_pZoneSmall;
  // 			}
  //
  // 			DEBUG_ADD( "New ZoneSmall");
  //
  // 			g_pZoneSmall = new MZone;
  //
  // 			// SmallZone의 pointer를 대입
  // 			g_pZone	= g_pZoneSmall;
  //
  // 			ivfstream file;
  // 			if (!FileOpenBinary(mapFileName, file))
  // 			{
  // 				// priority를 정상으로
  // 				if (g_pLoadingThread!=NULL)
  // 				{
  // 					g_pLoadingThread->SetPriority(
  // THREAD_PRIORITY_LOWEST );
  // 				}
  //
  // 				return FALSE;
  // 			}
  //
  // 			DEBUG_ADD("[Load Zone] Load Zone Data");
  //
  // 			UI_DrawProgress( 75 );
  //
  // 			// Zone loading 실패?
  // 			if (!g_pZone->LoadFromFile( file ))
  // 			{
  // 				SetMode( MODE_QUIT );
  //
  // 				// priority를 정상으로
  // 				if (g_pLoadingThread!=NULL)
  // 				{
  // 					g_pLoadingThread->SetPriority(
  // THREAD_PRIORITY_LOWEST );
  // 				}
  // 				return FALSE;
  // 			}
  //
  // 			g_pZone->SetCurrentZoneID( n );
  //
  // 			//g_pTopView->LoadFromFileTileSPKLargeZone( file );
  // // tile만 load
  // 			//g_pTopView->LoadFromFileImageObjectSPKLargeZone( file
  // ); 			file.close();
  //
  //
  //
  // 			UI_DrawProgress( 85 );
  //
  // 			//-------------------------------------------------------
  // 			// Player의 좌표 근처의 Sprite들을 Load한다.
  // 			//-------------------------------------------------------
  // 			DEBUG_ADD("[Load Zone] Find Tile&ImageObject ID");
  //
  // 			CSpriteSetManager TileSSM;
  // 			CSpriteSetManager ImageObjectSSM;
  // 			g_pZone->GetNearSpriteSet(TileSSM, ImageObjectSSM,
  // g_pPlayer->GetX(), g_pPlayer->GetY());
  // 			g_pTopView->LoadFromFileTileAndImageObjectSet(TileSSM,
  // ImageObjectSSM);
  //
  // 			DEBUG_ADD("[Load Zone] Find Tile&ImageObject ID.. OK");
  //
  // 			UI_DrawProgress( 95 );
  // 		}
  //
  // 		//-------------------------------------------------------
  // 		// LoadingThread가 할일을 지정
  // 		//-------------------------------------------------------
  // 		//g_ThreadJob = THREADJOB_LOAD_IMAGEOBJECT_SMALLZONE;
  // 		//SetEvent(g_hFileEvent);
  //
  // 		ivfstream file;
  // 		file.open(mapFileName, std::ios::binary);
  //
  // 		file.seekg(g_pZone->GetTileFilePosition(), std::ios::beg);
  // 		g_pTopView->LoadFromFileTileSPKSmallZone( file );
  //
  // 		file.seekg(g_pZone->GetImageObjectFilePosition(),
  // std::ios::beg); 		g_pTopView->LoadFromFileImageObjectSPKSmallZone(
  // file );
  //
  // 		file.close();
  //
  // 		BOOL NeedMusicLoad = TRUE;
  //
  // 		DEBUG_ADD("[Load Zone] Before Music Check");
  //
  // 		DEBUG_ADD("[Load Zone] Before Music Play");
  //
  // 		//------------------------------------------
  // 		// 음악 연주해야 되는 경우이면
  // 		//------------------------------------------
  // 		/*
  // 		if (g_pUserOption->PlayMusic)
  // 		{
  // 			//------------------------------------------
  // 			// 종족에 따라서 음악이 달라진다.
  // 			//------------------------------------------
  // 			TYPE_MUSICID	newMusicID = 0;
  //
  // 			if (g_pPlayer!=NULL && g_pPlayer->IsSlayer())
  // 			{
  // 				newMusicID = pZoneInfo->MusicIDSlayer;
  // 			}
  // 			else
  // 			{
  // 				newMusicID = pZoneInfo->MusicIDVampire;
  // 			}
  //
  // 			//g_DXMusic.SetOriginalTempo();
  // 			//g_DXMusic.Play( (*g_pMusicTable)[ newMusicID
  // ].Filename ); 			int musicID = newMusicID;
  //
  // 			if (musicID!=MUSICID_NULL)
  // 			{
  // 				if (g_pUserOption->PlayWaveMusic)
  // 				{
  // 					g_pDXSoundStream->Load(
  // (*g_pMusicTable)[ musicID ].FilenameWav );
  // g_pDXSoundStream->Play( TRUE );
  // 				}
  // 				else
  // 				{
  // 					g_Music.Play( (*g_pMusicTable)[ musicID
  // ].Filename );
  // 				}
  // 			}
  // 		}
  // 		*/
  //
  // 		/*
  // 		if (g_pMP3 != NULL)
  // 		{
  // 			g_pMP3->Pause();
  // 			delete g_pMP3;
  // 		}
  //
  // 		g_pMP3 = new CMySound( (*g_pMusicTable)[
  // (*g_pZoneTable)[g_nZoneSmall].MusicID ].Filename );
  // g_pMP3->Play();
  // 		*/
  //
  // 		g_bZonePlayerInLarge = false;
  // 	}
  //
  // 	//----------------------------------------------------------------------
  // 	//
  // 	//					Large Zone
  // 	//
  // 	//----------------------------------------------------------------------
  // 	else
  // 	{
  // 		//ZONETABLE_INFO* pOldZoneInfo = (*g_pZoneTable).Get(
  // g_nZoneLarge );
  //
  // 		//-----------------------------------------------
  // 		// 다시 Load해야할 필요가 없는 경우
  // 		//-----------------------------------------------
  // 		if (g_nZoneLarge==n && g_pZoneLarge!=NULL && !bChangeMapShape)
  // 		{
  // 			DEBUG_ADD("ReleaseObject in LargeZone");
  //
  // 			// object들 제거
  // 			g_pZoneLarge->ReleaseObject();
  //
  // 			DEBUG_ADD("OK");
  //
  // 			// 그림이 Load되지 않은 상태면..
  // 			/*
  // 			if (!g_bZoneLargeLoadImage)
  // 			{
  // 				// Thread가 할일을 지정
  // 				g_ThreadJob =
  // THREADJOB_LOAD_IMAGEOBJECT_LARGEZONE;
  // SetEvent(g_hFileEvent);
  // 			}
  // 			*/
  //
  // 			// LargeZone의 pointer를 대입
  // 			g_pZone	= g_pZoneLarge;
  //
  // 			UI_DrawProgress( 95 );
  // 		}
  // 		//-----------------------------------------------
  // 		// 다시 Load해야될 경우
  // 		//-----------------------------------------------
  // 		else
  // 		{
  // 			// Load할 Zone의 종류(size)를 기억
  // 			g_nZoneLarge = n;
  //
  // 			DEBUG_ADD("[Load Zone] Release Old Tile&ImageObject
  // SPK");
  //
  // 			// 이전 Zone에 있던 Sprite들을 지워준다.
  // 			g_pTopView->ReleaseTileSPKLargeZone();
  // 			g_pTopView->ReleaseImageObjectSPKLargeZone();
  //
  // 			// LargeZone을 초기화한다.
  // 			if (g_pZoneLarge != NULL)
  // 			{
  // 				DEBUG_ADD( "Delete old ZoneLarge");
  //
  // 				delete g_pZoneLarge;
  // 			}
  //
  // 			DEBUG_ADD( "New ZoneLarge");
  //
  // 			g_pZoneLarge = new MZone;
  //
  // 			// LargeZone의 pointer를 대입
  // 			g_pZone	= g_pZoneLarge;
  //
  // 			ivfstream file;
  // 			if (!FileOpenBinary(mapFileName, file))
  // 			{
  // 				// priority를 정상으로
  // 				if (g_pLoadingThread!=NULL)
  // 				{
  // 					g_pLoadingThread->SetPriority(
  // THREAD_PRIORITY_LOWEST );
  // 				}
  //
  // 				return FALSE;
  // 			}
  //
  // 			DEBUG_ADD("[Load Zone] Load Zone Data");
  //
  // 			UI_DrawProgress( 80 );
  //
  // 			// Zone loading 실패?
  // 			if (!g_pZone->LoadFromFile( file ))
  // 			{
  // 				SetMode( MODE_QUIT );
  //
  // 				// priority를 정상으로
  // 				if (g_pLoadingThread!=NULL)
  // 				{
  // 					g_pLoadingThread->SetPriority(
  // THREAD_PRIORITY_LOWEST );
  // 				}
  //
  // 				return FALSE;
  // 			}
  //
  // 			//g_pTopView->LoadFromFileTileSPKLargeZone( file );
  // // Tile만 load
  // 			//g_pTopView->LoadFromFileImageObjectSPKLargeZone( file
  // ); 			file.close();
  //
  // 			UI_DrawProgress( 90 );
  //
  // 			//-------------------------------------------------------
  // 			// Player의 좌표 근처의 Sprite들을 Load한다.
  // 			//-------------------------------------------------------
  // 			DEBUG_ADD("[Load Zone] Find Tile&ImageObject ID");
  //
  // 			CSpriteSetManager TileSSM;
  // 			CSpriteSetManager ImageObjectSSM;
  // 			g_pZone->GetNearSpriteSet(TileSSM, ImageObjectSSM,
  // g_pPlayer->GetX(), g_pPlayer->GetY());
  // 			g_pTopView->LoadFromFileTileAndImageObjectSet(TileSSM,
  // ImageObjectSSM);
  //
  // 			DEBUG_ADD("[Load Zone] Find Tile&ImageObject ID.. OK");
  //
  // 			UI_DrawProgress( 95 );
  // 		}
  //
  // 		//-------------------------------------------------------
  // 		// LoadingThread가 할일을 지정
  // 		//-------------------------------------------------------
  // 		//g_ThreadJob = THREADJOB_LOAD_IMAGEOBJECT_SMALLZONE;
  // 		//SetEvent(g_hFileEvent);
  //
  // 		ivfstream file;
  // 		file.open(mapFileName.GetString(), std::ios::binary);
  //
  // 		file.seekg(g_pZone->GetTileFilePosition(), std::ios::beg);
  // 		g_pTopView->LoadFromFileTileSPKLargeZone( file );
  //
  // 		file.seekg(g_pZone->GetImageObjectFilePosition(),
  // std::ios::beg); 		g_pTopView->LoadFromFileImageObjectSPKLargeZone(
  // file );
  //
  // 		file.close();
  //
  //
  // 		BOOL NeedMusicLoad = TRUE;
  //
  //
  // 		DEBUG_ADD("[Load Zone] Before Music Play");
  //
  // 		//------------------------------------------
  // 		// 음악 연주해야 되는 경우이면
  // 		//------------------------------------------
  // 		/*
  // 		if (g_pUserOption->PlayMusic)
  // 		{
  // 			//------------------------------------------
  // 			// 종족에 따라서 음악이 달라진다.
  // 			//------------------------------------------
  // 			//TYPE_MUSICID	oldMusicID = 0;
  // 			TYPE_MUSICID	newMusicID = 0;
  //
  // 			DEBUG_ADD("[Load Zone] Before Music Check");
  //
  // 			if (g_pPlayer!=NULL && g_pPlayer->IsSlayer())
  // 			{
  // 				newMusicID = pZoneInfo->MusicIDSlayer;
  // 			}
  // 			else
  // 			{
  // 				newMusicID = pZoneInfo->MusicIDVampire;
  // 			}
  //
  // 			//g_DXMusic.SetOriginalTempo();
  // 			//g_DXMusic.Play( (*g_pMusicTable)[ newMusicID
  // ].Filename );
  //
  // 			int musicID = newMusicID;
  //
  // 			if (musicID!=MUSICID_NULL)
  // 			{
  // 				if (g_pUserOption->PlayWaveMusic)
  // 				{
  // 					g_pDXSoundStream->Load(
  // (*g_pMusicTable)[ musicID ].FilenameWav );
  // g_pDXSoundStream->Play( TRUE );
  // 				}
  // 				else
  // 				{
  // 					g_Music.Play( (*g_pMusicTable)[ musicID
  // ].Filename );
  // 				}
  // 			}
  // 		}
  // 		*/
  //
  // 		/*
  // 		if (g_pMP3 != NULL)
  // 		{
  // 			g_pMP3->Pause();
  // 			delete g_pMP3;
  // 		}
  //
  // 		g_pMP3 = new CMySound;// (*g_pMusicTable)[
  // (*g_pZoneTable)[g_nZoneLarge].MusicID ].Filename );
  // 		//g_pMP3->Play();
  // 		g_pMP3->Load( (*g_pMusicTable)[
  // (*g_pZoneTable)[g_nZoneLarge].MusicID ].Filename );
  // 		*/
  //
  // 		g_bZonePlayerInLarge = true;
  // 	}

  DEBUG_ADD("[Load Zone] Before Minimap Loading");

  //------------------------------------------------
  // UI에 zone설정..
  //------------------------------------------------
  SIZE zoneSize = {g_pZone->GetWidth(), g_pZone->GetHeight()};

  gC_vs_ui.SetSize(zoneSize);
  gC_vs_ui.SetZone(n);

#ifdef OUTPUT_DEBUG
//	if(C_VS_UI_MINIMAP::GetZoneName(n))
//	{
//		extern int CLogFile;
//
//		_write( CLogFile, "<<<", 3 );
//		_write( CLogFile, C_VS_UI_MINIMAP::GetZoneName(n),
// strlen(C_VS_UI_MINIMAP::GetZoneName(n))); 		_write( CLogFile, ">>>",
// 3 ); 		_write( CLogFile, "\n", 1 );
//	}
#endif

  LoadZoneInfo(n);
  g_pZone->SetCurrentZoneID(n);

  // 2004, 6, 22 sobeit add start - 질드레 코어 관련
  Add_GDR_Ghost(n);
  Add_GDR_Effect(1, false);
  Add_GDR_Effect(2, false);
  Add_GDR_Potal_Effect(n);
  // 2005, 1, 18, sobeit add start-퀘스트 땜에 - bDisableTileImage==false가
  // 대부분이므로..
  if (n == 4001) {
    for (int i = 0; i < g_pZone->GetHeight(); i++) {
      for (int j = 0; j < g_pZone->GetWidth(); j++) {
        const_cast<MSector &>(g_pZone->GetSector(i, j)).SetDisableTileImage();
      }
    }
  }
  // 2005, 1, 18, sobeit add end
  // 2004, 9, 21, sobeit add start - 아담 북 구름
  else if (n == 74) {
    if (g_pPlayer->GetY() < 60)
      g_pPlayer->SetShowAdamCloud(true);
    else
      g_pPlayer->SetShowAdamCloud(false);
  }
  // 2004, 9, 21, sobeit add end
  //	BYTE PkType = g_pZone->GetPKType();
  //	if(PkType != PK_TYPE_NULL)	// 그냥 일반 맵
  //	{
  //		if(PkType == PK_TYPE_DISABLE)		// 다 우리편..-_-; pk
  // 금지 			g_pPlayer->SetAttackModePeace();
  // else if(PkType == PK_TYPE_ALL)
  //// 나 빼고 다 적 			g_pPlayer->SetAttackModeAggress();
  ///else if(PkType ==
  // PK_TYPE_GUILD)	// 우리 길드 빼고 다 적..
  //		{
  //			int myGuildID = g_pPlayer->GetGuildNumber();
  //			int OtherGuildID = pCreature->GetGuildNumber();
  //			if( myGuildID		== GUILDID_VAMPIRE_DEFAULT ||
  //				myGuildID		==
  // GUILDID_SLAYER_DEFAULT || 				myGuildID
  // == GUILDID_OUSTERS_DEFAULT || 				OtherGuildID	== GUILDID_VAMPIRE_DEFAULT ||
  //				OtherGuildID	== GUILDID_SLAYER_DEFAULT ||
  //				OtherGuildID	== GUILDID_OUSTERS_DEFAULT ) //
  // 길드가 없는 녀석 들은.. 				return TRUE; // 혼자
  // 싸워라..-_-;
  //
  //			if(myGuildID == OtherGuildID)
  //				return FALSE; // 우리 길드...
  //			else
  //				return TRUE;
  //		}
  //	}
  //	else
  //		g_pPlayer->SetAttackModeNormal();

  // 2004, 6, 22 sobeit add end
  //------------------------------------------------
  // minimap 그림 설정
  //------------------------------------------------
  // g_pTopView->LoadMinimap( pZoneInfo->MinimapFilename );

  //------------------------------------------------
  // Random SoundID 시간 설정
  //------------------------------------------------
  g_ZoneRandomSoundTime =
      g_CurrentTime + ((rand() % 5) + 10) * 1000; // 10~15초후..

  //---------------------------------------------------------------
  // 하나의 소리에 대한 정보
  //---------------------------------------------------------------
  /*
  ZONESOUND_INFO* pSoundInfo	= new ZONESOUND_INFO;
  pSoundInfo->ID				= 1;
  pSoundInfo->SoundID			= SOUND_WORLD_WATER_DROP;
  pSoundInfo->Loop			= true;
  pSoundInfo->MinDelay		= 0;
  pSoundInfo->MaxDelay		= 24;
  pSoundInfo->StartHour		= 0;
  pSoundInfo->EndHour			= 24;

  g_pZoneSoundTable->AddData( pSoundInfo->ID, pSoundInfo );
  */

  //*/

  //------------------------------------------------
  // 이 Zone에서 등장하는 Creature에 대한 Sprite들
  //------------------------------------------------
  // g_pTopView->LoadFromFileCreatureSPK( 0 );
  // g_pTopView->LoadFromFileCreatureSPK( 1 );

  //------------------------------------------------
  // View에게 출력해야할 Zone을 지정한다.
  //------------------------------------------------
  // 이전에 출력된 creature들을 없앤다.
  g_pTopView->SetZone(g_pZone);

  // Debug Message
  DEBUG_ADD_FORMAT("LoadZone OK : size=(%d, %d)", g_pZone->GetWidth(),
                   g_pZone->GetHeight());

  // priority를 정상으로
  // 	if (g_pLoadingThread!=NULL)
  // 	{
  // 		g_pLoadingThread->SetPriority( THREAD_PRIORITY_LOWEST );
  // 	}

  //	MWorkThreadGlobal::Instance()->SetPriority(WORKTHREAD_PRIORITY_GAME_PLAYING);

  if (pZoneInfo->CreatureColorSet < MAX_COLORSET) {
    g_ZoneCreatureColorSet = pZoneInfo->CreatureColorSet;
  } else {
    g_ZoneCreatureColorSet = 0xFFFF;
  }

  g_bZoneSafe = pZoneInfo->Safety;
  g_bHolyLand = pZoneInfo->HolyLand;

  //	if( g_pWarManager->IsExist( pZoneInfo->ID ) )
  //	{
  //		g_bZoneSafe = false;
  //	}

  g_pWarManager->ClearWar();
  g_pTimeItemManager->clear();

  g_pTopView->SetBlindEffect(false);

  g_pUserInformation->WorldName = g_pServerInformation->GetServerGroupName();
#ifdef __DEBUG_OUTPUT__
  for (int w = 0; w < g_pZone->GetWidth(); w++) {
    for (int h = 0; h < g_pZone->GetHeight(); h++) {
      if (g_pZone->GetSector(w, h).IsBlockAny() && g_pZone->GetID() == 3001) {
        gC_vs_ui.SetBlock(w, h);
      }
    }
  }
#endif

#ifndef __METROTECH_TEST__
  if (gpC_base != NULL && g_pUserInformation->bCompetence == true &&
      g_pUserInformation->bCompetenceShape != true &&
      g_pZoneTable->Get(g_pZone->GetID())->CompetenceZone == false) {
    gpC_base->SendMessage(UI_CHAT_RETURN, CLD_NORMAL, 0, "*command ghost on");
  }
#endif

  CT_Done("[LoadZone] SUCCESS zoneID=%d g_pZone=%p", n, (void*)g_pZone);
  return TRUE;
}

//-----------------------------------------------------------------------------
// Move Zone
//-----------------------------------------------------------------------------
// Zone을 이동한다.
//-----------------------------------------------------------------------------
BOOL MoveZone(int n) {
  CT_Done("[MoveZone] ENTER zoneID=%d g_pZone(pre)=%p", n, (void*)g_pZone);
  if (g_pZone != NULL) {
    g_pPlayer->SetStop();
    g_pPlayer->SetAction(ACTION_STAND);
    g_pZone->RemovePlayer();
  }

  if (!LoadZone(n)) {
    CT_Done("[MoveZone] FAIL LoadZone zoneID=%d g_pZone(post)=%p", n,
            (void*)g_pZone);
    DEBUG_ADD_FORMAT("[Error] MoveZone failed. zoneID=%d", n);
    return FALSE;
  }
  CT_Done("[MoveZone] AFTER LoadZone g_pZone(post)=%p", (void*)g_pZone);

  if (g_pZone == NULL) {
    CT_Done("[MoveZone] FAIL g_pZone NULL after LoadZone zoneID=%d", n);
    DEBUG_ADD_FORMAT("[Error] MoveZone has no loaded zone. zoneID=%d", n);
    SetMode(MODE_QUIT);
    return FALSE;
  }

  g_pPlayer->SetZone(g_pZone);
  g_pZone->SetPlayer();
  g_pPlayer->SetTimeLightSight(g_pPlayer->GetTimeLightSight());
  CT_Done("[MoveZone] EXIT g_pZone(final)=%p", (void*)g_pZone);
  return TRUE;
}

//-----------------------------------------------------------------------------
// LoadZoneInfo
//-----------------------------------------------------------------------------
BOOL LoadZoneInfo(int n) {
  //------------------------------------------------
  // ZoneInfo
  //------------------------------------------------
  ZONETABLE_INFO *pZoneInfo = g_pZoneTable->Get(n);

  if (pZoneInfo == NULL) {
    DEBUG_ADD_FORMAT("[Error] Wrong Zone ID=%d", n);

    return FALSE;
  }

  //------------------------------------------------
  //
  // ZoneInfoFile Loading
  //
  //------------------------------------------------
  DEBUG_ADD("[Load Zone] Before Open ZoneInfoFile");

  //------------------------------------------------
  // ZoneInfo
  //------------------------------------------------
  //	MZoneInfo zoneInfo;
  //------------------------------------------------
  // File이 있는 경우만 loading한다.
  //------------------------------------------------
  ivfstream zoneInfoFile(pZoneInfo->InfoFilename.GetString(), std::ios::binary);

  // if (!access(pZoneInfo->InfoFilename.GetString(), 0))
  if (zoneInfoFile.is_open()) {
    DEBUG_ADD("[Load Zone] Load MinimapInfo");

    /*
    zoneInfo.LoadFromFile( zoneInfoFile );

    //------------------------------------------------
    // minimap에 안전지대를 표시한다.
    //------------------------------------------------
    g_pTopView->LoadMinimap( pZoneInfo->MinimapFilename, &zoneInfo );
    */
    WORD width, height;

    zoneInfoFile.read((char *)&width, 2);
    zoneInfoFile.read((char *)&height, 2);

    //------------------------------------------------
    // size 체크
    //------------------------------------------------
    if (width == g_pZone->GetWidth() && height == g_pZone->GetHeight()) {
      DEBUG_ADD("[Load Zone] Load PortalInfo");

      //------------------------------------------------
      // 포탈 정보를 loading한다.
      // 내부에서 해주는게 좋지만... 일단..
      //------------------------------------------------
      int numPortal;

      zoneInfoFile.read((char *)&numPortal, 4);

      DEBUG_ADD_FORMAT("numPortal = %d", numPortal);

      MPortal portal;
      RECT rect;

      int i;
      for (i = 0; i < numPortal; i++) {
        portal.LoadFromFile(zoneInfoFile);

        std::vector<WORD> &zoneID = portal.GetZoneID();

        int numZoneID = zoneID.size();

        DEBUG_ADD_FORMAT("[%d] numZoneID = %d", i, numZoneID);

        for (int i = 0; i < numZoneID; i++) {
          rect.left = portal.GetLeft();
          rect.top = portal.GetTop();
          rect.right = portal.GetRight();
          rect.bottom = portal.GetBottom();

          DEBUG_ADD_FORMAT("AddPortal. type=%d, zoneID=%d", portal.GetType(),
                           zoneID[i]);

          g_pZone->AddPortal(portal.GetType(), zoneID[i], rect);
        }

        //------------------------------------------------
        // UI에 설정한다.
        //------------------------------------------------
        if (numZoneID == 1) {
          DEBUG_ADD("UI SetPortal");

          gC_vs_ui.SetPortal(rect, zoneID[0]);
        } else {
          DEBUG_ADD("UI SetPortal - 60001");

          // 멀티 포탈인 경우.. ㅋㅋ.. _-_;
          gC_vs_ui.SetPortal(rect, 60001);
        }
      }

      DEBUG_ADD("PortalInfo OK");

      //------------------------------------------------
      // 안전지대 정보
      //------------------------------------------------
      int numSafe;

      zoneInfoFile.read((char *)&numSafe, 4);

      DEBUG_ADD_FORMAT("numSafe = %d", numSafe);

      B_RECT rect2;

      DEBUG_ADD_FORMAT("[g_pPlayer] %x", g_pPlayer);
      // BYTE bSlayer = (g_pPlayer==NULL || g_pPlayer->IsSlayer());
      Race MyRace = g_pPlayer->GetRace();
      DEBUG_ADD("bSlayer OK");

      for (i = 0; i < numSafe; i++) {
        zoneInfoFile.read((char *)&rect2, SIZE_B_RECT);

        RECT safeRect = {rect2.left, rect2.top, rect2.right, rect2.bottom};

        DEBUG_ADD_FORMAT("UI SetSafety. f=%d, (%d, %d, %d, %d)", rect2.flag,
                         safeRect.left, safeRect.top, safeRect.right,
                         safeRect.bottom);

        if (rect2.flag != 0) {
          BYTE fSafe = 0;

          if (rect2.flag & FLAG_MIP_SAFE_COMMON) {
            fSafe = FLAG_SECTOR_SAFE_COMMON;
          }

          if (rect2.flag & FLAG_MIP_SAFE_SLAYER) {
            fSafe |= FLAG_SECTOR_SAFE_SLAYER;
          }

          if (rect2.flag & FLAG_MIP_SAFE_VAMPIRE) {
            fSafe |= FLAG_SECTOR_SAFE_VAMPIRE;
          }
          if (rect2.flag & FLAG_MIP_SAFE_OUSTERS) {
            fSafe |= FLAG_SECTOR_SAFE_OUSTERS;
          }

          g_pZone->SetSafeSector(safeRect, fSafe);

          // 내가 안전한 위치인가?

          if (MyRace == RACE_OUSTERS && (rect2.flag & FLAG_MIP_SAFE_OUSTERS) ||
              MyRace == RACE_VAMPIRE && (rect2.flag & FLAG_MIP_SAFE_VAMPIRE) ||
              MyRace == RACE_SLAYER && (rect2.flag & FLAG_MIP_SAFE_SLAYER) ||
              (rect2.flag & FLAG_MIP_SAFE_COMMON)) {
            gC_vs_ui.SetSafetyZone(safeRect, true);
          }
          // 다른 종족이 안전한 위치인가?
          else {
            gC_vs_ui.SetSafetyZone(safeRect, false);
          }
        }
      }
      DEBUG_ADD("NUMSAFE OK");

      DEBUG_ADD("zoneinfoclose");
      zoneInfoFile.close();
      DEBUG_ADD("zoneinfoclose OK");

      DEBUG_ADD("Horn Setting Start");
      // FAZ 4 forensic: tablo durumu + horn vektoru
      CT_Done("ForensicHorn: ENTER ptr=%p size=%d hornCount=%d zoneID=%d",
                 (void*)g_pCreatureTable,
                 g_pCreatureTable ? (int)g_pCreatureTable->GetSize() : -1,
                 g_pZone ? (int)g_pZone->GetHorn().size() : -1,
                 g_pZone ? (int)g_pZone->GetID() : -1);

      // ousters horn을 맵에 심는다
      UI_PORTAL_LIST portalList;

      for (i = 0; i < g_pZone->GetHorn().size(); i++) {
        portalList = g_pZone->GetHorn()[i];
        CT_Done("ForensicHorn: outerLoop i=%d portalListSize=%d",
                   (int)i, (int)portalList.size());

        UI_PORTAL_LIST::iterator itr = portalList.begin();
        UI_PORTAL_LIST::iterator endItr = portalList.end();

        while (itr != endItr) {
          if (itr->zone_id == g_pZone->GetID()) {
            DEBUG_ADD_FORMAT(
                "zoneID : %d, x : %d, y : %d, portalX : %d, portalY : %d",
                itr->zone_id, itr->x, itr->y, itr->portal_x, itr->portal_y);
            int npcID = 659;

            MNPC *pNPC = new MNPC;
            CT_Done("ForensicHorn: post-new MNPC pNPC=%p npcID=%d", (void*)pNPC, npcID);

            pNPC->SetZone(g_pZone);

            int zoneID = (g_bZonePlayerInLarge ? g_nZoneLarge : g_nZoneSmall);

            int creatureType = npcID;
            CT_Done("ForensicHorn: pre-SpriteTypes idx=%d size=%d",
                       creatureType,
                       g_pCreatureTable ? (int)g_pCreatureTable->GetSize() : -1);
            // FAZ 4: 659 entry'sinde SpriteTypes alt-tablosu boş olabiliyor
            // (creature.inf'te bu NPC için sprite veri eksik). Convention
            // MCreature.cpp:1117'deki frame-count koruması ile aynı: alt-tablo
            // boşsa NPC'yi oluşturmuyoruz, aksi halde SpriteTypes[0] erişimi
            // operator[] içinde NULL deref ile crash eder.
            int sprListSize = (*g_pCreatureTable)[creatureType].SpriteTypes.GetSize();
            if (sprListSize == 0) {
                CT_Done("ForensicHorn: SKIP idx=%d - SpriteTypes empty (creature.inf data missing)", creatureType);
                delete pNPC;
                pNPC = NULL;
                itr++;
                continue;
            }
            int spriteType = SafeSpriteType0(creatureType);
            CT_Done("ForensicHorn: post-SpriteTypes spriteType=%d", spriteType);

            pNPC->SetGuildNumber(GUILDID_OUSTERS);

            // pNPC->SetCreatureType( 0 );
            pNPC->SetCreatureType(npcID);
            pNPC->SetGroundCreature();
            pNPC->SetID(MFakeCreature::GetFakeID());
            // pNPC->SetAction(ACTION_MOVE);
            pNPC->SetPosition(itr->portal_x, itr->portal_y);
            pNPC->SetServerPosition(itr->portal_x, itr->portal_y);
            pNPC->SetDirection(1);
            pNPC->SetCurrentDirection(1);
            pNPC->SetAction(ACTION_STAND);

            CT_Done("ForensicHorn: pre-Name idx=%d", npcID);
            pNPC->SetName((*g_pCreatureTable)[npcID].Name.GetString());
            CT_Done("ForensicHorn: post-Name");

            // NPC ID
            pNPC->SetNPCID(npcID);

            //
            pNPC->SetStatus(MODIFY_MAX_HP, 100);
            pNPC->SetStatus(MODIFY_CURRENT_HP, 100);

            CT_Done("ForensicHorn: pre-AddCreature");
            if (!g_pZone->AddCreature(pNPC)) {
              delete pNPC;
              pNPC = NULL;
            } else if (g_pPlayer->IsOusters()) {
              const char *pName = pNPC->GetName();
              int x = pNPC->GetX();
              ;
              int y = pNPC->GetY();

              UI_SetNPCInfo(pName, npcID, x, y);
            }
            CT_Done("ForensicHorn: post-AddCreature");
            DEBUG_ADD("AddNPC OK");
          }
          itr++;
        }
      }
      CT_Done("ForensicHorn: EXIT loop done");
      DEBUG_ADD("Horn Setting End");

      if (g_pZone->GetID() == 1311) {
        SetWeather(WEATHER_SPOT, 2);
      }
      /*
                              //---------------------------------------------------------------
                              //
                              //  Zone의 환경 사운드 설정
                              //
                              //---------------------------------------------------------------
                              long fpCurrent = zoneInfoFile.tellg();
                              zoneInfoFile.seekg( 0, ios::end );
                              long fpEof = zoneInfoFile.tellg();

                              if (!zoneInfoFile.eof()	// 왜 이게 제대로
         안되지- -;
                                      && fpCurrent!=fpEof)
                              {
                                      zoneInfoFile.seekg( fpCurrent, ios::beg );

                                      if (g_pZoneSoundTable)
                                      {
                                              DEBUG_ADD("Delete
         ZoneSoundTable");

                                              delete g_pZoneSoundTable;
                                      }

                                      if (g_pZoneSoundManager)
                                      {
                                              DEBUG_ADD("Delete
         ZoneSoundManager");

                                              delete g_pZoneSoundManager;
                                      }

                                      DEBUG_ADD("New ZoneSoundTable");

                                      // Zone의 사운드를 어떻게 낼까?에 대한
         정보 g_pZoneSoundTable = new MZoneSoundTable;

                                      DEBUG_ADD("Delete ZoneSoundManager");

                                      // Zone에 있는 사운드 자체에 대한 정보
                                      g_pZoneSoundManager = new
         MZoneSoundManager;


                                      DEBUG_ADD("Load SectorSound");

                                      //---------------------------------------------------------------
                                      // Sector에 사운드 정보를 Loading한다.
                                      //---------------------------------------------------------------
                                      g_pZone->LoadFromFileSectorSound(
         zoneInfoFile );

                                      //---------------------------------------------------------------
                                      // ZoneSoundTable에
                                      // 현재 Map에 대한 사운드 정보를
         Loading한다.
                                      //---------------------------------------------------------------
                                      DEBUG_ADD("Load ZoneSoundTable");

                                      g_pZoneSoundTable->LoadFromFile(
         zoneInfoFile );

                                      DEBUG_ADD("Load ZoneSoundTable OK");
                              }
                              else
                              {
                                      DEBUG_ADD("No ZoneSound Info");
                              }
      */
    } else {
      DEBUG_ADD_FORMAT("[Error] ZoneInfo Size Mismatch: (%d,%d) != (%d,%d)",
                       g_pZone->GetWidth(), g_pZone->GetHeight(), width,
                       height);
    }

    DEBUG_ADD("[Load Zone] Load ZoneInfo OK");
  } else {
    DEBUG_ADD("[Load Zone] ZoneInfo File OpenError");
  }

  DEBUG_ADD("[Load Zone] OK");

  return TRUE;
}

//-----------------------------------------------------------------------------
// Load World Map Info
// FAZ 4 (2026-05-05): #if 0 guard kaldırıldı, real impl aktive.
// Stub `int LoadWorldMapInfo() { return 1; }` ClientMissingStubs'tan kaldırıldı,
// link error vermesin diye burada gerçek tanım. Referans: GameMain.cpp:3422.
//-----------------------------------------------------------------------------
BOOL
LoadWorldMapInfo()
{
	const CZoneTable::ZONEINFO_MAP& rZoneMap = g_pZoneTable->GetZoneMap();

	CZoneTable::ZONEINFO_MAP::const_iterator pos	= rZoneMap.begin();
	CZoneTable::ZONEINFO_MAP::const_iterator endpos	= rZoneMap.end();

	for(; pos != endpos; ++pos)
	{
		//------------------------------------------------
		// ZoneInfo
		//------------------------------------------------
		ZONETABLE_INFO* pZoneInfo = pos->second;	

		// 월드맵에서 보이지 않는 다면 스킵
		if(!pZoneInfo->IsShownInWorldMap()) continue;

		if (pZoneInfo==NULL)
		{
			DEBUG_ADD_FORMAT("[Error] Wrong Zone");	

			return FALSE;
		}

		//------------------------------------------------
		//
		// ZoneInfoFile Loading
		//
		//------------------------------------------------
		DEBUG_ADD("[Load Zone] Before Open ZoneInfoFile");
		
		//------------------------------------------------
		// ZoneInfo
		//------------------------------------------------
		// MZoneInfo zoneInfo;
		WORLDMAP_UI_INFO& rWorldMapUIInfo = C_VS_UI_WORLD_MAP::GetWorldMapUIInfo(pZoneInfo->ID);

		//------------------------------------------------
		// File이 있는 경우만 loading한다.
		//------------------------------------------------
		ivfstream zoneInfoFile(pZoneInfo->InfoFilename.GetString(), std::ios::binary);

		//if (!access(pZoneInfo->InfoFilename.GetString(), 0))
		if(zoneInfoFile.is_open())
		{
			DEBUG_ADD("[Load Zone] Load MinimapInfo");
			
			WORD width, height;
			zoneInfoFile.read((char*)&width, 2);
			zoneInfoFile.read((char*)&height, 2);

			rWorldMapUIInfo.mapSize.cx = width;
			rWorldMapUIInfo.mapSize.cy = height;

			DEBUG_ADD("[Load Zone] Load PortalInfo");
				
			//------------------------------------------------
			// 포탈 정보를 loading한다.
			// 내부에서 해주는게 좋지만... 일단..
			//------------------------------------------------
			int numPortal;
			
			zoneInfoFile.read((char*)&numPortal, 4);
			
			DEBUG_ADD_FORMAT("numPortal = %d", numPortal);
			
			MPortal portal;
			
			for (int i = 0; i < numPortal; ++i)
			{			
				portal.LoadFromFile(zoneInfoFile);
				
				std::vector<WORD>& zoneID = portal.GetZoneID();
				
				int numZoneID = zoneID.size();
				
				DEBUG_ADD_FORMAT("[%d] numZoneID = %d", i, numZoneID);
				
				CRect rect;
				rect.left	= portal.GetLeft();
				rect.top	= portal.GetTop();
				rect.right	= portal.GetRight(); 
				rect.bottom = portal.GetBottom();

				const int skipEdge = 2;

				// 화면 끝부분의 포탈정보는 출력하지 않는다.
				if(rect.left >= skipEdge && rect.top >= skipEdge &&
				   rect.right <= width - skipEdge - 1 && rect.bottom <= height - skipEdge - 1)
				{			
					//------------------------------------------------
					// UI에 설정한다.
					//------------------------------------------------
					if (numZoneID==1)	// 걍 포탈
					{					
						DEBUG_ADD("UI SetPortal");
						
						rWorldMapUIInfo.SetPortal( rect, zoneID[0] );
					}
					else				// 멀티 포탈
					{
						DEBUG_ADD("UI SetPortal - 60001");
						rWorldMapUIInfo.SetPortal( rect, 60001 );
					}
				}

				DEBUG_ADD("PortalInfo OK");

				/*&
				//------------------------------------------------
				// 안전지대 정보 - Skip
				//------------------------------------------------
				int numSafe;
				
				zoneInfoFile.read((char*)&numSafe, 4);
				zoneInfoFile.seekg(SIZE_B_RECT * numSafe);	// rect2;
				DEBUG_ADD("SafeZone Skip");
				*/
			}
			
			DEBUG_ADD("[Load Zone] Load ZoneInfo OK");
		}
		else
		{		
			DEBUG_ADD("[Load Zone] ZoneInfo File OpenError");
		}
		
		DEBUG_ADD("[Load Zone] OK");

	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// Init Player
//-----------------------------------------------------------------------------
BOOL InitPlayer(int x, int y, int dir) {
  CT_Done("IP: ENTER x=%d y=%d dir=%d g_pPlayer=%p g_pTopView=%p g_pZone=%p",
             x, y, dir, (void*)g_pPlayer, (void*)g_pTopView, (void*)g_pZone);
  // Debug Message
  DEBUG_ADD_FORMAT("InitPlayer : (%d,%d) Dir=%d", x, y, dir);

  //------------------------------------------------
  // Creature의 종류
  //------------------------------------------------
  // g_pPlayer->SetCreatureType( 0 );

  // g_pPlayer->SetCreatureType( id%2 );
  // g_pPlayer->SetCreatureType( 1 );

  CT_Done("IP: pre ClearChatString");
  g_pPlayer->ClearChatString();
  CT_Done("IP: post ClearChatString, pre TopView->SetFadeEnd");

  //------------------------------------------------
  // 살아있게 한다. 확인용...
  //------------------------------------------------
  g_pTopView->SetFadeEnd();
  CT_Done("IP: post TopView->SetFadeEnd, pre Player setters");

  //------------------------------------------------
  // 위치 설정
  //------------------------------------------------
  // g_pPlayer->SetPosition(x, y);
  g_pPlayer->SetStop();
  CT_Done("IP: post SetStop");
  g_pPlayer->SetX(x);
  g_pPlayer->SetY(y);
  g_pPlayer->SetServerPosition(x, y);
  CT_Done("IP: post SetX/Y/ServerPosition");
  g_pPlayer->SetDirection(dir);
  g_pPlayer->SetCurrentDirection(dir);
  CT_Done("IP: post SetDirection/CurrentDirection");
  // g_pPlayer->SetAction( ACTION_MOVE );	// Default

  //------------------------------------------------
  //  Zone <--> Player
  //------------------------------------------------
  CT_Done("IP: pre g_pZone->SetPlayer");
  g_pZone->SetPlayer(); //&g_pPlayer->;
  CT_Done("IP: post g_pZone->SetPlayer, pre Player->SetZone");
  g_pPlayer->SetZone(g_pZone);
  CT_Done("IP: post Player->SetZone");

  //------------------------------------------------
  // 시야 확인..
  //------------------------------------------------
  // #ifdef CONNECT_SERVER
  //	g_pZone->KeepObjectInSight(x, y, g_pPlayer->GetSight());
  // #endif

  //------------------------------------------------
  // 상태값
  //------------------------------------------------
  // g_pPlayer->SetStatus(MODIFY_HP, 100);
  // g_pPlayer->SetStatus(MODIFY_MP, 100);

  //------------------------------------------------
  // 옷 입히기
  //------------------------------------------------
  ///*
  // g_pPlayer->SetAddonEnable(ADDON_HAIR);		// 머리
  // g_pPlayer->SetAddonEnable(ADDON_COAT);		// 상의
  // g_pPlayer->SetAddonEnable(ADDON_TROUSER);	// 하의
  // g_pPlayer->SetAddonEnable(ADDON_HELM);		// 모자
  // g_pPlayer->SetAddonEnable(ADDON_LEFTHAND);	// 왼손
  // g_pPlayer->SetAddonEnable(ADDON_RIGHTHAND);	// 오른손
  // g_pPlayer->SetAddonEnable(ADDON_MOTOR);		// 오토바이

  // g_pPlayer->SetAddon(ADDON_COAT, ADDONID_COAT1_MALE);
  // g_pPlayer->SetAddon(ADDON_TROUSER, ADDONID_TROUSER1_MALE);
  ////g_pPlayer->SetAddon(ADDON_HELM, 9);
  ////g_pPlayer->SetAddon(ADDON_RIGHTHAND, 7);
  // g_pPlayer->SetAddon(ADDON_HAIR, ADDONID_HAIR1_MALE);
  // g_pPlayer->RemoveAddon(ADDON_MOTOR);//, ADDONID_MOTORCYCLE_MALE);
  //*/

  //------------------------------------------------
  // 시야 설정
  //------------------------------------------------
  // g_pPlayer->SetLightSight( 12 );	// 빛의 크기
  // g_pPlayer->SetSight( 12 );		// 캐릭터의 시야

  //------------------------------------------------
  // 공격, 기술 관련
  //------------------------------------------------
  // 공격
  // g_pPlayer->SetBasicActionInfo( SKILL_ATTACK_MELEE );

  // 공격 가능거리 설정
  // g_pPlayer->SetBasicAttackDistance( 1 );

  // 특수 공격
  // g_pPlayer->SetSpecialActionInfo( MAGIC_BLESS );

  // 정지 동작
  CT_Done("IP: pre IsAlive (virtual)");
  bool bAlive = g_pPlayer->IsAlive();
  CT_Done("IP: post IsAlive=%d, pre SetAction", (int)bAlive);
  if (bAlive) {
    g_pPlayer->SetAction(ACTION_STAND);
  } else {
    g_pPlayer->SetAction(ACTION_DIE);
  }
  CT_Done("IP: post SetAction");

  // 움직이는 방법
  // g_pPlayer->SetMoveDevice( MCreature::MOVE_DEVICE_WALK );

  //-------------------------------------------------------
  // UI에 상태 적용
  //-------------------------------------------------------
  // gC_vs_ui.SetHP(g_pPlayer->GetHP(), g_pPlayer->GetMAX_HP());
  // gC_vs_ui.SetMP(g_pPlayer->GetMP(), g_pPlayer->GetMAX_MP());

  //-------------------------------------------------------
  // Server로 보낸 move packet 개수를 지우고
  // 처음부터~~ 다시 move하게 한다.
  //-------------------------------------------------------
  CT_Done("IP: pre ResetSendMove");
  g_pPlayer->ResetSendMove();
  CT_Done("IP: post ResetSendMove");

  //-------------------------------------------------------
  // Attack Mode
  //-------------------------------------------------------
  // g_pPlayer->SetAttackModeNormal();

  //-------------------------------------------------------
  //
  //				사용할 수 있는 기술
  //
  //-------------------------------------------------------
  /*
  (*g_pSkillManager).Init( MAX_SKILLDOMAIN );

  //---------------------------------------------------------------------
  // 기본 기술로부터 skill tree를 초기화한다.
  //---------------------------------------------------------------------
  (*g_pSkillManager)[SKILLDOMAIN_BLADE].SetRootSkill( SKILL_SINGLE_BLOW );
  (*g_pSkillManager)[SKILLDOMAIN_SWORD].SetRootSkill( SKILL_DOUBLE_IMPACT );
  (*g_pSkillManager)[SKILLDOMAIN_GUN].SetRootSkill( SKILL_SNIPPING );
  (*g_pSkillManager)[SKILLDOMAIN_GUN].SetRootSkill( SKILL_SHARP_SHOOTING );
  (*g_pSkillManager)[SKILLDOMAIN_ENCHANT].SetRootSkill( MAGIC_CREATE_HOLY_WATER
  );
  (*g_pSkillManager)[SKILLDOMAIN_HEAL].SetRootSkill( MAGIC_CURE_POISON );
  (*g_pSkillManager)[SKILLDOMAIN_VAMPIRE].SetRootSkill( MAGIC_HIDE );

  MSkillDomain& bladeDomain = (*g_pSkillManager)[SKILLDOMAIN_BLADE];
  MSkillDomain& swordDomain = (*g_pSkillManager)[SKILLDOMAIN_SWORD];
  MSkillDomain& gunDomain = (*g_pSkillManager)[SKILLDOMAIN_GUN];
  MSkillDomain& enchantDomain = (*g_pSkillManager)[SKILLDOMAIN_ENCHANT];
  MSkillDomain& healDomain = (*g_pSkillManager)[SKILLDOMAIN_HEAL];
  */

  //---------------------------------------------------------------------
  // 몇가지 skill을 배웠다고 표시한다.
  //---------------------------------------------------------------------
  /*
  swordDomain.LearnSkill( SKILL_DOUBLE_IMPACT );
  swordDomain.LearnSkill( SKILL_TRIPLE_SLASHER );
  swordDomain.LearnSkill( SKILL_SCREW_SLASHER );
  swordDomain.LearnSkill( SKILL_RAINBOW_SLASHER );
  swordDomain.LearnSkill( SKILL_CRUSH_COMBO );
  swordDomain.LearnSkill( SKILL_CROSS_COUNTER );

  bladeDomain.LearnSkill( SKILL_SINGLE_BLOW );
  bladeDomain.LearnSkill( SKILL_TWIN_BREAK );
  bladeDomain.LearnSkill( SKILL_TRIPLE_BREAK );
  bladeDomain.LearnSkill( SKILL_SPIRAL_SLAY );
  bladeDomain.LearnSkill( SKILL_WILD_SMASH );
  bladeDomain.LearnSkill( SKILL_ARMOR_CRASH );

  gunDomain.LearnSkill( SKILL_SNIPPING );
  gunDomain.LearnSkill( SKILL_SHARP_SHOOTING );
  gunDomain.LearnSkill( SKILL_FAST_RELOAD );
  gunDomain.LearnSkill( SKILL_KNOCKS_TARGET_BACK );

  enchantDomain.LearnSkill( MAGIC_LIGHT );
  enchantDomain.LearnSkill( MAGIC_DETECT_HIDDEN );
  enchantDomain.LearnSkill( MAGIC_CONTINUAL_LIGHT );
  enchantDomain.LearnSkill( MAGIC_DETECT_INVISIBILITY );
  enchantDomain.LearnSkill( MAGIC_VISIBLE );

  healDomain.LearnSkill( MAGIC_CURE_POISON );
  healDomain.LearnSkill( MAGIC_PROTECTION_FROM_CURSE );
  healDomain.LearnSkill( MAGIC_CURE_LIGHT_WOUNDS );
  healDomain.LearnSkill( MAGIC_CURE_POISON );
  healDomain.LearnSkill( MAGIC_PROTECTION_FROM_POISON );
  healDomain.LearnSkill( MAGIC_CURE_PARALYSIS );

  */
  // g_SkillAvailable.AddSkill( MAGIC_CURE_POISON );
  // g_SkillAvailable.AddSkill( MAGIC_PROTECTION_FROM_POISON );
  // g_SkillAvailable.AddSkill( MAGIC_CURE_PARALYSIS );

  //-------------------------------------------------------
  // 기술 검증 상태..
  //-------------------------------------------------------
  CT_Done("IP: pre SetWaitVerifyNULL");
  g_pPlayer->SetWaitVerifyNULL();
  CT_Done("IP: post SetWaitVerifyNULL");

  //-------------------------------------------------------
  //
  //						Item 관련
  //
  //-------------------------------------------------------
  //------------------------------------------------
  // ItemCheckBuffer상태
  //------------------------------------------------
  CT_Done("IP: pre ClearItemCheckBuffer");
  g_pPlayer->ClearItemCheckBuffer();
  CT_Done("IP: post ClearItemCheckBuffer");

  //------------------------------------------------
  // Gear 초기화
  //------------------------------------------------
  /*
  if (g_pPlayer->IsSlayer())
  {
          g_SlayerGear.Init();
  }
  else
  {
          g_VampireGear.Init();
  }

  //------------------------------------------------
  //  Inventory 초기화
  //------------------------------------------------
  g_Inventory.Init(10,6);
  */

  //------------------------------------------------
  // 정보창에 보여주는 정보..
  //------------------------------------------------
  CT_Done("IP: pre GetRace/IsMale");
  g_char_slot_ingame.Race = g_pPlayer->GetRace();
  g_char_slot_ingame.bl_female = !g_pPlayer->IsMale();
  CT_Done("IP: EXIT");

  return TRUE;
}

//-----------------------------------------------------------------------------
// Make ScreenShot
//-----------------------------------------------------------------------------
// 현재 화면의 ScreenShot을 저장한다.
//-----------------------------------------------------------------------------
void MakeScreenShot() {
  // FAZ 4 (2026-05-06): screenshot crash teşhis + NULL guard'lar.
  CT_Done("SCRSHOT: ENTER g_pClientConfig=%p g_pFileDef=%p g_pBack=%p g_ScreenShotNumber=%d",
             (void*)g_pClientConfig, (void*)g_pFileDef, (void*)g_pBack, g_ScreenShotNumber);

  if (g_pClientConfig == NULL || g_pFileDef == NULL || g_pBack == NULL) {
    CT_Done("SCRSHOT: SAFE-RETURN (NULL global)");
    return;
  }

  char str[256];

  int maxScreenShot = g_pClientConfig->MAX_SCREENSHOT;
  CT_Done("SCRSHOT: maxScreenShot=%d", maxScreenShot);

#ifdef OUTPUT_DEBUG
  maxScreenShot = 1000;
#endif

  _mkdir("ScreenShot");
  // MAX_SCREENSHOT개의 ScreenCapture만 가능하다.
  for (; g_ScreenShotNumber < maxScreenShot; g_ScreenShotNumber++) {
    // snprintf — sprintf format'ı "%s%03d.jpg" güvenli ama sz_buf 256 byte,
    // path uzun olursa overflow. snprintf güvence.
    snprintf(str, sizeof(str), "%s%03d.jpg",
             g_pFileDef->getProperty("PATH_SCREENSHOT").c_str(),
             g_ScreenShotNumber);
    CT_Done("SCRSHOT: try path=%s", str);

    int fd = _open(str, _O_RDONLY);

    // file이 없는 경우에 save하기 위해서..
    if (fd == -1) {
// Save
#ifdef OUTPUT_DEBUG
      if (g_bSaveSlideScreenShot &&
          g_rectScreenShot.left != g_rectScreenShot.right &&
          g_rectScreenShot.top != g_rectScreenShot.bottom) {
        if (g_rectScreenShot.left > g_rectScreenShot.right) {
          int temp = g_rectScreenShot.left;
          g_rectScreenShot.left = g_rectScreenShot.right;
          g_rectScreenShot.right = temp;
        }
        if (g_rectScreenShot.top > g_rectScreenShot.bottom) {
          int temp = g_rectScreenShot.top;
          g_rectScreenShot.top = g_rectScreenShot.bottom;
          g_rectScreenShot.bottom = temp;
        }

        int width = g_rectScreenShot.right - g_rectScreenShot.left + 8;
        int height = g_rectScreenShot.bottom - g_rectScreenShot.top + 8;

        width &= ~0x07;
        height &= ~0x07;

        CSpriteSurface surface;
        surface.InitOffsurface(width, height, DDSCAPS_SYSTEMMEMORY);

        POINT point0 = {0, 0};
        RECT rect = g_rectScreenShot;
        rect.left += 1;
        rect.top += 1;
        surface.BltNoColorkey(&point0, g_pBack, &rect);

        surface.SaveToBMP(str);
      } else {
#endif
        //			g_pBack->SaveToBMP(str);
        // Bug 13 (2026-05-11): SaveSurfaceToImage cagrisina ek diagnostic. JPG
        // path Lock surface NULL/format mismatch durumlarinda artik bool
        // donuyor (UtilityFunction.cpp guard'lari); fail durumunda log basilir.
        CT_Done("SCRSHOT: pre SaveSurfaceToImage path=%s", str);
        const bool ssi_ok = SaveSurfaceToImage(str, *g_pBack);
        CT_Done("SCRSHOT: post SaveSurfaceToImage ok=%d", ssi_ok ? 1 : 0);
#ifdef OUTPUT_DEBUG
      }
#endif

      // 2026-05-15: _close(fd) BURADA KALDIRILDI. fd == -1 (dosya zaten açılmadı,
      // screenshot save path'indeyiz). Modern UCRT (VS2022) _close(-1) icin
      // _invalid_parameter_handler invoke eder -> process abort. VS6 legacy CRT'de
      // no-op idi, yorum sahibi (yanlis varsayim) zarar yok demis. Screenshot
      // crash kaynagi. Else branch'inde (fd != -1) _close(fd) kalir.
      g_ScreenShotNumber++;
      g_lGameRunBreakTime = timeGetTime() - g_CurrentTime;
      return;
    } else {
      _close(fd);
    }
  }
#ifdef OUTPUT_DEBUG
  g_bSaveSlideScreenShot = false;
#endif
}

//-----------------------------------------------------------------------------
// PlaySound
//-----------------------------------------------------------------------------
// (*g_pSoundTable)에서의 soundID와 관련되는 Filename을 Play하면 된다.
// (*g_pSoundManager)에 있으면 바로 play하면 되고..
// 없으면 Wav를 Load해서 play하면 된다.
//-----------------------------------------------------------------------------
void PlaySound(TYPE_SOUNDID soundID, bool repeat, int x, int y) {
  if (g_pEventManager->GetEventByFlag(EVENTFLAG_NOT_PLAY_SOUND))
    return;

  __BEGIN_PROFILE("PlaySound1")

  // static int reload	= 0;
  // static int reuse	= 0;

  //-----------------------------------------------------------
  // 정의되지 않는 sound ID일 경우..
  //-----------------------------------------------------------
  if (!g_DXSound.IsInit() || soundID >= (*g_pSoundTable).GetSize() ||
      !g_pUserOption->PlaySound) {
    __END_PROFILE("PlaySound1")
    return;
  }

  //-----------------------------------------------------------
  // 초당 소리낼 수 있는 개수 제한
  //-----------------------------------------------------------
  if (!repeat && g_SoundPerSecond > g_pClientConfig->MAX_SOUND_PER_SECOND) {
    __END_PROFILE("PlaySound1")
    return;
  }

  int gapX = x - g_pPlayer->GetX();
  int gapY = y - g_pPlayer->GetY();

  int dist = max(abs(gapX), abs(gapY));

  // 거리가 어느 정도 이상이면 소리를 출력하지 않아야 한다.
  if (dist < 25) {
    //-----------------------------------------------------------
    // 없으면 --> Load & Play
    //-----------------------------------------------------------
    if ((*g_pSoundManager).IsDataNULL(soundID)) {
      const char *pFilename = (*g_pSoundTable)[soundID].Filename;

      if (pFilename == NULL) {
        __END_PROFILE("PlaySound1")
        return;
      }

      char strFilename[256];
      strcpy(strFilename, pFilename);

      LPDIRECTSOUNDBUFFER pBuffer = g_DXSound.LoadWav(strFilename);
      // LPDIRECTSOUNDBUFFER	pBuffer =
      // g_pWavePackFileManager->LoadFromFileData(soundID);

      //-----------------------------------------------------------
      // Loading 실패
      //-----------------------------------------------------------
      if (pBuffer == NULL) {
        (*g_pSoundTable)[soundID].Filename.Release();
        DEBUG_ADD_FORMAT("[Error] Failed to Load WAV. id=%d, fn=%s", soundID,
                         (*g_pSoundTable)[soundID].Filename);
      } else
      //-----------------------------------------------------------
      // Load에 성공 했으면...
      //-----------------------------------------------------------
      {
        // Replace됐으면 원래것을 메모리에서 지운다.
        LPDIRECTSOUNDBUFFER pOld;
        if ((*g_pSoundManager).SetData(soundID, pBuffer, pOld) != 0xFFFF) {
          g_DXSound.Release(pOld);
        }

        int gapX = x - g_pPlayer->GetX();
        int gapY = y - g_pPlayer->GetY();

        int dist = max(abs(gapX), abs(gapY));

        if (gapX > 3) {
          g_DXSound.CenterToRightPan(pBuffer, (gapX - 3) << 7);
        } else if (gapX < -3) {
          g_DXSound.CenterToLeftPan(pBuffer, (abs(gapX + 3)) << 7);
        } else {
          g_DXSound.CenterPan(pBuffer);
        }

        // 오토바이 소리 좀 줄이기 하드코딩. - -;
        if (soundID == SOUND_WORLD_BIKE_GO ||
            soundID == SOUND_WORLD_BIKE_STOP) {
          dist += 5;
        }

        int sub = (dist << 2) * CDirectSound::SliderValueToLegacyAttenuation(g_pUserOption->VolumeSound);
        // int sub = dist * g_pUserOption->VolumeSound;

        g_DXSound.SubVolumeFromMax(pBuffer, sub);

        // Play
        g_DXSound.Play(pBuffer, repeat, g_bGoodFPS);
        g_SoundPerSecond++;

        // Force Feel
        //-------------------------------------------------
        // if (gpC_Imm!=NULL && gpC_Imm->IsDevice())
        {
          //	gpC_Imm->ForceSound( strFilename );
        }

        // reload++;
      }
    }
    //-----------------------------------------------------------
    // 있는 경우 --> Play
    //-----------------------------------------------------------
    else {
      LPDIRECTSOUNDBUFFER pBuffer;
      if ((*g_pSoundManager).GetData(soundID, pBuffer)) {

        if (gapX > 3) {
          g_DXSound.CenterToRightPan(pBuffer, (gapX - 3) << 7);
        } else if (gapX < -3) {
          g_DXSound.CenterToLeftPan(pBuffer, (abs(gapX + 3)) << 7);
        } else {
          g_DXSound.CenterPan(pBuffer);
        }

        // 오토바이 소리 좀 줄이기 하드코딩. - -;
        if (soundID == SOUND_WORLD_BIKE_GO ||
            soundID == SOUND_WORLD_BIKE_STOP) {
          dist += 5;
        }

        int sub = (dist << 2) * CDirectSound::SliderValueToLegacyAttenuation(g_pUserOption->VolumeSound);
        // int sub = dist * g_pUserOption->VolumeSound;

        g_DXSound.SubVolumeFromMax(pBuffer, sub);

        g_DXSound.Play(pBuffer, repeat, g_bGoodFPS);
        g_SoundPerSecond++;

        // Force Feel
        //-------------------------------------------------
        /*
        if (gpC_Imm!=NULL && gpC_Imm->IsDevice())
        {
                const char* pFilename = (*g_pSoundTable)[soundID].Filename;

                if (pFilename!=NULL)
                {
                        char strFilename[256];
                        strcpy(strFilename, pFilename);

                        gpC_Imm->ForceSound( strFilename );
                }
        }
        */
      }

      // reuse ++;
    }
  }

  __END_PROFILE("PlaySound1")

  // #ifdef OUTPUT_DEBUG
  //	DEBUG_ADD_FORMAT("miss(%d) : match(%d)  / %d", reload, reuse,
  // reload+reuse); #endif
}

//-----------------------------------------------------------------------------
// PlaySound
//-----------------------------------------------------------------------------
// 소리나는 위치가 늘 player의 위치..
// CenterPan & MaxVolume
//-----------------------------------------------------------------------------
void PlaySound(TYPE_SOUNDID soundID) {
  if (g_pEventManager->GetEventByFlag(EVENTFLAG_NOT_PLAY_SOUND))
    return;

  __BEGIN_PROFILE("PlaySound2")

  //-----------------------------------------------------------
  // 정의되지 않는 sound ID일 경우..
  //-----------------------------------------------------------
  if (!g_DXSound.IsInit() || soundID >= (*g_pSoundTable).GetSize() ||
      !g_pUserOption->PlaySound) {
    __END_PROFILE("PlaySound2")
    return;
  }

  //-----------------------------------------------------------
  // 초당 소리낼 수 있는 개수 제한
  //-----------------------------------------------------------
  // player의 소리는 제한할 필요가 없다.
  // if (g_SoundPerSecond > g_pClientConfig->MAX_SOUND_PER_SECOND)
  //{
  //	__END_PROFILE("PlaySound2")
  // return;
  //}

  //-----------------------------------------------------------
  // 없으면 --> Load & Play
  //-----------------------------------------------------------
  if ((*g_pSoundManager).IsDataNULL(soundID)) {
    const char *pFilename = (*g_pSoundTable)[soundID].Filename;

    if (pFilename == NULL) {
      __END_PROFILE("PlaySound2")
      return;
    }

    char strFilename[256];
    strcpy(strFilename, pFilename);
    LPDIRECTSOUNDBUFFER pBuffer = g_DXSound.LoadWav(strFilename);
    // LPDIRECTSOUNDBUFFER	pBuffer =
    // g_pWavePackFileManager->LoadFromFileData(soundID);

    //-----------------------------------------------------------
    // Loading 실패
    //-----------------------------------------------------------
    if (pBuffer == NULL) {
      (*g_pSoundTable)[soundID].Filename.Release();
      DEBUG_ADD_FORMAT("[Error] Failed to Load WAV. id=%d, fn=%s", soundID,
                       (*g_pSoundTable)[soundID].Filename);
    }
    //-----------------------------------------------------------
    // Load에 성공 했으면...
    //-----------------------------------------------------------
    else {
      // Replace됐으면 원래것을 메모리에서 지운다.
      LPDIRECTSOUNDBUFFER pOld;
      if ((*g_pSoundManager).SetData(soundID, pBuffer, pOld) != 0xFFFF) {
        g_DXSound.Release(pOld);
      }

      g_DXSound.CenterPan(pBuffer);

      g_DXSound.SetMaxVolume(pBuffer);

      // Play
      g_DXSound.Play(pBuffer, false, g_bGoodFPS);

      g_SoundPerSecond++;

      //-------------------------------------------------
      // Force Feel
      //-------------------------------------------------
      // if (gpC_Imm!=NULL && gpC_Imm->IsDevice())
      {
        //	gpC_Imm->ForceSound( pFilename );
      }

      // reload++;
    }
  }
  //-----------------------------------------------------------
  // 있는 경우 --> Play
  //-----------------------------------------------------------
  else {
    LPDIRECTSOUNDBUFFER pBuffer;
    if ((*g_pSoundManager).GetData(soundID, pBuffer)) {
      g_DXSound.CenterPan(pBuffer);

      g_DXSound.SetMaxVolume(pBuffer);

      g_DXSound.Play(pBuffer, false, g_bGoodFPS);
      g_SoundPerSecond++;
    }

    // Force Feel
    //-------------------------------------------------
    /*
    const char* pFilename = (*g_pSoundTable)[soundID].Filename;

    if (pFilename!=NULL)
    {
            char strFilename[256];
            strcpy(strFilename, pFilename);
            if (gpC_Imm!=NULL && gpC_Imm->IsDevice())
            {
                    gpC_Imm->ForceSound( strFilename );
            }
    }
    */

    // reuse ++;
  }

  __END_PROFILE("PlaySound2")
}

void PlaySoundForce(TYPE_SOUNDID soundID) {
  __BEGIN_PROFILE("PlaySound2")

  //-----------------------------------------------------------
  // 정의되지 않는 sound ID일 경우..
  //-----------------------------------------------------------
  if (!g_DXSound.IsInit() || soundID >= (*g_pSoundTable).GetSize()) {
    __END_PROFILE("PlaySound2")
    return;
  }

  //-----------------------------------------------------------
  // 초당 소리낼 수 있는 개수 제한
  //-----------------------------------------------------------
  // player의 소리는 제한할 필요가 없다.
  // if (g_SoundPerSecond > g_pClientConfig->MAX_SOUND_PER_SECOND)
  //{
  //	__END_PROFILE("PlaySound2")
  // return;
  //}

  //-----------------------------------------------------------
  // 없으면 --> Load & Play
  //-----------------------------------------------------------
  if ((*g_pSoundManager).IsDataNULL(soundID)) {
    const char *pFilename = (*g_pSoundTable)[soundID].Filename;

    if (pFilename == NULL) {
      __END_PROFILE("PlaySound2")
      return;
    }

    char strFilename[256];
    strcpy(strFilename, pFilename);
    LPDIRECTSOUNDBUFFER pBuffer = g_DXSound.LoadWav(strFilename);
    // LPDIRECTSOUNDBUFFER	pBuffer =
    // g_pWavePackFileManager->LoadFromFileData(soundID);

    //-----------------------------------------------------------
    // Loading 실패
    //-----------------------------------------------------------
    if (pBuffer == NULL) {
      (*g_pSoundTable)[soundID].Filename.Release();
      DEBUG_ADD_FORMAT("[Error] Failed to Load WAV. id=%d, fn=%s", soundID,
                       (*g_pSoundTable)[soundID].Filename);
    }
    //-----------------------------------------------------------
    // Load에 성공 했으면...
    //-----------------------------------------------------------
    else {
      // Replace됐으면 원래것을 메모리에서 지운다.
      LPDIRECTSOUNDBUFFER pOld;
      if ((*g_pSoundManager).SetData(soundID, pBuffer, pOld) != 0xFFFF) {
        g_DXSound.Release(pOld);
      }

      g_DXSound.CenterPan(pBuffer);

      g_DXSound.SetMaxVolume(pBuffer);

      // Play
      g_DXSound.Play(pBuffer, false, g_bGoodFPS);

      g_SoundPerSecond++;

      //-------------------------------------------------
      // Force Feel
      //-------------------------------------------------
      // if (gpC_Imm!=NULL && gpC_Imm->IsDevice())
      {
        //	gpC_Imm->ForceSound( pFilename );
      }

      // reload++;
    }
  }
  //-----------------------------------------------------------
  // 있는 경우 --> Play
  //-----------------------------------------------------------
  else {
    LPDIRECTSOUNDBUFFER pBuffer;
    if ((*g_pSoundManager).GetData(soundID, pBuffer)) {
      g_DXSound.CenterPan(pBuffer);

      g_DXSound.SetMaxVolume(pBuffer);

      g_DXSound.Play(pBuffer, false, g_bGoodFPS);
      g_SoundPerSecond++;
    }

    // Force Feel
    //-------------------------------------------------
    /*
    const char* pFilename = (*g_pSoundTable)[soundID].Filename;

    if (pFilename!=NULL)
    {
            char strFilename[256];
            strcpy(strFilename, pFilename);
            if (gpC_Imm!=NULL && gpC_Imm->IsDevice())
            {
                    gpC_Imm->ForceSound( strFilename );
            }
    }
    */

    // reuse ++;
  }

  __END_PROFILE("PlaySound2")
}

//-----------------------------------------------------------------------------
// Stop Sound
//-----------------------------------------------------------------------------
void StopSound(TYPE_SOUNDID soundID) {
  //-----------------------------------------------------------
  // 정의되지 않는 sound ID일 경우..
  //-----------------------------------------------------------
  if (!g_DXSound.IsInit() || soundID == SOUNDID_NULL)
    return;

  //-----------------------------------------------------------
  // 정의되지 않는 sound ID일 경우..
  //-----------------------------------------------------------
  if (soundID == SOUNDID_NULL)
    return;

  //-----------------------------------------------------------
  // 없으면 --> 그냥 return
  //-----------------------------------------------------------
  if (soundID < g_pSoundManager->GetMaxIndex()) {
    if ((*g_pSoundManager).IsDataNULL(soundID)) {
      return;
    }
    //-----------------------------------------------------------
    // 있는 경우 --> Stop
    //-----------------------------------------------------------
    else {
      LPDIRECTSOUNDBUFFER pBuffer;
      if ((*g_pSoundManager).GetData(soundID, pBuffer)) {
        g_DXSound.Stop(pBuffer);
      }
    }
  }
}

//---------------------------------------------------------------------------
// Play Music Current Zone
//---------------------------------------------------------------------------
// 존에 따라 노래를 바꾼다.
//---------------------------------------------------------------------------
void PlayMusicCurrentZone() {
  // 다음 조건에서는 노래를 재생하지 않는다.
  if (g_Mode != MODE_GAME || g_pZone == NULL || !g_pUserOption->PlayMusic ||
      g_pEventManager->GetEventByFlag(EVENTFLAG_NOT_PLAY_SOUND))
    return;

  // 존 정보
  TYPE_ZONEID zoneID = g_bZonePlayerInLarge ? g_nZoneLarge : g_nZoneSmall;
  ZONETABLE_INFO *pZoneInfo = g_pZoneTable->Get(zoneID);
  if (pZoneInfo == NULL) {
    CT_Done("[PlayMusicCurrentZone] invalid zone state zoneID=%d large=%d small=%d inLarge=%d g_pZone=%p",
            zoneID, g_nZoneLarge, g_nZoneSmall, (int)g_bZonePlayerInLarge,
            (void*)g_pZone);
    DEBUG_ADD_FORMAT("[Error] Invalid zone state for music. zoneID=%d large=%d small=%d inLarge=%d",
                     zoneID, g_nZoneLarge, g_nZoneSmall,
                     (int)g_bZonePlayerInLarge);
    return;
  }

  // 플레이할 노래 정보
  TYPE_MUSICID MusicID = pZoneInfo->MusicID;

  // 아담의 성지는 전쟁중일 때 노래가 다르다.
  if (zoneID >= 71 && zoneID <= 75 && g_pUserOption->PlayWaveMusic) {
    const TYPE_MUSICID musicByAdam[] = {
        MUSIC_HOLYLAND,
        MUSIC_HOLYLAND_WAR,
    };

    int musicIdx = g_pWarManager->IsExist(zoneID) ? 1 : 0;

    MusicID = musicByAdam[musicIdx];
  }
  // 정해진 음악이 없다면 랜덤
  else if (MusicID == MUSICID_NULL) {
    const int randomSize = 13;
    const TYPE_MUSICID musicByRandom[randomSize] = {
        MUSIC_HELL_KNIGHT, MUSIC_HIDE_AWAY, MUSIC_HOLLOWEEN, MUSIC_LIVE_OR_DEAD,
        MUSIC_WHISPER,     MUSIC_MARCHING,  MUSIC_TREASURE,  MUSIC_ANONCIAD,
        MUSIC_CONCORDIA,   MUSIC_OVERTURE,  MUSIC_GEHENNA,   MUSIC_ADVANCE,
        MUSIC_SCHWARZWALD,
    };

    TYPE_MUSICID randomIdx = rand() % randomSize;

    MusicID = musicByRandom[randomIdx];
  }

  // 플레이할 노래가 있다면 노래 ㄱㄱ
  if (MusicID != MUSICID_NULL) {
    if (g_pUserOption->PlayWaveMusic) {
#ifdef __USE_MP3__
      DEBUG_ADD("MP3 OPEN1");
      g_pMP3->Open((*g_pMusicTable)[MusicID].FilenameWav);
      DEBUG_ADD("MP3 OPEN1 OK");
      DEBUG_ADD("MP3 PLAY3");
      g_pMP3->Play(false);
      DEBUG_ADD("MP3 PLAY3 OK");
//			g_pDXSoundStream->Load( (*g_pMusicTable)[ MusicID
//].FilenameWav ); 			g_pDXSoundStream->Play( FALSE );
#else
      // 			if( g_oggfile != NULL )
      // 				fclose(g_oggfile);
      //
      // 			g_oggfile = NULL;
      //
      // 			if( g_DXSound.IsInit() )
      // 			{
      // 				//g_pOGG->streamStop() ;
      //
      // 				string strName = (*g_pMusicTable)[
      // MusicID ].FilenameWav;
      //
      // 				g_pOGG->streamClose();
      // 				g_oggfile = fopen( (*g_pMusicTable)[
      // MusicID ].FilenameWav, "rb"); 				if( g_oggfile !=
      // NULL )
      // 				{
      // 					// 2004, 11, 8, sobeit add start
      // - 그냥 에러 체크 					if(SOUND_ERR_OK
      // == g_pOGG->streamLoad( g_oggfile, NULL
      // ))
      // 					{
      // 						if(SOUND_ERR_OK ==
      // g_pOGG->streamPlay( SOUND_PLAY_ONCE ))
      // 						{
      // 							int volume =
      // (g_pUserOption->VolumeMusic - 15) * 250;
      // g_pOGG->streamVolume( max( -10000, min( -1, volume ) ) );
      // 						}
      // 					}
      // 					// 2004, 11, 8, sobeit add end -
      // 그냥 에러 체크
      // 				}
      // 			}

      BGMManager::Instance()->PlayMusic((*g_pMusicTable)[MusicID].FilenameWav);
      BGMManager::Instance()->SetVolumeFromSlider(g_pUserOption->VolumeMusic);
#endif
    } else {
      g_Music.Play((*g_pMusicTable)[MusicID].Filename);
    }
  }
}

/*
//---------------------------------------------------------------------------
// Play Music Current Zone
//---------------------------------------------------------------------------
// 존에 따라 노래를 바꾼다.
//---------------------------------------------------------------------------
void
PlayMusicCurrentZone()
{
        // 시간에 따라 play
        if(g_pEventManager->GetEventByFlag(EVENTFLAG_NOT_PLAY_SOUND))
                return;

        const MUSIC_ID musicByTime[] =
        {
                MUSIC_LIVE_OR_DEAD,
                MUSIC_WINDMILL,
                MUSIC_WINDMILL,
                MUSIC_TREASURE,
                MUSIC_MARCHING,
                MUSIC_WHISPER,
                MUSIC_HELL_KNIGHT,
                MUSIC_LUNATIC,
                MUSIC_SAY_AGAIN,
                MUSIC_HIDE_AWAY,
                MUSIC_HELL_KNIGHT,
                MUSIC_HOLLOWEEN,
        };


        // 시간에 따라 play
        const MUSIC_ID musicByTimeXmas[] =
        {
                MUSIC_XMAS_SILENTNIGHT,
                MUSIC_XMAS_SILENTNIGHT,
                MUSIC_XMAS_SILVERBELL,
                MUSIC_XMAS_SILVERBELL,
                MUSIC_XMAS_JINGLEBELL,
                MUSIC_XMAS_JINGLEBELL,
                MUSIC_XMAS_JINGLEBELL,
                MUSIC_XMAS_JINGLEBELL,
                MUSIC_XMAS_SILVERBELL,
                MUSIC_XMAS_SILVERBELL,
                MUSIC_XMAS_SILENTNIGHT,
                MUSIC_XMAS_SILENTNIGHT
        };

        const MUSIC_ID musicByHolyLand[] =
        {
                MUSIC_HOLYLAND,
                MUSIC_HOLYLAND_WAR,
                MUSIC_OCTAVUS,
                MUSIC_SEPTIMUS,
                MUSIC_TERTIUS,
                MUSIC_QUARTUS,
        };

        // 크리스마스 이벤트 날짜 체크
//	SYSTEMTIME st;
//	GetLocalTime( &st );

        // 크리스마스면 크리스마스 노래를 틀어준다.
        int newMusicID;

        // 안전지대인가?
        int zoneID	= (g_bZonePlayerInLarge?g_nZoneLarge : g_nZoneSmall);
        ZONETABLE_INFO* pZoneInfo = g_pZoneTable->Get( zoneID );
        bool bSafetyZone = (pZoneInfo!=NULL && pZoneInfo->Safety);

//	if (bSafetyZone
//		&& st.wMonth==12 && st.wDay>=17 && st.wDay<=25)
//	{
//		newMusicID = musicByTimeXmas[ (g_pGameTime->GetHour() / 2) % 12
];
//	}
//	else
        if( pZoneInfo->HolyLand && g_pUserOption->PlayWaveMusic)
        {
                switch ( zoneID )
                {
                case 1201 :
                case 1211 :
                case 1212 :
                        newMusicID = musicByHolyLand[2];
                        break;
                case 1202 :
                case 1221 :
                case 1222 :
                        newMusicID = musicByHolyLand[4];
                        break;
                case 1203 :
                case 1231 :
                case 1232 :
                        newMusicID = musicByHolyLand[3];
                        break;
                case 1204 :
                case 1241 :
                case 1242 :
                        newMusicID = musicByHolyLand[5];
                        break;
                case 1205 :
                case 1251 :
                case 1252 :
                        newMusicID = musicByHolyLand[2];
                        break;
                case 1206 :
                case 1261 :
                case 1262 :
                        newMusicID = musicByHolyLand[4];
                        break;
                default :
                        if(g_pWarManager->IsExist( zoneID ) )
                                newMusicID = musicByHolyLand[1];
                        else
                                newMusicID = musicByHolyLand[0];
                        break;
                }
        } else
        {
                newMusicID = musicByTime[ (g_pGameTime->GetHour() / 2) % 12 ];
        }
        // 2004, 7, 6 sobeit add start - 질드레 레어 배경음 - bgm을 info에서
참조 하지 않는건지..
        //if(zoneID == 1410 || zoneID == 1411)
        //	newMusicID = MUSIC_ADVANCE ; //MUSIC_ILLUSIONS_WAY;
        //else if(zoneID == 1412 || zoneID == 1413)
        //	newMusicID = MUSIC_ADVANCE ; // MUSIC_GDR_LAIR;
        // 2004, 7, 6 sobeit add start

        if (g_Mode==MODE_GAME && g_pZone!=NULL && g_pUserOption->PlayMusic)
        {
                //int zondID = (g_bZonePlayerInLarge? g_nZoneLarge :
g_nZoneSmall);

                //ZONETABLE_INFO* pZoneInfo = (*g_pZoneTable).Get( zondID );

                if(MUSIC_ADVANCE == pZoneInfo->MusicIDSlayer)
                        newMusicID = pZoneInfo->MusicIDSlayer ;


                //if (pZoneInfo!=NULL)
                {
                        //------------------------------------------
                        // 종족에 따라서 음악이 달라진다.
                        //------------------------------------------
                        //TYPE_MUSICID	newMusicID = 0;

                        //if (g_pPlayer!=NULL && g_pPlayer->IsSlayer())
                        {
                        //	newMusicID = pZoneInfo->MusicIDSlayer;
                        }
                        //else
                        {
                        //	newMusicID = pZoneInfo->MusicIDVampire;
                        }

                        int musicID = newMusicID;

                        if (musicID!=MUSICID_NULL)
                        {
                                if (g_pUserOption->PlayWaveMusic)
                                {
#ifdef __USE_MP3__
                                        DEBUG_ADD("MP3 OPEN1");
                                        g_pMP3->Open( (*g_pMusicTable)[ musicID
].FilenameWav ); DEBUG_ADD("MP3 OPEN1 OK"); DEBUG_ADD("MP3 PLAY3");
                                        g_pMP3->Play( false );
                                        DEBUG_ADD("MP3 PLAY3 OK");
//					g_pDXSoundStream->Load(
(*g_pMusicTable)[ musicID ].FilenameWav );
//					g_pDXSoundStream->Play( FALSE );
#else
                                        if( g_oggfile != NULL )
                                                fclose(g_oggfile);

                                        g_oggfile = NULL;

                                        if( g_DXSound.IsInit() )
                                        {
                                                //g_pOGG->streamStop() ;

                                                string strName =
(*g_pMusicTable)[ musicID ].FilenameWav;

                                                g_pOGG->streamClose();
                                                g_oggfile = fopen(
(*g_pMusicTable)[ musicID ].FilenameWav, "rb"); if( g_oggfile != NULL )
                                                {
                                                        // 2004, 11, 8, sobeit
add start - 그냥 에러 체크 if(SOUND_ERR_OK == g_pOGG->streamLoad( g_oggfile,
NULL ))
                                                        {
                                                                if(SOUND_ERR_OK
== g_pOGG->streamPlay( SOUND_PLAY_ONCE ))
                                                                {
                                                                        int
volume = (g_pUserOption->VolumeMusic - 15) * 250; g_pOGG->streamVolume( max(
-10000, min( -1, volume ) ) );
                                                                }
                                                        }
                                                        // 2004, 11, 8, sobeit
add end - 그냥 에러 체크
                                                }
                                        }
#endif
                                }
                                else
                                {
                                        g_Music.Play( (*g_pMusicTable)[ musicID
].Filename );
                                }
                        }
                }
        }
}
*/

//---------------------------------------------------------------------------
// Set Weather
//---------------------------------------------------------------------------
// 날씨를 바꾼다.
//---------------------------------------------------------------------------
void SetWeather(int weather, int level) {
  static int previousWeather = weather;

  //---------------------------------------------
  // 이전의 날씨와 관련된 Sound를 없앤다.
  //---------------------------------------------
  StopSound(g_previousSoundID);

  DEBUG_ADD_FORMAT("[Set Weather] %d, %d", weather, level);

  //---------------------------------------------
  // 날씨 종류에 따라서...
  //---------------------------------------------
  if (previousWeather == WEATHER_SPOT)
    g_pWeather->Release();

  switch (weather) {
  //------------------------------
  // 맑은 날씨
  //------------------------------
  case WEATHER_CLEAR:
    // 이전에 비가 오던 날씨였으면...
    if (previousWeather == WEATHER_RAINY) {
      if (g_pTopView != NULL && g_pTopView->IsInit()) {
        g_pTopView->ClearShadowManager();
      }
    }

    g_pWeather->Stop();
    g_previousSoundID = SOUNDID_NULL;
    break;

  //------------------------------
  // 비
  //------------------------------
  case WEATHER_RAINY:
    // 이전에 맑은 날씨였다면...
    if (previousWeather != WEATHER_RAINY) {
      if (g_pTopView != NULL && g_pTopView->IsInit()) {
        g_pTopView->ClearShadowManager();
      }
    }

    g_pWeather->SetRain(level << 3);

    // 2는 비가 더 많이 올때의 소리
    g_previousSoundID =
        (level >= 15) ? SOUND_WORLD_WEATHER_RAIN_2 : SOUND_WORLD_WEATHER_RAIN_1;

    // 반복해서 비 소리 내기...
    PlaySound(g_previousSoundID, true, g_pPlayer->GetX(), g_pPlayer->GetY());
    break;

  //------------------------------
  // 눈
  //------------------------------
  case WEATHER_SNOWY:
    // 이전에 비가 오던 날씨였으면...
    if (previousWeather == WEATHER_RAINY) {
      if (g_pTopView != NULL && g_pTopView->IsInit()) {
        g_pTopView->ClearShadowManager();
      }
    }

    g_pWeather->SetSnow(level << 3);
    g_previousSoundID = SOUNDID_NULL;
    break;

  //------------------------------
  // Spot
  //------------------------------
  case WEATHER_SPOT:
    // 이전에 비가 오던 날씨였으면...
    if (previousWeather == WEATHER_RAINY) {
      if (g_pTopView != NULL && g_pTopView->IsInit()) {
        g_pTopView->ClearShadowManager();
      }
    }

    g_pWeather->SetSpot(level << 3);
    g_previousSoundID = SOUNDID_NULL;
    break;
  }

  previousWeather = weather;

  DEBUG_ADD("[Set Weather] OK");
}

//---------------------------------------------------------------------------
// Set Lightning
//---------------------------------------------------------------------------
void SetLightning(DWORD delay) {
  DEBUG_ADD("[Set Lightning]");

  //---------------------------------------------
  // 번개 표현..
  //---------------------------------------------
  if (g_pTopView != NULL && g_pTopView->IsInit()) {
    g_pTopView->SetFadeStart(1, 31, 10, 31, 31, 31);
  }

  // 1초 안의 거리에 있으면.. THUNDER1
  // 더 멀면 THUNDER2
  TYPE_SOUNDID sid = (delay <= 1000) ? SOUND_WORLD_WEATHER_THUNDER_1
                                     : SOUND_WORLD_WEATHER_THUNDER_2;

  // sound를 기억해뒀다가 나중에 출력한다.
  SOUND_NODE *pNode =
      new SOUND_NODE(sid, delay, g_pPlayer->GetX(), g_pPlayer->GetY());

  g_pZone->AddSound(pNode);
}

//---------------------------------------------------------------------------
// Open File
//---------------------------------------------------------------------------
bool FileOpenBinary(const char *filename, ivfstream &file) {
  if (file.is_open()) {
    file.close();
  }

  const int openMode = std::ios::in | std::ios::binary;
  file.open(filename, openMode);
  if (file.is_open()) {
    return true;
  }

  const bool relativePath =
      filename != NULL &&
      filename[0] != '\0' &&
      filename[0] != '\\' &&
      !(isalpha(static_cast<unsigned char>(filename[0])) && filename[1] == ':');

  if (relativePath && g_CWD[0] != '\0') {
    char rootedPath[MAX_PATH] = {0};
    _snprintf(rootedPath, sizeof(rootedPath) - 1, "%s\\%s", g_CWD, filename);
    rootedPath[sizeof(rootedPath) - 1] = '\0';

    if (file.is_open()) {
      file.close();
    }
    file.open(rootedPath, openMode);
    if (file.is_open()) {
      Phoenix_ClientLifecycleLogf("FileOpenBinary fallback ok original=%s rooted=%s",
                                  filename, rootedPath);
      return true;
    }
  }

  char currentDirectory[MAX_PATH] = {0};
  GetCurrentDirectory(MAX_PATH, currentDirectory);
  Phoenix_ClientLifecycleLogf("FileOpenBinary failed filename=%s cwd=%s moduleRoot=%s",
                              filename != NULL ? filename : "<null>",
                              currentDirectory,
                              g_CWD[0] != '\0' ? g_CWD : "<empty>");

  /*
  DEBUG_ADD_FORMAT("File Open OK : [%s]", filename);
  */
  return false;
}

//---------------------------------------------------------------------------
// Update Input
//---------------------------------------------------------------------------
void UpdateInput() {
  // Input Event
  // g_pDXInput->PollJoy();
  // g_pDXInput->PollKey();
  // g_pDXInput->PollMouse();
  if (g_pDXInput != NULL) {
    g_pDXInput->UpdateInput();
  }

//---------------------------------------------------
//
// mouse
//
//---------------------------------------------------
/*
static int xSign = 0;
static int ySign = 0;
static int mouseStep = 1;

// 부호 결정
int xSignNew = 0;
int ySignNew = 0;

if (g_pDXInput->m_mouse_x)
{
        xSignNew = (g_pDXInput->m_mouse_xdata > 0)? 1: -1;
}

if (g_pDXInput->m_mouse_y)
{
        ySignNew = (g_pDXInput->m_mouse_ydata > 0)? 1: -1;
}

//------------------------------------------------------
// 정지된 상태
//------------------------------------------------------
if (xSignNew==0 && ySignNew==0)
{
        mouseStep = 6;
}
//------------------------------------------------------
// 움직인 경우
//------------------------------------------------------
else
{
        //------------------------------------------------------
        // 같은 방향으로 이동하면..
        //------------------------------------------------------
        if (xSignNew==xSign && ySignNew==ySign)
        {
                // 최대 mouse 이동 pixel설정
                if (abs(mouseStep) < 20)
                {
                        mouseStep ++;
                }
        }
        //------------------------------------------------------
        // 다른 방향으로 이동하는 경우
        //------------------------------------------------------
        else
        {
                mouseStep = 6;
        }

        xSign = xSignNew;
        ySign = ySignNew;

        // 커서를 움직여 준다.
        int step = mouseStep >> 1;
        if (g_pDXInput->m_mouse_x) g_x += g_pDXInput->m_mouse_xdata * (step?
step : 1); if (g_pDXInput->m_mouse_y) g_y += g_pDXInput->m_mouse_ydata * (step?
step : 1);
}
*/

// sprintf(g_pDebugMessage->GetCurrent(), "(%d, %d)", g_pDXInput->m_mouse_xdata,
// g_pDXInput->m_mouse_ydata); g_pDebugMessage->Next();

// if (g_pDXInput->m_mouse_x) g_x += g_pDXInput->m_mouse_xdata*3;
// if (g_pDXInput->m_mouse_y) g_y += g_pDXInput->m_mouse_ydata*3;

// Cursor의 position을 얻어낸다.
/*
POINT point;
GetCursorPos(&point);
g_x = point.x;
g_y = point.y;


//-----------------------------------------------
// Mouse Cursor가 밖으로 나가지 않도록 한다.
//-----------------------------------------------
if (g_x<0) g_x=0;
else if (g_x>=SURFACE_WIDTH) g_x=SURFACE_WIDTH-1;
if (g_y<0) g_y=0;
else if (g_y>=SURFACE_HEIGHT) g_y=SURFACE_HEIGHT-1;
*/
// - -;;
// g_pDXInput->SetMousePosition( g_x, g_y );

//-----------------------------------------------
// 새로 입력된 것을 CInputManager에 저장
//-----------------------------------------------
/*
g_InputManager.SetPosition( g_x, g_y );

if (g_pDXInput->m_lbutton_down)	g_InputManager.SetLButtonDown();
if (g_pDXInput->m_lbutton_up)	g_InputManager.SetLButtonUp();
if (g_pDXInput->m_rbutton_down)	g_InputManager.SetRButtonDown();
if (g_pDXInput->m_rbutton_up)	g_InputManager.SetRButtonUp();
*/}

//---------------------------------------------------------------------------
// Update Mouse
//---------------------------------------------------------------------------
void UpdateMouse() {
  // Cursor의 position을 얻어낸다.
  POINT point;
  GetCursorPos(&point);
  ScreenToClient(g_hWnd, &point);
  g_x = point.x;
  g_y = point.y;

  //-----------------------------------------------
  // Mouse Cursor가 밖으로 나가지 않도록 한다.
  //-----------------------------------------------
  /*
  if (g_x<0) g_x=0;
  else if (g_x>=SURFACE_WIDTH) g_x=SURFACE_WIDTH-1;
  if (g_y<0) g_y=0;
  else if (g_y>=SURFACE_HEIGHT) g_y=SURFACE_HEIGHT-1;
  */
}

//---------------------------------------------------------------------------
// Check Time
//---------------------------------------------------------------------------
// speedhack체크를 위해서 1분마다 한번씩 패킷을 보낸다.
//---------------------------------------------------------------------------
void CheckTime() {
  if (g_pSocket != NULL) {
#if __CONTENTS(__CHECKTIME_CONTROLL)
    static DWORD nextTime = g_CurrentTime + 5000; // 스핵 때문에 막음
#else
    static DWORD nextTime = g_CurrentTime + 60000; // 기존 코드
#endif //__CHECKTIME_CONTROLL

    //------------------------------------------------------------------
    // 1분 마다 한번씩 garbarge packet을 보낸다.
    //------------------------------------------------------------------
    if (g_CurrentTime > nextTime) // 60 * 1000
    {
      CGVerifyTime _CGVerifyTime;

      //					_CGVerifyTime.SetTimeStamp(g_CurrentTime);
      g_pSocket->sendPacket(&_CGVerifyTime);

#if __CONTENTS(__CHECKTIME_CONTROLL)
      nextTime =
          timeGetTime() + 5000; // g_CurrentTime;	//스핵 때문에 막음
#else
      nextTime = timeGetTime() + 60000; // g_CurrentTime;	//기존 코드
#endif //__CHECKTIME_CONTROLL
    }
  }
}

//---------------------------------------------------------------------------
// keep Connection
//---------------------------------------------------------------------------
void KeepConnection() {
  /*
  // CGVerifyTime이 있으므로 이제 필요없다.
  #ifdef CONNECT_SERVER
          if (
                  #ifndef _DEBUG		// debug버전에서는 무조건
  KeepConnection이다. (g_pUserInformation->KeepConnection ||
  g_pUserInformation->IsMaster)
                          &&
                  #endif
                  g_pSocket!=NULL)

          {
                  static DWORD lastTime = g_CurrentTime;

                  //------------------------------------------------------------------
                  // 3분 마다 한번씩 garbarge packet을 보낸다.
                  //------------------------------------------------------------------
                  if (g_CurrentTime - lastTime > 180000)		// 3 *
  60 * 1000
                  {
                          #ifdef	CONNECT_SERVER
                                  CGSay _CGSay;
                                  _CGSay.setMessage( "*" );
                                  g_pSocket->sendPacket( &_CGSay );
                          #endif

                          lastTime = g_CurrentTime;
                  }
          }
  #endif
  */
}

//---------------------------------------------------------------------------
// Update Disconnected
//---------------------------------------------------------------------------
void UpdateDisconnected() {
  CT_BD("[SOCK] UpdateDisconnected enter mode=%d socket=%p zone=%p",
        (int)g_Mode, (void*)g_pSocket, g_pZone);
  CT_Done("DIAG: UpdateDisconnected ENTERED");
  PhoenixSocketUpdateTrace("UpdateDisconnected enter mode=%d socket=%p", (int)g_Mode, (void*)g_pSocket);
  PhoenixLoginSocketTrace("UpdateDisconnected: enter mode=%d socket=%p", (int)g_Mode, (void*)g_pSocket);
  DEBUG_ADD("UpdateDisconnected : Start Process");

  //--------------------------------------------------
  // socket제거
  //--------------------------------------------------
  ReleaseSocket();

  //--------------------------------------------------
  // 소리 중지
  //--------------------------------------------------
  StopSound(g_previousSoundID);
  StopSound(SOUND_WORLD_PROPELLER);

  //--------------------------------------------------
  // 음악 멈춘다.
  //--------------------------------------------------
  if (g_pUserOption->PlayWaveMusic) {
    //		g_pDXSoundStream->Stop();
    DEBUG_ADD("MP3 STOP5");
#ifdef __USE_MP3__
    g_pMP3->Stop();
#else
    if (g_DXSound.IsInit())
      BGMManager::Instance()->StopMusic();
//			g_pOGG->streamClose();
#endif
    DEBUG_ADD("MP3 STOP5 OK");
  } else {
    g_Music.Stop();
  }

  DEBUG_ADD("UpdateDisconnected : Stop Music OK");

  //--------------------------------------------------
  // Konu yükleniyor...
  //--------------------------------------------------
  //	StopLoadingThread();

  //	DEBUG_ADD("UpdateDisconnected : Stop Thread OK");

  //--------------------------------------------------
  // game object들 제거
  //--------------------------------------------------
  ReleaseGameObject();

  DEBUG_ADD("UpdateDisconnected : Release GameObject OK");

  //------------------------------------------------------
  // Close all dialogues.
  //------------------------------------------------------
  gC_vs_ui.CloseOption();
  //	gC_vs_ui.CloseInfo();
  //	gC_vs_ui.CloseShop();
  // UI_SetShop( NULL );
  // gC_vs_ui.CloseSkillTree();
  // gC_vs_ui.CloseGameMenu();
  // gC_vs_ui.CloseUserRegistrationWindow();

  //	DEBUG_ADD("UpdateDisconnected : ServerDisc");
  //	gC_vs_ui.ServerDisconnectMessage();
  //	DEBUG_ADD("UpdateDisconnected : ServerDisc OK");

  //	g_pUIDialog->CloseHelpDlg();
  g_pUIDialog->CloseMessageDlg();
  g_pUIDialog->ClosePCTalkDlg();

  DEBUG_ADD("UpdateDisconnected : Close All Dialog OK");

  //--------------------------------------------------
  // A dialogue box stating that the connection has been lost
  //--------------------------------------------------
  // The ultimate... temporary code...
  CFileIndexTable FIT;

  ivfstream indexFile(g_pFileDef->getProperty("FILE_SPRITEINDEX_UI").c_str(),
                      std::ios::binary);
  FIT.LoadFromFile(indexFile);
  indexFile.close();

  if (FIT.GetSize() <= SPRITEID_DISCONNECTED_CLOSE_PUSHED) {
    CT_Done("UpdateDisconnected : UI sprite index count invalid size=%d",
            (int)FIT.GetSize());
    DEBUG_ADD_ERR("UpdateDisconnected : UI sprite index count invalid");
    return;
  }

  // D3D9 composition buffer R5G6B5 kullanir. Bu eski özel dialog yolu
  // CSpritePack::Init() ile legacy 555/565 runtime state'ine baglaninca
  // 565 UI.spk verisi 555 gibi okunup yesil gorunebiliyor. Disconnected
  // UI.spk diskte 565 oldugu icin bu path'i sabit CSprite565 ile yukle.
  CSprite565 spriteDisconected;
  CSprite565 spriteDisconectedCloseFocused;
  CSprite565 spriteDisconectedClosePushed;

  // until you press Return...
  CSprite *pSpriteDisconected = &spriteDisconected;
  CSprite *pSpriteDisconectedCloseFocused = &spriteDisconectedCloseFocused;
  CSprite *pSpriteDisconectedClosePushed = &spriteDisconectedClosePushed;

  ivfstream spkFile(g_pFileDef->getProperty("FILE_SPRITE_UI").c_str(),
                    std::ios::binary);

  spkFile.seekg(FIT[SPRITEID_DISCONNECTED], std::ios::beg);
  const bool disconnectedLoaded = pSpriteDisconected->LoadFromFile(spkFile);

  spkFile.seekg(FIT[SPRITEID_DISCONNECTED_CLOSE_FOCUSED], std::ios::beg);
  const bool closeFocusedLoaded =
      pSpriteDisconectedCloseFocused->LoadFromFile(spkFile);

  spkFile.seekg(FIT[SPRITEID_DISCONNECTED_CLOSE_PUSHED], std::ios::beg);
  const bool closePushedLoaded =
      pSpriteDisconectedClosePushed->LoadFromFile(spkFile);

  spkFile.close();

  if (!disconnectedLoaded || !closeFocusedLoaded || !closePushedLoaded) {
    CT_Done("UpdateDisconnected : UI sprite load failed disconnected=%d focused=%d pushed=%d",
            disconnectedLoaded, closeFocusedLoaded, closePushedLoaded);
    DEBUG_ADD_ERR("UpdateDisconnected : UI sprite load failed");
    return;
  }

  DEBUG_ADD("UpdateDisconnected : Load Disconnected Dialog OK");

  //--------------------------------------------------
  // Öncelikle bağlantı kesildiğini belirten bir uyarı mesajı gösterir.
  //--------------------------------------------------
  //	if (CDirect3D::IsHAL())
  //	{
  //		POINT point;
  //		if (CDirect3D::GetDevice()->BeginScene()!=D3D_OK)
  //		{
  //			return;
  //		}
  //
  //		//gC_vs_ui.Show();
  //
  //		//-----------------------------------------------------------------
  //		// Disconnected dialog
  //		//-----------------------------------------------------------------
  //		g_pBack->Lock();
  //
  //		point.x = 400 - (pSpriteDisconected->GetWidth()>>1);
  //		point.y = 300 - (pSpriteDisconected->GetHeight()>>1);
  //
  //		g_pBack->BltSprite( &point, pSpriteDisconected );
  //
  ////		const int maskX = 116;
  ////		const int maskY = 109;
  ////		point.x = 400 - (pSpriteDisconected->GetWidth()>>1) + maskX;
  ////		point.y = 300 - (pSpriteDisconected->GetHeight()>>1) + maskY;
  ////		g_pBack->BltSprite( &point, pSpriteDisconectedMask );
  //
  //		g_pBack->Unlock();
  //
  //		//gC_vs_ui.MouseControl(M_MOVING, g_x, g_y);
  //		//gC_vs_ui.DrawMousePointer();
  //
  //		CDirect3D::GetDevice()->EndScene();
  //	}
  //	else
  {
    // gC_vs_ui.Show();
    POINT point;

    //-----------------------------------------------------------------
    // Disconnected dialog
    //-----------------------------------------------------------------
    g_pLast->Lock();

    //		if( g_pUserInformation->IsResolution1024 )
    //		{
    point.x = SURFACE_WIDTH / 2 - (pSpriteDisconected->GetWidth() >> 1);
    point.y = SURFACE_HEIGHT / 2 - (pSpriteDisconected->GetHeight() >> 1);
    //		}
    //		else
    //		{
    //			point.x = 400 - (pSpriteDisconected->GetWidth()>>1);
    //			point.y = 300 - (pSpriteDisconected->GetHeight()>>1);
    //		}
    g_pLast->BltSprite(&point, pSpriteDisconected);

    //		const int maskX = 116;
    //		const int maskY = 109;
    //		point.x = 400 - (pSpriteDisconected->GetWidth()>>1) + maskX;
    //		point.y = 300 - (pSpriteDisconected->GetHeight()>>1) + maskY;
    //		g_pBack->BltSprite( &point, pSpriteDisconectedMask );

    g_pLast->Unlock();

    // gC_vs_ui.MouseControl(M_MOVING, g_x, g_y);
    // gC_vs_ui.DrawMousePointer();

    //-----------------------------------------------------------------
    // Last를 Back으로 copy - 3D HAL이 아닌 경우만..
    //-----------------------------------------------------------------
    point.x = 0;
    point.y = 0;
    RECT rect = {0, 0, SURFACE_WIDTH, SURFACE_HEIGHT};

    g_pBack->BltNoColorkey(&point, g_pLast, &rect);
  }

  CDirectDraw::Flip();

  DEBUG_ADD("UpdateDisconnected : Draw Disconnected Dialog Once OK");

  //------------------------------------------------------
  // Main 화면..
  //------------------------------------------------------
  //	gC_vs_ui.StartTitle();

  DEBUG_ADD("UpdateDisconnected : UI Start Title OK");

  //------------------------------------------------------
  // ALT + TAB 상태가 아닐 때
  //------------------------------------------------------
  extern bool g_bTestMode;
  if (g_bActiveGame
#ifdef OUTPUT_DEBUG
      || g_bTestMode
#endif
  ) {
    DEBUG_ADD("UpdateDisconnected : Wait For Press Key");

    const int closeX = 111;
    const int closeY = 69;
    const int closeWidth = 198;
    const int closeHeight = 20;

    BOOL bClickClose = FALSE, bClickFocused = FALSE;

    MSG msg;

    while (1) {
      // UI'ye giriş yapılmasını engellemem gerekiyor ama...
      if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
        if (!GetMessage(&msg, NULL, 0, 0))
          break;
        // return msg.wParam;

        if (msg.message != WM_KEYDOWN && msg.message != WM_CHAR &&
            msg.message != WM_KEYUP && msg.message != WM_IME_COMPOSITION &&
            msg.message != WM_IME_STARTCOMPOSITION &&
            msg.message != WM_IME_ENDCOMPOSITION) {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }
      }

      UpdateInput();
      UpdateMouse();

      POINT point;

      RECT rectClose;
      //			if( g_pUserInformation->IsResolution1024 )
      //			{
      rectClose.left =
          SURFACE_WIDTH / 2 - (pSpriteDisconected->GetWidth() >> 1) + closeX;
      rectClose.top =
          SURFACE_HEIGHT / 2 - (pSpriteDisconected->GetHeight() >> 1) + closeY;
      //			}
      //			else
      //			{
      //				rectClose.left = 400 -
      //(pSpriteDisconected->GetWidth()>>1) + closeX;
      //rectClose.top = 300 - (pSpriteDisconected->GetHeight()>>1) + closeY;
      //			}

      rectClose.right = rectClose.left + closeWidth;
      rectClose.bottom = rectClose.top + closeHeight;

      if (g_pDXInput->KeyDown(DIK_ESCAPE) ||
          g_pDXInput->KeyDown(DIK_NUMPADENTER) ||
          g_pDXInput->KeyDown(DIK_RETURN) || g_pDXInput->KeyDown(DIK_LMENU) ||
          g_pDXInput->KeyDown(DIK_RMENU) || g_pDXInput->KeyDown(DIK_TAB) ||
          g_pDXInput->KeyDown(DIK_SPACE)) {
        break;
      }

      if (g_x > rectClose.left && g_x < rectClose.right &&
          g_y > rectClose.top && g_y < rectClose.bottom) {
        bClickFocused = TRUE;
      } else
        bClickFocused = FALSE;

      if (g_pDXInput->m_lb_down) {
        if (g_x > rectClose.left && g_x < rectClose.right &&
            g_y > rectClose.top && g_y < rectClose.bottom) {
          bClickClose = TRUE;
        }
      }

      if (g_pDXInput->m_lb_up) {
        if (bClickClose) {
          if (g_x > rectClose.left && g_x < rectClose.right &&
              g_y > rectClose.top && g_y < rectClose.bottom) {
            break;
          }
        }

        bClickClose = FALSE;
      }

      //			if (CDirect3D::IsHAL())
      //			{
      //				if
      //(CDirect3D::GetDevice()->BeginScene()!=D3D_OK)
      //				{
      //					return;
      //				}
      //
      //				gC_vs_ui.Show();
      //
      //				//-----------------------------------------------------------------
      //				// Disconnected dialog
      //				//-----------------------------------------------------------------
      //				g_pBack->Lock();
      //
      //				point.x = 400 -
      //(pSpriteDisconected->GetWidth()>>1); 				point.y
      //= 300 - (pSpriteDisconected->GetHeight()>>1);
      //
      //				g_pBack->BltSprite( &point,
      // pSpriteDisconected );
      //
      //				if (bClickFocused)
      //				{
      //					if (g_x > rectClose.left && g_x
      //< rectClose.right
      //						&& g_y > rectClose.top
      //&& g_y < rectClose.bottom)
      //					{
      //						point.x = 400 -
      //(pSpriteDisconected->GetWidth()>>1) + closeX;
      //point.y = 300 - (pSpriteDisconected->GetHeight()>>1) + closeY;
      //
      //						g_pBack->BltSprite(
      //&point, pSpriteDisconectedCloseFocused );
      //					}
      //				}
      //
      //				if (bClickClose)
      //				{
      //					if (g_x > rectClose.left && g_x
      //< rectClose.right
      //						&& g_y > rectClose.top
      //&& g_y < rectClose.bottom)
      //					{
      //						point.x = 400 -
      //(pSpriteDisconected->GetWidth()>>1) + closeX;
      //point.y = 300 - (pSpriteDisconected->GetHeight()>>1) + closeY;
      //
      //						g_pBack->BltSprite(
      //&point, pSpriteDisconectedClosePushed );
      //					}
      //				}
      //
      //				g_pBack->Unlock();
      //
      //				gC_vs_ui.MouseControl(M_MOVING, g_x,
      // g_y); 				gC_vs_ui.DrawMousePointer();
      //
      //				CDirect3D::GetDevice()->EndScene();
      //			}
      //			else
      {
        gC_vs_ui.Show();

        //-----------------------------------------------------------------
        // Disconnected dialog
        //-----------------------------------------------------------------
        g_pLast->Lock();

        //				if( g_pUserInformation->IsResolution1024
        //)
        //				{
        point.x = SURFACE_WIDTH / 2 - (pSpriteDisconected->GetWidth() >> 1);
        point.y = SURFACE_HEIGHT / 2 - (pSpriteDisconected->GetHeight() >> 1);
        //				}
        //				else
        //				{
        //					point.x = 400 -
        //(pSpriteDisconected->GetWidth()>>1);
        //point.y = 300 - (pSpriteDisconected->GetHeight()>>1);
        //				}

        g_pLast->BltSprite(&point, pSpriteDisconected);

        if (bClickFocused) {
          if (g_x > rectClose.left && g_x < rectClose.right &&
              g_y > rectClose.top && g_y < rectClose.bottom) {
            //						if(
            // g_pUserInformation->IsResolution1024 )
            //						{
            point.x = SURFACE_WIDTH / 2 -
                      (pSpriteDisconected->GetWidth() >> 1) + closeX;
            point.y = SURFACE_HEIGHT / 2 -
                      (pSpriteDisconected->GetHeight() >> 1) + closeY;
            //						}
            //						else
            //						{
            //							point.x = 400 -
            //(pSpriteDisconected->GetWidth()>>1) + closeX;
            //point.y = 300 - (pSpriteDisconected->GetHeight()>>1) + closeY;
            //						}

            g_pLast->BltSprite(&point, pSpriteDisconectedCloseFocused);
          }
        }

        if (bClickClose) {
          if (g_x > rectClose.left && g_x < rectClose.right &&
              g_y > rectClose.top && g_y < rectClose.bottom) {
            //						if(
            // g_pUserInformation->IsResolution1024 )
            //						{
            point.x = SURFACE_WIDTH / 2 -
                      (pSpriteDisconected->GetWidth() >> 1) + closeX;
            point.y = SURFACE_HEIGHT / 2 -
                      (pSpriteDisconected->GetHeight() >> 1) + closeY;
            //						}
            //						else
            //						{
            //							point.x = 400 -
            //(pSpriteDisconected->GetWidth()>>1) + closeX;
            //point.y = 300 - (pSpriteDisconected->GetHeight()>>1) + closeY;
            //						}

            g_pLast->BltSprite(&point, pSpriteDisconectedClosePushed);
          }
        }

        g_pLast->Unlock();

        gC_vs_ui.MouseControl(M_MOVING, g_x, g_y);
        gC_vs_ui.DrawMousePointer();

        //-----------------------------------------------------------------
        // Last'i Back'e kopyala - Yalnızca 3D HAL olmadığı durumlarda...
        //-----------------------------------------------------------------
        point.x = 0;
        point.y = 0;
        RECT rect = {0, 0, SURFACE_WIDTH, SURFACE_HEIGHT};

        g_pBack->BltNoColorkey(&point, g_pLast, &rect);
      }

      CDirectDraw::Flip();
    }

    pSpriteDisconected->Release();
    pSpriteDisconectedCloseFocused->Release();
    pSpriteDisconectedClosePushed->Release();

    // 2004, 07, 19 sobeit add start - 서버와 연결할수 없습니다. 확인 누르면
    // 게임 종료하게 수정
    if (true == g_pUserInformation->IsAutoLogIn) // 웹 자동 로긴모드 일 때만
      SetMode(MODE_QUIT);
    // 2004, 07, 19 sobeit add end

  } else {
    DEBUG_ADD("UpdateDisconnected : Now ALT+TAB Mode");

    //		if (CDirect3D::IsHAL())
    //		{
    //			if (CDirect3D::GetDevice()->BeginScene()!=D3D_OK)
    //			{
    //				return;
    //			}
    //
    //			gC_vs_ui.Show();
    //
    //			gC_vs_ui.MouseControl(M_MOVING, g_x, g_y);
    //			gC_vs_ui.DrawMousePointer();
    //
    //			CDirect3D::GetDevice()->EndScene();
    //		}
    //		else
    {
      POINT point;

      gC_vs_ui.Show();

      gC_vs_ui.MouseControl(M_MOVING, g_x, g_y);
      gC_vs_ui.DrawMousePointer();

      //-----------------------------------------------------------------
      // Last'i Back'e kopyala - Yalnızca 3D HAL olmadığı durumlarda...
      //-----------------------------------------------------------------
      point.x = 0;
      point.y = 0;
      RECT rect = {0, 0, SURFACE_WIDTH, SURFACE_HEIGHT};

      g_pBack->BltNoColorkey(&point, g_pLast, &rect);
    }

    CDirectDraw::Flip();

    pSpriteDisconected->Release();
    pSpriteDisconectedCloseFocused->Release();
    pSpriteDisconectedClosePushed->Release();
  }

  DEBUG_ADD("UpdateDisconnected : OK");

  /*
  //InitFail("Server가 응답하지 않습니다.");
  //InitFail("Server와의 접속이 끊어졌습니다.");
  g_pBack->GDI_Text(101,201, "Server가 응답하지 않습니다.", RGB(0,0,0));
  g_pBack->GDI_Text(100,200, "Server가 응답하지 않습니다.", RGB(220,220,220));

  g_pBack->GDI_Text(101,221, "[ESC]를 누르세요.", RGB(0,0,0));
  g_pBack->GDI_Text(100,220, "[ESC]를 누르세요.", RGB(220,220,220));

  CDirectDraw::Flip();

  // return을 누를 때까지...
  while (1)
  {
          UpdateInput();

          if (g_pDXInput->KeyDown(DIK_ESCAPE))
          {
                  break;
          }
  }

  // Title화면 UI시작
  //gC_vs_ui.StartTitle();

  //g_pBack->GDI_Text(101,201, "잠시 기다려 주세요.", RGB(0,0,0));
  //g_pBack->GDI_Text(100,200, "잠시 기다려 주세요.", RGB(220,220,220));
  //CDirectDraw::Flip();
  */
}

// ok를 누른 뒤 실행되는 것.
void RunAfterServerDisconnect() {}

//-----------------------------------------------------------------------------
// Select LastSelected Character
//-----------------------------------------------------------------------------
// 이전에 선택한 캐릭터를 선택하기
//-----------------------------------------------------------------------------
void SelectLastSelectedCharacter() {
  if (g_pWorldPlayerConfigTable == NULL) {
    g_pWorldPlayerConfigTable = new WorldPlayerConfigTable;
  }

  if (g_pWorldPlayerConfigTable != NULL && g_pUserInformation != NULL &&
      g_pServerInformation != NULL) {
    if (g_pWorldPlayerConfigTable->empty()) {
      g_pWorldPlayerConfigTable->LoadFromFile(
          g_pFileDef->getProperty("FILE_INFO_PLAYER_INFO").c_str());
    }

    int worldID = g_pServerInformation->GetServerGroupID();

    PlayerConfigTable *pPCTable =
        g_pWorldPlayerConfigTable->GetPlayerConfigTable(worldID);

    if (pPCTable != NULL) {
      const char *playerID = g_pUserInformation->UserID.GetString();

      if (strlen(playerID) >= 12)
        return;

      PlayerConfig *pConfig = pPCTable->GetPlayerConfig(playerID);

      if (pConfig != NULL) {
        gC_vs_ui.SelectCharacter(pConfig->GetLastSlot());
      }
    }
  }
}

//-----------------------------------------------------------------------------
// Select LastSelected Character
//-----------------------------------------------------------------------------
// 현재 선택한 캐릭터를 저장하기
//-----------------------------------------------------------------------------
void SaveLastSelectedCharacter(int slot) {
  if (g_pWorldPlayerConfigTable == NULL) {
    g_pWorldPlayerConfigTable = new WorldPlayerConfigTable;
  }

  if (g_pWorldPlayerConfigTable != NULL && g_pServerInformation != NULL &&
      g_pUserInformation != NULL) {
    int worldID = g_pServerInformation->GetServerGroupID();
    const char *playerID = g_pUserInformation->UserID.GetString();

    PlayerConfigTable *pPCTable =
        g_pWorldPlayerConfigTable->GetPlayerConfigTable(worldID);

    if (pPCTable == NULL) {
      // worldID가 없으면 생성해서 추가
      pPCTable = new PlayerConfigTable;

      g_pWorldPlayerConfigTable->AddPlayerConfigTable(worldID, pPCTable);
    }

    PlayerConfig *pConfig = pPCTable->GetPlayerConfig(playerID);

    bool bNewPlayer = false;
    if (pConfig == NULL) {
      bNewPlayer = true;

      // playerID가 없으면 생성해서 추가
      pConfig = new PlayerConfig;
      if (playerID != NULL && strlen(playerID) < 12)
        pConfig->SetPlayerID(playerID);
      else
        pConfig->SetPlayerID("Phoenix");

      pPCTable->AddPlayerConfig(pConfig);
    }

    int oldSlot = pConfig->GetLastSlot();

    // 항상 저장하게 하자..
    // if (oldSlot != slot || bNewPlayer)
    {
      pConfig->SetLastSlot(slot);

      g_pWorldPlayerConfigTable->SaveToFile(
          g_pFileDef->getProperty("FILE_INFO_PLAYER_INFO").c_str());
    }
  }
}

//-----------------------------------------------------------------------------
// Set WatchMode ( true || false )
//-----------------------------------------------------------------------------
// 구경하는 mode..
//-----------------------------------------------------------------------------
void SetWatchMode(bool active) {
  //-------------------------------------------------------------------
  // Player를 Zone에서 제거
  // UI 작동 못하게 설정?
  // 이동 방식 변경
  //-------------------------------------------------------------------
  if (active) {
    g_pPlayer->SetStop();
    g_pPlayer->SetAction(ACTION_STAND);
    g_pZone->RemovePlayer();

    g_bWatchMode = true;
  }
  //-------------------------------------------------------------------
  //
  //-------------------------------------------------------------------
  else {
    g_pPlayer->SetZone(g_pZone);
    g_pZone->SetPlayer();
    g_pPlayer->SetTimeLightSight(g_pPlayer->GetTimeLightSight());

    g_bWatchMode = false;
  }
}

//-----------------------------------------------------------------------------
// Add ClientCreature
//-----------------------------------------------------------------------------
MCreature *AddClientCreature() {
  //------------------------------------------------
  // NPC
  //------------------------------------------------
  const int hairID[] = {ADDONID_HAIR1, ADDONID_HAIR2, ADDONID_HAIR3};

  static MCoat coat1;
  coat1.SetItemType(0);
  coat1.ClearItemOption();
  static MCoat coat2;
  coat2.SetItemType(1);
  coat2.ClearItemOption();
  static MItem *jacket[3] = {
      &coat1, // JACKET_BASIC,
      &coat1, // JACKET1 ,
      &coat2  // JACKET2
  };

  static MTrouser trouser1;
  trouser1.SetItemType(0);
  trouser1.ClearItemOption();
  static MTrouser trouser2;
  trouser2.SetItemType(1);
  trouser2.ClearItemOption();
  static MItem *pants[3] = {
      &trouser1, // PANTS_BASIC ,
      &trouser1, // PANTS1 ,
      &trouser2  // PANTS2 ,
  };

  static MHelm helm1;
  helm1.SetItemType(0);
  helm1.ClearItemOption();
  static MHelm helm2;
  helm2.SetItemType(1);
  helm2.ClearItemOption();
  static MItem *helm[3] = {
      NULL,   // NONE
      &helm1, // HELMET1 ,
      &helm2, // HELMET2
  };

  static MSword sword;
  sword.SetItemType(0);
  sword.ClearItemOption();
  static MBlade blade;
  blade.SetItemType(0);
  blade.ClearItemOption();
  static MGunAR gunAR;
  gunAR.SetItemType(0);
  gunAR.ClearItemOption();
  static MGunTR gunSR;
  gunSR.SetItemType(0);
  gunSR.ClearItemOption();
  static MGunSG gunSG;
  gunSG.SetItemType(0);
  gunSG.ClearItemOption();
  static MGunSMG gunSMG;
  gunSMG.SetItemType(0);
  gunSMG.ClearItemOption();
  static MCross cross;
  cross.SetItemType(0);
  cross.ClearItemOption();
  static MMace mace;
  mace.SetItemType(0);
  mace.ClearItemOption();
  static MItem *weapon[11] = {NULL,    // WEAPON_NONE ,
                              &sword,  // WEAPON_SWORD ,
                              &blade,  // WEAPON_BLADE ,
                              NULL,    // WEAPON_SHIELD ,
                              &sword,  // WEAPON_SWORD_SHIELD ,
                              &gunAR,  // WEAPON_AR ,
                              &gunSR,  // WEAPON_SR ,
                              &gunSG,  // WEAPON_SG ,
                              &gunSMG, // WEAPON_SMG ,
                              &cross,  // WEAPON_CROSS ,
                              &mace};

  static MShield shield;
  shield.SetItemType(0);
  shield.ClearItemOption();

  const int createPositionY[100] = {
      20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 26, 26, 25,
      25, 25, 25, 25, 25, 27, 27, 27, 27, 27, 26, 26, 26, 26, 26, 28, 28,
      28, 28, 28, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 32, 31, 32, 32,
      32, 35, 35, 34, 34, 34, 34, 37, 37, 35, 36, 39, 38, 38, 40};

  int creatureID;

  while (1) {
    creatureID = 1001 + rand() % 50000;

    MCreature *pCreature = g_pZone->GetCreature(creatureID);

    if (pCreature == NULL) {
      break;
    }
  }

  int x, y;

  //-----------------------------------
  // 옷 입는 Creature
  //-----------------------------------
  if (0) // rand()%2)
  {
    MCreatureWear *pCreature = new MCreatureWear;

    pCreature->SetZone(g_pZone);

    // (*g_pCreatureTable)의 정보들 중의 하나..
    pCreature->SetCreatureType(rand() % 2);

    // Move 종류를 다르게 한다.
    switch (0) // rand()%2)//rand()%3)
    {
    case 0:
      pCreature->SetGroundCreature();
      break;

    case 1:
      pCreature->SetFlyingCreature();
      break;

    case 2:
      pCreature->SetUndergroundCreature();
      break;
    }

    while (1) {
      x = rand() % (g_pZone->GetWidth() - 1) + 1;
      y = rand() % (g_pZone->GetHeight() - 1) + 1;
      // x = rand()%(20-1)+10;
      // y = rand()%(20-1)+20;	// 20~40
      /*
      int p = rand()%100;

      y = createPositionY[p];
      x = 12 - ((40-y)>>1) + (rand()%(41-y));

      if (rand()%2)
      {
              x += rand()%4;
      }
      else
      {
              x -= rand()%4;
      }
      */

      if (x >= 0 && x < g_pZone->GetWidth() && y >= 0 &&
          y < g_pZone->GetHeight()) {
        if (g_pZone->GetSector(x, y).IsEmpty()) {
          pCreature->SetPosition(x, y);
          pCreature->SetServerPosition(x, y);
          break;
        }
      }
    }
    // pCreature->SetID(1000 + y*256 + x);

    pCreature->SetID(creatureID);

    // 이름 설정 - 일단은 ID로
    // char str[80];
    // sprintf(str, "ID=%d", pCreature->GetID());
    // pCreature->SetName( str );

    pCreature->SetDirection(DIRECTION_DOWN);
    pCreature->SetNextAction(ACTION_STAND);

    pCreature->SetAddonHair(hairID[rand() % 3], 0); // type, color

    pCreature->SetAddonItem(jacket[rand() % 3]);
    pCreature->SetAddonItem(pants[rand() % 3]);
    pCreature->SetAddonItem(helm[rand() % 3]);
    int w = rand() % 10;
    pCreature->SetAddonItem(weapon[w]);
    if (w == 3 || w == 4) {
      pCreature->SetAddonItem(&shield);
    }

    if (!g_pZone->AddCreature(pCreature)) {
      delete pCreature;
      return NULL;
    }

    return pCreature;
  }
  //-----------------------------------
  // 옷 없는 Creature
  //-----------------------------------
  else {
    static int cType = 371; // 199//11;

    LoadCreatureType(cType);

    MCreature *pCreature;

    if ((*g_pCreatureTable)[cType].pItemWearInfo != NULL) {
      pCreature = new MCreatureWear;

      pCreature->SetCreatureType(cType); // 11+rand()%(23-11+1) );

      SetAddonToSlayer((MCreatureWear *)pCreature, cType);
    } else {
      pCreature = new MCreature;

      pCreature->SetCreatureType(cType); // 11+rand()%(23-11+1) );
    }

    // 존재하는 Zone을 설정한다.
    pCreature->SetZone(g_pZone);

    // (*g_pCreatureTable)의 정보들 중의 하나..
    // pCreature->SetCreatureType( (rand()%((*g_pCreatureTable).GetSize()-4))+4
    // );

    //------------------------------------------------------------
    // 바토리인 경우.. 하드코딩(-_-);
    //------------------------------------------------------------
    pCreature->SetStatus(MODIFY_MAX_HP, 100);
    if (cType == 217) {
      pCreature->SetStatus(MODIFY_CURRENT_HP, 10);
      pCreature->SetStatus(MODIFY_ALIGNMENT, -10000);
    } else {
      pCreature->SetStatus(MODIFY_CURRENT_HP, rand() % 101);
      pCreature->SetStatus(MODIFY_ALIGNMENT, -5000 + rand() % 10000);
    }

    cType++;

    if (cType == g_pCreatureTable->GetSize()) {
      cType = 296; // 104;//11;
    } else if (cType == 111) {
      cType = 197;
    } else if (cType == 200) {
      cType = 208;
    } else if (cType == 210) {
      cType = 217;
    } else if (cType == 220) {
      cType = 229;
    } else if (cType == 231) {
      cType = 239;
    } else if (cType == 241) {
      cType = 250;
    } else if (cType == 251) {
      cType = 256;
    } else if (cType == 257) {
      cType = 266;
    } else if (cType == 267) {
      cType = 276;
    } else if (cType == 277) {
      cType = 286;
    } else if (cType == 288) {
      cType = 318;
    } else if (cType == 320) {
      cType = 328;
    } else if (cType == 330) {
      cType = 338;
    } else if (cType == 340) {
      cType = 348;
    }

    pCreature->SetLevelName(rand() % 100);

    // int numGuildMark = g_pTopView->m_GuildSPK();
    // int guildMark = ((numGuildMark==0)? -1 : rand()%numGuildMark);
    // pCreature->SetGuildNumber( guildMark );

    if (pCreature->IsVampire()) {
      pCreature->SetGuildNumber(1);
    }

    // Move 종류를 다르게 한다.
    switch (0) // rand()%2)//rand()%3)
    {
    case 0:
      pCreature->SetGroundCreature();
      break;

    case 1:
      pCreature->SetFlyingCreature();
      break;

    case 2:
      pCreature->SetUndergroundCreature();
      break;
    }

    while (1) {
      x = rand() % 40 + 10;
      y = rand() % 40 + 10;

      // x = rand()%(g_pZone->GetWidth()-1)+1;
      // y = rand()%(g_pZone->GetHeight()-1)+1;
      // x = rand()%(20-1)+10;
      // y = rand()%(20-1)+20;	// 20~40
      /*
      int p = rand()%100;

      y = 140 - createPositionY[p];
      x = 12 - ((40-createPositionY[p])>>1) + (rand()%(41-createPositionY[p]));

      if (rand()%2)
      {
              x += rand()%4;
      }
      else
      {
              x -= rand()%4;
      }
      */

      if (x >= 0 && x < g_pZone->GetWidth() && y >= 0 &&
          y < g_pZone->GetHeight()) {
        if (g_pZone->GetSector(x, y).IsEmpty()) {
          pCreature->SetPosition(x, y);
          pCreature->SetServerPosition(x, y);
          break;
        }
      }
    }
    // pCreature->SetID(1000 + y*256 + x);
    pCreature->SetID(creatureID);

    // 이름 설정 - 일단은 ID로
    // if (pCreature->IsVampire())
    {
      // char str[80];
      // sprintf(str, "ID=%d, Type=%d", pCreature->GetID(),
      // pCreature->GetCreatureType());
      pCreature->SetName(
          (*g_pCreatureTable)[pCreature->GetCreatureType()].Name.GetString());

      // 함수자체가 클라이언트 전용-.-;;; 랜덤-_-;
      if ((pCreature->GetID() & 0x00000007) == 7) {
        pCreature->SetPlayerParty();
      }
    }

    int direction = DIRECTION_LEFTDOWN;

    int creatureType = pCreature->GetCreatureType();
    // FAZ 4 (2026-05-05): SafeSpriteType0 — boş SpriteTypes / OOB guard
    int spriteType = SafeSpriteType0(creatureType);
    if (spriteType <= 0 || g_pCreatureSpriteTable == NULL ||
        spriteType >= (int)g_pCreatureSpriteTable->GetSize()) {
      return FALSE;
    }
    int frameID = (*g_pCreatureSpriteTable)[spriteType].FrameID;

    if (frameID < g_pTopView->m_CreatureFPK.GetSize()) {
      if (g_pTopView->m_CreatureFPK[frameID][0][1].GetSize() == 0) {
        if (g_pTopView->m_CreatureFPK[frameID][0][3].GetSize() != 0) {
          direction = DIRECTION_RIGHTDOWN;
        } else if (g_pTopView->m_CreatureFPK[frameID][0][2].GetSize() != 0) {
          direction = DIRECTION_DOWN;
        }
      } else if (g_pTopView->m_CreatureFPK[frameID][0][3].GetSize() == 0) {
        if (g_pTopView->m_CreatureFPK[frameID][0][3].GetSize() != 0) {
          direction = DIRECTION_RIGHTDOWN;
        } else if (g_pTopView->m_CreatureFPK[frameID][0][2].GetSize() != 0) {
          direction = DIRECTION_DOWN;
        }
      } else {
        // default
        direction = DIRECTION_LEFTDOWN;
      }
    }

    pCreature->SetDirection(direction); // DIRECTION_UP);
    pCreature->SetCurrentDirection(direction);
    pCreature->SetNextAction(ACTION_STAND);

    if (!g_pZone->AddCreature(pCreature)) {
      delete pCreature;
      return NULL;
    }

    // [ TEST CODE ]
    // pCreature->SetChatString( "1234567890abcdefgijklmnopqrstuvwxyz");

    // 임의의 frame으로 시작한다.
    // int end = rand()%20;
    // for (int a=0; a<end; a++)
    //{
    //	pCreature->Action();
    //}

    return pCreature;
  }

  return NULL;
}

//-----------------------------------------------------------------------------
// GetMakeItemFitPosition
//-----------------------------------------------------------------------------
//
// 성수만들기, 폭탄/지뢰 만들기.. 등에서 사용된다.
//
// pItem으로 인해서 생성되는 item(itemClass, itemType)이 들어갈 위치를
// fitPoint를 통해서 얻을 수 있다.
//
// return true인 경우만 fitPoint에 들어갈 수 있는 위치가 저장된다.
// return false이면 들어갈 위치가 없다는 의미이다.
//-----------------------------------------------------------------------------
bool GetMakeItemFitPosition(MItem *pItem, ITEM_CLASS itemClass, int itemType,
                            POINT &fitPoint) {
  bool bFindPos = false; // 자리가 있나?

  // 임시로 성수를 만들어서 들어갈 자리를 찾는다.
  MItem *pResultItem = MItem::NewItem(itemClass);
  pResultItem->SetItemType(itemType);

  if (g_pInventory->GetFitPosition(pResultItem, fitPoint)) {
    bFindPos = true;

    MItem *pOldItem = g_pInventory->GetItem(fitPoint.x, fitPoint.y);

    // 빈 곳에 추가할려고 하고..
    // 하나만 남아있다면..
    // 현재 위치에서 그대로 바꾼다.
    if (pOldItem == NULL && pItem->GetNumber() == 1) {
      fitPoint.x = pItem->GetGridX();
      fitPoint.y = pItem->GetGridY();

      bFindPos = true;
    }
  }
  /*
  else
  {
          // 빈 자리가 없는 경우에
          // 하나만 남아있다면.. 그 자리에서 바꾸면 된다.
          if (pItem->GetNumber()==1)
          {
                  fitPoint.x = pItem->GetGridX();
                  fitPoint.y = pItem->GetGridY();

                  bFindPos = true;
          }
  }
  */

  // 임시로 만든것 제거
  delete pResultItem;

  return bFindPos;
}
// 2004, 03, 29 sobeit add start - 질드레 맵 고스트 추가
void Add_GDR_Ghost(int ZoneID) {
  if (1412 != ZoneID && 1413 != ZoneID) // 질드레 레어, 질드레 하드
    return;
  CRarFile GhostFile;
  GhostFile.SetRAR("data\\ui\\txt\\TutorialEtc.rpk", RPK_PASSWORD);
  if (!GhostFile.Open("ghostPos.xml"))
    return;

  XMLTree computerTree;
  XMLParser parser;
  char szTempBuffer[64];
  int MapX = 0, MapY = 0;

  sprintf(szTempBuffer, "PositionList_%d", ZoneID);
  parser.parse((char *)GhostFile.GetFilePointer(), &computerTree);

  const XMLTree *pMapElement = computerTree.GetChild(szTempBuffer);
  if (pMapElement != NULL) {
    const size_t GhostMax = pMapElement->GetChildCount();

    for (size_t GhostCount = 0; GhostCount < GhostMax; GhostCount++) {
      sprintf(szTempBuffer, "Position%d", GhostCount + 1);
      const XMLTree *pGhostPos = pMapElement->GetChild(szTempBuffer);
      if (pGhostPos != NULL) {
        const XMLAttribute *pPosX = pGhostPos->GetAttribute("x");
        if (pPosX != NULL) {
          MapX = pPosX->ToInt();
        }
        const XMLAttribute *pPosY = pGhostPos->GetAttribute("y");
        if (pPosY != NULL) {
          MapY = pPosY->ToInt();
        }

        // create ghost
        MFakeCreature *pFakeCreature = g_pZone->NewFakeCreature(
            CREATURETYPE_GHOST, MapX, MapY, rand() % 8);

        if (!g_pZone->AddFakeCreature(pFakeCreature)) {
          delete pFakeCreature;
          continue;
        }
        pFakeCreature->SetZone(g_pZone);
        pFakeCreature->SetFakeCreatureType(MFakeCreature::FAKE_CREATURE_GHOST);
        pFakeCreature->SetMoveType(MCreature::CREATURE_FAKE_UNDERGROUND);
        pFakeCreature->AddEffectStatus(
            (EFFECTSTATUS)(EFFECTSTATUS_GHOST_1 + rand() % 2), 0xFFFF);
      }
    }
  }
  GhostFile.Release();
}
// 2004, 03, 29 sobeit add end - 질드레 맵 고스트 추가
