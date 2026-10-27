




// GameInit.cpp





//---------------------------------------------------------------------------





// ???? ???? ?��?(??? ????? ??)?? ???? / Release





//---------------------------------------------------------------------------











//-----------------------------------------------------------------------------







// Include files





//-----------------------------------------------------------------------------





#include "client_PCH.h"
#include "crash_trace_tool.h"
#include "login_socket_trace.h"

#include "AcceleratorManager.h"





#include "AddonDef.h"





#include "CGameUpdate.h"





#include "COpeningUpdate.h"





#include "CWaitPacketUpdate.h"





#include "CWaitUIUpdate.h"





#include "Client.h"





#include "ClientCommunicationManager.h"





#include "DEServerInformation.h"





#include "ExperienceTable.h"





#include "GameObject.h"





#include "KeyAccelerator.h"





#include "MChatManager.h"





#include "MCompareManager.h"





#include "MEventManager.h"





#include "MGameStringTable.h"





#include "MGuildMarkManager.h"





#include "MHelpDisplayer.h"





#include "MHelpManager.h"





#include "MHelpStringTable.h"





#include "MJusticeAttackManager.h"





#include "MLevelNameTable.h"





#include "MMoneyManager.h"





#include "MMusic.h"





#include "MNPCScriptTable.h"





#include "MObjectSelector.h"





#include "MParty.h"





#include "MPriceManager.h"





#include "MStorage.h"





#include "MTradeManager.h"





#include "MZoneSoundManager.h"





#include "MathTable.h"





#include "ModifyStatusManager.h"





#include "MonsterNameTable.h"





#include "PacketDef.h"





#include "ProfileManager.h"





#include "Profiler.h"





#include "RequestClientPlayerManager.h"





#include "RequestFileManager.h"





#include "RequestServerPlayerManager.h"





#include "RequestUserManager.h"





#include "ServerInfo.h"





#include "SpriteIDDef.h"





#include "TempInformation.h"





#include "UIDialog.h"





#include "UIMessageManager.h"





#include "UserInformation.h"
#include "MilitarySlot.h"





#include "VS_UI.h"





#include "WavePackFileManager.h"





#include "WhisperManager.h"





#include "client_PCH.h"





#include <MMSystem.h>











// #include "MFileDef.h"





#include "BGMManager.h"





#include "CMP3.h"





#include "CSprite555.h"





#include "CSprite565.h"





#include "DebugInfo.h"





#include "FameInfo.h"





#include "MAttachOrbitEffect.h"





#include "MGuildInfoMapper.h"





#include "MMonsterKillQuestInfo.h"





#include "MTestDef.h"





#include "MTimeItemManager.h"





#include "MWarManager.H"





#include "MWorkThreadGlobal.h"





#include "PCConfigTable.h"





#include "Properties.h"





#include "RankBonusTable.h"





#include "ServerInfoFileParser.h"





#include "ShrineInfoManager.h"





#include "SoundSetting.h"





#include "SystemAvailabilities.h"





#include "UIFunction.h"

















#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)





#include "MPremiumGiveItemInfo.h"





#endif //__PREMIUM_GIVE_ITEM_UI











extern BYTE g_macAddress[6];





extern BOOL GetMacAddressFromSock();











// #include "CJpeg.h"





// #define	new			DEBUG_NEW











// ToT SafeDelete ToT 2003.5.11 by sonee











extern int g_Dimension;











BOOL g_bEnable3DHAL = TRUE;











//----------------------------------------------------------------------





// Title Loading - 2001.8.20 global - -;;





//----------------------------------------------------------------------





// CSpritePack		g_TitleSPK;





// CSpritePack		g_TitleAniSPK;





// int				g_TitleSpriteID = 0;





// int				g_TitleSpriteDarkness = 4;





bool g_bTitleLoading = false;





// CSprite*		g_pTitleLoadingSprite = NULL;











CSpritePack *g_pTitleLoadingSPK = NULL;











int g_TitleSpriteAlpha = 32;





// CDirectDrawSurface	g_TitleSurface;











// [Futec]





extern char g_FutecIP[20];





extern unsigned int g_FutecPort;











#ifdef OUTPUT_DEBUG





extern CRITICAL_SECTION g_Lock;





#endif











//----------------------------------------------------------------------





// AddonSPK   - 2001.8.20





//----------------------------------------------------------------------





TYPE_SPRITEID g_AddonSPKNum = 0;





const int g_AddonSPKLoadingTimes = 20;





bool g_AddonSPKLoaded[g_AddonSPKLoadingTimes] = {





    false,





};





int g_AddonSPKIndexFirst[g_AddonSPKLoadingTimes] = {





    0,





};





int g_AddonSPKIndexLast[g_AddonSPKLoadingTimes] = {





    0,





};





long g_AddonSPKIndexFP[g_AddonSPKLoadingTimes] = {





    0,





};





bool g_AddonSPKAllLoaded = false;











bool g_bHALAvailable = false;





extern bool g_bHALAvailable;











void StartTitleLoading();





void DrawTitleLoading();





void EndTitleLoading(bool SendLogin = false);





bool PrepareLoadingAddonSPK();





bool LoadingAddonSPK(bool bLoadingAll);











// #define	CLIENT_VERSION		"[TestVersion] : 2000.12.05.09"





//---------------------------------------------------------------------------





//  Start TitleLoading





//---------------------------------------------------------------------------





//  2001.8.20





//---------------------------------------------------------------------------





void StartTitleLoading() {











  g_bTitleLoading = true;











  if (g_pTitleLoadingSPK == NULL) {





    g_pTitleLoadingSPK = new CSpritePack;





    g_pTitleLoadingSPK->LoadFromFile(





        g_pFileDef->getProperty("FILE_SPRITE_UI").c_str());





  }





}











//---------------------------------------------------------------------------





// Draw TitleLoading





//---------------------------------------------------------------------------





// 2001.8.20





//---------------------------------------------------------------------------





void DrawTitleLoading() {
  int iSpriteID_CI = 0;











  bool bHighResolution = g_pUserInformation->iResolution_x > 800;





  int ConvX = 0;





  int ConvY = 0;











  if (bHighResolution) // 1024*768, 1280*960, 1280*1024





  {





    if (g_pUserInformation->iResolution_x <= 1024)





      iSpriteID_CI = SPRITEID_CI_1024_768;





    else {





      // 1280*720





      if (g_pUserInformation->iResolution_y < 960) {





        iSpriteID_CI = SPRITEID_CI_1280_720;





      }





      // 1280*960





      else if (g_pUserInformation->iResolution_y < 1024) {





        iSpriteID_CI = SPRITEID_CI_1280_960;





      }





      // 1280*1024





      else {





        iSpriteID_CI = SPRITEID_CI_1280_1024;





      }





    }





  } else





    iSpriteID_CI = SPRITEID_CI_800_600;











  if (g_bTitleLoading) {





    //--------------------------------------------------------





    // 2001.8.20





    //--------------------------------------------------------





    if (g_pTitleLoadingSPK != NULL) 
    {
        // [ANTIGRAVITY] Moved pointZero calculation inside NULL check to prevent 0xC0000005.
        // Also added check for sprite existence to avoid out-of-bounds access.
        if (iSpriteID_CI >= (int)g_pTitleLoadingSPK->GetSize())
        {
             CT_Done("DIAG: DrawTitleLoading - Invalid Sprite ID %d (Max: %d)", iSpriteID_CI, g_pTitleLoadingSPK->GetSize());
             return;
        }

        POINT pointZero = {
            (SURFACE_WIDTH - (*g_pTitleLoadingSPK)[iSpriteID_CI].GetWidth()) / 2,
            (SURFACE_HEIGHT - (*g_pTitleLoadingSPK)[iSpriteID_CI].GetHeight()) / 2};











      //			point.x = 400 -





      //(g_pTitleLoadingSprite->GetWidth()>>1); 			point.y = 460 -





      //(g_pTitleLoadingSprite->GetHeight()>>1);





      //





      CSpriteSurface *pSurface = NULL;





      //			if (!CDirect3D::IsHAL())





      if (g_TitleSpriteAlpha != 32) {





        pSurface = g_pLast;





      } else {





        pSurface = g_pLast;





      }











      // pSurface->FillSurface(0);





      POINT deliberationPoint = {





          SURFACE_WIDTH -





              (*g_pTitleLoadingSPK)[SPRITEID_CI_DELIBERATION].GetWidth() - 6,





          6};











      pSurface->Lock();











      if (g_TitleSpriteAlpha == 32) {





        pSurface->BltSprite(&pointZero, &(*g_pTitleLoadingSPK)[iSpriteID_CI]);





#if __CONTENTS(__USER_GRADE)











        pSurface->BltSprite(&deliberationPoint,





                            &(*g_pTitleLoadingSPK)[SPRITEID_CI_DELIBERATION]);





#endif //__USER_GRADE





      } else {





        pSurface->BltSpriteAlpha(&pointZero,





                                 &(*g_pTitleLoadingSPK)[iSpriteID_CI],





                                 g_TitleSpriteAlpha);





      }





      pSurface->Unlock();





    }











    if (g_TitleSpriteAlpha == 32)





      CDirectDraw::Flip();





  }





}











//---------------------------------------------------------------------------





// End TitleLoading





//---------------------------------------------------------------------------





