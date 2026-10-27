//----------------------------------------------------------------------
//
// Filename    : GCUpdateInfoHandler.cpp
// Written By  : Reiot
// Description : 
//
//----------------------------------------------------------------------

// include files
#include "client_PCH.h"
#include "crash_trace_tool.h"
#include "frame_perf_probe.h"
#include "MTopView.h"
#include "GCUpdateInfo.h"
#include "ClientDef.h"
#include "AddonDef.h"
#include <new>
#include <Windows.h>
#include "MGameTime.h"
#include "MQuickSlot.h"
#include "MEffectStatusDef.h"
#include "MSkillManager.h"
#include "MMoneyManager.h"
#include "CDirectSound.h"
#include "COrderedList.h"
#include "MZoneTable.h"
#include "UIDialog.h"
#include "TalkBox.h"
#include "MWeather.h"
#include "MStorage.h"
//#include "MFileDef.h"
#include "Properties.h"
#include "DEServerInformation.h"
#include "UserInformation.h"
#include "MEventManager.h"
#include "UIMessageManager.h"
#include "ServerInfo.h"
#include "ClientPlayer.h"
#include "UIFunction.h"
#include "MGuildInfoMapper.h"
#include "CSystemInfo.h"
#include "EffectSpriteTypeDef.h"

extern "C" void Phoenix_FatalDiagLogf(const char* format, ...);
extern void Phoenix_EmergencyMemoryReclaim(const char* optionalMapPath);

// Hop arasi ~160MB buyumenin hangi GCUI asamasinda oldugunu gormek icin.
static void Phoenix_GCUI_LogMem(const char *tag, int zoneId) {
	MEMORYSTATUSEX ms{};
	ms.dwLength = sizeof(ms);
	GlobalMemoryStatusEx(&ms);
	Phoenix_FatalDiagLogf(
		"mem gcui-%s zone=%d availVirt=%llu",
		tag != NULL ? tag : "?", zoneId,
		static_cast<unsigned long long>(ms.ullAvailVirtual));
}
#include "VS_UI.h"

#ifdef __GAME_CLIENT__

	#include "ClientPlayer.h"
	#include "PCSlayerInfo2.h"
	#include "PCVampireInfo2.h"
	#include "PCOustersInfo2.h"
	#include "Cpackets/CGReady.h"
	#include "VS_UI_GameCommon2.h"
	#include "MGameStringTable.h"
	#include "MEffectSpriteTypeTable.h"
#endif



extern DWORD		g_CurrentTime;
extern DWORD		g_CurrentFrame;
extern MGameTime*	g_pGameTime;
extern CMessageArray*			g_pNoticeMessage;
extern HWND			g_hWnd;
extern BOOL			g_bActiveApp;
extern BOOL			g_bActiveGame;

extern int			g_nZoneLarge;
extern MZone*		g_pZoneLarge;				
extern bool			g_bZonePlayerInLarge;

extern bool			LoadingAddonSPK( bool bLoadingAll );
extern bool				g_bHALAvailable;
extern DWORD			g_dwVideoMemory;
extern BOOL g_bEnable3DHAL;
extern WORD				g_ZoneCreatureColorSet;


DWORD	g_PreviousCreatureType = -1;
int		g_MorphCreatureType = 0;

//----------------------------------------------------------------------
// Ŭ���̾�Ʈ�� ���� �����κ��� GCUpdateInfo ��Ŷ�� �ް� �Ǹ�,
// ��Ŷ ���� �����͵��� Ŭ���̾�Ʈ�� ������ ��, ������ �ε���
// ���� ���� ���� ������ CGReady ��Ŷ�� ������ �ȴ�.
//----------------------------------------------------------------------
void GCUpdateInfoHandler::execute ( GCUpdateInfo * pPacket , Player * pPlayer )