void EndTitleLoading(bool SendLogin) {
  CT_Done("DIAG: EndTitleLoading called (SendLogin:%d)", SendLogin);





  //	g_TitleAniSPK.Release();





  //	g_TitleSPK.Release();





  //	g_TitleSpriteID = 0;





  //	g_TitleSpriteDarkness = 4;





  g_bTitleLoading = false;











  if (g_pTitleLoadingSPK != NULL) {





    delete g_pTitleLoadingSPK;





    g_pTitleLoadingSPK = NULL;





  }





  // 2004, 7, 15, sobeit modify start











  if (SendLogin && true == g_pUserInformation->IsAutoLogIn) {





    static LOGIN login;





    login.sz_id = new char[g_pUserInformation->UserID.GetLength() + 1];





    login.sz_password = new char[32 + 1];











    strcpy(login.sz_id, g_pUserInformation->UserID.GetString());





    strcpy(login.sz_password,





           g_pUserInformation->AutoLogInKeyValue.GetString());











    gpC_base->SendMessage(UI_LOGIN, 0, 0, (void *)&login);





  }





  // 2004, 7, 15, sobeit modify end





}











//---------------------------------------------------------------------------





// PrepareLoadingAddonSPK





//---------------------------------------------------------------------------





bool PrepareLoadingAddonSPK() {





  return true;











  return true;





}











//---------------------------------------------------------------------------





// LoadingAddonSPK





//---------------------------------------------------------------------------





bool LoadingAddonSPK(bool bLoadingAll) {





  return true;











  return true;





}











//---------------------------------------------------------------------------





// Init Volume





//---------------------------------------------------------------------------





BOOL InitVolume() {





  // Debug Message





  DEBUG_ADD("[ InitGame ]  Volume");











  //--------------------------------------------------------





  // Master volume





  //--------------------------------------------------------





  // g_pMasterVolume = (IVolume*)new CVolumeOutMaster;





  if (!g_pMasterVolume || !g_pMasterVolume->IsAvailable()) {





    // InitFail("Master Volume");





    //  Debug Message





    DEBUG_ADD("[Error] Volume : MasterVolume Failed!");





  }











  //--------------------------------------------------------





  // Wave volume





  //--------------------------------------------------------





  // g_pWaveVolume = (IVolume*)new CVolumeOutWave;





  if (!g_pWaveVolume || !g_pWaveVolume->IsAvailable()) {





    // InitFail("Wave Volume");





    //  Debug Message





    DEBUG_ADD("[Error] Volume : WaveVolume Failed!");





  }











  if (g_pMasterVolume != NULL) {





    g_MasterVolumeOriginal = g_pMasterVolume->GetCurrentVolume();





  }











  if (g_pWaveVolume != NULL) {





    g_WaveVolumeOriginal = g_pWaveVolume->GetCurrentVolume();





  }











  return TRUE;





}











//---------------------------------------------------------------------------





// Init Input





//---------------------------------------------------------------------------





BOOL InitInput() {





  // Debug Message





  DEBUG_ADD("[ InitGame ]  Input");











  if (g_pDXInput == NULL) {





    g_pDXInput = new CDirectInput;





  }











  if (!g_pDXInput->Init(g_hWnd, g_hInstance, CDirectInput::NONEXCLUSIVE)) {





    InitFail("DirectInput Error!");





    return false;





  }











  g_pDXInput->SetAcquire(true);











  return TRUE;





}











//---------------------------------------------------------------------------





// Surface





//---------------------------------------------------------------------------





BOOL InitSurface() {





  DEBUG_ADD("[ InitGame ]  Surface");











  //--------------------------------------------------------





  // Back Surface





  //--------------------------------------------------------





  if (g_pBack != NULL) {





    delete g_pBack;





  }





  g_pBack = new CSpriteSurface;





  bool bBackInitResult = g_pBack->InitBacksurface();
  CT_Done("DIAG: g_pBack->InitBacksurface Result=%d, g_pBack->GetSurface()=%p", bBackInitResult, (void*)g_pBack->GetSurface());











  //--------------------------------------------------------





  //--------------------------------------------------------











  // [ANTIGRAVITY] Moved g_pLast creation UP to ensure it exists before DrawTitleLoading uses it.
  if (g_pLast == NULL)
  {
      CT_Done("DIAG: Early creation of g_pLast surface for Splash screens");
      g_pLast = new CSpriteSurface;
      g_pLast->InitOffsurface(SURFACE_WIDTH, SURFACE_HEIGHT, DDSCAPS_SYSTEMMEMORY);
      // Fix #12 diagnostic (2026-05-14): c0000374 InitOffsurface ile StartTitleLoading
      // arasinda; SetTransparency/FillSurface arasi localize et.
      CT_Done("DIAG-12: pre g_pLast->SetTransparency");
      g_pLast->SetTransparency(0);
      CT_Done("DIAG-12: pre g_pLast->FillSurface");
      g_pLast->FillSurface(CDirectDraw::Color(0, 0, 0));
      CT_Done("DIAG-12: post g_pLast->FillSurface");
  }

  CT_Done("DIAG: Calling StartTitleLoading()");
  StartTitleLoading();

  CT_Done("DIAG: Calling DrawTitleLoading()");
  DrawTitleLoading();











  g_pBack->SetTransparency(0);





  g_pBack->FillSurface(CDirectDraw::Color(31, 31, 31));











  //--------------------------------------------------------





  //





  //--------------------------------------------------------





  DEBUG_ADD("[ InitGame ]  Surface - new g_pTopView");











  if (g_pTopView == NULL) {





    g_pTopView = new MTopView;











    if (!PrepareLoadingAddonSPK()) {





      return FALSE;





    }





  }











  {





    // [ANTIGRAVITY] Removed destructive delete of g_pLast. 
    // This surface is now persistent for the duration of the init/render cycle.
    g_pTopView->SetSurface(g_pLast);
    CT_Done("TRACE: TopView SetSurface done");





  }











  //--------------------------------------------------------





  //





  //--------------------------------------------------------





  if (g_pUserOption->UseGammaControl && g_pUserOption->GammaValue != 100) {





    CDirectDraw::SetGammaRamp(g_pUserOption->GammaValue);





  }











  //--------------------------------------------------------





  //                UI





  //--------------------------------------------------------





  DEBUG_ADD("[ InitGame ]  Surface - InitializeGL");











  InitializeGL(CDirectDraw::Get_BPP(), CDirectDraw::Get_Count_Rbit(),





               CDirectDraw::Get_Count_Gbit(), CDirectDraw::Get_Count_Bbit());
  CT_Done("TRACE: InitializeGL done");











  DEBUG_ADD("[ InitGame ]  Surface - Initialize Font");





  DEBUG_ADD("[ InitGame ]  Surface - UI");











  // ??????? ??? ?? loading





  LoadingAddonSPK(false);
  CT_Done("TRACE: LoadingAddonSPK done");





  DrawTitleLoading();
  CT_Done("TRACE-A: DrawTitleLoading returned OK");

  CT_Done("TRACE-B: About to call gC_vs_ui.Release");
  gC_vs_ui.Release();
  CT_Done("TRACE-C: gC_vs_ui.Release OK");











  DEBUG_ADD("[ InitGame ]  Surface - InitD3D");





  {





    CT_Done("TRACE-D: About to call gC_vs_ui.Init");
    gC_vs_ui.Init(g_pLast, UI_ResultReceiver);
    CT_Done("TRACE-E: gC_vs_ui.Init OK");
    CT_Done("TRACE: gC_vs_ui Init done");





  }











#if __CONTENTS(__MOVE_ZONE)





#ifdef __MOVE_ZONE_UI__





  gC_vs_ui.MoveZoneUi_On();





#else





  gC_vs_ui.MoveZoneUi_Off();





#endif //__MOVE_ZONE_UI__





#endif //__MOVE_ZONE











  //--------------------------------------------------------





  // 2001.8.20





  //--------------------------------------------------------





  DrawTitleLoading();











  //--------------------------------------------------------





  //





  //--------------------------------------------------------











  CIndexSprite::SetColorSet();











  //--------------------------------------------------------





  // View.





  //--------------------------------------------------------





  // g_pTopView->SetSurface(g_pLast);





  if (g_Mode == MODE_CHANGE_OPTION && !g_pTopView->Init()) {





    return FALSE;





  }











  DrawTitleLoading();





  //	EndTitleLoading();











  // g_pTopView->SetDarkBits(5);











  // ColorSet





  CIndexSprite::SetColorSet();











  //--------------------------------------------------------





  // Cursor Surfac





  //--------------------------------------------------------





  // Flip Surface, (32*32)





  if (CDirectDraw::IsFullscreen()) {





    if (g_pCursorSurface != NULL) {





      delete g_pCursorSurface;





      g_pCursorSurface = NULL;





    }











    // g_pCursorSurface = new CStorageSurface;





  }





  // g_pCursorSurface.Init(2, CURSOR_SURFACE_WIDTH, CURSOR_SURFACE_HEIGHT);





  DrawTitleLoading();
  CT_Done("TRACE: DrawTitleLoading done");











  return TRUE;





}











//---------------------------------------------------------------------------





// Init Sound





//---------------------------------------------------------------------------





BOOL InitSound() {





  // Debug Message





  DEBUG_ADD("[ InitGame ]  InitSound");











  //--------------------------------------------------------





  //





  //                 DirectSound





  //





  //--------------------------------------------------------











  if (g_DXSound.IsInit()) {





    DEBUG_ADD("[ InitGame ]  Already Init");











    return TRUE;





  } else {





    if (g_DXSound.Init(g_hWnd)) {











      int value = g_pUserOption->VolumeSound;











      g_DXSound.SetVolumeLimitFromSlider(value);











      BGMManager::Instance()->Init(g_DXSound.GetDS());





    } else {





      DEBUG_ADD("[Error] Init Sound Error");











      return FALSE;





    }











    //-----------------------------------------------------------





    // RAM





    //-----------------------------------------------------------





    MEMORYSTATUS ms;





    ZeroMemory(&ms, sizeof(MEMORYSTATUS));





    ms.dwLength = sizeof(MEMORYSTATUS);











    GlobalMemoryStatus(&ms);











    if (ms.dwTotalPhys >= 100 * 1024 * 1024) {











      g_pClientConfig->MAX_SOUNDPART = 100;





    }











    g_pSoundManager->Init(g_pSoundTable->GetSize(),





                          g_pClientConfig->MAX_SOUNDPART);











    return TRUE;





  }





}











//---------------------------------------------------------------------------





// Init Music





//---------------------------------------------------------------------------





BOOL InitMusic() {





  // Debug Message





  DEBUG_ADD("[ InitGame ]  Music");











  //--------------------------------------------------------





  //





  //                 Music





  //





  //--------------------------------------------------------





  // g_Music.Init(g_hWnd);











  g_Music.Init(g_hWnd);











  // int volume = g_Music.GetVolume();





  //			DEBUG_ADD_FORMAT("[MIDI] Volume in Init = %x", volume);











  int value = g_pUserOption->VolumeMusic;











  WORD volume = BGMManager::SliderValueToMidiWord(value);











  g_Music.SetVolume(volume);











  DEBUG_ADD("MP3 new");





#ifdef __USE_MP3__





  if (g_pMP3 == NULL) {





    g_pMP3 = new CMP3;











    DEBUG_ADD("MP3 new OK");





    int value = g_pUserOption->VolumeMusic;











    LONG volume = (LONG)BGMManager::SliderValueToMidiWord(value); // value*SOUND_DEGREE + SOUND_MIN;











    DEBUG_ADD("MP3 SetVolume1");





    g_pMP3->SetVolume(volume);





    DEBUG_ADD("MP3 SetVolume1 OK");





  }





#else





  if (g_DXSound.IsInit()) {











    BGMManager::Instance()->SetVolumeFromSlider(g_pUserOption->VolumeMusic);





  }





#endif











  return TRUE;





}











//---------------------------------------------------------------------------





// Init Draw





//---------------------------------------------------------------------------





BOOL InitDraw() {





  // Debug Message





  DEBUG_ADD("[ InitGame ]  Draw");











  //--------------------------------------------------------





  // ClientConfig





  //--------------------------------------------------------





  g_bHAL = (g_pUserOption->Use3DHAL) ? true : false;











  bool g_bUseIMEWindow = true;











  //--------------------------------------------------------





  //





  //                 DirectDraw





  //





  //--------------------------------------------------------





  // if (g_bHAL)





  if (g_bHAL) {





    if (g_bFullScreen) {





      CDirectDraw::Init(g_hWnd, SURFACE_WIDTH, SURFACE_HEIGHT,





                        CDirectDraw::FULLSCREEN, true, g_bUseIMEWindow);





    } else {





      CDirectDraw::Init(g_hWnd, SURFACE_WIDTH, SURFACE_HEIGHT,





                        CDirectDraw::WINDOWMODE, true, g_bUseIMEWindow);





    }











    //--------------------------------------------------------





    // Video Memory





    //--------------------------------------------------------





    DWORD dwTotal;





    DWORD dwFree;





    HRESULT hr = CDirectDraw::QueryAvailableVideoMemory(
        DDSCAPS_VIDEOMEMORY, &dwTotal, &dwFree);











    DEBUG_ADD_FORMAT(





        "[VidemoMemory from GetAvailableVidMem()] Before Init Draw = %d/%d",





        dwFree, dwTotal);











    //--------------------------------------------------------





    //





    //--------------------------------------------------------











    DDCAPS driverCaps;





    ZeroMemory(&driverCaps, sizeof(driverCaps));





    driverCaps.dwSize = sizeof(driverCaps);











    hr = CDirectDraw::QueryDeviceCaps(&driverCaps);





    if (hr != DD_OK) {





      InitFail("[Error] GetCaps to Get VidMem");





      return FALSE;





    }











#ifdef OUTPUT_DEBUG





    if (driverCaps.dwCaps2 & DDCAPS2_NONLOCALVIDMEMCAPS) {





      DEBUG_ADD_FORMAT("DDCAPS2_NONLOCALVIDMEMCAPS Enable");





    } else {





      DEBUG_ADD_FORMAT("DDCAPS2_NONLOCALVIDMEMCAPS Disable");





    }





#endif











    dwTotal = driverCaps.dwVidMemTotal;





    dwFree = driverCaps.dwVidMemFree;











    DEBUG_ADD_FORMAT("[VidemoMemory from GetDDCaps()] Before Init Draw = %d/%d",





                     dwFree, dwTotal);











    DEBUG_ADD("[ InitGame ]  Init Draw OK");











    BOOL enoughMemory = TRUE;











    //--------------------------------------------------------





    //





    //--------------------------------------------------------





    if (dwFree < 8388608) {





      enoughMemory = FALSE;





    }











    //--------------------------------------------------------





    //





    //--------------------------------------------------------





    bool bUse3D = (enoughMemory && g_bHAL && (CDirect3D::CheckHAL()));











    if (bUse3D) {





      DEBUG_ADD("[ InitGame ]  Init 3D");











      if (CDirect3D::Init()) {





        g_bEnable3DHAL = TRUE;











        D3DDEVICEDESC7 devDesc;











        // Phase 4 (D3D9 port): D3D9 mode'da CDirect3D::GetDevice() NULL doner
        // (3D layer henuz port edilmedi). NULL guard, devDesc default init.
        // Not: D3DDEVICEDESC7 struct'inda dwSize alani yok (D3DCAPS.H:387);
        // ZeroMemory yeterli — sonraki dwDevCaps/dpcTriCaps kullanimlari 0 doner.
        if (CDirect3D::GetDevice() != NULL) {
          CDirect3D::GetDevice()->GetCaps(&devDesc);
        } else {
          ZeroMemory(&devDesc, sizeof(devDesc));
        }











#ifdef OUTPUT_DEBUG





        if (devDesc.dwDevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM) {





          DEBUG_ADD_FORMAT("D3DDEVCAPS_TEXTURENONLOCALVIDMEM Enable");





        } else {





          DEBUG_ADD_FORMAT("D3DDEVCAPS_TEXTURENONLOCALVIDMEM Disable");





        }





#endif





      } else {





        bUse3D = false;





      }





      g_bHALAvailable = true;





    }











    //--------------------------------------------------------





    //





    //--------------------------------------------------------





    if (!bUse3D) {





      DEBUG_ADD("[ InitGame ]  3D Failed. Release Draw and Re-Init 2D");











      CDirectDraw::ReleaseAll();











      if (g_bFullScreen) {





        CDirectDraw::Init(g_hWnd, SURFACE_WIDTH, SURFACE_HEIGHT,





                          CDirectDraw::FULLSCREEN, false, g_bUseIMEWindow);





      } else {





        CDirectDraw::Init(g_hWnd, SURFACE_WIDTH, SURFACE_HEIGHT,





                          CDirectDraw::WINDOWMODE, false, g_bUseIMEWindow);





      }











      g_bEnable3DHAL = FALSE;





    }





  }





  //--------------------------------------------------------





  //





  // 3D





  //





  //--------------------------------------------------------





  else {





    DEBUG_ADD("[ InitGame ]  Init 2D");











    if (g_bFullScreen) {





      CDirectDraw::Init(g_hWnd, SURFACE_WIDTH, SURFACE_HEIGHT,





                        CDirectDraw::FULLSCREEN, false, g_bUseIMEWindow);





    } else {





      CDirectDraw::Init(g_hWnd, SURFACE_WIDTH, SURFACE_HEIGHT,





                        CDirectDraw::WINDOWMODE, false, g_bUseIMEWindow);





    }





  }











  CSpriteSurface::InitEffectTable();











  CDirectDrawSurface::SetGammaFunction();











  return TRUE;





}











// -----------------------------------------------------------------------------





// LoadingThreadProc





// -----------------------------------------------------------------------------











//-----------------------------------------------------------------------------





// Init Thread





//-----------------------------------------------------------------------------





BOOL InitThread() {





  // Debug Message





  DEBUG_ADD("[ InitGame ]  Thread");











  return TRUE;





}











//---------------------------------------------------------------------------





// update loop





//---------------------------------------------------------------------------





BOOL InitGameUpdate() {





  if (g_pCOpeningUpdate != NULL) {





    delete g_pCOpeningUpdate;





  }











  if (g_pCGameUpdate != NULL) {





    delete g_pCGameUpdate;





  }











  if (g_pCWaitPacketUpdate != NULL) {





    delete g_pCWaitPacketUpdate;





  }











  if (g_pCWaitUIUpdate != NULL) {





    delete g_pCWaitUIUpdate;





  }











  g_pCOpeningUpdate = new COpeningUpdate;





  g_pCGameUpdate = new CGameUpdate;





  g_pCWaitPacketUpdate = new CWaitPacketUpdate;





  g_pCWaitUIUpdate = new CWaitUIUpdate;











  return TRUE;





}