{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__

	Phoenix_FrameProbeStageScope fpGCUpdateInfoTotal(
		"GCUpdateInfo.Total",
		g_hWnd,
		(int)g_bActiveApp,
		(int)g_bActiveGame);

	CT_Done("[GCUI] EXECUTE ENTER zoneID=%d", (int)pPacket->getZoneID());

	#ifndef __DEBUG_OUTPUT__
		DEBUG_ADD("Execute UpdateInfo");
	#endif

	// FAZ 4 KAYIT (2026-05-02): Önceki agresif early return GCUpdateInfo
	// karakter spawn sequence'ini kırdı, mode wait state'te kaldı.
	// Skip GERİ ALINDI — orijinal flow devam, MItem method çağrıları
	// crash potansiyeli (Brief G/B-FIX kalıcı fix gerek).


	// Thread Loading�� �����Ѵ�.
// 	MWorkThreadGlobal::Instance()->StopThread();
// 	MWorkThreadGlobal::Instance()->SetPriority(WORKTHREAD_PRIORITY_MAIN_LOADING);
// 	g_pTopView->ClearWaitToLoadSet();
	
	// EventManager�� Zone���� �̺�Ʈ�� �����Ѵ�
	{
		Phoenix_FrameProbeStageScope fpStage(
			"GCUpdateInfo.RemoveZoneEvents",
			g_hWnd,
			(int)g_bActiveApp,
			(int)g_bActiveGame);
		g_pEventManager->RemoveAllEventByType(EVENTTYPE_ZONE);
	}

	//-----------------------------------------------------------
	// �ٸ� �κп��� mode�� �����ϴ� ��찡 �־
	// mode�� �������ش�.
	//-----------------------------------------------------------
	CLIENT_MODE	previousMode = g_Mode;
	g_Mode = MODE_WAIT_UPDATEINFO;

	//UI_SaveUserOption();

	//-----------------------------------------------------------
	// ���� server�� ���� ǥ��
	//-----------------------------------------------------------
	if (g_pServerInformation!=NULL)
	{
		g_pServerInformation->SetServerGroupStatus( pPacket->getServerStat() );
	}

	//-----------------------------------------------------------
	// AddonSPK�� ���� �ε��ϴµ�..
	// ���⼭�� loading �ȵ� �κ��� �� loading�ؾ� �Ѵ�. 2001.8.20
	//-----------------------------------------------------------
	//LoadingAddonSPK( true );

//	g_pUserOption->LoadFromFile( FILE_INFO_USEROPTION );
//	UI_AffectUserOption();
	

	int i;

	//-----------------------------------------------------------
	// Sound�� ������� �ʴ´�.
	//-----------------------------------------------------------
	{
		Phoenix_FrameProbeStageScope fpStage(
			"GCUpdateInfo.SoundMute",
			g_hWnd,
			(int)g_bActiveApp,
			(int)g_bActiveGame);
		g_DXSound.SetMute();
	}

	//-----------------------------------------------------------
	// updateInfo�� ó���ϱ� �����Ҷ��� �ð��� �����Ѵ�.
	//-----------------------------------------------------------
	DWORD	startTime	= timeGetTime();
	
	ClientPlayer * pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);

	if (pClientPlayer == NULL)
	{
		DEBUG_ADD("[Error] dynamic_cast<ClientPlayer*> failed");
		return;
	}

	const int gcuiZoneId = (int)pPacket->getZoneID();
	Phoenix_GCUI_LogMem("enter", gcuiZoneId);

	//--------------------------------------------------
	// ������ �ε��� �����Ѵ�.
	//--------------------------------------------------
	pClientPlayer->setPlayerStatus( CPS_WAITING_FOR_LOADING );

	
	//--------------------------------------------------
	//
	//			 Player ���� ����
	//
	//--------------------------------------------------
	Race wasInterfaceRace = UI_GetInterfaceRace();//TRUE;
	int spActionInfo = ACTIONINFO_NULL;
	g_PreviousCreatureType = -1;

	bool bIsAdvancementClass = false;

	bool backupbInvincible = true;

	if (g_pPlayer!=NULL)
	{	
		bIsAdvancementClass = g_pPlayer->IsAdvancementClass();
		
		if (g_MorphCreatureType!=0)
		{
			g_PreviousCreatureType = g_MorphCreatureType;
			g_MorphCreatureType = 0;
		}
		else
		{
			g_PreviousCreatureType = g_pPlayer->GetCreatureType();
		}

		//--------------------------------------------------
		// interface ���濡 ���Ͽ�..
		//--------------------------------------------------
		/*
		if (g_pPlayer->IsVampire())
		{
			wasSlayerInterface = FALSE;
		}
		*/
		// �׾�����, dialog �� �ݱ�
		if (g_pPlayer->IsDead())
		{
			//UI_CloseAllDialog();
		}
		

		//--------------------------------------------------
		// ������ ������̴� ��� ����..
		//--------------------------------------------------
		spActionInfo = g_pPlayer->GetSpecialActionInfo();
		if (g_pZone!=NULL)
		{
			g_pZone->RemoveCreature( g_pPlayer->GetID() );
		}
		backupbInvincible = g_pPlayer->GetInvincible();//20070409 by dieaice iddqd���� ����
		delete g_pPlayer;
	}	
	g_pPlayer = new MPlayer;

	//20070409 by dieaice iddqd���� ����
	g_pPlayer->SetInvincible(backupbInvincible);
	DEBUG_ADD_FORMAT("[g_pPlayer] %x", g_pPlayer);
	
	//gC_vs_ui.CloseTimeCount();

	//--------------------------------------------------
	//
	//		���� �ʱ�ȭ
	//
	//--------------------------------------------------
	DEBUG_ADD( "Remove All Addon" );
	
	for (i=0; i<ADDON_MAX; i++)
	{
		g_pPlayer->RemoveAddon((enum ADDON)i);
	}			

	g_pPlayer->SetBasicActionInfo( SKILL_ATTACK_MELEE );
	g_pPlayer->SetBasicAttackDistance( 1 );
		


	//--------------------------------------------------
	//
	//	Skill Info Table 초기화
	//
	//--------------------------------------------------
	DEBUG_ADD( "SkillInfoTable Init" );
	{
		MEMORYSTATUSEX ms{};
		ms.dwLength = sizeof(ms);
		GlobalMemoryStatusEx(&ms);
		// Dump: SkillManager::Init → ProtectedFile vector resize bad_alloc
		// (availVirt ~9MB, WS ~1.7GB). Zone load'dan ONCE reclaim.
		if (ms.ullAvailVirtual < (128ULL * 1024ULL * 1024ULL)) {
			Phoenix_FatalDiagLogf(
				"GCUI pre-skill reclaim availVirt=%llu",
				static_cast<unsigned long long>(ms.ullAvailVirtual));
			Phoenix_EmergencyMemoryReclaim(NULL);
		}
	}

	try {
		g_pSkillInfoTable->Init();
	} catch (const std::bad_alloc &) {
		Phoenix_FatalDiagLogf("GCUI SkillInfoTable::Init bad_alloc — reclaim+retry");
		Phoenix_EmergencyMemoryReclaim(NULL);
		try {
			g_pSkillInfoTable->Init();
		} catch (const std::bad_alloc &) {
			Phoenix_FatalDiagLogf("soft fail: SkillInfoTable::Init OOM -> MODE_QUIT");
			g_bLastLoadZoneWasOOM = true;
			g_DXSound.UnSetMute();
			UI_EndProgress();
			if (g_pSystemMessage != NULL)
				g_pSystemMessage->Add("Map load failed: not enough memory.");
			SetMode(MODE_QUIT);
			return;
		}
	}

	//--------------------------------------------------
	//
	// Skill Tree 초기화
	//
	//--------------------------------------------------
	DEBUG_ADD( "SkillManager Init" );
	try {
		g_pSkillManager->Init();
	} catch (const std::bad_alloc &) {
		Phoenix_FatalDiagLogf("GCUI SkillManager::Init bad_alloc — reclaim+retry");
		Phoenix_EmergencyMemoryReclaim(NULL);
		try {
			g_pSkillManager->Init();
		} catch (const std::bad_alloc &) {
			Phoenix_FatalDiagLogf("soft fail: SkillManager::Init OOM -> MODE_QUIT");
			g_bLastLoadZoneWasOOM = true;
			g_DXSound.UnSetMute();
			UI_EndProgress();
			if (g_pSystemMessage != NULL)
				g_pSystemMessage->Add("Map load failed: not enough memory.");
			SetMode(MODE_QUIT);
			return;
		}
	}

	Phoenix_GCUI_LogMem("post-skill-init", gcuiZoneId);

	g_PreviousCreatureType = -1;
	g_MorphCreatureType = 0;

	EffectInfo *pEInfo = new EffectInfo;
	*pEInfo = *pPacket->getEffectInfo();
	// release build mismatch korumasi — sadece PCType uygunsa gecerli, dynamic_cast NULL doner
	PCVampireInfo2 *VampireInfo = dynamic_cast<PCVampireInfo2*>(pPacket->getPCInfo());
	PCOustersInfo2 *OustersInfo = dynamic_cast<PCOustersInfo2*>(pPacket->getPCInfo());
	
	if( pEInfo != NULL )
	{
		for(int i = 0 ; i < pEInfo->getListNum(); i++)
		{
			const std::pair<WORD,DWORD> Element		= pEInfo->popFrontListElement();
			const EFFECTSTATUS			status		= (EFFECTSTATUS)Element.first;
			const DWORD					delay		= Element.second;
			
			if( status == EFFECTSTATUS_TRANSFORM_TO_BAT )
			{
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
				if (VampireInfo != NULL && 2 == VampireInfo->GetBatType())
#if __CONTENTS(__SECOND_TRANSFORTER)
				{
					if (VampireInfo->getWingItemType()==1)
						g_PreviousCreatureType = CREATURETYPE_SHAPE_OF_DEMON;
					else if(VampireInfo->getWingItemType()==0)
#endif //__SECOND_TRANSFORTER
						g_PreviousCreatureType = CREATURETYPE_FLITTERMOUSE;
#if __CONTENTS(__SECOND_TRANSFORTER)
				}
#endif //__SECOND_TRANSFORTER
				else
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
				if(bIsAdvancementClass)
					g_PreviousCreatureType = CREATURETYPE_VAMPIRE_GHOST;
				else
					g_PreviousCreatureType = CREATURETYPE_BAT;
			} else
			if( status == EFFECTSTATUS_TRANSFORM_TO_WOLF )
			{
				g_PreviousCreatureType = CREATURETYPE_WOLF;
			} else
			if( status == EFFECTSTATUS_TRANSFORM_TO_WERWOLF )
			{
				g_PreviousCreatureType = CREATURETYPE_WER_WOLF;
			}
		}
		delete pEInfo;
	}