//---------------------------------------------------------------------------





// Surface





//---------------------------------------------------------------------------





BOOL InitGame() {
  CT_Done(">>> ENTER InitGame");













//---------------------------------------------------------------------





// Profiler





//---------------------------------------------------------------------





#ifdef OUTPUT_DEBUG





  if (g_pProfiler != NULL) {





    g_pProfiler->Release();





  } else {





    g_pProfiler = new Profiler;





  }





#endif











  //---------------------------------------------------------------------





  // MathTable





  //---------------------------------------------------------------------





  MathTable::FCreateSines();











  //---------------------------------------------------------------------





  // Effect Orbit Table





  //---------------------------------------------------------------------





  MAttachOrbitEffect::InitOrbitPosition();











  //---------------------------------------------------------------------





  // Init





  //---------------------------------------------------------------------





  if (g_pGameStringTable == NULL) {





    g_pGameStringTable = new MStringArray;





  }











  if (g_pSystemMessage == NULL) {





    g_pSystemMessage = new CMessageArray;





  }











  if (g_pGameMessage == NULL) {





    g_pGameMessage = new CMessageArray;





  }











  if (g_pHelpMessage == NULL) {





    g_pHelpMessage = new CMessageArray;





  }











  if (g_pNoticeMessage == NULL) {





    g_pNoticeMessage = new CMessageArray;





  }





  // 2008.07.17 zzi





  if (g_pGMMessage == NULL) {





    g_pGMMessage = new CMessageArray;





  }





#if __CONTENTS(__WORLDCHATING)





  if (g_pWorldChatingMessage == NULL) {





    g_pWorldChatingMessage = new CMessageArray;





  }











#endif // g_pWorldChatingMessage





  if (g_pEventMessage == NULL) {





    g_pEventMessage = new CMessageArray;





  }





  //-





  // 2004, 6, 18 sobeit add start - nick name table





  if (g_pNickNameStringTable == NULL) {





    g_pNickNameStringTable = new MStringArray;





  }







  //---------------------------------------------------------------------





  // nick name std::string table Loading





  //---------------------------------------------------------------------





  ivfstream





      gameStringTableTable; //(FILE_INFO_gameStringTable, std::ios::binary);

  CT_Done("DIAG: Reached NICKNAME loading block");







  // --- DIAGNOSTIC: Wrap in try-catch to detect hidden exceptions ---



  {



    bool nickFileOK = false;
    try {
      std::string nickPath = g_pFileDef->getProperty("FILE_INFO_NICKNAME_STRING");
      nickFileOK = FileOpenBinary(nickPath.c_str(), gameStringTableTable);
      
      if (!nickFileOK) {
        CT_Done("InitGame FAIL: NICKNAME_STRING (nickFileOK) is FALSE!");
        return FALSE;



      }





    } catch (const std::exception& e) {



      char exBuf[512];



      wsprintfA(exBuf, "DIAG EXCEPTION in NICKNAME_STRING: %s", e.what());



      MessageBox(NULL, exBuf, "InitGame EXCEPTION", MB_OK | MB_ICONERROR);



      return FALSE;



    } catch (...) {
      CT_Done("InitGame FAIL: Caught unknown exception in early init block!");
      return FALSE;



    }





    (*g_pNickNameStringTable).LoadFromFile_NickNameString(gameStringTableTable);





    gameStringTableTable.close();



  }









  // 2004, 6, 18 sobeit add end - nick name table





  //---------------------------------------------------------------------





  // GameStringTable Loading





  //---------------------------------------------------------------------





  ivfstream





      gameStringTableTable2; //(FILE_INFO_gameStringTable, std::ios::binary);







  {



    bool strFileOK = false;



    try {



      std::string strPath = g_pFileDef->getProperty("FILE_INFO_STRING");



      char msgBuf2[512];



      wsprintfA(msgBuf2, "DIAG: getProperty OK for STRING, path=[%s]", strPath.c_str());



      CT_Done("InitGame Diagnostic: %s", msgBuf2);



      



      
      CT_Done("PRE: Calling FileOpenBinary...");
      
      CT_Done("PRE: Calling FileOpenBinary...");
      strFileOK = FileOpenBinary(strPath.c_str(), gameStringTableTable2);
      CT_Done("POST: FileOpen Result = %d", (int)strFileOK);
      
      CT_Done("POST: FileOpen Result = %d", (int)strFileOK);
      



      if (!strFileOK) {
        CT_Done("InitGame FAIL: COMMON_STRING (strFileOK) is FALSE!");
        return FALSE;



      }



    } catch (const std::exception& e) {



      char exBuf2[512];



      wsprintfA(exBuf2, "DIAG EXCEPTION in STRING: %s", e.what());



      MessageBox(NULL, exBuf2, "InitGame EXCEPTION", MB_OK | MB_ICONERROR);



      return FALSE;



    } catch (...) {
      CT_Done("InitGame FAIL: Caught unknown exception in early init block!");
      return FALSE;



    }



    (*g_pGameStringTable).LoadFromFile(gameStringTableTable2);



    gameStringTableTable2.close();



  }









  //---------------------------------------------------





  // System Message





  //---------------------------------------------------





  g_pSystemMessage->Init(MAX_SYSTEMMESSAGE, 256);











  g_pNoticeMessage->Init(7, 256);











  //---------------------------------------------------





  // Game Message





  //---------------------------------------------------





  g_pGameMessage->Init(MAX_GAMEMESSAGE, 256);











  //---------------------------------------------------





  // Help Message





  //---------------------------------------------------





  g_pHelpMessage->Init(MAX_HELPMESSAGE, 256);











  //---------------------------------------------------





  // GM & Special Message - 2008.07.17 zzi





  //---------------------------------------------------





  g_pGMMessage->Init(MAX_GMMESSAGE, 256);





  g_pEventMessage->Init(MAX_EVENTMESSAGE, 256);





#if __CONTENTS(__WORLDCHATING)





  g_pWorldChatingMessage->Init(MAX_GMMESSAGE, 256);





#endif //__WORLDCHATING











  // Debug Message





  DEBUG_ADD("---------------[   InitGame   ]---------------");











  //----------------------------------------------------------------------





  // WSA Startup





  //----------------------------------------------------------------------





  DEBUG_ADD("[ InitGame ]  Socket - Before WSAStartup");











  WORD wVersionRequested;





  WSADATA wsaData;











  wVersionRequested = MAKEWORD(2, 0);











  if (WSAStartup(wVersionRequested, &wsaData) != 0) {





    // Tell the user that we couldn't find a useable





    // WinSock DLL.





    MessageBox(g_hWnd, "couldn't find a useable WinSock DLL.", NULL, MB_OK);





    return FALSE;





  }











  //--------------------------------------------------------





  //





  //                 Init...





  //





  //--------------------------------------------------------







// --- DIAGNOSTIC CHAIN REPLACEMENT ---

  CT_Done("TRACE: Calling InitClientConfig..."); bool bInitConfig = InitClientConfig(); CT_Done("TRACE: InitClientConfig returned %d", (int)bInitConfig);
  if(!bInitConfig) return FALSE;



  CT_Done("TRACE: Calling InitInfomation..."); bool bInitInfo = InitInfomation(); CT_Done("TRACE: InitInfomation returned %d", (int)bInitInfo);
  if(!bInitInfo) return FALSE;



  CT_Done("TRACE: Calling InitUserOption..."); bool bInitOption = InitUserOption(); CT_Done("TRACE: InitUserOption returned %d", bInitOption);

  if(!bInitOption) return FALSE;



  CT_Done("TRACE: Calling InitVolume..."); bool bInitVolume = InitVolume(); CT_Done("TRACE: InitVolume returned %d", bInitVolume);

  if(!bInitVolume) return FALSE;



  CT_Done("TRACE: Calling InitInput..."); bool bInitInput = InitInput(); CT_Done("TRACE: InitInput returned %d", bInitInput);

  if(!bInitInput) return FALSE;



  CT_Done("TRACE: Calling InitDraw..."); bool bInitDraw = InitDraw(); CT_Done("TRACE: InitDraw returned %d", bInitDraw);

  if(!bInitDraw) return FALSE;



  CT_Done("TRACE: Calling InitThread..."); bool bInitThread = InitThread(); CT_Done("TRACE: InitThread returned %d", bInitThread);

  if(!bInitThread) return FALSE;



  bool bInitSurface = InitSurface();
  CT_Done("TRACE: InitSurface returned");

  if(!bInitSurface) { CT_Done("TRACE: InitSurface FAILED"); return FALSE; }
  CT_Done("TRACE: InitSurface OK, about to InitGameObject");



  CT_Done("TRACE: Calling InitGameObject...");
  bool bInitGameObject = InitGameObject();
  CT_Done("TRACE: InitGameObject returned");

  if(!bInitGameObject) return FALSE;



  bool bInitGameUpdate = InitGameUpdate();

  if(!bInitGameUpdate) return FALSE;





  if (bInitConfig && bInitInfo && bInitOption && bInitVolume && bInitInput &&

      bInitDraw && bInitThread && bInitSurface && bInitGameObject && bInitGameUpdate) {





    CT_Done("TRACE: Calling UI_AffectUserOption");
  UI_AffectUserOption();
  CT_Done("TRACE: UI_AffectUserOption done");





    //----------------------------------------





    // guild ??? ?????��?





    //----------------------------------------











    //----------------------------------------





    // ?? profile ????





    //----------------------------------------





    CT_Done("DIAG: Before DeleteProfiles");
    // BYPASSED: ProfileManager::DeleteProfiles();
    CT_Done("DIAG: After DeleteProfiles");





    ProfileManager::InitProfiles();











    CT_Done("DIAG: Before SetMode");
    SetMode(MODE_MAINMENU);
    CT_Done("DIAG: InitGame returning TRUE");











// Debug Message





#ifdef OUTPUT_DEBUG





    // Fullscreen?





    if (CDirectDraw::IsFullscreen()) {





      DEBUG_ADD("# Fullscreen.");





    } else {





      DEBUG_ADD("# Windowed Mode.");





    }











    // 565?





    if (CDirectDraw::Is565()) {





      DEBUG_ADD("# 5:6:5 mode");





    } else {





      DEBUG_ADD("# 5:5:5 mode or ???.");





    }











    // 3D HAL?





    if (CDirect3D::IsHAL()) {





      DEBUG_ADD("# 3D Hardware Accel.");





    }











    DEBUG_ADD("---------------[  InitGame OK  ]---------------");





#endif











    return TRUE;





  }











  // Init?? ?????? ???





  return FALSE;





}