/*if(pInfo->getWingSylphType() == 2)
	{
		g_pPlayer->SetWingEffect1();
	}*/

	g_pGuildInfoMapper->ClearGuildName();

	//--------------------------------------------------
	//
	//			 player�� ������ �����Ѵ�.
	//
	//--------------------------------------------------		
	{
		Phoenix_FrameProbeStageScope fpStage(
			"GCUpdateInfo.SetPCInfo",
			g_hWnd,
			(int)g_bActiveApp,
			(int)g_bActiveGame);

		switch (pPacket->getPCInfo()->getPCType())
		{
			//--------------------------------------------------
			// Slayer�� ���
			//--------------------------------------------------
			case PC_SLAYER :
			{
				// release build mismatch korumasi
				PCSlayerInfo2* pSlayerInfo = dynamic_cast<PCSlayerInfo2*>(pPacket->getPCInfo());
				if (pSlayerInfo == NULL)
				{
					DEBUG_ADD("[Error] dynamic_cast<PCSlayerInfo2*> failed");
					break;
				}
				SetPCSlayerInfo( pSlayerInfo );

				if (wasInterfaceRace != RACE_SLAYER)
				{
					UI_ChangeInterfaceRace(RACE_SLAYER);
				}
			}
			break;

			//--------------------------------------------------
			// Vampire�� ���
			//--------------------------------------------------
			case PC_VAMPIRE :
			{
				if (VampireInfo == NULL)
				{
					DEBUG_ADD("[Error] dynamic_cast<PCVampireInfo2*> failed");
					break;
				}
				SetPCVampireInfo( VampireInfo );

				if (wasInterfaceRace != RACE_VAMPIRE)
				{
					UI_ChangeInterfaceRace(RACE_VAMPIRE);
				}
			}
			break;

			//--------------------------------------------------
			// Ousters�� ���
			//--------------------------------------------------
			case PC_OUSTERS :
			{
				if (OustersInfo == NULL)
				{
					DEBUG_ADD("[Error] dynamic_cast<PCOustersInfo2*> failed");
					break;
				}
				SetPCOustersInfo( OustersInfo );

				if (wasInterfaceRace != RACE_OUSTERS)
				{
					UI_ChangeInterfaceRace(RACE_OUSTERS);
				}
			}
			break;
		}
	}
	// 2004, 6, 4 sobeit add start - sms charge
	g_char_slot_ingame.m_SMS_Charge		= pPacket->GetSMSCharge();
	// 2004, 6, 4 sobeit add end - sms charge
	
	// 2004, 6, 15 sobeit add end - nick name
	NicknameInfo* TempNick	= pPacket->getNicknameInfo();
	const char* szNickName;
	if(TempNick != NULL)
	{
		if(TempNick->getNicknameType() == NicknameInfo::NICK_NONE)
		{
			szNickName = "";
		}
		else 
		if(TempNick->getNicknameType() == NicknameInfo::NICK_CUSTOM_FORCED ||
			TempNick->getNicknameType() == NicknameInfo::NICK_CUSTOM)
		{
			szNickName = (TempNick->getNickname()).c_str();
		}
		else // �г��� �ε����� ���� ��
		{
			szNickName = (*g_pNickNameStringTable)[TempNick->getNicknameIndex()].GetString();
		}

		g_pPlayer->SetNickName(TempNick->getNicknameType(), (char*)szNickName);
//		g_char_slot_ingame.m_NickNameType = TempNick->getNicknameType();
//		g_char_slot_ingame.m_NickName = szNickName;
	}
	// 2004, 6, 15 sobeit add end - nick name

	// 2004, 9, 15, sobeit add start - non pk ����
	g_pUserInformation->IsNonPK = (pPacket->isNonPK())?true:false;
	// 2004, 9, 15, sobeit add end - non pk ����
	
	// 2004, 10, 18, sobeit add start - ���Ͽ� ��� ����
	g_pUserInformation->dwUnionID = pPacket->getGuildUnionID();
	g_pUserInformation->bUnionGrade = pPacket->getGuildUnionUserType();
	g_pPlayer->SetUnionGuildID(g_pUserInformation->dwUnionID);
	// 2004, 10, 18, sobeit add end
	
	// 2004, 12, 2, sobeit add start �Ŀ�¯ ����Ʈ 
	g_char_slot_ingame.m_Powerjjang_Point		= pPacket->GetPowerjjang_Point();
	// 2004, 12, 2, sobeit add end

	
	//--------------------------------------------------
	// ������ �����ص״� actionInfo�� ����
	//--------------------------------------------------
	g_pPlayer->SetSpecialActionInfo( spActionInfo );
	
	
	//--------------------------------------------------
	// ���� ��� ����
	//--------------------------------------------------
	g_pPlayer->SetAttackModeNormal();



	//-----------------------------------------------------------
	// ������ �����.
	//-----------------------------------------------------------
	if (g_pStorage!=NULL)
	{
		delete g_pStorage;
		g_pStorage = NULL;		
	}

	//-----------------------------------------------------------
	// ���� ����~~
	//-----------------------------------------------------------
	UI_UnlockGear();
	UI_UnlockItem();
	UI_UnlockItemTrade();
	UI_CloseShop();
	UI_CloseBookcase();
	UI_CloseExchange();
	UI_CloseExchangeAsk();
	UI_CloseExchangeCancel();
	UI_CloseRepair();
	UI_CloseSell();
	UI_CloseStorage();
	UI_ClosePartyCancel();
	UI_ClosePartyAsk();
	UI_ClosePartyRequest();
	UI_ClosePartyCancel();
	g_pUIMessageManager->Execute(UI_CLOSE_TRACE_WINDOW, 0, 0, NULL);