//-----------------------------------------------------------------------------





// Init Socket





//-----------------------------------------------------------------------------





BOOL InitSocket() {
  PhoenixLoginSocketTrace("InitSocket: enter g_pSocket=%p", (void*)g_pSocket);











  // ??? release





  PhoenixLoginSocketTrace("InitSocket: before ReleaseSocket g_pSocket=%p", (void*)g_pSocket);
  ReleaseSocket();
  PhoenixLoginSocketTrace("InitSocket: after ReleaseSocket g_pSocket=%p", (void*)g_pSocket);











  // Debug Message





  DEBUG_ADD("[ InitGame ]  Socket");











  //----------------------------------------------------------------------





  // ??? ?????? ?��???��?.





  // ???? ?????? $VSHOME/bin??, ??? ?????? $VSHOME/conf ?? ??????? ???.?





  // // command line ???? ??? ?????? ?????? ?? ????? ???.





  //----------------------------------------------------------------------











  try {





    DEBUG_ADD("[ InitGame ]  Socket - Before new Properties");











    std::string configFilename;











    if (!g_pUserInformation->bKorean) {





      configFilename = g_pFileDef->getProperty("FILE_INFO_GAMECLIENT").c_str();











      g_pConfigForeign = new Properties();





      g_pConfigForeign->load(configFilename);





    } else {





      switch (g_Dimension) {











      case 1:





        configFilename =





            g_pFileDef->getProperty("FILE_INFO_GAMECLIENT2").c_str();





        break;











      default:





        configFilename =





            g_pFileDef->getProperty("FILE_INFO_GAMECLIENT").c_str();





        break;





      }











      g_pConfigKorean = new Properties();





      g_pConfigKorean->load(configFilename);





    }











  } catch (Throwable &t) {
    PhoenixLoginSocketTrace("InitSocket: config load exception %s", t.toString().c_str());











    DEBUG_ADD_ERR(t.toString().c_str());











    CT_Done("DIAG: Before SetMode");
    SetMode(MODE_MAINMENU);
    CT_Done("DIAG: InitGame returning TRUE");





    UpdateDisconnected();











    return FALSE;





  }











  //----------------------------------------------------------------------





  // Create Socket





  //----------------------------------------------------------------------





  Socket *pSocket = NULL;





  try {





    DEBUG_ADD("[ InitGame ]  Socket - Before new PacketFactoryManager");











    g_pPacketFactoryManager = new PacketFactoryManager();











    DEBUG_ADD("[ InitGame ]  Socket - Before new PacketValidator");











    g_pPacketValidator = new PacketValidator();











    DEBUG_ADD("[ InitGame ]  Socket - Before Init...");











    g_pPacketFactoryManager->init();





    g_pPacketValidator->init();











    DEBUG_ADD("[ InitGame ]  Socket - Before new Socket...");











    //---------------------------------------------------------





    // Address ??? ???????





    //---------------------------------------------------------





    int maxAddress = 1;











    try {





      if (g_pUserInformation->bKorean)





        maxAddress =





            atoi(g_pConfigKorean->getProperty("MaxLoginServerAddress").c_str());





      else





        maxAddress = atoi(





            g_pConfigForeign->getProperty("MaxLoginServerAddress").c_str());





    } catch (NoSuchElementException) {





    }











    static int previousTryServer = 0;











    {











      int i = previousTryServer % maxAddress;











      try {





        std::string serverAddressString;





        serverAddressString = "LoginServerAddress";











        uint port;





        if (g_pUserInformation->bKorean)





          port = g_pConfigKorean->getPropertyInt("LoginServerPort");





        else





          port = g_pConfigForeign->getPropertyInt("LoginServerPort");











        std::string ServerAddress;





        if (g_FutecPort == 0) {





          if (i != 0) {





            char str[10];





            sprintf(str, "%d", i);





            serverAddressString += str;





          }











          if (g_pUserInformation->bKorean)





            ServerAddress = g_pConfigKorean->getProperty(serverAddressString);





          else





            ServerAddress = g_pConfigForeign->getProperty(serverAddressString);











          try {





            int portNum;





            if (g_pUserInformation->bKorean)





              portNum = g_pConfigKorean->getPropertyInt("LoginServerPortNum");





            else





              portNum = g_pConfigForeign->getPropertyInt("LoginServerPortNum");











            // port ??????





            if (portNum > 1) {





              if (g_pUserInformation->bKorean)





                port = g_pConfigKorean->getPropertyInt("LoginServerBasePort") +





                       rand() % portNum;





              else





                port = g_pConfigForeign->getPropertyInt("LoginServerBasePort") +





                       rand() % portNum;





            }





          } catch (NoSuchElementException &) {





          }





        }











        else {





          ServerAddress = g_FutecIP;





          port = g_FutecPort;





        }











        if (ServerAddress[0] < '0' || ServerAddress[0] > '9') {





          struct hostent *h;











          if ((h = gethostbyname(ServerAddress.c_str())) == NULL) {





            // -_-;





            throw ConnectException("The address of the LoginServer cannot be found.");





          }











          char *pIP = (char *)inet_ntoa(*((struct in_addr *)h->h_addr));











          ServerAddress = pIP;





        }











        DEBUG_ADD_FORMAT("Conneting( %s : %d )", ServerAddress.c_str(), port);
        PhoenixLoginSocketTrace("InitSocket: connecting address=%s port=%u maxAddress=%d tryIndex=%d",
                                ServerAddress.c_str(), port, maxAddress, i);





        pSocket = new Socket(ServerAddress, port);











        DEBUG_ADD("[ InitGame ]  Socket - Before connect");
        CT_Done("DIAG: Socket - Starting non-blocking connect attempt...");











        // try to connect to server
        // Referans akisi: once blocking connect tamamlanir, sonra non-blocking moda gecilir.
        pSocket->connect();
        pSocket->setNonBlocking();
        PhoenixLoginSocketTrace("InitSocket: connect returned pSocket=%p", (void*)pSocket);











      } catch (ConnectException) {











        if (pSocket != NULL) {





          delete pSocket;





          pSocket = NULL;





        }











        previousTryServer++;











        throw;





      }





    }











    // Non-blocking mode is already enabled immediately after a successful connect.











    // make no-linger socket





    pSocket->setLinger(0);











    DEBUG_ADD("[ InitGame ]  Socket - new ClientPlayer");











    // create player





    PhoenixLoginSocketTrace("InitSocket: before new ClientPlayer pSocket=%p", (void*)pSocket);
    g_pSocket = new ClientPlayer(pSocket);
    PhoenixLoginSocketTrace("InitSocket: after new ClientPlayer g_pSocket=%p", (void*)g_pSocket);





    pSocket = NULL;











  } catch (Throwable &t) {
    PhoenixLoginSocketTrace("InitSocket: socket setup exception %s", t.toString().c_str());











    DEBUG_ADD(t.toString().c_str());











    if (pSocket != NULL) {





      delete pSocket;





    }











    CT_Done("DIAG: Before SetMode");
    SetMode(MODE_MAINMENU);
    CT_Done("DIAG: InitGame returning TRUE");





    UpdateDisconnected();











    return FALSE;





  }











  //----------------------------------------------------------------------





  // RequestServerPlayerManager





  //----------------------------------------------------------------------





  PhoenixLoginSocketTrace("InitSocket: socket ready before manager cleanup g_pSocket=%p", (void*)g_pSocket);

  if (g_pProfileManager != NULL) {





    g_pProfileManager->ReleaseRequire();





  }











  if (g_pRequestServerPlayerManager != NULL) {





    DEBUG_ADD("[ InitGame ]  delete RequestServerPlayerManager");





    delete g_pRequestServerPlayerManager;





  }











  if (g_pRequestClientPlayerManager != NULL) {





    DEBUG_ADD("[ InitGame ]  delete RequestClientPlayerManager");





    delete g_pRequestClientPlayerManager;





  }











  if (g_pClientCommunicationManager != NULL) {





    DEBUG_ADD("[ InitGame ]  delete ClientCommunicationManager");





    delete g_pClientCommunicationManager;





  }











  if (g_pRequestUserManager != NULL) {





    DEBUG_ADD("[ InitGame ]  delete g_pRequestUserManager");





    delete g_pRequestUserManager;





  }











  if (g_pWhisperManager != NULL) {





    DEBUG_ADD("[ InitGame ]  delete g_pWhisperManager");





    delete g_pWhisperManager;





  }











  if (g_pRequestFileManager != NULL) {





    DEBUG_ADD("[ InitGame ]  delete g_pRequestFileManager");





    delete g_pRequestFileManager;





  }











  DEBUG_ADD("[ InitGame ] new ClientCommunicationManager");





  g_pClientCommunicationManager = new ClientCommunicationManager;











#ifdef __METROTECH_TEST__





  g_UDPTest.Init();





#endif











  if (g_pClientConfig->MAX_REQUEST_SERVICE > 0) {





    DEBUG_ADD("[ InitGame ] new RequestServerPlayerManager");





    g_pRequestServerPlayerManager = new RequestServerPlayerManager;











    DEBUG_ADD("[ InitGame ] new RequestClientPlayerManager");





    g_pRequestClientPlayerManager = new RequestClientPlayerManager;











    DEBUG_ADD("[ InitGame ] RequestServerPlayerManager Init");





    g_pRequestServerPlayerManager->Init();





  }











  DEBUG_ADD("[ InitGame ] new g_pRequestUserManager");





  g_pRequestUserManager = new RequestUserManager;











  DEBUG_ADD("[ InitGame ] new g_pWhisperManager");





  g_pWhisperManager = new WhisperManager;











  DEBUG_ADD("[ InitGame ] new g_pRequestFileManager");





  g_pRequestFileManager = new RequestFileManager;











  DEBUG_ADD("[ InitGame ]  Socket OK");
  PhoenixLoginSocketTrace("InitSocket: return TRUE g_pSocket=%p", (void*)g_pSocket);











  return TRUE;





}