//	UI_CloseSubInventory(); // subInventory�� ����..������ ���� ����..
	UI_ClearSubInventory();

	if (g_pUIDialog!=NULL)
	{
		g_pUIDialog->ClosePCTalkDlg();
		g_pUIDialog->CloseMessageDlg();
//		g_pUIDialog->CloseHelpDlg();
	}

	if (g_pPCTalkBox!=NULL)
	{
		g_pPCTalkBox->Release();
	}
		

	//--------------------------------------------------
	// QuickSlot�� ���ش�. �ʿ��Ѱ�? - -;
	//--------------------------------------------------	
	g_pQuickSlot = NULL;
	g_pArmsBand1 = NULL;
	g_pArmsBand2 = NULL;

	Phoenix_GCUI_LogMem("pre-progress", gcuiZoneId);			
	UI_StartProgress( pPacket->getZoneID() );
	Phoenix_GCUI_LogMem("post-progress", gcuiZoneId);

	//UI_SetCurrentServerName(g_pServerInformation->GetServerGroupName(), 
	//						g_pServerInformation->GetServerName(), 
	//						g_pServerInformation->GetServerStatus());

	UI_DrawProgress(0);

	DEBUG_ADD("Init g_pTopView");
	
	//------------------------------
	// �ʱ�ȭ ���� �ʾ����� �ʱ�ȭ�Ѵ�.
	//------------------------------
	if (!g_pTopView->IsInit())
	{
		g_pTopView->Init();			
	}

	UI_DrawProgress( 15 );
	
	//--------------------------------------------------
	//
	// Zone�� ȯ�� ������ �����Ѵ�.
	//
	//--------------------------------------------------
	DEBUG_ADD("[UpdateInfo] Set Weather");
	
	//--------------------------------------------------
	// Zone�̵��Ҷ� �ٸ� ������ �ٲ�� �����..
	// ������ ������ �����Ѵ�.
	//--------------------------------------------------
	if (g_pWeather->GetWeatherType()==MWeather::WEATHER_SNOW
		&& pPacket->getWeather()!=WEATHER_SNOWY
		
		|| g_pWeather->GetWeatherType()==MWeather::WEATHER_RAIN
		&& pPacket->getWeather()!=WEATHER_RAINY	)
	{
		g_pWeather->Release();
	}
	
	SetWeather(pPacket->getWeather(), pPacket->getWeatherLevel());

	DEBUG_ADD_FORMAT( "[UpdateInfo] Set Dark/Light : %d / %d", pPacket->getDarkLevel(), pPacket->getLightLevel());
	
	int darkLevel = pPacket->getDarkLevel();
	int lightSight = pPacket->getLightLevel();

	if (g_pUserInformation->IsMaster)
	{
		darkLevel = 0;
		lightSight = 13;
	}

	// ȭ���� ��ӱ�
	g_pTopView->SetDarkBits( darkLevel );		
	
	// Player�þ� ���� ũ��
	g_pPlayer->SetTimeLightSight( lightSight );

	
	//--------------------------------------------------
	//
	//		Game �ð�
	//
	//--------------------------------------------------
	DEBUG_ADD( "Set Game Time" );
	
	GameTime gt = pPacket->getGameTime();
	
	// ���� �ð� ����
	g_pGameTime->SetStartTime(g_CurrentTime, 
							gt.getYear(),
							gt.getMonth(),
							gt.getDay(),
							gt.getHour(),
							gt.getMinute(),
							gt.getSecond()
							);
  
	// �ð� ����
	g_pGameTime->SetTimeRatio( 24 );


	//--------------------------------------------------
	//
	//	 Gear�� ������ �����Ѵ�.
	//
	//--------------------------------------------------
	// 2004, 10 ,21, sobeit Modify start - ��������̺� ���������� ����
	CT_Done("GCUI: pre SetGearInfo");
	{
		Phoenix_FrameProbeStageScope fpStage(
			"GCUpdateInfo.SetGearInfo",
			g_hWnd,
			(int)g_bActiveApp,
			(int)g_bActiveGame);
		SetGearInfo( pPacket->getGearInfo(), pPacket->getBloodBibleSignInfo() );
	}
	CT_Done("GCUI: post SetGearInfo");
	// 2004, 10 ,21, sobeit Modify end - ��������̺� ���������� ����
	//--------------------------------------------------
	// ���°� ���..
	//--------------------------------------------------
	CT_Done("GCUI: pre CalculateStatus");
	{
		Phoenix_FrameProbeStageScope fpStage(
			"GCUpdateInfo.CalculateStatus",
			g_hWnd,
			(int)g_bActiveApp,
			(int)g_bActiveGame);
		g_pPlayer->CalculateStatus();
	}
	CT_Done("GCUI: post CalculateStatus");


	//---------------------------------------------------------------
	// �ӽ� Belt
	//---------------------------------------------------------------
	/*
	MItem* pItem = MItem::NewItem( ITEM_CLASS_BELT );
			
	// object ID
	pItem->SetID( 9999 );

	// type / option
	pItem->SetItemType(	1 );
	pItem->SetItemOption( 0 );

	g_SlayerGear.AddItem( pItem, (MSlayerGear::GEAR_SLAYER)MSlayerGear::GEAR_SLAYER_BELT );			
	*/
	/*
	// �ӽ÷� Į �ֱ�
	static MBlade	blade;	blade.SetItemType( 0 ); blade.SetItemOption( 0 );
	//static MSword	sword;	sword.SetItemType( 0 );	sword.SetItemOption( 0 );
	g_pPlayer->SetAddonItem( &blade);
	*/

	/*
	// �ӽ÷� �� �ֱ�
	static MGunSMG	gun;	gun.SetItemType( 0 ); gun.SetItemOption( 0 );
	MMagazine* pMagazine = (MMagazine*)MItem::NewItem( (ITEM_CLASS)ITEM_CLASS_MAGAZINE );

	// �ǹ� ���� - -;
	pMagazine->SetID( 0 );

	// �̰Ŵ� �ѿ� ���缭 ����ߵȴ�.
	for (int j=0; j<g_ItemTable[ITEM_CLASS_MAGAZINE].GetSize(); j++)			
	{
		pMagazine->SetItemType(	j );

		if (pMagazine->IsInsertToItem( (MItem*)&gun ))
		{
			break;
		}
	}

	// �ǹ� ����
	pMagazine->SetItemOption( 0 );

	// źâ ����
	pMagazine->SetNumber( 1000 );

	//------------------------------------
	// źâ ����
	//------------------------------------
	gun.SetMagazine( pMagazine );

	g_pPlayer->SetAddonItem( &gun );
	*/


	//--------------------------------------------------
	//
	//			Inventory �ʱ�ȭ
	//
	//--------------------------------------------------
	{
		Phoenix_FrameProbeStageScope fpStage(
			"GCUpdateInfo.SetInventoryInfo",
			g_hWnd,
			(int)g_bActiveApp,
			(int)g_bActiveGame);
		SetInventoryInfo( pPacket->getInventoryInfo() );
	}


	//--------------------------------------------------
	//
	// ��Ÿ Item ���� == Mouse�� �پ� �ִ� item
	//
	//--------------------------------------------------
	{
		Phoenix_FrameProbeStageScope fpStage(
			"GCUpdateInfo.SetExtraInfo",
			g_hWnd,
			(int)g_bActiveApp,
			(int)g_bActiveGame);
		SetExtraInfo( pPacket->getExtraInfo() );
	}


	//--------------------------------------------------
	//
	// �������
	//
	//--------------------------------------------------
	if (pPacket->hasMotorcycle())
	{
		Phoenix_FrameProbeStageScope fpStage(
			"GCUpdateInfo.MotorcycleInfo",
			g_hWnd,
			(int)g_bActiveApp,
			(int)g_bActiveGame);

		//--------------------------------------------
		// ������� ����
		//--------------------------------------------
		MItem*	pMotor = new MMotorcycle;	
		pMotor->SetID( 0 );
		pMotor->SetItemType( 0 ); 
//		pMotor->SetItemOption( 0 );
						
		//--------------------------------------------
		// ������� ���� �����۵�..
		//--------------------------------------------
		RideMotorcycleInfo* pMotorInfo = pPacket->getRideMotorcycleInfo();

		if (pMotorInfo!=NULL)
		{
			pMotor->SetID( pMotorInfo->getObjectID() );
			pMotor->SetItemType( pMotorInfo->getItemType() );
			pMotor->SetItemOptionList( pMotorInfo->getOptionType() );

			int num = pMotorInfo->getListNum();

			for (i=0; i<num; i++)
			{
				RideMotorcycleSlotInfo* pMotorSlotInfo = pMotorInfo->popFrontListElement();

				if (pMotorSlotInfo!=NULL)
				{
					// slot ������ �о pMotor�� �߰��ϸ� �ȴ�.
					// Motor�� MGridItemManager�̴�.

					delete pMotorSlotInfo;
				}
			}
		}

		//--------------------------------------------
		// ������̿� Ÿ�� �Ѵ�.
		//--------------------------------------------
		CT_Done("GCUI: pre SetAddonItem(pMotor) pMotor=%p", (void*)pMotor);
		g_pPlayer->SetAddonItem( pMotor );
		CT_Done("GCUI: post SetAddonItem(pMotor), pre delete");

		// -_-;;
		// ���߿��� player�� ������̸� ���� �ؾ��Ѵ�. -_-;;
		// ����� ������ ������ ���� �� ����� T_T;
		delete pMotor;
		CT_Done("GCUI: post delete pMotor");
	}

	CT_Done("GCUI: pre UI_DrawProgress(17)");
	UI_DrawProgress(17);
	CT_Done("GCUI: post UI_DrawProgress(17)");

	//--------------------------------------------------
	// ���� ��� ������ skill���� �ٽ� üũ�Ѵ�.
	//--------------------------------------------------
	CT_Done("GCUI: pre SetAvailableSkills");
	{
		Phoenix_FrameProbeStageScope fpStage(
			"GCUpdateInfo.SetAvailableSkills",
			g_hWnd,
			(int)g_bActiveApp,
			(int)g_bActiveGame);
		g_pSkillAvailable->SetAvailableSkills();
	}
	CT_Done("GCUI: post SetAvailableSkills");

	//--------------------------------------------------
	//
	// �ʿ��� ĳ���� Sprite Load
	//
	//--------------------------------------------------
	// [ TEST CODE ]
	// ĳ���� �� �ε�
//	DEBUG_ADD( "Load Creature Sprites" );
	
	//----------------------------------------
	// Player ĳ���� �׸� Load
	//----------------------------------------
	// Slayer����	- �׳�.. - -; �ǹ̰� ������..
//	LoadCreature( 0 );
//	UI_DrawProgress(25);
//
//	// Slayer����
//	LoadCreature( 1 );
//	UI_DrawProgress(35);
//
//	// Vampire����
//	LoadCreature( 2 );
//	UI_DrawProgress(38);
//
//	// Vampire����
//	LoadCreature( 3 );
//	UI_DrawProgress(39);
//
//	// ���� : spriteType�̴�.
//	LoadCreature( 58 );
//	// ���� : spriteType�̴�.
//	LoadCreature( 59 );
//	UI_DrawProgress(40);


//	MWorkThreadGlobal::Instance()->SetPriority(WORKTHREAD_PRIORITY_MAIN_LOADING);


	CT_Done("GCUI: pre getNPCCount/getMonsterCount");
	int numNPC = pPacket->getNPCCount();
	int numMonster = pPacket->getMonsterCount();
	CT_Done("GCUI: numNPC=%d numMonster=%d", numNPC, numMonster);

	int total = numNPC+numMonster;

	if (total==0) total = 1;

	COrderedList<int>	listUse;

	//--------------------------------------------------
	// Add NPC Sprite Type
	//--------------------------------------------------
	{
		Phoenix_FrameProbeStageScope fpStage(
			"GCUpdateInfo.BuildSpriteTypeList",
			g_hWnd,
			(int)g_bActiveApp,
			(int)g_bActiveGame);

		CT_Done("GCUI: pre NPC sprite loop");
		for (i=0; i<numNPC; i++)
		{
			int spriteType = pPacket->getNPCType( i );

			// 2001.8.20 �ּ�ó��
			//LoadCreature( spriteType );

			// �� zone���� ����ϴ� creature�� SpriteType
			listUse.Add( spriteType );

			//progress += progressInc;
			//UI_DrawProgress(progress);
		}

		//--------------------------------------------------
		// Add Monster Sprite Type
		//--------------------------------------------------
		for (i=0; i<numMonster; i++)
		{
			int spriteType = pPacket->getMonsterType( i );

			// �� zone���� ����ϴ� creature�� SpriteType
			listUse.Add( spriteType );
		}
	}

	//--------------------------------------------------
	// �� Zone���� ������� �ʴ� creature���� �����Ѵ�.
	//	
	// ��, LargeZone���� LargeZone���� �̵��ϴ� ��쿡��!
	//--------------------------------------------------
	{
		Phoenix_FrameProbeStageScope fpStage(
			"GCUpdateInfo.AddMonsterSpriteTypes",
			g_hWnd,
			(int)g_bActiveApp,
			(int)g_bActiveGame);

		if (g_bZonePlayerInLarge && g_nZoneLarge!=ZONEID_NULL && g_pZoneLarge!=NULL)
		{
			ZONETABLE_INFO* pZoneInfo = g_pZoneTable->Get( pPacket->getZoneID() );

			// Get NULL → eski kod pZoneInfo->Property ile AV (dump 07:53).
			if (pZoneInfo == NULL)
			{
				Phoenix_FatalDiagLogf(
					"GCUpdateInfo AddMonsterSpriteTypes: ZoneTable miss zoneID=%d",
					(int)pPacket->getZoneID());
				g_pTopView->AddMonsterSpriteTypes( listUse );
			}
			else if (pZoneInfo->Property & FLAG_ZONESIZE_SMALL)
			{
				g_pTopView->AddMonsterSpriteTypes( listUse );
			}
			else
			{
				ReleaseUselessCreatureSPKExcept( listUse );
			}
		}
		else
		{
			g_pTopView->AddMonsterSpriteTypes( listUse );
		}
	}
	//*/

	int progress = 40;
	int progressInc = 30 / total;

	//--------------------------------------------------
	// Load Monster & NPC
	//--------------------------------------------------
	if(g_pUserOption->IsPreLoadMonster)
	//if(false)
	{
		Phoenix_GCUI_LogMem("pre-creature-preload", gcuiZoneId);
		Phoenix_FrameProbeStageScope fpStage(
			"GCUpdateInfo.PreLoadCreatureSprites",
			g_hWnd,
			(int)g_bActiveApp,
			(int)g_bActiveGame);

		COrderedList<int>::DATA_LIST::const_iterator listPos = listUse.GetIterator();

		int listSize = listUse.GetSize();

		for(i = 0; i < listSize; ++i, ++listPos)
		{		
			LoadCreature( *listPos );

			progress += progressInc;
			UI_DrawProgress(progress);
		}
		Phoenix_GCUI_LogMem("post-creature-preload", gcuiZoneId);
	}


	if(progress < 70)
	{
		UI_DrawProgress( 70 );
	}

	//--------------------------------------------------
	// Zone Loading...			
	//--------------------------------------------------
	// ���� ���̾�����.. Zone �̵�.
	DEBUG_ADD( "Load Zone" );
	
	//--------------------------------------------------
	// �ӽ� Zone��ǥ
	//--------------------------------------------------
	g_pPlayer->SetX( pPacket->getZoneX() );
	g_pPlayer->SetY( pPacket->getZoneY() );

	CT_Done("GCUI: pre Zone Load (previousMode=%d zoneID=%d)", (int)previousMode, (int)pPacket->getZoneID());
	{
		Phoenix_FrameProbeStageScope fpStage(
			previousMode == MODE_GAME ? "GCUpdateInfo.MoveZone" : "GCUpdateInfo.LoadZone",
			g_hWnd,
			(int)g_bActiveApp,
			(int)g_bActiveGame);

		BOOL zoneLoaded = FALSE;
		if (previousMode == MODE_GAME)
		{
			CT_Done("GCUI: pre MoveZone");
			zoneLoaded = MoveZone( pPacket->getZoneID() );
			CT_Done("GCUI: post MoveZone");
			CT_Done("[GCUI] AFTER MoveZone g_pZone=%p", (void*)g_pZone);
		}
		else
		{
			CT_Done("GCUI: pre LoadZone");
			zoneLoaded = LoadZone( pPacket->getZoneID() );
			CT_Done("GCUI: post LoadZone");
			CT_Done("[GCUI] AFTER LoadZone g_pZone=%p", (void*)g_pZone);
		}

		if (!zoneLoaded || g_pZone == NULL)
		{
			CT_Done("[GCUI] ABORT zone transition failed zoneID=%d loaded=%d g_pZone=%p oom=%d",
				(int)pPacket->getZoneID(), (int)zoneLoaded, (void*)g_pZone,
				g_bLastLoadZoneWasOOM ? 1 : 0);
			DEBUG_ADD_FORMAT("[Error] GCUpdateInfo zone transition failed. zoneID=%d loaded=%d g_pZone=%p oom=%d",
				(int)pPacket->getZoneID(), (int)zoneLoaded, (void*)g_pZone,
				g_bLastLoadZoneWasOOM ? 1 : 0);
			g_DXSound.UnSetMute();
			UI_EndProgress();
			if (g_bLastLoadZoneWasOOM)
			{
				Phoenix_FatalDiagLogf(
					"soft fail: GCUpdateInfo abort OOM zoneID=%d -> MODE_QUIT (no zone)",
					(int)pPacket->getZoneID());
				if (g_pSystemMessage != NULL)
					g_pSystemMessage->Add("Map load failed: not enough memory.");
				SetMode(MODE_QUIT);
				return;
			}
			SetMode(MODE_QUIT);
			return;
		}
	}

	Phoenix_GCUI_LogMem("post-zone-load", gcuiZoneId);