//-----------------------------------------------------------------------------





// ReleaseGameUpdate()





//-----------------------------------------------------------------------------





void ReleaseGameUpdate() {





  if (g_pCOpeningUpdate != NULL) {





    delete g_pCOpeningUpdate;





    g_pCOpeningUpdate = NULL;





  }











  if (g_pCGameUpdate != NULL) {





    delete g_pCGameUpdate;





    g_pCGameUpdate = NULL;





  }











  if (g_pCWaitPacketUpdate != NULL) {





    delete g_pCWaitPacketUpdate;





    g_pCWaitPacketUpdate = NULL;





  }











  if (g_pCWaitUIUpdate != NULL) {





    delete g_pCWaitUIUpdate;





    g_pCWaitUIUpdate = NULL;





  }





}











//-----------------------------------------------------------------------------





// Name: ReleaseAllObjects()





// Desc: Finished with all objects we use; release them





//-----------------------------------------------------------------------------





void ReleaseAllObjects() {





  DEBUG_ADD("---------------[ Start ReleaseAllObjects ]---------------");











  //----------------------------------------------------------------





  //





  //				 thread





  //





  //----------------------------------------------------------------











  DEBUG_ADD("[Release] Work Thread");











  g_bActiveGame = false;











  EndTitleLoading();











  //----------------------------------------------------------------





  // GameUpdate ????





  //





  // update loop ????? ??? ??????? ????.





  //----------------------------------------------------------------





  ReleaseGameUpdate();











#ifndef __USE_MP3__











  BGMManager::Instance()->Destory();





#endif











  //----------------------------------------------------------------





  //





  //		Global ???? ????





  //





  //----------------------------------------------------------------





  ReleasePacketItemTable();











  //----------------------------------------------------------------





  // UI ????





  //----------------------------------------------------------------





  if (g_pUIDialog != NULL) {





    DEBUG_ADD("[Release] UI Dialog");





    g_pUIDialog->Release();





  }











  DEBUG_ADD("[Release] UI");





  CT_Done("TRACE: gC_vs_ui Release PRE");
  gC_vs_ui.Release();
  CT_Done("TRACE: gC_vs_ui Release POST");











  //----------------------------------------------------------------





  //





  //					Game???? ????





  //





  //----------------------------------------------------------------











  //----------------------------------------------------------------





  // Weather





  //----------------------------------------------------------------





  DEBUG_ADD("[Release] Weather");





  if (g_pWeather != NULL) {





    delete g_pWeather;





    g_pWeather = NULL;





  }











  //----------------------------------------------------------------





  // EffectManager





  //----------------------------------------------------------------





  DEBUG_ADD("[Release] Inventory Effect Manager");





  if (g_pInventoryEffectManager != NULL) {





    delete g_pInventoryEffectManager;





    g_pInventoryEffectManager = NULL;





  }











  //----------------------------------------------------------------





  // Player ????





  //----------------------------------------------------------------





  DEBUG_ADD("[Release] g_pPlayer");





  if (g_pPlayer != NULL) {





    if (g_pZone != NULL) {





      g_pZone->RemoveCreature(g_pPlayer->GetID());





    }





    SAFE_DELETE(g_pPlayer);





  }











  //----------------------------------------------------------------





  // Zone





  //----------------------------------------------------------------





  DEBUG_ADD("[Release] Zone");





  SAFE_DELETE(g_pZoneSmall);





  SAFE_DELETE(g_pZoneLarge);





  g_pZone = NULL;











  DEBUG_ADD("[Release] MCreature::MoveTable");





  MCreature::ReleaseMoveTable();











  DEBUG_ADD("[Release] DirectInput");











  //----------------------------------------------------------------





  //





  //				System	& Util..





  //





  //----------------------------------------------------------------





  //----------------------------------------------------------------





  // Input





  //----------------------------------------------------------------





  SAFE_DELETE(g_pDXInput);





  //----------------------------------------------------------------





  // Music





  //----------------------------------------------------------------





  DEBUG_ADD("[Release] Music");





  g_Music.Stop();





  g_Music.UnInit();





  // g_DXMusic.Stop();











  //----------------------------------------------------------------





  // Sound





  //----------------------------------------------------------------











  DEBUG_ADD("[Release] MP3");





  SAFE_DELETE(g_pMP3);





  DEBUG_ADD("[Release] ZoneSound");











  SAFE_DELETE(g_pZoneSoundTable);





  SAFE_DELETE(g_pZoneSoundManager);





  SAFE_DELETE(g_pWavePackFileManager);











  DEBUG_ADD("[Release] Sound");











  UnInitSound();











  //----------------------------------------------------------------





  // Avi





  //----------------------------------------------------------------





  DEBUG_ADD("[Release] AVI");











  if (g_pAvi != NULL) {





    g_pAvi->Close();





    delete g_pAvi;





    g_pAvi = NULL;





  }











  //----------------------------------------------------------------





  // UnInit DirectInput





  //----------------------------------------------------------------











  //----------------------------------------------------------------





  // View





  //----------------------------------------------------------------





  DEBUG_ADD("[Release] MTopView");











  if (g_pTopView != NULL) {











    {





      g_pTopView->Release();





      delete g_pTopView;





    }











    g_pTopView = NULL;





  }











  //----------------------------------------------------------------





  // Cursor Surface





  //----------------------------------------------------------------





  DEBUG_ADD("[Release] g_pCursorSurface");





  SAFE_DELETE(g_pCursorSurface);











  //----------------------------------------------------------------





  // delete Surfaces





  //----------------------------------------------------------------





  DEBUG_ADD("[Release] Back Surface");





  SAFE_DELETE(g_pBack);











  DEBUG_ADD("[Release] Last Surface");





  SAFE_DELETE(g_pLast);











  //----------------------------------------------------------------





  // Socket





  //----------------------------------------------------------------





  DEBUG_ADD("[Release] Socket");





  ReleaseSocket();











  //----------------------------------------------------------------





  // ??..





  //----------------------------------------------------------------





  WSACleanup();











  //----------------------------------------------------------------





  // Volume





  //----------------------------------------------------------------





  DEBUG_ADD("[Release] Volume");





  if (g_pWaveVolume != NULL) {





    g_pWaveVolume->SetCurrentVolume(g_WaveVolumeOriginal);





    g_pWaveVolume->Done();











    delete g_pWaveVolume;





    g_pWaveVolume = NULL;





  }











  if (g_pMasterVolume != NULL) {





    g_pMasterVolume->SetCurrentVolume(g_MasterVolumeOriginal);





    g_pMasterVolume->Done();











    delete g_pMasterVolume;





    g_pMasterVolume = NULL;





  }











  //----------------------------------------------------------------





  // global ????...





  //----------------------------------------------------------------





  // if (g_bNeedUpdate)





  {





    if (g_pActionInfoTable)





      g_pActionInfoTable->Release();





    if (g_pCreatureSpriteTable != NULL)





      g_pCreatureSpriteTable->Release();





    //		if (g_pAddonSpriteTable!=NULL) g_pAddonSpriteTable->Release();





    //		if (g_pCreatureActionSpriteTable!=NULL)





    //g_pCreatureActionSpriteTable->Release();





    if (g_pCreatureTable)





      g_pCreatureTable->Release();





    if (g_pCreatureSpriteTypeMapper)





      g_pCreatureSpriteTypeMapper->Release();





    if (g_pEffectSpriteTypeTable)





      g_pEffectSpriteTypeTable->Release();





    if (g_pActionEffectSpriteTypeTable)





      g_pActionEffectSpriteTypeTable->Release();





    if (g_pEffectStatusTable)





      g_pEffectStatusTable->Release();





    if (g_pGameMessage != NULL)





      g_pGameMessage->Release();





    if (g_pGameStringTable)





      g_pGameStringTable->Release();





    if (g_pInteractionObjectTable)





      g_pInteractionObjectTable->Release();





    if (g_pItemOptionTable)





      g_pItemOptionTable->Release();





    if (g_pNPCTable)





      g_pNPCTable->Release();





    if (g_pNPCTalkBox)





      g_pNPCTalkBox->Release();





    if (g_pPCTalkBox)





      g_pPCTalkBox->Release();





    if (g_pShopTemplateTable)





      g_pShopTemplateTable->Release();





    if (g_pSkillAvailable)





      g_pSkillAvailable->clear();





    if (g_pSkillInfoTable)





      g_pSkillInfoTable->Release();





    if (g_pSkillManager)





      g_pSkillManager->Release();





    if (g_pSlayerGear)





      g_pSlayerGear->Release();





    if (g_pVampireGear)





      g_pVampireGear->Release();





    if (g_pOustersGear)





      g_pOustersGear->Release();





    if (g_pInventory)





      g_pInventory->Release();





    if (g_pSoundManager)





      g_pSoundManager->Release();





    if (g_pSoundTable)





      g_pSoundTable->Release();





    if (g_pSystemMessage)





      g_pSystemMessage->Release();





    if (g_pUsingColorSetTable)





      g_pUsingColorSetTable->Release();





    if (g_pZoneTable)





      g_pZoneTable->Release();





    if (g_pZoneSoundManager)





      g_pZoneSoundManager->Release();





    if (g_pNPCScriptTable)





      g_pNPCScriptTable->Release();











    // 2004, 06, 18, sobeit add start - nick name table





    if (g_pNickNameStringTable != NULL)





      g_pNickNameStringTable->Release();





    // 2004, 06, 18, sobeit add end - nick name table











    // 2008.07.17 - zzi





    if (g_pGMMessage)





      g_pGMMessage->Release();





    if (g_pEventMessage)





      g_pEventMessage->Release();





#if __CONTENTS(__WORLDCHATING)





    if (g_pWorldChatingMessage)





      g_pWorldChatingMessage->Release();





#endif //__WORLDCHATING





  }











  SAFE_DELETE(g_pRegenTowerInfoManager);





  SAFE_DELETE(g_pSystemAvailableManager);





  SAFE_DELETE(g_pWorldPlayerConfigTable);





  SAFE_DELETE(g_pServerInformation);





  SAFE_DELETE(g_pUserInformation);





  SAFE_DELETE(g_pTempInformation);





  SAFE_DELETE(g_pGameMessage);





  SAFE_DELETE(g_pHelpMessage);





  SAFE_DELETE(g_pNoticeMessage);





  SAFE_DELETE(g_pSystemMessage);





  SAFE_DELETE(g_pGameStringTable);





  SAFE_DELETE(g_pClientConfig);





  SAFE_DELETE(g_pUserOption);





  SAFE_DELETE(g_pOperatorOption);





  SAFE_DELETE(g_pUIDialog);





  SAFE_DELETE(g_pProfileManager);





  SAFE_DELETE(g_pGuildMarkManager);





  SAFE_DELETE(g_pGuildInfoMapper);





  SAFE_DELETE(g_pEventManager);





  SAFE_DELETE(g_pEffectSpriteTypeTable);





  SAFE_DELETE(g_pActionEffectSpriteTypeTable);





  SAFE_DELETE(g_pCreatureTable);





  SAFE_DELETE(g_pCreatureSpriteTypeMapper);





  SAFE_DELETE(g_pItemTable);





  SAFE_DELETE(g_pCreatureSpriteTable);





  //	SAFE_DELETE( g_pAddonSpriteTable );





  //	SAFE_DELETE( g_pCreatureActionSpriteTable );





  SAFE_DELETE(g_pEffectStatusTable);





  SAFE_DELETE(g_pInteractionObjectTable);





  SAFE_DELETE(g_pItemOptionTable);





  SAFE_DELETE(g_pNPCTable);





  SAFE_DELETE(g_pJusticeAttackManager);





  SAFE_DELETE(g_pNPCScriptTable);





  SAFE_DELETE(g_pShopTemplateTable);





#if __CONTENTS(__GPS_ADD) // MGPS_INFO





  SAFE_DELETE(g_pGpsInfoTable);





#endif // MGPS_INFO





  SAFE_DELETE(g_pSkillInfoTable);





  SAFE_DELETE(g_pRankBonusTable);





  SAFE_DELETE(g_pExperienceTable);





  SAFE_DELETE(g_pSoundTable);





  SAFE_DELETE(g_pMusicTable);





  SAFE_DELETE(g_pZoneTable);





  SAFE_DELETE(g_pLevelNameTable);





  SAFE_DELETE(g_pQuestInfoManager);





  SAFE_DELETE(g_pSoundManager);





  SAFE_DELETE(g_pSkillAvailable);





  SAFE_DELETE(g_pSkillManager);





  SAFE_DELETE(g_pSlayerGear);





  SAFE_DELETE(g_pVampireGear);





  SAFE_DELETE(g_pOustersGear);





  SAFE_DELETE(g_pInventory);





  SAFE_DELETE(g_pParty);





  SAFE_DELETE(g_pTradeManager);





  SAFE_DELETE(g_pStorage);





  SAFE_DELETE(g_pStorage2);





  SAFE_DELETE(g_pGameTime);





  SAFE_DELETE(g_pKeyAccelerator);





  SAFE_DELETE(g_pAcceleratorManager);





  SAFE_DELETE(g_pModifyStatusManager);





  SAFE_DELETE(g_pEffectGeneratorTable);





  SAFE_DELETE(g_pUsingColorSetTable);





  SAFE_DELETE(g_pActionInfoTable);





  SAFE_DELETE(g_pPriceManager);





  SAFE_DELETE(g_pMoneyManager);





  SAFE_DELETE(g_pPCTalkBox);





  SAFE_DELETE(g_pNPCTalkBox);





  SAFE_DELETE(g_pChatManager);





  SAFE_DELETE(g_pHelpManager);





  SAFE_DELETE(g_pHelpStringTable);











  //	SAFE_DELETE( g_pCompareManager );











  // ???? ???? ???? ???? ?????? 2005. 04.13 ???





  // Start





  SAFE_DELETE(g_pCompareManager);





  // ???? ???? ???? ???? ?????? 2005. 04.13 ???





  // End











  SAFE_DELETE(g_pHelpDisplayer);





  SAFE_DELETE(g_pObjectSelector);





  SAFE_DELETE(g_pUIMessageManager);





  SAFE_DELETE(g_pMonsterNameTable);





  SAFE_DELETE(g_pQuestInfoManager);





  SAFE_DELETE(g_pTimeItemManager);





  SAFE_DELETE(g_pFameInfoTable);





  SAFE_DELETE(g_pWarManager);











  // 2004, 6, 18, sobeit add start





  SAFE_DELETE(g_pNickNameStringTable);





  // 2004, 6, 18, sobeit add end











  // 2008.07.17 - zzi





  SAFE_DELETE(g_pGMMessage);





  SAFE_DELETE(g_pEventMessage);











#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)





  SAFE_DELETE(g_pPremiumGiveItemInfoList);





#endif //__PREMIUM_GIVE_ITEM_UI





#if __CONTENTS(__WORLDCHATING)





  SAFE_DELETE(g_pWorldChatingMessage);





#endif //__WORLDCHATING











  //----------------------------------------------------------------





  // DirectDraw





  //----------------------------------------------------------------





  DEBUG_ADD("[Release] CDirect3D");











  CDirect3D::Release();











  DEBUG_ADD("[Release] CDirectDraw");











  CDirectDraw::ReleaseAll();











  //---------------------------------------------------------------------





  // Profiler





  //---------------------------------------------------------------------





  DEBUG_ADD("[Release] Profiler");





  SAFE_DELETE(g_pProfiler);





  DEBUG_ADD("[Release] All OK");











  SAFE_DELETE(g_pFileDef);





}