//	MWorkThreadGlobal::Instance()->SetPriority(WORKTHREAD_PRIORITY_THREAD_LOADING);

	// Packet Encrypt
	CT_Done("[GCUI] PRE setEncryptCode g_pZone=%p", (void*)g_pZone);
	CT_Done("GCUI: pre setEncryptCode");
	{
		Phoenix_FrameProbeStageScope fpStage(
			"GCUpdateInfo.SetEncryptCode",
			g_hWnd,
			(int)g_bActiveApp,
			(int)g_bActiveGame);
		g_pSocket->setEncryptCode();
	}
	CT_Done("GCUI: post setEncryptCode, pre NPC pos loop");

	//-----------------------------------------------------------
	//
	// Zone������ NPC ��ǥ ����
	//
	//-----------------------------------------------------------
	{
		Phoenix_FrameProbeStageScope fpStage(
			"GCUpdateInfo.NPCPositionLoop",
			g_hWnd,
			(int)g_bActiveApp,
			(int)g_bActiveGame);

		int npcLoopIter = 0;
		while (1)
		{
			NPCInfo* pInfo = pPacket->popNPCInfo();

			if (pInfo==NULL)
			{
				break;
			}
			CT_Done("GCUI-NPC: iter %d pInfo=%p", npcLoopIter, (void*)pInfo);

			const char* pName = pInfo->getName().c_str();
			int npcID = pInfo->getNPCID();
			int x = pInfo->getX();
			int y = pInfo->getY();

			if(g_pZone)
			{
				int zoneID	= g_pZone->GetID();
				if(zoneID< 1500 || zoneID >1506)
					pName = (*g_pCreatureTable)[npcID].Name;
			}else
				pName = (*g_pCreatureTable)[npcID].Name;

			UI_SetNPCInfo( pName, npcID, x, y );

			delete pInfo;
			npcLoopIter++;
		}
		CT_Done("GCUI: post NPC pos loop iters=%d", npcLoopIter);
	}

	//--------------------------------------------------
	//
	//	���� Player�� �ɷ��ִ� Effect Status�� �����Ѵ�.
	//
	//--------------------------------------------------
	// [!] Zone Loading ���Ŀ� �ؾ��Ѵ�.
	//--------------------------------------------------
	
	//--------------------------------------------------
	// ���� �ð��� Frame ����� �ٽ� ���ش�.
	//--------------------------------------------------
	g_CurrentTime = timeGetTime();	// �ʿ��ұ�? -_-;
	
	// updateInfo�� ó���Ѵٰ� �ҿ�� �ð�
	// ...�� frame���� ȯ���� ��
	int delayedFrame = ConvertMillisecondToFrame( g_CurrentTime - startTime );		

	// ���� frame ����
	g_CurrentFrame += delayedFrame;
//	g_pPlayer->SetBatColorSet(0xFFFF);

	PCType pcType = pPacket->getPCInfo()->getPCType();

	//--------------------------------------------------
	// Vampire�� ���
	//--------------------------------------------------
	if(pcType == PC_VAMPIRE)
	{
		PCVampireInfo2 *pInfo = dynamic_cast< PCVampireInfo2* >( pPacket->getPCInfo() );
		
		// ���� �����̾� ����Ʈ ����Ʈ
		if( bIsAdvancementClass && g_pPlayer->GetCreatureType() == CREATURETYPE_VAMPIRE_GHOST )
			g_pPlayer->SetAdvanceBatEffect();
#if __CONTENTS(__FAST_TRANSFORTER)
		else if(g_pPlayer->GetCreatureType() == CREATURETYPE_FLITTERMOUSE)
			g_pPlayer->SetWingEffect1();
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
		else if(g_pPlayer->GetCreatureType() == CREATURETYPE_SHAPE_OF_DEMON)
			g_pPlayer->SetWingEffect2();
#endif //__SECOND_TRANSFORTER
	}

	CT_Done("GCUI: pre SetEffectInfo");
	{
		Phoenix_FrameProbeStageScope fpStage(
			"GCUpdateInfo.SetEffectInfo",
			g_hWnd,
			(int)g_bActiveApp,
			(int)g_bActiveGame);
		SetEffectInfo( g_pPlayer, pPacket->getEffectInfo(), delayedFrame );
	}
	CT_Done("GCUI: post SetEffectInfo");