//-----------------------------------------------------------------------------





// Release Socket





//-----------------------------------------------------------------------------





void ReleaseSocket() {
  PhoenixLoginSocketTrace("ReleaseSocket: enter g_pSocket=%p", (void*)g_pSocket);





  //----------------------------------------------------------------





  //





  //							socket





  //





  //----------------------------------------------------------------





  DEBUG_ADD("----- Start ReleaseSocket -----");











  if (g_pSocket != NULL) {
    PhoenixLoginSocketTrace("ReleaseSocket: before disconnect g_pSocket=%p", (void*)g_pSocket);





    // g_pSocket->setPlayerStatus( CPS_END_SESSION );











    g_pSocket->disconnect(UNDISCONNECTED);
    PhoenixLoginSocketTrace("ReleaseSocket: after disconnect g_pSocket=%p", (void*)g_pSocket);











    // g_pSocket->getSocket()->close();





    DEBUG_ADD("delete g_pSocket");











    PhoenixLoginSocketTrace("ReleaseSocket: before delete g_pSocket=%p", (void*)g_pSocket);
    delete g_pSocket;





    g_pSocket = NULL;
    PhoenixLoginSocketTrace("ReleaseSocket: after delete g_pSocket=NULL");





  }











  SAFE_DELETE(g_pConfigKorean);





  SAFE_DELETE(g_pConfigForeign);











  if (g_pPacketFactoryManager != NULL) {





    DEBUG_ADD("delete g_pPacketFactoryManager");











    delete g_pPacketFactoryManager;





    g_pPacketFactoryManager = NULL;





  }











  if (g_pPacketValidator != NULL) {





    DEBUG_ADD("delete g_pPacketValidator");











    delete g_pPacketValidator;





    g_pPacketValidator = NULL;





  }











  if (g_pProfileManager != NULL) {





    g_pProfileManager->ReleaseRequire();





  }











  if (g_pRequestServerPlayerManager != NULL) {





    DEBUG_ADD("delete g_pRequestServerPlayerManager");





    delete g_pRequestServerPlayerManager;





    g_pRequestServerPlayerManager = NULL;





  }











  if (g_pRequestClientPlayerManager != NULL) {





    DEBUG_ADD("delete g_pRequestClientPlayerManager");





    delete g_pRequestClientPlayerManager;





    g_pRequestClientPlayerManager = NULL;





  }











  if (g_pClientCommunicationManager != NULL) {





    DEBUG_ADD("delete g_pClientCommunicationManager");





    delete g_pClientCommunicationManager;





    g_pClientCommunicationManager = NULL;





  }











  if (g_pWhisperManager != NULL) {





    DEBUG_ADD("delete g_pWhisperManager");





    delete g_pWhisperManager;





    g_pWhisperManager = NULL;





  }











  if (g_pRequestFileManager != NULL) {





    DEBUG_ADD("delete g_pRequestFileManager");





    delete g_pRequestFileManager;





    g_pRequestFileManager = NULL;





  }











  if (g_pRequestUserManager != NULL) {





    DEBUG_ADD("delete g_pRequestUserManager");





    delete g_pRequestUserManager;





    g_pRequestUserManager = NULL;





  }











  DEBUG_ADD("----- End ReleaseSocket -----");





}