// 	// �ε��� �ɶ����� ��Ѹ���.
// 	MWorkThreadGlobal::Instance()->SetPriority(WORKTHREAD_PRIORITY_THREAD_LOADING);
// 
// 	int nLoadCount = 90;
// 	while(MWorkThreadGlobal::Instance()->IsWorking()) {
// 		UI_DrawProgress(nLoadCount++);
// 		Sleep(300);
// 	}
// 	

	/*MWorkThreadGlobal::Instance()->SetPriority(WORKTHREAD_PRIORITY_GAME_PLAYING);*/
	
	UI_DrawProgress(100);

	//--------------------------------------------------
	//
	// Skill Tree
	//
	//--------------------------------------------------
	/*
	g_SkillManager.Init( MAX_SKILLDOMAIN );

	//---------------------------------------------------------------------
	// �⺻ ����κ��� skill tree�� �ʱ�ȭ�Ѵ�.
	//---------------------------------------------------------------------
	g_SkillManager[SKILLDOMAIN_BLADE].SetRootSkill( SKILL_SINGLE_BLOW );
	g_SkillManager[SKILLDOMAIN_SWORD].SetRootSkill( SKILL_DOUBLE_IMPACT );
	g_SkillManager[SKILLDOMAIN_GUN].SetRootSkill( SKILL_SNIPPING );
	g_SkillManager[SKILLDOMAIN_GUN].SetRootSkill( SKILL_SHARP_SHOOTING );
	g_SkillManager[SKILLDOMAIN_ENCHANT].SetRootSkill( MAGIC_CREATE_HOLY_WATER );
	g_SkillManager[SKILLDOMAIN_HEAL].SetRootSkill( MAGIC_CURE_POISON );
	g_SkillManager[SKILLDOMAIN_VAMPIRE].SetRootSkill( MAGIC_HIDE );	
	*/

	/*
	g_SkillAvailable.AddSkill( SKILL_DOUBLE_IMPACT );
	g_SkillAvailable.AddSkill( SKILL_TRIPLE_SLASHER );
	g_SkillAvailable.DisableSkill( SKILL_TRIPLE_SLASHER );
	g_SkillAvailable.AddSkill( SKILL_SCREW_SLASHER );
	
	g_SkillAvailable.AddSkill( SKILL_SINGLE_BLOW );
	g_SkillAvailable.DisableSkill( SKILL_SINGLE_BLOW );
	
	g_SkillAvailable.AddSkill( SKILL_ARMOR_CRASH );

	g_SkillAvailable.AddSkill( SKILL_SNIPPING );
	g_SkillAvailable.AddSkill( SKILL_KNOCKS_TARGET_BACK );

	g_SkillAvailable.AddSkill( MAGIC_LIGHT );
	g_SkillAvailable.AddSkill( MAGIC_DETECT_HIDDEN );
	g_SkillAvailable.DisableSkill( MAGIC_DETECT_HIDDEN );
	
	g_SkillAvailable.AddSkill( MAGIC_CURE_POISON );
	g_SkillAvailable.AddSkill( MAGIC_PROTECTION_FROM_CURSE );
	
	g_SkillAvailable.AddSkill( MAGIC_CURE_POISON );
	g_SkillAvailable.DisableSkill( MAGIC_CURE_POISON );
	g_SkillAvailable.AddSkill( MAGIC_PROTECTION_FROM_POISON );
	g_SkillAvailable.AddSkill( MAGIC_CURE_PARALYSIS );
	*/

	CT_Done("GCUI: pre UI_EndProgress");
	{
		Phoenix_FrameProbeStageScope fpStage(
			"GCUpdateInfo.UIEndProgress",
			g_hWnd,
			(int)g_bActiveApp,
			(int)g_bActiveGame);
		UI_EndProgress();
	}
	CT_Done("GCUI: post UI_EndProgress");


	//-----------------------------------------------------------
	// Sound�� ����� �� �ְ� �Ѵ�.
	//-----------------------------------------------------------
	{
		Phoenix_FrameProbeStageScope fpStage(
			"GCUpdateInfo.SoundUnMute",
			g_hWnd,
			(int)g_bActiveApp,
			(int)g_bActiveGame);
		g_DXSound.UnSetMute();
	}
	CT_Done("GCUI: post UnSetMute");

	// blood drainüũ���� �� �� ��� ��.��
	UI_SetHP( g_pPlayer->GetHP(), g_pPlayer->GetMAX_HP() );
	
	//UI_AffectUserOption();

	//--------------------------------------------------
	// message �ʱ�ȭ
	//--------------------------------------------------
	g_pSystemMessage->Clear();
	g_pGameMessage->Clear();
	g_pNoticeMessage->Clear();
	//2008.07.17 zzi
	if (g_pGMMessage) g_pGMMessage->Clear();
	if (g_pEventMessage) g_pEventMessage->Clear();

	CT_Done("[GCUI] PRE GetPKType g_pZone=%p", (void*)g_pZone);
	if( g_pZone->GetPKType() == PK_TYPE_USER_PK )
	{
		g_pNoticeMessage->Add((*g_pGameStringTable)[UI_STRING_MESSAGE_PKSERVER_USE].GetString());
	}

	///////////////////////////////////////////////////////////////
	// chyaya
	/*
	MEvent event;
	//event.eventID = EVENTID_ADVANCEMENT_QUEST_ENDIN;
	event.eventID = EVENTID_LOST_CHILD_PROLOGUE;
	event.eventType = EVENTTYPE_ZONE;
	event.eventFlag = EVENTFLAG_ONLY_EVENT_BACKGROUND | EVENTFLAG_NOT_DRAW_UI | EVENTFLAG_NOT_DRAW_CREATURE | EVENTFLAG_NOT_DRAW_INFORMATION | EVENTFLAG_NOT_DRAW_CREATURE_SHADOW | EVENTFLAG_NOT_DRAW_ITEM | EVENTFLAG_NOT_DRAW_MOUSE_POINTER | EVENTFLAG_NOT_DRAW_EFFECT | EVENTFLAG_DENY_INPUT | EVENTFLAG_NOT_FADE_SCREEN | EVENTFLAG_NOT_PLAY_SOUND;
	
	// �����̾� ���� ����Ʈ ���� �̺�Ʈ
	event.parameter1 = 0;
	event.parameter4 = 3;
	g_pEventManager->AddEvent(event);
	/**/
	///////////////////////////////////////////////////////////////

	//--------------------------------------------------
	// Player �������� ��� üũ
	//--------------------------------------------------
	bool bPlayerShapeCheck = g_pPlayer && g_pPlayer->IsOperator();
	g_pUserInformation->attrOperator.SetAttr(OPERATORTYPE_PLAYERSHAPE, bPlayerShapeCheck);
	
	//--------------------------------------------------
	// player�� ��ġ�� ��ٸ���.
	//--------------------------------------------------
	SetMode( MODE_WAIT_SETPOSITION );

	
/*	MItem* pItem = g_pInventory->FindItem(ITEM_CLASS_SUB_INVENTORY);
	if(pItem)
	{
		CGUseItemFromInventory _CGUseItemFromInventory;
		_CGUseItemFromInventory.setObjectID( pItem->GetID());
		_CGUseItemFromInventory.setX( pItem->GetGridX());
		_CGUseItemFromInventory.setY( pItem->GetGridY());

		g_pSocket->sendPacket( &_CGUseItemFromInventory );		
	}*/



//	FILE *fp=fopen("InventoryLog.txt","rt");
//	if( fp != NULL )
//	{
//		char szBuffer[128] = "InventoryLogStart012301230123012301230123012301230123InventoryLogEnd";		                                       
//		fseek(fp,0,SEEK_END);
//		int filesize = ftell(fp);
//
//		if( filesize < strlen(szBuffer) )
//		{
//			char filebuffer[128];
//			sprintf(filebuffer,"InventoryLog:%d", filesize);
//			
//			SendBugReport( filebuffer );
//		}
//		fclose(fp);
//	}
	
	FILE *fp=fopen("Phoenix.HIS","rt");
	if( fp == NULL )
	{
		fp = fopen("Phoenix.HIS","wt");
		fprintf(fp,"!@#");
		CSystemInfo SystemInfo;
		SendBugReport("SystemInfo : %d %d CPU : %dMhz",g_bEnable3DHAL, g_dwVideoMemory,SystemInfo.GetCpuClock());
	}
	fclose(fp);

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
	//ij-ch 2008.09.30 �����̾� ����â ���� Add
	if(gC_vs_ui.IsRunningRequest_PremiumGiveItem()==false)
		gC_vs_ui.Request_PremiumGiveItem();
	//ij-ch 2008.09.30 �����̾� ����â ���� End
#endif //__PREMIUM_GIVE_ITEM_UI

//	ExecuteHelpEvent( HELP_EVENT_INTERFACE );

#endif

	Phoenix_GCUI_LogMem("exit", gcuiZoneId);

	__END_CATCH
}