//-----------------------------------------------------------------------------





// UnInit Sound





//-----------------------------------------------------------------------------





void UnInitSound() {





  //-------------------------------------





  // UnInit DirectSound





  //-------------------------------------





  if (g_pSoundManager != NULL) {





    g_pSoundManager->Release();





  }











  g_DXSound.Release();





}











//-----------------------------------------------------------------------------





// Init GameObject





//-----------------------------------------------------------------------------





BOOL InitGameObject() {
  CT_Done("IGO: Entry");





  DEBUG_ADD("[ InitGame ]  GameObject");











#if __CONTENTS(__CRYPT)





//	if(g_pCryptManager == NULL)





//	{





//		g_pCryptManager = new CCryptManager;





//		g_pCryptManager->InitCryptManager();





//	}





#endif





  //----------------------------------------





  //





  //----------------------------------------





  CT_Done("IGO: SystemAvailMgr");
  if (g_pSystemAvailableManager == NULL) {





    g_pSystemAvailableManager = new SystemAvailabilitiesManager;





    g_pSystemAvailableManager->LoadFromFile(





        g_pFileDef->getProperty("FILE_FILTER_INFO").c_str());





  }





  CT_Done("IGO: ServerInfo");
  if (g_pServerInformation == NULL) {





    g_pServerInformation = new DEServerInformation;





    g_pServerInformation->SetServerGroupName("ServerGroup");





    g_pServerInformation->SetServerName("Server");





  }











  CT_Done("IGO: UserInfo");
  if (g_pUserInformation == NULL) {





    g_pUserInformation = new UserInformation;





  }











  if (g_pTempInformation == NULL) {





    g_pTempInformation = new TempInformation;





  }











  CT_Done("IGO: SkillMgr");
  if (g_pSkillManager == NULL) {





    g_pSkillManager = new MSkillManager;





  }











  if (g_pSkillAvailable == NULL) {





    g_pSkillAvailable = new MSkillSet;





  }











  if (g_pSlayerGear == NULL) {





    g_pSlayerGear = new MSlayerGear;





  }











  if (g_pVampireGear == NULL) {





    g_pVampireGear = new MVampireGear;





  }











  if (g_pOustersGear == NULL) {





    g_pOustersGear = new MOustersGear;





  }











  if (g_pInventory == NULL) {





    g_pInventory = new MInventory;





  }

  // FAZ 4 (2026-04-28): MilitarySlot init eksikti - g_pMilitarySlot->Init NULL deref.
  if (g_pMilitarySlot == NULL) {
    g_pMilitarySlot = new MilitarySlot;
  }











  if (g_pParty == NULL) {





    g_pParty = new MParty;





  } else {





    g_pParty->Release();





  }











  // if (g_pTradeManager==NULL)





  //{





  //	g_pTradeManager = new MTradeManager;





  // }











  if (g_pGameTime == NULL) {





    g_pGameTime = new MGameTime;





  }











  if (g_pPriceManager == NULL) {





    g_pPriceManager = new MPriceManager;





  }











  CT_Done("IGO: MoneyMgr");
  if (g_pMoneyManager == NULL) {





    g_pMoneyManager = new MMoneyManager;





  }











  if (g_pPCTalkBox == NULL) {





    g_pPCTalkBox = new PCTalkBox;





  }











  if (g_pNPCTalkBox == NULL) {





    g_pNPCTalkBox = new MStringList;





  }











  CT_Done("IGO: UIMsgMgr");
  if (g_pUIMessageManager == NULL) {





    g_pUIMessageManager = new UIMessageManager;





    g_pUIMessageManager->Init();





  }











  if (g_pObjectSelector == NULL) {





    g_pObjectSelector = new MObjectSelector;





  }











#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)





  if (g_pPremiumGiveItemInfoList == NULL) {





    g_pPremiumGiveItemInfoList = new MPremiumGiveItemInfoList;





  }





#endif //__PREMIUM_GIVE_ITEM_UI











  //----------------------------------------





  // Weather





  //----------------------------------------





  if (g_pWeather != NULL) {





    delete g_pWeather;





  }





  CT_Done("IGO: Weather");
  g_pWeather = new MWeather;











  //----------------------------------------





  // ScreenEffectManager





  //----------------------------------------





  if (g_pInventoryEffectManager != NULL) {





    delete g_pInventoryEffectManager;





  }





  g_pInventoryEffectManager = new MScreenEffectManager;











  //----------------------------------------





  // Player ????





  //----------------------------------------





  if (g_pPlayer != NULL) {





    if (g_pZone != NULL) {





      g_pZone->RemoveCreature(g_pPlayer->GetID());





    }











    delete g_pPlayer;





  }











  // Player ???? ????





  CT_Done("IGO: new MPlayer");
  
  CT_Done("IGO: MPlayer Safety Check...");
  CT_Done(" - sizeof(MPlayer) = %d", sizeof(MPlayer));
  CT_Done(" - g_pEffectSpriteTypeTable = %p", g_pEffectSpriteTypeTable);
  CT_Done(" - g_pEffectStatusTable = %p", g_pEffectStatusTable);

  if (g_pEffectSpriteTypeTable == NULL) {
      CT_Done("WARNING: g_pEffectSpriteTypeTable is NULL! This will cause crash in MCreature!");
  }
  if (g_pEffectStatusTable == NULL) {
      CT_Done("WARNING: g_pEffectStatusTable is NULL! This will cause crash in MCreature!");
  }

  try {
      CT_Done("IGO: Executing -> new MPlayer");
      g_pPlayer = new MPlayer;
      CT_Done("IGO: new MPlayer SUCCESS");
  } catch (...) {
      CT_Done("ERROR: C++ Exception caught during new MPlayer!");
      return FALSE;
  }

  CT_Done("IGO: new MPlayer allocated OK, ptr=%p", g_pPlayer);
  CT_Done("IGO: new MPlayer OK");





  DEBUG_ADD_FORMAT("[g_pPlayer] %x", g_pPlayer);











  CT_Done("IGO: about to SetCreatureType");
  CT_Done("IGO: Calling SetCreatureType (Internal Armor Enabled)...");
    if (g_pPlayer != NULL) g_pPlayer->SetCreatureType(CREATURETYPE_SLAYER_MALE);
    CT_Done("IGO: SetCreatureType Call Finished");











  //--------------------------------------------------





  //





  // Skill Tree ????





  //





  //--------------------------------------------------





  CT_Done("IGO: SkillMgr Init");
  try {
    CT_Done("IGO: SkillMgr ptr check: %p", g_pSkillManager);
    if (g_pSkillManager != NULL) {
      g_pSkillManager->Init();
      CT_Done("IGO: SkillMgr->Init RETURNED");
    } else CT_Done("ERROR: g_pSkillManager is NULL!");
  } catch(...) { CT_Done("CRASH: Caught Exception in SkillMgr->Init!"); }











  return TRUE;





}






