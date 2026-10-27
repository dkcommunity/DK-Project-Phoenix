//-----------------------------------------------------------------------------
// UIMessageManager.cpp
//-----------------------------------------------------------------------------
// If you try to do something with QuickSlot... QuickSlot might disappear... 
// We're currently unable to fix this due to timing issues in the UI.
//-----------------------------------------------------------------------------
#include "client_PCH.h"
#include <io.h>
#include <process.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "Client.h"
#include "crash_trace_tool.h"
#include "PhoenixResolutionLive.h"	// 2026-09-02 O3 — canli cozunurluk
#include "login_socket_trace.h"
#include "UIMessageManager.h"
#include "UIFunction.h"
#include "VS_UI.h"
#include "VS_UI_Friend_System.h" 
#include "VS_UI_Mouse_pointer.h"
#include "VS_UI_GameCommon2.h"
#include "TalkBox.h"
#include "MShopShelf.h"
#include "MPriceManager.h"
#include "MMoneyManager.h"
#include "UIDialog.h"
#include "SkillDef.h"
#include "ClientFunction.h"
#include "MGameStringTable.h"
#include "MItemOptionTable.h"
#include "CToken.h"
#include "UserOption.h"
#include "MWeather.h"
#include "MStatusManager.h"
#include "MChatManager.h"
#include "PacketDef.h"
#include "UtilityFunction.h"
#include "ClientConfig.h"
#include "MMusic.h"
#include "MMusicTable.h"
#include "CDirectSound.h"
#include "TitleMusic.h"  // Title music random pool (2026-05-14)
#include "SoundDef.h"
#include "DEServerInformation.h"
#include "MParty.h"
#include "MItemFinder.h"
#include "ServerInfo.h"
#include "MZoneTable.h"
//#include "MFileDef.h"
#include "Properties.h"
#include "UserInformation.h"
#include "TempInformation.h"
#include "ProfileManager.h"

namespace
{
	bool PhoenixIsAudioSliderValue(int value)
	{
		return value >= 0 && value <= 100;
	}

	LONG PhoenixSliderValueToMp3Volume(int value)
	{
		const int clampedValue = CDirectSound::ClampSliderValue(value);
		return (LONG)((clampedValue * 0xFFFF + 50) / 100);
	}
}
static void PhoenixUILoginHandshakeTrace(const char* fmt, ...)
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

namespace {

const int PHOENIX_REGISTER_ID_MIN = 4;
const int PHOENIX_REGISTER_ID_MAX = 8;
const int PHOENIX_REGISTER_PASSWORD_MIN = 6;
const int PHOENIX_REGISTER_PASSWORD_MAX = 8;

bool PhoenixRegisterIsAlpha(char ch)
{
	return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

bool PhoenixRegisterIsDigit(char ch)
{
	return ch >= '0' && ch <= '9';
}

bool PhoenixRegisterIsAccountChar(char ch)
{
	return PhoenixRegisterIsAlpha(ch) || PhoenixRegisterIsDigit(ch);
}

bool PhoenixRegisterValidateID(const char* text)
{
	if (text == NULL)
		return false;

	const int len = (int)strlen(text);
	if (len < PHOENIX_REGISTER_ID_MIN || len > PHOENIX_REGISTER_ID_MAX)
		return false;

	if (!PhoenixRegisterIsAlpha(text[0]))
		return false;

	for (int i = 0; i < len; ++i)
	{
		if (!PhoenixRegisterIsAccountChar(text[i]))
			return false;
	}

	return true;
}

bool PhoenixRegisterValidatePassword(const char* text)
{
	if (text == NULL)
		return false;

	const int len = (int)strlen(text);
	if (len < PHOENIX_REGISTER_PASSWORD_MIN || len > PHOENIX_REGISTER_PASSWORD_MAX)
		return false;

	for (int i = 0; i < len; ++i)
	{
		if (!PhoenixRegisterIsAccountChar(text[i]))
			return false;
	}

	return true;
}

void PhoenixRegisterFreePayload(NEW_REGISTRATION* pReg)
{
	if (pReg == NULL)
		return;

	DeleteNewArray(pReg->sz_id);
	DeleteNewArray(pReg->sz_password);
	DeleteNewArray(pReg->sz_repassword);
	DeleteNewArray(pReg->sz_name);
	DeleteNewArray(pReg->sz_email);
	DeleteNewArray(pReg->sz_address);
	DeleteNewArray(pReg->sz_ssn_number_part1);
	DeleteNewArray(pReg->sz_ssn_number_part2);
	DeleteNewArray(pReg->sz_homepage);
	DeleteNewArray(pReg->sz_woo);
	DeleteNewArray(pReg->sz_phone);
	memset(pReg, 0, sizeof(NEW_REGISTRATION));
}

void PhoenixRegisterPopupFailure(NEW_REGISTRATION* pReg, const char* message)
{
	CT_Done("UI_NEW_USER_REGISTRATION failed: %s", message != NULL ? message : "<null>");
	UI_UserRegistrationFailedMessage();

	if (g_pUIDialog != NULL)
	{
		g_pUIDialog->PopupFreeMessageDlg(
			message != NULL ? message : "Account has not been created.",
			-1,
			-1,
			UI_DIALOG_TITLE_OK);
	}

	PhoenixRegisterFreePayload(pReg);
}

}
#include "CGameUpdate.h"
#include "MTestDef.h"
#include "AcceleratorDef.h"
#include "BGMManager.h"

#include "Packet\CPackets\CGRequestNewbieItem.h"
#include "Packet\CPackets\CGSilverCoating.h"
#include "Packet\CPackets\CLChangeServer.h"
#include "Packet\CPackets\CLGetServerList.h"
#include "Packet\CPackets\CGSelectPortal.h"
#include "Packet\CPackets\CGSelectWayPoint.h"
#include "Packet\CPackets\CGPartyInvite.h"
#include "Packet\CPackets\CGPartyLeave.h"
#include "packet\Cpackets\CGResurrect.h"
#include "packet\Cpackets\CLSelectWorld.h"
#include "packet\Cpackets\CLSelectServer.h"
#include "packet\Cpackets\CLQueryCharacterName.h"
#include "packet\CPackets\CGRelicToObject.h"
#include "packet\CPackets\CGSelectGuild.h"
#include "packet\CPackets\CGSelectGuildMember.h"
#include "packet\CPackets\CGRequestGuildMemberList.h"
#include "packet\CPackets\CGTryJoinGuild.h"
#include "packet\CPackets\CGJoinGuild.h"
#include "packet\CPackets\CGRegistGuild.h"
#include "packet\CPackets\CGModifyGuildMember.h"
#include "packet\CPackets\CGAddItemToItem.h"
#include "packet\CPackets\CGModifyGuildIntro.h"
#include "packet\CPackets\CGModifyGuildMemberIntro.h"
#include "packet\CPackets\CGSkillToNamed.H"
#include "packet\CPackets\CGSelectRankBonus.H"
#include "packet\CPackets\CGWithdrawTax.H"
#include "packet\CPackets\CGTypeStringList.h"
#include "packet\CPackets\CGLotterySelect.h"
#include "packet\CPackets\CGTakeOutGood.h"
#include "packet/CPackets/CGMixItem.h"					// -_- Rain? The assistant said this is what happens if you do it this way
#include "packet/CPackets/CGDownSkill.h"
#include "packet/GPackets/GCMiniGameScores.h"
#include "packet/CPackets/CGSubmitScore.h"
#include "packet/CPackets/CGAddItemToCodeSheet.h"
#include "packet/CPackets/CGFailQuest.h"
#include "packet/CPackets/CGSelectRegenZone.h"
#include "packet/CPackets/CGTameMonster.h"
#include "packet/CPackets/CGPetGamble.h"
#include "packet\CPackets\CGUseMessageItemFromInventory.h"
#include "packet\Cpackets\CGPartySay.h"
#include "packet\Cpackets\CGDepositPet.h"
#include "packet\Cpackets\CGWithdrawPet.h"
#include "packet\Cpackets\CGSMSSend.h"
#include "packet\Cpackets\CGSMSAddressList.h"
#include "packet\Cpackets\CGDeleteSMSAddress.h"
#include "packet\Cpackets\CGAddSMSAddress.h"
#include "packet\Cpackets\CGSelectNickname.h"
#include "packet\Cpackets\CGModifyNickname.h"
#include "packet\Cpackets\CGGQuestAccept.h"
#include "packet\Cpackets\CGGQuestCancel.h"
#include "packet\Cpackets\CGUseItemFromGQuestInventory.h"
#include "packet\Cpackets\CLAgreement.h"
#include "packet\Cpackets\CGRequestInfo.h"

#include "RequestFunction.h"
#include "RequestServerPlayerManager.h"
#include "RequestUserManager.h"
#include "ClientCommunicationManager.h"
#include "WhisperManager.h"
#include "packet\Rpackets\RCSay.h"
#include "packet\Cpackets\CGGuildChat.h"
#include "CMP3.h"
#include "RankBonusTable.h"
#include "Profiler.h"

#include "DebugInfo.h"
#include "MEventManager.h"
#include "MOustersGear.h"
#include "SoundSetting.h"
#include "ShrineInfoManager.h"

#include "MTimeItemManager.h"
#include "MFakeCreature.h"

#include "packet\Cpackets\CGRequestUnion.h"
#include "packet\Cpackets\CGQuitUnion.h"
#include "packet\Cpackets\CGExpelGuild.h"
#include "Packet\Cpackets\CGRequestGuildList.h"
#include "Packet\Cpackets\CGRequestUnionInfo.h"
#include "Packet\Cpackets\CGAcceptUnion.h"
#include "Packet\Cpackets\CGDenyUnion.h"
#include "Packet\Cpackets\CGQuitUnionAccept.h"
#include "Packet\Cpackets\CGQuitUnionDeny.h"
#include "Packet\Cpackets\CGRangerSay.h"
#include "Packet\Cpackets\CGModifyTaxRatio.h"
#include "Packet\Cpackets\CGAppointSubmaster.h"
//
#include "packet\Cpackets\CGRequestUnion.h"
#include "packet\Cpackets\CGQuitUnion.h"
#include "packet\Cpackets\CGExpelGuild.h"
#include "Packet\Cpackets\CGRequestGuildList.h"
#include "Packet\Cpackets\CGRequestUnionInfo.h"
#include "Packet\Cpackets\CGAcceptUnion.h"
#include "Packet\Cpackets\CGDenyUnion.h"
#include "Packet\Cpackets\CGQuitUnionAccept.h"
#include "Packet\Cpackets\CGQuitUnionDeny.h"
#include "Packet\Cpackets\CGRangerSay.h"
#include "Packet\Cpackets\CGModifyTaxRatio.h"
#include "Packet\Cpackets\CGAppointSubmaster.h"

#include "Packet/Cpackets/CGDisplayItem.h"
#include "Packet/Cpackets/CGUndisplayItem.h"
#include "Packet/Cpackets/CGStoreOpen.h"
#include "Packet/Cpackets/CGStoreClose.h"
#include "Packet/Cpackets/CGStoreSign.h"
#include "Packet/Cpackets/CGRequestStoreInfo.h"
#include "Packet/Gpackets/GCMyStoreInfo.h"
#include "Packet/Gpackets/GCOtherStoreInfo.h"
#include "Packet/Cpackets/CGBuyStoreItem.h"
#include "Packet/Gpackets/GCRemoveStoreItem.h"
#include "Packet/Gpackets/GCAddStoreItem.h"
//

#include "Packet\Cpackets\CGUsePowerPoint.h"
#include "Packet\Cpackets\CGRequestPowerPoint.h"
#include "Packet\Cpackets\CGDonationMoney.h"
#include "Packet\Cpackets\CGGetEventItem.h"

#include "Packet\Cpackets\CGRequestWebMarket.h"
#include "Packet\Cpackets\CGRequestRankGemData.h"
#include "Packet\Cpackets\CGRequestTmallList.h"
#include "Packet\Cpackets\CGBuyTmallGoods.h"
#include "Packet\Cpackets\CGCashChargeRequest.h"

#include "Packet\Cpackets\CGChangeGearSlot.h"

#if __CONTENTS(__TIPOJYU_CASTLE)
#include "Packet\Cpackets\CGInstanceDoungeonEnter.h"
#endif //__TIPOJYU_CASTLE

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
#include "Packet\Cpackets\CGSelectWeekItem.h"
#include "Packet\Cpackets\CGSelectAllWeekItem.h"
#endif //__PREMIUM_GIVE_ITEM_UI

#if __CONTENTS(__AUTO_ITEM_ROOTING)
#include "AutoItemRooting.h"
#endif //__AUTO_ITEM_ROOTING

#if __CONTENTS(__REPAIRSYSTEM_RENEWAL)
#include "Packet\Cpackets\CGRequestRepairinfo.h"
#endif //__REPAIRSYSTEM_RENEWAL

#if __CONTENTS(__FRIEND_ADDITION)
#include "Packet\Cpackets\CGFriendReqToAdd.h"
#include "Packet\Cpackets\CGFriendReqToDel.h"
#include "Packet\Cpackets\CGFriendResToAdd.h"
#include "Packet\Cpackets\CGFriendViewInfo.h"
#include "Packet\Cpackets\CGFriendUpdateGlobalOption.h"
#include "Packet\Cpackets\CGFriendUpdateIndividualOption.h"
#include "Packet\Gpackets\GCFriendReqToAdd.h"
#include "Packet\Gpackets\GCFriendReqToDel.h"
#endif //__FRIEND_ADDITION



//-----------------------------------------------------------------------------
// Special characters used in Chat
//-----------------------------------------------------------------------------
#define	SYMBOL_GLOBALCHAT			'!'			// Zone Chat
#define	SYMBOL_WHISPER				'/'			// WHISPER
#define	SYMBOL_COMMAND				'@'			// Special Commands
#define SYMBOL_MASTER_COMMAND		'*'			// Special commands for administrators

#define	STRING_IGNORE				"Reject"		// Refusal to engage in dialogue
#define	STRING_IGNORE_ENG			"ignore"	// Refusal to engage in dialogue
#define	STRING_ACCEPT				"Allowe"		// Allow chat
#define	STRING_ACCEPT_ENG			"accept"	// Allow chat
#define	STRING_ACCEPT_CURSE			"What did you say?"	// View bad words
#define	STRING_ACCEPT_CURSE_ENG		"what?"		// View bad words
#define	STRING_FILTER_CURSE			"Proper Language"	// Show only correct words
#define	STRING_FILTER_CURSE_ENG		"filter"	// Show only correct words

int g_C2G = 0;

//-----------------------------------------------------------------------------
// extern
//-----------------------------------------------------------------------------
#if defined(OUTPUT_DEBUG) && defined(_DEBUG)
	extern int g_ShowImageObjectID;
#endif

#ifdef OUTPUT_DEBUG
	extern MString g_ProfileInfoName;
#endif

bool							g_bEventQuestEnding = false;
extern bool		LoadingAddonSPK(bool bLoading);
extern CGameUpdate*		g_pCGameUpdate;

#if __CONTENTS(__MOVE_ZONE)
extern C_VS_UI_GPS		*gpC_Gps;
#endif //__MOVE_ZONE

//-----------------------------------------------------------------------------
// Global
//-----------------------------------------------------------------------------
UIMessageManager*	g_pUIMessageManager = NULL;

extern BOOL g_bEnable3DHAL;
extern BOOL g_bSetHotKey;
extern bool g_bWatchMode;
extern bool	g_bTestMode;
extern bool g_bHolyLand;
extern bool g_bZoneSafe;

#if __CONTENTS(__GEAR_SWAP_CHANGE)	//UIMessageManager.cpp		extern GearInfo* g_pGearInfo;
extern GearInfo* g_pGearInfo;
#endif	// __GEAR_SWAP_CHANGE
//-----------------------------------------------------------------------------
// Execute Logout
//-----------------------------------------------------------------------------
void
ExecuteLogout()
{
	//--------------------------------------------------
	// If alive
	//--------------------------------------------------
	if (g_Mode==MODE_GAME)
	{
		DEBUG_ADD("ExecuteLogout");

		UI_SaveUserOption();

		// Weather Pause
		SetWeather(WEATHER_CLEAR, 0);

		// If you die, send a resurrection packet and log out.
		// But... it doesn't really matter when it actually gets processed... - -;
		if (!g_pPlayer->IsAlive())
		{
			CGResurrect _CGResurrect;
				
			g_pSocket->sendPacket( &_CGResurrect );				

			gC_vs_ui.FinishRequestDie();
			gC_vs_ui.FinishRequestResurrect();
			gC_vs_ui.CloseRequestShrineMinimap();
		}

		if (1)//g_pPlayer->IsAlive())
		{
			//--------------------------------------------------
			// The music stops.
			//--------------------------------------------------
			if (g_pUserOption->PlayWaveMusic)
			{
//				g_pDXSoundStream->Stop();
				DEBUG_ADD("MP3 STOP7");
#ifdef __USE_MP3__
				g_pMP3->Stop();
#else
				if( g_DXSound.IsInit() )
					BGMManager::Instance()->StopMusic();
					//g_pOGG->streamClose();
#endif
				DEBUG_ADD("MP3 STOP7 OK");
			}
			else
			{
				g_Music.Stop();
			}

			//--------------------------------------------------
			// Thread Loading END..
			//--------------------------------------------------
//			StopLoadingThread();

			//
			// ?stemci, sunucuya oturumu kapatma paketini g?nderdi.
			//
			//--------------------------------------------------
			// Send a CGLogout packet to the game server.
			//--------------------------------------------------
				UI_SaveHotKeyToServer();

				CGLogout cgLogout;
			
				g_pSocket->sendPacket( &cgLogout );
				//g_pSocket->setPlayerStatus( CPS_END_SESSION );

				g_pSocket->processOutput();

				g_pSocket->setPlayerStatus( CPS_WAITING_FOR_GC_RECONNECT_LOGIN );	

				
				//g_pSocket->disconnect();

				// 2001.6.12 : release ...without doing so, reconnect.
				//ReleaseSocket();

			/*
			if (g_pZone != NULL)
			{
				g_pZone->RemovePlayer();
				g_pPlayer->SetStop();
				g_pPlayer->SetAction( ACTION_STAND );
				//g_pZone->UnSetLight(g_pPlayer->GetX(), g_pPlayer->GetY(), g_pPlayer->GetLightSight());	// Remove from view
			}
			*/

			// This takes you to the character selection screen message
			/*
			if (g_pCGameUpdate!=NULL)
			{
				gC_vs_ui.CloseGameMenu();

				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_WAIT_FOR_CHARACTER_SELECT_MODE].GetString(), -1, -1, 0 );			
				
				g_pCGameUpdate->UpdateDraw();				
				CDirectDraw::Flip();

				g_pCGameUpdate->UpdateDraw();				
				CDirectDraw::Flip();
			}
			*/

			// The weather has come to a standstill...
			//SetWeather(WEATHER_CLEAR, 0);
			ReleaseGameObject();

			// 2001.6.12 : Back online~
			//SetMode( MODE_MAINMENU );	
			SetMode( MODE_WAIT_RECONNECT_LOGIN );

			//------------------------------------------------------
			// [ TEST CODE ]
			// Is it okay to initialize the sound here?
			//------------------------------------------------------
			//InitSound();
			g_CurrentFrame		= 0;

			g_bSetHotKey	= FALSE;

			//------------------------------------------------------
			// ui dialog
			//------------------------------------------------------
			g_pUIDialog->HidePCTalkDlg();
			g_pUIDialog->CloseMessageDlg();


			//------------------------------------------------------
			// Allow everyone to chat
			//------------------------------------------------------
			g_pChatManager->ClearID();
			g_pChatManager->SetAcceptMode();

//			gC_vs_ui.ServerDisconnectMessage();
//			gC_vs_ui.CloseAllDialog();

			// Start Title Screen UI\
            //gC_vs_ui.EndTitle();\
            //07/03/2007: Fixed a bug in the "Exit?" message window during the title screen redesign
			// By first displaying a fake character selection screen, we prevent keyboard control from switching to the title screen 
            // while waiting for the packet to arrive. Since actual control is received only after the packet arrives, 
            // any key inputs before that are intercepted and ignored by the fake screen.
			gC_vs_ui.StartTitle();
			UI_StartCharacterManager(0);
	
			gC_vs_ui.DropItem();

			g_bUIInput = FALSE;

			// It deletes the recipient of the whisper.
			g_pUserInformation->WhisperID.Release();

			//----------------------------------------------
			// message Remove
			//----------------------------------------------
			g_pSystemMessage->Clear();
			g_pGameMessage->Clear();
			g_pNoticeMessage->Clear();

			//------------------------------------------------------
			// Remove
			//------------------------------------------------------
			if (g_pUserOption->PlayWaveMusic)
			{
//				g_pDXSoundStream->Stop();			
				DEBUG_ADD("MP3 STOP8");
#ifdef __USE_MP3__
				g_pMP3->Stop();
#else
				if( g_DXSound.IsInit() )
					BGMManager::Instance()->StopMusic();
					//g_pOGG->streamClose();
#endif
				DEBUG_ADD("MP3 STOP8 OK");

				if (g_pUserOption->PlayMusic)
				{
					int musicID = GetTitleMusicID();  // Title random pool (TitleMusic.h) - 2026-05-14

					if (musicID!=MUSICID_NULL)
					{
#ifdef __USE_MP3__					
//						g_pDXSoundStream->Load( (*g_pMusicTable)[ musicID ].FilenameWav );
//						g_pDXSoundStream->Play( FALSE );
						DEBUG_ADD("MP3 OPEN2");
						g_pMP3->Open( (*g_pMusicTable)[ musicID ].FilenameWav );
						DEBUG_ADD("MP3 OPEN2 OK");
						DEBUG_ADD("MP3 PLAY4");
						g_pMP3->Play( false );
						DEBUG_ADD("MP3 PLAY4 OK");
#else
// 						if( g_oggfile != NULL )
// 							fclose(g_oggfile);
// 						
// 						g_oggfile = NULL;
// 
// 						if( g_DXSound.IsInit() )
// 						{
// 							g_oggfile = fopen( (*g_pMusicTable)[ musicID ].FilenameWav ,"rb");
// 							if( g_oggfile != NULL )
// 							{
// 								g_pOGG->streamLoad( g_oggfile, NULL );
// 								g_pOGG->streamPlay( SOUND_PLAY_ONCE );
// 								int volume = (g_pUserOption->VolumeMusic - 15) * 250;
// 								g_pOGG->streamVolume( max( -10000, min( -1, volume ) ) );
// 							}
// 						}

						BGMManager::Instance()->PlayMusic((*g_pMusicTable)[ musicID ].FilenameWav);

						// 2026-05-19 BGM VOLUME FIX LogOut CharacterSelect'te BGM
						// max volume calmasi. VorbisSound::Open her PlayMusic'ta yeni
						// DirectSound buffer yaratiyor (default 0 dB = max). SetVolume
						// canonical pattern eksikti (cf. GameMain.cpp:1483-1485,
						// 4332-4334, 652-655 ve UIMessageManager.cpp:761).
						{
							BGMManager::Instance()->SetVolumeFromSlider(g_pUserOption->VolumeMusic);
						}
#endif
					}
				}
			}
			else
			{
				g_Music.Stop();

				if (g_pUserOption->PlayMusic)
				{
					int musicID = GetTitleMusicID();  // Title random pool (TitleMusic.h) - 2026-05-14

					if (musicID!=MUSICID_NULL)
					{
						g_Music.Play( (*g_pMusicTable)[ musicID ].Filename );
					}
				}
			}
		}
		else
		{
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_LOGOUT_DIED].GetString() );
		}
	}
	else
	{
		DEBUG_ADD("Logout Not ModeGame");
	}
}

#if __CONTENTS(__080405_FIREST_UI_UPDATE)
void
ExecuteQuitExit()
{
	//--------------------------------------------------
	// If alive
	//--------------------------------------------------
	if (g_Mode==MODE_GAME)
	{
		DEBUG_ADD("ExecuteQuitExit");

		UI_SaveUserOption();

		// Weather Pause
		SetWeather(WEATHER_CLEAR, 0);

		// If the character dies, send a resurrection packet and log out.
        // But... this doesn't really make much sense when it's actually implemented... - -;
		if (!g_pPlayer->IsAlive())
		{
			CGResurrect _CGResurrect;
				
			g_pSocket->sendPacket( &_CGResurrect );

			gC_vs_ui.FinishRequestDie();
			gC_vs_ui.FinishRequestResurrect();
			gC_vs_ui.CloseRequestShrineMinimap();
		}

		if(1)//g_pPlayer->IsAlive())
		{
			//--------------------------------------------------
			// Send a CGLogout packet to the game server.
			//--------------------------------------------------
			UI_SaveHotKeyToServer();

			CGLogout cgLogout;
			g_pSocket->sendPacket( &cgLogout );
			g_pSocket->processOutput();

			//--------------------------------------------------
			// FIX 2026-05-12: Exit Game butonu uygulamayi kapat.
			// Onceki fix PostQuitMessage'i UI cleanup'tan SONRA cagiriyordu
			// ama ReleaseGameObject + g_pUIDialog->HidePCTalkDlg + DropItem
			// + g_pSystemMessage->Clear zinciri g_pPlayer/g_pZone dangling
			// pointer kullaniyor crash, PostQuitMessage hic cagrilmiyordu.
			//
			// Cozum: socket notify sonrasi HEMEN PostQuitMessage(0) + return.
			// Cleanup'i atla. Frame loop bir tick daha (state valid, cleanup
			// yapilmadi), sonraki GetMessage WM_QUIT alir main return
			// process exit. OS process kaynaklarini temizler.
			//
			// CPS_WAITING_FOR_GC_RECONNECT_LOGIN da kaldirildi server'a
			// CGLogout zaten gonderildi, ek status server-side timeout/idle
			// ile dogal sekilde handle edilir.
			//--------------------------------------------------
			SetMode( MODE_QUIT );
			PostQuitMessage(0);
			return;

			g_CurrentFrame		= 0;

			g_bSetHotKey	= FALSE;

			//------------------------------------------------------
			// ui dialog
			//------------------------------------------------------
			g_pUIDialog->HidePCTalkDlg();
			g_pUIDialog->CloseMessageDlg();

			gC_vs_ui.DropItem();

			g_bUIInput = FALSE;

			// It deletes the recipient of the whisper.
			g_pUserInformation->WhisperID.Release();

			//----------------------------------------------
			// message Remove
			//----------------------------------------------
			g_pSystemMessage->Clear();
			g_pGameMessage->Clear();
			g_pNoticeMessage->Clear();

			// NOT: PostQuitMessage(0) yukari tasindi (CGLogout sonrasi, cleanup
			// oncesi). Buradaki cleanup zinciri crash kaynagi idi ??dead code.
		}
		else
		{
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_LOGOUT_DIED].GetString() );
		}
	}
	else
	{
		DEBUG_ADD("Logout Not ModeGame");
	}
}
#endif //__080405_FIREST_UI_UPDATE

//-----------------------------------------------------------------------------
// Play TitleMusic
//-----------------------------------------------------------------------------
void
PlayTitleMusic()
{
	//----------------------------------------------------------------
	// WAV Output
	//----------------------------------------------------------------
	if (g_pUserOption->PlayWaveMusic)
	{
		g_Music.Stop();

		if (g_pUserOption->PlayMusic)
		{
			int musicID = GetTitleMusicID();  // Title random pool (TitleMusic.h) - 2026-05-14

			if (musicID!=MUSICID_NULL)
			{
#ifdef __USE_MP3__
				LONG volume = PhoenixSliderValueToMp3Volume(g_pUserOption->VolumeMusic);
//				g_pDXSoundStream->SetVolumeLimit( volume );
				DEBUG_ADD("MP3 SetVolume2");
				g_pMP3->SetVolume( volume );
				DEBUG_ADD("MP3 SetVolume2 OK");

				DEBUG_ADD("MP3 OPEN3");
				g_pMP3->Open( (*g_pMusicTable)[ musicID ].FilenameWav );
				DEBUG_ADD("MP3 OPEN3 OK");
				DEBUG_ADD("MP3 PLAY5");
				g_pMP3->Play( false );				
				DEBUG_ADD("MP3 PLAY5 OK");
//				g_pDXSoundStream->Load( (*g_pMusicTable)[ musicID ].FilenameWav );
//				g_pDXSoundStream->Play( FALSE );				
#else
// 				if( g_oggfile != NULL )
// 					fclose(g_oggfile);
// 
// 				g_oggfile = NULL;
// 
// 				if( g_DXSound.IsInit() )
// 				{
// 					g_oggfile = fopen( (*g_pMusicTable)[ musicID ].FilenameWav, "rb") ;
// 					if( g_oggfile != NULL )
// 					{
// 						int volume = (g_pUserOption->VolumeMusic - 15) * 250;
//
// 						g_pOGG->streamVolume( max( -10000, min( -1, volume ) ) );
// 						g_pOGG->streamLoad( g_oggfile,NULL );
// 						g_pOGG->streamPlay( SOUND_PLAY_ONCE );
// 					}
// 				}

				BGMManager::Instance()->PlayMusic((*g_pMusicTable)[ musicID ].FilenameWav);

				// 2026-05-19 BGM VOLUME FIX (twin of ExecuteLogout @ line 473)
				// PlayTitleMusic() OGG branch'i SetVolume canonical follow-up'tan
				// yoksun Options'da PlayMusic checkbox toggle MODE_MAINMENU iken
				// tetiklendiginde ayni bug (max volume) ortaya kor.
				// Pattern referans GameMain.cpp:1483-1485 / 4332-4334 / 652-655.
				{
					BGMManager::Instance()->SetVolumeFromSlider(g_pUserOption->VolumeMusic);
				}
#endif
			}
		}
		else
		{
//			g_pDXSoundStream->Stop();
			DEBUG_ADD("MP3 STOP9");
#ifdef __USE_MP3__
			g_pMP3->Stop();
#else
			if( g_DXSound.IsInit() )
				BGMManager::Instance()->StopMusic();
				//g_pOGG->streamClose();
#endif
			DEBUG_ADD("MP3 STOP9 OK");
		}
	}
	//----------------------------------------------------------------
	// MID Output
	//----------------------------------------------------------------
	else
	{
//		g_pDXSoundStream->Stop();
		DEBUG_ADD("MP3 STOP10");
#ifdef __USE_MP3__
		g_pMP3->Stop();
#else
		if( g_DXSound.IsInit() )
			BGMManager::Instance()->StopMusic();
//			g_pOGG->streamClose();
#endif
		DEBUG_ADD("MP3 STOP10 OK");

		if (g_pUserOption->PlayMusic)
		{
			if (g_Music.IsPlay())
			{
				g_Music.Resume();
			}
			else //if (g_Mode==MODE_GAME)
			{
				WORD volume = BGMManager::SliderValueToMidiWord(g_pUserOption->VolumeMusic);

				if (g_Music.IsPause())
				{
					g_Music.SetVolume( volume );					
					g_Music.Resume();
				}
				else
				{
					g_Music.Pause();
					g_Music.SetVolume( volume );
					g_Music.Resume();					

					int musicID = GetTitleMusicID();  // Title random pool (TitleMusic.h) - 2026-05-14

					if (musicID!=MUSICID_NULL)
					{
						g_Music.Play( (*g_pMusicTable)[ musicID ].Filename );
					}
				}					
			}				
		}
		else
		{
			if (g_Music.IsPlay())
			{
				g_Music.Pause();
			}
			else
			{
				g_Music.Stop();
			}
		}
	}
}

//-----------------------------------------------------------------------------
// PlayGameMusic
//-----------------------------------------------------------------------------
void
PlayGameMusic()
{
	//----------------------------------------------------------------
	// WAV Output
	//----------------------------------------------------------------
	if (g_pUserOption->PlayWaveMusic)
	{
		g_Music.Stop();

		if (g_pUserOption->PlayMusic)
		{
#ifdef __USE_MP3__
			LONG volume = PhoenixSliderValueToMp3Volume(g_pUserOption->VolumeMusic);
//			g_pDXSoundStream->SetVolumeLimit( volume );
			DEBUG_ADD("MP3 SetVolume3");
			g_pMP3->SetVolume( volume );
			DEBUG_ADD("MP3 SetVolume3 OK");
#else
			if( g_DXSound.IsInit() )
			{
//				g_pOGG->streamVolume( max( -10000, min( -1, volume ) ) );
				BGMManager::Instance()->SetVolumeFromSlider(g_pUserOption->VolumeMusic);
			}
#endif

			PlayMusicCurrentZone();				
		}
		else
		{					
//			g_pDXSoundStream->Stop();				
			DEBUG_ADD("MP3 STOP11");
#ifdef __USE_MP3__
			g_pMP3->Stop();
#else
			if( g_DXSound.IsInit() )
				BGMManager::Instance()->StopMusic();
				//g_pOGG->streamClose();
#endif
			DEBUG_ADD("MP3 STOP11 OK");
		}
	}
	//----------------------------------------------------------------
	// MID Output
	//----------------------------------------------------------------
	else
	{
//		g_pDXSoundStream->Stop();
		DEBUG_ADD("MP3 STOP12");
#ifdef __USE_MP3__
		g_pMP3->Stop();
#else
		if( g_DXSound.IsInit() )
			BGMManager::Instance()->StopMusic();
			//g_pOGG->streamClose();
#endif
		
		DEBUG_ADD("MP3 STOP12 OK");

		if (g_pUserOption->PlayMusic)
		{
			DEBUG_ADD("MP3 STOP12 OK2");
			if (g_Music.IsPlay())
			{
				DEBUG_ADD("MP3 STOP12 RESUME");
				g_Music.Resume();
				DEBUG_ADD("MP3 STOP12 RESUME OK");
			}
			else //if (g_Mode==MODE_GAME)
			{
				DEBUG_ADD("MP3 STOP12 pp");
				WORD volume = BGMManager::SliderValueToMidiWord(g_pUserOption->VolumeMusic);
				DEBUG_ADD("MP3 STOP12 pp1");
				g_Music.SetVolume( volume );
				DEBUG_ADD("MP3 STOP12 pp2");
				
				PlayMusicCurrentZone();
				DEBUG_ADD("MP3 STOP12 PlayMusicCurrentZone OK");
			}				
		}
		else
		{
			if (g_Music.IsPlay())
			{
				g_Music.Pause();
			}
			else
			{
				g_Music.Stop();
			}
		}
	}
	DEBUG_ADD("PlayGameMusic OK");
}

//-----------------------------------------------------------------------------
//
// constructor / destructor
//
//-----------------------------------------------------------------------------
UIMessageManager::UIMessageManager()
{
	for (int i=0; i<MAX_UI_MESSAGE; i++)
	{
		m_UIMessageFunction[i] = NULL;
	}
}

UIMessageManager::~UIMessageManager()
{
	// Just for testing... - -;
	for (int i=0; i<MAX_UI_MESSAGE; i++)
	{
		m_UIMessageFunction[i] = NULL;
	}
}

//-----------------------------------------------------------------------------
//
// member functions
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Init
//-----------------------------------------------------------------------------
void
UIMessageManager::Init()
{	
	m_UIMessageFunction[UI_STORAGE_BUY] = Execute_UI_STORAGE_BUY;
	m_UIMessageFunction[UI_SELECT_STORAGE_SLOT] = Execute_UI_SELECT_STORAGE_SLOT;
	m_UIMessageFunction[UI_RUNNING_GAMEMENU] = Execute_UI_RUNNING_GAMEMENU;
	m_UIMessageFunction[UI_FINISH_LEVELUP_BUTTON] = Execute_UI_FINISH_LEVELUP_BUTTON;
	m_UIMessageFunction[UI_CLICK_BONUS_POINT] = Execute_UI_CLICK_BONUS_POINT;
	m_UIMessageFunction[UI_INFO] = Execute_UI_INFO;
//	m_UIMessageFunction[UI_MINIMAP_TOGGLE] = Execute_UI_MINIMAP_TOGGLE;
	m_UIMessageFunction[UI_DROP_MONEY] = Execute_UI_DROP_MONEY;
	m_UIMessageFunction[UI_EXCHANGE_REQUEST_CANCEL] = Execute_UI_EXCHANGE_REQUEST_CANCEL;
	m_UIMessageFunction[UI_EXCHANGE_ACCEPT] = Execute_UI_EXCHANGE_ACCEPT;
	m_UIMessageFunction[UI_EXCHANGE_MONEY] = Execute_UI_EXCHANGE_MONEY;
	m_UIMessageFunction[UI_WITHDRAW_MONEY] = Execute_UI_WITHDRAW_MONEY;
	m_UIMessageFunction[UI_ITEM_SELECT_EXCHANGE] = Execute_UI_ITEM_SELECT_EXCHANGE;
	m_UIMessageFunction[UI_DEPOSIT_MONEY] = Execute_UI_DEPOSIT_MONEY;
	m_UIMessageFunction[UI_CHECK_EXIST_ID] = Execute_UI_CHECK_EXIST_ID;
	m_UIMessageFunction[UI_RUN_NEWUSER_REGISTRATION] = Execute_UI_RUN_NEWUSER_REGISTRATION;
	m_UIMessageFunction[UI_INFO_CLOSE] = Execute_UI_INFO_CLOSE;
	m_UIMessageFunction[UI_GAMEMENU_CONTINUE] = Execute_UI_GAMEMENU_CONTINUE;

	//
	// PDS
	//
//	m_UIMessageFunction[UI_CHANGE_PCS_CONNECTED_SLOT] = Execute_UI_CHANGE_PCS_CONNECTED_SLOT;		// I clicked on someone else who was already connected.
//	m_UIMessageFunction[UI_PLEASE_PCS_CONNECT_ME] = Execute_UI_PLEASE_PCS_CONNECT_ME;			// I clicked on the waiting room where people were gathered.
	//m_UIMessageFunction[UI_PCS_CONNECTOR_GRANTED] = Execute_UI_PCS_CONNECTOR_GRANTED;			// I accepted the connection from the user in the waiting room.
//	m_UIMessageFunction[UI_QUIT_PCS_ONLINE_MODE] = Execute_UI_QUIT_PCS_ONLINE_MODE;			// I clicked the "Quit" button in PCS online mode.
//	m_UIMessageFunction[UI_END_PCS] = Execute_UI_END_PCS;								// While I was sending it, I pressed "End."
//	m_UIMessageFunction[UI_SEND_PCS_NUMBER] = Execute_UI_SEND_PCS_NUMBER;
//	m_UIMessageFunction[UI_PDS_CLOSED] = Execute_UI_PDS_CLOSED;							// Slayer PDS has closed. !Current connections will remain active.

//	m_UIMessageFunction[UI_PLEASE_SET_SLAYER_VALUE] = Execute_UI_PLEASE_SET_SLAYER_VALUE;		// I opened the Gage menu in Slayer PDS.

//	m_UIMessageFunction[UI_LEARN_SLAYER_SKILL] = Execute_UI_LEARN_SLAYER_SKILL;				// I selected a skill from the skill tree.
//	m_UIMessageFunction[UI_CLOSE_SKILL_VIEW] = Execute_UI_CLOSE_SKILL_VIEW;

	m_UIMessageFunction[UI_BACKGROUND_MOUSE_FOCUS] = Execute_UI_BACKGROUND_MOUSE_FOCUS;			// If the mouse pointer is stationary within a UI window and that window closes, notify the client.
	m_UIMessageFunction[UI_REMOVE_BACKGROUND_MOUSE_FOCUS] = Execute_UI_REMOVE_BACKGROUND_MOUSE_FOCUS;// When a topmost window, such as the Game Menu, is displayed, it is best to remove the background focus.

	//
	// Store Message
	//
	m_UIMessageFunction[UI_BUY_ITEM] = Execute_UI_BUY_ITEM;

	m_UIMessageFunction[UI_SELL_ITEM] = Execute_UI_SELL_ITEM;
	m_UIMessageFunction[UI_SELL_ALL_ITEM] = Execute_UI_SELL_ALL_ITEM;
	m_UIMessageFunction[UI_REPAIR_ITEM] = Execute_UI_REPAIR_ITEM;
	m_UIMessageFunction[UI_SILVERING_ITEM] = Execute_UI_SILVERING_ITEM;
	m_UIMessageFunction[UI_ITEM_SELL_FINISHED] = Execute_UI_ITEM_SELL_FINISHED;
	m_UIMessageFunction[UI_ITEM_REPAIR_FINISHED] = Execute_UI_ITEM_REPAIR_FINISHED;
	m_UIMessageFunction[UI_ITEM_SILVERING_FINISHED] = Execute_UI_ITEM_SILVERING_FINISHED;

	m_UIMessageFunction[UI_CLOSE_SHOP] = Execute_UI_CLOSE_SHOP;
	m_UIMessageFunction[UI_CLOSE_STORAGE] = Execute_UI_CLOSE_STORAGE;
	m_UIMessageFunction[UI_CLOSE_EXCHANGE] = Execute_UI_CLOSE_EXCHANGE;
	m_UIMessageFunction[UI_CANCEL_EXCHANGE] = Execute_UI_CANCEL_EXCHANGE;
	m_UIMessageFunction[UI_OK_EXCHANGE] = Execute_UI_OK_EXCHANGE;

	m_UIMessageFunction[UI_CHARACTER_MANAGER_FINISHED] = Execute_UI_CHARACTER_MANAGER_FINISHED;	// It notifies you when you return to the home screen from the Characters Window.
	m_UIMessageFunction[UI_TERMINATION] = Execute_UI_TERMINATION;						// End
	m_UIMessageFunction[UI_LOGIN] = Execute_UI_LOGIN;								// User login request
	m_UIMessageFunction[UI_LOGOUT] = Execute_UI_LOGOUT;
	m_UIMessageFunction[UI_NEW_OPTION] = Execute_UI_NEW_OPTION;								// Game Menu butonlar??(Gear/Skill/Quest/Pet/Friend/Party/TMall/Option/Help)
	m_UIMessageFunction[UI_REQUEST_TMALL_LIST] = Execute_UI_REQUEST_TMALL_LIST;
	m_UIMessageFunction[UI_REQUEST_CASH_CHARGE] = Execute_UI_REQUEST_CASH_CHARGE;	// 2026-08-20 PayPal Cash Charge
	m_UIMessageFunction[UI_BUY_TMALL_ITEM] = Execute_UI_BUY_TMALL_ITEM;
	m_UIMessageFunction[UI_REQUEST_MILITARY_SLOTDATA] = Execute_UI_REQUEST_MILITARY_SLOTDATA;

#if __CONTENTS(__080405_FIREST_UI_UPDATE)
	m_UIMessageFunction[UI_QUITEXIT] = Execute_UI_QUITEXIT;
#endif //__080405_FIREST_UI_UPDATE

	m_UIMessageFunction[UI_NEW_USER_REGISTRATION] = Execute_UI_NEW_USER_REGISTRATION;			// New account registration request
	m_UIMessageFunction[UI_CONNECT] = Execute_UI_CONNECT;							   // Game connection.
	m_UIMessageFunction[UI_CHAT_RETURN] = Execute_UI_CHAT_RETURN;						// Enter text in the chat window.
//	m_UIMessageFunction[UI_CHAT_SELECT_NAME] = Execute_UI_CHAT_SELECT_NAME;						// Select a name for the chat window.
	m_UIMessageFunction[UI_SELECT_SKILL] = Execute_UI_SELECT_SKILL;						// Skill selection.
	m_UIMessageFunction[UI_CANCEL_SELECT_SKILL] = Execute_UI_CANCEL_SELECT_SKILL;				// The skill selection has been canceled.

	//
	// Item message
	//

	// drop
	m_UIMessageFunction[UI_ITEM_DROP_TO_CLIENT] = Execute_UI_ITEM_DROP_TO_CLIENT;				// I dropped the item on the floor.
	m_UIMessageFunction[UI_ITEM_DROP_TO_INVENTORY] = Execute_UI_ITEM_DROP_TO_INVENTORY; 		// Place it in the inventory.
	//m_UIMessageFunction[UI_ITEM_DROP_TO_TRADEGRID] = Execute_UI_ITEM_DROP_TO_TRADEGRID;			// Place it on the exchange grid.
	m_UIMessageFunction[UI_ITEM_DROP_TO_GEAR] = Execute_UI_ITEM_DROP_TO_GEAR;				// Place it in the gear.
	m_UIMessageFunction[UI_ITEM_DROP_TO_QUICKSLOT] = Execute_UI_ITEM_DROP_TO_QUICKSLOT;			// Place it in the gear.

	// pick up
	// When the client retrieves data, the flow is Client ?? UI.
	//m_UIMessageFunction[UI_ITEM_PICKUP_FROM_TRADEGRID] = Execute_UI_ITEM_PICKUP_FROM_TRADEGRID;
	m_UIMessageFunction[UI_ITEM_PICKUP_FROM_INVENTORY] = Execute_UI_ITEM_PICKUP_FROM_INVENTORY;
	m_UIMessageFunction[UI_ITEM_PICKUP_FROM_GEAR] = Execute_UI_ITEM_PICKUP_FROM_GEAR;
	m_UIMessageFunction[UI_ITEM_PICKUP_FROM_QUICKSLOT] = Execute_UI_ITEM_PICKUP_FROM_QUICKSLOT;

	// insert to other Item
	//m_UIMessageFunction[UI_ITEM_INSERT_FROM_TRADEGRID] = Execute_UI_ITEM_INSERT_FROM_TRADEGRID;
	m_UIMessageFunction[UI_ITEM_INSERT_FROM_INVENTORY] = Execute_UI_ITEM_INSERT_FROM_INVENTORY;
	m_UIMessageFunction[UI_ITEM_INSERT_FROM_GEAR] = Execute_UI_ITEM_INSERT_FROM_GEAR;
	m_UIMessageFunction[UI_ITEM_INSERT_FROM_QUICKSLOT] = Execute_UI_ITEM_INSERT_FROM_QUICKSLOT;  // If there is a backlog - Not yet supported -

	// use
	m_UIMessageFunction[UI_ITEM_USE] = Execute_UI_ITEM_USE;							// Used in Inventory.
	m_UIMessageFunction[UI_ITEM_USE_QUICKSLOT] = Execute_UI_ITEM_USE_QUICKSLOT;				// Use in the Quick Item Slot.

	//
	// Character management
	//
	m_UIMessageFunction[UI_NEW_CHARACTER] = Execute_UI_NEW_CHARACTER;
	m_UIMessageFunction[UI_DELETE_CHARACTER] = Execute_UI_DELETE_CHARACTER;

	//m_UIMessageFunction[UI_CLOSE_BBS] = Execute_UI_CLOSE_BBS;

//	m_UIMessageFunction[UI_CHANGE_GAME_OPTION] = Execute_UI_CHANGE_GAME_OPTION;
//	m_UIMessageFunction[UI_CLOSE_GAME_OPTION] = Execute_UI_CLOSE_GAME_OPTION;
	m_UIMessageFunction[UI_CHANGE_OPTION] = Execute_UI_CHANGE_OPTION;
	m_UIMessageFunction[UI_CLOSE_OPTION] = Execute_UI_CLOSE_OPTION;
	m_UIMessageFunction[UI_APPLY_RESOLUTION] = Execute_UI_APPLY_RESOLUTION;	// O3
	m_UIMessageFunction[UI_RESET_BUTTON] = Execute_UI_RESET_BUTTON;
	m_UIMessageFunction[UI_CLOSE_BOOKCASE] = Execute_UI_CLOSE_BOOKCASE;
	m_UIMessageFunction[UI_CLOSE_BRIEFING] = Execute_UI_CLOSE_BRIEFING;
	m_UIMessageFunction[UI_CLOSE_COMPUTER] = Execute_UI_CLOSE_COMPUTER;
	m_UIMessageFunction[UI_CLOSE_TUTORIAL_EXIT] = Execute_UI_CLOSE_TUTORIAL_EXIT;

	m_UIMessageFunction[UI_CLOSE_DESC_DIALOG] = Execute_UI_CLOSE_DESC_DIALOG;
	m_UIMessageFunction[UI_CLOSE_ELEVATOR] = Execute_UI_CLOSE_ELEVATOR;
	m_UIMessageFunction[UI_SELECT_ELEVATOR] = Execute_UI_SELECT_ELEVATOR;

	//m_UIMessageFunction[UI_SELECT_SERVER] = Execute_UI_SELECT_SERVER;

	//m_UIMessageFunction[UI_REQUEST_SERVER_LIST] = Execute_UI_REQUEST_SERVER_LIST;

	m_UIMessageFunction[UI_ITEM_TO_QUICKITEMSLOT] = Execute_UI_ITEM_TO_QUICKITEMSLOT;

	m_UIMessageFunction[UI_CLOSE_SLAYER_PORTAL] = Execute_UI_CLOSE_SLAYER_PORTAL;
	m_UIMessageFunction[UI_SLAYER_PORTAL] = Execute_UI_SLAYER_PORTAL;

	m_UIMessageFunction[UI_FINISH_REQUEST_PARTY_BUTTON] = Execute_UI_FINISH_REQUEST_PARTY_BUTTON;
	m_UIMessageFunction[UI_PARTY_REQUEST_CANCEL] = Execute_UI_PARTY_REQUEST_CANCEL;
	m_UIMessageFunction[UI_PARTY_ACCEPT] = Execute_UI_PARTY_ACCEPT;

	m_UIMessageFunction[UI_CLOSE_PARTY_MANAGER] = Execute_UI_CLOSE_PARTY_MANAGER;
	m_UIMessageFunction[UI_AWAY_PARTY] = Execute_UI_AWAY_PARTY;

	m_UIMessageFunction[UI_FINISH_REQUEST_DIE_BUTTON] = Execute_UI_FINISH_REQUEST_DIE_BUTTON;

	m_UIMessageFunction[UI_CLOSE_TEAM_LIST] = Execute_UI_CLOSE_TEAM_LIST;
	m_UIMessageFunction[UI_CLOSE_TEAM_INFO] = Execute_UI_CLOSE_TEAM_INFO;
	m_UIMessageFunction[UI_CLOSE_TEAM_MEMBER_INFO] = Execute_UI_CLOSE_TEAM_MEMBER_INFO;
	m_UIMessageFunction[UI_CLOSE_TEAM_REGIST] = Execute_UI_CLOSE_TEAM_REGIST;
	m_UIMessageFunction[UI_CLOSE_TEAM_MEMBER_LIST] = Execute_UI_CLOSE_TEAM_MEMBER_LIST;
	
	m_UIMessageFunction[UI_REQUEST_GUILD_INFO] = Execute_UI_REQUEST_GUILD_INFO;
	m_UIMessageFunction[UI_REQUEST_GUILD_MEMBER_LIST] = Execute_UI_REQUEST_GUILD_MEMBER_LIST;
	m_UIMessageFunction[UI_MODIFY_GUILD_MEMBER] = Execute_UI_MODIFY_GUILD_MEMBER;

	m_UIMessageFunction[UI_SELECT_TEAM_MEMBER_LIST] = Execute_UI_SELECT_TEAM_MEMBER_LIST;		// void_ptr = TEAM_NAME
	m_UIMessageFunction[UI_SELECT_READY_TEAM_LIST] = Execute_UI_SELECT_READY_TEAM_LIST;		// void_ptr = TEAM_NAME
	m_UIMessageFunction[UI_SELECT_REGIST_TEAM_LIST] = Execute_UI_SELECT_REGIST_TEAM_LIST;		// void_ptr = TEAM_NAME

	m_UIMessageFunction[UI_JOIN_READY_TEAM] = Execute_UI_JOIN_READY_TEAM;				// void_ptr = TEAM_NAME
	m_UIMessageFunction[UI_JOIN_REGIST_TEAM] = Execute_UI_JOIN_REGIST_TEAM;			// void_ptr = TEAM_NAME

	m_UIMessageFunction[UI_REGIST_GUILD_MEMBER] = Execute_UI_REGIST_GUILD_MEMBER;			// void_ptr = introduction max:150byte Close the window!
	m_UIMessageFunction[UI_REGIST_GUILD_TEAM] = Execute_UI_REGIST_GUILD_TEAM;			// left = TEAM_NAME, void_ptr = introduction max:150byte Close the window!

	m_UIMessageFunction[UI_CLOSE_FILE_DIALOG] = Execute_UI_CLOSE_FILE_DIALOG;


	m_UIMessageFunction[UI_CONNECT_SERVER] = Execute_UI_CONNECT_SERVER;
	m_UIMessageFunction[UI_CLOSE_SERVER_SELECT] = Execute_UI_CLOSE_SERVER_SELECT;

	m_UIMessageFunction[UI_NEWCHARACTER_CHECK] = Execute_UI_NEWCHARACTER_CHECK;
	
//	m_UIMessageFunction[UI_SELECT_EXPLOSIVE] = Execute_UI_SELECT_EXPLOSIVE;
//	m_UIMessageFunction[UI_CLOSE_SELECT_EXPLOSIVE] = Execute_UI_CLOSE_SELECT_EXPLOSIVE;

	m_UIMessageFunction[UI_ENCHANT_ACCEPT] = Execute_UI_ENCHANT_ACCEPT;
	m_UIMessageFunction[UI_ENCHANT_CANCEL] = Execute_UI_ENCHANT_CANCEL;

	m_UIMessageFunction[UI_MESSAGE_BOX] = Execute_UI_MESSAGE_BOX;

	m_UIMessageFunction[UI_CLOSE_OTHER_INFO] = Execute_UI_CLOSE_OTHER_INFO;

	m_UIMessageFunction[UI_MODIFY_TEAM_INFO] = Execute_UI_MODIFY_TEAM_INFO;
	m_UIMessageFunction[UI_MODIFY_GUILD_MEMBER_INTRO] = Execute_UI_MODIFY_GUILD_MEMBER_INTRO;

	m_UIMessageFunction[UI_SEND_NAME_FOR_SOUL_CHAIN] = Execute_UI_SEND_NAME_FOR_SOUL_CHAIN;
	m_UIMessageFunction[UI_CLOSE_TRACE_WINDOW] = Execute_UI_CLOSE_TRACE_WINDOW;

	m_UIMessageFunction[UI_RUN_CONNECT] = Execute_UI_RUN_CONNECT;

	m_UIMessageFunction[UI_SELECT_GRADE_SKILL] = Excute_UI_SELECT_GRADE_SKILL;

	m_UIMessageFunction[UI_USE_XMAS_TREE] = Excute_UI_USE_XMAS_TREE;
	m_UIMessageFunction[UI_CLOSE_XMAS_CARD_WINDOW] = Excute_UI_CLOSE_XMAS_CARD_WINDOW;

	m_UIMessageFunction[UI_SEND_BRING_FEE] = Excute_UI_SEND_BRING_FEE;
	m_UIMessageFunction[UI_CLOSE_BRING_FEE_WINDOW] = Excute_UI_CLOSE_BRING_FEE_WINDOW;

	m_UIMessageFunction[UI_CLOSE_WAR_LIST_WINDOW] = Excute_UI_CLOSE_WAR_LIST;

	m_UIMessageFunction[UI_CLOSE_BLOOD_BIBLE_STATUS] = Execute_UI_CLOSE_BLOOD_BIBLE_STATUS;
	m_UIMessageFunction[UI_SEND_NAME_FOR_COUPLE] = Execute_UI_SEND_NAME_FOR_COUPLE;
	m_UIMessageFunction[UI_CLOSE_INPUT_NAME] = Execute_UI_CLOSE_INPUT_NAME_WINDOW;

	m_UIMessageFunction[UI_ITEM_USE_GEAR] = Execute_UI_ITEM_USE_GEAR;
	m_UIMessageFunction[UI_GO_BILING_PAGE] = Execute_GO_BILING_PAGE;
	m_UIMessageFunction[UI_CLOSE_POPUP_MESSAGE] = Execute_UI_CLOSE_POPUP_MESSAGE;
	m_UIMessageFunction[UI_CLOSE_QUEST_STATUS] = Execute_UI_CLOSE_QUEST_STATUS;

	m_UIMessageFunction[UI_CLOSE_LOTTERY_CARD] = Execute_UI_CLOSE_LOTTERY_CARD;
	m_UIMessageFunction[UI_LOTTERY_CARD_STATUS] = Execute_UI_LOTTERY_CARD_STATUS;
	m_UIMessageFunction[UI_FINISH_SCRATCH_LOTTERY]	= Execute_UI_FINISH_SCRATCH_LOTTERY;
	m_UIMessageFunction[UI_CLOSE_IMAGE_NOTICE] = Execute_UI_CLOSE_IMAGE_NOTICE;

	m_UIMessageFunction[UI_CLOSE_ITEM_LIST]	= Execute_UI_CLOSE_ITEM_LIST;
	m_UIMessageFunction[UI_SELECT_ITEM_FROM_SHOP] = Execute_UI_SELECT_ITEM_FROM_SHOP;
	m_UIMessageFunction[UI_CLOSE_BULLETIN_BOARD_WINDOW] = Execute_UI_CLOSE_BULLETIN_BOARD;

	m_UIMessageFunction[UI_TRANS_ITEM_CANCEL] = Execute_UI_TRANS_ITEM_CANCEL;
	m_UIMessageFunction[UI_TRANS_ITEM_ACCEPT] = Execute_UI_TRANS_ITEM_ACCEPT;
	m_UIMessageFunction[UI_FINISH_REQUEST_RESURRECT_BUTTON] = Execute_UI_REQUEST_RESURRECT;

	m_UIMessageFunction[UI_CLOSE_MIXING_FORGE] = Execute_UI_CLOSE_MIXING_FORGE;
	m_UIMessageFunction[UI_USE_MIXING_FORGE] = Execute_UI_MIXING_FORGE;

	m_UIMessageFunction[UI_CLOSE_REMOVE_OPTION] = Execute_UI_CLOSE_REMOVE_OPTION;
	m_UIMessageFunction[UI_SEND_REMOVE_OPTION] = Execute_UI_SEND_REMOVE_OPTION;

	m_UIMessageFunction[UI_CLOSE_OUSTERS_SKILL_INFO] = Execute_UI_CLOSE_OUSTERS_SKILL_INFO;
	m_UIMessageFunction[UI_OUSTERS_LEARN_SKILL] = Execute_UI_LEARN_OUSTERS_SKILL;

	m_UIMessageFunction[UI_RUN_LEVELUP] = Execute_UI_RUN_LEVELUP;
	m_UIMessageFunction[UI_CLOSE_HORN] = Execute_UI_CLOSE_HORN;
	m_UIMessageFunction[UI_OUSTERS_DOWN_SKILL] = Execute_UI_OUSTERS_DOWN_SKILL;	
	
	m_UIMessageFunction[UI_CLEAR_ALL_STAGE] = Execute_UI_CLEAR_ALL_STAGE;
	m_UIMessageFunction[UI_CLOSE_FINDING_MINE] = Execute_UI_CLOSE_FINDING_MINE;
	m_UIMessageFunction[UI_CLOSE_NEMONEMO] = Execute_UI_CLOSE_NEMONEMO;
	m_UIMessageFunction[UI_CLOSE_PUSHPUSH] = Execute_UI_CLOSE_PUSHPUSH;
	m_UIMessageFunction[UI_CLOSE_CRAZY_MINE] = Execute_UI_CLOSE_CRAZY_MINE;
	m_UIMessageFunction[UI_CLOSE_ARROW_TILE] = Execute_UI_CLOSE_ARROW_TILE;
	m_UIMessageFunction[UI_CLEAR_STAGE] = Execute_UI_CLEAR_STAGE;
	m_UIMessageFunction[UI_REQUEST_FORCE_DIE] = Execute_UI_FORCE_DIE;
	m_UIMessageFunction[UI_ADD_ITEM_TO_CODE_SHEET] = Execute_UI_ADD_ITEM_TO_CODE_SHEET;
	m_UIMessageFunction[UI_SEND_BUG_REPORT] = Execute_UI_SEND_BUG_REPORT;
	m_UIMessageFunction[UI_GO_TO_BEGINNER_ZONE] = Execute_UI_GO_BEGINNER_ZONE;
	m_UIMessageFunction[UI_POPUP_MESSAGE_OK] = Execute_UI_POPUP_MESSAGE_OK;

	m_UIMessageFunction[UI_CLOSE_SHRINE_MINIMAP] = Execute_UI_CLOSE_SHRINE_MINIMAP;
	m_UIMessageFunction[UI_WARP_REGEN_TOWER] = Execute_UI_WARP_TO_REGEN_TOWER;
	m_UIMessageFunction[UI_CLOSE_MAILBOX] = Execute_UI_CLOSE_MAILBOX;
	m_UIMessageFunction[UI_CLOSE_PET_INFO] = Execute_UI_CLOSE_PET_INFO;
	m_UIMessageFunction[UI_PET_GAMBLE] = Execute_UI_PET_GAMBLE;
	m_UIMessageFunction[UI_CLOSE_USE_PET_FOOD] = Execute_UI_CLOSE_USE_PET_FOOD;

	// 2004, 5, 11 sobeit add start - Pet Boarding
	m_UIMessageFunction[UI_CLOSE_PETSTORAGE] = Execute_UI_CLOSE_PETSTORAGE;
	m_UIMessageFunction[UI_CLOSE_KEEP_PETITEM] = Execute_UI_CLOSE_KEEP_PETITEM;
	m_UIMessageFunction[UI_CLOSE_GET_KEEP_PETITEM] = Execute_UI_CLOSE_GET_KEEP_PETITEM;
	m_UIMessageFunction[UI_KEEP_PETITEM] = Execute_UI_KEEP_PETITEM;
	m_UIMessageFunction[UI_GET_KEEP_PETITEM] = Execute_UI_GET_KEEP_PETITEM;
	// 2004, 5, 11 sobeit add end -  Pet Boarding

	// 2004, 5, 28 sobeit add start - Regarding SMS
	m_UIMessageFunction[UI_CLOSE_SMS_MESSAGE] = Execute_UI_CLOSE_SMS_MESSAGE;
	m_UIMessageFunction[UI_SEND_SMS_MESSAGE] = Execute_UI_SEND_SMS_MESSAGE;
	m_UIMessageFunction[UI_SMS_OPEN_LIST] = Execute_UI_SMS_OPEN_LIST;
	m_UIMessageFunction[UI_SMS_RECORD] = Execute_UI_SMS_RECORD;
	m_UIMessageFunction[UI_SMS_DELETE] = Execute_UI_SMS_DELETE;
	m_UIMessageFunction[UI_CLOSE_SMS_LIST] = Execute_UI_CLOSE_SMS_LIST;
	m_UIMessageFunction[UI_CLOSE_SMS_RECORD] = Execute_UI_CLOSE_SMS_RECORD;
	m_UIMessageFunction[UI_SMS_ADD_SEND_LIST] = Execute_UI_SMS_ADD_SEND_LIST;
	// 2004, 5, 28 sobeit add end

	// 2004, 6, 15 sobeit add start - nameing Related
	m_UIMessageFunction[UI_CLOSE_NAMING] = Execute_UI_CLOSE_NAMING;
	m_UIMessageFunction[UI_CHANGE_CUSTOM_NAMING] = Execute_UI_CHANGE_CUSTOM_NAMING;
	m_UIMessageFunction[UI_SELECT_NAMING] = Execute_UI_SELECT_NAMING;
	m_UIMessageFunction[UI_CLOSE_NAMING_CHANGE] = Execute_UI_CLOSE_NAMING_CHANGE;
	m_UIMessageFunction[UI_RUN_NAMING_CHANGE] = Execute_UI_RUN_NAMING_CHANGE;
	// 2004, 6, 15 sobeit add end

	// 2004, 7, 12 sobeit add start - quest Related
	m_UIMessageFunction[UI_CLOSE_QUEST_MANAGER] = Execute_UI_CLOSE_QUEST_MANAGER;
	m_UIMessageFunction[UI_CLOSE_QUEST_LIST] = Execute_UI_CLOSE_QUEST_LIST;
	m_UIMessageFunction[UI_CLOSE_QUEST_DETAIL] = Execute_UI_CLOSE_QUEST_DETAIL;
	m_UIMessageFunction[UI_CLOSE_QUEST_MISSION] = Execute_UI_CLOSE_QUEST_MISSION;
	m_UIMessageFunction[UI_CLOSE_QUEST_ITEM] = Execute_UI_CLOSE_QUEST_ITEM;
	m_UIMessageFunction[UI_CLOSE_QUEST_ICON] = Execute_UI_CLOSE_QUEST_ICON;
	m_UIMessageFunction[UI_GQUEST_ACCEPT] = Execute_UI_GQUEST_ACCEPT;
	m_UIMessageFunction[UI_GQUEST_GIVEUP] = Execute_UI_GQUEST_GIVEUP;

	// 2004, 7, 12 sobeit add end - quest Related

	m_UIMessageFunction[UI_ITEM_USE_GQUEST_ITEM] = Execute_UI_ITEM_USE_GQUEST_ITEM;

	// 2004,10.9 csm guild Related 
	m_UIMessageFunction[UI_REQUEST_UNION] =	 Execute_UI_ITEM_USE_REQUEST_UNION;
	m_UIMessageFunction[UI_REQUEST_UNION_QUIT] =	 Execute_UI_ITEM_USE_QUIT;
	m_UIMessageFunction[UI_REQUEST_UNION_EXPERGUILD] =	 Execute_UI_ITEM_USE_EXPER;
	m_UIMessageFunction[UI_REQUEST_UNION_REQUEST_INFO] =   Execute_UI_ITEM_USE_UNION_INFO;
	m_UIMessageFunction[UI_REQUEST_UNION_REQUEST_GUILD_LIST] =		Execute_UI_ITEM_USE_REQUEST_GUILD_LIST;
	m_UIMessageFunction[UI_REQUEST_UNION_ACCEPT] =		Execute_UI_ITEM_USE_UNION_ACCEPT;
	m_UIMessageFunction[UI_REQUEST_UNION_DENY] =	Execute_UI_ITEM_USE_UNION_DENY;
	m_UIMessageFunction[UI_REQUEST_UNION_QUIT_ACCEPT] =		Execute_UI_ITEM_USE_UNION_QUIT_ACCEPT;
	m_UIMessageFunction[UI_REQUEST_UNION_QUIT_DENY] =	Execute_UI_ITEM_USE_UNION_QUIT_DENY;

	m_UIMessageFunction[UI_RECALL_BY_NAME] =	 Execute_UI_RECALL_BY_NAME;

	m_UIMessageFunction[UI_MODIFY_TAX] =	 Execute_UI_UI_MODIFY_TAX;

	m_UIMessageFunction[UI_APPOINT_SUBMASTER] =	 Execute_UI_APPOINT_SUBMASTER;


	m_UIMessageFunction[UI_DISPLAY_ITEM] =	             Execute_UI_DISPLAY_ITEM;        
    m_UIMessageFunction[UI_UNDISPLAY_ITEM] =	         Execute_UI_UNDISPLAY_ITEM;
    m_UIMessageFunction[UI_STORE_SIGN] =	             Execute_UI_STORE_SIGN;
    m_UIMessageFunction[UI_STORE_OPEN] =				 Execute_UI_STORE_OPEN;
    m_UIMessageFunction[UI_STORE_CLOSE] =				 Execute_UI_STORE_CLOSE;
	m_UIMessageFunction[UI_REQUEST_STORE_INFO] =		 Execute_UI_REQUEST_STORE_INFO;
	m_UIMessageFunction[UI_MY_STORE_INFO] =				 Execute_UI_MY_STORE_INFO;
    m_UIMessageFunction[UI_OTHER_STORE_INFO] =			Execute_UI_OTHER_STORE_INFO;
    m_UIMessageFunction[UI_BUY_STORE_ITEM] =			Execute_UI_BUY_STORE_ITEM;
    m_UIMessageFunction[UI_REMOVE_STORE_ITEM] =			Execute_UI_REMOVE_STORE_ITEM;
    m_UIMessageFunction[UI_ADD_STORE_ITEM] =			Execute_UI_ADD_STORE_ITEM;
	m_UIMessageFunction[UI_CLOSE_PERSNALSHOP] =			Execute_UI_CLOSE_PERSNALSHOP;
	
	m_UIMessageFunction[UI_CLOSE_POWER_JJANG]			=	 Execute_UI_CLOSE_POWER_JJANG;
	m_UIMessageFunction[UI_REQUEST_POWER_JJANG_POINT]	=	 Execute_UI_REQUEST_POWER_JJANG_POINT;
	m_UIMessageFunction[UI_POWER_JJANG_GAMBLE]			=	 Execute_UI_POWER_JJANG_GAMBLE;

	m_UIMessageFunction[UI_CLOSE_SWAPADVANCEMENTITEM]			=	 Execute_UI_CLOSE_SWAPADVANCEMENTITEM;
	m_UIMessageFunction[UI_SWAPADVANCEMENTITEM]			=	 Execute_UI_SWAPADVANCEMENTITEM;

	m_UIMessageFunction[UI_LEARN_ADVANCE_SKILL]			=	 Execute_UI_LEARN_ADVANCE_SKILL;
	
	
	m_UIMessageFunction[UI_CAMPAIGN_HELP] =	 Execute_UI_CAMPAIGN_HELP;

	m_UIMessageFunction[UI_RUN_NEXT_GQUEST_EXCUTE_ELEMENT] =	 Execute_UI_RUN_NEXT_GQUEST_EXCUTE_ELEMENT;

	m_UIMessageFunction[UI_GQUEST_SET_ACTION] =	 Execute_UI_GQUEST_SET_ACTION;
	m_UIMessageFunction[UI_GQUEST_ENDING_EVENT] =	 Execute_UI_GQUEST_ENDING_EVENT;

	m_UIMessageFunction[UI_REQUEST_EVENT_ITEM] =	 Execute_UI_REQUEST_EVENT_ITEM;

	m_UIMessageFunction[UI_CLOSE_INVENTORY_SUB]			=	 Execute_UI_CLOSE_INVENTORY_SUB;
	m_UIMessageFunction[UI_ITEM_DROP_TO_INVENTORY_SUB]		=	 Execute_UI_ITEM_DROP_TO_INVENTORY_SUB;
	m_UIMessageFunction[UI_ITEM_INSERT_TO_INVENTORY_SUB]	=	Execute_UI_ITEM_INSERT_TO_INVENTORY_SUB;//20070330 by diesirace
	m_UIMessageFunction[UI_ITEM_PICKUP_FROM_INVENTORY_SUB]	=	 Execute_UI_ITEM_PICKUP_FROM_INVENTORY_SUB;
	m_UIMessageFunction[UI_ITEM_USE_SUBINVENTORY]	=	 Execute_UI_ITEM_USE_SUBINVENTORY;
	
	m_UIMessageFunction[UI_WEDDING_CONTRIBUTION]	=	 Execute_UI_WEDDING_CONTRIBUTION;


	m_UIMessageFunction[UI_NETMARBLE_AGREEMENT] = Execute_UI_NETMARBLE_AGREEMENT;

	m_UIMessageFunction[UI_BLOOD_BURST] = Execute_UI_BLOOD_BURST;
	m_UIMessageFunction[UI_CLIENT_REMOVE_EFFECT_STATUS] = Execute_UI_CLIENT_REMOVE_EFFECT_STATUS;
	m_UIMessageFunction[UI_MARKET_ACCOUNT] = Execute_UI_MARKET_ACCOUNT;
	
	m_UIMessageFunction[UI_NONPK_SERVER_USE] = Execute_UI_NONPK_SERVER_USE;

	m_UIMessageFunction[UI_PLAYER_PETINFO] = Execute_UI_PLAYER_PETINFO;
	m_UIMessageFunction[UI_FORCEATTACK_SET] = Execute_UI_FORCEATTACK_SET;
// by svi
#if __CONTENTS(__AUTO_ATTACT)	//__AUTO_ATTACT
	m_UIMessageFunction[UI_AUTOATTACK_SET] = Execute_UI_AUTOATTACK_SET;
#endif //__AUTO_ATTACT
// end
	m_UIMessageFunction[UI_SCREENSHOT] = Execute_UI_SCREENSHOT;

	m_UIMessageFunction[UI_UPDATE_AVAILABLE_SKILLS] = Execute_UI_UPDATE_AVAILABLE_SKILLS;
	m_UIMessageFunction[UI_PLAYER_ACTION_TO_SEND_PACKET] = Execute_UI_PLAYER_ACTION_TO_SEND_PACKET;

	m_UIMessageFunction[UI_RELOAD_INFO_FILE] = Execute_UI_RELOAD_INFO_FILE;
#if __CONTENTS(__GEAR_SWAP_CHANGE)	// UIMessageManager::Init() UI ???????????????????? ????.
	m_UIMessageFunction[UI_ITEM_USE_GEAR_CHANGE] = Execute_UI_GEAR_CHANGE_SEND_PACKET;
#endif	// __GEAR_SWAP_CHANGE

#if __CONTENTS(__TIPOJYU_CASTLE)	// UIMessageManager::Init() UI ???????????????????? ????.
	m_UIMessageFunction[UI_TIPOYU_MOVE] = Execute_UI_TIPOJYU_MOVE_SEND_PACKET;
#endif	// __TIPOJYU_CASTLE

#if __CONTENTS(__ITEM_SHOW)
	m_UIMessageFunction[UI_ITEM_SHOW_AND_CREATE_ITEM] = Execute_UI_ITEM_SHOW_AND_CREATE_ITEM;
	m_UIMessageFunction[UI_ITEM_SHOW_AND_CLEAR_ITEM] = Execute_UI_ITEM_SHOW_AND_CLEAR_ITEM;
#endif //__ITEM_SHOW

#if __CONTENTS(__MOVE_ZONE)
	m_UIMessageFunction[UI_MOVE_ZONE] = Execute_UI_MOVE_ZONE;
#endif //__MOVE_ZONE

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
	m_UIMessageFunction[UI_SELECT_WEEKITEM] = Execute_UI_SELECT_WEEKITEM;
	m_UIMessageFunction[UI_SELECT_ALL_WEEKITEM] = Execute_UI_SELECT_ALL_WEEKITEM;
	m_UIMessageFunction[UI_FINISH_PREMIUM_GIVEITEM_BUTTON] = Execute_UI_FINISH_PREMIUM_GIVEITEM_BUTTON;
#endif //__PREMIUM_GIVE_ITEM_UI

#if __CONTENTS(__REPAIRSYSTEM_RENEWAL)	//?????????????????? ???????????????? ????
	m_UIMessageFunction[UI_REQUEST_REPAIRINFO] = Execute_UI_REQUEST_REPAIRINFO;
#endif	//__PREMIUM_GIVE_ITEM_UI
#if __CONTENTS(__PET_MIXINGFORGE)
	m_UIMessageFunction[UI_USE_PET_MIXING_FORGE] = Execute_UI_USE_PET_MIXING_FORGE;
	m_UIMessageFunction[UI_CLOSE_PET_MIXING_FORGE] = Execute_UI_CLOSE_PET_MIXING_FORGE;
#endif //__PET_MIXINGFORGE
	m_UIMessageFunction[UI_DESTROY] = Execute_UI_DESTROY;
#if __CONTENTS(__FRIEND_ADDITION)
	m_UIMessageFunction[UI_FRIEND_INFO_DATA] = Execute_UI_FRIEND_INFO_DATA;
	m_UIMessageFunction[UI_FRIEND_SYSTEM_OPTION] = Execute_UI_FRIEND_SYSTEM_OPTION;
	m_UIMessageFunction[UI_FRIEND_INFO_OPTION] = Execute_UI_FRIEND_INFO_OPTION;
	m_UIMessageFunction[UI_FRIEND_ADD] = Execute_UI_FRIEND_ADD;
	m_UIMessageFunction[UI_FRIEND_DEL] = Execute_UI_FRIEND_DEL;
	m_UIMessageFunction[UI_FRIEND_RES_ADD] = Execute_UI_FRIEND_RES_ADD;
#endif //__FRIEND_ADDITION

	m_UIMessageFunction[UI_REMOVE_ITEM] = Execute_UI_REMOVE_ITEM;
}

//-----------------------------------------------------------------------------
// Execute
//-----------------------------------------------------------------------------
// message????????????????? ?????????????? ????????????????.
//-----------------------------------------------------------------------------
void			
UIMessageManager::Execute(DWORD message, int left, int right, void* void_ptr)
{
	// ???????????? MODE_GAME????
	// NULL??????????????? ??????????????..
	if (g_Mode==MODE_GAME
		&& (g_pZone==NULL
			|| g_pPlayer==NULL
			|| g_pInventory==NULL
			|| g_pPlayer->IsSlayer() && g_pSlayerGear==NULL
			|| g_pPlayer->IsVampire() && g_pVampireGear==NULL
			|| g_pPlayer->IsOusters() && g_pOustersGear==NULL
			|| g_pTempInformation==NULL
			// 2004, 12, 17, sobeit add start
			|| message >= MAX_UI_MESSAGE
			// 2004, 12, 17, sobeit add end
			)
		)
	{
		DEBUG_ADD("[Error] UI message. game object pointer NULL");

		return;
	}
		
	if (m_UIMessageFunction[message]==NULL)
	{
		DEBUG_ADD_FORMAT("[Error] NO UI_MessageFunction. msg=%d", message);
	}
	else
	{
		(*m_UIMessageFunction[message])(left, right, void_ptr);

		DEBUG_ADD("[UI] Execute OK");
	}
}


//-----------------------------------------------------------------------------
// 2026-09-03 — karakter adi Latin-only kurali (kullanici istegi: "!!!!"
// gibi isaretler kullanilamasin, sadece Latin alfabesi).
//
// Yalniz A-Z, a-z, 0-9 kabul edilir; ilk karakter harf olmali. Cift-byte
// (CJK) girisler, '-', '_' ve tum noktalama isaretleri reddedilir. Hesap/
// ID akislarindaki IsValidID DEGISTIRILMEDI — bu kural yalniz karakter
// olusturma (UI_NEW_CHARACTER + UI_NEWCHARACTER_CHECK) yollarinda gecerli.
// Ayrica isim kutusu (m_lev_name, VS_UI_NewTitle) yazim aninda da ayni
// seti filtreler; bu fonksiyon eski title dahil tum gonderim yollarinin
// son kapisidir.
//-----------------------------------------------------------------------------
static bool Phoenix_IsLatinCharacterName(const char* strName)
{
	if (strName == NULL || strName[0] == '\0')
		return false;

	const unsigned char first = (unsigned char)strName[0];
	if (!((first >= 'A' && first <= 'Z') || (first >= 'a' && first <= 'z')))
		return false;   // ilk karakter harf olmali

	for (const char* p = strName; *p != '\0'; ++p)
	{
		const unsigned char ch = (unsigned char)*p;
		const bool bOk = (ch >= 'A' && ch <= 'Z')
		              || (ch >= 'a' && ch <= 'z')
		              || (ch >= '0' && ch <= '9');
		if (!bOk)
			return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
//
// ????????????????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_NEW_CHARACTER(int left, int right, void* void_ptr)
{
	DEBUG_ADD("UI_NEW_CHARACTER");
	if (g_Mode!=MODE_WAIT_SELECTPC)
	{
		DEBUG_ADD("Not Mode MODE_WAIT_SELECTPC");
		return;
	}

	//
	// ((C_VS_UI::NEW_CHARACTER *)void_ptr) = 
	//	char *	sz_name;
	//	bool		bl_female;
	//	int		STR;
	//	int		DEX;
	//	int		INT;
	//	int		slot;
	//	
	//	int		face;
	//	int		skin_color;
	//	int		hair_color;
	//	int		coat_color;
	//	int		trouser_color;
	//
	NEW_CHARACTER* pChar = (NEW_CHARACTER *)void_ptr;
	
	BOOL bAllOK = TRUE;

	//---------------------------------------------
	// ???? ?????? ??
	//---------------------------------------------
	int len = strlen(pChar->sz_name);	
	
	if (len<PlayerInfo::minIDLength || len>PlayerInfo::maxIDLength)
	{
		char strTemp[128];
		sprintf(strTemp, (*g_pGameStringTable)[STRING_USER_REGISTER_ID_LENGTH].GetString(), PlayerInfo::minIDLength, PlayerInfo::maxIDLength);
		g_pUIDialog->PopupFreeMessageDlg( strTemp, -1, -1, UI_DIALOG_TITLE_OK );
		bAllOK = FALSE;
	}

	if (bAllOK)
	{
		// 2026-09-03 — Latin-only: eski "-_" iznine son verildi, CJK ve
		// isaretler reddedilir (Phoenix_IsLatinCharacterName aciklamasi)
		if (!IsValidID(pChar->sz_name, NULL)
			|| !Phoenix_IsLatinCharacterName(pChar->sz_name))
		{
			//---------------------------------------------
			// ????????ID?????????
			//---------------------------------------------						
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString(), -1, -1, UI_DIALOG_TITLE_OK );
			bAllOK = FALSE;
		}
		else
		{
			char strName[80];
			strcpy(strName, pChar->sz_name);

			// ?????????? ?????? ??????????????? ??????????????
			if (g_pChatManager->RemoveCurse(strName))
			{
				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString(), -1, -1, UI_DIALOG_TITLE_OK );
				bAllOK = FALSE;
			}
			else
			{
				MChatManager badNameManager;
				badNameManager.LoadFromFile( g_pFileDef->getProperty("FILE_INFO_CHAT_2").c_str());

				if (badNameManager.RemoveCurse(strName))
				{
					g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString(), -1, -1, UI_DIALOG_TITLE_OK );
					bAllOK = FALSE;
				}
			}
		}
	}

	//---------------------------------------------
	// ?????data??????????????????..
	//---------------------------------------------
	if (bAllOK)
	{

			//char wansungName[16];

			//---------------------------------------------
			// ??????????--> ????????
			//---------------------------------------------
			//UI_JohapToWansung( pChar->sz_name, wansungName );

			CLCreatePC	_CLCreatePC;
			_CLCreatePC.setName ( pChar->sz_name );//wansungName );
			
			g_pUserInformation->Character[pChar->slot] = pChar->sz_name;
			//g_pUserInformation->CharacterW[pChar->slot] = wansungName;

			HairStyle hairStyle[3] =
			{
				HAIR_STYLE1,
				HAIR_STYLE2,
				HAIR_STYLE3
			};

			// ?????? ????
			pChar->hair_color = max(0, min(pChar->hair_color, MAX_COLORSET-1));
			pChar->skin_color = max(0, min(pChar->skin_color, MAX_COLORSET-1));
			
			_CLCreatePC.setSlot ( pChar->slot==0? SLOT1 : pChar->slot==1? SLOT2 : SLOT3 );
			_CLCreatePC.setRace ( pChar->race );
			_CLCreatePC.setSex ( pChar->bl_female? FEMALE : MALE );
			_CLCreatePC.setHairStyle ( hairStyle[pChar->face] );
			_CLCreatePC.setHairColor ( pChar->hair_color );					
			_CLCreatePC.setSkinColor ( pChar->skin_color );
			_CLCreatePC.setShirtColor ( pChar->coat_color, MAIN_COLOR );
			_CLCreatePC.setShirtColor ( pChar->coat_color, SUB_COLOR );
			_CLCreatePC.setJeansColor ( pChar->trouser_color, MAIN_COLOR );
			_CLCreatePC.setJeansColor ( pChar->trouser_color, SUB_COLOR );
			_CLCreatePC.setSTR( pChar->STR );
			_CLCreatePC.setDEX( pChar->DEX );
			_CLCreatePC.setINT( pChar->INT );

			g_pSocket->sendPacket( &_CLCreatePC );

			g_pSocket->setPlayerStatus( CPS_AFTER_SENDING_CL_CREATE_PC );	

			

		SetMode(MODE_WAIT_CREATEPCOK);
	}
}


//-----------------------------------------------------------------------------
//
// Run NewUser Registration
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_RUN_NEWUSER_REGISTRATION(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_RUN_NEWUSER_REGISTRATION");
	CT_Done("[UI] Execute_UI_RUN_NEWUSER_REGISTRATION ENTER mode=%d",
		(int)g_pClientConfig->NEW_USER_REGISTERATION_MODE);

	switch (g_pClientConfig->NEW_USER_REGISTERATION_MODE)
	{
		//-----------------------------------------------------------
		// CLIENT - ???????????????????????
		//-----------------------------------------------------------
		case ClientConfig::NUR_CLIENT :
		{
			gC_vs_ui.RunNewUserRegistration();
		}
		break;

		//-----------------------------------------------------------
		// HOMEPAGE - ????????????????????????????
		//-----------------------------------------------------------
		case ClientConfig::NUR_HOMEPAGE :		
		{
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_HOMEPAGE].GetString(), -1, -1, UI_DIALOG_TITLE_OK );			

			// ??????..
			SetMode( MODE_QUIT );

			char str[256];

			GetWindowsDirectory(
				str,  // address of buffer for Windows directory
				255        // size of directory buffer
			);

			sprintf(str, "%s\\Explorer.exe", str);
			
			CDirectDraw::RestoreDisplayModeCompat();
			if(g_pUserInformation->IsNetmarble)
				_spawnl(_P_NOWAIT, str, "Explorer.exe", g_pClientConfig->URL_HOMEPAGE_NEW_USER_NETMARBLE.GetString(), NULL);
			else
				_spawnl(_P_NOWAIT, str, "Explorer.exe", g_pClientConfig->URL_HOMEPAGE_NEW_USER.GetString(), NULL);
		}
		break;

		//-----------------------------------------------------------
		// MESSAGE_HOMEPAGE - ???????????????? ?????????? message
		//-----------------------------------------------------------
		case ClientConfig::NUR_MESSAGE_HOMEPAGE :	
		{
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_HOMEPAGE].GetString(), -1, -1, UI_DIALOG_TITLE_OK);			
		}
		break;
		
		//-----------------------------------------------------------
		// DENY - ?????????????????????		
		//-----------------------------------------------------------
		case ClientConfig::NUR_DENY :				
		{
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_DENY].GetString(), -1, -1, UI_DIALOG_TITLE_OK );						
		}
		break;	
	}

}

//-----------------------------------------------------------------------------
//
// Char Info Close
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_INFO_CLOSE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_INFO_CLOSE");
	
	if (g_Mode!=MODE_WAIT_SELECTPC
		&& g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_WAIT_SELECTPC");
		return;
	}
	
	gC_vs_ui.CloseInfo();
}

//-----------------------------------------------------------------------------
//
// Check Exist ID
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CHECK_EXIST_ID(int left, int right, void* void_ptr)
{
	//
	// left = id
	//
	DEBUG_ADD("[UI] UI_CHECK_EXIST_ID");
	
	const char* pName = (const char*)void_ptr;

	//--------------------------------------------------
	// ID ??
	//--------------------------------------------------
	if (IsValidID(pName, NULL))
	{
		int len = strlen(pName);	
		
		if (len<PlayerInfo::minIDLength || len>PlayerInfo::maxIDLength)
		{
			char strTemp[128];
			sprintf(strTemp, "The ID is between %d and %d characters long", PlayerInfo::minIDLength, PlayerInfo::maxIDLength);
			g_pUIDialog->PopupFreeMessageDlg( strTemp );						
		}
		else
		{
			//--------------------------------------------------
			// socket ??????
			//--------------------------------------------------
			if (!InitSocket())
			{						
				//InitFail("[Error] Can't init Socket");
				return;
			}

				CLQueryPlayerID _CLQueryPlayerID;
				//_CLDeletePC.setName( g_pUserInformation->CharacterW[pChar->slot].GetString() );
				_CLQueryPlayerID.setPlayerID( pName );

				g_pSocket->sendPacket( &_CLQueryPlayerID );

				g_pSocket->setPlayerStatus( CPS_AFTER_SENDING_CL_QUERY_PLAYER_ID );


			//gC_vs_ui.AleadyExistIdMessage(); // ????????
			//gC_vs_ui.NoAleadyExistIdMessage(); // ????????					
		}
	}
	else
	{
		g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString() );
	}

	// ????????????????????????. ????*_*;
	DeleteNewArray( void_ptr );
}


//-----------------------------------------------------------------------------
//
//		??????????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_DELETE_CHARACTER(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_DELETE_CHARACTER");
	
	if (g_Mode!=MODE_WAIT_SELECTPC)
	{
		DEBUG_ADD("Not Mode MODE_WAIT_SELECTPC");
		return;
	}
	
	DELETE_CHARACTER *pChar = (DELETE_CHARACTER*)void_ptr;

	//--------------------------------------------------
	// ???????????
	//--------------------------------------------------
	// ??????????
	//if (g_pUserInformation->IsNetmarble || g_pUserInformation->bChinese || IsValidSSN( pChar->sz_part1, pChar->sz_part2 ))
	//if (!(g_pUserInformation->IsNetmarble && g_pUserInformation->IsNetmarbleLogin) || g_pUserInformation->bChinese || IsValidSSN( pChar->sz_part1, pChar->sz_part2 ))
	if (!(g_pUserInformation->IsNetmarble) || g_pUserInformation->bChinese || IsValidSSN( pChar->sz_part1, pChar->sz_part2 ))
	{
#if __CONTENTS(__CHAR_DELETE)
		char strTemp[20];
		sprintf(strTemp,"%s",pChar->sz_part1);      
		CLDeletePC _CLDeletePC;
		_CLDeletePC.setName( g_pUserInformation->Character[pChar->slot].GetString() );
		_CLDeletePC.setSlot( pChar->slot==0? SLOT1 : pChar->slot==1? SLOT2 : SLOT3 );
		_CLDeletePC.setSSN( strTemp );  
		g_pSocket->sendPacket( &_CLDeletePC ); 
		g_pSocket->setPlayerStatus( CPS_AFTER_SENDING_CL_DELETE_PC );
		DeleteNewArray( pChar->sz_part1 );

		g_pUserInformation->Slot = pChar->slot;
		SetMode( MODE_WAIT_DELETEPCOK );
#else
		char strTemp[20];
		
		if( g_pUserInformation->bChinese )
		{
			//sprintf(strTemp,"%s",pChar->sz_part1);
		} else
		if(!(g_pUserInformation->IsNetmarble && g_pUserInformation->IsNetmarbleLogin))
		{
			// xxxxxx-xxxxxxx ???????????? ????????
			sprintf(strTemp, "%s-%s", pChar->sz_part1, pChar->sz_part2);
		}
		else
			sprintf(strTemp, "1");

			CLDeletePC _CLDeletePC;
			//_CLDeletePC.setName( g_pUserInformation->CharacterW[pChar->slot].GetString() );
			_CLDeletePC.setName( g_pUserInformation->Character[pChar->slot].GetString() );
			_CLDeletePC.setSlot( pChar->slot==0? SLOT1 : pChar->slot==1? SLOT2 : SLOT3 );
			
			_CLDeletePC.setSSN( strTemp );

			g_pSocket->sendPacket( &_CLDeletePC );

			g_pSocket->setPlayerStatus( CPS_AFTER_SENDING_CL_DELETE_PC );
			

		// ?????????
		//pChar->sz_part1
		//pChar->sz_part2					

		// ?????????????????????????? message
		//gC_vs_ui.Invalid_SSN_Message();

		// ??????????????
		//gC_vs_ui.DeleteCharacter(slot);

		//
		// ((DELETE_CHARACTER *)void_ptr) = sz_part1, sz_part2, slot
		//

		// ??????????
	//	if(!(g_pUserInformation->IsNetmarble && g_pUserInformation->IsNetmarbleLogin))
	//	{
	//		DeleteNewArray( pChar->sz_part1 );
	//		DeleteNewArray( pChar->sz_part2 );
	//	}

		//gC_vs_ui.Invalid_SSN_Message();
		//gC_vs_ui.DeleteCharacter( pChar->slot );

		//--------------------------------------------
		// ??????????????slot??????
		//--------------------------------------------
		g_pUserInformation->Slot = pChar->slot;

		SetMode( MODE_WAIT_DELETEPCOK );
#endif	//__CHAR_DELETE
	}
	//else  if(g_pUserInformation->IsNetmarble && g_pUserInformation->IsNetmarbleLogin)
	else  if(g_pUserInformation->IsNetmarble)
	{
		char strTemp[20];
		sprintf(strTemp,"%s",pChar->sz_part1);      
		CLDeletePC _CLDeletePC;
		_CLDeletePC.setName( g_pUserInformation->Character[pChar->slot].GetString() );
		_CLDeletePC.setSlot( pChar->slot==0? SLOT1 : pChar->slot==1? SLOT2 : SLOT3 );
		_CLDeletePC.setSSN( strTemp );  
		g_pSocket->sendPacket( &_CLDeletePC ); 
		g_pSocket->setPlayerStatus( CPS_AFTER_SENDING_CL_DELETE_PC );
		DeleteNewArray( pChar->sz_part1 );

		g_pUserInformation->Slot = pChar->slot;
		SetMode( MODE_WAIT_DELETEPCOK );
	}
	else
	{
		if(!g_pUserInformation->IsNetmarble)
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_SSN].GetString(), -1, -1, UI_DIALOG_TITLE_OK);
		else
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_ERROR_INVALID_DELETE].GetString(), -1, -1, UI_DIALOG_TITLE_OK);
	}
	
	
}

	
//-----------------------------------------------------------------------------
//
//		???????? ??????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_TERMINATION(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_TERMINATION");
	CT_Done("[UI] Execute_UI_TERMINATION ENTER g_Mode=%d (MAINMENU=%d)", (int)g_Mode, (int)MODE_MAINMENU);

	if (g_Mode!=MODE_MAINMENU)
	{
		DEBUG_ADD("Not Mode MODE_MAINMENU");
		CT_Done("[UI] Execute_UI_TERMINATION early-return (not MAINMENU) -> PostQuitMessage(0) fallback");
		// FAZ 1.6 ??Exit dialog Evet -> UI_TERMINATION; g_Mode MAINMENU de?ilse
		// orijinal kod sessizce d?n?yordu. Kullan?c??"Evet"e bast???nda kapatmay??
		// bekledi?i i?in do?rudan mesaj kuyru?una WM_QUIT d??.
		PostQuitMessage(0);
		return;
	}

	CT_Done("[UI] Execute_UI_TERMINATION -> SetMode(MODE_QUIT)");
	SetMode( MODE_QUIT );
}

//-----------------------------------------------------------------------------
//
// ?????????????????
//
//-----------------------------------------------------------------------------
/*
void
UIMessageManager::Execute_UI_NEW_USER_REGISTRATION(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_NEW_USER_REGISTRATION");
	
	//
	// ((NEW_REGISTRATION *)void_ptr) = ...
	//
	// // !std::string ptr?????????????? ??????
	//
	//struct NEW_REGISTRATION
	//{
	//	char *	sz_id;
	//	char *	sz_password;
	//	char *	sz_repassword;
	//	char *	sz_name;
	//	char *	sz_email;
	//	char *	sz_address;
	//	char *	sz_ssn_number_part1; // ??????
	//	char *	sz_ssn_number_part2; // ??????
	//	char *	sz_homepage;
	//	char *	sz_woo;
	//	char *	sz_phone;
	//	bool		bl_female;
	//	bool		bl_announce_my_info;
	//};
	//			

	NEW_REGISTRATION* pReg = (NEW_REGISTRATION *)void_ptr;

	BOOL AllOK = TRUE;

	//--------------------------------------------------
	// std::string ?????? ??
	//--------------------------------------------------
	if (pReg->sz_id==NULL 
		|| pReg->sz_password==NULL
		|| pReg->sz_name==NULL
		|| pReg->sz_ssn_number_part1==NULL
		|| pReg->sz_ssn_number_part2==NULL
		|| pReg->sz_email==NULL)
	{
		// ???????????????? ???? ?????
		AllOK = FALSE;

		g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_USER_REGISTER_EMPTY_FIELD].GetString());
	}
	else
	{
		int len;
		char strTemp[128];
			
		
		//--------------------------------------------------
		// ID ?????? ??
		//--------------------------------------------------
		len = strlen(pReg->sz_id);	
		
		if (len<PlayerInfo::minIDLength || len>PlayerInfo::maxIDLength)
		{
			sprintf(strTemp, (*g_pGameStringTable)[STRING_USER_REGISTER_ID_LENGTH].GetString(), PlayerInfo::minIDLength, PlayerInfo::maxIDLength);
			g_pUIDialog->PopupFreeMessageDlg( strTemp );
			AllOK = FALSE;
		}

		//---------------------------------------------
		// ????????ID?????? ??????.
		//---------------------------------------------						
		if (AllOK)
		{
			if (!IsValidID(pReg->sz_id, NULL))
			{
				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString() );
				AllOK = FALSE;
			}
			else
			{
				char strName[80];
				strcpy(strName, pReg->sz_id);

				// ?????????? ?????? ??????????????? ??????????????
				if (g_pChatManager->RemoveCurse(strName))
				{
					g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString() );
					AllOK = FALSE;
				}								
			}
		}
		
		//--------------------------------------------------
		// Password ?????? ??
		//--------------------------------------------------
		if (AllOK)
		{
			len = strlen(pReg->sz_password);	
			
			if (len<PlayerInfo::minPasswordLength || len>PlayerInfo::maxPasswordLength)
			{
				sprintf(strTemp, (*g_pGameStringTable)[STRING_USER_REGISTER_PASSWORD_LENGTH].GetString(), PlayerInfo::minPasswordLength, PlayerInfo::maxPasswordLength);
				g_pUIDialog->PopupFreeMessageDlg( strTemp );
				AllOK = FALSE;
			}
			else if (!IsValidPassword(pReg->sz_id))
			//else if (!IsValidID(pReg->sz_id))	// ?????????????? ????????
			// ???? ???????????????????????..  ID?????????????????????? ?????? ???? ????..
			{
				// ID??????????????????????????
				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_ID_SPECIAL].GetString() );
				AllOK = FALSE;
			}			
			else if (!IsValidPassword(pReg->sz_password))
			{
				// ?????????? ?????????????
				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_PASSWORD_SPECIAL].GetString() );
				AllOK = FALSE;
			}
			else
			{
				//--------------------------------------------------
				// ???????????????????????
				//--------------------------------------------------
				char* str = pReg->sz_password;
				
				char ch;

				bool AllNumber = TRUE;

				while (ch=*str++, ch)
				{
					if (ch<'0' || ch>'9')
					{
						AllNumber = FALSE;
						break;
					}
				}

				if (AllNumber)	// ?????? ???????????????.
				{
					g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_PASSWORD_NUMBER].GetString() );
					AllOK = FALSE;
				}
			}

		}

		//--------------------------------------------------
		// ????
		//--------------------------------------------------
		if (AllOK)
		{
			len = strlen(pReg->sz_name);	
			
			if (len>PlayerInfo::maxNameLength)
			{
				sprintf(strTemp, (*g_pGameStringTable)[STRING_USER_REGISTER_NAME_LENGTH].GetString(), PlayerInfo::maxNameLength);
				g_pUIDialog->PopupFreeMessageDlg( strTemp );
				AllOK = FALSE;
			}		
		}

		//--------------------------------------------------
		// ????
		//--------------------------------------------------
		if (AllOK)
		{
			// ???????????????????
			if (//strlen(pReg->sz_ssn_number) == 6+1+7
				//&& pReg->sz_ssn_number[6]=='-')
				1)
			{
				char ssn1[7];
				char ssn2[8];

				
				//pReg->sz_ssn_number[6] = '\0';
				//strcpy(ssn1, pReg->sz_ssn_number);

				//pReg->sz_ssn_number[6] = '-';
				//strcpy(ssn2, pReg->sz_ssn_number + 7);
				
				strcpy(ssn1, pReg->sz_ssn_number_part1);
				strcpy(ssn2, pReg->sz_ssn_number_part2);

				//--------------------------------------------------
				// ???????????
				//--------------------------------------------------
				if (!IsValidSSN( ssn1, ssn2 ))
				{
					sprintf(strTemp, (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_SSN].GetString());
					g_pUIDialog->PopupFreeMessageDlg( strTemp );
					AllOK = FALSE;
				}
				
			}		
			//else
			//{
			//	sprintf(strTemp, (*g_pGameStringTable)[STRING_USER_REGISTER_SSN_FORMAT].GetString());
			//	g_pUIDialog->PopupFreeMessageDlg( strTemp );
			//	AllOK = FALSE;
			//}
		}
	}

	//--------------------------------------------------
	// ???????????????..
	//--------------------------------------------------
	if (AllOK)
	{
		if (!InitSocket())
		{						
			//InitFail("[Error] Can't init Socket");
			return;
		}

		#ifdef	CONNECT_SERVER

			//--------------------------------------------------
			// CLVersionCheck
			//--------------------------------------------------
			#ifndef _DEBUG

				if (!g_bTestMode)
				{
					DWORD version = g_pUserInformation != NULL ? g_pUserInformation->GameVersion : 0;
					if (version == 0)
						return;

					CLVersionCheck _CLVersionCheck;
					_CLVersionCheck.setVersion( version );

					g_pSocket->sendPacket( &_CLVersionCheck );
				}
				
			#endif
			
			char ssnAll[20];

			sprintf(ssnAll, "%s-%s", pReg->sz_ssn_number_part1, pReg->sz_ssn_number_part2);							
			
			//--------------------------------------------------
			// CLRegisterPlayer
			//--------------------------------------------------
			CLRegisterPlayer	_CLRegisterPlayer;
			
			//--------------------------------------------------
			// ????????
			//--------------------------------------------------
			_CLRegisterPlayer.setID( pReg->sz_id );
			_CLRegisterPlayer.setPassword( pReg->sz_password );
			_CLRegisterPlayer.setName( pReg->sz_name );
			_CLRegisterPlayer.setSSN( ssnAll );
			_CLRegisterPlayer.setEmail( pReg->sz_email );						
			_CLRegisterPlayer.setSex( pReg->bl_female? FEMALE:MALE );
			
			//--------------------------------------------------
			// default
			//--------------------------------------------------
			_CLRegisterPlayer.setNation( KOREA );


			//--------------------------------------------------
			// ???????????????? ????
			//--------------------------------------------------
			if (pReg->sz_address!=NULL)
			{
				_CLRegisterPlayer.setAddress( pReg->sz_address );
			}
			else
			{
				_CLRegisterPlayer.setAddress( "NULL" );
			}

			if (pReg->sz_phone!=NULL)
			{
				_CLRegisterPlayer.setCellular( pReg->sz_phone );
			}
			else
			{
				_CLRegisterPlayer.setCellular( "NULL" );
			}

			if (pReg->sz_homepage!=NULL)
			{
				_CLRegisterPlayer.setHomepage( pReg->sz_homepage );
			}
			else
			{
				_CLRegisterPlayer.setHomepage( "NULL" );
			}
			
			if (pReg->sz_phone!=NULL)
			{
				_CLRegisterPlayer.setTelephone( pReg->sz_phone );
			}
			else
			{
				_CLRegisterPlayer.setTelephone( "NULL" );
			}

			if (pReg->sz_woo!=NULL)
			{
				_CLRegisterPlayer.setZipCode( pReg->sz_woo );
			}
			else
			{
				_CLRegisterPlayer.setZipCode( "NULL" );
			}

			//--------------------------------------------------
			// ?????? ??????.. 
			//--------------------------------------------------
			_CLRegisterPlayer.setProfile( "profile" );

			g_pSocket->setPlayerStatus( CPS_AFTER_SENDING_CL_REGISTER_PLAYER );

			g_pSocket->sendPacket( &_CLRegisterPlayer );
			
		#endif	

		// id?????
		g_pUserInformation->UserID = pReg->sz_id;

		SetMode( MODE_WAIT_REGISTERPLAYEROK );

		//gC_vs_ui.StartCharacterManager();
		DeleteNewArray(pReg->sz_id);
		DeleteNewArray(pReg->sz_password);
		DeleteNewArray(pReg->sz_repassword);
		DeleteNewArray(pReg->sz_name);
		DeleteNewArray(pReg->sz_email);
		DeleteNewArray(pReg->sz_address);
		DeleteNewArray(pReg->sz_ssn_number_part1);
		DeleteNewArray(pReg->sz_ssn_number_part2);
		DeleteNewArray(pReg->sz_homepage);
		DeleteNewArray(pReg->sz_woo);
		DeleteNewArray(pReg->sz_phone);

		//gC_vs_ui.CloseUserRegistrationWindow();
		//gC_vs_ui.StartCharacterManager();
		//gC_vs_ui.ClearAllCharacter();
	}
	
}
*/

void
UIMessageManager::Execute_UI_NEW_USER_REGISTRATION(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_NEW_USER_REGISTRATION");
	CT_Done("UI_NEW_USER_REGISTRATION enter left=%d right=%d payload=%p",
		left,
		right,
		void_ptr);

	NEW_REGISTRATION* pReg = (NEW_REGISTRATION *)void_ptr;
	if (pReg == NULL)
	{
		PhoenixRegistrationTrace("CLIENT UI_REG enter payload=NULL mode=%d socket=%p", (int)g_Mode, (void*)g_pSocket);
		PhoenixRegisterPopupFailure(NULL, "Account has not been created.");
		return;
	}

	PhoenixRegistrationTrace("CLIENT UI_REG enter id=%s idLen=%u passLen=%u mode=%d socket=%p",
		pReg->sz_id != NULL ? pReg->sz_id : "<null>",
		pReg->sz_id != NULL ? (unsigned int)strlen(pReg->sz_id) : 0,
		pReg->sz_password != NULL ? (unsigned int)strlen(pReg->sz_password) : 0,
		(int)g_Mode,
		(void*)g_pSocket);

	if (!PhoenixRegisterValidateID(pReg->sz_id))
	{
		PhoenixRegistrationTrace("CLIENT UI_REG reject invalid_id id=%s", pReg->sz_id != NULL ? pReg->sz_id : "<null>");
		PhoenixRegisterPopupFailure(pReg, "Account name must be 4-8 letters/numbers and start with a letter.");
		return;
	}

	if (!PhoenixRegisterValidatePassword(pReg->sz_password))
	{
		PhoenixRegistrationTrace("CLIENT UI_REG reject invalid_password id=%s", pReg->sz_id != NULL ? pReg->sz_id : "<null>");
		PhoenixRegisterPopupFailure(pReg, "Password must be 6-8 letters/numbers.");
		return;
	}

	PhoenixRegistrationTrace("CLIENT UI_REG before InitSocket id=%s socket=%p", pReg->sz_id, (void*)g_pSocket);
	if (!InitSocket())
	{
		PhoenixRegistrationTrace("CLIENT UI_REG InitSocket failed id=%s socket=%p", pReg->sz_id, (void*)g_pSocket);
		PhoenixRegisterPopupFailure(pReg, "Unable to connect to login server.");
		return;
	}
	PhoenixRegistrationTrace("CLIENT UI_REG after InitSocket id=%s socket=%p", pReg->sz_id, (void*)g_pSocket);

	// Account create sonucu LCRegisterPlayerOK/Error tarafindan gosterilir.
	// CLVersionCheck burada gonderilirse LCVersionCheckError g_bNeedUpdate set edip
	// registration result handler'inin UI sprite'ini maskeleyebilir. Login akisi
	// kendi version check'ini yapmaya devam eder.
	PhoenixRegistrationTrace("CLIENT UI_REG skip CLVersionCheck for NewIDCreate id=%s", pReg->sz_id);

	CLRegisterPlayer _CLRegisterPlayer;
	_CLRegisterPlayer.setID(pReg->sz_id);
	_CLRegisterPlayer.setPassword(pReg->sz_password);

	g_pSocket->setPlayerStatus(CPS_AFTER_SENDING_CL_REGISTER_PLAYER);
	g_pSocket->sendPacket(&_CLRegisterPlayer);
	PhoenixRegistrationTrace("CLIENT UI_REG queued CLRegisterPlayer id=%s status=%d socket=%p",
		pReg->sz_id,
		(int)g_pSocket->getPlayerStatus(),
		(void*)g_pSocket);

	g_pUserInformation->UserID = pReg->sz_id;
	SetMode(MODE_WAIT_REGISTERPLAYEROK);
	PhoenixRegistrationTrace("CLIENT UI_REG SetMode WAIT_REGISTERPLAYEROK id=%s mode=%d modeNext=%d",
		pReg->sz_id,
		(int)g_Mode,
		(int)g_ModeNext);

	CT_Done("UI_NEW_USER_REGISTRATION sent CLRegisterPlayer id=%s", pReg->sz_id);
	PhoenixRegisterFreePayload(pReg);
}

//-----------------------------------------------------------------------------
//
// Login
//
//-----------------------------------------------------------------------------
extern BYTE g_macAddress[6];

void
UIMessageManager::Execute_UI_LOGIN(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_LOGIN");

	UI_SaveUserOption();

	//
	// ((C_VS_UI::LOGIN *)void_ptr) = id, password
	//
	// !std::string ptr?????????????? ??????
	//
	LOGIN*	login = (LOGIN*)void_ptr;
	PhoenixUILoginHandshakeTrace("UI_LOGIN entry mode=%d login=%p id=%s",
		(int)g_Mode,
		(void*)login,
		(login != NULL && login->sz_id != NULL) ? login->sz_id : "<null>");
	PhoenixLoginSocketTrace("UI_LOGIN: enter mode=%d login=%p id=%s",
		(int)g_Mode,
		(void*)login,
		(login != NULL && login->sz_id != NULL) ? login->sz_id : "<null>");

	if (g_Mode!=MODE_MAINMENU)
	{
		PhoenixUILoginHandshakeTrace("UI_LOGIN rejected mode=%d", (int)g_Mode);
		PhoenixLoginSocketTrace("UI_LOGIN: rejected by mode mode=%d", (int)g_Mode);
		DeleteNewArray(login->sz_id);
		DeleteNewArray(login->sz_password);

		DEBUG_ADD("Not Mode MODE_MAINMENU");
		return;
	}	

	// ?????????? ???? ???? ???? - 2001.8.20
	LoadingAddonSPK( false );

	if (login->sz_id!=NULL && login->sz_password!=NULL)
	{
		//if (IsValidID(login->sz_id, NULL))
		{
			// ID?????
			//UI_BackupLoginID(((LOGIN *)void_ptr)->sz_id);
			
			if (IsValidPassword(login->sz_password))
			{
				{
					DEBUG_ADD("[Execute_UI_LOGIN]");
					PhoenixUILoginHandshakeTrace("UI_LOGIN before InitSocket socket=%p", (void*)g_pSocket);
					PhoenixLoginSocketTrace("UI_LOGIN: before InitSocket g_pSocket=%p", (void*)g_pSocket);
					if (!InitSocket())
					{
						//InitFail("[Error] Can't init Socket");
						DEBUG_ADD("[Execute_UI_LOGIN] Can't init Socket");
						DeleteNewArray(login->sz_id);
						DeleteNewArray(login->sz_password);
						DEBUG_ADD("[Execute_UI_LOGIN] Can't init Socket2");
						PhoenixUILoginHandshakeTrace("UI_LOGIN InitSocket failed");
						PhoenixLoginSocketTrace("UI_LOGIN: InitSocket failed");
						return;
					}
					PhoenixUILoginHandshakeTrace("UI_LOGIN after InitSocket socket=%p", (void*)g_pSocket);
					PhoenixLoginSocketTrace("UI_LOGIN: after InitSocket g_pSocket=%p", (void*)g_pSocket);

					//char wansungID[16];

					//---------------------------------------------
					// ??????????--> ????????
					//---------------------------------------------
					//UI_JohapToWansung( login->sz_id, wansungID );
	
					//--------------------------------------------------
					// ????????????????????????????????
					// ?????? ?????? ?????????????? ????????CLLogin ????????.
					// ????,
					// Version?? packet?????????????? - -;
					//--------------------------------------------------
					//--------------------------------------------------
					// CLVersionCheck
					//--------------------------------------------------
					#if !defined(_DEBUG) && !defined(OUTPUT_DEBUG)
						DWORD version = g_pUserInformation != NULL ? g_pUserInformation->GameVersion : 0;
						if (version == 0)
						{
							DeleteNewArray(login->sz_id);
							DeleteNewArray(login->sz_password);
							return;
						}

						CLVersionCheck _CLVersionCheck;
						_CLVersionCheck.setVersion( version );

						PhoenixLoginSocketTrace("UI_LOGIN: sending CLVersionCheck version=%lu", version);
						g_pSocket->sendPacket( &_CLVersionCheck );
						PhoenixLoginSocketTrace("UI_LOGIN: sent CLVersionCheck");


					#endif
					

					//--------------------------------------------------
					// CLLogin
					//--------------------------------------------------
					CLLogin clLogin;

					//clLogin.setID("Reiot");
					//clLogin.setPassword("fpdldhxm");

					// FAZ 4 (2026-04-26): __METROTECH_TEST__ icin ID'ye '#' prefix
					// eklemek server'da TestClientUser tablosuna INSERT yapan
					// internal-test-only branch'e dusurur. Bizde TestClientUser
					// tablosu yok ve test client branch'ini kullanmiyoruz.
					// Plain ID gonder, server normal Player tablosu flow'una girsin.
					std::string temp_id;
					DEBUG_ADD("[Execute_UI_LOGIN] #");

					//if(g_pUserInformation->IsNetmarble)
					//	temp_id += "@";

					temp_id += login->sz_id;
										
					clLogin.setID( temp_id );//wansungID);
					clLogin.setPassword(login->sz_password);			

					// 2004,03,16 sobeit add start - mac address
//					BYTE MacAddress[6];
//					memset(MacAddress, 0,sizeof(BYTE)*6);
//					memcpy(MacAddress, g_macAddress, sizeof(BYTE)*6);
//					GetMacAddressFromNetBIOS(MacAddress);
					clLogin.setMacAddress(g_macAddress);
					// 2004,03,16 sobeit add end - mac address

					// by svi
					int ipaddr = 0;
					ivfstream ipaddrFile;
					if(FileOpenBinary("data/info/ipaddr.inf", ipaddrFile)) {	
						ipaddrFile.read((char*)&ipaddr, 4);
						ipaddrFile.close();
					}
					
					clLogin.setRealIpaddr( ipaddr );

					clLogin.SetLoginMode(g_pUserInformation->IsAutoLogIn);
					
					if (g_pSocket)
					{
						PhoenixUILoginHandshakeTrace("UI_LOGIN sending CLLogin socket=%p id=%s realIp=%d auto=%d",
							(void*)g_pSocket,
							temp_id.c_str(),
							ipaddr,
							(int)g_pUserInformation->IsAutoLogIn);
						PhoenixLoginSocketTrace("UI_LOGIN: sending CLLogin socket=%p id=%s realIp=%d auto=%d",
							(void*)g_pSocket,
							temp_id.c_str(),
							ipaddr,
							(int)g_pUserInformation->IsAutoLogIn);
						g_pSocket->sendPacket( &clLogin );
						PhoenixUILoginHandshakeTrace("UI_LOGIN sent CLLogin");
						PhoenixLoginSocketTrace("UI_LOGIN: sent CLLogin");
						g_pSocket->setPlayerStatus( CPS_AFTER_SENDING_CL_LOGIN );
						PhoenixUILoginHandshakeTrace("UI_LOGIN status CPS_AFTER_SENDING_CL_LOGIN");
						PhoenixLoginSocketTrace("UI_LOGIN: set CPS_AFTER_SENDING_CL_LOGIN");
					}
					else
					{
						PhoenixUILoginHandshakeTrace("UI_LOGIN socket null before CLLogin");
						PhoenixLoginSocketTrace("UI_LOGIN: g_pSocket is null before CLLogin send");
					}
					DEBUG_ADD("[Execute_UI_LOGIN] SendPacket OK");

					//----------------------------------------------------
					// UserInformation??????????????.
					//----------------------------------------------------
					g_pUserInformation->UserID = login->sz_id;
					//g_pUserInformation->UserIDW = wansungID;
				}
				
				DEBUG_ADD("[Execute_UI_LOGIN] ClearAllCharacter");
				PhoenixLoginSocketTrace("UI_LOGIN: before ClearAllCharacter");
				gC_vs_ui.ClearAllCharacter();
				DEBUG_ADD("[Execute_UI_LOGIN] ClearAllCharacter OK");
				PhoenixLoginSocketTrace("UI_LOGIN: after ClearAllCharacter");
				if(false == g_pUserInformation->IsAutoLogIn)
				{
					PhoenixUILoginHandshakeTrace("UI_LOGIN before SetMode MODE_WAIT_LOGINOK");
					PhoenixLoginSocketTrace("UI_LOGIN: before SetMode MODE_WAIT_LOGINOK");
					SetMode( MODE_WAIT_LOGINOK );
					PhoenixUILoginHandshakeTrace("UI_LOGIN after SetMode MODE_WAIT_LOGINOK");
					PhoenixLoginSocketTrace("UI_LOGIN: after SetMode MODE_WAIT_LOGINOK");
				}
				DEBUG_ADD("[Execute_UI_LOGIN] SetMode OK");
			}
			else
			{
				// ?????????? ?????????????
				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_PASSWORD_SPECIAL].GetString(), -1, -1, UI_DIALOG_TITLE_OK );
			}
		}
		//---------------------------------------------
		// ????????ID?????????
		//---------------------------------------------
		//else
		//{
			//g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString() );							
		//}
	}
	

	// (!) ???????????????????????? ???? ????????????.

	//gC_vs_ui.StartGame();
	//gC_vs_ui.StartCharacterManager();

	DeleteNewArray(login->sz_id);
	DeleteNewArray(login->sz_password);
	DEBUG_ADD("[Execute_UI_LOGIN] OK");
	PhoenixLoginSocketTrace("UI_LOGIN: exit ok mode=%d g_pSocket=%p", (int)g_Mode, (void*)g_pSocket);
}

//-----------------------------------------------------------------------------
//
// ?????????????? ????????main menu??????????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CHARACTER_MANAGER_FINISHED(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CHARACTER_MANAGER_FINISHED");

	if (g_Mode!=MODE_WAIT_SELECTPC)
	{
		DEBUG_ADD("g_Mode not MODE_WAIT_SELECTPC");

		return;
	}

	// ??????????
#ifdef __DESIGNED_JAPAN
	if(g_pUserInformation->IsNetmarble && !g_pUserInformation->IsNetmarbleJapan)
#else	
	if(g_pUserInformation->IsNetmarble)
#endif
	{
		SetMode( MODE_MAINMENU );
		return;
	}

	//
	// Client, Server????logout packet ??????.
	//
	//--------------------------------------------------
	// Login ??????????CLLogout ??????????????????
	//--------------------------------------------------
	/*
	#ifdef	CONNECT_SERVER		
		
		// hot key save????.
		//UI_SaveHotKeyToServer();

		CLLogout clLogout;
	
		g_pSocket->sendPacket( &clLogout );
		//g_pSocket->setPlayerStatus( CPS_END_SESSION );
		//g_pSocket->disconnect();

		ReleaseSocket();
	#endif					


	SetMode( MODE_MAINMENU );
	*/

#if __CONTENTS(__TITLE_UI_RENWEAL)
	//20081224 ?????????????????? ?????? ?????????? ???? ?????? ?????????????.
	Sleep(500);
	UI_SetWorldList();
	SetMode( MODE_WAIT_SELECT_WORLD );
#else	// __TITLE_UI_RENWEAL
	UI_SetServerList();
	SetMode( MODE_WAIT_SELECT_SERVER );
#endif	//__TITLE_UI_RENWEAL

}

//-----------------------------------------------------------------------------
//
// UI_CONNECT - ?????????????? ?????? ????
//
//-----------------------------------------------------------------------------
// ?????? ??????????????????...
void
UIMessageManager::Execute_UI_CONNECT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CONNECT");
	
	if (g_Mode!=MODE_WAIT_SELECTPC)
	{
		DEBUG_ADD("Not MODE_WAIT_SELECTPC");
		return;
	}

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
	if(g_pPremiumGiveItemInfoList!=NULL)
	{
		// ?????????? ?????? ?????????????????????
		g_pPremiumGiveItemInfoList->Give_Item_List_ClearAll();
	}
#endif //__PREMIUM_GIVE_ITEM_UI

	//------------------------------------------------------------
	// ?????? ?????? - 2001.8.20
	//------------------------------------------------------------
	if (g_pUserOption!=NULL)
	{
		if (g_pUserOption->PlayWaveMusic)
		{
//			if (g_pDXSoundStream!=NULL)
//			{
//				g_pDXSoundStream->Stop();
//			}
#ifdef __USE_MP3__
			if (g_pMP3 != NULL)
			{
				DEBUG_ADD("MP3 STOP13");
				g_pMP3->Stop();
				DEBUG_ADD("MP3 STOP13 OK");
			}
#else
// 			if (g_pOGG != NULL)
// 			{
				DEBUG_ADD("MP3 STOP13");
				if( g_DXSound.IsInit() )
					BGMManager::Instance()->StopMusic();
					//g_pOGG->streamClose();
				DEBUG_ADD("MP3 STOP13 OK");
//			}
		}
		else
		{
			g_Music.Stop();
		}
	}

	//------------------------------------------------------------
	// ??????????????????????????
	//------------------------------------------------------------
	if (g_pUIDialog!=NULL)
	{
		//g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_LOGOUT_DIED].GetString() );
		g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CONNECTING_SERVER].GetString(), -1, -1, DIALOG_TITLE_NO_BUTTON);

//		if (CDirect3D::IsHAL())
//		{
//			DEBUG_ADD("UI show1");
//			gC_vs_ui.Show();
//			//gC_vs_ui.DrawMousePointer();		
//			DEBUG_ADD("UI show1 OK");
//		}
//		else
		{
			POINT point;
			
			DEBUG_ADD("UI show2");
			gC_vs_ui.Show();
			DEBUG_ADD("UI show2 OK");

			//gC_vs_ui.DrawMousePointer();

			//-----------------------------------------------------------------
			// Last????Back?????? copy - 3D HAL???????? ?????.
			//-----------------------------------------------------------------		
			point.x = 0;
			point.y = 0;
			RECT rect = { 0, 0, SURFACE_WIDTH, SURFACE_HEIGHT };
			g_pBack->BltNoColorkey( &point, g_pLast, &rect );	
		}
		CDirectDraw::Flip();

		// ???? ???????? ????????????.. ??????
		DEBUG_ADD("close msg dlg");
		g_pUIDialog->CloseMessageDlg();
	}

	DEBUG_ADD("LoadingAddonSPK");

	//------------------------------------------------------------
	// Test 2001.8.20
	//------------------------------------------------------------
	// ???? ?????? ???? ????
	// ?????????? ???? ???? ???? - 2001.8.20
	LoadingAddonSPK( false );
	

	DEBUG_ADD_FORMAT("CLSelectPC(%d)", left);

	CLSelectPC clSelectPC;
	// CGConnect ?????????????????????? ????????????????.					
	//clSelectPC.setPCName( (const char *)g_pUserInformation->CharacterW[left] );//"sigi");
	clSelectPC.setPCName( (const char *)g_pUserInformation->Character[left] );//"sigi");
	clSelectPC.setPCType( (PCType)(g_pUserInformation->Race[left]));

	DEBUG_ADD_FORMAT("socket Set PC type");

	g_pSocket->setPCType(clSelectPC.getPCType());
	g_pSocket->setPCName(clSelectPC.getPCName());

	DEBUG_ADD_FORMAT("user Information");

	g_pUserInformation->CharacterID = g_pUserInformation->Character[left];
	g_pUserInformation->FaceStyle = g_pUserInformation->FaceStyleSlot[left];	
	g_pUserInformation->IsMaster = 
		strncmp( g_pUserInformation->CharacterID, (*g_pGameStringTable)[UI_STRING_MESSAGE_MASTER_NAME].GetString(), (*g_pGameStringTable)[UI_STRING_MESSAGE_MASTER_NAME].GetLength() ) == 0 
//		(strstr(g_pUserInformation->CharacterID, "??????")!=NULL)
		|| g_pInventory->FindItem(ITEM_CLASS_ETC, 0);
//		|| g_pPlayer->GetCreatureType()==CREATURETYPE_SLAYER_OPERATOR
//		|| g_pPlayer->GetCreatureType()==CREATURETYPE_VAMPIRE_OPERATOR;
//		g_pPlayer ?????????? ??????????????????...????????????????.???~

	#ifdef OUTPUT_DEBUG
		DEBUG_ADD("MasterCheck");
		std::ifstream file("master.txt");

		char str[256];
		while (!file.eof())
		{
			file.getline(str, 256);
			
			int n = file.gcount();

			if (n <= 0)
				break;
			
			str[n] = '\0';

			g_pUserInformation->IsMaster |= (strstr((char*)g_pUserInformation->CharacterID, str)!=NULL);
		}
		
		file.close();
		DEBUG_ADD("MasterCheck OK");
	#endif
	
	//g_pUserInformation->CharacterIDW = g_pUserInformation->CharacterW[left];
	DEBUG_ADD("send CLSelectPC");
	g_pSocket->sendPacket( &clSelectPC );
	g_pSocket->setPlayerStatus( CPS_AFTER_SENDING_CL_SELECT_PC );

	// ???? ??????????
	UpdateSocketOutput();

	SaveLastSelectedCharacter( left );

	DEBUG_ADD("set mode MODE_WAIT_RECONNECT");
	SetMode(MODE_WAIT_RECONNECT);
#endif
	//------------------------------
	// ?????? UI
	//------------------------------
	//gC_vs_ui.StartGame();
					
	//
	// Skill????StartGame()???????????????????????????? ????.
	//
	//gC_vs_ui.AddSlayerSkill(SKILL_FLASH_SLASHER);
}


// ???????????????????????? ????
struct ToUpperFunctor
{
	void operator()(char& ch)
	{
		if(ch >= 'a' && ch <= 'z')
			ch -= ('a' - 'A');
	}
};

//-----------------------------------------------------------------------------
//
// Chat?????? Enter??????????????
//
//-----------------------------------------------------------------------------
// ??????????
void
UIMessageManager::Execute_UI_CHAT_RETURN(int left, int right, void* void_ptr)
{
	//
	// left = true???? pcs?????? ???????????????? false?????? ????????????
	//
	// ! pcs?????? ???????????slot ???????????? ?????????? ?????? ??????????????????.
	//   => gC_vs_ui.GetSendPossibleSlot();
	//      ????????????????0, 1, 2 ????, NOT_SELECTED???? ?????? slot??????????
	//
	// void_ptr = input std::string
	//	right = color
	//

	DEBUG_ADD("[UI] UI_CHAT_RETURN");

	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	
	DEBUG_ADD_FORMAT("[Original] %s", (char*)void_ptr );
	
	char* chatString = (char*)void_ptr; 
//	g_pChatManager->RemoveCurse(chatString);
	//-------------------------------------------------------------
	// ???? ?????????????
	//-------------------------------------------------------------
	if (chatString!=NULL 
		&& chatString[0]!=SYMBOL_WHISPER	// ???????? ???? ?????.
		//&& chatString[0]!=SYMBOL_MASTER_COMMAND
		&& left==CLD_PARTY)
	{
		if (g_pParty!=NULL )
		{
			// by svi
			if(g_pUserInformation->bKorean == true && g_pClientCommunicationManager!=NULL && 1==2)
			{
				RCSay _RCSay;
				_RCSay.setName( g_pUserInformation->CharacterID.GetString() );
				_RCSay.setMessage( chatString );
				_RCSay.setColor( right );

				if (g_pParty->GetSize()==0)
				{
					DEBUG_ADD("PartySize==0");
				}
			
				for (int i=0; i<g_pParty->GetSize(); i++)
				{
					PARTY_INFO* pInfo = g_pParty->GetMemberInfo( i );
					
					if (pInfo!=NULL)										
					{
						int port = g_pClientConfig->CLIENT_COMMUNICATION_UDP_PORT;

						RequestUserInfo* pUserInfo = g_pRequestUserManager->GetUserInfo( pInfo->Name.GetString() );

						if (pUserInfo!=NULL)
						{
							if (pUserInfo->UDPPort!=0)
								port = pUserInfo->UDPPort;
						}
						
						g_pClientCommunicationManager->sendPacket( 
								pInfo->IP, 
								port,
								&_RCSay );
					}
					else
					{
						DEBUG_ADD_FORMAT("pInfo[%d]==NULL", i);
					}
				}			
				UI_AddChatToHistory( chatString, g_pUserInformation->CharacterID.GetString(), CLD_PARTY, right );
			}
			else	// ??????????????
			{
				CGPartySay _CGPartySay;
				_CGPartySay.setColor(right);
				_CGPartySay.setMessage(chatString);
				g_pSocket->sendPacket( &_CGPartySay );
			}
		}
	}
	// ?????????
	else if (chatString!=NULL 
		&& chatString[0]!=SYMBOL_WHISPER	// ???????? ???? ?????.
		&& chatString[0]!=SYMBOL_MASTER_COMMAND
		&& (left==CLD_GUILD || left == CLD_UNION))
	{
		if(g_pUserInformation->GuildName.GetLength() > 0)
		{
			CGGuildChat _CGGuildChat;
			// 2004, 11, 11, sobeit add start
			if(left==CLD_GUILD)
				_CGGuildChat.SetType(0);
			else
				_CGGuildChat.SetType(1);
			// 2004, 11, 11, sobeit add end
			_CGGuildChat.setMessage( std::string(chatString) );
			_CGGuildChat.setColor( right );
			
			g_pSocket->sendPacket( &_CGGuildChat );
		}
	}
	//-------------------------------------------------------------
	// Player??????????????????.. chatting??????.. ??????????..   --;
	//-------------------------------------------------------------
	else //if (g_pPlayer->IsAlive())
	{
		BOOL bZoneChat = (left==CLD_ZONECHAT);
	
		/*	
		#ifdef __GAME_CLIENT__
			if (g_pSystemMessage!=NULL)
			{
				// ??????.. - -;;
				g_pSystemMessage->Add((*g_pGameStringTable)[STRING_MESSAGE_CHAT_BE_GOOD].GetString());
			}
		#endif
		*/

		//int pcsChat = left;

		//-------------------------------------------------------------
		//
		// PCS chat
		//
		//-------------------------------------------------------------
		//if (pcsChat)
		//{
			/*
			int slot = gC_vs_ui.GetSendPossibleSlot();

			if (slot!=NOT_SELECTED)
			{
				char* strUI = (char*)chatString;

				#ifdef	CONNECT_SERVER
					CGPhoneSay _CGPhoneSay;
					_CGPhoneSay.setSlotID( slot );
					_CGPhoneSay.setMessage( strUI );

					g_pSocket->sendPacket( &_CGPhoneSay );					
				#endif

				char temp[128];
				sprintf(temp, "[%s] %s", g_pUserInformation->CharacterID.GetString(), strUI);
				UI_AddChatToHistory( temp );								
			}
			*/
		//}
		//-------------------------------------------------------------
		//
		// ???? ????
		//
		//-------------------------------------------------------------
		//else
		{
			//
			// chat history Window??????????????.. ???? ???? ????????????.
			//
			//break;

			// server????message ??????????
			//g_Socket.Send(g_String);

			const char* strUI = chatString;
			char* strOrg = new char[128];
			char* str = strOrg;

			strcpy( strOrg, strUI );

			if (str!=NULL && str[0]!=NULL)
			{
				RemoveStringSpace( str );							

				DEBUG_ADD_FORMAT("[After RemoveSpace] %s", str );
					
				if (str!=NULL && str[0]!=NULL)
				{
					// 2004, 10, 25, sobeit add start - ?????? ?????? ??????
					if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_DRAGON_EYES)
#if __CONTENTS(__HERO_SYSTEM
//						|| g_pPlayer->HasEffectStatus(EFFECTSTATUS_SLAYER_HERO) 
//						|| g_pPlayer->HasEffectStatus(EFFECTSTATUS_VAMPIRE_HERO)
//						|| g_pPlayer->HasEffectStatus(EFFECTSTATUS_OUSTERS_HERO)
#endif //__HERO_SYSTEM
						)
					{
						if(0 == strncmp(str, (*g_pGameStringTable)[UI_STRING_MESSAGE_RANGER_SAY].GetString(),(*g_pGameStringTable)[UI_STRING_MESSAGE_RANGER_SAY].GetLength()))
						{
							char TempBuffer[128]; 
							strcpy(TempBuffer, str+(*g_pGameStringTable)[UI_STRING_MESSAGE_RANGER_SAY].GetLength());
							CGRangerSay _CGRangerSay;
							_CGRangerSay.setMessage(TempBuffer);

							g_pSocket->sendPacket( &_CGRangerSay );
										
							// Player????Chat pWansungStringing????????
							g_pPlayer->SetChatString( str );//+1 );

							// history????????
							char temp[128];
							strcpy(temp, str );//+1);
							//sprintf(temp, "[%s] %s", g_pUserInformation->CharacterID.GetString(), str+1);
							//UI_AddChatToHistory( temp );								
							UI_AddChatToHistory( temp, g_pUserInformation->CharacterID.GetString(), CLD_ZONECHAT, right );
							return;
						}
					}
					// 2004, 10, 25, sobeit add end - ?????? ?????? ??????
					if (bZoneChat && str[0]!=SYMBOL_WHISPER
						&& str[0]!=SYMBOL_COMMAND && str[0]!=SYMBOL_MASTER_COMMAND)			
					{
						// system message test
						//------------------------------------------------------------
						//
						// '!'???????????? global????????.
						//
						//------------------------------------------------------------
						//case SYMBOL_GLOBALCHAT :	//if (str[0]=='!')//pWansungString[0]=='/')
						{
							//if (strlen(str) > 1 )	//pWansungString) > 1)
							{		
								//---------------------------------------------------------
								// ?????? ???????????? ????????????
								// ?????? ???????????????????????????
								//---------------------------------------------------------
								if (g_pPlayer->IsUndergroundCreature()
									|| g_pPlayer->IsInCasket()
									|| g_pPlayer->GetCreatureType()==CREATURETYPE_BAT
									|| g_pPlayer->GetCreatureType()==CREATURETYPE_VAMPIRE_GHOST
									|| g_pPlayer->GetCreatureType()==CREATURETYPE_WOLF
									|| g_pPlayer->GetCreatureType()==CREATURETYPE_WER_WOLF
#if __CONTENTS(__FAST_TRANSFORTER)
									|| g_pPlayer->GetCreatureType()==CREATURETYPE_FLITTERMOUSE
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
									|| g_pPlayer->GetCreatureType()==CREATURETYPE_SHAPE_OF_DEMON
#endif //__SECOND_TRANSFORTER
									)
								{
									// history????????
									//char temp[128];
									//sprintf(temp, "[%s] .....", g_pUserInformation->CharacterID.GetString());
									//UI_AddChatToHistory( temp );								
									UI_AddChatToHistory( ".....", g_pUserInformation->CharacterID.GetString(), CLD_NORMAL, right );
								}
								//---------------------------------------------------------
								// global say
								//---------------------------------------------------------
								else
								{
									//---------------------------------------------------------
									// delay???????????????????? ????????????
									//---------------------------------------------------------
									if (1)//g_CurrentTime > g_pUserInformation->GlobalSayTime+g_pClientConfig->DELAY_GLOBAL_SAY
										//#if defined(OUTPUT_DEBUG) && defined(_DEBUG)
										//	|| 1
										//#endif
										//)
									{
										CGGlobalChat _CGGlobalChat;
										_CGGlobalChat.setMessage( str );//+ 1 );	//pWansungString+1 );
										_CGGlobalChat.setColor( right );
										g_pSocket->sendPacket( &_CGGlobalChat );

										
										// Player????Chat pWansungStringing????????
										g_pPlayer->SetChatString( str, right );//+1 );

										// history????????
										char temp[128];
										strcpy(temp, str );//+1);
										//sprintf(temp, "[%s] %s", g_pUserInformation->CharacterID.GetString(), str+1);
										//UI_AddChatToHistory( temp );								
										UI_AddChatToHistory( temp, g_pUserInformation->CharacterID.GetString(), CLD_ZONECHAT, right );

										// ?????? ????????????????????
										//g_pUserInformation->GlobalSayTime = g_CurrentTime;

										// [??????] ????????????????
//										__BEGIN_HELP_EVENT
////											ExecuteHelpEvent( HE_CHAT_SHOUT );	
//										__END_HELP_EVENT
									}
									//else
									//{
										// ??????????????????????????????????????
									//	UI_AddChatToHistory( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_GLOBAL_SAY].GetString(), NULL, CLD_INFO );
									//}
								}
							}
						}
						//break;

					}					
					else // ??????.. ???????? ???? ??????????. - -;;
					switch ( str[0] )
					{
						//------------------------------
						// ??????????--> ????????
						//------------------------------
						//char* pWansungString = new char [strlen(str)+1];

						//UI_JohapToWansung( str, pWansungString );

						//------------------------------------------------------------
						//
						// '/'???????????? whisper????
						//
						//------------------------------------------------------------
						case SYMBOL_WHISPER :						
						{

							if (strlen(str) > 1 )
							{
								const int ChatCommandCnt = 2;

								const char *szEnglishCommand[ChatCommandCnt] = {
									"/DJEL", "/W",
								};
				
								const char *szKoreanCommand[ChatCommandCnt] = { nullptr };

								void (*pCommandFunction[ChatCommandCnt])(STRING_VEC&, LPCTSTR, int) = {
									ChatCommand_Where,
									ChatCommand_Whisper,
								};

								STRING_VEC	ChatCommandStr;
								CToken		strToken(str);

								for (const char* pCommand = strToken.GetToken();
									pCommand != NULL;
									pCommand = strToken.GetToken())
								{
									ChatCommandStr.push_back(pCommand);
								}
								
								STRING_VEC::iterator CommandPos = ChatCommandStr.begin();
								if(CommandPos == ChatCommandStr.end())
									break;

								std::for_each(CommandPos->begin(), CommandPos->end(), ToUpperFunctor());
																
								for(int i = 0; i < ChatCommandCnt; ++i)
								{
									if((szEnglishCommand[i] != NULL && *CommandPos == szEnglishCommand[i]) ||
									   (szKoreanCommand[i] != NULL && *CommandPos == szKoreanCommand[i]))
									{
										(*pCommandFunction[i])(ChatCommandStr, strUI, right);
										break;
									}
								}
							}
						}

						break;

						

						//------------------------------------------------------------
						//
						// '@'???????????? ???? ?????????
						//
						//------------------------------------------------------------
						case SYMBOL_COMMAND :
						//else if (//g_pPlayer->IsVampire() && 
						//			str[0]=='@')
						{
							if (strlen(str) > 1 )
							{
								// '@'?????? ??????????????..
								CToken strToken(str+1);

								// [??????]+[ ]+[??????] ?????? ???????????????????? ????.										
								const char* pCommand = strToken.GetToken();
								const char* pData = strToken.GetEnd();

								char pLwrCommand[128];
								strcpy(pLwrCommand, pCommand);
								strcpy(pLwrCommand, _strlwr(pLwrCommand));

								
								DEBUG_ADD_FORMAT("[Command] %s %s", pCommand, pData);
								
								//-------------------------------------------------------
								// ???? ????
								//-------------------------------------------------------
								if (strcmp(pCommand, STRING_IGNORE)==0
									|| strcmp(pLwrCommand, STRING_IGNORE_ENG)==0)
								{
									//-------------------------------------------------------
									// ?????????????????????????????.
									//-------------------------------------------------------
									if (pData==NULL)
									{
										g_pChatManager->ClearID();
										g_pChatManager->SetIgnoreMode();	

										// ???? ID??????????????
										g_pChatManager->AddID( g_pUserInformation->CharacterID.GetString() );

										char strTemp[128];
										sprintf(strTemp, (*g_pGameStringTable)[STRING_MESSAGE_CHAT_IGNORE_ALL].GetString(), pData);
										UI_AddChatToHistory( strTemp, NULL, CLD_INFO, right );

										// [??????] ????????
//										__BEGIN_HELP_EVENT
////											ExecuteHelpEvent( HE_CHAT_REJECT );	
//										__END_HELP_EVENT
									}
									//-------------------------------------------------------
									// ????????????????????????????.
									//-------------------------------------------------------
									else
									{
										int nameLen = strlen( pData );

										//------------------------------------------------------
										// ID?????????????????????									
										//------------------------------------------------------
										if (nameLen<PlayerInfo::minIDLength || nameLen>PlayerInfo::maxIDLength)
										{
											char strTemp[256];
											sprintf(strTemp, (*g_pGameStringTable)[STRING_USER_REGISTER_ID_LENGTH].GetString(), 
													PlayerInfo::minIDLength,
													nameLen>PlayerInfo::maxIDLength);

											UI_AddChatToHistory( strTemp, NULL, CLD_INFO, right );
										}
										//------------------------------------------------------
										// ??????
										//------------------------------------------------------
										else
										{
											if (g_pChatManager->IsAcceptMode())
											{
												g_pChatManager->AddID( pData );
											}
											else
											{
												g_pChatManager->RemoveID( pData );
											}

											char strTemp[128];
											sprintf(strTemp, (*g_pGameStringTable)[STRING_MESSAGE_CHAT_IGNORE].GetString(), pData);
											UI_AddChatToHistory( strTemp, NULL, CLD_INFO, right );

											// [??????] ???????? ????????
//											__BEGIN_HELP_EVENT
////												ExecuteHelpEvent( HE_CHAT_REJECT_USER );	
//											__END_HELP_EVENT
										}
									}
								}
								//-------------------------------------------------------
								// ???? ?????
								//-------------------------------------------------------
								else if (strcmp(pCommand, STRING_ACCEPT)==0
										|| strcmp(pLwrCommand, STRING_ACCEPT_ENG)==0)
								{
									//-------------------------------------------------------
									// ??????????????????????????????
									//-------------------------------------------------------
									if (pData==NULL)
									{
										g_pChatManager->ClearID();
										g_pChatManager->SetAcceptMode();	

										char strTemp[128];
										sprintf(strTemp, (*g_pGameStringTable)[STRING_MESSAGE_CHAT_ACCEPT_ALL].GetString(), pData);
										UI_AddChatToHistory( strTemp, NULL, CLD_INFO, right );
									}
									//-------------------------------------------------------
									// ?????????????????????????????
									//-------------------------------------------------------
									else
									{
										int nameLen = strlen( pData );

										//------------------------------------------------------
										// ID?????????????????????									
										//------------------------------------------------------
										if (nameLen<PlayerInfo::minIDLength || nameLen>PlayerInfo::maxIDLength)
										{
											char strTemp[256];
											sprintf(strTemp, (*g_pGameStringTable)[STRING_USER_REGISTER_ID_LENGTH].GetString(), 
													PlayerInfo::minIDLength,
													nameLen>PlayerInfo::maxIDLength);

											UI_AddChatToHistory( strTemp, NULL, CLD_INFO, right );
										}
										//------------------------------------------------------
										// ??????
										//------------------------------------------------------
										else
										{
											if (g_pChatManager->IsAcceptMode())
											{
												g_pChatManager->RemoveID( pData );
											}
											else
											{	
												g_pChatManager->AddID( pData );
											}

											char strTemp[128];
											sprintf(strTemp, (*g_pGameStringTable)[STRING_MESSAGE_CHAT_ACCEPT].GetString(), pData);
											UI_AddChatToHistory( strTemp, NULL, CLD_INFO, right );
										}
									}
								}
								//-------------------------------------------------------
								// ?????????? ??????
								//-------------------------------------------------------
								else if (strcmp(pCommand, STRING_ACCEPT_CURSE)==0
										|| strcmp(pLwrCommand, STRING_ACCEPT_CURSE_ENG)==0)
								{
									g_pUserOption->FilteringCurse = FALSE;

									char strTemp[128];
									sprintf(strTemp, (*g_pGameStringTable)[STRING_MESSAGE_CHAT_ACCEPT_CURSE].GetString(), pData);
									UI_AddChatToHistory( strTemp, NULL, CLD_INFO, right );
								}
								//-------------------------------------------------------
								// ?????????? ??????
								//-------------------------------------------------------
								else if (strcmp(pCommand, STRING_FILTER_CURSE)==0
										|| strcmp(pLwrCommand, STRING_FILTER_CURSE_ENG)==0)
								{
									g_pUserOption->FilteringCurse = TRUE;

									char strTemp[128];
									sprintf(strTemp, (*g_pGameStringTable)[STRING_MESSAGE_CHAT_FILTER_CURSE].GetString(), pData);
									UI_AddChatToHistory( strTemp, NULL, CLD_INFO, right );
								}

								#if defined(OUTPUT_DEBUG) //&& defined(_DEBUG)
									//-------------------------------------------------------
									// ??????????
									//-------------------------------------------------------
									else if (strcmp(pCommand, "connect")==0
											|| strcmp(pLwrCommand, "connect")==0)
									{
										CToken strToken(pData);

										// [IP]+[]+[Name]
										const char* pIP = strToken.GetToken();
										const char* pName = strToken.GetEnd();

										//RequestConnect( pIP, pName );
										if (g_pParty!=NULL)
										{
											PARTY_INFO* pInfo = g_pParty->GetMemberInfo( pName );

											if (pInfo!=NULL)
											{
												pInfo->IP = pIP;
											}
										}
									}
									//-------------------------------------------------------
									// ??????????
									//-------------------------------------------------------
									else if (strcmp(pCommand, "disconnect")==0
											|| strcmp(pLwrCommand, "disconnect")==0)
									{
										const char* pName = pData;
										//RequestDisconnect( pData );
										if (g_pParty!=NULL)
										{
											PARTY_INFO* pInfo = g_pParty->GetMemberInfo( pName );

											if (pInfo!=NULL)
											{
												pInfo->IP = "0.0.0.0";
											}
										}
									}
									//-------------------------------------------------------
									// Profile ???? ??????
									//-------------------------------------------------------
									else if (strcmp(pCommand, "profile")==0
											|| strcmp(pLwrCommand, "profile")==0)
									{
										const char* pName = pData;

										if (!g_pProfileManager->HasProfile(pName)
											&& !g_pProfileManager->HasProfileNULL(pName))
										{
											g_pProfileManager->RequestProfile(pName);
										}
									}
									//-------------------------------------------------------
									// ????????
									//-------------------------------------------------------
									else if (strcmp(pCommand, "say")==0
											|| strcmp(pLwrCommand, "say")==0)
									{
										if (g_pParty!=NULL
											&& g_pClientCommunicationManager!=NULL)
										{
											RCSay _RCSay;
											_RCSay.setName( g_pUserInformation->CharacterID.GetString() );
											_RCSay.setMessage( pData );
											_RCSay.setColor( right );
										
											for (int i=0; i<g_pParty->GetSize(); i++)
											{
												PARTY_INFO* pInfo = g_pParty->GetMemberInfo( i );
												
												if (pInfo!=NULL)										
												{
													int port = g_pClientConfig->CLIENT_COMMUNICATION_UDP_PORT;

													RequestUserInfo* pUserInfo = g_pRequestUserManager->GetUserInfo( pInfo->Name.GetString() );

													if (pUserInfo!=NULL)
													{
														if (pUserInfo->UDPPort!=0)
															port = pUserInfo->UDPPort;
													}

													g_pClientCommunicationManager->sendPacket( 
															pInfo->IP, 
															port, 
															&_RCSay );
												}
											}	
										}
									}
									//-------------------------------------------------------
									// npc ??????????
									//-------------------------------------------------------
									else if (strcmp(pCommand, "npc")==0
											|| strcmp(pLwrCommand, "npc")==0)
									{
										g_MaxNPC = atoi( pData );
									}									
									//-------------------------------------------------------
									// ProfilerInfoName ??????
									//-------------------------------------------------------
									else if (strcmp(pCommand, "profileName")==0
											|| strcmp(pLwrCommand, "profilename")==0)
									{
										if (g_pProfiler!=NULL)
										{
											g_ProfileInfoName = pData;
										}
									}
									//-------------------------------------------------------
									// ProfilerInfoName ??????
									//-------------------------------------------------------
									else if (strcmp(pCommand, "profileClear")==0
											|| strcmp(pLwrCommand, "profileclear")==0)
									{
										if (g_pProfiler!=NULL)
										{
											g_pProfiler->Release();
										}
									}
									//-------------------------------------------------------
									// ?????? ??????
									//-------------------------------------------------------
									else if (strcmp(pCommand, "gamma")==0
											|| strcmp(pLwrCommand, "gamma")==0)
									{
										const char* pValue = pData;

										int value = atoi( pValue );

										if (value==999)
										{
											CDirectDraw::RestoreGammaRamp();
										}
										else
										{
											CDirectDraw::SetGammaRamp( value );
										}
									}
									
								#endif
							}
						}
						break;
						case SYMBOL_MASTER_COMMAND:
						{
							static std::vector<std::string> vMasterCommand;
							static std::string	strTempCommand;

							char *pMessage = str;
							bool bInvincible = g_pPlayer->GetInvincible();//static?????????????? ????????????????

							if(strcmp(str, "*iddqd") == 0 || strcmp(str, "*Tnrrkt") == 0 || strcmp(str, "*??????") == 0)
							{
								if(bInvincible == true)
									strTempCommand = "*command invincible off";
								else
									strTempCommand = "*command invincible on";
								g_pPlayer->SetInvincible(!bInvincible);

								pMessage = const_cast<char*>(strTempCommand.c_str());
							}
							else if(strcmp(str, "*gaonashi") == 0 || strcmp(str, "*gaonasi") == 0 || strcmp(str, "*rkdhsktl") == 0 || strcmp(str, "*????????????") == 0 || strcmp(str,"*ghost") == 0)
							{
								if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_GHOST))
									strTempCommand = "*command ghost off";
								else
									strTempCommand = "*command ghost on";
								pMessage = const_cast<char*>(strTempCommand.c_str());
							}
							else if(strcmp(str, "*goes") == 0 )
							{
								strTempCommand = "*warp 8000 75 77";
								pMessage = const_cast<char*>(strTempCommand.c_str());
							}
							else if(strncmp(str, "*mc", 3) == 0 && isdigit(str[4]))
							{
//								if(vMasterCommand.empty())
								{
									std::string filename = "MasterCommand";
									filename += str[4];
									filename += ".txt";
									if(!_access(filename.c_str(), 0))
									{
										FILE *fp;

										fp = fopen(filename.c_str(), "rt");

										if(fp != NULL)
										{
											char szTemp[512];
											while(fgets(szTemp, 512, fp))
											{
												if( strlen(szTemp) > 0 )
												{
													if(szTemp[strlen(szTemp)-1] == '\n')
													szTemp[strlen(szTemp)-1] = '\0';
													char *pszTemp = new char[strlen(szTemp)+1];
													strcpy( pszTemp, szTemp );
													
													Execute_UI_CHAT_RETURN(left, right, (void *)pszTemp);
												}

//												CGSay _CGSay;
//												_CGSay.setMessage( szTemp );	//pWansungString );
//												_CGSay.setColor( right );
//												g_pSocket->sendPacket( &_CGSay );
											}
										}
										else
										{
											DEBUG_ADD("[Master Command] File pointer is NULL.");
										}
									}
									else
									{
										DEBUG_ADD("[Master Command] File access is Denied.");
									}
								}
							}
							else if(strncmp(str, "*C2G", 4) == 0 )
							{
								if( strcmp(str+5, "on") == 0 )
								{
									g_C2G = 1;
								}
								else
								{
									g_C2G--;
								}
//								for( int grid_y = 0; grid_y < g_pInventory->GetHeight(); grid_y++ )
//								for( int grid_x = 0; grid_x < g_pInventory->GetWidth(); grid_x++ )
//								{
//									C_VS_UI_INVENTORY::AutoMove( grid_x, grid_y );
//									gpC_base->DispatchMessage();
//								}
							}
							else if( g_C2G > 0 && strncmp(str, "*create", 7 ) == 0 )
							{
								g_C2G++;
							}

//							{
//								std::string aaaa[4];
//								aaaa[0] = "*set war_active off";
//								aaaa[1] = "*command removeRaceWar";
//								aaaa[2] = "*set war_active on";
//								aaaa[3] = "*set war_active off";
//
//								CGSay _CGSay;
//
//								_CGSay.setColor( right );
//
//								for(int i=0;i<4;i++)
//								{
//									_CGSay.setMessage( aaaa[i].c_str() );									
//									g_pSocket->sendPacket( &_CGSay );
//								}
//							}

							bool bSay = true;
#ifndef __METROTECH_TEST
							if(g_pUserInformation->bCompetence == true && g_pUserInformation->bCompetenceShape != true)
							{
								if(g_pZoneTable->Get( g_pZone->GetID() )->CompetenceZone == true)
								{
								}
								else
								{
									if(strstr(pMessage, "*command ghost off") != NULL)
										bSay = false;
								}
							}
#endif

							if(bSay == true)
							{
//??????????
#if __CONTENTS(__AUTO_ITEM_ROOTING)
								if(g_pPlayer != NULL)
								{
									memset((char*)g_pPlayer->arrchRootingTest, NULL, sizeof(g_pPlayer->arrchRootingTest));

									strcat(g_pPlayer->arrchRootingTest, str);
									if(strstr(g_pPlayer->arrchRootingTest, "*command autoItemRooting") == NULL)
									{
										memset((char*)g_pPlayer->arrchSummon, NULL, sizeof(g_pPlayer->arrchRootingTest));
										strcat(g_pPlayer->arrchSummon, str);
									}
								}
#endif // __AUTO_ITEM_ROOTING

								CGSay _CGSay;
								_CGSay.setMessage( pMessage );	//pWansungString );
								_CGSay.setColor( right );
								g_pSocket->sendPacket( &_CGSay );
								
//								UI_AddChatToHistory( pMessage, g_pUserInformation->CharacterID.GetString(), CLD_NORMAL, right );
							}
							break;
						}

						//------------------------------------------------------------
						//
						//	???? ????
						//
						//------------------------------------------------------------
						default :
						{
							//------------------------------
							// ?????? ???????????? ????????????
							// ?????? ???????????????????????????
							//------------------------------
							if (g_pPlayer->IsUndergroundCreature()
								|| g_pPlayer->IsInCasket()
								|| g_pPlayer->GetCreatureType()==CREATURETYPE_BAT
								|| g_pPlayer->GetCreatureType()==CREATURETYPE_VAMPIRE_GHOST
								|| g_pPlayer->GetCreatureType()==CREATURETYPE_WOLF
								|| g_pPlayer->GetCreatureType()==CREATURETYPE_WER_WOLF
#if __CONTENTS(__FAST_TRANSFORTER)
									|| g_pPlayer->GetCreatureType()==CREATURETYPE_FLITTERMOUSE
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
									|| g_pPlayer->GetCreatureType()==CREATURETYPE_SHAPE_OF_DEMON
#endif //__SECOND_TRANSFORTER
								)
							{
								// history????????
								//char temp[128];
								//sprintf(temp, "[%s] .....", g_pUserInformation->CharacterID.GetString());
								//UI_AddChatToHistory( temp );
								UI_AddChatToHistory( ".....", g_pUserInformation->CharacterID.GetString(), CLD_NORMAL, right );
							}
							else
							{
								CGSay _CGSay;
								_CGSay.setMessage( str );	//pWansungString );
								_CGSay.setColor( right );
								g_pSocket->sendPacket( &_CGSay );

									
								// Player????Chat pWansungStringing????????
								g_pPlayer->SetChatString( str, right );

								// history????????
								//char temp[256];
								//sprintf(temp, "%s> %s", g_pUserInformation->CharacterID.GetString(), str);
								//UI_AddChatToHistory( temp );
								UI_AddChatToHistory( str, g_pUserInformation->CharacterID.GetString(), CLD_NORMAL, right );
							}
						}	
						
						//delete [] str;
						//delete [] pWansungString;
					}
				}
			}

			DeleteNewArray(strOrg);
		}					
	}

	DeleteNewArray(void_ptr);

//	__BEGIN_HELP_EVENT
		ExecuteHelpEvent( HELP_EVENT_CHAT );
//	__END_HELP_EVENT

}


//-----------------------------------------------------------------------------
//
// UI_NEW_OPTION ??GameMenu butonlar??
// FAZ 4 (2026-05-03): ?nceden Execute_UI_NEW_OPTION yaln?zca declare
// ediliyordu (UIMessageManager.h:61), implementasyon yoktu. Sonu??
// GameMenu'deki t?m butonlar (Gear/Skill/Quest/Pet/Friend/Party/TMall/
// Option/Help) silent no-op idi.
//
// 2026-05-22: ROOT CAUSE FIX ??CloseGameMenu ?a?r?s??eklendi.
// ?nceki bug: C_VS_UI_GAME::HotKey_X handler'lar??"if (!m_pC_gamemenu) skip"
// kontrol??yap?yor. GameMenu::Run dispatch'i SendMessage(UI_NEW_OPTION,N)
// senkron ?a?r????Execute_UI_NEW_OPTION case N ??HotKey_X. AMA m_pC_gamemenu
// hala set (Run sonras??Finish() ?a?r?l?yor olsa bile bu noktada). Sonu??
// HotKey_X no-op ??buton t?klan?r ama hedef pencere a??lmaz. Referans pattern
// (Referans Proje UIMessageManager.cpp:4794) ?nce CloseGameMenu() ?a??r?r,
// m_pC_gamemenu = NULL yapar, sonra HotKey_X dispatch eder.
//
// 2026-05-22: Defansif game state pre-check eklendi (referans pattern 4787).
// Player MODE_GAME de?ilse veya ?ld?yse hi?bir dispatch yap?lmaz.
//
// 2026-05-22: case 4 PET fix ??HotKey_PetInfo dispatch (referans 4807 pattern).
// 2026-05-22: case 5 FRIEND fix ??toggle pattern (referans 4809-4838 pattern,
// open/close + FriendWaitList paralel).
//
// Dispatch tablosu:
//   1=GEAR    ??HotKey_Gear
//   2=SKILL   -> ToggleMilitary (referans Skill/Rune penceresi)
//   3=QUEST   ??RunQuestStatusWindow (Phoenix; referans HotKey_Quest e?de?er)
//   4=PET     ??HotKey_PetInfo
//   5=FRIEND  ??C_VS_UI_FRIEND_SYSTEM + C_VS_UI_FRIEND_WAIT toggle
//   6=PARTY   ??HotKey_Party
//   7=TMALL   ??ToggleTMall
//   8=OPTION  ??RunOption(true) (Phoenix; Title Option layout)
//   9=HELP    ??HotKey_Help (Phoenix; referans CGRequestPayEventList CHS-specific)
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_NEW_OPTION(int left, int right, void* void_ptr)
{
	DEBUG_ADD_FORMAT("[UI] UI_NEW_OPTION %d", left);

	if (g_Mode != MODE_GAME || (g_pPlayer != NULL && g_pPlayer->IsDead()))
	{
		DEBUG_ADD("[UI] UI_NEW_OPTION: skip (Not MODE_GAME or player dead)");
		return;
	}

	gC_vs_ui.CloseGameMenu();

	switch (left)
	{
	case 1:  gC_vs_ui.HotKey_Gear();             break;
	case 2:  gC_vs_ui.ToggleMilitary();          break;
	case 3:  gC_vs_ui.RunQuestStatusWindow();    break;
	case 4:  gC_vs_ui.HotKey_PetInfo();          break;
	case 5:
		{
			C_VS_UI_FRIEND_SYSTEM* pFriend =
				(C_VS_UI_FRIEND_SYSTEM*)gpC_window_manager->GetWindowbyName("FriendSystem");
			if (pFriend)
			{
				if (pFriend->Running())
					pFriend->Finish();
				else
					pFriend->Start();
			}
			else
			{
				pFriend = new C_VS_UI_FRIEND_SYSTEM;
				pFriend->SetWindowName("FriendSystem");
				pFriend->Start();
			}
			C_VS_UI_FRIEND_WAIT* pFriendW =
				(C_VS_UI_FRIEND_WAIT*)gpC_window_manager->GetWindowbyName("FriendWaitList");
			if (pFriendW)
			{
				if (pFriendW->Running())
					pFriendW->Finish();
				else
					pFriendW->Start();
			}
			else
			{
				pFriendW = new C_VS_UI_FRIEND_WAIT;
				pFriendW->SetWindowName("FriendWaitList");
				pFriendW->Start();
			}
			break;
		}
	case 6:  gC_vs_ui.HotKey_Party();            break;
	case 7:  gC_vs_ui.ToggleTMall();             break;
	case 8:  gC_vs_ui.RunOption(true);           break;
	case 9:  gC_vs_ui.HotKey_Help();             break;
	default:
		DEBUG_ADD_FORMAT("[UI] UI_NEW_OPTION: bilinmeyen left=%d", left);
		break;
	}
}

//-----------------------------------------------------------------------------
// UI_REQUEST_TMALL_LIST
//
// GameMenu T-Mall butonu C_VS_UI_TMALL::ToggleTMall() icinden bu mesaji
// yollar. Phoenix'te mesaj declare edilmis ama register/execute edilmemisti.
// Referans davranis: secili T-Mall kategorisi icin CGRequestTmallList paketi
// server'a gonderilir; cevap GCTMallGoodsList -> UI_RunTMall ile pencereyi acar.
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_REQUEST_TMALL_LIST(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_REQUEST_TMALL_LIST");

	if (g_Mode != MODE_GAME || g_pSocket == NULL)
		return;

	CGRequestTmallList requestTmallList;
	requestTmallList.setKind((BYTE)left);
	g_pSocket->sendPacket(&requestTmallList);
}

//-----------------------------------------------------------------------------
// UI_REQUEST_CASH_CHARGE
//
// 2026-08-20 PayPal Cash Charge: TMall "Recharge" (Button_ChongZhi) bu
// mesaji yollar. Bos body'li CGCashChargeRequest (549) server'a gonderilir;
// cevap GCCashChargeURL (550) ? GCCashChargeURLHandler checkout URL'ini
// WebView2 "Cash Charge" penceresinde acar (fallback: ShellExecute).
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_REQUEST_CASH_CHARGE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_REQUEST_CASH_CHARGE");

	if (g_Mode != MODE_GAME || g_pSocket == NULL)
		return;

	CGCashChargeRequest cashChargeRequest;
	g_pSocket->sendPacket(&cashChargeRequest);
}

//-----------------------------------------------------------------------------
// UI_BUY_TMALL_ITEM
//
// T-Mall satin alma onayi ExecF_Buy_TMall() icinden bu mesaji yollar.
// Referans davranis: kategori ve urun id ile CGBuyTmallGoods paketi gonderilir.
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_BUY_TMALL_ITEM(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_BUY_TMALL_ITEM");

	if (g_Mode != MODE_GAME || g_pSocket == NULL)
		return;

	CGBuyTmallGoods buyTmallGoods;
	buyTmallGoods.setKind((BYTE)left);
	buyTmallGoods.setGoodsID((ObjectID_t)right);
	g_pSocket->sendPacket(&buyTmallGoods);
}

//-----------------------------------------------------------------------------
// UI_REQUEST_MILITARY_SLOTDATA
//
// Skill/Rune penceresi acilmadan once server'dan RankGem slot verisi istenir.
// Phoenix'te bu mesaj register edilmedigi icin ToggleMilitary() sessiz no-op'a
// dusuyordu; K hotkey ve GameMenu Skill butonu bu yuzden pencere acmiyordu.
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_REQUEST_MILITARY_SLOTDATA(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_REQUEST_MILITARY_SLOTDATA");

	if (g_Mode != MODE_GAME || g_pSocket == NULL)
		return;

	CGRequestRankGemData requestRankGemData;
	g_pSocket->sendPacket(&requestRankGemData);
}

//-----------------------------------------------------------------------------
// Logout
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_LOGOUT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_LOGOUT");

	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	
	if (g_pPlayer->IsWaitVerify())
	{
		DEBUG_ADD_FORMAT("WaitVerify %d", g_pPlayer->GetWaitVerify());
		return;
	}
	
	//-------------------------------------------------------------------
	// Logout ?????????????????? ?????? ????????????????????
	// ?????????????????????? ????????????????????
	//-------------------------------------------------------------------
	int zoneID	= (g_bZonePlayerInLarge?g_nZoneLarge : g_nZoneSmall);

	//ZONETABLE_INFO* pZoneInfo = g_pZoneTable->Get( zoneID );
	
	//-------------------------------------------------------------------
	// ?????????????????? logout????????
	// ???? ???? ??????????.
	//-------------------------------------------------------------------
	if (g_bZoneSafe //pZoneInfo!=NULL	&& pZoneInfo->Safety
		// ???????????? ????????..
		|| g_pPlayer->IsInSafeSector()
		|| g_pZone->GetID() == 8000				// ????????????????? ???? ??????
		// ???? ?????? ????????????...
//		|| g_pPlayer->IsDead() && g_pPlayer->GetDeadDelayLast() < 4)
		//|| g_pZone->GetCreatureNumber()==1)
#ifdef __METROTECH_TEST__
		|| 0
#endif
		)
	{
		ExecuteLogout();
	}
	//-------------------------------------------------------------------
	// ??????5?????? logout ????????
	//-------------------------------------------------------------------
	else if (g_pPlayer!=NULL && g_pPlayer->IsAlive())
	{		
		if (g_pUserInformation->LogoutTime == 0)
		{
			// 5?????????????? Logout ????????
			
			#ifdef _DEBUG
				g_pUserInformation->LogoutTime = g_CurrentTime + 2000;
			#else
				g_pUserInformation->LogoutTime = g_CurrentTime + 5000;
			#endif

			MEvent event;
			event.eventID = EVENTID_LOGOUT;
			event.eventDelay = 5000;
			event.eventFlag = EVENTFLAG_SHOW_DELAY_STRING;
			event.eventType = EVENTTYPE_ZONE;
			event.m_StringsID.push_back(STRING_MESSAGE_LOGOUT_AFTER_SECOND);

			g_pEventManager->AddEvent(event);

			g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_LOGOUT);
//			g_pSystemMessage->AddFormat((*g_pGameStringTable)[STRING_MESSAGE_LOGOUT_AFTER_SECOND].GetString(), 5);
		}
		else if (g_pUserInformation->LogoutTime > g_CurrentTime)
		{
			DWORD sec = (g_pUserInformation->LogoutTime - g_CurrentTime) / 1000;

			if (sec > 0)
			{
				MEvent event;
				event.eventID = EVENTID_LOGOUT;
				event.eventDelay = sec*1000;
				event.eventFlag = EVENTFLAG_SHOW_DELAY_STRING;
				event.eventType = EVENTTYPE_ZONE;
				event.m_StringsID.push_back(STRING_MESSAGE_LOGOUT_AFTER_SECOND);
				
				g_pEventManager->AddEvent(event);
				g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_LOGOUT);

//				g_pSystemMessage->AddFormat((*g_pGameStringTable)[STRING_MESSAGE_LOGOUT_AFTER_SECOND].GetString(), sec);
			}
		}

		gC_vs_ui.HotKey_ESC();
	}
	//-------------------------------------------------------------------
	// ?????? ?????.
	//-------------------------------------------------------------------
	else
	{
		g_pSystemMessage->Add( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_LOGOUT_DIED].GetString() );

		gC_vs_ui.HotKey_ESC();
	}	
}

#if __CONTENTS(__080405_FIREST_UI_UPDATE)
//-----------------------------------------------------------------------------
//
// Logout
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_QUITEXIT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_LOGOUT");

	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	
	if (g_pPlayer->IsWaitVerify())
	{
		DEBUG_ADD_FORMAT("WaitVerify %d", g_pPlayer->GetWaitVerify());
		return;
	}
	
	//-------------------------------------------------------------------
	// Logout ?????????????????? ?????? ????????????????????
	// ?????????????????????? ????????????????????
	//-------------------------------------------------------------------
	int zoneID	= (g_bZonePlayerInLarge?g_nZoneLarge : g_nZoneSmall);

	//ZONETABLE_INFO* pZoneInfo = g_pZoneTable->Get( zoneID );
	
	//-------------------------------------------------------------------
	// ?????????????????? logout????????
	// ???? ???? ??????????.
	//-------------------------------------------------------------------
	if (g_bZoneSafe //pZoneInfo!=NULL	&& pZoneInfo->Safety
		// ???????????? ????????..
		|| g_pPlayer->IsInSafeSector()
		|| g_pZone->GetID() == 8000				// ????????????????? ???? ??????
		// ???? ?????? ????????????...
//		|| g_pPlayer->IsDead() && g_pPlayer->GetDeadDelayLast() < 4)
		//|| g_pZone->GetCreatureNumber()==1)
#ifdef __METROTECH_TEST__
		|| 0
#endif
		)
	{
		ExecuteQuitExit();
	}
	//-------------------------------------------------------------------
	// ??????5?????? logout ????????
	//-------------------------------------------------------------------
	else if (g_pPlayer!=NULL && g_pPlayer->IsAlive())
	{		
		if (g_pUserInformation->QuitExitTime == 0)
		{
			// 5?????????????? Logout ????????
			
			#ifdef _DEBUG
				g_pUserInformation->QuitExitTime = g_CurrentTime + 2000;
			#else
				g_pUserInformation->QuitExitTime = g_CurrentTime + 5000;
			#endif

			MEvent event;
			event.eventID = EVENTID_LOGOUT;
			event.eventDelay = 5000;
			event.eventFlag = EVENTFLAG_SHOW_DELAY_STRING;
			event.eventType = EVENTTYPE_ZONE;
			event.m_StringsID.push_back(STRING_MESSAGE_QUITEXIT_AFTER_SECOND);

			g_pEventManager->AddEvent(event);

			g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_LOGOUT);
//			g_pSystemMessage->AddFormat((*g_pGameStringTable)[STRING_MESSAGE_QUITEXIT_AFTER_SECOND].GetString(), 5);
		}
		else if (g_pUserInformation->QuitExitTime > g_CurrentTime)
		{
			DWORD sec = (g_pUserInformation->QuitExitTime - g_CurrentTime) / 1000;

			if (sec > 0)
			{
				MEvent event;
				event.eventID = EVENTID_LOGOUT;
				event.eventDelay = sec*1000;
				event.eventFlag = EVENTFLAG_SHOW_DELAY_STRING;
				event.eventType = EVENTTYPE_ZONE;
				event.m_StringsID.push_back(STRING_MESSAGE_QUITEXIT_AFTER_SECOND);
				
				g_pEventManager->AddEvent(event);
				g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_LOGOUT);

//				g_pSystemMessage->AddFormat((*g_pGameStringTable)[STRING_MESSAGE_QUITEXIT_AFTER_SECOND].GetString(), sec);
			}
		}

		gC_vs_ui.HotKey_ESC();
	}
	//-------------------------------------------------------------------
	// ?????? ?????.
	//-------------------------------------------------------------------
	else
	{
		g_pSystemMessage->Add( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_LOGOUT_DIED].GetString() );

		gC_vs_ui.HotKey_ESC();
	}	
}
#endif //__080405_FIREST_UI_UPDATE

//-----------------------------------------------------------------------------
//
// Skill????????????????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_SELECT_SKILL(int left, int right, void* void_ptr)
{
	DEBUG_ADD_FORMAT("[UI] UI_SELECT_SKILL(%d)", left);

	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	//
	// dw_left = Skill ID
	//
	if (g_pPlayer!=NULL)
	{
		//left = SKILL_OUSTERS_COMBO ; 

		//if(gC_vs_ui.IsRunningPersnalShop()  && left == SKILL_LOVE_CHAIN)	return ; 

		g_pPlayer->SetSpecialActionInfo( left );
		
		// ?????????????? actionCount???????????????????????????
		// ???? ????????. ???????????????????????????????. ????.. - -;
		//g_pPlayer->UnSetRepeatAction();
	}

	// [??????] Skill icon???? ????
//	__BEGIN_HELP_EVENT
////		ExecuteHelpEvent( HE_SKILL_ICON_CHANGE );
//	__END_HELP_EVENT
}

//-----------------------------------------------------------------------------
//
// Skill ?????? ?????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CANCEL_SELECT_SKILL(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CANCEL_SELECT_SKILL");

	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	//
	// Skill ?????? ?????
	//
	if (g_pPlayer!=NULL)
	{
		g_pPlayer->SetSpecialActionInfoNULL();
	}
}

// ???? ?????????? ????????????????????????????????????????.
bool IsExistCorpseFromPlayer(MCreature* OriginCreature, int creature_type)
{
	int sx, sy;

	sx = OriginCreature->GetX();
	sy = OriginCreature->GetY();

	for(int i=-1;i<=1;i++)
	{
		for(int j=-1;j<=1;j++)
		{
			if( sx+i < 0 || sy + j < 0 || sx+i >= g_pZone->GetWidth() || sy+j >= g_pZone->GetHeight() )
				continue;

			const MSector& sector = g_pZone->GetSector( sx+i, sy+j );
			MItem* pItem = sector.GetItem();
			
			if( pItem != NULL && pItem->GetItemClass() == ITEM_CLASS_CORPSE)
			{
				MCreature* pCreature = (dynamic_cast<MCorpse*>(pItem))->GetCreature();
				if( pCreature != NULL && pCreature->GetCreatureType() == creature_type )
					return true;
			}
		}
	}
	return false;
}


//-----------------------------------------------------------------------------
//
// mouse?????? Zone?????? Item?????????????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_DROP_TO_CLIENT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_DROP_TO_CLIENT");

	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}

	MItem* pItem = UI_GetMouseItem();//(MItem*)void_ptr;		

	// Capture The Flag ????????????
	// ?????? ?????????? 3????????????????.		
	// ????????????????? ??????????????.

	if(!( pItem != NULL && pItem->GetItemClass() == ITEM_CLASS_EVENT_ITEM && pItem->GetItemType() == 27 ) )
	{
		if( IsExistCorpseFromPlayer( dynamic_cast<MCreature*>( g_pPlayer ), 670 ) )
		{
			g_pSystemMessage->Add( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_DROP_ITEM_BY_FLAG].GetString() );
			return;
		}
	}
				
	if (!g_bWatchMode
		// ???? ?????????????? ??????????.
		&& !UI_IsRunningExchange()
		// ???? ???? ????????. ?????? ???? ???????????????? ??????????.
		&& g_pUserInformation->ItemDropEnableTime < g_CurrentTime)
	{
		//void_ptr = MItem *

		if( pItem != NULL && 
				(
					gC_vs_ui.IsRunningRemoveOptionFromRareItem() && pItem->GetItemClass() == ITEM_CLASS_MIXING_ITEM 
				)
			)
//			pItem != NULL && pItem->GetItemClass() == ITEM_CLASS_QUEST_ITEM && pItem->GetItemType() == 4 )
			return;
				
		if (pItem != NULL && (pItem->GetItemClass() == ITEM_CLASS_RELIC ||
			pItem->GetItemClass() == ITEM_CLASS_BLOOD_BIBLE || 
			pItem->GetItemClass() == ITEM_CLASS_CASTLE_SYMBOL ||
			pItem->GetItemClass() == ITEM_CLASS_EVENT_ITEM && (pItem->GetItemType() == 27 || pItem->GetItemType() == 31) ||
			pItem->GetItemClass() == ITEM_CLASS_SWEEPER || pItem->GetItemClass() == ITEM_CLASS_PET_FOOD
			))
		{
			// ???????????????? ?????????????????? ??-_-;
			int CreatureID = g_pTopView->GetSelectedCreature();

			MCreature *pCreature = NULL;
			bool		bCorpse = false;

			if(CreatureID == -1)
			{
				CreatureID = g_pTopView->GetSelectedItemID();
				MItem *selectedItem = g_pZone->GetItem(CreatureID);
				if(selectedItem != NULL)
				{
					if(selectedItem->GetItemClass() == ITEM_CLASS_CORPSE)
					{
						MCorpse *pCorpse = (MCorpse *)selectedItem;
						pCreature = pCorpse->GetCreature();
						bCorpse = true;
					}
				}
			}
			else
			{
//				((g_pPlayer->IsSlayer() && pCreature->GetCreatureType() >= 371 && pCreature->GetCreatureType() <= 373)
//				|| (g_pPlayer->IsVampire() && pCreature->GetCreatureType() >= 374 && pCreature->GetCreatureType() <= 376))
				pCreature = g_pZone->GetCreature(CreatureID);
			}

			if(g_pPlayer->IsItemCheckBufferNULL() && pCreature != NULL)
			{
				if(bCorpse)	// ???????????????????? ??????
				{
					if(pCreature->GetCreatureType() >= 371 && pCreature->GetCreatureType() <= 376 || 
						pCreature->GetCreatureType() >= 560 && pCreature->GetCreatureType() <= 563 ||
						pCreature->GetCreatureType() >= 526 && pCreature->GetCreatureType() <= 549 ||
						pCreature->GetCreatureType() == 670	&& pItem->GetItemClass() == ITEM_CLASS_EVENT_ITEM || // ??????????
						pCreature->GetCreatureType() == 672 && pItem->GetItemClass() == ITEM_CLASS_SWEEPER
					)
					{
						if(abs(g_pPlayer->GetX()-pCreature->GetX()) < 3 && abs(g_pPlayer->GetY()-pCreature->GetY()) < 3)
						{
							CGRelicToObject _CGRelicToObject;
							_CGRelicToObject.setItemObjectID(pItem->GetID());
							_CGRelicToObject.setObjectID(CreatureID);
							g_pSocket->sendPacket( &_CGRelicToObject );
							g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_DROP_TO_RELICTABLE );
						}
						else
						{
							g_pSystemMessage->Add((*g_pGameStringTable)[UI_STRING_MESSAGE_TOO_FAR].GetString());
						}
						return;
					}
				}
				else	// ???????????????????? ??????
				{
					if(pItem->GetItemClass() == ITEM_CLASS_PET_FOOD &&
						pCreature->GetCreatureType() == 687 && pItem->GetNumber() == 1)	// ??????????????????????????????????
					{
						if(abs(g_pPlayer->GetX()-pCreature->GetX()) < 3 && abs(g_pPlayer->GetY()-pCreature->GetY()) < 3)
						{
							CGTameMonster _CGTameMonster;
							_CGTameMonster.setObjectID(CreatureID);
							g_pSocket->sendPacket( &_CGTameMonster );
							g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_DROP_TO_CREATURE );
						}
						else
						{
							g_pSystemMessage->Add((*g_pGameStringTable)[UI_STRING_MESSAGE_TOO_FAR].GetString());
						}
						return;
					}
					// ?????????? ???????????????? ??????????????????
					else if(pItem->GetItemClass() == ITEM_CLASS_EVENT_ITEM && pItem->GetItemType() == 31 &&
						(pCreature->GetCreatureType() == 793 || pCreature->GetCreatureType() == 794 || pCreature->GetCreatureType() == 795))
					{
						CGRelicToObject _CGRelicToObject;
						_CGRelicToObject.setItemObjectID(pItem->GetID());
						_CGRelicToObject.setObjectID(CreatureID);
						g_pSocket->sendPacket( &_CGRelicToObject );
						g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_DROP_TO_RELICTABLE );
						pCreature->RemoveCauseCriticalWoundsEffect();

					}
				}
			}

		}

		// ????????????????????MItemTable????ItemMoveControl ????????????????
		// ?????????????? ??????????????????????????
		// 2006.12.26 by chyaya


		/*
		if (pItem!=NULL 
			&& !pItem->IsQuestItem()
			&& g_pPlayer->IsItemCheckBufferNULL()
			// ??????????Event GiftBox ???????????? ???????????????????
			&& !(pItem->GetItemClass()==ITEM_CLASS_EVENT_GIFT_BOX&& pItem->GetItemType()==1)
			&& !(pItem->GetItemClass()==ITEM_CLASS_COUPLE_RING)
			&& !(pItem->GetItemClass()==ITEM_CLASS_OUSTERS_HARMONIC_PENDENT) 
			
			&& !(pItem->GetItemClass()==ITEM_CLASS_VAMPIRE_COUPLE_RING 
			&& (pItem->GetItemClass() == ITEM_CLASS_CODE_SHEET && !gC_vs_ui.IsRunningQuestInventory() ) )
			&& !(pItem->GetItemClass() == ITEM_CLASS_LUCKY_BAG && pItem->GetItemType() == 3)
			&& !(pItem->GetItemClass() == ITEM_CLASS_EVENT_ITEM && (pItem->GetItemType() == 28||pItem->GetItemType() == 31))
			

			// sjheon ???? ???????? ?????????????????? ??????????????????? 2005.05.02 Add
			//&& !(pItem->GetItemClass() == ITEM_CLASS_EVENT_ETC && (pItem->GetItemType() == 18))		// ???????????? 
			&& !(pItem->GetItemClass() == ITEM_CLASS_EVENT_ITEM && (pItem->GetItemType() >= 32 && pItem->GetItemType() <= 36))		// ???? ????????
			&& !(pItem->GetItemClass() == ITEM_CLASS_SLAYER_PORTAL_ITEM) 		// ??????????
			// sjheon ???????????? ?????????????????? ??????????????????? 2005.05.02 End 
			// Sjheon 2006.03.08 
			&& !(pItem->GetItemClass() == ITEM_CLASS_COMMON_QUEST_ITEM)  
			&& !(pItem->GetItemClass() == ITEM_CLASS_ETHEREAL_CHAIN) 
			&& !(pItem->GetItemClass() == ITEM_CLASS_CHECK_MONEY) 

			&& !(pItem->GetItemClass() == ITEM_CLASS_EVENT_GIFT_BOX && (pItem->GetItemType() == 27))		//???????????????? ??????
			&& !(pItem->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && (pItem->GetItemType() >= 20 && pItem->GetItemType() <= 23))		// // ??????????????~ ????????????
			)
			*/

		//-----------------------------------------------------------------
		// ???????????? ?????????? ?????
		//-----------------------------------------------------------------
		if(pItem != NULL && !pItem->IsQuestItem() && g_pPlayer->IsItemCheckBufferNULL() &&
		   (
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
		   !pItem->GetCashItem() && 
#endif
		   !pItem->GetItemMoveContorl().GetAttr(ITEMMOVE_CANNOT_DROP) || // ?????????????????????????? ????
		    pItem->GetItemClass() == ITEM_CLASS_CODE_SHEET && !gC_vs_ui.IsRunningQuestInventory())
			) // ???? ???????????? ??????????. -_-;;
		{
			//-----------------------------------------------------------------------
			// ?????????? ???? ?????? ????????????
			//-----------------------------------------------------------------------
			if (g_pTradeManager==NULL)
			{
					//---------------------------------------------------
					// item?????????????????????????? ?????? ?????? ?????.
					//---------------------------------------------------
					//if (g_pPlayer->IsItemCheckBufferNULL())
					{
						//---------------------------------------------------
						// Server????item?????????????????packet??????????????
						// ?????????????????????? ??????.. 
						// server?????? item????zone?????? ?????????????????
						//---------------------------------------------------
						CGAddMouseToZone _CGAddMouseToZone;
						_CGAddMouseToZone.setObjectID( pItem->GetID() );

						g_pSocket->sendPacket( &_CGAddMouseToZone );

						
						//---------------------------------------------------
						// ????????????. ????????AddNewItem???????? ????????????????.
						// timing ??????????????????.
						//---------------------------------------------------
						// mouse?????? item????????????
						//gC_vs_ui.DropItem();

						// buffer????item???????????????? (????????item??????????????)
						g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_DROP_TO_ZONE );

						// item??????????????.
						//delete pItem;
					}

			}
			//-----------------------------------------------------------------------
			// ?????????????? ???????????? ??????????????????
			//-----------------------------------------------------------------------
			else
			{
				DEBUG_ADD("Cannot Drop Item to Zone while Trading.");
			}
		}
		//-----------------------------------------------------------------
		// ?????? ???????? ???? ???????????? ???? ?????
		//-----------------------------------------------------------------
		else
		{
			DEBUG_ADD( "[Error] There is another item in ItemCheckBuffer or Mouse NULL");
		}
	}
	else
	{
		DEBUG_ADD( "Can't Drop");
	}
}


//-----------------------------------------------------------------------------
//
// mouse????Item????Inventory?????????????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_DROP_TO_INVENTORY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_DROP_TO_INVENTORY");
	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	//
	// left = grid_x, right = grid_y
	// void_ptr = MItem *		// ????????item
	//							// bAcceptMyTrade???? inventory????item????.
	//
	MItem* pItem = (MItem*)void_ptr;		// inventory???????? ??????????NULL?????????? ????)
	MItem* pMouseItem = UI_GetMouseItem();	// ?????? mouse????item

	// pMouseItem????pItem????????????..
	// pMouseItem???????? ???????????..
	
	BOOL bAcceptMyTrade = (g_pTradeManager!=NULL && g_pTradeManager->IsAcceptMyTrade());
	BOOL bAcceptOtherTrade = (g_pTradeManager!=NULL && g_pTradeManager->IsAcceptOtherTrade());

	//-----------------------------------------------------------------
	// ?????????????????????? ?????
	//-----------------------------------------------------------------
	if (g_pPlayer->IsItemCheckBufferNULL() && pMouseItem!=NULL
		&& g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{		
		BOOL bSendPacketTradeRemove = FALSE;	// ???????????? ??????
		BOOL bSendPacket = TRUE;				// mouse --> inventory
		BOOL bSendPacketTradeAdd = FALSE;		// ???????? ????????????????
		
		TYPE_OBJECTID removeItemID = OBJECTID_NULL;
		TYPE_OBJECTID toInventoryItemID = (pMouseItem==NULL)? OBJECTID_NULL : pMouseItem->GetID();

		//---------------------------------------------------
		// ???? ??????????????????????????????.
		//---------------------------------------------------
		if (bAcceptMyTrade)
		{
			if (g_pTempInformation->GetMode() == TempInformation::MODE_NULL
				&& g_pPlayer->IsItemCheckBufferNULL())
			{	
				g_pTempInformation->SetMode(TempInformation::MODE_TRADE_VERIFY_DROP_TO_INVENTORY);
				g_pTempInformation->Value1 = pMouseItem->GetID();
				g_pTempInformation->Value2 = left;
				g_pTempInformation->Value3 = right;
				g_pTempInformation->pValue = pMouseItem;

				g_pPlayer->SetItemCheckBuffer( pMouseItem, MPlayer::ITEM_CHECK_BUFFER_DROP_TO_INVENTORY );

				//toInventoryItemID = pMouseItem->GetID();

				//-------------------------------------------------------------
				// inventory???????? item??????????????????????????????????????????.
				//-------------------------------------------------------------
				if (pItem!=NULL && pItem->IsTrade())
				{
					removeItemID = pItem->GetID();				
					
					bSendPacketTradeRemove = TRUE;
				}

				//bSendPacketTradeAdd = TRUE;
			}
			else
			{
				bSendPacket = FALSE;
			}
		}	
		//---------------------------------------------------
		// ?????? ???????????? ?????????????????????????? ?????
		//---------------------------------------------------
		else if (gC_vs_ui.IsRunningExchange())
		{
			//---------------------------------------------------
			// ?????????????? ????????????????????????????????..
			//---------------------------------------------------
			if (pItem!=NULL && pItem->IsTrade())
			{
				removeItemID = pItem->GetID();

				bSendPacketTradeRemove = TRUE;
			}

			//---------------------------------------------------
			// ?????????? ??????????????????
			//---------------------------------------------------
			if (pMouseItem->IsTrade())
			{
				bSendPacketTradeAdd = TRUE;

				// ???? ????????OK?????? ?????????.
				if (bAcceptOtherTrade)
				{
					g_pTradeManager->RefuseOtherTrade();
				}
			}
			//---------------------------------------------------
			// ??????. ???????????? OK????????????????
			//---------------------------------------------------
			else
			{
				/*
				if (bAcceptMyTrade)
				#ifdef CONNECT_SERVER
					CGTradeFinish _CGTradeFinish;
					_CGTradeFinish.setTargetObjectID( g_pTradeManager->GetOtherID() );
					_CGTradeFinish.setCode( CG_TRADE_FINISH_RECONSIDER );

					g_pSocket->sendPacket( &_CGTradeFinish );
				#endif
				*/
			}
		}

		//---------------------------------------------------------
		// SendPacket - Trade Remove Item
		//---------------------------------------------------------
		if (bSendPacketTradeRemove)
		{
				CGTradeRemoveItem _CGTradeRemoveItem;
				_CGTradeRemoveItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
				_CGTradeRemoveItem.setItemObjectID( removeItemID );

				g_pSocket->sendPacket( &_CGTradeRemoveItem );

				g_pTradeManager->SetNextAcceptTime();
		}

		//---------------------------------------------------
		// packet?????????? ???? ???? ?????
		//---------------------------------------------------
		if (bSendPacket)
		{

				//---------------------------------------------------
				// mouse???????? item????Inventory??????????????
				// server????packet??????????????
				// pItem????grid????????inventory??????????????????.
				//---------------------------------------------------
				CGAddMouseToInventory _CGAddMouseToInventory;
				_CGAddMouseToInventory.setObjectID( toInventoryItemID );
				_CGAddMouseToInventory.setInvenX( left );//pItem->GetGridX() );
				_CGAddMouseToInventory.setInvenY( right );//pItem->GetGridY() );

				g_pSocket->sendPacket( &_CGAddMouseToInventory );

		}

		//---------------------------------------------------
		// ?????????? ?????????? ??????????..
		//---------------------------------------------------
		if (bSendPacketTradeAdd)
		{
				CGTradeAddItem _CGTradeAddItem;
				_CGTradeAddItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
				_CGTradeAddItem.setItemObjectID( pMouseItem->GetID() );

				g_pSocket->sendPacket( &_CGTradeAddItem );
				
		}

		//---------------------------------------------------
		// ??????????????????.
		//---------------------------------------------------
		if (!bAcceptMyTrade)
		{
			MItem* pOldItem = NULL;
			if (g_pInventory->ReplaceItem(pMouseItem,		// ????????item
												left, right,	// ???????????? 
												pOldItem))		// ?????????? item
			{
				if (pOldItem != NULL) // replace ?????????
				{				
					UI_PickUpItem( pOldItem );
				}
				else
				{
					UI_DropItem();
				}
			}

			// ??????????????????????????????
			if(pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_TREE 
				&& pMouseItem->GetItemType() != 12
				&& pMouseItem->GetItemType() != 25
				&& pMouseItem->GetItemType() != 26
				&& pMouseItem->GetItemType() != 27
				&& pMouseItem->GetItemType() != 28
				&& pMouseItem->GetItemType() != 41
				&& pMouseItem->GetItemType() != 54	// ??????????????????2006.11.30 by chyaya
#if __CONTENTS(__BLITZ_COUPON_EVENT)
				&& pMouseItem->GetItemType() != 67	// ???????????????? 2008.8.29 by ij-ch
#endif //__BLITZ_COUPON_EVENT
#if __CONTENTS(__2008_AUTUMN_LEAVES_EVENT)
				&& pMouseItem->GetItemType() != 80	// ???? ?????? 2008.9.25 by ij-ch
#endif //__2008_AUTUMN_LEAVES_EVENT
				)
			{
				int mx,my;
				
				int offset = 0;
// ?????????????? ?????????????? ???? ?????? ???? ??????
				int TreeSizeX = 3,TreeSizeY = 4;
				
				if(pMouseItem->GetItemType() < 12)
					offset = 0;
				else if( pMouseItem->GetItemType() < 25 )
					offset = 13; 
				else if( pMouseItem->GetItemType() < 41 )
					offset = 29;
				else if( pMouseItem->GetItemType() < 54 )
					offset = 42;
#if __CONTENTS(__BLITZ_COUPON_EVENT)
				else if( pMouseItem->GetItemType() < 67 )
					offset = 55;
#endif //__BLITZ_COUPON_EVENT
#if __CONTENTS(__2008_AUTUMN_LEAVES_EVENT)
				else if( pMouseItem->GetItemType() < 80 )
					offset = 68;
#endif //__2008_AUTUMN_LEAVES_EVENT
#if __CONTENTS(__CHRISTMAS_SOCKS_EVENT)
				else if( pMouseItem->GetItemType() < 90 )
				{
					offset = 81;
					TreeSizeY = 3;
				}
#endif //__CHRISTMAS_SOCKS_EVENT

				my = (pMouseItem->GetItemType() - offset) / TreeSizeX;
				mx = (pMouseItem->GetItemType() - offset) % TreeSizeX;

				const int pointX = pMouseItem->GetGridX() - mx;
				const int pointY = pMouseItem->GetGridY() - my;
				
				bool bTree = true;

				for(int y = 0; y < TreeSizeY; y++)
				{
					for(int x = 0; x < TreeSizeX; x++)
					{
						MItem *pPartOfTree = g_pInventory->GetItem(x+pointX, y+pointY);
						if(pPartOfTree != NULL && 
							pPartOfTree->GetItemClass() == ITEM_CLASS_EVENT_TREE &&
							pPartOfTree->GetItemType() == y * TreeSizeX + x + offset
#if __CONTENTS(__EVENTTREE_PILE)
							&& pPartOfTree->GetNumber() == 1
#endif //__EVENTTREE_PILE
							)
						{
						}
						else
						{
							bTree = false;
						}
					}
				}

				if(bTree)
					g_pPlayer->SetItemCheckBuffer(NULL, MPlayer::ITEM_CHECK_BUFFER_TREE_MERGE);
			}
		}
	}
	//-----------------------------------------------------------------
	// ?????? ???????? ???? ???????????? ???? ?????
	//-----------------------------------------------------------------
	else
	{
		DEBUG_ADD( "[Error] There is another item in ItemCheckBuffer or Mouse NULL");		

		if(!g_pPlayer->IsItemCheckBufferNULL())
			DEBUG_ADD_FORMAT("[ITEMDROP] !g_pPlayer->IsItemCheckBufferNULL() %d", g_pPlayer->GetItemCheckBufferStatus());
		if(pMouseItem == NULL)
			DEBUG_ADD("[ITEMDROP] pMouseItem == NULL)");
		if(g_pTempInformation->GetMode()!=TempInformation::MODE_NULL)
			DEBUG_ADD_FORMAT("[ITEMPDROP] g_pTempInformation->Mode!=TempInformation::MODE_NULL %d", g_pTempInformation->Mode);
	}

	// ?????????????? code????. *_*;
	//gC_vs_ui.DropItem();
}

//-----------------------------------------------------------------------------
//
// QuickSlot????Item?????????? ?????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_DROP_TO_QUICKSLOT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_DROP_TO_QUICKSLOT");

	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	
	MOustersArmsBand* pQuickSlot = NULL;

	int slot = left;
	
	if( g_pPlayer->IsOusters() )
	{
		if( g_pArmsBand1 == NULL )
			pQuickSlot = g_pArmsBand2;
		else
		{
			if( g_pArmsBand1->GetPocketNumber() > slot )
				pQuickSlot = g_pArmsBand1;
			else				
			{
				pQuickSlot = g_pArmsBand2;
				slot -= g_pArmsBand1->GetPocketNumber();
			}
		}

		if( pQuickSlot == NULL )
			return;
	}

	//
	// left = grid_x
	// void_ptr = MItem *
	//
	//---------------------------------------------------
	// ?????? UI?????????????? ??????????.
	// ???? ??????????????.
	// ????????, ?????????? ???? ??????????. - -;
	// ???????????????????????????????????????????? by larosel
	//---------------------------------------------------
	MItem* pSlotItem = NULL ;// = (MItem*)g_pQuickSlot->GetItem( left );;//gpC_mouse_pointer->GetPickUpItem();
	if( g_pPlayer->IsSlayer() )
		pSlotItem = (MItem*)g_pQuickSlot->GetItem( left );
	else
		pSlotItem = (MItem*)pQuickSlot->GetItem( slot );
	
	MItem* pMouseItem = gpC_mouse_pointer->GetPickUpItem();//g_pQuickSlot->GetItem( left );
	
	bool	Replace = FALSE;
	
	if( g_pPlayer->IsSlayer() )
		Replace = g_pQuickSlot->ReplaceItem( pMouseItem, left, pSlotItem );
	else
		Replace = pQuickSlot->ReplaceItem( pMouseItem, slot, pSlotItem );

	if (Replace)
	{
		if (pSlotItem) // replace ?????????
		{
			gpC_mouse_pointer->PickUpItem((MItem *)pSlotItem);
		}
		else
		{
			// not replace, just drop at empty grid.
			gpC_mouse_pointer->DropItem();
		}
	}
	
	//-----------------------------------------------------------------
	// ?????????????????????? ?????
	//-----------------------------------------------------------------
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)//g_pPlayer->IsItemCheckBufferNULL())
	{
		// ??????????packet??????????????????.
		if (1)//g_pQuickSlot->GetItem( left )==NULL)
		{

				//MItem* pItem = (MItem*)void_ptr;

				//---------------------------------------------------
				// mouse???????? item????QuickSlot??????????????
				// server????packet??????????????
				//---------------------------------------------------
				CGAddMouseToQuickSlot _CGAddMouseToQuickSlot;
				_CGAddMouseToQuickSlot.setObjectID( pMouseItem->GetID() );
				
				if( g_pPlayer->IsSlayer() )
					_CGAddMouseToQuickSlot.setSlotID( left );//pItem->GetItemSlot() );
				else
				{
					if( pQuickSlot == g_pArmsBand2 )
						_CGAddMouseToQuickSlot.setSlotID( slot + 3 );
					else
						_CGAddMouseToQuickSlot.setSlotID( slot );
				}
				
				g_pSocket->sendPacket( &_CGAddMouseToQuickSlot );
		
				//---------------------------------------------------
				// UI?????? ???? ??????????. ???? ??????
				//---------------------------------------------------
				// ???? QuickSlot????????????.
				//g_pQuickSlot->AddItem( pItem, left );
				
				//UI_DropItem();

			PlaySound( pMouseItem->GetInventorySoundID() );
		}
		//---------------------------------------------------
		// ?????? ???? ???? ?????. 
		//---------------------------------------------------
		else
		{						
			DEBUG_ADD_FORMAT("[Error] Can't add item to quickslot, already exist slot=%d", left);
		}
	}
	//-----------------------------------------------------------------
	// ?????? ???????? ???? ???????????? ???? ?????
	//-----------------------------------------------------------------
	else
	{
		DEBUG_ADD( "[Error] There is another item in ItemCheckBuffer");
	}
}


//-----------------------------------------------------------------------------
//
// Gear????Item???????????????????
//
//-----------------------------------------------------------------------------				
void
UIMessageManager::Execute_UI_ITEM_DROP_TO_GEAR(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ADD_ITEM_TO_GEAR");
	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	//
	// left = slot enum
	// void_ptr = MItem * ????????item(NULL???? ????.)
	//

	if (!g_bWatchMode)
	{
		//-----------------------------------------------------------------
		// ?????????????????????? ?????
		//-----------------------------------------------------------------
		if (g_pPlayer->IsItemCheckBufferNULL()
			&& g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
		{
			MItem* pMouseItem = (MItem*)gpC_mouse_pointer->GetPickUpItem();	// ????????? item
			MItem* pGearItem = (MItem*)void_ptr;	// gear??????????. ??????????????????
			
			//-----------------------------------------------------------------
			// mouse?????????????????????????????.
			//-----------------------------------------------------------------
			if (pMouseItem!=NULL)
			{
				//-----------------------------------------------------------------
				// ?????????????????? ????????????..
				//-----------------------------------------------------------------
//20080918 ?????? ???? if(g_pPlayer->SetAddonItem(pMouseItem)) ?????????????? ???????????????????????????? ???? ???? ??????????
//?????? ?????? ???? ?????? ?????????? ???????????? ???????????????????? ??????????????? ???????????????????? ?????? ???? ????????????.
//?????? ???????????????????? ?????? ???????????? ?????? ???? ???? ????????? SetAddonItem()?????????? ?????? ?????????????????????? ?????? ???? ??????
//?????? ??????????GCRealWearingInfoHandler.cpp?????? ????????????????.
//???????????????????? m_bAffectStatus???????????????????????????????????? ???? ?????????????????????????????????? ?????????? ???????????? ???????????????????????? ????.
				//if(g_pPlayer->SetAddonItem(pMouseItem))
				if(pMouseItem->IsQuestItem() || pMouseItem->IsAffectStatus())
				{				
					MItem* pRemovedItem = NULL;

					// mouse --> gear
					switch(g_pPlayer->GetRace())
					{
					case RACE_SLAYER:
						g_pSlayerGear->ReplaceItem( pMouseItem, (MSlayerGear::GEAR_SLAYER)left, pRemovedItem );
						
						//-----------------------------------------------------------
						// belt???????????????????????????? quick item????reset???? ????.
						//-----------------------------------------------------------
						if ((MSlayerGear::GEAR_SLAYER)left == MSlayerGear::GEAR_SLAYER_BELT)
						{
							gC_vs_ui.ResetSlayerQuickItemSize();
						}
						break;

					case RACE_VAMPIRE:
						g_pVampireGear->ReplaceItem( pMouseItem, (MVampireGear::GEAR_VAMPIRE)left, pRemovedItem );
						break;

					case RACE_OUSTERS:
						g_pOustersGear->ReplaceItem( pMouseItem, (MVampireGear::GEAR_VAMPIRE)left, pRemovedItem );
						break;
					}

					UI_DropItem();

					//-----------------------------------------------------------
					// ???????? ?????
					//-----------------------------------------------------------
					if (pRemovedItem!=NULL)
					{
						UI_PickUpItem( pRemovedItem );
					}								

					//----------------------------------------------------
					// Skill ??
					//----------------------------------------------------
					g_pSkillAvailable->SetAvailableSkills();

						//---------------------------------------------------
						// mouse???????? item1????Gear??????????????
						// server????packet??????????????
						//---------------------------------------------------
						CGAddMouseToGear _CGAddMouseToGear;
						_CGAddMouseToGear.setObjectID( pMouseItem->GetID() );
						_CGAddMouseToGear.setSlotID( pMouseItem->GetItemSlot() );//pItem->GetItemSlot() );	

#if __CONTENTS(__GEAR_SWAP_CHANGE)	//_CGAddMouseToGear.setSlotID(right);
						DWORD	dwGearID	= 0;
						switch(g_pPlayer->GetRace())
						{
						case RACE_SLAYER:
							dwGearID	= g_pSlayerGear->GetAccepGearID();
							break;

						case RACE_VAMPIRE:
							dwGearID	= g_pVampireGear->GetAccepGearID();
							break;
				
						case RACE_OUSTERS:
							dwGearID	= g_pOustersGear->GetAccepGearID();
							break;
						}
						_CGAddMouseToGear.setGearSlotID(dwGearID);
#endif	// __GEAR_SWAP_CHANGE

						g_pSocket->sendPacket( &_CGAddMouseToGear );


					//----------------------------------------------------
					// ???????????????????? ?????? ??????
					//----------------------------------------------------
//					__BEGIN_HELP_EVENT
//						ITEM_CLASS itemClass = pMouseItem->GetItemClass();
//
//						//----------------------------------------------------
//						// ????
//						//----------------------------------------------------
//						if (pMouseItem->IsGunItem())
//						{
//							// [??????] ???????????????????
////							ExecuteHelpEvent( HE_ITEM_WEAR_GUN );
//						}
//						//----------------------------------------------------
//						// ????
//						//----------------------------------------------------
//						else if (itemClass==ITEM_CLASS_BELT)
//						{
//							// [??????] ???? ???????????????
////							ExecuteHelpEvent( HE_ITEM_WEAR_BELT );
//						}
//						//----------------------------------------------------
//						// ????
//						//----------------------------------------------------
//						else if (itemClass==ITEM_CLASS_SWORD)
//						{
//							// [??????] ???????????????????
////							ExecuteHelpEvent( HE_ITEM_WEAR_SWORD );
//						}
//						//----------------------------------------------------
//						// ????
//						//----------------------------------------------------
//						else if (itemClass==ITEM_CLASS_BLADE)
//						{
//							// [??????] ???????????????????
////							ExecuteHelpEvent( HE_ITEM_WEAR_BLADE );
//						}
//						//----------------------------------------------------
//						// ????????
//						//----------------------------------------------------
//						else if (itemClass==ITEM_CLASS_CROSS)
//						{
//							// [??????] ???????????????????????
////							ExecuteHelpEvent( HE_ITEM_WEAR_CROSS );
//						}
//						//----------------------------------------------------
//						// ????????
//						//----------------------------------------------------
//						else if (itemClass==ITEM_CLASS_MACE)
//						{
//							// [??????] ???????????????????????
////							ExecuteHelpEvent( HE_ITEM_WEAR_MACE );
//						}				
//					__END_HELP_EVENT
				}		
			}
			else
			{
				DEBUG_ADD( "[Error] Mouse Item is NULL" );
			}
		}
		//-----------------------------------------------------------------
		// ?????? ???????? ???? ???????????? ???? ?????
		//-----------------------------------------------------------------
		else
		{
			DEBUG_ADD( "[Error] There is another item in ItemCheckBuffer");
		}
	}
	else
	{
		DEBUG_ADD( "[Watch Mode]");
	}

	//-------------------------------------------------
	// Player?????????????????? ??????????????????..
	//-------------------------------------------------
	//if (g_pPlayer->IsStop())
	{
		//-----------------------------------------
		// player????pItem??????????????.
		//-----------------------------------------
		//g_pPlayer->SetAddonItem( pItem );

		//-----------------------------------------
		// UI?????? Item????Gear??????????????.
		//-----------------------------------------
		//gC_vs_ui.DropItem();
	}
	//else
	{
	//	(*g_pSlayerGear).AddItem( pItem, (enum MSlayerGear::GEAR_SLAYER)left );
	}
}


//-----------------------------------------------------------------------------
//
// QuickSlot???????? item????mouse?????????? ?????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_PICKUP_FROM_QUICKSLOT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_PICKUP_FROM_QUICKSLOT");
	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}

	//
	// left = slot enum
	// void_ptr = MItem *
	//
	MOustersArmsBand* pQuickSlot = NULL;
		
	if( g_pPlayer->IsOusters() )
	{
		if( g_pArmsBand1 == NULL )
			pQuickSlot = g_pArmsBand2;
		else
		{
			if( g_pArmsBand1->GetPocketNumber() > left )
				pQuickSlot = g_pArmsBand1;
			else
				pQuickSlot = g_pArmsBand2;
		}
		
		if( pQuickSlot == NULL )
			return;
	}
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		MItem* pItem = (MItem*)void_ptr;

			//---------------------------------------------------
			// QuickSlot???????? item????mouse????????????-_-;)????
			// server????packet??????????????
			//---------------------------------------------------
			CGAddQuickSlotToMouse _CGAddQuickSlotToMouse;
			_CGAddQuickSlotToMouse.setObjectID( pItem->GetID() );
			if( pQuickSlot != NULL)
			{
				if( pQuickSlot == g_pArmsBand2 )
					_CGAddQuickSlotToMouse.setSlotID( pItem->GetItemSlot() + 3 );
				else
					_CGAddQuickSlotToMouse.setSlotID( pItem->GetItemSlot() );
			} else
				_CGAddQuickSlotToMouse.setSlotID( pItem->GetItemSlot() );
			
			g_pSocket->sendPacket( &_CGAddQuickSlotToMouse );
			

		// ???? QuickSlot?????? ?????????? mouse?????????????.
		
		if( g_pPlayer->IsSlayer() )
			g_pQuickSlot->RemoveItem( pItem->GetID() );
		else
		{
			if( pQuickSlot != NULL )
				pQuickSlot->RemoveItem( pItem->GetID() );
		}

		UI_PickUpItem(pItem);
	}
}


//-----------------------------------------------------------------------------
//
// Inventory???????? item????mouse?????????? ?????
//
//-----------------------------------------------------------------------------				
void
UIMessageManager::Execute_UI_ITEM_PICKUP_FROM_INVENTORY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_PICKUP_FROM_INVENTORY"); 
	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	//
	// left = grid x
	// right = grid y
	// void_ptr = MItem *
	//
	//if (g_pTempInformation->Mode==TempInformation::MODE_NULL)
	MItem* pItem = (MItem*)void_ptr;
	{
		BOOL bSendPacket = TRUE;
		
		BOOL bSendPacketTradeRemove = FALSE;

		BOOL bAcceptMyTrade = (g_pTradeManager!=NULL && g_pTradeManager->IsAcceptMyTrade());
		BOOL bAcceptOtherTrade = (g_pTradeManager!=NULL && g_pTradeManager->IsAcceptOtherTrade());

		//---------------------------------------------------------
		// ???????? OK ????????????????????????????????????.
		//---------------------------------------------------------
		if (bAcceptMyTrade)
		{
			if (g_pTempInformation->GetMode() == TempInformation::MODE_NULL
				&& g_pPlayer->IsItemCheckBufferNULL())
			{	
				g_pTempInformation->SetMode(TempInformation::MODE_TRADE_VERIFY_PICKUP_FROM_INVENTORY);
				g_pTempInformation->Value1 = pItem->GetID();
				g_pTempInformation->Value2 = left;
				g_pTempInformation->Value3 = right;
				g_pTempInformation->pValue = pItem;

				g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_PICKUP_FROM_INVENTORY );

				if (pItem->IsTrade())
				{
					bSendPacketTradeRemove = TRUE;
				}
			}
			else
			{
				bSendPacket = FALSE;
			}
		}
		//---------------------------------------------------------
		// ??????????????????item????????????
		//---------------------------------------------------------
		else if (gC_vs_ui.IsRunningExchange())
		{
			//---------------------------------------------------
			// ???????? ???? ??????????????. ??????
			//---------------------------------------------------
			if (pItem->IsTrade())
			{
				UI_PickUpItem( pItem );

				bSendPacketTradeRemove = TRUE;
				
				// ???? ????????OK?????? ?????????.
				if (bAcceptOtherTrade)
				{
					g_pTradeManager->RefuseOtherTrade();
				}
			}
			//---------------------------------------------------
			// ??????. ???????????? OK????????????????
			//---------------------------------------------------
			else
			{
				UI_PickUpItem( pItem );

				/*
				if (bAcceptMyTrade)
				{
					#ifdef CONNECT_SERVER
						CGTradeFinish _CGTradeFinish;
						_CGTradeFinish.setTargetObjectID( g_pTradeManager->GetOtherID() );
						_CGTradeFinish.setCode( CG_TRADE_FINISH_RECONSIDER );

						g_pSocket->sendPacket( &_CGTradeFinish );					
					#endif
				}
				*/
			}
		}
		else
		{
			UI_PickUpItem( pItem );
		}

		//---------------------------------------------------------
		// SendPacket - Trade Remove Item
		//---------------------------------------------------------
		if (bSendPacketTradeRemove)
		{
				CGTradeRemoveItem _CGTradeRemoveItem;
				_CGTradeRemoveItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
				_CGTradeRemoveItem.setItemObjectID( pItem->GetID() );

				g_pSocket->sendPacket( &_CGTradeRemoveItem );

				g_pTradeManager->SetNextAcceptTime();
		}

		//---------------------------------------------------------
		// SendPacket - Inventory to Mouse
		//---------------------------------------------------------
		if (bSendPacket)
		{
				//---------------------------------------------------
				// Inventory???????? item????mouse????????????-_-;)????
				// server????packet??????????????
				//---------------------------------------------------
				CGAddInventoryToMouse _CGAddInventoryToMouse;
				_CGAddInventoryToMouse.setObjectID( pItem->GetID() );
				_CGAddInventoryToMouse.setX( pItem->GetGridX() );
				_CGAddInventoryToMouse.setY( pItem->GetGridY() );
				
				g_pSocket->sendPacket( &_CGAddInventoryToMouse );				
		}
	}

#if __CONTENTS(__GEAR_SWAP_CHANGE)	//UIMessageManager::Execute_UI_ITEM_PICKUP_FROM_INVENTORY
	if(pItem != NULL)
	{
		switch(g_pPlayer->GetRace())
		{
			case RACE_SLAYER:
				if(!g_pSlayerGear->GetActive())
					pItem->UnSetAffectStatus();
				break;
			case RACE_VAMPIRE:
				if(!g_pVampireGear->GetActive())
					pItem->UnSetAffectStatus();
				break;
			case RACE_OUSTERS:
				if(!g_pOustersGear->GetActive())
					pItem->UnSetAffectStatus();
				break;
		}
	}
#endif // __GEAR_SWAP_CHANGE
}


//-----------------------------------------------------------------------------
//
// Gear?????????????? ???? item????mouse?????????? ?????
//
//-----------------------------------------------------------------------------				
void
UIMessageManager::Execute_UI_ITEM_PICKUP_FROM_GEAR(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_REMOVE_ITEM_FROM_GEAR");
	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	//
	// left = slot enum
	// void_ptr = MItem *
	//

	MItem* pItem = (MItem*)void_ptr;	// gear???????? item
	if (!g_bWatchMode)
	{
		int itemSlot = left;

		if (pItem!=NULL)
		{
			//-------------------------------------------------
			// Player?????????????????? ????????
			//-------------------------------------------------
			//if (g_pPlayer->IsStop())
			{
				//---------------------------------------------
				// AddonItem?????????? ??????????.
				//---------------------------------------------
				if (g_pPlayer->RemoveAddonItem( pItem ))
				{
					MItem* pRemovedItem = NULL;

					//---------------------------------------------
					// gear?????? ??????????.
					//---------------------------------------------
					switch(g_pPlayer->GetRace())
					{
					case RACE_SLAYER:
						pRemovedItem = g_pSlayerGear->RemoveItem( (MSlayerGear::GEAR_SLAYER)itemSlot );
						
						//-----------------------------------------------------------
						// belt???????????????????????????? quick item????reset???? ????.
						//-----------------------------------------------------------
						if (pRemovedItem!=NULL 
							&& pRemovedItem->GetItemSlot() == MSlayerGear::GEAR_SLAYER_BELT)
						{
							gC_vs_ui.ResetSlayerQuickItemSize();
						}					
						break;

					case RACE_VAMPIRE:
						pRemovedItem = g_pVampireGear->RemoveItem( (MVampireGear::GEAR_VAMPIRE)itemSlot );
						break;

					case RACE_OUSTERS:
						pRemovedItem = g_pOustersGear->RemoveItem( (MOustersGear::GEAR_OUSTERS)itemSlot );
						break;
					}


					//---------------------------------------------
					// UI?????? Gear????Item????pickup???? ????.
					//---------------------------------------------
					if (pRemovedItem!=NULL)
					{
						UI_PickUpItem( pRemovedItem );
						
							//---------------------------------------------------
							// Inventory???????? item????mouse????????????-_-;)????
							// server????packet??????????????
							//---------------------------------------------------
							CGAddGearToMouse _CGAddGearToMouse;
							_CGAddGearToMouse.setObjectID( pRemovedItem->GetID() );
							_CGAddGearToMouse.setSlotID( pRemovedItem->GetItemSlot() );
							
							g_pSocket->sendPacket( &_CGAddGearToMouse );							
					}

					//----------------------------------------------------
					// Skill ??
					//----------------------------------------------------
					g_pSkillAvailable->SetAvailableSkills();
				}			
			}
			//else
			{
			//	(*g_pSlayerGear).AddItem( pItem, (enum MSlayerGear::GEAR_SLAYER)left );
			}
		}
	}
	else
	{
		DEBUG_ADD( "[Watch Mode]");		
	}

#if __CONTENTS(__GEAR_SWAP_CHANGE)			// UIMessageManager::Execute_UI_ITEM_PICKUP_FROM_GEAR
	if(pItem != NULL)
	{
		pItem->SetAffectStatus();
		switch(g_pPlayer->GetRace())
		{
			case RACE_SLAYER:
				if(!g_pSlayerGear->GetActive())
					pItem->UnSetAffectStatus();
				break;
			case RACE_VAMPIRE:
				if(!g_pVampireGear->GetActive())
					pItem->UnSetAffectStatus();
				break;
			case RACE_OUSTERS:
				if(!g_pOustersGear->GetActive())
					pItem->UnSetAffectStatus();
				break;
		}
	}
#endif		// __CONTENTS
}

//-----------------------------------------------------------------------------
//
// Inventory?????? Item?????????????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_USE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_USE");
	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	//
	// void_ptr = MItem *
	//
	MItem* pItem = g_pInventory->GetItemToModify( left );//(MItem*)void_ptr;

	//BYTE PlayerAction = g_pPlayer->GetAction();
	if ( /*( PlayerAction == ACTION_STAND ||	// ???????????????????????? ???? ?????? ?????? ?????????????????????
		   PlayerAction == ACTION_MOVE ) &&*/
		 g_pPlayer->IsItemCheckBufferNULL() )
	{
		// ?????????? ?????????? ??????-????;
		if(	g_pPlayer->IsSlayer() && pItem->IsSlayerItem() ||
			g_pPlayer->IsVampire() && pItem->IsVampireItem() ||
			g_pPlayer->IsOusters() && pItem->IsOustersItem())
		{
			pItem->UseInventory();
		}
	}
	//----------------------------------------------------
	// item check buffer not null
	//----------------------------------------------------
	else
	{
		#ifdef OUTPUT_DEBUG
			if (pItem==NULL)
			{
				DEBUG_ADD("[Wait] Item is NULL");
			}
			else 
			{
				DEBUG_ADD("[Wait] Item Check Buffer is Not NULL");
			}
		#endif
	}
}

//-----------------------------------------------------------------------------
//
// QuickSlot?????? Item?????????????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_USE_QUICKSLOT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_USE_QUICKSLOT");

	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	//
	// left = slot
	// right = by FunctionKey
	//
	//MItem* pItem = (MItem*)void_ptr;
	if (g_pPlayer!=NULL && (g_pQuickSlot!=NULL && g_pPlayer->IsSlayer() || 
		g_pPlayer->IsOusters() && (g_pArmsBand1 != NULL || g_pArmsBand2 != NULL) ) )
	{
		int bByFunctionKey = right;	// by FunctionKey
		MSlotItemManager *pQuickSlot = g_pQuickSlot;

		int slot = left;
//		const MItem* pSlotItem = g_pQuickSlot->GetItem( left );
		MItem * pSlotItem = NULL;
		
		if( g_pPlayer->IsOusters() )
		{
			if( g_pArmsBand1 == NULL )
				pQuickSlot = g_pArmsBand2;
			else
			{
				if( g_pArmsBand1->GetPocketNumber() > slot )
					pQuickSlot = g_pArmsBand1;
				else
				{
					pQuickSlot = g_pArmsBand2;
					slot -= g_pArmsBand1->GetPocketNumber();
				}
			}
		}

		if( pQuickSlot == NULL )
			return;

		pSlotItem = pQuickSlot->GetItem( slot );

		if (pSlotItem!=NULL)
		{
			MItem *pItem = NULL;
			
			pItem = pQuickSlot->GetItemToModify( pSlotItem->GetID() );

			if (g_pPlayer->IsItemCheckBufferNULL())
			{
//				__BEGIN_HELP_EVENT
//					if (bByFunctionKey)
//					{
//						// [??????] functionKey??????????????????????????
////						ExecuteHelpEvent( HE_PRESSED_FUNCTION_KEY_FOR_QUICKITEM );
//					}
//				__END_HELP_EVENT

				// ?????????? ?????????? ??????-????;
				if(g_pPlayer->IsSlayer() && pItem->IsSlayerItem() ||
					g_pPlayer->IsVampire() && pItem->IsVampireItem() ||
					g_pPlayer->IsOusters() && pItem->IsOustersItem())
				{
					pItem->UseQuickItem();
				}
			}
			//----------------------------------------------------
			// item check buffer not null
			//----------------------------------------------------
			else
			{
				DEBUG_ADD("[Wait] Item Check Buffer is Not NULL");
			}
		}
		else
		{
			DEBUG_ADD_FORMAT("[Error] There is no item in slot=%d", left);
		}
	}
	else
	{
		DEBUG_ADD("[Error] There is no QuickSlot");
	}

}

//-----------------------------------------------------------------------------
//
// mouse????Item????Inventory????Item????????????????
//
//-----------------------------------------------------------------------------
// ???????????????????????????? ???? ????????.
// max ?????????????????? ??????????????????
// ??????. ????????? ?????? ????????????.
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_INSERT_FROM_INVENTORY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_INSERT_FROM_INVENTORY");

	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}

	// gpC_mouse_pointer->GetPickUpItem()		// ????????? ????
	//void_ptr = MItem *		// ?????????
	// Inventory????(left,right)
	MItem* pItem = (MItem*)void_ptr;
	MItem* pMouseItem = gpC_mouse_pointer->GetPickUpItem();

	if (pMouseItem==NULL)
	{
		DEBUG_ADD("[Error] Mouse Item is NULL");
		return;
	}

	if (pItem==NULL)
	{
		DEBUG_ADD("[Error] InvenItem is NULL");
		return;
	}

	TYPE_OBJECTID mouseItemID = ((pMouseItem==NULL)?OBJECTID_NULL : pMouseItem->GetID());

	if (pItem->IsPileItem())
	{
		if (pItem->GetItemClass()==pMouseItem->GetItemClass()
			&& pItem->GetItemType()==pMouseItem->GetItemType()
			&& pItem->GetNumber() < pItem->GetMaxNumber())
		{
			BOOL bSendPacket = TRUE;

			BOOL bAcceptMyTrade = (g_pTradeManager!=NULL && g_pTradeManager->IsAcceptMyTrade());
			BOOL bAcceptOtherTrade = (g_pTradeManager!=NULL && g_pTradeManager->IsAcceptOtherTrade());

			//---------------------------------------------------------
			// ???????? OK ????????????????????????????????????.
			//---------------------------------------------------------
			if (bAcceptMyTrade)
			{	
				// ??????????????????UI?????? ???? ?????? ??????
				// pItem????????????? item????.
				if (g_pTempInformation->GetMode() == TempInformation::MODE_NULL
					&& g_pPlayer->IsItemCheckBufferNULL())
				{	
					g_pTempInformation->SetMode(TempInformation::MODE_TRADE_VERIFY_INSERT_TO_INVENTORY);
					g_pTempInformation->Value1 = pItem->GetID();
					g_pTempInformation->Value2 = left;
					g_pTempInformation->Value3 = right;
					g_pTempInformation->pValue = pItem;

					g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_INSERT_FROM_INVENTORY );
				}
				else
				{
					bSendPacket = FALSE;
				}
			}
			else
			{
				//g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_INSERT_FROM_INVENTORY );
				MItem* pMouseItem = gpC_mouse_pointer->GetPickUpItem();

				//----------------------------------------------------
				// pMouseItem????pItem????????????????
				//----------------------------------------------------
				int total = pMouseItem->GetNumber() + pItem->GetNumber();
				if ( total > pItem->GetMaxNumber() )
				{
					// ???? ???????????? ?????
					pMouseItem->SetNumber( total - pItem->GetMaxNumber() );
					pItem->SetNumber( pItem->GetMaxNumber() );
				}
				else
				{
					// ?????pItem???????????????????? ?????
					pItem->SetNumber( total );
					UI_DropItem();

					delete pMouseItem;
				}

				//----------------------------------------------------
				// ???????? ???????????????????? ?????
				//----------------------------------------------------
				// Remove and Add - -;
				if (gC_vs_ui.IsRunningExchange() && pItem->IsTrade())
				{
						//----------------------------------------------------
						// ?????? ??????????????
						//----------------------------------------------------
						CGTradeRemoveItem _CGTradeRemoveItem;
						_CGTradeRemoveItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
						_CGTradeRemoveItem.setItemObjectID( pItem->GetID() );

						g_pSocket->sendPacket( &_CGTradeRemoveItem );

						g_pTradeManager->SetNextAcceptTime();

						
						//----------------------------------------------------
						// ?????????????? --> ?????? ??????
						//----------------------------------------------------
						CGAddMouseToInventory _CGAddMouseToInventory;
						_CGAddMouseToInventory.setObjectID( mouseItemID );
						_CGAddMouseToInventory.setInvenX( pItem->GetGridX() );
						_CGAddMouseToInventory.setInvenY( pItem->GetGridY() );								

						g_pSocket->sendPacket( &_CGAddMouseToInventory );

						bSendPacket = FALSE;

						
						//----------------------------------------------------
						// ???? ???????? ??????????????
						//----------------------------------------------------
						CGTradeAddItem _CGTradeAddItem;
						_CGTradeAddItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
						_CGTradeAddItem.setItemObjectID( pItem->GetID() );

						g_pSocket->sendPacket( &_CGTradeAddItem );
						

					// ???? ????????OK?????? ?????????.
					if (bAcceptOtherTrade) 
					{
						g_pTradeManager->RefuseOtherTrade();
					}
				}

				PlaySound( pItem->GetInventorySoundID() );
			}

			//----------------------------------------------------
			// Server???????????????????
			//----------------------------------------------------
				if (bSendPacket)
				{
					CGAddMouseToInventory _CGAddMouseToInventory;
					_CGAddMouseToInventory.setObjectID( mouseItemID );
					_CGAddMouseToInventory.setInvenX( pItem->GetGridX() );
					_CGAddMouseToInventory.setInvenY( pItem->GetGridY() );								

					g_pSocket->sendPacket( &_CGAddMouseToInventory );

					
					//----------------------------------------------------
					// ????Item class?????????? ????
					//----------------------------------------------------
					//if (pItem->GetItemClass()==ITEM_CLASS_MONEY)
					//{
					//}
				}
				
		}
	}
}

//-----------------------------------------------------------------------------
//
// mouse????Item????Gear????Item????????????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_INSERT_FROM_GEAR(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_INSERT_FROM_GEAR");

	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}

	// gpC_mouse_pointer->GetPickUpItem()		// ????????? ????
	//void_ptr = MItem *		// ?????????
	// Gear Slot(left)

	if (!g_bWatchMode
		&& g_pPlayer->IsItemCheckBufferNULL())
	{
		MItem* pItem = (MItem*)void_ptr;
		MItem* pMouseItem = gpC_mouse_pointer->GetPickUpItem();

		//----------------------------------------------------
		// Magazine
		//----------------------------------------------------
		if (pItem->GetItemClass()==ITEM_CLASS_MAGAZINE)
		{
			//----------------------------------------------------
			// Server???????????????????
			//----------------------------------------------------
				//---------------------------------------------------
				// mouse???????? item????Gear??????????????
				// server????packet??????????????
				//---------------------------------------------------
				CGAddMouseToGear _CGAddMouseToGear;
				_CGAddMouseToGear.setObjectID( pMouseItem->GetID() );
				_CGAddMouseToGear.setSlotID( pItem->GetItemSlot() );

#if __CONTENTS(__GEAR_SWAP_CHANGE)		//_CGAddMouseToGear.setSlotID(right);
				DWORD	dwGearID	= 0;
				switch(g_pPlayer->GetRace())
				{
				case RACE_SLAYER:
					dwGearID	= g_pSlayerGear->GetSendGearID();
					break;

				case RACE_VAMPIRE:
					dwGearID	= g_pVampireGear->GetSendGearID();
					break;
				
				case RACE_OUSTERS:
					dwGearID	= g_pOustersGear->GetSendGearID();
					break;
				}
				_CGAddMouseToGear.setGearSlotID(dwGearID);
#endif	//__GEAR_SWAP_CHANGE			

				g_pSocket->sendPacket( &_CGAddMouseToGear );

				
				// ?????????? ?????????????
				g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_INSERT_FROM_GEAR );

				// ????????????????m_p_current_pickup_item????pItem????????????????.
		}
		//----------------------------------------------------
		// ???? ?????
		//----------------------------------------------------
		else
		{
			// ??????????. - -;
		}
	}
	else
	{
		DEBUG_ADD( "[Watch Mode]");
	}
}


//-----------------------------------------------------------------------------
//
// mouse????Item????QuickSlot????Item????????????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_INSERT_FROM_QUICKSLOT(int left, int right, void* void_ptr)
{				
	DEBUG_ADD("[UI] UI_ITEM_INSERT_FROM_QUICKSLOT");
	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}

	
	if (g_pPlayer->IsItemCheckBufferNULL())
	{
		// gpC_mouse_pointer->GetPickUpItem()		// ????????? ????
		//void_ptr = MItem *		// ?????????
		// QuickSlot????(left)
		MItem* pItem = (MItem*)void_ptr;
		MItem* pMouseItem = (MItem*)gpC_mouse_pointer->GetPickUpItem();

		if (pItem->GetItemClass()==pMouseItem->GetItemClass()
			&& pItem->GetItemType()==pMouseItem->GetItemType())
		{
			//----------------------------------------------------
			// Server???????????????????
			//----------------------------------------------------
				CGAddMouseToQuickSlot _CGAddMouseToQuickSlot;
				_CGAddMouseToQuickSlot.setObjectID( pMouseItem->GetID() );
				if ( g_pPlayer->IsOusters() )
				{
					int slotID = left;
					
					if( g_pArmsBand1 != NULL &&g_pArmsBand1->GetPocketNumber() > slotID )
						_CGAddMouseToQuickSlot.setSlotID( slotID );
					else
					{
						_CGAddMouseToQuickSlot.setSlotID( pItem->GetItemSlot() + 3);
					}
				}
				else
					_CGAddMouseToQuickSlot.setSlotID( pItem->GetItemSlot() );

				g_pSocket->sendPacket( &_CGAddMouseToQuickSlot );

				
				// ?????????? ?????????????????????.
				// ????????????????m_p_current_pickup_item????pItem????????????????.
				//g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_INSERT_FROM_INVENTORY );
				//MItem* pMouseItem = (MItem*)gpC_mouse_pointer->GetPickUpItem();

				//----------------------------------------------------
				// pMouseItem????pItem????????????????
				//----------------------------------------------------
				int total = pMouseItem->GetNumber() + pItem->GetNumber();
				if ( total > pItem->GetMaxNumber() )
				{
					// ???? ???????????? ?????
					pMouseItem->SetNumber( total - pItem->GetMaxNumber() );
					pItem->SetNumber( pItem->GetMaxNumber() );
				}
				else
				{
					// ?????pItem???????????????????? ?????
					pItem->SetNumber( total );
					UI_DropItem();

					delete pMouseItem;
				}

				PlaySound( pItem->GetInventorySoundID() );

				//----------------------------------------------------
				// ????Item class?????????? ????
				//----------------------------------------------------
				//if (pItem->GetItemClass()==ITEM_CLASS_MONEY)
				//{
				//}
				
		}
	}
}


//-----------------------------------------------------------------------------
//
// ?????? ?????? ????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLOSE_SHOP(int left, int right, void* void_ptr) 
{
	DEBUG_ADD("[UI] UI_CLOSE_SHOP");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseShop();

		// shop????????????
		gC_vs_ui.SetShop( NULL );


		// ???? ???????????????????????????? ????.
		g_pUIDialog->ShowPCTalkDlg();
	}
}

//-----------------------------------------------------------------------------
//
// ???????????? ?????????????????????????????? ????
//
//-----------------------------------------------------------------------------
//
// void_ptr = MShop
// left = slot
// right = item count
//
void
UIMessageManager::Execute_UI_BUY_ITEM(int left, int right, void* void_ptr)
{				
	DEBUG_ADD("[UI] UI_BUY_ITEM");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	// ?????????? ?????? ?????
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		int index = left;					// ???????????????????? ????
		int number = right;					// ??????????????
		MShop* pShop = (MShop*)void_ptr;	// ??????
		int npcID = (*g_pPCTalkBox).GetNPCID();					

		if (pShop!=NULL)
		{					
			MShopShelf* pShopShelf = pShop->GetCurrentShelf();
			
			if (pShopShelf!=NULL)
			{
				MItem* pItem = pShopShelf->GetItem( index );
				
				if (pItem!=NULL)
				{
					POINT point;
					
					int oldNumber = pItem->GetNumber();					

					//-------------------------------------------------
					// ??????????????????????.
					//-------------------------------------------------							
					bool bBuyPossible = false;
					//GAME_STRINGID buyImpossibleMessage = STRING_MESSAGE_CANNOT_BUY;
					int buyImpossibleMessage = STRING_MESSAGE_CANNOT_BUY;

					switch (pShop->GetShopType())
					{
						//-------------------------------------------------
						// ?????? ??????
						//-------------------------------------------------
						case MShop::SHOP_NORMAL :
						{
							int price = number * (*g_pPriceManager).GetItemPrice(pItem, MPriceManager::NPC_TO_PC, pShopShelf->GetShelfType()==MShopShelf::SHELF_UNKNOWN);
							int money = (*g_pMoneyManager).GetMoney();

							bBuyPossible = (price <= money);

							buyImpossibleMessage = STRING_MESSAGE_CANNOT_BUY_NO_MONEY;
						}
						break;

						//-------------------------------------------------
						// ???????? ??????
						//-------------------------------------------------
						case MShop::SHOP_EVENT_STAR :
						{
							STAR_ITEM_PRICE starPrice;

							g_pPriceManager->GetItemPrice( pItem, starPrice );

							if (starPrice.type!=-1 && starPrice.number!=0)
							{
								// ?????? ????????????????.
								MItemClassTypeNumberFinder starFinder(ITEM_CLASS_EVENT_STAR, 
																		starPrice.type);


								((MItemManager*)g_pInventory)->FindItem( starFinder );

								// ????????????????????????????????.
								bBuyPossible = (starFinder.GetTotalNumber() >= starPrice.number);

								buyImpossibleMessage = STRING_MESSAGE_CANNOT_BUY_NO_STAR;
							}							
							
						}
						break;
					}

					//-------------------------------------------------
					// ?????? ?????????????
					//-------------------------------------------------
					if (bBuyPossible)
					{								
						//-------------------------------------------------
						// inventory?????????? ??????????????
						//-------------------------------------------------
						pItem->SetNumber( number );

						if (g_pInventory->GetFitPosition(pItem, point))
						{								
								CGShopRequestBuy _CGShopRequestBuy;
								_CGShopRequestBuy.setObjectID( npcID );

								_CGShopRequestBuy.setShopType( pShop->GetCurrent() );
								_CGShopRequestBuy.setShopIndex( index );

								_CGShopRequestBuy.setX(point.x);
								_CGShopRequestBuy.setY(point.y);

								_CGShopRequestBuy.setItemNum( number );

								g_pSocket->sendPacket( &_CGShopRequestBuy );

								
								//-------------------------------------------------
								// ???????????????? Temp Information??????
								//-------------------------------------------------
								(*g_pTempInformation).Mode	= TempInformation::MODE_SHOP_BUY;
								(*g_pTempInformation).Value1 = pShop->GetCurrent();
								(*g_pTempInformation).Value2 = index;
								(*g_pTempInformation).Value3 = point.x;
								(*g_pTempInformation).Value4 = point.y;
								(*g_pTempInformation).pValue = (void*)pShop;

								//-------------------------------------------------
								// ???? ?????????? ?????? ??????????..
								//-------------------------------------------------
								UI_LockItemTrade();
						}
						else
						{
							// inventory???????????????????????
							g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_BUY_NO_SPACE ].GetString());
							
							// 2004, 5, 7 , sobeit add start - ?????????????????????????????????????????? ?????? ??????????
							ExecuteHelpEvent( HELP_EVENT_STORAGE_BUY );
							// 2004, 5, 6, sobeit add end
						}

						pItem->SetNumber( oldNumber );

					}
					//-------------------------------------------------
					// ???????? ??????.. ???????????????
					//-------------------------------------------------					
					else
					{
						g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[buyImpossibleMessage].GetString());
					}
				}
				else
				{
					// ??????????index ???????????????????? ?????? ?????
					DEBUG_ADD_FORMAT("[Error] There is NO Item in index=%d", index);
				}
			}
			else
			{
				// ???????????????? ?????????????
				DEBUG_ADD_FORMAT("[Error] There is NO Shelf type=%d", (int)pShop->GetCurrent());
			}
		}
		else
		{
			// shop?????????? ?????????????
			DEBUG_ADD_FORMAT("[Error] There is NO Shop. npc id=%d", npcID);
		}
	}
}

//-----------------------------------------------------------------------------
//
// ?????? ???? dialog ?????? ????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_SELL_FINISHED(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_SELL_FINISHED");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	if (g_pTempInformation!=NULL
		&& g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.FinishItemSelling();

		// ???? ???????????????????????????? ????.
		g_pUIDialog->ShowPCTalkDlg();
	}
}

//-----------------------------------------------------------------------------
//
// ?????????????????????????????? ????
//
//-----------------------------------------------------------------------------
//
// gridX   //void_ptr = Mitem*
// gridY
//
void
UIMessageManager::Execute_UI_SELL_ITEM(int left, int right, void* void_ptr)
{				
	DEBUG_ADD("[UI] UI_SELL_ITEM");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	//MItem* pItem = (MItem*)void_ptr;
	// ?????????? ?????? ?????
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		const MItem* pItem = g_pInventory->GetItem( left, right );

		if (pItem!=NULL && g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
		{
			// Event GiftBox ???????????? ?????????????????
			
			if (//pItem->GetItemClass()!=ITEM_CLASS_EVENT_GIFT_BOX &&
				!pItem->IsUniqueItem() || (g_pTimeItemManager->IsExist( pItem->GetID())))
			{
				//-----------------------------------------------------
				// ???????????? ???? ?????? packet??????????????
				//-----------------------------------------------------
					CGShopRequestSell	_CGShopRequestSell;

					_CGShopRequestSell.setObjectID( (*g_pPCTalkBox).GetNPCID() );		// NPC ID	
					_CGShopRequestSell.setOpCode( SHOP_REQUEST_SELL_NORMAL );
					_CGShopRequestSell.setItemObjectID( pItem->GetID() );	// Item Object ID

					g_pSocket->sendPacket( &_CGShopRequestSell );
			
						
					//-------------------------------------------------
					// ???????????????? Temp Information??????
					//-------------------------------------------------
					(*g_pTempInformation).Mode	= TempInformation::MODE_SHOP_SELL;
					(*g_pTempInformation).pValue = (void*)pItem;

					//-------------------------------------------------
					// ???? ?????????? ?????? ??????????..
					//-------------------------------------------------
					UI_LockItemTrade();
			}
			else
			{
				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_SELL ].GetString());
			}
		}
		else
		{
			DEBUG_ADD("[Error] Item Is NULL");
		}
	}
}


//-----------------------------------------------------------------------------
//
// ?????????????????????????????? ????
//
//-----------------------------------------------------------------------------
//
// gridX   //void_ptr = Mitem*
// gridY
//
void
UIMessageManager::Execute_UI_SELL_ALL_ITEM(int left, int right, void* void_ptr)
{				
	DEBUG_ADD("[UI] UI_SELL_ALL_ITEM");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	//MItem* pItem = (MItem*)void_ptr;
	// ?????????? ?????? ?????
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		//-----------------------------------------------------
		// ???????????? ???? ?????? packet??????????????
		//-----------------------------------------------------
			CGShopRequestSell	_CGShopRequestSell;

			_CGShopRequestSell.setObjectID( (*g_pPCTalkBox).GetNPCID() );		// NPC ID	
			_CGShopRequestSell.setOpCode( SHOP_REQUEST_SELL_ALL_SKULL );
			_CGShopRequestSell.setItemObjectID( 0 );	// Item Object ID

			g_pSocket->sendPacket( &_CGShopRequestSell );
	
				
			//-------------------------------------------------
			// ???????????????? Temp Information??????
			//-------------------------------------------------
			(*g_pTempInformation).Mode	= TempInformation::MODE_SHOP_SELL_ALL_SKULL;
			
			//-------------------------------------------------
			// ???? ?????????? ?????? ??????????..
			//-------------------------------------------------
			UI_LockItemTrade();
	}
}

//-----------------------------------------------------------------------------
//
//	Mouse Focus
// 
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_REMOVE_BACKGROUND_MOUSE_FOCUS(int left, int right, void* void_ptr)
{	
	if (g_Mode!=MODE_GAME)
	{
		return;
	}

	g_bUIInput = TRUE;
}

void
UIMessageManager::Execute_UI_BACKGROUND_MOUSE_FOCUS(int left, int right, void* void_ptr)
{	
	if (g_Mode!=MODE_GAME)
	{
		return;
	}

	//
	// (left, right) = mouse (x, y)
	//
	g_bUIInput = FALSE;
}
/*
//-----------------------------------------------------------------------------
//
// Skill View?????????? ????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLOSE_SKILL_VIEW(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CLOSE_SKILL_VIEW");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	gC_vs_ui.CloseSkillView();

	// ???? ???????????????????????????? ????.
	//g_pUIDialog->ShowPCTalkDlg();

	// mode ??????
	//(*g_pTempInformation).Mode = TempInformation::MODE_NULL;
}


//-----------------------------------------------------------------------------
//
//	Skill?????????????????? ????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_LEARN_SLAYER_SKILL(int left, int right, void* void_ptr)
{
	//
	// left = ACTIONINFO
	//
	DEBUG_ADD("[UI] UI_LEARN_SLAYER_SKILL");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	if ((*g_pTempInformation).Mode == TempInformation::MODE_SKILL_LEARN)
	{
		(*g_pTempInformation).Value3 = left;

		#ifdef	CONNECT_SERVER
			CGLearnSkill _CGLearnSkill;
			
			//_CGLearnSkill.setObjectID( (*g_pPCTalkBox).GetNPCID() );
			_CGLearnSkill.setSkillType( left );
			_CGLearnSkill.setSkillDomainType( (*g_pTempInformation).Value1 );

			g_pSocket->sendPacket( &_CGLearnSkill );

			
		#endif
	}
}
*/
//-----------------------------------------------------------------------------
//
// PDS?????????? ????
//
//-----------------------------------------------------------------------------
/*
void
UIMessageManager::Execute_UI_PDS_CLOSED(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_PDS_CLOSED");
	
	gC_vs_ui.AcquirePDSClosedMessage();
}
*/

//-----------------------------------------------------------------------------
//
// Slayer ?????? ?????? ?????? ??????????????????.
//
//-----------------------------------------------------------------------------
/*
void
UIMessageManager::Execute_UI_PLEASE_SET_SLAYER_VALUE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_PLEASE_SET_SLAYER_VALUE");
	
	//C_VS_UI_SLAYER_PDS::m_str		= g_pPlayer->GetSTR();
	//C_VS_UI_SLAYER_PDS::m_dex		= g_pPlayer->GetDEX();
	//C_VS_UI_SLAYER_PDS::m_int		= g_pPlayer->GetINT();
	//C_VS_UI_SLAYER_PDS::m_hp		= g_pPlayer->GetHP();
	//C_VS_UI_SLAYER_PDS::m_mp		= g_pPlayer->GetMP();
	//C_VS_UI_SLAYER_PDS::m_th		= g_pPlayer->GetTOHIT();
	//C_VS_UI_SLAYER_PDS::m_ac		= g_pPlayer->GetAC();
	//C_VS_UI_SLAYER_PDS::m_cc		= g_pPlayer->GetCARRYWEIGHT();
	//C_VS_UI_SLAYER_PDS::m_str_max	= g_pPlayer->GetSTR(); // (!)
	//C_VS_UI_SLAYER_PDS::m_dex_max	= g_pPlayer->GetDEX(); // (!)
	//C_VS_UI_SLAYER_PDS::m_int_max	= g_pPlayer->GetINT();
	//C_VS_UI_SLAYER_PDS::m_HP_MAX	= g_pPlayer->GetMAX_HP();
	//C_VS_UI_SLAYER_PDS::m_MP_MAX	= g_pPlayer->GetMAX_MP();
	//C_VS_UI_SLAYER_PDS::m_th_max	= g_pPlayer->GetTOHIT(); // (!)
	//C_VS_UI_SLAYER_PDS::m_ac_max	= g_pPlayer->GetAC();	 // (!)
	//C_VS_UI_SLAYER_PDS::m_cc_max	= g_pPlayer->GetCARRYWEIGHT(); // (!)	
}
*/

//-----------------------------------------------------------------------------
//
// PCS ?????????????? ????--> ???? ????????????????????.
//
//-----------------------------------------------------------------------------
/*
void
UIMessageManager::Execute_UI_SEND_PCS_NUMBER(int left, int right, void* void_ptr)
{
	//
	// left = pcs number
	//
	DEBUG_ADD("[UI] UI_SEND_PCS_NUMBER");
	
	int pcsNumber = left;


	//-------------------------------------------
	// Server???????????????????? ????
	//-------------------------------------------
	#ifdef CONNECT_SERVER
		CGDialUp _CGDialUp;
		_CGDialUp.setPhoneNumber( pcsNumber );

		g_pSocket->sendPacket( &_CGDialUp );

		
	//-------------------------------------------
	// Client Only
	//-------------------------------------------
	#else
		char str[128];
		strcpy(str, "Noname");
		UI_OnLinePCS(str, left);
	#endif

	//gC_vs_ui.OnLinePCS(); // PCS????????????????????????????????!
}
 */

//-----------------------------------------------------------------------------
//
// ?????? ?????? ????
//
//-----------------------------------------------------------------------------
/*
case UI_PCS_CONNECTOR_GRANTED:
//
// ?????? ???? ??????????????????
//
// left = pcs number
//
DEBUG_ADD("[UI] UI_PCS_CONNECTOR_GRANTED");

UI_OnLinePCS();
break;
*/

//-----------------------------------------------------------------------------
//
// PCS ?????????? ??????????.
//
//-----------------------------------------------------------------------------
// PCS ????????PCS number????send???? connecting ???? END??????????????
/*
void
UIMessageManager::Execute_UI_END_PCS(int left, int right, void* void_ptr) 
{
	DEBUG_ADD("[UI] UI_END_PCS");
	
	//------------------------------------------------
	// ???????????????????????????
	//------------------------------------------------
	#ifdef	CONNECT_SERVER			
		CGPhoneDisconnect _CGPhoneDisconnect;
			
		for (int i=0; i<3; i++)
		{
			if (g_pUserInformation->OtherPCSNumber[i] != 0)
			{
				_CGPhoneDisconnect.setSlotID( i );

				g_pSocket->sendPacket( &_CGPhoneDisconnect );

			}
		}
	#endif
}
*/

//-----------------------------------------------------------------------------
//
// Online ?????????? PCS??????????
//
//-----------------------------------------------------------------------------
/*
void
UIMessageManager::Execute_UI_QUIT_PCS_ONLINE_MODE(int left, int right, void* void_ptr)
{
	//
	// PCS online mode?????? quit ??????????????????
	//
	DEBUG_ADD("[UI] UI_QUIT_PCS_ONLINE_MODE");
	
	//------------------------------------------------
	// ???????????????????????????
	//------------------------------------------------
	#ifdef	CONNECT_SERVER			
		CGPhoneDisconnect _CGPhoneDisconnect;
			
		for (int i=0; i<3; i++)
		{
			if (g_pUserInformation->OtherPCSNumber[i] != 0)
			{
				_CGPhoneDisconnect.setSlotID( i );

				g_pSocket->sendPacket( &_CGPhoneDisconnect );

			}
		}
	#endif

	UI_AcquireQuitPCSOnlineModeMessage();
}
*/

//-----------------------------------------------------------------------------
//
// ???? ???????????????.
//
//-----------------------------------------------------------------------------
/*
void
UIMessageManager::Execute_UI_CHANGE_PCS_CONNECTED_SLOT(int left, int right, void* void_ptr)
{
	//
	// ???????? ???????????????????????.
	//
	// left = pcs number
	// right = slot 
	//
	DEBUG_ADD("[UI] UI_CHANGE_PCS_CONNECTED_SLOT");
	
	int slot = right;

	gC_vs_ui.GrantPCSWaitRoomToDuplex( slot );
}
*/

//-----------------------------------------------------------------------------
//
// Online ?????????? PCS??????????
//
//-----------------------------------------------------------------------------
// ?????? click! (???? ??????-duplex-?????? ????????
/*
void
UIMessageManager::Execute_UI_PLEASE_PCS_CONNECT_ME(int left, int right, void* void_ptr)
{
	//
	// ???????????? ?????????????????????? (?????????? ???? ???? ????????
	//
	// left = pcs number
	// right = slot 
	//
	DEBUG_ADD("[UI] UI_PLEASE_PCS_CONNECT_ME");
	
	int slot = right;

	// ????????slot??????????????
	gC_vs_ui.GrantPCSWaitRoomToDuplex( slot );
}
*/

//-----------------------------------------------------------------------------
//
// GameMenu Continue
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_GAMEMENU_CONTINUE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_GAMEMENU_CONTINUE");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	gC_vs_ui.CloseGameMenu();
}

//-----------------------------------------------------------------------------
//
// Minimap Toggle
//
//-----------------------------------------------------------------------------
/*
void
UIMessageManager::Execute_UI_MINIMAP_TOGGLE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_MINIMAP_TOGGLE");
	
	g_pUserOption->DrawMinimap = !g_pUserOption->DrawMinimap;
}
*/

//-----------------------------------------------------------------------------
//
// Drop Money
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_DROP_MONEY(int left, int right, void* void_ptr)
{
	return;
	//
	// ???????????????????? drop????.
	//
	// left = money
	//
	DEBUG_ADD("[UI] UI_DROP_MONEY");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	//-----------------------------------------------------------------------------
	// ?????? 0???? ??????
	//-----------------------------------------------------------------------------
	if (left > 0)
	{
		if (g_pMoneyManager!=NULL)
		{
				CGDropMoney _CGDropMoney;
				_CGDropMoney.setAmount( left );

				g_pSocket->sendPacket( &_CGDropMoney );

				

			//-----------------------------------------------
			// ??????????????????
			//-----------------------------------------------
			g_pMoneyManager->UseMoney( left );
		}
		else
		{
			DEBUG_ADD("[Error] MoneyManager is NULL");
		}
	}
}

//-----------------------------------------------------------------------------
//
// Click Bonus Point - ????????????????????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLICK_BONUS_POINT(int left, int right, void* void_ptr)
{
	//
	// vampire bonus point?????????????????? ??????????????????
	//
	// g_char_slot_ingame????bonus point???????????? ???????????????? ????????????.
	//
	// left = { 0:STR, 1:DEX, 2:INT }
	//
	DEBUG_ADD("[UI] UI_CLICK_BONUS_POINT");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	const int bonusPart[] =
	{
		INC_STR,
		INC_DEX,
		INC_INT
	};

	// ??????????????????..
	if ((*g_pTempInformation).Mode==TempInformation::MODE_NULL)
	{
		int bonusPoint = g_pPlayer->GetBonusPoint();

		//----------------------------------------------
		// vampire???? bonuspoint?????????? ????
		//----------------------------------------------
		// 2005, 1, 18, sobeit modify start - ?????? ???????????????????????????? ????.
//		if (!g_pPlayer->IsSlayer()// || g_pPlayer->IsOusters()
//			&& bonusPoint > 0)
		if(bonusPoint > 0)
		// 2005, 1, 18, sobeit modify end - ?????? ???????????????????????????? ????.
		{
			
				CGUseBonusPoint _CGUseBonusPoint;

				_CGUseBonusPoint.setWhich( bonusPart[left] );

				g_pSocket->sendPacket( &_CGUseBonusPoint );

				
				//----------------------------------------------
				// ?????????????? ?????
				//----------------------------------------------
				(*g_pTempInformation).Mode = TempInformation::MODE_BONUSPOINT_USE;
				(*g_pTempInformation).Value1 = bonusPart[left];				
		
				
		}
	}
}


//-----------------------------------------------------------------------------
//
// CharInfo ??????????????????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_INFO(int left, int right, void* void_ptr)
{
	//
	// character info button?????????????? ?????????????????????????????????!
	//
//	DEBUG_ADD("[UI] UI_CHARINFO");
//	
//	
//	if (g_Mode!=MODE_GAME)
//	{
//		DEBUG_ADD("Not Mode MODE_GAME");
//		return;
//	}
//
//
//	if (gC_vs_ui.IsRunningElevator())
//	{
//		DEBUG_ADD("[Execute_UI_CHARINFO] before RunCharInfo-Elevator Running");
//	}
//
//	//----------------------------------------------
//	// ??????????????????????
//	//----------------------------------------------
//	if (gC_vs_ui.IsRunningCharInfo())
//	{
//		gC_vs_ui.CloseCharInfo();
//	}
//	//----------------------------------------------
//	// ??????.
//	//----------------------------------------------
//	else
//	{
//		// set slot
//
//		// g_char_slot_ingame ?????????? ???????? ??????????????????.
//		/*
//		g_char_slot_ingame.sz_name = "yaho";
//		g_char_slot_ingame.bl_vampire = true;//false;
//		g_char_slot_ingame.MP_MAX = 100;
//		g_char_slot_ingame.HP_MAX = 100;
//		g_char_slot_ingame.HP = 100;
//		g_char_slot_ingame.MP = 100;
//		g_char_slot_ingame.bl_female = false;
//
//		g_char_slot_ingame.bonus_point = 5;
//		*/
//		g_char_slot_ingame.sz_name = g_pUserInformation->CharacterID.GetString();
//		g_char_slot_ingame.bl_vampire = g_pPlayer->IsVampire();
//		g_char_slot_ingame.bl_female = !g_pPlayer->IsMale();
//		
//		
//		if (g_pPlayer->IsSlayer())
//		{
//			g_char_slot_ingame.level = 1;//g_pPlayer->GetLEVEL();
//
//			//g_char_slot_ingame.skin_color = g_pPlayer->GetAddonInfo(ADDON_COAT).ColorSet1;
//			//g_char_slot_ingame.hair_color = g_pPlayer->GetAddonInfo(ADDON_HAIR).ColorSet1;
//			g_char_slot_ingame.hair_color = g_pUserInformation->HairColor;
//			g_char_slot_ingame.skin_color = g_pUserInformation->SkinColor;
//		}
//		else
//		{
//			g_char_slot_ingame.level = g_pPlayer->GetVampExp();
//
//			//g_char_slot_ingame.skin_color = g_pPlayer->GetBodyColor1();//pCreatureWear->GetAddonInfo(ADDON_COAT).ColorSet1;
//			//g_char_slot_ingame.hair_color = pCreatureWear->GetAddonInfo(ADDON_HAIR).ColorSet1;
//			g_char_slot_ingame.hair_color = g_pUserInformation->HairColor;
//			g_char_slot_ingame.skin_color = g_pUserInformation->SkinColor;
//		}
//		
//		if (g_pPlayer->IsMale())
//		{
//			switch (g_pUserInformation->FaceStyle)
//			{
//				case HAIR_STYLE1 :
//					g_char_slot_ingame.man_info.face = M_FACE1;
//				break;
//				case HAIR_STYLE2 :
//					g_char_slot_ingame.man_info.face = M_FACE2;
//				break;
//				case HAIR_STYLE3 :
//					g_char_slot_ingame.man_info.face = M_FACE3;
//				break;
//			}
//		}
//		else
//		{
//			switch (g_pUserInformation->FaceStyle)
//			{
//				case HAIR_STYLE1 :
//					g_char_slot_ingame.woman_info.face = W_FACE1;
//				break;
//				case HAIR_STYLE2 :
//					g_char_slot_ingame.woman_info.face = W_FACE2;
//				break;
//				case HAIR_STYLE3 :
//					g_char_slot_ingame.woman_info.face = W_FACE3;
//				break;
//			}
//		}
//
//		g_char_slot_ingame.level = g_pPlayer->GetLEVEL();
//		g_char_slot_ingame.alignment = (ALIGNMENT)ConvertAlignment( g_pPlayer->GetAlignment() );
//		g_char_slot_ingame.EXP_CUR	= g_pPlayer->GetVampExp();
//
//		g_char_slot_ingame.STR_CUR =g_pPlayer->GetSTR();
//		g_char_slot_ingame.DEX_CUR =g_pPlayer->GetDEX();
//		g_char_slot_ingame.INT_CUR =g_pPlayer->GetINT();
//		g_char_slot_ingame.STR_MAX =g_pPlayer->GetMAX_STR();
//		g_char_slot_ingame.DEX_MAX =g_pPlayer->GetMAX_DEX();
//		g_char_slot_ingame.INT_MAXX =g_pPlayer->GetMAX_INT();
//		g_char_slot_ingame.STR_EXP_CUR =g_pPlayer->GetSTR_EXP();
//		g_char_slot_ingame.DEX_EXP_CUR =g_pPlayer->GetDEX_EXP();
//		g_char_slot_ingame.INT_EXP_CUR =g_pPlayer->GetINT_EXP();

		// test code
//		g_char_slot_ingame.STR_EXP_NEXT_LEVEL	= 10000;
//		g_char_slot_ingame.DEX_EXP_NEXT_LEVEL	= 10000;
		//g_char_slot_ingame.INT_EXP_NEXT_LEVEL	= 10000;
//		g_char_slot_ingame.STR_EXP_CUR_LEVEL	= 5000;
//		g_char_slot_ingame.DEX_EXP_CUR_LEVEL	= 5000;
//		g_char_slot_ingame.INT_EXP_CUR_LEVEL	= 5000;


		//g_char_slot_ingame.DAM =g_pPlayer->GetDAM();
		//g_char_slot_ingame.AC =g_pPlayer->GetAC();
		//g_char_slot_ingame.TOHIT =g_pPlayer->GetTOHIT();
//		g_char_slot_ingame.HP =g_pPlayer->GetHP();
//		g_char_slot_ingame.MP =g_pPlayer->GetMP();
//		g_char_slot_ingame.HP_MAX =g_pPlayer->GetMAX_HP();
//		g_char_slot_ingame.MP_MAX =g_pPlayer->GetMAX_MP();
//		g_char_slot_ingame.SILVER_HP = g_pPlayer->GetSilverDamage();
//		//g_char_slot_ingame.CC =g_pPlayer->GetCARRYWEIGHT();
//		g_char_slot_ingame.FAME =g_pPlayer->GetFAME();
//		g_char_slot_ingame.NOTERITY =g_pPlayer->GetNotoriety();
//
//		DEBUG_ADD("[UI] CalculateStatus");
//		
//		g_pPlayer->CalculateStatus();
//
//		DEBUG_ADD("[UI] RunCharInfo");
//		
//
//		gC_vs_ui.RunInfo();
//	}
//
//	if (gC_vs_ui.IsRunningElevator())
//	{
//		DEBUG_ADD("[Execute_UI_CHARINFO] after RunCharInfo-Elevator Running");
//	}
//
//	DEBUG_ADD("[UI] UI_CHARINFO OK");
}

//-----------------------------------------------------------------------------
//
// ???????????????????????????????????? ????
//
//-----------------------------------------------------------------------------
//
// void_ptr = Mitem*
//
void
UIMessageManager::Execute_UI_REPAIR_ITEM(int left, int right, void* void_ptr)
{				
	DEBUG_ADD("[UI] UI_REPAIR_ITEM");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	// ?????????? ?????? ?????
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		MItem* pItem = NULL;// = (MItem*)void_ptr;
		int itemID = left;

		//-------------------------------------------------------------
		// itemID????0?????????? ???? ??????????
		//-------------------------------------------------------------
		if (itemID==0)
		{
			//-----------------------------------------------------
			// ???????????? ?????????? ?????? packet??????????????
			//-----------------------------------------------------
				CGRequestRepair	_CGRequestRepair;

				_CGRequestRepair.setObjectID( 0 );
				
				g_pSocket->sendPacket( &_CGRequestRepair );
			
				//-------------------------------------------------
				// ???????????????? Temp Information??????
				//-------------------------------------------------
				(*g_pTempInformation).Mode		= TempInformation::MODE_SHOP_REPAIR;
				(*g_pTempInformation).Value1	= false;	// inventory???????? item?????
				(*g_pTempInformation).Value2	= true;		// gear???????? item?????
				(*g_pTempInformation).pValue	= NULL;

				//-------------------------------------------------
				// ???? ?????????? ?????? ??????????..
				//-------------------------------------------------
				UI_LockItemTrade();
		}
		//-------------------------------------------------------------
		// item ??????????????
		//-------------------------------------------------------------
		else
		{
			BOOL bInInventory = FALSE;
			BOOL bInGear = FALSE;

			//------------------------------------------------------------------------------
			// inventory?????? ??????
			//------------------------------------------------------------------------------
			pItem = g_pInventory->GetItemToModify( itemID );

			if (pItem==NULL)
			{
				//------------------------------------------------------------------------------
				// gear?????? ??????
				//------------------------------------------------------------------------------
				switch(g_pPlayer->GetRace())
				{
				case RACE_SLAYER:
					pItem = g_pSlayerGear->GetItemToModify( itemID );
					
					if (pItem!=NULL)
					{
						bInGear = TRUE;
					}
					break;

				case RACE_VAMPIRE:
					pItem = g_pVampireGear->GetItemToModify( itemID );
					
					if (pItem!=NULL)
					{
						bInGear = TRUE;
					}
					break;

				case RACE_OUSTERS:
					pItem = g_pOustersGear->GetItemToModify( itemID );
					
					if (pItem!=NULL)
					{
						bInGear = TRUE;
					}
					break;
				}
			}
			else
			{
				bInInventory = TRUE;
			}

			//-------------------------------------------------
			// inventory????gear????????????????????.
			//-------------------------------------------------
			if (bInInventory || bInGear)
			{
				//-------------------------------------------------
				// ?????????????????? item?????? ??????.
				// ?????????????????????????????????????.
				//-------------------------------------------------
				if (pItem->GetItemClass() != ITEM_CLASS_VAMPIRE_AMULET
					|| !pItem->IsUniqueItem()
					|| !pItem->IsQuestItem()
					|| pItem->GetMaxDurability() != -1
					|| pItem->GetItemClass()==ITEM_CLASS_KEY
					|| pItem->IsChargeItem())
				{
					//-------------------------------------------------
					// ?????? ?????????????????.
					//-------------------------------------------------
					int price = g_pPriceManager->GetItemPrice(pItem, MPriceManager::REPAIR);
					int money = g_pMoneyManager->GetMoney();
#if __CONTENTS(__REPAIRSYSTEM_RENEWAL)
					MPriceManager::REPAIRPIRCEPAIR getPricePair = g_pPriceManager->GetRepairPricePair(pItem->GetID());
					price = getPricePair.second;
#endif //__REPAIRSYSTEM_RENEWAL
					//-------------------------------------------------					
					// ??????????0???? ?????????????? ??????.
					// ???? key????????????????????????????????
					//-------------------------------------------------					
					if (price!=0 || pItem->GetItemClass()==ITEM_CLASS_KEY)
					{
						//-------------------------------------------------
						// ?????? ?????????????
						//-------------------------------------------------
						if (money!=0 && price <= money)
						{														
							//-----------------------------------------------------
							// ???????????? ?????????? ?????? packet??????????????
							//-----------------------------------------------------
								CGRequestRepair	_CGRequestRepair;

								_CGRequestRepair.setObjectID( pItem->GetID() );
								
								g_pSocket->sendPacket( &_CGRequestRepair );
							
								//-------------------------------------------------
								// ???????????????? Temp Information??????
								//-------------------------------------------------
								(*g_pTempInformation).Mode		= TempInformation::MODE_SHOP_REPAIR;
								(*g_pTempInformation).Value1	= bInInventory;	// inventory???????? item?????
								(*g_pTempInformation).Value2	= bInGear;		// gear???????? item?????
								(*g_pTempInformation).pValue	= (void*)pItem;

								//-------------------------------------------------
								// ???? ?????????? ?????? ??????????..
								//-------------------------------------------------
								UI_LockItemTrade();
						}
						//-------------------------------------------------
						// ?????? ???????????????
						//-------------------------------------------------					
						else
						{
							g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_BUY_NO_MONEY ].GetString());
						}
					}
					//-------------------------------------------------
					// ?????????????? ?????? ?????
					//-------------------------------------------------
					else
					{
						g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_REPAIR].GetString());
					}
				}
				//-------------------------------------------------
				// ?????????????? ?????? ?????
				//-------------------------------------------------
				else
				{
					g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_REPAIR].GetString());
				}
			}		
		}
	}
}

//-----------------------------------------------------------------------------
//
// Silvering ?????????????????????? ????
//
//-----------------------------------------------------------------------------
//
// void_ptr = Mitem*
//
void
UIMessageManager::Execute_UI_SILVERING_ITEM(int left, int right, void* void_ptr)
{				
	DEBUG_ADD("[UI] UI_SILVERING_ITEM");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	// ?????????? ?????? ?????
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		MItem* pItem = NULL;// = (MItem*)void_ptr;
		int itemID = left;

		BOOL bInInventory = FALSE;
		BOOL bInGear = FALSE;

		//------------------------------------------------------------------------------
		// inventory?????? ??????
		//------------------------------------------------------------------------------
		pItem = g_pInventory->GetItemToModify( itemID );

		if (pItem==NULL)
		{
			//------------------------------------------------------------------------------
			// gear?????? ??????
			//------------------------------------------------------------------------------
			switch(g_pPlayer->GetRace())
			{
			case RACE_SLAYER:
				pItem = g_pSlayerGear->GetItemToModify( itemID );
				
				if (pItem!=NULL)
				{
					bInGear = TRUE;
				}
				break;

			case RACE_VAMPIRE:
				pItem = g_pVampireGear->GetItemToModify( itemID );
				
				if (pItem!=NULL)
				{
					bInGear = TRUE;
				}
				break;

			case RACE_OUSTERS:
				pItem = g_pOustersGear->GetItemToModify( itemID );
				
				if (pItem!=NULL)
				{
					bInGear = TRUE;
				}
				break;
			}
		}
		else
		{
			bInInventory = TRUE;
		}

		//-------------------------------------------------
		// inventory????gear??????????silvering????..
		//-------------------------------------------------
		if (bInInventory || bInGear)
		{
			//-------------------------------------------------
			// Silvering ???????????? item?????? ??????.
			//-------------------------------------------------
			if (pItem->GetSilverMax() > 0)
			{
				//-------------------------------------------------
				// ?????? ?????????????????.
				//-------------------------------------------------
				int price = (*g_pPriceManager).GetItemPrice(pItem, MPriceManager::SILVERING);
				int money = (*g_pMoneyManager).GetMoney();

				//-------------------------------------------------					
				// ??????????0???? silvering ???????? ??????.
				//-------------------------------------------------					
				if (price!=0)
				{
					//-------------------------------------------------
					// ?????? ?????????????
					//-------------------------------------------------
					if (money!=0 && price <= money)
					{														
						//-----------------------------------------------------
						// ???????????? silverig???? ?????? packet??????????????
						//-----------------------------------------------------
							CGSilverCoating	_CGSilverCoating;

							_CGSilverCoating.setObjectID( pItem->GetID() );
							
							g_pSocket->sendPacket( &_CGSilverCoating );
						
							//-------------------------------------------------
							// ???????????????? Temp Information??????
							//-------------------------------------------------
							(*g_pTempInformation).Mode		= TempInformation::MODE_SHOP_SILVERING;
							(*g_pTempInformation).Value1	= bInInventory;	// inventory???????? item?????
							(*g_pTempInformation).Value2	= bInGear;		// gear???????? item?????
							(*g_pTempInformation).pValue	= (void*)pItem;

							//-------------------------------------------------
							// ???? ?????????? ?????? ??????????..
							//-------------------------------------------------
							UI_LockItemTrade();
					}
					//-------------------------------------------------
					// ?????? ???????????????
					//-------------------------------------------------					
					else
					{
						g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_BUY_NO_MONEY ].GetString());
					}
				}
				//-------------------------------------------------
				// Silvering???????? ?????? ?????
				//-------------------------------------------------
				else
				{
					g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_SILVERING].GetString());
				}
			}
			//-------------------------------------------------
			// ?????????????? ?????? ?????
			//-------------------------------------------------
			else
			{
				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_SILVERING].GetString());
			}
		}		
	}
}

//-----------------------------------------------------------------------------
//
// ?????? ????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_REPAIR_FINISHED(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_REPAIR_FINISHED");
	
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation!=NULL
		&& g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.FinishItemRepairing();

		// ???? ???????????????????????????? ????.
		g_pUIDialog->ShowPCTalkDlg();
	}
}

//-----------------------------------------------------------------------------
//
// silvering ????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_SILVERING_FINISHED(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_SILVERING_FINISHED");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}


	if (g_pTempInformation!=NULL
		&& g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.FinishItemSilvering();

		// ???? ???????????????????????????? ????.
		g_pUIDialog->ShowPCTalkDlg();
	}
}

//-----------------------------------------------------------------------------
//
// ??????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_FINISH_LEVELUP_BUTTON(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_FINISH_LEVELUP_BUTTON");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	gC_vs_ui.FinishLevelUp();
}

//-----------------------------------------------------------------------------
//
// ???????? ????? ???? ??????????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_RUNNING_GAMEMENU(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_RUNNING_GAMEMENU");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
//	g_pUIDialog->CloseHelpDlg();
	g_pUIDialog->CloseMessageDlg();
	g_pUIDialog->ClosePCTalkDlg();
}

//-----------------------------------------------------------------------------
//
// ???????????????
//
//-----------------------------------------------------------------------------
// left = ?????TRUE), ??????FALSE)
void
UIMessageManager::Execute_UI_STORAGE_BUY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_STORAGE_BUY");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	BOOL buy = (BOOL)left;

	//-----------------------------------------------------------------------------
	// ???????????? ??????????????..
	//-----------------------------------------------------------------------------
	if (buy)
	{
		if (g_pTempInformation->GetMode()==TempInformation::MODE_STORAGE_BUY)
		{		
			//--------------------------------------------------------
			// ??????
			//--------------------------------------------------------
			if (g_pMoneyManager->GetMoney() >= g_pTempInformation->Value1)
			{
					CGStashRequestBuy _CGStashRequestBuy;
					
					g_pSocket->sendPacket( &_CGStashRequestBuy );					

				// mode ??????
				g_pTempInformation->SetMode(TempInformation::MODE_STORAGE_BUY_WAIT);

			}
			//--------------------------------------------------------
			// ?????? ???????????????
			//--------------------------------------------------------
			else
			{
				// ???? ???????????????????????????? ????.
				g_pUIDialog->ShowPCTalkDlg();

				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_BUY_NO_MONEY ].GetString());
				
				g_pTempInformation->SetMode(TempInformation::MODE_NULL);
			}
		}
		else
		{
			DEBUG_ADD("[Error] Mode is Not MODE_STORAGE_BUY");
		}

		// ???????????.. ?????packet????????????????????
		// NPC???? dialog??????????????.
	}
	//-----------------------------------------------------------------------------
	// ???????????????????
	//-----------------------------------------------------------------------------
	else
	{
		// ???? ???????????????????????????? ????.
		g_pUIDialog->ShowPCTalkDlg();

		g_pTempInformation->SetMode(TempInformation::MODE_NULL);
	}					
}

//-----------------------------------------------------------------------------
//
// ???????????? slot??????????????????
//
//-----------------------------------------------------------------------------
// left = tab (0, 1, 2)
// right = slot
//
void
UIMessageManager::Execute_UI_SELECT_STORAGE_SLOT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_SELECT_STORAGE_SLOT");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if(g_pStorage == NULL)
		return;

	g_pStorage->SetCurrent( left );	// ??????

	int slot = right;

	MItem* pMouseItem = (MItem*)gpC_mouse_pointer->GetPickUpItem();
	const MItem* pStorageItem = g_pStorage->GetItem( slot );

	//-----------------------------------------------------------------------------
	// ???????????????????????????????????? ?????
	//-----------------------------------------------------------------------------
	if (pMouseItem==NULL)
	{
		//---------------------------------------------------
		// ??????????slot????item??????????????????????
		//---------------------------------------------------
		if (pStorageItem!=NULL)
		{
			MItem* pRemovedItem = g_pStorage->RemoveItem( slot );

			if (pRemovedItem!=NULL)
			{
				 UI_PickUpItem( (MItem*)pStorageItem );

					CGStashToMouse _CGStashToMouse;
					_CGStashToMouse.setObjectID( pStorageItem->GetID() );
					_CGStashToMouse.setRack( g_pStorage->GetCurrent() );
					_CGStashToMouse.setIndex( slot );									

					g_pSocket->sendPacket( &_CGStashToMouse );					
			}
		}
	}
	//-----------------------------------------------------------------------------
	// ????????? item?????????????? ???????????? ?????
	//-----------------------------------------------------------------------------
	else 
	{
		BOOL bSendPacket = TRUE;

		TYPE_OBJECTID mouseItemID = pMouseItem->GetID();		

		if(pMouseItem->GetItemClass() == ITEM_CLASS_SUB_INVENTORY && pMouseItem->GetEnchantLevel()>0) 
		{
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_STORAGE].GetString());
			return ;
		}
	
		// ????????????????????MItemTable????ItemMoveControl ????????????????
		// ?????????????? ??????????????????????????
		// 2006.12.26 by chyaya

		/*
		// Event GiftBox ???????????? ?????????????????
		if (pMouseItem->GetItemClass()!=ITEM_CLASS_EVENT_GIFT_BOX
			&& pMouseItem->GetItemClass()!=ITEM_CLASS_VAMPIRE_AMULET
			&& pMouseItem->GetItemClass()!=ITEM_CLASS_COUPLE_RING
			&& pMouseItem->GetItemClass()!=ITEM_CLASS_VAMPIRE_COUPLE_RING
			&& pMouseItem->GetItemClass()!=ITEM_CLASS_OUSTERS_HARMONIC_PENDENT 
			
			&& !pMouseItem->IsQuestItem()
			&& pMouseItem->IsNormalItem()

			&& !((pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_TREE) && pMouseItem->GetItemType() >= 13  )
			&& pMouseItem->GetItemClass() != ITEM_CLASS_EVENT_ITEM
			&& pMouseItem->GetItemClass() != ITEM_CLASS_CODE_SHEET
			&& !(pMouseItem->GetItemClass() == ITEM_CLASS_LUCKY_BAG && pMouseItem->GetItemType() == 3)

			
			// Sjheon 2006.03.10 
			//&& pMouseItem->GetItemClass()   != ITEM_CLASS_COMMON_QUEST_ITEM
			&& !(pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_GIFT_BOX && (pMouseItem->GetItemType() == 27))		//???????????????? ??????
			
			&& !(pMouseItem->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && (pMouseItem->GetItemType() >= 20 && pMouseItem->GetItemType() <= 23))

			// sjheo 2005.05.02 ???? ?????????????? ??????????????  Add 
			&& !(pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_ETC && (pMouseItem->GetItemType() == 18))
			&& !(pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_ITEM && (pMouseItem->GetItemType() >= 32 && pMouseItem->GetItemType() <= 36))

			&& !(pMouseItem->GetItemClass() == ITEM_CLASS_CHECK_MONEY) 

			// sjheo 2005.05.02 ???? ?????????????? ?????????????? End
			)
			*/


		if(!pMouseItem->IsQuestItem() && pMouseItem->IsNormalItem() &&			// ???????????????????? ????????????
		   !pMouseItem->GetItemMoveContorl().GetAttr(ITEMMOVE_CANNOT_KEEP_STORAGE))	// ?????????? ???????? ???????????? ????
		{
			//---------------------------------------------------
			// ???????????????????? ??????????item ??????
			//---------------------------------------------------
			if (pStorageItem==NULL)
			{
				UI_DropItem();	// mouse?????? item??????.

				g_pStorage->SetItem( slot, pMouseItem );							
			}
			//---------------------------------------------------
			// ?????? ??????..
			//---------------------------------------------------
			else
			{
				//---------------------------------------------------
				// ?????? ???????? ???????????? ?????
				//---------------------------------------------------
				if (pStorageItem->IsPileItem()
					&& pStorageItem->GetItemClass()==pMouseItem->GetItemClass()
					&& pStorageItem->GetItemType()==pMouseItem->GetItemType())
				{
					//---------------------------------------------------
					// ?????????? ??????????
					//---------------------------------------------------
					if (pStorageItem->GetNumber() < pStorageItem->GetMaxNumber())
					{
						PlaySound( pMouseItem->GetInventorySoundID() );
						
						MItem* pModifyStorageItem = g_pStorage->RemoveItem( slot );

						//----------------------------------------------------
						// pMouseItem????pStorageItem????????????????
						//----------------------------------------------------
						int total = pMouseItem->GetNumber() + pStorageItem->GetNumber();
						if ( total > pStorageItem->GetMaxNumber() )
						{
							// ???? ???????????? ?????
							pMouseItem->SetNumber( total - pStorageItem->GetMaxNumber() );
							pModifyStorageItem->SetNumber( pStorageItem->GetMaxNumber() );
						}
						else
						{
							// ?????pItem???????????????????? ?????
							pModifyStorageItem->SetNumber( total );
							UI_DropItem();

							delete pMouseItem;
						}	

						//----------------------------------------------------
						// ?????? ???? ??????????.
						//----------------------------------------------------
						g_pStorage->SetItem( slot, pModifyStorageItem );					
						
					}
					//---------------------------------------------------
					// ?????????? ????????????. ?????????.
					//---------------------------------------------------				
					else
					{
						bSendPacket = FALSE;
					}
				}
				//---------------------------------------------------				
				// ?????? ?????????? ???????????? ?????--> ??????
				//---------------------------------------------------				
				else
				{				
					MItem* pTempItem = pMouseItem;
					
					UI_PickUpItem( (MItem*)pStorageItem );

					g_pStorage->RemoveItem( slot );	// ???? ??????????.. - -;

					g_pStorage->SetItem( slot, pTempItem );
				}
			}

				if (bSendPacket)
				{		
					CGMouseToStash _CGMouseToStash;
					_CGMouseToStash.setObjectID( mouseItemID );
					_CGMouseToStash.setRack( g_pStorage->GetCurrent() );
					_CGMouseToStash.setIndex( slot );									

					g_pSocket->sendPacket( &_CGMouseToStash );				
				}
		}
		else
		{
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_STORAGE].GetString());
		}


	}
}

//-----------------------------------------------------------------------------
//
// ?????????? ??????????????,
//
//-----------------------------------------------------------------------------
// left = money
//
void
UIMessageManager::Execute_UI_DEPOSIT_MONEY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_DEPOSIT_MONEY");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	//-----------------------------------------------------------------------------
	// ?????? 0???? ??????
	//-----------------------------------------------------------------------------
	if (left > 0)
	{
		if (g_pMoneyManager!=NULL && g_pStorage!=NULL)
		{
				CGStashDeposit _CGStashDeposit;
				_CGStashDeposit.setAmount( left );

				g_pSocket->sendPacket( &_CGStashDeposit );				

			//-----------------------------------------------
			// ?????? ??????????????????????
			//-----------------------------------------------
			g_pMoneyManager->UseMoney( left );
			g_pStorage->GetMoneyManager()->AddMoney( left );
		}
		else
		{
			DEBUG_ADD("[Error] MoneyManager or Storage is NULL");
		}
	}
}

//-----------------------------------------------------------------------------
//
// ???????????? ?????? ????????
//
//-----------------------------------------------------------------------------
// left = money
//
void
UIMessageManager::Execute_UI_WITHDRAW_MONEY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_WITHDRAW_MONEY");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	//-----------------------------------------------------------------------------
	// ?????? 0???? ??????
	//-----------------------------------------------------------------------------
	if (left > 0)
	{
		if (g_pMoneyManager!=NULL && g_pStorage!=NULL)
		{
				CGStashWithdraw _CGStashWithdraw;
				_CGStashWithdraw.setAmount( left );

				g_pSocket->sendPacket( &_CGStashWithdraw );				

			//-----------------------------------------------
			// ???????????? ?????? player?????? ??????
			//-----------------------------------------------
			g_pStorage->GetMoneyManager()->UseMoney( left );
			g_pMoneyManager->AddMoney( left );						
		}
		else
		{
			DEBUG_ADD("[Error] MoneyManager or Storage is NULL");
		}
	}
}

//-----------------------------------------------------------------------------
//
//	???????????????? ????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLOSE_STORAGE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CLOSE_STORAGE");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	UI_CloseStorage();		

	// ???? ???????????????????????????? ????.
	g_pUIDialog->ShowPCTalkDlg();
}

//-----------------------------------------------------------------------------
//
// ???????? ?????? ??????
//
//-----------------------------------------------------------------------------
// left = bGive - ?????????????? ????????
// right = money
//
void
UIMessageManager::Execute_UI_EXCHANGE_MONEY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_EXCHANGE_MONEY");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	if (g_pTradeManager!=NULL)
	{
		BOOL bGive = left;
		int money = right;
			
		BOOL bAcceptMyTrade = g_pTradeManager->IsAcceptMyTrade();

		int code;		

		//---------------------------------------------------------
		// ???????? OK ????????????????????????????????????.
		//---------------------------------------------------------
		if (bAcceptMyTrade)
		{	
			if (g_pTempInformation->GetMode() == TempInformation::MODE_NULL)
			{
				if (bGive)
				{
					g_pTempInformation->SetMode(TempInformation::MODE_TRADE_VERIFY_ADD_MONEY);
				}
				else
				{
					g_pTempInformation->SetMode(TempInformation::MODE_TRADE_VERIFY_REMOVE_MONEY);

					g_pTradeManager->SetNextAcceptTime();
				}

				g_pTempInformation->Value1	= money;
			}
		}
		//---------------------------------------------------------
		// OK ?????????????? ?????? ????.
		//---------------------------------------------------------
		else
		{
			/*
			//-----------------------------------------------------------
			// ?????? ???????.. OK?????
			//-----------------------------------------------------------
			g_pTradeManager->RefuseOtherTrade();
			g_pTradeManager->RefuseMyTrade();
		
			//-----------------------------------------------------------
			// client ????????
			//-----------------------------------------------------------
			if (bGive)
			{
				//if (!g_pMoneyManager->GetOtherMoneyManager->CanAddMoney(money))
				//{
				//	money = g_pMoneyManager->GetOtherMoneyManager->GetMaxAddMoney();
				//}

				g_pMoneyManager->UseMoney( money );
				g_pTradeManager->GetMyMoneyManager()->AddMoney( money );
			}
			else
			{
				g_pTradeManager->GetMyMoneyManager()->UseMoney( money );
				g_pMoneyManager->AddMoney( money );	
			}
			*/

			if (!bGive)
			{
				g_pTradeManager->SetNextAcceptTime();
			}
		}	

		//-----------------------------------------------------------
		// client ????????
		//-----------------------------------------------------------
		if (bGive)
		{
			code = CG_TRADE_MONEY_INCREASE;
		}
		else
		{
			code = CG_TRADE_MONEY_DECREASE;
		}
		
		//-----------------------------------------------------------
		// server????????
		//-----------------------------------------------------------
			CGTradeMoney _CGTradeMoney;
			_CGTradeMoney.setTargetObjectID( g_pTradeManager->GetOtherID() );
			_CGTradeMoney.setAmount( money );
			_CGTradeMoney.setCode( code );				

			g_pSocket->sendPacket( &_CGTradeMoney );			

//			g_pTradeManager->SetNextAcceptTime();
	}
	else
	{
		DEBUG_ADD( "[Error] TradeManager is NULL");
	}


}

//-----------------------------------------------------------------------------
//
// ???????? ???????????? ?????????? 
//
//-----------------------------------------------------------------------------
// gridXY = (left,right)
// ????????item = (MItem*)void_ptr
/*
void
UIMessageManager::Execute_UI_ITEM_PICKUP_FROM_TRADEGRID(int left, int right, void* void_ptr)
{
	//
	// hi/low dw_left????grid (x, y)????????
	//        dw_right????item????screen ????????.
	//
	DEBUG_ADD("[UI] UI_ITEM_PICKUP_FROM_TRADEGRID");
	
	int		gridX = left;
	int		gridY = right;
	MItem*	pItem = (MItem *)void_ptr;

	UI_PickUpItem( pItem );
}
*/

//-----------------------------------------------------------------------------
//
// ???????? ?????????? ????????,
//
//-----------------------------------------------------------------------------
// gridXY = (left, right)
// ?????????? item = (MItem*)void_ptr
/*
void
UIMessageManager::Execute_UI_ITEM_INSERT_FROM_TRADEGRID(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_INSERT_FROM_TRADEGRID");
	
	int		gridX = left;
	int		gridY = right;
	MItem*	pItem = (MItem *)void_ptr;
	MItem*	pMouseItem = (MItem*)gpC_mouse_pointer->GetPickUpItem();
}
*/

//-----------------------------------------------------------------------------
//
// ???????? ???????????????? ????
//
//-----------------------------------------------------------------------------
// gridXY = (left, right)
// ?????? ???? ?????? ????????? item = (MItem*)void_ptr
/*
void
UIMessageManager::Execute_UI_ITEM_DROP_TO_TRADEGRID(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_DROP_TO_TRADEGRID");
	
	int		gridX = left;
	int		gridY = right;
	MItem*	pItem = (MItem *)void_ptr;
}
*/

//-----------------------------------------------------------------------------
//
// ???????? ???????????????? ????
//
//-----------------------------------------------------------------------------
// gridXY = (left, right)
// ?????? ???? ?????? ????????? item = (MItem*)void_ptr
void
UIMessageManager::Execute_UI_ITEM_SELECT_EXCHANGE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_SELECT_EXCHANGE");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTradeManager!=NULL)
	{
		int		gridX = left;
		int		gridY = right;
		MItem*	pItem = (MItem *)void_ptr;

		BOOL bAcceptMyTrade = g_pTradeManager->IsAcceptMyTrade();		

		// ????????????????????MItemTable????ItemMoveControl ????????????????
		// ?????????????? ??????????????????????????
		// 2006.12.26 by chyaya
		/*
		else if((pItem->GetItemClass() == ITEM_CLASS_SUB_INVENTORY && pItem->GetEnchantLevel()>0) 
			   || (pItem->GetItemClass() == ITEM_CLASS_EVENT_ITEM && (pItem->GetItemType() >= 32 && pItem->GetItemType() <= 36))
			   || (pItem->GetItemClass() == ITEM_CLASS_SLAYER_PORTAL_ITEM)
				// Sjheon 2006.03.10
			   || (pItem->GetItemClass() == ITEM_CLASS_ETHEREAL_CHAIN)
			   || (pItem->GetItemClass() == ITEM_CLASS_COMMON_QUEST_ITEM)

			   || (pItem->GetItemClass() == ITEM_CLASS_OUSTERS_HARMONIC_PENDENT)

			   || (pItem->GetItemClass() == ITEM_CLASS_EVENT_GIFT_BOX && (pItem->GetItemType() == 27))		//???????????????? ??????
			   || (pItem->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && (pItem->GetItemType() >= 20 && pItem->GetItemType() <= 23))
			   )
	   */

		// ?????? ?????? ???? ????-????;
		if(pItem->GetItemClass()	== ITEM_CLASS_EVENT_GIFT_BOX	&&
		   pItem->IsTrade()			== false						&&
		   pItem->GetItemType()		!= 1							&&
		   pItem->GetItemType()		!= 27							&&
		   pItem->GetItemType()		!= 47							&&
		   pItem->GetItemType()		!= 51							&&
		   pItem->GetItemType()		!= 53)
		{
			MItem* pFindItem = ((MItemManager*)g_pInventory)->FindItem( MEventGiftBoxItemFinder() );
			if(pFindItem != NULL)
			{
				// ???? ?????????????? ????
				return;
			}
		}
		else if(pItem->GetItemClass() == ITEM_CLASS_SUB_INVENTORY && pItem->GetEnchantLevel() > 0 ||
			pItem->GetItemMoveContorl().GetAttr(ITEMMOVE_CANNOT_TRADE)
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
			||pItem->GetCashItem()
#endif
			)
		{
			UI_PopupMessage( STRING_MESSAGE_CANNOT_TRADE );
			return;
		}

		//---------------------------------------------------------
		// ???????? OK ????????????????????????????????????.
		//---------------------------------------------------------
		if (bAcceptMyTrade)
		{	
			if (g_pTempInformation->GetMode() == TempInformation::MODE_NULL
				&& g_pPlayer->IsItemCheckBufferNULL())
			{
				g_pTempInformation->Value1 = pItem->GetID();
				g_pTempInformation->Value2 = gridX;
				g_pTempInformation->Value3 = gridY;
				g_pTempInformation->pValue = pItem;

				//-----------------------------------------------------------
				// ???????? ???? ??????????--> ?????
				//-----------------------------------------------------------
				if (pItem->IsTrade())
				{
					g_pTempInformation->SetMode(TempInformation::MODE_TRADE_VERIFY_REMOVE_ITEM);

					g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_TRADE_REMOVE );

						CGTradeRemoveItem _CGTradeRemoveItem;
						_CGTradeRemoveItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
						_CGTradeRemoveItem.setItemObjectID( pItem->GetID() );

						g_pSocket->sendPacket( &_CGTradeRemoveItem );

						g_pTradeManager->SetNextAcceptTime();

					// [??????] ???? ????- ???????????????
//					__BEGIN_HELP_EVENT
////						ExecuteHelpEvent( HE_TRADE_ITEM_REMOVE );	
//					__END_HELP_EVENT
				}
				//-----------------------------------------------------------
				// ????????????????
				//-----------------------------------------------------------
				else
				{
						g_pTempInformation->SetMode(TempInformation::MODE_TRADE_VERIFY_ADD_ITEM);
						g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_TRADE_ADD );
						g_pTempInformation->pValue_Cannot_ItemExchange	= pItem;						//2008.09.25 shootkj	?????????? ????

						CGTradeAddItem _CGTradeAddItem;
						_CGTradeAddItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
						_CGTradeAddItem.setItemObjectID( pItem->GetID() );

						g_pSocket->sendPacket( &_CGTradeAddItem );
						

					// [??????] ???? ????- ????????????????
//					__BEGIN_HELP_EVENT
////						ExecuteHelpEvent( HE_TRADE_ITEM_ADD );	
//					__END_HELP_EVENT
				}				
			}			
		}
		//---------------------------------------------------------
		// OK ?????????????? ?????? ????.
		//---------------------------------------------------------
		else
		{		
			//-----------------------------------------------------------
			// ???????? ???? ??????????--> ?????
			//-----------------------------------------------------------
			if (pItem->IsTrade())
			{
				//-----------------------------------------------------------
				// ?????? ???????.. OK?????
				//-----------------------------------------------------------
				g_pTradeManager->RefuseOtherTrade();
				g_pTradeManager->RefuseMyTrade();

				pItem->UnSetTrade();		

					CGTradeRemoveItem _CGTradeRemoveItem;
					_CGTradeRemoveItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
					_CGTradeRemoveItem.setItemObjectID( pItem->GetID() );

					g_pSocket->sendPacket( &_CGTradeRemoveItem );

					g_pTradeManager->SetNextAcceptTime();

				// [??????] ???? ????- ???????????????
//				__BEGIN_HELP_EVENT
////					ExecuteHelpEvent( HE_TRADE_ITEM_REMOVE );	
//				__END_HELP_EVENT
			}
			//-----------------------------------------------------------
			// ???????? ?????? ???? ??????????--> ??????
			//-----------------------------------------------------------
			else
			{
				// ?????????????????????????????????? ??????
				if (pItem->GetItemClass()==ITEM_CLASS_EVENT_GIFT_BOX
					&& pItem->GetItemType()==1)
				{
				}
				else
				{
					// ?????????????????????? ??????????????????.
					if (pItem->GetItemClass()==ITEM_CLASS_EVENT_GIFT_BOX)// && pItem->GetItemClass() < 2)
					{
						g_pTempInformation->SetMode(TempInformation::MODE_TRADE_VERIFY_ADD_ITEM);
						g_pTempInformation->pValue	= pItem;
						
						g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_TRADE_ADD );
					}
					// ???????????????? ??????????????.
					else
					{
						//-----------------------------------------------------------
						// ?????? ???????.. OK?????
						//-----------------------------------------------------------
						g_pTradeManager->RefuseOtherTrade();
						g_pTradeManager->RefuseMyTrade();

						pItem->SetTrade();		
					}
						g_pTempInformation->SetMode(TempInformation::MODE_TRADE_VERIFY_ADD_ITEM);
						g_pTempInformation->pValue_Cannot_ItemExchange	= pItem;						//2008.09.25 shootkj	?????????? ????

						CGTradeAddItem _CGTradeAddItem;
						_CGTradeAddItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
						_CGTradeAddItem.setItemObjectID( pItem->GetID() );

						g_pSocket->sendPacket( &_CGTradeAddItem );
						

					// [??????] ???? ????- ????????????????
//					__BEGIN_HELP_EVENT
////						ExecuteHelpEvent( HE_TRADE_ITEM_ADD );	
//					__END_HELP_EVENT
				}
			}	
		}
	}
	else
	{
		DEBUG_ADD( "[Error] TradeManager is NULL");
	}
}


//-----------------------------------------------------------------------------
//
// ???? ????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_OK_EXCHANGE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_OK_EXCHANGE");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	if (g_pTradeManager!=NULL
		&& g_pTradeManager->IsAcceptTime())
	{
		MItem* pMouseItem = UI_GetMouseItem();

		//--------------------------------------------------------
		// ???????????????????????
		// ???????????????????????????????????????????
		//--------------------------------------------------------
		if ((pMouseItem==NULL || pMouseItem->GetItemClass()!=ITEM_CLASS_EVENT_GIFT_BOX)
			&& g_pTradeManager->CanTrade())
		{
			g_pTradeManager->AcceptMyTrade();
			
				CGTradeFinish _CGTradeFinish;
				_CGTradeFinish.setTargetObjectID( g_pTradeManager->GetOtherID() );
				_CGTradeFinish.setCode( CG_TRADE_FINISH_ACCEPT );

				g_pSocket->sendPacket( &_CGTradeFinish );
				
		}
		//--------------------------------------------------------
		// ?????????????????? ?????
		//--------------------------------------------------------
		else
		{
			// ????????????????????.
			// ????????. ?????? ??????????????????.
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_TRADE_NO_SPACE].GetString() );
		}
	}
	else
	{
		DEBUG_ADD( "[Error] TradeManager is NULL or NotAcceptTime");
	}
}

//-----------------------------------------------------------------------------
//
// ???? ?????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CANCEL_EXCHANGE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CANCEL_EXCHANGE");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	BOOL bSendPacket = FALSE;

	if (g_pTradeManager!=NULL)
	{	
		BOOL bAcceptMyTrade = g_pTradeManager->IsAcceptMyTrade();

		//---------------------------------------------------------------
		// OK ?????? ??????????????????????????????????.
		//---------------------------------------------------------------
		if (bAcceptMyTrade)
		{
			if (g_pTempInformation->GetMode() == TempInformation::MODE_NULL)
			{
				g_pTempInformation->SetMode(g_pTempInformation->MODE_TRADE_VERIFY_CANCEL);

				bSendPacket = TRUE;
			}
			else
			{
				DEBUG_ADD( "[Error] Mode is not MODE_NULL");
				// 2009.08.18 neclipse ????..
				// ?????? ?????? : ?????????????? ????????????????????????????????????????????? ???????????? ??????
				g_pTradeManager->RefuseMyTrade();
			
				bSendPacket = TRUE;
			}
		}
		//---------------------------------------------------------------
		// ???? ???? (?????? ?????????? ?????????- -;;)
		//---------------------------------------------------------------
		else
		{		
			//---------------------------------------------------------------
			// ???? ??????.
			//---------------------------------------------------------------
			g_pTradeManager->RefuseMyTrade();
			
			bSendPacket = TRUE;
		}
	}	
	else
	{
		DEBUG_ADD( "[Error] TradeManager is NULL");
	}

	//---------------------------------------------------------------
	// ????OK?????????????packet
	//---------------------------------------------------------------
	if (bSendPacket)
	{
			CGTradeFinish _CGTradeFinish;
			_CGTradeFinish.setTargetObjectID( g_pTradeManager->GetOtherID() );
			_CGTradeFinish.setCode( CG_TRADE_FINISH_RECONSIDER );

			g_pSocket->sendPacket( &_CGTradeFinish );
			
	}
}

//-----------------------------------------------------------------------------
//
// ??????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLOSE_EXCHANGE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CLOSE_EXCHANGE");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	BOOL bAcceptMyTrade = (g_pTradeManager!=NULL && g_pTradeManager->IsAcceptMyTrade());

	BOOL bSendPacket = FALSE;

	int otherID;

	//---------------------------------------------------------------
	// OK ?????? ??????????????????????????????????.
	//---------------------------------------------------------------
	if (bAcceptMyTrade)
	{
		if (g_pTempInformation->GetMode() == TempInformation::MODE_NULL)
		{
			g_pTempInformation->SetMode(g_pTempInformation->MODE_TRADE_VERIFY_CLOSE);
			
			otherID = g_pTradeManager->GetOtherID();

			bSendPacket = TRUE;
		}
		else
		{
			DEBUG_ADD( "[Error] Mode is not MODE_NULL");

			// 2009.08.18 neclipse ????..
			// ?????? ?????? : ?????????????? ????????????????????????????????????????????? ???????????? ??????
			if (g_pTradeManager!=NULL)
			{			
				otherID = g_pTradeManager->GetOtherID();

				g_pTradeManager->CancelTrade();

				bSendPacket = TRUE;
			}

			//---------------------------------------------------------------
			// ???? ???????~~ ???~????
			//---------------------------------------------------------------
			UI_CloseExchange();	
		}
	}
	//---------------------------------------------------------------
	// ???? ?????.
	//---------------------------------------------------------------
	else
	{		
		if (g_pTradeManager!=NULL)
		{			
			otherID = g_pTradeManager->GetOtherID();

			g_pTradeManager->CancelTrade();

			bSendPacket = TRUE;
		}

		//---------------------------------------------------------------
		// ???? ???????~~ ???~????
		//---------------------------------------------------------------
		UI_CloseExchange();	
	}
	
	//---------------------------------------------------------------
	// ??????????????????packet
	//---------------------------------------------------------------
	if (bSendPacket)
	{
			CGTradeFinish _CGTradeFinish;
			_CGTradeFinish.setTargetObjectID( otherID );
			_CGTradeFinish.setCode( CG_TRADE_FINISH_REJECT );

			g_pSocket->sendPacket( &_CGTradeFinish );
	}
}

//-----------------------------------------------------------------------------
//
// ???????????????? ??????
//
//-----------------------------------------------------------------------------
// left = ??????????????
// void_ptr = ????????????!=NULL)
/*
void
UIMessageManager::Execute_UI_CHAT_SELECT_NAME(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CHAT_SELECT_NAME");
	
	char str[80];

	const char* pID = (const char*)void_ptr;
		
	switch (left)
	{
		case M_LEFTBUTTON_DOWN :
			sprintf(str, "/%s ", pID);
			gC_vs_ui.SetInputString( str );
		break;

		case M_RIGHTBUTTON_DOWN :
			#ifdef OUTPUT_DEBUG
				sprintf(str, "*trace %s", pID);
				gC_vs_ui.SetInputString( str );
			#else
				gC_vs_ui.AddInputString( pID );
			#endif
		break;
	}
}
*/

//-----------------------------------------------------------------------------
//
// ????????? Y/N?????????? ??????
//
//-----------------------------------------------------------------------------
// left = (BOOL)??????????
//
void
UIMessageManager::Execute_UI_EXCHANGE_ACCEPT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_EXCHANGE_ACCEPT");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	BOOL accept = (BOOL)left;

	int code;

	//---------------------------------------------------------
	// ????????? Y/N?????????? ??????
	//---------------------------------------------------------
	if (g_pTempInformation->GetMode() == TempInformation::MODE_TRADE_REQUEST)
	{
		//---------------------------------------------------------
		// ?????
		//---------------------------------------------------------
		if (accept)
		{
			code = CG_TRADE_PREPARE_CODE_ACCEPT;

			//---------------------------------------------------------
			// ???????? ??????.
			//---------------------------------------------------------
			UI_RunExchange( g_pTempInformation->Value1 );	// otherID ??????

			CGRequestInfo _CGRequestInfo;
			_CGRequestInfo.setValue( g_pTempInformation->Value1 );
			_CGRequestInfo.setCode( CGRequestInfo::REQUEST_CHARACTER_INFO );
			
			g_pSocket->sendPacket( &_CGRequestInfo );

			MCreature* pCreature = g_pZone->GetCreature(g_pTempInformation->Value1);
			if(pCreature != NULL)
				UI_RunOtherInfo(pCreature);
		}
		//---------------------------------------------------------
		// ????
		//---------------------------------------------------------
		else
		{	
			if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_TRADE)
			{
				g_pPlayer->SetWaitVerifyNULL();
			}

			code = CG_TRADE_PREPARE_CODE_REJECT;
		}

			CGTradePrepare _CGTradePrepare;
			_CGTradePrepare.setTargetObjectID( g_pTempInformation->Value1 );
			_CGTradePrepare.setCode( code );				

			g_pSocket->sendPacket( &_CGTradePrepare );			
	
		g_pTempInformation->SetMode(TempInformation::MODE_NULL);		
	}	
}

//-----------------------------------------------------------------------------
//
// ?????????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_EXCHANGE_REQUEST_CANCEL(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_EXCHANGE_REQUEST_CANCEL");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	//---------------------------------------------------------
	// ????????? Y/N?????????? ??????
	//---------------------------------------------------------
	if (g_pTempInformation->GetMode() == TempInformation::MODE_TRADE_REQUEST)
	{	
		if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_TRADE)
		{
			g_pPlayer->SetWaitVerifyNULL();
		}

			CGTradePrepare _CGTradePrepare;
			_CGTradePrepare.setTargetObjectID( g_pTempInformation->Value1 );
			_CGTradePrepare.setCode( CG_TRADE_PREPARE_CODE_CANCEL );				

			g_pSocket->sendPacket( &_CGTradePrepare );			
	
		g_pTempInformation->SetMode(TempInformation::MODE_NULL);
		
		// ???? ???????????????????????.
		UI_CloseExchangeCancel();
	}	
}

/*
//-----------------------------------------------------------------------------
//
// Game Option ??????
//
//-----------------------------------------------------------------------------
// left = ??????option
// right = ????
void
UIMessageManager::Execute_UI_CHANGE_GAME_OPTION(int left, int right, void* void_ptr)
{
	DEBUG_ADD_FORMAT("[UI] Execute_UI_CHANGE_GAME_OPTION[%d] = %d", left, right);
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	int value = right;
	
	switch (left)
	{
		//-------------------------------------------------------------------
		// DRAW_MINIMAP
		//-------------------------------------------------------------------		
		//case C_VS_UI_GAMEMENU_OPTION::DRAW_MINIMAP :
		//	g_pUserOption->DrawMinimap = (BOOL)value;
		//break;

		//-------------------------------------------------------------------
		// DRAW_ZONE_NAME
		//-------------------------------------------------------------------
		//case C_VS_UI_GAMEMENU_OPTION::DRAW_ZONE_NAME :
		//	g_pUserOption->DrawZoneName = (BOOL)value;
		//break;

		//-------------------------------------------------------------------
		// DRAW_GAME_TIME
		//-------------------------------------------------------------------
		//case C_VS_UI_GAMEMENU_OPTION::DRAW_GAME_TIME :
		//	g_pUserOption->DrawGameTime = (BOOL)value;
		//break;

		//-------------------------------------------------------------------
		// DRAW_BLENDING_CHARACTER_SHADOW
		//-------------------------------------------------------------------
		//case C_VS_UI_GAMEMENU_OPTION::DRAW_BLENDING_CHARACTER_SHADOW :
		//	///g_pUserOption->BlendingShadow = (BOOL)value;
		//break;

		//-------------------------------------------------------------------
		// PLAY_WAV
		//-------------------------------------------------------------------
		case C_VS_UI_GAMEMENU_OPTION::PLAY_WAV :
			g_pUserOption->PlayWaveMusic = (BOOL)value;

			PlayGameMusic();			
		break;

		//-------------------------------------------------------------------
		// BLOOD_DROP
		//-------------------------------------------------------------------
		case C_VS_UI_GAMEMENU_OPTION::BLOOD_DROP :
			g_pUserOption->BloodDrop = (BOOL)value;
		break;

		//-------------------------------------------------------------------
		// PLAY_SOUND
		//-------------------------------------------------------------------
		case C_VS_UI_GAMEMENU_OPTION::PLAY_SOUND :
			g_pUserOption->PlaySound = (BOOL)value;

			if (g_pUserOption->PlaySound)
			{
				g_DXSound.UnSetMute();
			}
			else
			{
				if (g_pSoundManager != NULL)
				{
					g_pSoundManager->Stop();
				}
				
				g_DXSound.SetMute();
			}
		break;

		//-------------------------------------------------------------------
		// PLAY_MUSIC
		//-------------------------------------------------------------------
		case C_VS_UI_GAMEMENU_OPTION::PLAY_MUSIC :
			g_pUserOption->PlayMusic = (BOOL)value;

			PlayGameMusic();
		break;

		//-------------------------------------------------------------------
		// VOLUME_SOUND
		//-------------------------------------------------------------------
		case C_VS_UI_GAMEMENU_OPTION::VOLUME_SOUND :
			if (PhoenixIsAudioSliderValue(value))
			{
				g_pUserOption->VolumeSound = value;
				g_DXSound.SetVolumeLimitFromSlider(value);
			}
		break;

		//-------------------------------------------------------------------
		// VOLUME_MUSIC 
		//-------------------------------------------------------------------
		case C_VS_UI_GAMEMENU_OPTION::VOLUME_MUSIC :

			if (PhoenixIsAudioSliderValue(value))
			{
				g_pUserOption->VolumeMusic = value;

				if (g_pUserOption->PlayWaveMusic)
				{
					DEBUG_ADD_FORMAT("Change VOLUME_MUSIC = %d", value);

					//LONG maxVolume = g_DXSound.GetVolumeLimit();

					//g_DXSound.SetVolumeLimit( volume );
					//g_DXSound.AddVolume( g_pDXSoundStream->GetBuffer(), 0 );
					BGMManager::Instance()->SetVolumeFromSlider(value);

					// ??????????max volume?????? ??????????
					//g_DXSound.SetVolumeLimit( maxVolume );
				}
				else
				{
					WORD volume = BGMManager::SliderValueToMidiWord(value);

					if (!g_pUserOption->PlayMusic || g_Music.IsPause())
					{
						g_Music.SetVolume( volume );
					}
					else
					{
						g_Music.Pause();
						g_Music.SetVolume( volume );
						g_Music.Resume();
					}
				}
			}
		break;

		//-------------------------------------------------------------------
		// ????????HP??????????/??????????
		//-------------------------------------------------------------------
		case C_VS_UI_GAMEMENU_OPTION::PARTY_HPBAR_ALPHA :
			g_pUserOption->DrawTransHPBar = (BOOL)value;
		break;
		
	}
}


//-----------------------------------------------------------------------------
//
// GameOption ????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLOSE_GAME_OPTION(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_GAME_OPTION");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	gC_vs_ui.CloseGameMenuOption();

	//---------------------------------------------------------------
	// ???? ????????????????. - -;
	//---------------------------------------------------------------
	g_pUserOption->SaveToFile( FILE_INFO_USEROPTION );
}
*/

//-----------------------------------------------------------------------------
//
// Title Option ??????
//
//-----------------------------------------------------------------------------
// left = ??????option
// right = ????
void
UIMessageManager::Execute_UI_CHANGE_OPTION(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CHANGE_TITLE_OPTION");
	CT_Done("[UI-CHANGE_OPT] ENTER left=%d right=%d g_pUserOption=%p", left, right, (void*)g_pUserOption);

//	if (g_Mode!=MODE_MAINMENU)
//	{
//		DEBUG_ADD("Not Mode MODE_MAINMENU");
//		return;
//	}

	
	int value = right;

	switch (left)
	{
		//-------------------------------------------------------------------
		// USE 3D HAL
		//-------------------------------------------------------------------
//		case C_VS_UI_OPTION::CHECK_3D :
//			g_pUserOption->Use3DHAL = (BOOL)value;
//		break;

		//-------------------------------------------------------------------
		// DRAW_MINIMAP
		//-------------------------------------------------------------------
		//case C_VS_UI_OPTION::DRAW_MINIMAP :
		//	g_pUserOption->DrawMinimap = (BOOL)value;
		//break;

		//-------------------------------------------------------------------
		// DRAW_ZONE_NAME
		//-------------------------------------------------------------------
		//case C_VS_UI_OPTION::DRAW_ZONE_NAME :
		//	g_pUserOption->DrawZoneName = (BOOL)value;
		//break;

		//-------------------------------------------------------------------
		// DRAW_GAME_TIME
		//-------------------------------------------------------------------
		//case C_VS_UI_OPTION::DRAW_GAME_TIME :
		//	g_pUserOption->DrawGameTime = (BOOL)value;
		//break;

		//-------------------------------------------------------------------
		// DRAW_BLENDING_CHARACTER_SHADOW
		//-------------------------------------------------------------------
		//case C_VS_UI_OPTION::DRAW_BLENDING_CHARACTER_SHADOW :
	//		g_pUserOption->BlendingShadow = (BOOL)value;
		//break;

		//-------------------------------------------------------------------
		// PLAY_WAV
		//-------------------------------------------------------------------
		case C_VS_UI_OPTION::CHECK_WAV :
			g_pUserOption->PlayWaveMusic = (BOOL)value;

			if(g_Mode == MODE_MAINMENU)
				PlayTitleMusic();			
			else 
				PlayGameMusic();			
		break;

		//-------------------------------------------------------------------
		// BLOOD_DROP
		//-------------------------------------------------------------------
//		case C_VS_UI_OPTION::BLOOD_DROP :
//			g_pUserOption->BloodDrop = (BOOL)value;
//		break;

		//-------------------------------------------------------------------
		// PLAY_SOUND
		//-------------------------------------------------------------------
		case C_VS_UI_OPTION::CHECK_SOUND :
			g_pUserOption->PlaySound = (BOOL)value;

			if (g_pUserOption->PlaySound)
			{
				g_DXSound.UnSetMute();
				g_pSystemMessage->Add((*g_pGameStringTable)[STRING_MESSAGE_EFFECTSOUND_ENABLE].GetString());
			}
			else
			{
				if (g_pSoundManager != NULL)
				{
					g_pSoundManager->Stop();
				}
				
				g_DXSound.SetMute();
				g_pSystemMessage->Add((*g_pGameStringTable)[STRING_MESSAGE_EFFECTSOUND_DISABLE].GetString());
			}
		break;

		//-------------------------------------------------------------------
		// PLAY_MUSIC
		//-------------------------------------------------------------------
		case C_VS_UI_OPTION::CHECK_MUSIC :
			g_pUserOption->PlayMusic = (BOOL)value;

			//Wlzzi ??????. -- ?????????????? 
			//				: Music Off ????????, ???? On ???????? ?????? ?????? ????????????????. ???????? ????????
			//				: ??????????. ???? g_pUserOption->PlayWaveMusic ?????? ?? ???? ??????.
			if( g_Mode == MODE_GAME && g_pUserOption->PlayMusic)
			{
				PlayMusicCurrentZone();
			}
			else
			{
				PlayTitleMusic();
				if(value)
				{
					g_pSystemMessage->Add((*g_pGameStringTable)[STRING_MESSAGE_BACKGROUNDSOUND_ENABLE].GetString());
				}
				else
				{
					g_pSystemMessage->Add((*g_pGameStringTable)[STRING_MESSAGE_BACKGROUNDSOUND_DISABLE].GetString());
				}
			}
		break;

		//-------------------------------------------------------------------
		// VOLUME_SOUND
		//-------------------------------------------------------------------
		case C_VS_UI_OPTION::CHECK_VALUE_SOUND_VOLUME :
			if (PhoenixIsAudioSliderValue(value))
			{
				g_pUserOption->VolumeSound = value;
				g_DXSound.SetVolumeLimitFromSlider(value);

				PlaySound(SOUND_SLAYER_BUTTON);
			}
		break;

		//-------------------------------------------------------------------
		// VOLUME_MUSIC 
		//-------------------------------------------------------------------
		case C_VS_UI_OPTION::CHECK_VALUE_MUSIC_VOLUME :

			CT_Done("[UI-CHANGE_OPT] CHECK_VALUE_MUSIC_VOLUME value=%d (prev VolumeMusic=%d)",
				value, g_pUserOption ? g_pUserOption->VolumeMusic : -999);
			if (PhoenixIsAudioSliderValue(value))
			{
				g_pUserOption->VolumeMusic = value;
				CT_Done("[UI-CHANGE_OPT] VolumeMusic WRITTEN=%d", g_pUserOption->VolumeMusic);

				if (g_pUserOption->PlayWaveMusic)
				{
					/*
					LONG volume = value*SOUND_DEGREE + SOUND_MIN;

					LONG maxVolume = g_DXSound.GetVolumeLimit();

					g_DXSound.SetVolumeLimit( volume );
					g_DXSound.AddVolume( g_pDXSoundStream->GetBuffer(), 0 );

					// ??????????max volume?????? ??????????
					g_DXSound.SetVolumeLimit( maxVolume );
					*/
//					LONG volume = PhoenixSliderValueToMp3Volume(value);

//					DEBUG_ADD_FORMAT("Change VOLUME_MUSIC = %ld", volume);
					
//					g_pDXSoundStream->SetVolumeLimit( volume );
					DEBUG_ADD("MP3 SetVolume4");
#ifdef __USE_MP3__
					LONG volume = PhoenixSliderValueToMp3Volume(value);
					g_pMP3->SetVolume( volume );
#else
					//if( g_DXSound.IsInit() && g_pOGG != NULL )
					if(g_DXSound.IsInit())
					{
						//g_pOGG->streamVolume( max( -10000, min( -1, volume ) ) );
						BGMManager::Instance()->SetVolumeFromSlider(value);
					}
#endif
					DEBUG_ADD("MP3 SetVolume4 OK");
				}
				else
				{
					WORD volume = BGMManager::SliderValueToMidiWord(value);

					if (!g_pUserOption->PlayMusic || g_Music.IsPause())
					{
						g_Music.SetVolume( volume );
					}
					else
					{
						g_Music.Pause();
						g_Music.SetVolume( volume );
						g_Music.Resume();
					}				
				}
			}
		break;

		//-------------------------------------------------------------------
		// ????????HP??????????/??????????
		//-------------------------------------------------------------------
//		case C_VS_UI_OPTION::PARTY_HPBAR_ALPHA :
//			g_pUserOption->DrawTransHPBar = (BOOL)value;
//		break;
	}
}


//-----------------------------------------------------------------------------
//
// Option ????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLOSE_OPTION(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_TITLE_OPTION");
	CT_Done("[UI] Execute_UI_CLOSE_OPTION ENTER g_Mode=%d", (int)g_Mode);

//	if (g_Mode!=MODE_MAINMENU)
//	{
//		DEBUG_ADD("Not Mode MODE_MAINMENU");
//		return;
//	}


	gC_vs_ui.CloseOption();
	CT_Done("[UI] Execute_UI_CLOSE_OPTION after CloseOption()");

	//---------------------------------------------------------------
	// ???? ????????????????. - -;
	//---------------------------------------------------------------
	CT_Done("[UI-CLOSE_OPT] PRE-SAVE VolumeMusic=%d VolumeSound=%d PlayMusic=%d PlaySound=%d path='%s'",
		g_pUserOption ? g_pUserOption->VolumeMusic : -999,
		g_pUserOption ? g_pUserOption->VolumeSound : -999,
		g_pUserOption ? g_pUserOption->PlayMusic : -999,
		g_pUserOption ? g_pUserOption->PlaySound : -999,
		g_pFileDef->getProperty("FILE_INFO_USEROPTION").c_str());
	g_pClientConfig->SaveToFile( g_pFileDef->getProperty("FILE_INFO_CLIENTCONFIG").c_str());
	g_pUserOption->SaveToFile( g_pFileDef->getProperty("FILE_INFO_USEROPTION").c_str());
	CT_Done("[UI-CLOSE_OPT] POST-SAVE");

	// FAZ 1.8 ??MODE_CHANGE_OPTION tetikleyicisi devre d????
	// Orijinal davran??: Option kapan?nca IsHAL() / Use3DHAL uyumsuzsa t?m
	// UI'yi Release+Init ediyor (g_Mode=MODE_CHANGE_OPTION=3). Modernize'da
	// dgVoodoo zaten HAL em?lasyonu sa?l?yor, bu reinit yolu (CDirectDraw
	// yeniden config, surface recreate) clean-exit veya crash'e yol a??yor.
	// FAZ 2.1'de dgVoodoo entegrasyonu oturdu?unda tekrar de?erlendirilecek.
	CT_Done("[UI] Execute_UI_CLOSE_OPTION mode-change check: g_Mode=%d IsHAL=%d Use3DHAL=%d g_bEnable3DHAL=%d",
		(int)g_Mode,
		(int)CDirect3D::IsHAL(),
		(int)(g_pUserOption ? g_pUserOption->Use3DHAL : -1),
		(int)g_bEnable3DHAL);
#if 0
	if (g_Mode==MODE_MAINMENU)
	{
		if (CDirect3D::IsHAL())
		{
			// 3D?????? ????????. ?????? ?????? ?????
			if (!g_pUserOption->Use3DHAL)
			{
				g_Mode = MODE_CHANGE_OPTION;
			}
		}
		else
		{
			// 3D?????? ??????. ?????? ???? ?????
			if (g_bEnable3DHAL && g_pUserOption->Use3DHAL)
			{
				g_Mode = MODE_CHANGE_OPTION;
			}
		}
	}
#endif
	CT_Done("[UI] Execute_UI_CLOSE_OPTION EXIT (mode-change suppressed) g_Mode=%d", (int)g_Mode);
}

//-----------------------------------------------------------------------------
//
// 2026-09-02 O3 — canli cozunurluk uygulama (left=w, right=h).
// Kuyruklu UI mesaji: pencere iterasyonu disinda calisir, bu yuzden title
// agacini + option menusunu guvenle yeniden kurabiliriz.
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_APPLY_RESOLUTION(int left, int right, void* void_ptr)
{
	const int w = left;
	const int h = right;
	CT_Done("[UI] Execute_UI_APPLY_RESOLUTION %dx%d g_Mode=%d", w, h, (int)g_Mode);

	// plan F3 — relayout icin eski cozunurluk Apply oncesi yakalanir
	const int oldW = (g_pUserInformation != NULL) ? g_pUserInformation->iResolution_x : 0;
	const int oldH = (g_pUserInformation != NULL) ? g_pUserInformation->iResolution_y : 0;

	if (!Phoenix_ResolutionLive_Apply(w, h))
	{
		CT_Done("[UI] Execute_UI_APPLY_RESOLUTION: Apply FAILED — degisiklik yok");
		return;
	}

	// Tam-ekran offscreen'i yeni boyutla yeniden kur (VS_UI cizim hedefi)
	if (gpC_base != NULL)
		gpC_base->ReinitSurfaceForResolution();

	// Title agaci construction-time koordinat saklar — komple yeniden kur.
	// Option menusu de eski merkeze gore duruyor: kapat + yeniden ac
	// (OptionRenewal.set + UserOption kaydi kapanis akisinda yapilir).
	if (g_Mode == MODE_MAINMENU)
	{
		gC_vs_ui.CloseOption();
		gC_vs_ui.StartTitle();
		gC_vs_ui.RunOption(true);
	}
	// 2026-09-03 plan F2b — oyun icinde de canli apply (Reborn parity):
	// eskiden in-game secim yalnizca resolution.inf'e yaziliyordu (restart
	// pending) ama menu label'i yeni degeri gosteriyordu → "1920 sectim,
	// 1440 kaldi" hissi. Artik ayni kuyruklu mesaj yolundan canli uygulanir;
	// dunya redraw'u Apply icindeki RestoreSurface+SetFirstDraw (F2a) yapar.
	// Option menusu eski merkeze gore durdugundan kapat + yeniden ac.
	else if (g_Mode == MODE_GAME)
	{
		// plan F3 — HUD panelleri (simpleInformation + minimap/skill grubu)
		// yeni cozunurluge gore yeniden konumlanir: per-resolution defter
		// kaydi varsa o, yoksa default anchor + clamp.
		gC_vs_ui.RelayoutHudForResolution(oldW, oldH);

		gC_vs_ui.CloseOption();
		gC_vs_ui.RunOption(false);
	}

	CT_Done("[UI] Execute_UI_APPLY_RESOLUTION OK %dx%d", w, h);
}

//-----------------------------------------------------------------------------
//
// Option Reset
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_RESET_BUTTON(int left, int right, void* void_ptr)
{
	g_pUserOption->ResetUserOption();
}

//-----------------------------------------------------------------------------
//
// Bookcase ????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLOSE_BOOKCASE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_BOOKCASE");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	UI_CloseBookcase();
}

//-----------------------------------------------------------------------------
//
// Briefing ????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLOSE_BRIEFING(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_BRIEFING");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	UI_CloseBriefing();
}

//-----------------------------------------------------------------------------
//
// Computer ????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLOSE_COMPUTER(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_COMPUTER");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	UI_CloseComputer();
}

//-----------------------------------------------------------------------------
//
// Tutorial?????? ???????????????? ?????? ???? ??????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_CLOSE_TUTORIAL_EXIT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_TUTORIAL_EXIT");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return; 
	}

	
	UI_CloseTutorialExit();

	int arms = left;	// 0: ????1: ????2: ????????3: ????????4: AR 5: TR 6: SMG 7:SG

	if (arms >= 0 && arms <=7)
	{
		ITEM_CLASS itemClass[] =
		{
			ITEM_CLASS_BLADE,
			ITEM_CLASS_SWORD,
			ITEM_CLASS_MACE,
			ITEM_CLASS_CROSS,
			ITEM_CLASS_AR,
			ITEM_CLASS_SR,
			ITEM_CLASS_SMG,
			ITEM_CLASS_SG
		};
		

			CGRequestNewbieItem _CGRequestNewbieItem;
		
			_CGRequestNewbieItem.setItemClass( itemClass[arms] );
			
			g_pSocket->sendPacket( &_CGRequestNewbieItem );
			
	}
}


//-----------------------------------------------------------------------------
//
// Desc Dialog ?????? ????
//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_CLOSE_DESC_DIALOG(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_DESC_DIALOG");

	
	//if (g_Mode!=MODE_GAME)
	{
	//	DEBUG_ADD("Not Mode MODE_GAME");
	//	return;
	}
	
	// Close	
	gC_vs_ui.CloseDescDialog();
}

//-----------------------------------------------------------------------------
//
// Elevator?????? ????
//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_CLOSE_ELEVATOR(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_ELEVATOR");
	
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	// Close
	gC_vs_ui.CloseElevator();
}

//-----------------------------------------------------------------------------
//
// Elevator ??????????????
//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_SELECT_ELEVATOR(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_SELECT_ELEVATOR");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	// left 0 : 4????....
	//		1 : 3
	//		2 : 2
	//		3 : 1
	//		4 : ?????? 1????
	int selectPortal = 4-left;

	if (selectPortal >= 0 && selectPortal <= 4)
	{
		const MSector& sector = g_pZone->GetSector(g_pPlayer->GetX(), g_pPlayer->GetY());

		if (sector.IsPortal())
		{
			int numPortal = sector.GetPortalSize();		

			if (numPortal <= 0 || selectPortal >= numPortal)
			{
				DEBUG_ADD("UI_SELECT_ELEVATOR invalid portal index");
				return;
			}

			PORTAL_LIST::const_iterator	iPortal = sector.GetPortalBegin();

			for (int i=0; i<selectPortal; i++)
			{
				iPortal++;		
			}		

			PORTAL_INFO portalInfo = *iPortal;

			int zoneID = portalInfo.ZoneID;

			// zoneID????????????.
			//-----------------------------------------------------
			// Packet ??????????
			//-----------------------------------------------------
				CGSelectPortal _CGSelectPortal;

				_CGSelectPortal.setZoneID( zoneID );

				g_pSocket->sendPacket( &_CGSelectPortal );
		}
	}

	gC_vs_ui.CloseElevator();
}

//-----------------------------------------------------------------------------
//
// Server ??????????????
//
//-----------------------------------------------------------------------------
/*
void	
UIMessageManager::Execute_UI_SELECT_SERVER(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_SELECT_SERVER");

	
	if (g_Mode!=MODE_WAIT_SELECTPC)
	{
		DEBUG_ADD("Not Mode MODE_WAIT_SELECTPC");
		return;
	}

	
	// left : serverGroupID
	int selectedGroup = left;


	//-----------------------------------------------------
	// Server?????? ??????
	//-----------------------------------------------------
	if (g_pServerInformation!=NULL)
	{
		//-----------------------------------------------------
		// ?????? ???? ?????????
		//-----------------------------------------------------
		const ServerGroup* pGroup = g_pServerInformation->GetData( selectedGroup );

		if (pGroup!=NULL)
		{
			const char* pGroupName = pGroup->GetGroupName();

			//-----------------------------------------------------
			// ?????? ???????? ???? server????????????..
			//-----------------------------------------------------
			if (pGroupName != g_pServerInformation->GetServerGroupName())
			{
				char str[80];
				strcpy(str, pGroupName);	
				
				// UI??????????
				//gC_vs_ui.SetServerDefault( str, selectedGroup );

			
				//-----------------------------------------------------
				// Packet ??????????
				//-----------------------------------------------------
				#ifdef CONNECT_SERVER
					CLChangeServer _CLChangeServer;

					_CLChangeServer.setServerGroupID( selectedGroup );

					g_pSocket->sendPacket( &_CLChangeServer );

					gC_vs_ui.CharManagerDisable();
				#endif

				// Server????????????????
				g_pServerInformation->SetServerGroupName( pGroupName );			
			}
		}
	}
}
*/

//-----------------------------------------------------------------------------
//
// Server ??????????????
//
//-----------------------------------------------------------------------------
/*
void	
UIMessageManager::Execute_UI_REQUEST_SERVER_LIST(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_REQUEST_SERVER_LIST");

	
	if (g_Mode!=MODE_WAIT_SELECTPC)
	{
		DEBUG_ADD("Not Mode MODE_WAIT_SELECTPC");
		return;
	}

	
	#ifdef CONNECT_SERVER
		if (gC_vs_ui.IsCharManagerEnable())
		{
			CLGetServerList _CLGetServerList;					
			
			g_pSocket->sendPacket( &_CLGetServerList );

			gC_vs_ui.CharManagerDisable();
		}
	#endif
}
*/

//-----------------------------------------------------------------------------
//
// Inventory --> QuickSlot
//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_ITEM_TO_QUICKITEMSLOT(int left, int right, void* void_ptr)
{
//	DEBUG_ADD("[UI] UI_ITEM_TO_QUICKITEMSLOT");
//	// focus_grid_x, focus_grid_y, (MItem *)p_item
//
//	
//	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
//	{
//		DEBUG_ADD("Not Mode MODE_GAME or Dead");
//		return;
//	}
//				
//	//-----------------------------------------------------------------
//	// ?????????????????????? ?????
//	//-----------------------------------------------------------------
//	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL
//		&& g_pPlayer->IsItemCheckBufferNULL())
//	{
//		if (gC_vs_ui.IsRunningExchange())
//		{
//			// ???? ???????????? Mouse????????????
//			Execute_UI_ITEM_PICKUP_FROM_INVENTORY(left, right, void_ptr);
//		}
//		else
//		{
//			MItem* pItem = (MItem*)void_ptr;
//
//			//---------------------------------------------------------
//			// ?????? ???????? ??????????????.
//			// QuickSlot????????????????
//			//---------------------------------------------------------
//			if (pItem->IsQuickItem() && g_pQuickSlot!=NULL)
//			{		
//				int itemID = pItem->GetID();
//				int itemX = pItem->GetGridX();
//				int itemY = pItem->GetGridY();
//
//				int num = g_pQuickSlot->GetSize();
//				int addTotal = -1;
//
//				int addIndex = -1;
//
//				//---------------------------------------------------------
//				// QuickSlot????????????????????????????
//				//---------------------------------------------------------
//				for (int i=0; i<num; i++)
//				{
//					MItem* pQuickItem = g_pQuickSlot->GetItem( i );
//
//					//---------------------------------------------------------
//					// ???????? ?????? ???????????? ??????????????.
//					//---------------------------------------------------------
//					if (pQuickItem==NULL)
//					{						
//						addIndex = i;
//
//						MItem* pRemoveItem = g_pInventory->RemoveItem( itemX, itemY );
//
//						g_pQuickSlot->AddItem( pRemoveItem, addIndex );
//
//						PlaySound( pRemoveItem->GetInventorySoundID() );
//
//						break;
//					}
//					//---------------------------------------------------------
//					// ?????? ??????????. ?????????????? ????????????????????.
//					//---------------------------------------------------------
//					else
//					{
//						//--------------------------------------------------------
//						// ?????? ???????? item?????? ??????????????.
//						//--------------------------------------------------------
//						if (pQuickItem->GetItemClass()==pItem->GetItemClass()
//							&& pQuickItem->GetItemType()==pItem->GetItemType())
//						{
//							//----------------------------------------------------
//							// ?????? ??????????max?????????? ??????????.
//							//----------------------------------------------------
//							addTotal = pQuickItem->GetNumber() + pItem->GetNumber();
//							if ( addTotal <= pQuickItem->GetMaxNumber() )
//							{
//								// i???????????? ????????????????????.								
//								addIndex = i;
//
//								//---------------------------------------------------
//								// Inventory???????????????? ??????????.
//								//---------------------------------------------------
//								MItem* pRemoveItem = g_pInventory->RemoveItem( itemX, itemY );
//								if (pRemoveItem!=NULL)
//								{
//									// assert( pRemoveItem==pItem );
//									delete pRemoveItem;
//								}
//
//								pQuickItem->SetNumber( addTotal );
//
//								PlaySound( pQuickItem->GetInventorySoundID() );
//								
//								break;
//							}
//						}
//					}
//				}
//
//				if (addIndex != -1)
//				{
//					//---------------------------------------------------------
//					// SendPacket - Inventory to QuickItem
//					// ?????????? Inventory --> Mouse --> QuickItem????.
//					//---------------------------------------------------------
//						//---------------------------------------------------
//						// Inventory???????? item????mouse????????????-_-;)????
//						// server????packet??????????????
//						//---------------------------------------------------
//						CGAddInventoryToMouse _CGAddInventoryToMouse;
//						_CGAddInventoryToMouse.setObjectID( itemID );
//						_CGAddInventoryToMouse.setX( itemX );
//						_CGAddInventoryToMouse.setY( itemY );
//						
//						g_pSocket->sendPacket( &_CGAddInventoryToMouse );
//
//						//---------------------------------------------------
//						// mouse???????? item????QuickSlot??????????????
//						// server????packet??????????????
//						//---------------------------------------------------
//						CGAddMouseToQuickSlot _CGAddMouseToQuickSlot;
//						_CGAddMouseToQuickSlot.setObjectID( itemID );
//						_CGAddMouseToQuickSlot.setSlotID( addIndex );
//						
//						g_pSocket->sendPacket( &_CGAddMouseToQuickSlot );
//						
//				}
//
//			}
//			//---------------------------------------------------------
//			// Quick slot????????????????????..
//			// Gear???????????????? -_-;
//			//---------------------------------------------------------
//			else
//			{
//				//---------------------------------------------------------
//				// ????..  mouse?????????.
//				//---------------------------------------------------------
//				g_pInventory->RemoveItem( pItem->GetID() );
//				Execute_UI_ITEM_PICKUP_FROM_INVENTORY(left, right, void_ptr);
//
//				int maxSlot;
//
//				//---------------------------------------------------------
//				// gear???????????????????????
//				//---------------------------------------------------------
//				if (g_pPlayer->IsSlayer())
//				{
//					maxSlot = MSlayerGear::MAX_GEAR_SLAYER;
//				}
//				else
//				{
//					maxSlot = MVampireGear::MAX_GEAR_VAMPIRE;
//				}
//
//				MItem* pGearItem = NULL;
//				MItem* pChangeItem = NULL;
//				int addSlot = -1;
//
//				for (int slot=0; slot<maxSlot; slot++)
//				{
//					MItem* pGearItem = NULL;
//
//					if (g_pPlayer->IsSlayer() && g_pSlayerGear->CanReplaceItem(pItem, (MSlayerGear::GEAR_SLAYER)slot, pGearItem)
//						|| g_pPlayer->IsVampire() && g_pVampireGear->CanReplaceItem(pItem, (MVampireGear::GEAR_VAMPIRE)slot, pGearItem)
//						|| g_pPlayer->IsOusters() && g_pOustersGear->CanReplaceItem(pItem, (MOustersGear::GEAR_OUSTERS)slot, pGearItem))
//					{
//						addSlot = slot;
//						pChangeItem = pGearItem;
//
//						// ???????????????? ????????????????.
//						// ?????????? ???????. ????????????????
//						if (pGearItem==NULL)
//						{
//							break;
//						}							
//					} 
//				}
//				
//				//---------------------------------------------------------
//				// gear??????????..
//				//---------------------------------------------------------
//				if (addSlot != -1)
//				{
//					Execute_UI_ITEM_DROP_TO_GEAR(addSlot, 0, pChangeItem);
//				}
//			}
//		}		
//	}
//	//-----------------------------------------------------------------
//	// ?????? ???????? ???? ???????????? ???? ?????
//	//-----------------------------------------------------------------
//	else
//	{
//		DEBUG_ADD( "[Error] There is another item in ItemCheckBuffer");
//	}
}

//-----------------------------------------------------------------------------
//
// Close Slayer Portal
//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_CLOSE_SLAYER_PORTAL(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CLOSE_SLAYER_PORTAL");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}


		CGSelectWayPoint _CGSelectWayPoint;
		
		_CGSelectWayPoint.setZoneID( 0 );
		_CGSelectWayPoint.setX( 0 );
		_CGSelectWayPoint.setY( 0 );

		g_pSocket->sendPacket( &_CGSelectWayPoint );

	g_pZone->RemoveHelicopter( g_pPlayer->GetID() );

	UI_CloseSelectWayPoint();
}

//-----------------------------------------------------------------------------
//
// Slayer Portal
//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_SLAYER_PORTAL(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_SLAYER_PORTAL");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}


	int zoneID = left;
	int zoneX = HIWORD(right);
	int zoneY = LOWORD(right);
	
	int cost = int( pow(g_pPlayer->GetLEVEL(), 1.3) * 100 );

//	if( g_pMoneyManager->GetMoney() < cost && g_pPlayer->IsOusters() )
//	{
//		UI_PopupMessage( STRING_MESSAGE_WAR_NOT_ENOUGH_MONEY );
//		return;
//	}
	
		CGSelectWayPoint _CGSelectWayPoint;
		
		_CGSelectWayPoint.setZoneID( zoneID );
		_CGSelectWayPoint.setX( zoneX );
		_CGSelectWayPoint.setY( zoneY );

		g_pSocket->sendPacket( &_CGSelectWayPoint );


	if(g_pPlayer->IsSlayer())
	{
		g_pZone->RemoveHelicopter( g_pPlayer->GetID() );
		UI_CloseSelectWayPoint();
	}
	else if(g_pPlayer->IsOusters())
		gC_vs_ui.CloseHorn();
		
}

//-----------------------------------------------------------------------------
//
// ???? ????????????????????????
//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_FINISH_REQUEST_PARTY_BUTTON(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_FINISH_REQUEST_PARTY_BUTTON");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}


	BOOL bTimeout = left;

	UI_ClosePartyRequest();

	if (bTimeout)
	{
			// ???? ???? ????????.
			CGPartyInvite _CGPartyInvite;
			_CGPartyInvite.setTargetObjectID( g_pTempInformation->PartyInviter );
			_CGPartyInvite.setCode( CG_PARTY_INVITE_REJECT );	

			g_pSocket->sendPacket( &_CGPartyInvite );		
	}
//	OutputDebugString("party 2\n");
}

//-----------------------------------------------------------------------------
//
// ???? ???????????????????
//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_PARTY_REQUEST_CANCEL(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_PARTY_REQUEST_CANCEL");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}


	
	//---------------------------------------------------------
	// ????????? Y/N?????????? ??????
	//---------------------------------------------------------
	if (g_pTempInformation->GetMode() == TempInformation::MODE_PARTY_REQUEST)
	{	
		if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_PARTY)
		{
			g_pPlayer->SetWaitVerifyNULL();
		}

			CGPartyInvite _CGPartyInvite;
			_CGPartyInvite.setTargetObjectID( g_pTempInformation->Value1 );
			_CGPartyInvite.setCode( CG_PARTY_INVITE_CANCEL );				

			g_pSocket->sendPacket( &_CGPartyInvite );			
	
		g_pTempInformation->SetMode(TempInformation::MODE_NULL);
		
		// ???? ???????????????????????.
		UI_ClosePartyCancel();
	}	
}

//-----------------------------------------------------------------------------
//
// ???? ???? ??????
//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_PARTY_ACCEPT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_PARTY_ACCEPT");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}


	BOOL accept = (BOOL)left;

	int code;

	//---------------------------------------------------------
	// ????????? Y/N?????????? ??????
	//---------------------------------------------------------
	//if (g_pTempInformation->Mode == TempInformation::MODE_PARTY_REQUEST)
	{
		//---------------------------------------------------------
		// ?????
		//---------------------------------------------------------
		if (accept)
		{
			code = CG_PARTY_INVITE_ACCEPT;
		}
		//---------------------------------------------------------
		// ????
		//---------------------------------------------------------
		else
		{	
			//if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_TRADE)
			{
			//	g_pPlayer->SetWaitVerifyNULL();
			}

			code = CG_PARTY_INVITE_REJECT;
		}

			CGPartyInvite _CGPartyInvite;
			_CGPartyInvite.setTargetObjectID( g_pTempInformation->PartyInviter );
			_CGPartyInvite.setCode( code );				

			g_pSocket->sendPacket( &_CGPartyInvite );			
	
		//g_pTempInformation->Mode = TempInformation::MODE_NULL;		
	}
}

//-----------------------------------------------------------------------------
//
// Close Party Manager
//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_CLOSE_PARTY_MANAGER(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CLOSE_PARTY_MANAGER");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}


	UI_CloseParty();
}

//-----------------------------------------------------------------------------
//
// ???? ????/????
//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_AWAY_PARTY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_AWAY_PARTY");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}


	// ???? ???? ???? 
	// left :: -1 == ????, 0~4 == ????
	BOOL bLeftParty = (left==-1);

	if (g_pPlayer->IsWaitVerifyNULL())
	{
		//-----------------------------------------------------------------
		// ?????? ???????????????????????
		//-----------------------------------------------------------------
		if (bLeftParty)
		{
			if (g_pParty->GetSize() > 0)
			{
					CGPartyLeave _CGPartyLeave;
				
					_CGPartyLeave.setTargetName( g_pUserInformation->CharacterID.GetString() );
					
					g_pSocket->sendPacket( &_CGPartyLeave );
					
					g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_PARTY_LEAVE );
			}
		}
		//-----------------------------------------------------------------
		// ?????? ???? ?????????????????? ?????
		//-----------------------------------------------------------------
		else
		{
			if (g_pParty->IsKickAvailableTime())
			{
				int kickIndex = left;

				PARTY_INFO*	pInfo = g_pParty->GetMemberInfo( kickIndex );

				if (pInfo!=NULL)
				{
						CGPartyLeave _CGPartyLeave;
					
						_CGPartyLeave.setTargetName( pInfo->Name.GetString() );
						
						g_pSocket->sendPacket( &_CGPartyLeave );			

						g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_PARTY_LEAVE );
				}
			}
			else
			{
				g_pGameMessage->Add( (*g_pGameStringTable)[STRING_MESSAGE_NO_AUTHORITY].GetString() );
			}
		}		
	}
}


//-----------------------------------------------------------------------------
//
// ???? ????/????
//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_FINISH_REQUEST_DIE_BUTTON(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_FINISH_REQUEST_DIE_BUTTON");

	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}


	// ??????-_-???? ???? left == TRUE : timeout
	BOOL bTimeout = left;

	if (g_pPlayer->IsDead() && !g_pPlayer->IsWaitVerify())
	{
		int TempZoneID = g_pPlayer->GetResurrectZoneID();
		if(TempZoneID)
		{
			CGSelectWayPoint _CGSelectWayPoint;

			_CGSelectWayPoint.setZoneID( TempZoneID );
			_CGSelectWayPoint.setX( 0 );
			_CGSelectWayPoint.setY( 0 );

			g_pSocket->sendPacket( &_CGSelectWayPoint );

			g_pPlayer->SetResurrectZoneID(0);
		}
		else
		{
			CGResurrect _CGResurrect;
			g_pSocket->sendPacket( &_CGResurrect );
		}	
			// ???????? Verify???????? ????????????????
			// ?????????????? ???? ????... - -;
		g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_RESURRECT );

		gC_vs_ui.FinishRequestDie();
		gC_vs_ui.FinishRequestResurrect();
		gC_vs_ui.CloseRequestShrineMinimap();
		gC_vs_ui.CloseTimeCount();
	}
}

//-----------------------------------------------------------------------------
//
// ?????? ??????????????
//
//-----------------------------------------------------------------------------

#if __CONTENTS(__TITLE_UI_RENWEAL)
void	UIMessageManager::Execute_UI_CONNECT_SERVER(int left, int right, void* void_ptr)
{
//	DEBUG_ADD_FORMAT("[UI] UI_CONNECT_SERVER. %s(%d)", (left? "group" : "server"), right);

	// left == true : group
	//         false : server
    //  right : id

	if(NULL == g_pSocket)
		return;
	
	BOOL bSelectGroup = left;
	int	selectID = right;

	//-------------------------------------------------------
	// World???????????????????
	//-------------------------------------------------------
	if (bSelectGroup)
	{
//		if (g_Mode==MODE_WAIT_SELECT_WORLD)
		{
			// CLSelectWorld
				CLSelectWorld clSelectWorld;
			
				clSelectWorld.setWorldID( selectID );				
				g_pUserInformation->WorldID = selectID;

				g_pSocket->sendPacket( &clSelectWorld );				

			g_pServerInformation->SetServerGroupID( selectID );

			SetMode( MODE_WAIT_SERVER_LIST );
		}
//		else
		{
//			DEBUG_ADD("Not MODE_WAIT_SELECT_WORLD");
		}
	}
	//-------------------------------------------------------
	// Server???????????????????
	//-------------------------------------------------------
	else
	{
		if (g_Mode==MODE_WAIT_SELECT_SERVER)
		{
			// CLSelectServer
				CLSelectServer clSelectServer;
			
				clSelectServer.setServerGroupID( selectID );				
				g_pUserInformation->ServerID = selectID;

				g_pSocket->sendPacket( &clSelectServer );				

				g_pSocket->setPlayerStatus( CPS_AFTER_SENDING_CL_GET_PC_LIST );

			g_pServerInformation->SetServerID( selectID );			

			SetMode( MODE_WAIT_PCLIST );
			//20081224 ?????????????????? ?????? ?????????? ???? ?????? ?????????????.
			Sleep(500);
		}
		else
		{
			DEBUG_ADD("Not MODE_WAIT_SELECT_SERVER");
		}
	}
		
}

#else// __TITLE_UI_RENWEAL

void UIMessageManager::Execute_UI_CONNECT_SERVER(int left, int right, void* void_ptr)
{
//	DEBUG_ADD_FORMAT("[UI] UI_CONNECT_SERVER. %s(%d)", (left? "group" : "server"), right);

	// left == true : group
	//         false : server
    //  right : id

	if(NULL == g_pSocket)
		return;
	
	BOOL bSelectGroup = left;
	int	selectID = right;

	//-------------------------------------------------------
	// World???????????????????
	//-------------------------------------------------------
	if (bSelectGroup)
	{
		if (g_Mode==MODE_WAIT_SELECT_WORLD)
		{
			// CLSelectWorld
				CLSelectWorld clSelectWorld;
			
				clSelectWorld.setWorldID( selectID );				
				g_pUserInformation->WorldID = selectID;

				g_pSocket->sendPacket( &clSelectWorld );				

			g_pServerInformation->SetServerGroupID( selectID );

			SetMode( MODE_WAIT_SERVER_LIST );
		}
		else
		{
			DEBUG_ADD("Not MODE_WAIT_SELECT_WORLD");
		}
	}
	//-------------------------------------------------------
	// Server???????????????????
	//-------------------------------------------------------
	else
	{
		if (g_Mode==MODE_WAIT_SELECT_SERVER)
		{
			// CLSelectServer
				CLSelectServer clSelectServer;
			
				clSelectServer.setServerGroupID( selectID );				
				g_pUserInformation->ServerID = selectID;

				g_pSocket->sendPacket( &clSelectServer );				

				g_pSocket->setPlayerStatus( CPS_AFTER_SENDING_CL_GET_PC_LIST );

			g_pServerInformation->SetServerID( selectID );			

			SetMode( MODE_WAIT_PCLIST );
		}
		else
		{
			DEBUG_ADD("Not MODE_WAIT_SELECT_SERVER");
		}
	}
		
}

#endif //__TITLE_UI_RENWEAL

//-----------------------------------------------------------------------------
//
// server ?????? ?????? ????
//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_CLOSE_SERVER_SELECT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CLOSE_SERVER_SELECT");

	// left == true : group 
	//         false : server
	BOOL bSelectGroup = left;

	//------------------------------------------------------------
	// group?????????? back???? ??????????????
	//------------------------------------------------------------
	if (bSelectGroup)
	{
		SetMode( MODE_MAINMENU );
	}
	//------------------------------------------------------------
	// server?????????? back???? group????????????..
	//------------------------------------------------------------
	else
	{
		UI_SetWorldList();

		SetMode( MODE_WAIT_SELECT_WORLD );
	}	
}

//-----------------------------------------------------------------------------
//
// ???????????? ????????
//
//-----------------------------------------------------------------------------
void	
UIMessageManager::Execute_UI_NEWCHARACTER_CHECK(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_NEWCHARACTER_CHECK");

	char* pName = (char*)void_ptr;

	if (pName!=NULL)
	{
		if (g_pUserInformation->WhisperID.GetLength()==0)
		{
			//---------------------------------------------
			// ???? ?????? ??
			//---------------------------------------------
			BOOL bAllOK = TRUE;

			int len = strlen(pName);	
			
			if (len<PlayerInfo::minIDLength || len>PlayerInfo::maxIDLength)
			{
				char strTemp[128];
				sprintf(strTemp, (*g_pGameStringTable)[STRING_USER_REGISTER_ID_LENGTH].GetString(), PlayerInfo::minIDLength, PlayerInfo::maxIDLength);
				g_pUIDialog->PopupFreeMessageDlg( strTemp, -1, -1, UI_DIALOG_TITLE_OK );
				bAllOK = FALSE;
			}

			if (bAllOK)
			{
				// 2026-09-03 — Latin-only (Phoenix_IsLatinCharacterName
				// aciklamasi; "-_" izni kaldirildi)
				if (!IsValidID(pName, NULL)
					|| !Phoenix_IsLatinCharacterName(pName))
				{
					//---------------------------------------------
					// ????????ID?????????
					//---------------------------------------------						
					g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString(), -1, -1, UI_DIALOG_TITLE_OK );
					bAllOK = FALSE;
				}
				else
				{
					char strName[80];
					strcpy(strName, pName);

					// ?????????? ?????? ??????????????? ??????????????
					if (g_pChatManager->RemoveCurse(strName))
					{
						g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString() , -1, -1, UI_DIALOG_TITLE_OK);
						bAllOK = FALSE;
					}
					else
					{
						MChatManager badNameManager;
						badNameManager.LoadFromFile( g_pFileDef->getProperty("FILE_INFO_CHAT_2").c_str());

						if (badNameManager.RemoveCurse(strName))
						{
							g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString(), -1, -1, UI_DIALOG_TITLE_OK );
							bAllOK = FALSE;
						}
					}
				}
			}

			if (bAllOK)
			{
					CLQueryCharacterName clQueryCharacterName;

					clQueryCharacterName.setCharacterName( pName );

					g_pSocket->sendPacket( &clQueryCharacterName );

					// ??????. -_-;;
					g_pUserInformation->WhisperID = pName;
			}
		}
	}
}


//-----------------------------------------------------------------------------
//
// ??????/???? ?????? ??????????
//
//-----------------------------------------------------------------------------
/*
void
UIMessageManager::Execute_UI_SELECT_EXPLOSIVE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_SELECT_EXPLOSIVE");
	
	int type = left;		// ??????(2) / ????(3)
	int itemType = right;	// ??????(??????:0~3, ????:0~5)


	//--------------------------------------------------------
	// ?????????????????????? ?????
	//--------------------------------------------------------
	if (g_pPlayer->IsItemCheckBufferNULL()
		&& g_pTempInformation->Mode==TempInformation::MODE_NULL)
	{
		//--------------------------------------------------------
		// ???????????? ?????
		//--------------------------------------------------------
		if (type==2 && itemType>=0 && itemType<6
			|| type==3 && itemType>=0 && itemType<4)
		{
			int itemClass = ((type==2)? ITEM_CLASS_MINE : ITEM_CLASS_BOMB);
				
			#ifdef CONNECT_SERVER
				CGMakeItem _CGMakeItem;
			
				_CGMakeItem.setItemClass( itemClass );
				_CGMakeItem.setItemType( itemType );

				g_pSocket->sendPacket( &_CGMakeItem );
				
			#endif	

			// item ?????
			g_pTempInformation->Mode = TempInformation::MODE_SKILL_MAKE_ITEM;
			g_pTempInformation->Value1 = itemClass;
			g_pTempInformation->Value2 = itemType;

			// ?????????????? ??????
			UI_LockItem();
			
			// ?????????? ??????
			UI_CloseSelectExplosive();
		}
		//--------------------------------------------------------
		// ?????? ?????????????
		//--------------------------------------------------------
		else
		{
			// ???????????? ???????????????????~~
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_ERROR_ETC_ERROR].GetString() );
		}
	}
	else
	{
		DEBUG_ADD("ItemCheckBuffer is not NULL. wait~~" );
	}
}
*/

//-----------------------------------------------------------------------------
//
// ??????/???? ?????????????????? ????
//
//-----------------------------------------------------------------------------
/*
void
UIMessageManager::Execute_UI_CLOSE_SELECT_EXPLOSIVE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_SELECT_EXPLOSIVE");
	
	UI_CloseSelectExplosive();
}
*/

void
UIMessageManager::Execute_UI_CLOSE_TEAM_LIST(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CLOSE_TEAM_LIST");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseTeamList();

		g_pUIDialog->UnSetLockInputPCTalk();
//		g_pUIDialog->ClosePCTalkDlg();
//		g_pPlayer->SetWaitVerifyNULL();
		// ???? ???????????????????????????? ????.
		//g_pUIDialog->ShowPCTalkDlg();
	}
}

void
UIMessageManager::Execute_UI_CLOSE_TEAM_INFO(int left, int right, void* void_ptr)
{
	gC_vs_ui.CloseTeamInfo(); 
}

void
UIMessageManager::Execute_UI_CLOSE_TEAM_MEMBER_INFO(int left, int right, void* void_ptr)
{
	gC_vs_ui.CloseTeamMemberInfo();
}

void
UIMessageManager::Execute_UI_CLOSE_TEAM_REGIST(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CLOSE_TEAM_REGIST");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		// ???? ???????????????????????????? ????.
		if(!gC_vs_ui.ReturnTeamRegistMember())
			g_pUIDialog->ShowPCTalkDlg();

		gC_vs_ui.CloseTeamRegist();
	}
}

void
UIMessageManager::Execute_UI_CLOSE_TEAM_MEMBER_LIST(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_CLOSE_TEAM_MEMBER_LIST");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	
	gC_vs_ui.CloseTeamMemberList();
//	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
//	{
//
//		// ???? ???????????????????????????? ????.
//		g_pUIDialog->ShowPCTalkDlg();
//	}
}

void
UIMessageManager::Execute_UI_REQUEST_GUILD_INFO(int left, int right, void* void_ptr)
{
		CGSelectGuild  _CGSelectGuild ;
			
		_CGSelectGuild.setGuildID(g_pPlayer->GetGuildNumber());
		g_pSocket->sendPacket( &_CGSelectGuild  );				

}

	
void
UIMessageManager::Execute_UI_REQUEST_GUILD_MEMBER_LIST(int left, int right, void* void_ptr)
{
		CGRequestGuildMemberList  _CGRequestGuildMemberList ;
			
		g_pSocket->sendPacket( &_CGRequestGuildMemberList  );				

}

void
UIMessageManager::Execute_UI_MODIFY_GUILD_MEMBER(int left, int right, void* void_ptr)
{
		CGModifyGuildMember _CGModifyGuildMember;

		_CGModifyGuildMember.setGuildID(left);
		_CGModifyGuildMember.setGuildMemberRank(right);
		_CGModifyGuildMember.setName(std::string((char *)void_ptr));

		g_pSocket->sendPacket( &_CGModifyGuildMember );				

		gC_vs_ui.CloseTeamMemberInfo();
}

void
UIMessageManager::Execute_UI_SELECT_TEAM_MEMBER_LIST(int left, int right, void* void_ptr)
{
	char *pName = (char *)void_ptr;

	if(pName != NULL)
	{
			CGSelectGuildMember  _CGSelectGuildMember ;
			
			_CGSelectGuildMember.setName(std::string(pName));
			g_pSocket->sendPacket( &_CGSelectGuildMember  );				
	}
}


void
UIMessageManager::Execute_UI_SELECT_READY_TEAM_LIST(int left, int right, void* void_ptr)		// void_ptr = TEAM_NAME
{
	char *pName = (char *)void_ptr;

	if(pName != NULL)
	{
			CGSelectGuild  _CGSelectGuild ;
			
			_CGSelectGuild.setGuildID(left);
			g_pSocket->sendPacket( &_CGSelectGuild  );				
	}
}

void
UIMessageManager::Execute_UI_SELECT_REGIST_TEAM_LIST(int left, int right, void* void_ptr)		// void_ptr = TEAM_NAME
{
	char *pName = (char *)void_ptr;

	if(pName != NULL)
	{
			CGSelectGuild  _CGSelectGuild ;
			
			_CGSelectGuild.setGuildID(left);
			g_pSocket->sendPacket( &_CGSelectGuild  );				
	}
}

	
void
UIMessageManager::Execute_UI_JOIN_READY_TEAM(int left, int right, void* void_ptr)				// void_ptr = TEAM_NAME
{
	CGTryJoinGuild _CGTryJoinGuild;
	_CGTryJoinGuild.setGuildID(left);
	_CGTryJoinGuild.setGuildMemberRank(2);
	g_pSocket->sendPacket( &_CGTryJoinGuild );
}

void
UIMessageManager::Execute_UI_JOIN_REGIST_TEAM(int left, int right, void* void_ptr)			// void_ptr = TEAM_NAME
{
	CGTryJoinGuild _CGTryJoinGuild;
	_CGTryJoinGuild.setGuildID(left);
	_CGTryJoinGuild.setGuildMemberRank(3);
	g_pSocket->sendPacket( &_CGTryJoinGuild );
}

	
void
UIMessageManager::Execute_UI_REGIST_GUILD_MEMBER(int left, int right, void* void_ptr)			// void_ptr = introduction max:150byte ??????????!
{

	if(void_ptr == NULL)
	{
		// ???????????????????
		g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_GUILD_REGIST_FAIL_INTRO].GetString() );
		return;
	}

	CGJoinGuild _CGJoinGuild;
	_CGJoinGuild.setGuildID(left);
	_CGJoinGuild.setGuildMemberRank(right);
	_CGJoinGuild.setGuildMemberIntro(std::string((char *)void_ptr));
	g_pSocket->sendPacket( &_CGJoinGuild );

	gC_vs_ui.CloseTeamRegist();
	gC_vs_ui.CloseTeamInfo();
}

void
UIMessageManager::Execute_UI_REGIST_GUILD_TEAM(int left, int right, void* void_ptr)			// left = TEAM_NAME, void_ptr = introduction max:150byte ??????????!
{

	if(void_ptr == NULL || (char *)left == NULL)
	{
		// ???????????????????
		g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_GUILD_REGIST_FAIL_INTRO].GetString() );
		return;
	}
//20090130 ??????????????????? ?????????????????????????????????????.
	bool bYokName = g_pChatManager->RemoveCurse((char *)left);
	if (bYokName)
	{
		g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[UI_STRING_MESSAGE_GUILDNAMEYOK].GetString());
		return;
	}

	CGRegistGuild _CGRegistGuild;
	_CGRegistGuild.setGuildName(std::string((char *)left));
	_CGRegistGuild.setGuildIntro(std::string((char *)void_ptr));
	g_pSocket->sendPacket( &_CGRegistGuild );

	gC_vs_ui.CloseTeamRegist();
}

	
void
UIMessageManager::Execute_UI_CLOSE_FILE_DIALOG(int left, int right, void* void_ptr)
{
	switch(left)
	{
	case C_VS_UI_FILE_DIALOG::MODE_PROFILE_SELECT:
		if(void_ptr != NULL)
		{
			char *p_str = (char *)void_ptr;
			const int str_size = strlen(p_str);

			if(str_size > 4)
			{
				const POINT bigSize = { 55, 70 };
				
				CDirectDrawSurface surface;

				surface.InitOffsurface( bigSize.x, bigSize.y, DDSCAPS_SYSTEMMEMORY );
				
				RECT destBigRect = { 0, 0, bigSize.x, bigSize.y };

				CDirectDrawSurface bmpSurface;
				
				if (LoadImageToSurface(p_str, bmpSurface))
				{
					// surface???????? default Profile??????????????
					// size????????????????????????..	
					RECT bmpRect = { 0, 0, bmpSurface.GetWidth(), bmpSurface.GetHeight() };

					// BigSize
					surface.FillSurface( 0 );
					surface.Blt(&destBigRect, &bmpSurface, &bmpRect);

					char saveBmpName[512];
					strcpy(saveBmpName, g_pFileDef->getProperty("DIR_PROFILE").c_str());
					strcat(saveBmpName, "\\");
					strcat(saveBmpName, g_char_slot_ingame.sz_name.c_str());
					strcat(saveBmpName, ".bmp");
					surface.SaveToBMP(saveBmpName);

					g_pProfileManager->InitProfiles();
					gC_vs_ui.RefreshInfoImage();

				}

			}
		}
		break;
	}
	gC_vs_ui.CloseFileDialog();
}

//-----------------------------------------------------------------------------
//
// mouse????Item(EVENT_STAR)?????? Inventory????Item????Enchant????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ENCHANT_ACCEPT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ENCHANT_ACCEPT");

	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}

	// gpC_mouse_pointer->GetPickUpItem()		// ????????? ????
	//void_ptr = MItem *		// ?????????
	// Inventory????(left,right)
	MItem* pItem = (MItem*)void_ptr;
	MItem* pMouseItem = gpC_mouse_pointer->GetPickUpItem();

	if (pMouseItem==NULL)
	{
		DEBUG_ADD("[Error] Mouse Item is NULL");
		return;
	}

	if (pItem==NULL)
	{
		DEBUG_ADD("[Error] InvenItem is NULL");
		return;
	}

	TYPE_OBJECTID mouseItemID = ((pMouseItem==NULL)?OBJECTID_NULL : pMouseItem->GetID());

	if (g_pTradeManager!=NULL)
		return;

	if (g_pPlayer->IsItemCheckBufferPickupSomeFromInventory())
	{
		DEBUG_ADD("[UI] UI_ENCHANT_ACCEPT refused: EventStar split GCCreateItem pending (PICKUP_SOME)");
		return;
	}

	if (pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_STAR && pMouseItem->GetNumber() != 1)
	{
		DEBUG_ADD_FORMAT("[UI] UI_ENCHANT_ACCEPT refused: EventStar stack num=%d (need 1)", pMouseItem->GetNumber());
		return;
	}

	//----------------------------------------------------
	// Server
	//----------------------------------------------------
	if(g_pTempInformation->GetMode() == TempInformation::MODE_NULL && g_pPlayer->IsItemCheckBufferNULL())
	{
		CGAddItemToItem _CGAddItemToItem; 
		_CGAddItemToItem.setObjectID( mouseItemID );
		_CGAddItemToItem.setX( pItem->GetGridX() );
		_CGAddItemToItem.setY( pItem->GetGridY() );								
		
		g_pSocket->sendPacket( &_CGAddItemToItem );
		
		g_pPlayer->SetItemCheckBuffer( pMouseItem, MPlayer::ITEM_CHECK_BUFFER_ITEM_TO_ITEM );
		g_pTempInformation->SetMode(TempInformation::MODE_ITEM_TO_ITEM);
		g_pTempInformation->Value1 = pItem->GetGridX();
		g_pTempInformation->Value2 = pItem->GetGridY();

//		UI_LockItem();
	}
}

void
UIMessageManager::Execute_UI_ENCHANT_CANCEL(int left, int right, void* void_ptr)
{
	UI_CloseEnchant();
}

void 
UIMessageManager::Execute_UI_MESSAGE_BOX(int left, int right, void* void_ptr)
{
	if(left < MAX_GAME_STRING)
		g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[left].GetString() );
}


void 
UIMessageManager::Execute_UI_CLOSE_OTHER_INFO(int left, int right, void* void_ptr)
{
	gC_vs_ui.CloseOtherInfo();
}

void UIMessageManager::Execute_UI_MODIFY_TEAM_INFO(int left, int right, void* void_ptr)
{
	if(void_ptr != NULL)
	{
		CGModifyGuildIntro _CGModifyGuildIntro;
		_CGModifyGuildIntro.setGuildID(left);
		_CGModifyGuildIntro.setGuildIntro(std::string((char *)void_ptr));
		g_pSocket->sendPacket( &_CGModifyGuildIntro );
	}
}

void UIMessageManager::Execute_UI_MODIFY_GUILD_MEMBER_INTRO(int left, int right, void* void_ptr)
{
	if(void_ptr != NULL)
	{
		CGModifyGuildMemberIntro _CGModifyGuildMemberIntro;
		_CGModifyGuildMemberIntro.setGuildID(left);
		_CGModifyGuildMemberIntro.setGuildMemberIntro(std::string((char *)void_ptr));
		g_pSocket->sendPacket( &_CGModifyGuildMemberIntro );
	}
}

void UIMessageManager::Execute_UI_SEND_NAME_FOR_SOUL_CHAIN(int left, int right, void* void_ptr)
{
	// ???? ????????????????????????????????
	if(g_pPlayer->IsWaitVerify())
		return;

	if(void_ptr != NULL)
	{
		CGSkillToNamed _CGSkillToNamed;
		_CGSkillToNamed.setSkillType(SKILL_SOUL_CHAIN);
		_CGSkillToNamed.setTargetName(std::string((char *)void_ptr));
		_CGSkillToNamed.setCEffectID(0);

		g_pSocket->sendPacket( &_CGSkillToNamed );
		g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_SKILL_SUCCESS);

//		(*g_pSkillInfoTable)[SKILL_SOUL_CHAIN].SetNextAvailableTime();
	}
}

void UIMessageManager::Execute_UI_CLOSE_TRACE_WINDOW(int left, int right, void* void_ptr)
{
//	//------------------------------------------------------------------
//	// Player?????????????skill????????????????????????????????
//	//------------------------------------------------------------------	
//	if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_SKILL_SUCCESS)
//	{		
//		g_pPlayer->SetWaitVerifyNULL();
//	}
//	else
//	{
//		DEBUG_ADD("[Error] Player is not WaitVerifySkillSuccess");
//	}
//
//	//------------------------------------------------------------------
//	// Item Lock????????.
//	//------------------------------------------------------------------
//	if (g_pPlayer->GetItemCheckBufferStatus()==MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY)
//	{
//		g_pPlayer->ClearItemCheckBuffer();
//	}

	gC_vs_ui.CloseTraceWindow();
	g_pUIDialog->UnSetLockInputPCTalk();
}

// ?????????? ??????
void UIMessageManager::Execute_UI_RUN_CONNECT(int left, int right, void* void_ptr)
{
	UI_RunConnect();
}

void UIMessageManager::Excute_UI_SELECT_GRADE_SKILL(int left, int right, void* void_ptr)
{
	if(left >= g_pRankBonusTable->GetSize() || (*g_pRankBonusTable)[left].GetStatus() == RankBonusInfo::STATUS_CANNOT_LEARN || (*g_pRankBonusTable)[left].GetStatus() == RankBonusInfo::STATUS_LEARNED
		|| !g_pPlayer->IsWaitVerifyNULL() || g_pTempInformation->GetMode() != TempInformation::MODE_NULL)
		return;

	CGSelectRankBonus _CGSelectRankBonus;
	_CGSelectRankBonus.setRankBonusType(left);
	g_pSocket->sendPacket( &_CGSelectRankBonus );
	g_pTempInformation->SetMode(TempInformation::MODE_SKILL_LEARN);

}

void UIMessageManager::Excute_UI_USE_XMAS_TREE(int left, int right, void* void_ptr)				// left = to, right = from, void_ptr = message
{
	DEBUG_ADD("Excute_UI_USE_XMAS_TREE");
	MItem *pItem = (MItem *)left;

	if(pItem == NULL  || !g_pPlayer->IsItemCheckBufferNULL())
		return;

	char *sz_ptr = (char *)void_ptr;

	if(sz_ptr == NULL)
	{
		// ?????????? ????????.
		g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_XMAS_CARD_CANNOT_USE].GetString() );
		return;
	}

	CGUseMessageItemFromInventory _CGUseMessageItemFromInventory;
	_CGUseMessageItemFromInventory.setObjectID( pItem->GetID() );
	_CGUseMessageItemFromInventory.setX( pItem->GetGridX() );
	_CGUseMessageItemFromInventory.setY( pItem->GetGridY() );
	_CGUseMessageItemFromInventory.setMessage(sz_ptr);
					
	g_pSocket->sendPacket( &_CGUseMessageItemFromInventory );
					
	g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);

	if(right == 1)
		gC_vs_ui.CloseBulletinBoardWindow();
	else
		gC_vs_ui.CloseXmasCardWindow();
	
	DEBUG_ADD("Excute_UI_USE_XMAS_TREE OK");
}

void UIMessageManager::Excute_UI_CLOSE_XMAS_CARD_WINDOW(int left, int right, void* void_ptr)
{
	gC_vs_ui.CloseXmasCardWindow();
}


void UIMessageManager::Excute_UI_SEND_BRING_FEE(int left,int right, void* void_ptr)
{
	// ?????????? ????????????????.
	if(g_pTempInformation->GetMode() != TempInformation::MODE_NULL)
	{
		DEBUG_ADD("Excute_UI_SEND_BRING_FEE TempInformation is not NULL");
		return;
	}
	// ?????? ????????????
	DEBUG_ADD("Excute_UI_SEND_BRING_FEE");
	// ???????????? ???????????????? ???? ??????????????????????????????

	g_pTempInformation->SetMode(TempInformation::MODE_WAIT_BRING_FEE);
	g_pTempInformation->Value1 = left;

	CGWithdrawTax _CGWithdrawTax;

	_CGWithdrawTax.setGold((DWORD)left);
	g_pSocket->sendPacket( &_CGWithdrawTax );

	DEBUG_ADD("Excute_UI_SEND_BRING_FEE OK");
}

void UIMessageManager::Excute_UI_CLOSE_BRING_FEE_WINDOW(int left,int right, void* void_ptr)
{
	// ?????????? ???????????
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseBringFeeWindow();
		g_pUIDialog->ShowPCTalkDlg();
	}
}

void UIMessageManager::Excute_UI_CLOSE_WAR_LIST(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseWarListWindow();	
}

void UIMessageManager::Execute_UI_CLOSE_BLOOD_BIBLE_STATUS(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseBloodBibleStatus();	
}

void UIMessageManager::Execute_UI_SEND_NAME_FOR_COUPLE(int left, int right, void *void_ptr)
{	
	if( g_pPlayer->GetWaitVerify() != MPlayer::WAIT_VERIFY_NULL 
		&& g_pTempInformation->GetMode()!=TempInformation::MODE_NULL)
	{
		g_pUIDialog->ClosePCTalkDlg();
		return;
	}


	if(!strcmp(g_char_slot_ingame.sz_name.c_str() , (char*)void_ptr))
	{
		UI_PopupMessage( UI_STRING_MESSAGE_NOT_USER );
		return ; 
	}

	int npcID = (*g_pPCTalkBox).GetNPCID();
	
	CGTypeStringList _CGTypeStringList;

	_CGTypeStringList.setType((CGTypeStringList::StringType) left);
	_CGTypeStringList.addString( std::string((char*) void_ptr) );	
	_CGTypeStringList.setParam( npcID );
	
	g_pSocket->sendPacket( &_CGTypeStringList );

//	gC_vs_ui.CloseInputNameWindow();
	gC_vs_ui.SetDisableInputName();

	g_pPlayer->SetWaitVerify(MPlayer::WAIT_VERIFY_NPC_ASK);
	g_pTempInformation->SetMode(TempInformation::MODE_INPUT_NAME);
	g_pTempInformation->Value1 = (int)timeGetTime();
}

void UIMessageManager::Execute_UI_CLOSE_INPUT_NAME_WINDOW(int left, int right, void *void_ptr)
{
	if( g_pPlayer->GetWaitVerify() != MPlayer::WAIT_VERIFY_NPC_ASK	)
	{
		g_pUIDialog->ClosePCTalkDlg();
		gC_vs_ui.CloseInputNameWindow();
		g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_NULL );
	}  else
	{
		if(g_pTempInformation->GetMode() == TempInformation::MODE_INPUT_NAME)
		{
			DWORD dwTime = (DWORD) g_pTempInformation->Value1;
			
			if( timeGetTime() - dwTime >= 1000*60 )
			{
				g_pUIDialog->ClosePCTalkDlg();
				gC_vs_ui.CloseInputNameWindow();
				g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_NULL );
				g_pTempInformation->SetMode( TempInformation::MODE_NULL );
			}
		}
	}
}


//-----------------------------------------------------------------------------
//
// Gear?????? Item?????????????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_USE_GEAR(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_USE_GEAR");

	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	//
	// left = slot
	// right = by FunctionKey
	//
	//MItem* pItem = (MItem*)void_ptr;
	if (g_pPlayer!=NULL && (g_pPlayer->IsSlayer() && g_pSlayerGear!=NULL || g_pPlayer->IsVampire() && g_pVampireGear!= NULL || g_pPlayer->IsOusters() && g_pOustersGear!= NULL))
	{
		MItem* pSlotItem = static_cast<MItem*>(void_ptr);

		if (pSlotItem!=NULL && pSlotItem->IsAffectStatus())
		{

			if (g_pPlayer->IsItemCheckBufferNULL())
			{
				// ?????????? ?????????? ??????-????;
				if(g_pPlayer->IsSlayer() && pSlotItem->IsSlayerItem() ||
					g_pPlayer->IsVampire() && pSlotItem->IsVampireItem() ||
					g_pPlayer->IsOusters() && pSlotItem->IsOustersItem())
				{
					pSlotItem->UseQuickItem();
				}
			}
			//----------------------------------------------------
			// item check buffer not null
			//----------------------------------------------------
			else
			{
				DEBUG_ADD("[Wait] Item Check Buffer is Not NULL");
			} 
		}
		else
		{
			DEBUG_ADD_FORMAT("[Error] There is no item in slot=%d", left);
		}
	}
	else
	{
		DEBUG_ADD("[Error] There is no Gear");
	}

}

void
UIMessageManager::Execute_GO_BILING_PAGE(int left, int right, void* void_ptr)
{
	if(left == TRUE)
	{
		// ??????..
		SetMode( MODE_QUIT );
		
		char str[256];
		
		GetWindowsDirectory(
			str,  // address of buffer for Windows directory
			255        // size of directory buffer
			);
		
		sprintf(str, "%s\\Explorer.exe", str);
		
		CDirectDraw::RestoreDisplayModeCompat();
		
		_spawnl(_P_NOWAIT, str, "Explorer.exe", g_pClientConfig->URL_HOMEPAGE_BILING.GetString(), NULL);
	}	
}

void 
UIMessageManager::Execute_UI_CLOSE_POPUP_MESSAGE(int left, int right, void *void_ptr)
{
	gC_vs_ui.ClosePopupMessage();
}


void 
UIMessageManager::Execute_UI_CLOSE_QUEST_STATUS(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseQuestStatusWindow();
}

void 
UIMessageManager::Execute_UI_CLOSE_LOTTERY_CARD(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseLotteryCard();
	gC_vs_ui.SetQuestStatusInit();

	if(g_bEventQuestEnding && right == 5)
	{
		UI_CloseAllDialog();
		MEvent event;
		event.eventID = EVENTID_OUSTERS_FIN;
		event.eventType = EVENTTYPE_ZONE;
		event.eventFlag = EVENTFLAG_ONLY_EVENT_BACKGROUND | EVENTFLAG_NOT_DRAW_UI | EVENTFLAG_NOT_DRAW_CREATURE | 
			EVENTFLAG_NOT_DRAW_INFORMATION | EVENTFLAG_NOT_DRAW_CREATURE_SHADOW | EVENTFLAG_NOT_DRAW_ITEM | 
			EVENTFLAG_NOT_DRAW_MOUSE_POINTER | EVENTFLAG_NOT_DRAW_EFFECT | EVENTFLAG_DENY_INPUT | 
			EVENTFLAG_NOT_FADE_SCREEN | EVENTFLAG_NOT_PLAY_SOUND;
		event.parameter1 = 0;
		event.parameter4 = EVENTBACKGROUNDID_QUEST_2;
		g_pEventManager->AddEvent(event);			
	}
	g_bEventQuestEnding = false;	
}

void 
UIMessageManager::Execute_UI_LOTTERY_CARD_STATUS(int left, int right, void *void_ptr)
{
	// left = step, right = ???? ID
	
	CGLotterySelect _CGLotterySelect;
	
	_CGLotterySelect.setType( TYPE_SELECT_LOTTERY );
	_CGLotterySelect.setQuestLevel ( (DWORD) left-1 );
	_CGLotterySelect.setGiftID ( (DWORD) right );

	g_pSocket->sendPacket( &_CGLotterySelect );
}

void 
UIMessageManager::Execute_UI_FINISH_SCRATCH_LOTTERY(int left, int right, void *void_ptr)
{
	CGLotterySelect _CGLotterySelect;
	
	_CGLotterySelect.setType( TYPE_FINISH_SCRATCH );
	_CGLotterySelect.setQuestLevel ( 0 );
	_CGLotterySelect.setGiftID ( 0 );

	g_pSocket->sendPacket( &_CGLotterySelect );
	
}

void 
UIMessageManager::Execute_UI_CLOSE_IMAGE_NOTICE(int left,int right, void *void_ptr)
{
	gC_vs_ui.CloseImageNotice();
}

void 
UIMessageManager::Execute_UI_SELECT_ITEM_FROM_SHOP(int left, int right, void *void_ptr)
{
	CGTakeOutGood _CGTakeOutGood;

	_CGTakeOutGood.setObjectID( (DWORD) left );

	g_pSocket->sendPacket( &_CGTakeOutGood );
}

void 
UIMessageManager::Execute_UI_CLOSE_ITEM_LIST( int left, int right, void *void_ptr)
{
	g_pUIDialog->ClosePCTalkDlg();	
	gC_vs_ui.CloseItemListWindow();
}

void 
UIMessageManager::Execute_UI_CLOSE_BULLETIN_BOARD(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseBulletinBoardWindow();
}

void 
UIMessageManager::Execute_UI_TRANS_ITEM_CANCEL(int left, int right, void *void_ptr)
{
	UI_CloseTransItem();
}

void 
UIMessageManager::Execute_UI_TRANS_ITEM_ACCEPT(int left, int right, void *void_ptr)
{	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	// gpC_mouse_pointer->GetPickUpItem()		// ????????? ????
	//void_ptr = MItem *		// ?????????
	// Inventory????(left,right)
	MItem* pItem = (MItem*)void_ptr;
	MItem* pMouseItem = gpC_mouse_pointer->GetPickUpItem();

	if (pMouseItem==NULL)
	{
		DEBUG_ADD("[Error] Mouse Item is NULL");
		return;
	}
	if (pItem==NULL)
	{
		DEBUG_ADD("[Error] InvenItem is NULL");
		return;
	}
	TYPE_OBJECTID mouseItemID = ((pMouseItem==NULL)?OBJECTID_NULL : pMouseItem->GetID());

	// ?????????????? Enchant??????????????!
	if (g_pTradeManager!=NULL)
		return;
	
	//g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_INSERT_FROM_INVENTORY );
	
	//----------------------------------------------------
	// Server???????????????????
	//----------------------------------------------------
	if(g_pTempInformation->GetMode() == TempInformation::MODE_NULL && g_pPlayer->IsItemCheckBufferNULL())
	{
		CGAddItemToItem _CGAddItemToItem;
		_CGAddItemToItem.setObjectID( mouseItemID );
		_CGAddItemToItem.setX( pItem->GetGridX() );
		_CGAddItemToItem.setY( pItem->GetGridY() );								
		
		g_pSocket->sendPacket( &_CGAddItemToItem );
		
		g_pPlayer->SetItemCheckBuffer( pMouseItem, MPlayer::ITEM_CHECK_BUFFER_ITEM_TO_ITEM );
		g_pTempInformation->SetMode(TempInformation::MODE_ITEM_TO_ITEM);
		g_pTempInformation->Value1 = pItem->GetGridX();
		g_pTempInformation->Value2 = pItem->GetGridY();

//		UI_LockItem();
	}
}

void 
UIMessageManager::Execute_UI_REQUEST_RESURRECT(int left, int right, void *void_ptr)
{
	if( left == 0 || left == 1)
	{
		// Resurrect Scroll
		MItem* pItem = g_pInventory->FindItem( ITEM_CLASS_RESURRECT_ITEM, left );
		if( pItem == NULL )
		{
			return;
		}
		
		CGUseItemFromInventory _CGUseItemFromInventory;
		_CGUseItemFromInventory.setObjectID( pItem->GetID() );
		_CGUseItemFromInventory.setX( pItem->GetGridX() );
		_CGUseItemFromInventory.setY( pItem->GetGridY() );
		
		g_pSocket->sendPacket( &_CGUseItemFromInventory );				
		g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
	}
	else
	if( left == 2 )
	{
		// Eternity
		CGSkillToSelf			_CGSkillToSelf;

		_CGSkillToSelf.setSkillType( SKILL_ETERNITY );
		g_pSocket->sendPacket( &_CGSkillToSelf );
	}
	else if(left == 3)
	{
		CGSelectRegenZone _CGSelectRegenZone;
		_CGSelectRegenZone.setRegenZoneID(14);
		g_pSocket->sendPacket( &_CGSelectRegenZone );
	}
	else if(left == 4)
	{
		CGSkillToSelf			_CGSkillToSelf;

		_CGSkillToSelf.setSkillType( SKILL_REQUEST_RESURRECT );
		g_pSocket->sendPacket( &_CGSkillToSelf );

		g_pPlayer->SetStatus( MODIFY_CURRENT_HP, HP);
		g_pPlayer->SetStatus( MODIFY_CURRENT_MP, MP);
	}
	
}

void 
UIMessageManager::Execute_UI_CLOSE_MIXING_FORGE(int left, int right, void *void_ptr)
{
	if(left == 0 )
	{
		gC_vs_ui.CloseMixingForge();
		g_pPlayer->ClearItemCheckBuffer();
		g_pTempInformation->SetMode( TempInformation::MODE_NULL );
	} else
	{
		g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[left].GetString() );		
	}
}

void 
UIMessageManager::Execute_UI_MIXING_FORGE(int left, int right, void* void_ptr)
{
	if( g_pPlayer->GetItemCheckBufferStatus() != MPlayer::ITEM_CHECK_BUFFER_USE_MIXING_ITEM ||
		g_pTempInformation->GetMode() != TempInformation::MODE_NULL )
		return;

	MItem *pFirstItem = g_pInventory->GetItemToModify( left );
	MItem *pSecondItem = g_pInventory->GetItemToModify( right );

	if( pFirstItem == NULL || pSecondItem == NULL )
	{
		return;
	}

	if( pFirstItem->GetItemClass() == pSecondItem->GetItemClass() &&
		pFirstItem->GetItemType() == pSecondItem->GetItemType() &&
		pFirstItem->GetItemOptionListCount() == 1 &&
		pSecondItem->GetItemOptionListCount() == 1 &&
		!pFirstItem->IsUniqueItem() && ! pSecondItem->IsUniqueItem() &&
		!pFirstItem->IsQuestItem() && ! pSecondItem->IsQuestItem() )
	{		
		MItem* pMixingItem = g_pPlayer->GetItemCheckBuffer();

		CGMixItem _CGMixItem;

		_CGMixItem.setObjectID( pMixingItem->GetID() );
		_CGMixItem.setX( pMixingItem->GetGridX() );
		_CGMixItem.setY( pMixingItem->GetGridY() );
		_CGMixItem.setTargetObjectID( 0, pFirstItem->GetID() );
		_CGMixItem.setTargetObjectID( 1, pSecondItem->GetID() );
		
		g_pSocket->sendPacket( &_CGMixItem );
		g_pPlayer->SetItemCheckBuffer( pMixingItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY );
		
		g_pTempInformation->SetMode( TempInformation::MODE_ITEM_MIXING );
		g_pTempInformation->Value1 = pFirstItem->GetGridX();
		g_pTempInformation->Value2 = pFirstItem->GetGridY();

		g_pTempInformation->Value3 = pSecondItem->GetGridX();
		g_pTempInformation->Value4 = pSecondItem->GetGridY();
		
		g_pTempInformation->pValue = (void*) pMixingItem;
	}		
}

void 
UIMessageManager::Execute_UI_CLOSE_REMOVE_OPTION(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseRemoveOptionFromRareItem();
}

void 
UIMessageManager::Execute_UI_SEND_REMOVE_OPTION(int left, int right, void *void_ptr)
{
	MItem *pMouseItem = (MItem*) void_ptr;
	MItem *pCurItem = g_pInventory->GetItemToModify( right );


	if( left < 0 || left > 1 || pMouseItem == NULL || pCurItem == NULL)
	{
		g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_ERROR_ETC_ERROR].GetString());
		return;
	}

	CGMixItem _CGMixItem;
	_CGMixItem.setObjectID( pMouseItem->GetID() );
	_CGMixItem.setX( pCurItem->GetGridX() );
	_CGMixItem.setY( pCurItem->GetGridY() );
	_CGMixItem.setTargetObjectID( 0, pCurItem->GetID() );
	_CGMixItem.setTargetObjectID( 1, left );
	g_pSocket->sendPacket( &_CGMixItem );
	gC_vs_ui.CloseRemoveOptionFromRareItem();

	g_pPlayer->SetItemCheckBuffer( pMouseItem, MPlayer::ITEM_CHECK_BUFFER_ITEM_TO_ITEM );
	
	g_pTempInformation->SetMode(TempInformation::MODE_ITEM_TO_ITEM);
	g_pTempInformation->Value1 = pCurItem->GetGridX();
	g_pTempInformation->Value2 = pCurItem->GetGridY();
	g_pTempInformation->Value3 = left;
}

void 
UIMessageManager::Execute_UI_CLOSE_OUSTERS_SKILL_INFO(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseOustersSkillInfo();
}

void 
UIMessageManager::Execute_UI_LEARN_OUSTERS_SKILL(int left, int right, void *void_ptr)
{
	if( g_pTempInformation->GetMode() != TempInformation::MODE_NULL )
		return;		
		
	DEBUG_ADD("[UI] UI_LEARN_OUSTERS_SKILL");
	
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	
	
	g_pTempInformation->SetMode(TempInformation::MODE_SKILL_LEARN);
	{
		(*g_pTempInformation).Value3 = left;
		
		CGLearnSkill _CGLearnSkill;
		
		//_CGLearnSkill.setObjectID( (*g_pPCTalkBox).GetNPCID() );
		_CGLearnSkill.setSkillType( left );
		_CGLearnSkill.setSkillDomainType( SKILLDOMAIN_OUSTERS );
		
		g_pSocket->sendPacket( &_CGLearnSkill );
		
	}
}

void 
UIMessageManager::Execute_UI_RUN_LEVELUP(int left, int right, void *void_ptr)
{
	UI_LevelUp();
}

void 
UIMessageManager::Execute_UI_CLOSE_HORN(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseHorn();
}


void 
UIMessageManager::Execute_UI_OUSTERS_DOWN_SKILL(int left, int right, void *void_ptr)
{
	if( left < 0 || left >= MAX_ACTIONINFO)
		return;

	int curLevel = (*g_pSkillInfoTable)[left].GetExpLevel();

	// 2004, 11, 8, sobeit modify start - ?????????? ?????????????? 0?????????? ????????????.
	if( curLevel > 30 || (curLevel == 1 && (*g_pSkillInfoTable)[left].CanDelete == 0) )
		return;	
	// 2004, 11, 8, sobeit modify end
	
	CGDownSkill	_CGDownSkill;

	_CGDownSkill.setSkillType( (SkillType_t) left );

	g_pSocket->sendPacket( &_CGDownSkill );

	DEBUG_ADD("[UI] Execute_UI_OUSTERS_DOWN_SKILL"); 
}

void 
UIMessageManager::Execute_UI_CLEAR_ALL_STAGE(int left, int right, void *void_ptr)
{	
	// LEFT = GAMETYPE
	switch( left )
	{
	case 0 :				// ??????!!!
		break;
	case 1 :
		break;
	}
	gC_vs_ui.CloseArrowTile();
	gC_vs_ui.CloseCrazyMine();
}

void 
UIMessageManager::Execute_UI_CLOSE_FINDING_MINE(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseFindingMine();
}

void 
UIMessageManager::Execute_UI_CLOSE_NEMONEMO(int left, int right, void *void_ptr)
{
	//gC_vs_ui.CloseNemo
}

void 
UIMessageManager::Execute_UI_CLOSE_PUSHPUSH(int left, int right, void *void_ptr)
{
}

void 
UIMessageManager::Execute_UI_CLOSE_CRAZY_MINE(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseCrazyMine();

	CGFailQuest _CGFailQuest;

	_CGFailQuest.setFail( false );

	g_pSocket->sendPacket( &_CGFailQuest );
	gC_vs_ui.SetQuestStatusInit();
}

void 
UIMessageManager::Execute_UI_CLOSE_ARROW_TILE(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseArrowTile();
	CGFailQuest _CGFailQuest;

	_CGFailQuest.setFail( false );

	g_pSocket->sendPacket( &_CGFailQuest );
	gC_vs_ui.SetQuestStatusInit();
}

void 
UIMessageManager::Execute_UI_CLEAR_STAGE(int left, int right, void *void_ptr)
{
	WORD	GameType = HIWORD(left);
	WORD	Stage = LOWORD(left);
	DWORD	Time = (DWORD)right;

	Time/=10;

	Time = min(65535,Time);
	DEBUG_ADD_FORMAT("[Execute_UI_CLEAR_STAGE] GameType:%d Stage:%d Time:%d", GameType, Stage,Time);	
	
	CGSubmitScore _CGSubmitScore;

	_CGSubmitScore.setGameType( GameType == 0 ? GAME_ARROW : GAME_MINE );
	_CGSubmitScore.setLevel( Stage );
	_CGSubmitScore.setScore( WORD(Time) );
	g_pSocket->sendPacket( &_CGSubmitScore );
}

void
UIMessageManager::Execute_UI_FORCE_DIE(int left, int right, void *void_ptr)
{
	// ?????? ????????
	gC_vs_ui.CloseAllDialog();
	
	switch(left)
	{
	case 10 :						// ArrowTile ????????????????
		break;
	case 11 :						// CrazyMine ????????????????			
		break;
	}
	
	// ????????????????????????????????????????
	CGFailQuest		_CGFailQuest;
	
	_CGFailQuest.setFail( true );
	g_pSocket->sendPacket( &_CGFailQuest );
	gC_vs_ui.SetQuestStatusInit();
}

void 
UIMessageManager::Execute_UI_ADD_ITEM_TO_CODE_SHEET(int left, int right, void *void_ptr)
{
	MItem* pItem = (MItem*)void_ptr;

	if( pItem == NULL || right < 0 || right >= 5)
		return;

	CGAddItemToCodeSheet _CGAddItemToCodeSheet;

	_CGAddItemToCodeSheet.setX( HIWORD( left ) );
	_CGAddItemToCodeSheet.setY( LOWORD( left ) );
	_CGAddItemToCodeSheet.setObjectID( pItem->GetID() );
	
	g_pSocket->sendPacket( &_CGAddItemToCodeSheet );
	g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_ITEM_TO_ITEM );	

	g_pTempInformation->SetMode(TempInformation::MODE_ITEM_CODE_SHEET);
	g_pTempInformation->Value1 = right;	
	g_pTempInformation->Value2 = left;
	g_pTempInformation->pValue = (void*)gpC_mouse_pointer->GetPickUpItem();
}


void 
UIMessageManager::Execute_UI_SEND_BUG_REPORT(int left,int right, void *void_ptr)
{
	SendBugReport("[UI_BUG] %d, %s", left, reinterpret_cast<char*>( void_ptr ) );
}

void 
UIMessageManager::Execute_UI_GO_BEGINNER_ZONE(int left, int right, void* void_ptr)
{
//	Execute_UI_CLOSE_POPUP_MESSAGE(left,right,void_ptr);
//
//	CGSelectWayPoint _CGSelectWayPoint;
//
//	_CGSelectWayPoint.setZoneID( 1122 );
//	_CGSelectWayPoint.setX( 0 );
//	_CGSelectWayPoint.setY( 0 );
//
//	g_pSocket->sendPacket( &_CGSelectWayPoint );
}

void
UIMessageManager::Execute_UI_POPUP_MESSAGE_OK(int left, int right, void* void_ptr)
{
	gC_vs_ui.CloseMailBox();
	
	Execute_UI_CLOSE_POPUP_MESSAGE(left,right,void_ptr);

	DWORD recvID = (DWORD) left;
	
	int zoneID = 0;

	switch(recvID)
	{
	// ????????
	case 1:
		zoneID = 1122;
		break;

	case 2:
		zoneID = 1122;
		break;
		
	// ?????? ?????? ??????
	case 4:
		zoneID = 1131;
		break;

	// ?????? ?????? ??????
	case 6:
		zoneID = 72;
		break;

	}

	CGSelectWayPoint _CGSelectWayPoint;
	
	_CGSelectWayPoint.setZoneID( zoneID );
	_CGSelectWayPoint.setX( 0 );
	_CGSelectWayPoint.setY( 0 );
	
	g_pSocket->sendPacket( &_CGSelectWayPoint );
}

void 
UIMessageManager::Execute_UI_CLOSE_SHRINE_MINIMAP(int left, int right, void* void_ptr)
{
	// Random-_- ?????? ?????????? ???????????????? ????????? 

	bool bSended = false;

	for(int i = 0; i < g_pRegenTowerInfoManager->GetSize(); i++ )
	{
		const RegenTowerInfo* pInfo = &g_pRegenTowerInfoManager->Get(i);
		
		if( pInfo->owner == (int)g_pPlayer->GetRace() )
		{
			CGSelectRegenZone _CGSelectRegenZone;
			_CGSelectRegenZone.setRegenZoneID( i );
			g_pSocket->sendPacket( &_CGSelectRegenZone );
			bSended = true;
			break;
		}
	}
	gC_vs_ui.CloseRequestShrineMinimap();

	if( bSended == false )
	{
		Execute_UI_FINISH_REQUEST_DIE_BUTTON(left, right, void_ptr);
	}
	gC_vs_ui.FinishRequestDie();
	gC_vs_ui.FinishRequestResurrect();	
}

void 
UIMessageManager::Execute_UI_WARP_TO_REGEN_TOWER(int left, int right, void* void_ptr)
{
	if( g_pRegenTowerInfoManager == NULL || left < 0 || 
		left >= g_pRegenTowerInfoManager->GetSize() || 
		g_pRegenTowerInfoManager->Get(left).owner != g_pPlayer->GetRace() )
		return;

	CGSelectRegenZone _CGSelectRegenZone;
	
	_CGSelectRegenZone.setRegenZoneID( left );

	g_pSocket->sendPacket( &_CGSelectRegenZone );
	gC_vs_ui.CloseRequestShrineMinimap();
	gC_vs_ui.FinishRequestDie();
	gC_vs_ui.FinishRequestResurrect();	
}

void 
UIMessageManager::Execute_UI_CLOSE_MAILBOX(int left, int right, void *void_ptr)
{
	gC_vs_ui.CloseMailBox();
}

void
UIMessageManager::Execute_UI_CLOSE_PET_INFO(int left, int right, void* void_ptr)
{
	gC_vs_ui.ClosePetInfo();
}

void	
UIMessageManager::Execute_UI_PET_GAMBLE(int left, int right, void* void_ptr)
{
	CGPetGamble _CGPetGamble;
	g_pSocket->sendPacket( &_CGPetGamble );
	// ???????????? ????????????????????
	UI_LockItem();
}

void
UIMessageManager::Execute_UI_CLOSE_USE_PET_FOOD(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	gC_vs_ui.CloseUsePetFood();
}

void	 
UIMessageManager::Execute_UI_CLOSE_PETSTORAGE(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	gC_vs_ui.ClosePetStorage();
	
	// ????????????????
	if (g_pStorage!=NULL)
	{
		delete g_pStorage;
		g_pStorage = NULL;
	}

	// storage????????????
	gC_vs_ui.SetPetStorage( NULL );

//	// ???? ???????????????????????????? ????.
//	g_pUIDialog->ShowPCTalkDlg();
	g_pUIDialog->UnSetLockInputPCTalk();
	g_pTempInformation->SetMode(TempInformation::MODE_NULL);

}
void	 
UIMessageManager::Execute_UI_CLOSE_KEEP_PETITEM(int left, int right, void* void_ptr)
{
	gC_vs_ui.CloseKeepPetItemDialog();
}
void	 
UIMessageManager::Execute_UI_CLOSE_GET_KEEP_PETITEM(int left, int right, void* void_ptr)
{
	gC_vs_ui.CloseGetKeepPetItemDialog();
}

void	 
UIMessageManager::Execute_UI_KEEP_PETITEM(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_KEEP_PETITEM");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	if (g_pTempInformation->GetMode() != TempInformation::MODE_NULL)
		return;
	MItem* pItem = (MItem*)void_ptr;

	if(NULL == pItem)
		return;

	CGDepositPet _CGDepositPet;
	_CGDepositPet.setObjectID(pItem->GetID());
	if(-1 != right)
		_CGDepositPet.setIndex(right);
	else
		return;
	g_pSocket->sendPacket( &_CGDepositPet );	

	g_pTempInformation->SetMode(TempInformation::MODE_PETITEM_MOVETO_PETSTORAGE);
	g_pTempInformation->Value1 = right;
	g_pTempInformation->Value2 = pItem->GetGridX();
	g_pTempInformation->Value3 = pItem->GetGridY();
	g_pTempInformation->pValue = pItem;
	UI_LockItem(); 
}
void	 
UIMessageManager::Execute_UI_GET_KEEP_PETITEM(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_GET_KEEP_PETITEM");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	if (g_pTempInformation->GetMode() != TempInformation::MODE_NULL)
		return;
	MItem* pItem = (MItem*)void_ptr;

	if(NULL == pItem)
		return;

	CGWithdrawPet _CGWithdrawPet;
	_CGWithdrawPet.setObjectID(pItem->GetID());
	if(right<20 &&right>-1)
		_CGWithdrawPet.setIndex(right);
	else
		return;
	g_pSocket->sendPacket( &_CGWithdrawPet );	
	g_pTempInformation->SetMode(TempInformation::MODE_PETITEM_MOVETO_INVENTORY);
	g_pTempInformation->Value1 = right;
	g_pTempInformation->Value2 = pItem->GetGridX();
	g_pTempInformation->Value3 = pItem->GetGridY();
	g_pTempInformation->pValue = pItem;
	UI_LockItem();
}


void	 
UIMessageManager::Execute_UI_CLOSE_SMS_MESSAGE(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		if(gC_vs_ui.IsRunningSMSRecord())
			gC_vs_ui.CloseSMSRecord();
		if(gC_vs_ui.IsRunningSMSList())
			gC_vs_ui.CloseSMSList();
		gC_vs_ui.CloseSMSMessage(); 
		
	}

}
void
UIMessageManager::Execute_UI_SEND_SMS_MESSAGE(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	if (g_pTempInformation->GetMode() != TempInformation::MODE_NULL)
		return;

	std::list<std::string> *TempStr = (std::list<std::string>*)(void_ptr);

	if(NULL == TempStr || TempStr->size()>5)
		return ;
	
	CGSMSSend _CGSMSSend;
	_CGSMSSend.setCallerNumber(std::string((char *)left));
	_CGSMSSend.setMessage(std::string((char *)right));
	_CGSMSSend.clearString(); 
	std::list<std::string>::iterator itr = TempStr->begin();
	while(itr != TempStr->end())
	{
		std::string str = (*itr);
		_CGSMSSend.addString(str);
		itr++;
	}
	g_pSocket->sendPacket( &_CGSMSSend );
	
	gC_vs_ui.CloseSMSMessage();
	if(gC_vs_ui.IsRunningSMSList())
		gC_vs_ui.CloseSMSList();
	if(gC_vs_ui.IsRunningSMSRecord())
		gC_vs_ui.CloseSMSRecord();
}


void	 
UIMessageManager::Execute_UI_SMS_OPEN_LIST(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.RunSMSList();
		// sms ???? ???? ???? ??????
		CGSMSAddressList _CGSMSAddressList;
		g_pSocket->sendPacket( &_CGSMSAddressList );
	}
}

void	 
UIMessageManager::Execute_UI_SMS_RECORD(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		CGAddSMSAddress	_CGAddSMSAddress;
		_CGAddSMSAddress.setCharacterName(std::string((char *)left));
		_CGAddSMSAddress.setCustomName((std::string((char *)right)));
		_CGAddSMSAddress.setNumber((std::string((char *)void_ptr)));
		g_pSocket->sendPacket( &_CGAddSMSAddress );

		g_pTempInformation->SetMode(TempInformation::MODE_SMS_ADD_ADDRESS);
		g_pTempInformation->Value1 = (int)left;
		g_pTempInformation->Value2 = (int)right;
		g_pTempInformation->Value3 = (int)void_ptr;

	}

}


void	 
UIMessageManager::Execute_UI_SMS_DELETE(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	// ????????? ???? ??????
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		CGDeleteSMSAddress	_CGDeleteSMSAddress;
		_CGDeleteSMSAddress.setElementID((DWORD)left);
		g_pSocket->sendPacket( &_CGDeleteSMSAddress );

		g_pTempInformation->SetMode(TempInformation::MODE_SMS_DELETE_ADDRESS);
		g_pTempInformation->Value1 = left;
	}
}

void	 
UIMessageManager::Execute_UI_CLOSE_SMS_LIST(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseSMSList();
	}
}

void	 
UIMessageManager::Execute_UI_CLOSE_SMS_RECORD(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseSMSRecord();
	}
}
void	 
UIMessageManager::Execute_UI_SMS_ADD_SEND_LIST(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.AddSMSSendList((char*)left);
	}
}


void	 
UIMessageManager::Execute_UI_CLOSE_NAMING(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseNaming();
	}
}

void	 
UIMessageManager::Execute_UI_SELECT_NAMING(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		CGSelectNickname	_CGSelectNickName;
		_CGSelectNickName.setNicknameID((WORD)left);
		g_pSocket->sendPacket( &_CGSelectNickName );

//		g_pTempInformation->SetMode(TempInformation::MODE_NICKNAME_CHANGE_CUSTOM);
//		g_pTempInformation->Value1 = left;
	}
}


void	 
UIMessageManager::Execute_UI_CLOSE_NAMING_CHANGE(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseNamingChange();
	}
}


void	 
UIMessageManager::Execute_UI_CHANGE_CUSTOM_NAMING(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	// left : name, right : item ID(0=???????? void_ptr:penitem pointer
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		char szTemp[22];
		strcpy(szTemp, (char*) left);
		if(strlen(szTemp) == 0)
			return;
//		if (!IsValidID(szTemp, "-_"))
//		{
//			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString() );
//			bAllOK = FALSE;
//		}
//		else
//		{
//			char strName[80];
//			strcpy(strName, pChar->sz_name);
//
//			// ?????????? ?????? ??????????????? ??????????????
//			if (g_pChatManager->RemoveCurse(strName))
//			{
//				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_USER_REGISTER_INVALID_ID].GetString() );
//				bAllOK = FALSE;
//			}
//		}
		g_pChatManager->RemoveCurse(szTemp );

		// ??????????effect status???????? ???? ????????????????????????????????? ??????????.
		MItem* pItem = NULL;	
		
		if(right != 0)
			pItem = g_pInventory->GetItemToModify( right );//(MItem*)void_ptr;
		
		if(pItem!= NULL || right == 0)
		{
			CGModifyNickname	_CGModifyNickname; 
			_CGModifyNickname.setNickname(std::string((char *)szTemp));

			if(right != 0)
			{ 
				// ?????? ???????????????????? ??????????- ??????????????????
			//	Execute_UI_ITEM_USE(pItem->GetID(), 0, (void*)pItem);
				// ??????????????????????????????????.
				_CGModifyNickname.setNicknameID(pItem->GetID());
				g_pSocket->sendPacket( &_CGModifyNickname );

				g_pTempInformation->SetMode(TempInformation::MODE_NICKNAME_CHANGE_CUSTOM);
				g_pTempInformation->Value1 = left;					// ???? ????????????????
				g_pTempInformation->Value2 = pItem->GetItemType();  // ??????????????

				if(25 != g_pTempInformation->Value2)  // ????????????????????
					g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
			}
			else
			{
				// ???????????????????????????? ??????.
				_CGModifyNickname.setNicknameID(0);
				g_pSocket->sendPacket( &_CGModifyNickname );
				g_pTempInformation->SetMode(TempInformation::MODE_NICKNAME_CHANGE_CUSTOM);
				g_pTempInformation->Value1 = left;					// ???? ????????????????
				g_pTempInformation->Value2 = 0;  // 
			}
		}
		else
			UI_PopupMessage (UI_STRING_MESSAGE_FAIL_SEARCH_ITEM);
	}
}

void	 
UIMessageManager::Execute_UI_RUN_NAMING_CHANGE(int left, int right, void* void_ptr)
{ 
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		MItem* pItem = (MItem*)left;
		if(pItem->GetItemClass() != ITEM_CLASS_EVENT_GIFT_BOX)
			return;
		// pItem ????????????????????????????????????????
			// 1?????(2??????) - ????????????????????, ????????????????????
			// ????????(1??????) - ???? ?????????? ?????? ??????????????????????????????
		// pItem?????????????????????????????????????????? ???? ???????????????????????
			// ???????????????????????????? ???? ??????
		
		if(pItem->GetItemType() == 22 || pItem->GetItemType() == 25) // ?????? ??????????????
			gC_vs_ui.RunNamingChange(pItem, (char*)g_pPlayer->GetNickName().c_str());
//		// ????????????
		else if(pItem->GetItemType() == 24) // extra naming pen // ???????????? ??????????
			gC_vs_ui.RunNamingChange(pItem, (char*)(*g_pGameStringTable)[UI_STRING_MESSAGE_ADD_PLAYER_NICKNAME].GetString());
//		// ??????????????????
//		else if(pItem->GetItemType() == 25) // limited naming pen // ?????????????? ???? ??????????
//			gC_vs_ui.RunNamingChange(pItem, (char*)g_pPlayer->GetNickName().c_str());
		else if(pItem->GetItemType() == 23) // ??????????????????
		{
			MFakeCreature *pFakeCreature = (MFakeCreature *)g_pZone->GetFakeCreature(g_pPlayer->GetPetID());
			if(NULL != pFakeCreature)
			{
				MPetItem *pPetItem = pFakeCreature->GetPetItem();
				if(pPetItem != NULL)
				{
					if(2 == pPetItem->GetItemType()) // ???????????? ?????????????????? ??????
						UI_PopupMessage( UI_STRING_MESSAGE_PET_NAMING_WOLVERINE );
					else
						gC_vs_ui.RunNamingChange(pItem, (char*)pFakeCreature->GetNickName().c_str());
				}
			}
			else
				UI_PopupMessage( UI_STRING_MESSAGE_PET_NAMING_SUMMON );
		}
		//gC_vs_ui.RunNamingChange(pItem, char* szCustom);

	}
}

void	 
UIMessageManager::Execute_UI_CLOSE_QUEST_MANAGER(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseQuestManager();
	}
}
void	 
UIMessageManager::Execute_UI_CLOSE_QUEST_LIST(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseQuestList();
	}
}
void	 
UIMessageManager::Execute_UI_CLOSE_QUEST_DETAIL(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseQuestDetail();
	}
}
void	 
UIMessageManager::Execute_UI_CLOSE_QUEST_MISSION(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseQuestMission();
	}
}
void	 
UIMessageManager::Execute_UI_CLOSE_QUEST_ITEM(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseQuestItem();
	}
}
void	 
UIMessageManager::Execute_UI_CLOSE_QUEST_ICON(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		gC_vs_ui.CloseQuestIcon(left);
	}
}
void	 
UIMessageManager::Execute_UI_GQUEST_ACCEPT(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		CGGQuestAccept	_CGGQuestAccept;
		_CGGQuestAccept.setQuestID((DWORD)left);
		g_pSocket->sendPacket( &_CGGQuestAccept );
	}
}
void	 
UIMessageManager::Execute_UI_GQUEST_GIVEUP(int left, int right, void* void_ptr)
{
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}

	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		CGGQuestCancel	_CGGQuestCancel;
		_CGGQuestCancel.setQuestID((DWORD)left);
		g_pSocket->sendPacket( &_CGGQuestCancel );
	}
}
//-----------------------------------------------------------------------------
//
// GQuest Inventory?????? Item?????????????????
//
//-----------------------------------------------------------------------------
void
UIMessageManager::Execute_UI_ITEM_USE_GQUEST_ITEM(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_GQUEST_ITEM");
	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead())
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	MItem* pItem =  (MItem*)void_ptr ;//(MItem*)void_ptr; 

	if(NULL != pItem)
	{
		CGUseItemFromGQuestInventory _CGUseItemFromGQuestInventory;
		_CGUseItemFromGQuestInventory.setIndex( left );
		
		g_pSocket->sendPacket( &_CGUseItemFromGQuestInventory );
		
		g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_GQUEST_INVENTORY);
	}
}


void
UIMessageManager::Execute_UI_ITEM_USE_REQUEST_GUILD_LIST(int left, int right, void* void_ptr)
{

	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_REQUEST_GUILD_LIST");
	CGRequestGuildList _CGRequestGuildlist;	
	_CGRequestGuildlist.setGuildType(left);
	g_pSocket->sendPacket(&_CGRequestGuildlist); 

}


void
UIMessageManager::Execute_UI_ITEM_USE_UNION_INFO(int left, int right, void* void_ptr)
{

	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_UNION_INFO");
	CGRequestUnionInfo _CGRequestUnioninfo;	
	g_pSocket->sendPacket(&_CGRequestUnioninfo); 

}


void
UIMessageManager::Execute_UI_ITEM_USE_REQUEST_UNION(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_REQUEST_UNION");
	
	if(g_pUserInformation->GuildGrade != 1) // ???????????????????
	{
		UI_PopupMessage(UI_STRING_MESSAGE_REQUEST_UNION_ERROR_1);
		return;
	}
	else if(g_pUserInformation->dwUnionID != 0)
	{
		UI_PopupMessage(UI_STRING_MESSAGE_REQUEST_UNION_ERROR_2);
		return;
	}
	CGRequestUnion _CGRequestUnion;	
	_CGRequestUnion.setGuildID(left);
	g_pSocket->sendPacket(&_CGRequestUnion); 

	gC_vs_ui.CloseTeamInfoDialogBox();
}
void
UIMessageManager::Execute_UI_ITEM_USE_QUIT(int left, int right, void* void_ptr)
{	
	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_REQUEST_DENY");

	if(g_pUserInformation->GuildGrade != 1) // ???????????????????
	{
		UI_PopupMessage(UI_STRING_MESSAGE_REQUEST_UNION_ERROR_1);
		return;
	}

	CGQuitUnion   _CGQuitUnion;
	_CGQuitUnion.setGuildID(left);
	_CGQuitUnion.setQuitMethod(right);
	g_pSocket->sendPacket(&_CGQuitUnion); 

	gC_vs_ui.ClosePopupMessage();
	gC_vs_ui.CloseTeamInfoDialogBox();
}
void
UIMessageManager::Execute_UI_ITEM_USE_EXPER(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_EXPER"); 
	if(g_pUserInformation->GuildGrade != 1) // ???????????????????
	{
		UI_PopupMessage(UI_STRING_MESSAGE_REQUEST_UNION_ERROR_1);
		return;
	}
	else if(g_pUserInformation->bUnionGrade != 0)
	{
		UI_PopupMessage(UI_STRING_MESSAGE_REQUEST_UNION_ERROR_3);
		return;
	}
	CGExpelGuild  _CGExpelGuild;
	_CGExpelGuild.setGuildID(left);
	g_pSocket->sendPacket(&_CGExpelGuild); 

	gC_vs_ui.CloseTeamInfoDialogBox();
	gC_vs_ui.CloseTeamInfo();
	gC_vs_ui.CloseTeamList();
}

void
UIMessageManager::Execute_UI_ITEM_USE_UNION_ACCEPT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_UNION_ACCEPT");
	CGAcceptUnion  _CGAcceptUnion;
	_CGAcceptUnion.setGuildID(left);
	g_pSocket->sendPacket(&_CGAcceptUnion); 

	gC_vs_ui.CloseMailBoxDialogBox();
}

void
UIMessageManager::Execute_UI_ITEM_USE_UNION_DENY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_UNION_DENY");
	CGDenyUnion  _CGDenyUnion;
	_CGDenyUnion.setGuildID(left);
	g_pSocket->sendPacket(&_CGDenyUnion); 

	gC_vs_ui.CloseMailBoxDialogBox();

}


void
UIMessageManager::Execute_UI_ITEM_USE_UNION_QUIT_ACCEPT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_UNION_QUIT_ACCEPT");
	CGQuitUnionAccept _CGQuitUnionAccept;
	_CGQuitUnionAccept.setGuildID(left);
	g_pSocket->sendPacket(&_CGQuitUnionAccept); 

	gC_vs_ui.CloseMailBoxDialogBox();
}

void
UIMessageManager::Execute_UI_ITEM_USE_UNION_QUIT_DENY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_UNION_QUIT_DENY");
	CGQuitUnionDeny _CGQuitUnionDeny;
	_CGQuitUnionDeny.setGuildID(left);
	g_pSocket->sendPacket(&_CGQuitUnionDeny); 

	gC_vs_ui.CloseMailBoxDialogBox();
}




void
UIMessageManager::Execute_UI_RECALL_BY_NAME(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_RECALL_BY_NAME");
	char *szSelectedID = gC_vs_ui.GetTeamMember_SelectedID();
	if(szSelectedID != NULL && strlen(szSelectedID)>0)
	{
		char szBuf[128];
		sprintf(szBuf, "*recall %s", szSelectedID );
		CGSay _CGSay;
		_CGSay.setMessage( szBuf );	//pWansungString );
		_CGSay.setColor( 0 );
		g_pSocket->sendPacket( &_CGSay );
	}
}

void
UIMessageManager::Execute_UI_UI_MODIFY_TAX(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_UI_MODIFY_TAX");

	CGModifyTaxRatio _CGModifyTaxRatio;
	_CGModifyTaxRatio.setRatio(left);
	g_pSocket->sendPacket( &_CGModifyTaxRatio );

}


void
UIMessageManager::Execute_UI_APPOINT_SUBMASTER(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_APPOINT_SUBMASTER");

	CGAppointSubmaster _CGAppointSubmaster;
	_CGAppointSubmaster.setGuildID(left);
	_CGAppointSubmaster.setName(std::string((char *)void_ptr));
	g_pSocket->sendPacket( &_CGAppointSubmaster );

}

//2004.11.27
void
UIMessageManager::Execute_UI_DISPLAY_ITEM(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_SELECT_PERSNALSHOP_SLOT");
	
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	if(g_pStorage2 == NULL)
		return;	
	
	
	MItem* pItem = (MItem*)void_ptr;
	g_pStorage2->SetCurrent( 0 );	// ??????
	
	int slot = right;
	
	
	const MItem* pStorageItem = g_pStorage2->GetItem( slot );
	//TYPE_OBJECTID mouseItemID = pMouseItem->GetID();		
	
	// ????????????????????MItemTable????ItemMoveControl ????????????????
	// ?????????????? ??????????????????????????
	// 2006.12.26 by chyaya
	
	/*
	// Event GiftBox ???????????? ?????????????????
	if (pItem->GetItemClass()!=ITEM_CLASS_EVENT_GIFT_BOX
	//&& pItem->GetItemClass()!=ITEM_CLASS_VAMPIRE_AMULET
	//		&& pItem->GetItemClass()!=ITEM_CLASS_COUPLE_RING
	//		&& pItem->GetItemClass()!=ITEM_CLASS_VAMPIRE_COUPLE_RING
	//		&& !pItem->IsQuestItem()
	//		&& pItem->IsNormalItem()
	//		&& !((pItem->GetItemClass() == ITEM_CLASS_EVENT_TREE) && pItem->GetItemType() >= 13  )
	//		&& pItem->GetItemClass() != ITEM_CLASS_EVENT_ITEM
	//		&& pItem->GetItemClass() != ITEM_CLASS_CODE_SHEET
	//		&& !(pItem->GetItemClass() == ITEM_CLASS_LUCKY_BAG && pItem->GetItemType() == 3)
	)
	*/
	
	if(!pItem->GetItemMoveContorl().GetAttr(ITEMMOVE_CANNOT_DISPLAY_PERSNALSHOP)
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
		&& !pItem->GetCashItem()
#endif
		)
	{
		int current_storage = g_pStorage2->GetCurrent();
		const int storage_size = g_pStorage2->GetSize();
		const int storage_max = STORAGE_SLOT;
		
		
		for(int i = 0; i < storage_max; i++)
		{
			const MItem *p_slot_item = g_pStorage2->GetItem(i);
			
			// ????????????????????????~ ??????
			if(p_slot_item == NULL)
			{
				CGDisplayItem _CGDisplayItem;
				_CGDisplayItem.setIndex(i);
				_CGDisplayItem.setItemObjectID(pItem->GetID());
				_CGDisplayItem.setPrice(pItem->GetPersnalPrice());
				//_CGDisplayItem.setXY(pMouseItem->GetGridX(),pMouseItem->GetGridY());
				_CGDisplayItem.setXY(pItem->GetGridX(),pItem->GetGridY());
				g_pSocket->sendPacket( &_CGDisplayItem ); 
				break;
			}
		}
		current_storage ++;
		if(current_storage >= storage_size)
			current_storage = 0;
		
	}
	else
	{
		g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[UI_STRING_CANNOT_DISPLAY_PERSNALSHOP].GetString());
	}
}

void
UIMessageManager::Execute_UI_UNDISPLAY_ITEM(int left, int right, void* void_ptr)
{
	
	MItem* pMouseItem = (MItem*)void_ptr;
	const MItem* pStorageItem = g_pStorage2->GetItem( right );
	
	if (pMouseItem!=NULL)
	{
//		if (pStorageItem!=NULL)
//		{
			MItem* pRemovedItem = g_pStorage2->RemoveItem( left );
			
			if (pRemovedItem!=NULL)
			{
				CGUndisplayItem _CGUndisplayItem;
				_CGUndisplayItem.setIndex(left);
				_CGUndisplayItem.setItemObjectID(pRemovedItem->GetID());
				//_CGUndisplayItem.setXY(left,right);
				_CGUndisplayItem.setXY(pMouseItem->GetGridX(),pMouseItem->GetGridY());
				
				g_pSocket->sendPacket( &_CGUndisplayItem );
			}
//		}
	}
	DEBUG_ADD("[UI] Execute_UI_UNDISPLAY_ITEM");
}

 void
UIMessageManager::Execute_UI_STORE_SIGN(int left, int right, void* void_ptr)
 {
	 DEBUG_ADD("[UI] Execute_UI_STORE_SIGN");

	DEBUG_ADD_FORMAT("[Original] %s", (char*)void_ptr );

	char * pernalshop_message = (char*)void_ptr;
    CGStoreSign _CGStoreSign;
	
	char str[250];
	memset(str,0,250);
	strcpy(str, (char*)void_ptr);
	
	g_pChatManager->RemoveCurse( str );
	_CGStoreSign.setSign(str);
	g_pSocket->sendPacket( &_CGStoreSign );

	gC_vs_ui.ClosePersnalShopMessage();
 }

 void
 UIMessageManager::Execute_UI_STORE_OPEN(int left, int right, void* void_ptr)
 {
	 	
		if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_LOVE_CHAIN)) return  ; 

		DEBUG_ADD("[UI] Execute_UI_STORE_OPEN");
		
		CGStoreOpen _CGCGStoreOpen;
		gC_vs_ui.SetPersnalShoptoUI(2);
		g_pSocket->sendPacket( &_CGCGStoreOpen );
				
 }
 
 void
UIMessageManager::Execute_UI_STORE_CLOSE(int left, int right, void* void_ptr)
 {
	 DEBUG_ADD("[UI] Execute_UI_STORE_CLOSE");
	 
	 CGStoreClose _CGStoreClose;
	 gC_vs_ui.SetPersnalShoptoUI(3);
	 
	 g_pSocket->sendPacket( &_CGStoreClose );


 }

 void
UIMessageManager::Execute_UI_REQUEST_STORE_INFO(int left, int right, void* void_ptr)
 {
	 DEBUG_ADD("[UI] Execute_UI_REQUEST_STORE_INFO");

	 // 2026-05-22: GameMenu Button_SHOP'tan cagrildiginda m_pC_gamemenu hala set
	 // (Run sonrasi Finish DisappearWindow yapar ama pointer silinmez). CloseGameMenu
	 // ile pointer NULL ??shop window acilir. DeleteNew NULL-safe oldugundan baska
	 // call sites'tan (NPC click, item shop vb.) gelirse no-op.
	 gC_vs_ui.CloseGameMenu();

	 if(g_pPlayer->IsFlyingCreature()
		 || g_pPlayer->IsUndergroundCreature() 
		 //|| g_pPlayer->IsGroundCreature() 
		 || g_pPlayer->IsFakeCreature() 
		 || g_pPlayer->GetMoveDevice()== 1
		 || g_pPlayer->GetMoveDevice()== 2
		 || gC_vs_ui.IsRunningSlayerPortal()
		 || gC_vs_ui.IsRunningHorn()
		)
	 {
		 DEBUG_ADD("[UI] Execute_DON'T OPEN PERSNAL SHOP Cuz your's type can't open PersnalShop./......");
	 }
	 else
	 {
		 if(right == 1)
			 gC_vs_ui.SetPersnalShoptoUI(1); 
	 

		 CGRequestStoreInfo _CGRequestStoreInfo;
		 _CGRequestStoreInfo.setOwnerObjectID(left);
		 g_pSocket->sendPacket( &_CGRequestStoreInfo );
	 }

 }

 void
UIMessageManager::Execute_UI_MY_STORE_INFO(int left, int right, void* void_ptr)
 {

	 DEBUG_ADD("[UI] Execute_UI_MY_STORE_INFO"); // ???????????????? ????????
	 
	 GCMyStoreInfo _GCMyStoreInfo;
	 _GCMyStoreInfo.getStoreInfo();

//	 if (g_pStorage2!=NULL)
//		 delete g_pStorage2;
//	 
//	 g_pStorage2 = new MStorage;
//	 
//	 g_pStorage2->Init( left ); 
//	 gC_vs_ui.RunPersnalShop();
//	 gC_vs_ui.SetPersnalShop( g_pStorage2 );
//	 
	 
	 g_pSocket->sendPacket( &_GCMyStoreInfo );

 }

void
UIMessageManager::Execute_UI_OTHER_STORE_INFO(int left, int right, void* void_ptr)
{
	// not sure
		 DEBUG_ADD("[UI] Execute_UI_OTHER_STORE_INFO");
	 
	  GCOtherStoreInfo _GCOtherStoreInfo;
	 _GCOtherStoreInfo.setObjectID(g_pTopView->GetSelectedCreature());
	  g_pSocket->sendPacket( &_GCOtherStoreInfo );
}

void
UIMessageManager::Execute_UI_BUY_STORE_ITEM(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_BUY_STORE_ITEM");
	
	if(g_pTopView->GetSelectedCreature() == NULL)
		return;
	MItem* pItem = (MItem*)void_ptr;		// inventory???????? ??????????NULL?????????? ????)
	
	if(pItem != NULL)		
	{
		CGBuyStoreItem _CGBuyStoreItem;
		_CGBuyStoreItem.setIndex(left);
		_CGBuyStoreItem.setItemObjectID(pItem->GetID());
		_CGBuyStoreItem.setOwnerObjectID(gC_vs_ui.GetOtherObjectID());
		
		g_pSocket->sendPacket( &_CGBuyStoreItem );
	}

}

void
UIMessageManager::Execute_UI_REMOVE_STORE_ITEM(int left, int right, void* void_ptr)
{
	 DEBUG_ADD("[UI] Execute_UI_REMOVE_STORE_ITEM");
	 
	 GCRemoveStoreItem _GCRemoveStoreItem;
	 _GCRemoveStoreItem.setIndex(0);
	 _GCRemoveStoreItem.setOwnerObjectID(g_pPlayer->GetID());
	 g_pSocket->sendPacket( &_GCRemoveStoreItem );
}

void
UIMessageManager::Execute_UI_ADD_STORE_ITEM(int left, int right, void* void_ptr)
{

	DEBUG_ADD("[UI] Execute_UI_ADD_STORE_ITEM");
	 
	 GCAddStoreItem _GCAddStoreItem;
	 _GCAddStoreItem.setIndex(0);
	 _GCAddStoreItem.setOwnerObjectID(g_pPlayer->GetID());
	 g_pSocket->sendPacket( &_GCAddStoreItem );
}


void
UIMessageManager::Execute_UI_CLOSE_PERSNALSHOP(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_PERSNALSHOP");
	gC_vs_ui.ClosePersnalShop();	
	
	if (g_pStorage2!=NULL)
	{
		delete g_pStorage2;
		g_pStorage2 = NULL;
	}
	 
	gC_vs_ui.SetPersnalShop( NULL );
}

void
UIMessageManager::Execute_UI_CLOSE_POWER_JJANG(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_POWER_JJANG");
	gC_vs_ui.ClosePowerjjang();
}
void
UIMessageManager::Execute_UI_REQUEST_POWER_JJANG_POINT(int left, int right, void* void_ptr)
{ 
	DEBUG_ADD("[UI] Execute_UI_REQUEST_POWER_JJANG_POINT");

	CGRequestPowerPoint _CGRequestPowerPoint;
	_CGRequestPowerPoint.setCellNum(std::string((char *)void_ptr));
	g_pSocket->sendPacket( &_CGRequestPowerPoint );
}
void
UIMessageManager::Execute_UI_POWER_JJANG_GAMBLE(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_POWER_JJANG_GAMBLE");

	CGUsePowerPoint _CGUsePowerPoint;
	g_pSocket->sendPacket( &_CGUsePowerPoint );
}


void
UIMessageManager::Execute_UI_CLOSE_SWAPADVANCEMENTITEM(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_SWAPADVANCEMENTITEM");

	gC_vs_ui.FinishSwapAdvancementItem();
}

void
UIMessageManager::Execute_UI_SWAPADVANCEMENTITEM(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_SWAPADVANCEMENTITEM");
	
	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	
	if (g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{
		const MItem* pItem = g_pInventory->GetItem( left, right );

		if (pItem!=NULL && g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
		{
			// Event GiftBox ???????????? ?????????????????
			
			if (!pItem->IsUniqueItem() && !pItem->IsQuestItem())
			{
				//-----------------------------------------------------
				// ???????????? ???? ?????? packet??????????????
				//-----------------------------------------------------
					CGShopRequestSell	_CGShopRequestSell;

					_CGShopRequestSell.setObjectID( (*g_pPCTalkBox).GetNPCID() );		// NPC ID	
					_CGShopRequestSell.setOpCode( SHOP_REQUEST_SWAP_ADVANCEMENT_ITEM );
					_CGShopRequestSell.setItemObjectID( pItem->GetID() );	// Item Object ID

					g_pSocket->sendPacket( &_CGShopRequestSell );
			
						
					//-------------------------------------------------
					// ???????????????? Temp Information??????
					//-------------------------------------------------
					(*g_pTempInformation).Mode	= TempInformation::MODE_SHOP_SELL;
					(*g_pTempInformation).pValue = (void*)pItem;

					//-------------------------------------------------
					// ???? ?????????? ?????? ??????????..
					//-------------------------------------------------
					UI_LockItemTrade();
			}
			else
			{
				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[UI_STRING_MESSAGE_SWAP_ADVANCEMENT_ITEM_ERROR ].GetString());
			}
		}
		else
		{
			DEBUG_ADD("[Error] Item Is NULL");
		}
	}
}

void
UIMessageManager::Execute_UI_LEARN_ADVANCE_SKILL(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_LEARN_ADVANCE_SKILL");
	CGLearnSkill _CGLearnSkill;
	
	_CGLearnSkill.setSkillType( left );
	_CGLearnSkill.setSkillDomainType( right );
	
	g_pSocket->sendPacket( &_CGLearnSkill );
}

// 2005, 1, 11, sobeit add start - ???????? ?????? ?????? ??????
void
UIMessageManager::Execute_UI_CAMPAIGN_HELP(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CAMPAIGN_HELP");
	
	// ?????? ?????? ???? ??????????- ???? * 10000

	if(left>0)
	{
		CGDonationMoney _CGDonationMoney;
		_CGDonationMoney.setGold(left*10000);
		_CGDonationMoney.setDonationType(right);
		g_pSocket->sendPacket( &_CGDonationMoney );
	}
// 2005, 1, 11, sobeit add end - ???????? ?????? ?????? ??????
}

// 2005, 1, 20, sobeit add start - ?????????????? 
void
UIMessageManager::Execute_UI_RUN_NEXT_GQUEST_EXCUTE_ELEMENT(int left, int right, void* void_ptr)
{
	// quest element List?????????????????? element?????????? ????.
	DEBUG_ADD("[UI] Execute_UI_RUN_NEXT_GQUEST_EXCUTE_ELEMENT");
	gC_vs_ui.RunNextGQuestExcuteElement();
}

void
UIMessageManager::Execute_UI_GQUEST_SET_ACTION(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_GQUEST_SET_ACTION");
	// ???? element ?????? ???????? ??????????????.
	if(left == 1) // dead
	{
		g_pPlayer->SetDead();
		g_pPlayer->SetResurrectZoneID(right);
	}
}

void
UIMessageManager::Execute_UI_GQUEST_ENDING_EVENT(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_GQUEST_ENDING_EVENT");
	// ???? ???????????????????? ???????? ?????? ????.
	MEvent event;
	event.eventID = EVENTID_ADVANCEMENT_QUEST_ENDING;
	event.eventType = EVENTTYPE_ZONE;
	event.eventFlag = EVENTFLAG_ONLY_EVENT_BACKGROUND | EVENTFLAG_NOT_DRAW_UI | EVENTFLAG_NOT_DRAW_CREATURE | EVENTFLAG_NOT_DRAW_INFORMATION | EVENTFLAG_NOT_DRAW_CREATURE_SHADOW | EVENTFLAG_NOT_DRAW_ITEM | EVENTFLAG_NOT_DRAW_MOUSE_POINTER | EVENTFLAG_NOT_DRAW_EFFECT | EVENTFLAG_DENY_INPUT | EVENTFLAG_NOT_FADE_SCREEN | EVENTFLAG_NOT_PLAY_SOUND;
	switch(left)
	{
	case 1: // ?????????? ?????? ?????????????? ??????
		event.parameter1 = 0;
		event.parameter4 = 1;
		g_pEventManager->AddEvent(event);
		break;
	case 2: // ?????????? ?????? ?????????????? ??????
		event.parameter1 = 0;
		event.parameter4 = 3;
		g_pEventManager->AddEvent(event);
		break;
	case 3: // ?????????? ?????? ?????????????? ??????
		event.parameter1 = 0;
		event.parameter4 = 5;
		g_pEventManager->AddEvent(event);
		break;
	}
	
}
// 2005, 1, 20, sobeit add end - ?????????????? 

// 2005, 1, 24, sobeit add start - ?????????????? ????????????
void
UIMessageManager::Execute_UI_REQUEST_EVENT_ITEM(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_REQUEST_EVENT_ITEM");
	{
		CGGetEventItem _CGGetEventItem;
		_CGGetEventItem.setEventType(left);
		g_pSocket->sendPacket( &_CGGetEventItem );
	}
}
// 2005, 1, 24, sobeit add end - ?????????????? ????????????

// 2005, 2, 25, sobeit add start - ?????? ?????????? ??????
void
UIMessageManager::Execute_UI_CLOSE_INVENTORY_SUB(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_CLOSE_INVENTORY_SUB");
	{
		gC_vs_ui.CloseSubInventory(left);
	}
}
//////////////////////////////////////////////////////////////////////////
//20070330 by diesirace?????? ?????????????????????????
//////////////////////////////////////////////////////////////////////////

void
UIMessageManager::Execute_UI_ITEM_INSERT_TO_INVENTORY_SUB(int left, int right, void* void_ptr)
{
	int insertX = HIWORD(right), insertY = LOWORD(right);
	TYPE_OBJECTID subInventoryID = left;

	DEBUG_ADD("[UI] Execute_UI_ITEM_INSERT_TO_INVENTORY_SUB");
	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead() ||
		!gC_vs_ui.IsRunningSubInventory(subInventoryID))
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}

	MItem* pItem = (MItem*)void_ptr;
	MItem* pMouseItem = UI_GetMouseItem();	// ?????? mouse????item

	if (pMouseItem==NULL)
	{
		DEBUG_ADD("[Error] Mouse Item is NULL");
		return;
	}

	if (pItem==NULL)
	{
		DEBUG_ADD("[Error] InvenItem is NULL");
		return;
	}

	TYPE_OBJECTID mouseItemID = ((pMouseItem==NULL)?OBJECTID_NULL : pMouseItem->GetID());

	if (pItem->IsPileItem())
	{
		if (pItem->GetItemClass()==pMouseItem->GetItemClass()
			&& pItem->GetItemType()==pMouseItem->GetItemType()
			&& pItem->GetNumber() < pItem->GetMaxNumber())
		{
			//----------------------------------------------------
			// pMouseItem????pItem????????????????
			//----------------------------------------------------
			int total = pMouseItem->GetNumber() + pItem->GetNumber();
			if ( total > pItem->GetMaxNumber() )
			{
				// ???? ???????????? ?????
				pMouseItem->SetNumber( total - pItem->GetMaxNumber() );
				pItem->SetNumber( pItem->GetMaxNumber() );
			}
			else
			{
				// ?????pItem???????????????????? ?????
				pItem->SetNumber( total );
				UI_DropItem();
			}
			//----------------------------------------------------
			// Server???????????????????
			//----------------------------------------------------
			CGAddMouseToInventory _CGAddMouseToInventory;
			_CGAddMouseToInventory.setObjectID( mouseItemID );
			_CGAddMouseToInventory.setInvenX( pItem->GetGridX() );
			_CGAddMouseToInventory.setInvenY( pItem->GetGridY() );
			_CGAddMouseToInventory.setInventoryItemObjectID(subInventoryID);
			g_pSocket->sendPacket( &_CGAddMouseToInventory );

		}
	}
}

void
UIMessageManager::Execute_UI_ITEM_DROP_TO_INVENTORY_SUB(int left, int right, void* void_ptr)
{
	int dropX = HIWORD(right), dropY = LOWORD(right);
	TYPE_OBJECTID subInventoryID = left;

	DEBUG_ADD("[UI] Execute_UI_ITEM_DROP_TO_INVENTORY_SUB");
	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead() ||
		!gC_vs_ui.IsRunningSubInventory(subInventoryID))
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}

	MItem* pMouseItem = UI_GetMouseItem();	// ?????? mouse????item

	if (pMouseItem==NULL)
	{
		DEBUG_ADD("[Error] Mouse Item is NULL");
		return;
	}

	TYPE_OBJECTID mouseItemID = ((pMouseItem==NULL)?OBJECTID_NULL : pMouseItem->GetID());


	//-----------------------------------------------------------------
	// ?????????????????????? ?????
	//-----------------------------------------------------------------
	if (g_pPlayer->IsItemCheckBufferNULL() && pMouseItem!=NULL
		&& g_pTempInformation->GetMode()==TempInformation::MODE_NULL)
	{		
		if(pMouseItem->GetItemClass() == ITEM_CLASS_SUB_INVENTORY)
			return;

		TYPE_OBJECTID toInventoryItemID = (pMouseItem==NULL)? OBJECTID_NULL : pMouseItem->GetID();
		
		CGAddMouseToInventory _CGAddMouseToInventory;
		_CGAddMouseToInventory.setObjectID( toInventoryItemID );
		_CGAddMouseToInventory.setInvenX( dropX );//pItem->GetGridX() );
		_CGAddMouseToInventory.setInvenY( dropY );//pItem->GetGridY() );
		_CGAddMouseToInventory.setInventoryItemObjectID(subInventoryID);

		g_pSocket->sendPacket( &_CGAddMouseToInventory );

		MItem* pOldItem = NULL;
		if (gC_vs_ui.ReplaceSubInventoryItem(subInventoryID,
											pMouseItem,		// ????????item
											dropX, dropY,	// ???????????? 
											pOldItem))		// ?????????? item
		{
			if (pOldItem != NULL) // replace ?????????
			{				
				UI_PickUpItem( pOldItem );
			}
			else
			{
				UI_DropItem();
			}
		}
	}
	//-----------------------------------------------------------------
	// ?????? ???????? ???? ???????????? ???? ?????
	//-----------------------------------------------------------------
	else
	{
		DEBUG_ADD( "[Error] There is another item in ItemCheckBuffer or Mouse NULL");		

		if(!g_pPlayer->IsItemCheckBufferNULL())
			DEBUG_ADD_FORMAT("[ITEMDROP] !g_pPlayer->IsItemCheckBufferNULL() %d", g_pPlayer->GetItemCheckBufferStatus());
		if(pMouseItem == NULL)
			DEBUG_ADD("[ITEMDROP] pMouseItem == NULL)");
		if(g_pTempInformation->GetMode()!=TempInformation::MODE_NULL)
			DEBUG_ADD_FORMAT("[ITEMPDROP] g_pTempInformation->Mode!=TempInformation::MODE_NULL %d", g_pTempInformation->Mode);
	}
}


void
UIMessageManager::Execute_UI_ITEM_PICKUP_FROM_INVENTORY_SUB(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] UI_ITEM_PICKUP_FROM_INVENTORY"); 
	
	MItem* pItem = (MItem*)void_ptr;
	
	MSubInventory* pSubInventory = dynamic_cast<MSubInventory*>(pItem->GetParentInventory());

	TYPE_OBJECTID subInventoryID = pSubInventory ? pSubInventory->GetID() : OBJECTID_NULL;

	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead() ||
		!gC_vs_ui.IsRunningSubInventory(subInventoryID))
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}

	UI_PickUpItem( pItem );
	//---------------------------------------------------
	// Inventory???????? item????mouse????????????-_-;)????
	// server????packet??????????????
	//---------------------------------------------------
	CGAddInventoryToMouse _CGAddInventoryToMouse;
	_CGAddInventoryToMouse.setObjectID( pItem->GetID() );
	_CGAddInventoryToMouse.setX( pItem->GetGridX() );
	_CGAddInventoryToMouse.setY( pItem->GetGridY() );
	_CGAddInventoryToMouse.setInventoryItemObjectID(subInventoryID);
	
	g_pSocket->sendPacket( &_CGAddInventoryToMouse );				
}

// 2005, 2, 25, sobeit add end - ?????? ?????????? ??????


// 2005, 3, 2, sobeit add start
void
UIMessageManager::Execute_UI_ITEM_USE_SUBINVENTORY(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_ITEM_USE_SUBINVENTORY"); 
	
	if (g_Mode!=MODE_GAME || g_pPlayer->IsDead() )
	{
		DEBUG_ADD("Not Mode MODE_GAME or Dead");
		return;
	}
	MItem* pSubInventory = g_pInventory->GetItem(right);

	if(NULL == pSubInventory)
		return;

	MItem* pItem = ((MSubInventory*)pSubInventory)->GetItemToModify( left );//(MItem*)void_ptr;
	
	if (pItem!=NULL && g_pPlayer->IsItemCheckBufferNULL())
	{
		// ?????????? ?????????? ??????-????;
		if(g_pPlayer->IsSlayer() && pItem->IsSlayerItem() ||
			g_pPlayer->IsVampire() && pItem->IsVampireItem() ||
			g_pPlayer->IsOusters() && pItem->IsOustersItem())
		{
			// ?????? ?????????? ???????????? ???????? ??????????.???? ?????? ...20070329
//		if(!(pItem->IsPileItem() && pItem->IsDetachableItem() && right))
			pItem->UseInventory(pSubInventory->GetID());
		}
	}
	//----------------------------------------------------
	// item check buffer not null
	//----------------------------------------------------
	else
	{
		#ifdef OUTPUT_DEBUG
			if (pItem==NULL)
			{
				DEBUG_ADD("[Wait] Item is NULL");
			}
			else 
			{
				DEBUG_ADD("[Wait] Item Check Buffer is Not NULL");
			}
		#endif
	}
}

// 2005, 3, 2, sobeit add end



// 2005, 4, 21, sjheon add start -???????????? ??????
void
UIMessageManager::Execute_UI_WEDDING_CONTRIBUTION(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_WEDDING_CONTRIBUTION");
	
	// ????  ????????

	if(left>0)
	{
		CGDonationMoney _CGDonationMoney;
		_CGDonationMoney.setGold(left*10000);
		_CGDonationMoney.setDonationType(right);
		g_pSocket->sendPacket( &_CGDonationMoney );
	}
// 2005, 4, 21, sjheon add end - ???????????? ??????
}

void 
UIMessageManager::Execute_UI_NETMARBLE_AGREEMENT( int left, int right, void* void_ptr )
{
	CLAgreement _CLAgreement;	
	_CLAgreement.setAgree( left == 1 );

	g_pSocket->sendPacket( &_CLAgreement);
}

void 
UIMessageManager::Execute_UI_BLOOD_BURST( int left, int right, void* void_ptr )
{
	int SkillType = 0;

	if(left == 1)
	{
		if(g_pPlayer->IsSlayer())
		{
			int domain_level_Sword = (*g_pSkillManager)[SKILLDOMAIN_SWORD].GetDomainLevel();
			int domain_level_Blade = (*g_pSkillManager)[SKILLDOMAIN_BLADE].GetDomainLevel();
			int domain_level_Gun = (*g_pSkillManager)[SKILLDOMAIN_GUN].GetDomainLevel();
			int domain_level_Heal = (*g_pSkillManager)[SKILLDOMAIN_HEAL].GetDomainLevel();
			int domain_level_Enchant = (*g_pSkillManager)[SKILLDOMAIN_ENCHANT].GetDomainLevel();

			int domainLevel = 0 ;			
			domainLevel = max(domain_level_Sword , domain_level_Blade)  ;
			domainLevel = max(domainLevel , domain_level_Gun)  ;
			domainLevel = max(domainLevel , domain_level_Heal)  ;
			domainLevel = max(domainLevel , domain_level_Enchant)  ; 
		
			if(domainLevel == domain_level_Sword)
				SkillType = SKILL_HOLY_SWORD ; 
			else if(domainLevel == domain_level_Blade)
				SkillType = SKILL_HOLY_BLADE ; 
			else if(domainLevel == domain_level_Gun)
				SkillType = SKILL_BURST_GUN ; 
			else if(domainLevel == domain_level_Enchant)
				SkillType = SKILL_HOLY_MACE ; 
			else if(domainLevel == domain_level_Heal)
				SkillType = SKILL_HOLY_CROSS ; 
			
			//SkillType = SKILL_HOLY_CROSS ; 
		}
		else if(g_pPlayer->IsVampire())
			SkillType = SKILL_DARK_FORCE ; 
		
		else if(g_pPlayer->IsOusters())
		{
		
			MCreatureWear *pCreatureWear = (MCreatureWear *)g_pPlayer;
			const MCreatureWear::ADDON_INFO& addonInfoChakram = g_pPlayer->GetAddonInfo(ADDON_RIGHTHAND);
			bool bChakram = addonInfoChakram.bAddon && addonInfoChakram.ItemClass == ITEM_CLASS_OUSTERS_CHAKRAM;

			bool bWristlet = addonInfoChakram.bAddon && addonInfoChakram.ItemClass == ITEM_CLASS_OUSTERS_WRISTLET;
			
			
			if(bChakram)
				SkillType = SKILL_NATURAL_FORCE ; 
			else if(bWristlet)
			{
				int ItemClass = addonInfoChakram.ItemClass	; 
				int ItemType  = addonInfoChakram.ItemType	;
				ITEMTABLE_INFO iInfo = (*g_pItemTable)[ItemClass][ItemType];
				
				if(iInfo.ElementalType == ITEMTABLE_INFO::ELEMENTAL_TYPE_FIRE)
					SkillType = SKILL_FIRE_FORCE ; 
				else if(iInfo.ElementalType == ITEMTABLE_INFO::ELEMENTAL_TYPE_WATER)
					SkillType = SKILL_WATER_FORCE ; 
				else if(iInfo.ElementalType == ITEMTABLE_INFO::ELEMENTAL_TYPE_EARTH)
					SkillType = SKILL_EARTH_FORCE ; 

			}
			else return ;
		}
	}
	else if(left ==  2)
	{
		if(g_pPlayer->IsSlayer())
			SkillType = SKILL_LOSE_SIGHT ; 
		else if(g_pPlayer->IsVampire())
			SkillType = SKILL_WIDE_BLOOD_DRAIN ; 
		else if(g_pPlayer->IsOusters())
			SkillType = SKILL_NATURAL_PEACE ; 
	}
	else if(left ==  3)
	{
		SkillType = SKILL_PARTY_AURA ; 
	}

	CGSkillToSelf _CGSkillToSelf; 
	_CGSkillToSelf.setSkillType( SkillType );
	_CGSkillToSelf.setCEffectID( 0 );
	g_pSocket->sendPacket( &_CGSkillToSelf );


	// 2001.8.20 ????????
	g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_SKILL_SUCCESS, SkillType );
}

void 
UIMessageManager::Execute_UI_CLIENT_REMOVE_EFFECT_STATUS( int left, int right, void* void_ptr )
{
	int iEffectState = 0 ; 
	if(left == SKILL_CLIENT_MAGICAL_PET_CHANGER)
	{	
		UI_RemoveEffectStatus(EFFECTSTATUS_CLIENT_MAGICAL_PET_CHANGER) ; 
	}
#if __CONTENTS(__EXPERT_PET_CHANGER)
	else if(left == SKILL_CLIENT_EXPERT_PET_CHANGER)
	{	
		UI_RemoveEffectStatus(EFFECTSTATUS_CLIENT_EXPERT_PET_CHANGER) ; 
	}
#endif //__EXPERT_PET_CHANGER
}


void 
UIMessageManager::Execute_UI_MARKET_ACCOUNT( int button, int state, void* void_ptr )
{
	// 2026-05-22: GameMenu Button_GOODLIST'tan cagrildiginda m_pC_gamemenu hala set.
	// CloseGameMenu ile pointer NULL ??market window acilir. DeleteNew NULL-safe.
	gC_vs_ui.CloseGameMenu();

	if(button == 0 && state == 0)
	{
		CGRequestWebMarket	_CGRequestWebMarket	;
		_CGRequestWebMarket.setCode(CGRequestWebMarket::REQUEST_WEB_MARKET)	; 
		g_pSocket->sendPacket( &_CGRequestWebMarket );
	}
	
	else if(button == 0 && state == 1)
	{
		CGRequestWebMarket	_CGRequestWebMarket	;
		_CGRequestWebMarket.setCode(CGRequestWebMarket::REQUEST_BUY_COMPLETE)	; 
		g_pSocket->sendPacket( &_CGRequestWebMarket );
	}	

	
	else if(button == 1 && state == 0 )
	{
		CGRequestWebMarket	_CGRequestWebMarket	; 
		_CGRequestWebMarket.setCode(CGRequestWebMarket::REQUEST_GOODS_LIST)	; 
		g_pSocket->sendPacket( &_CGRequestWebMarket );	
	}
	
}

void 
UIMessageManager::Execute_UI_NONPK_SERVER_USE( int left, int right, void* void_ptr )
{
	UI_PopupMessage(UI_STRING_MESSAGE_NONPKSERVER_USE, -1 ,-1, UI_DIALOG_TITLE_OK);
}	

void 
UIMessageManager::Execute_UI_PLAYER_PETINFO( int left, int right, void* void_ptr )
{
	MPetItem *pPetItem = NULL;
	PETINFO petInfo;	
	MFakeCreature *pFakeCreature = (MFakeCreature *)g_pZone->GetFakeCreature(g_pPlayer->GetPetID());
	if(pFakeCreature != NULL)
	{
		pPetItem = pFakeCreature->GetPetItem();
		if(pPetItem != NULL)
		{
			petInfo.HP = pPetItem->GetCurrentDurability();
			petInfo.ATTR = pPetItem->GetEnchantLevel();
			petInfo.ATTR_VALUE = pPetItem->GetSilver();
			petInfo.LEVEL = pPetItem->GetNumber();
			petInfo.FOODTYPE = pPetItem->GetPetFoodType();
			petInfo.EXP_REMAIN = pPetItem->GetPetExpRemain();
			petInfo.CUT_HEAD = pPetItem->IsCanCutHead();
			petInfo.GAMBLE = pPetItem->IsCanGamble();		
	
			if(pPetItem->GetItemOptionListCount() == 0)
#if __CONTENTS(!__NEW_PET_INCUBUS)
				petInfo.OPTION = -1;
			else
				petInfo.OPTION = pPetItem->GetItemOptionList().front();
#else
			{
				petInfo.OPTION_LIST.clear();
			}
			else
			{
				const std::list<TYPE_ITEM_OPTION> &optionList = pPetItem->GetItemOptionList();
				std::list<TYPE_ITEM_OPTION>::const_iterator itr= optionList.begin();
				
				for (; itr!=optionList.end() && *itr < g_pItemOptionTable->GetSize(); itr++)
				{
					petInfo.OPTION_LIST.push_back( *itr );
				}
			}
#endif //__NEW_PET_INCUBUS

			petInfo.ITEM_TYPE = pPetItem->GetItemType();
			petInfo.ITEM_ID = pPetItem->GetID();
			petInfo.NAME = pPetItem->GetPetName();
			petInfo.ATTACK = pPetItem->IsCanAttack();
			petInfo.ChangeAbleNickName = pFakeCreature->GetOwnerID() == g_pPlayer->GetID();
			petInfo.ENCHANTSKILLTYPE = pPetItem->GetEnchantSkillType();
#if __CONTENTS(__FUNCTION_PETFOOD)
			petInfo.FUNCTION_PETFOODTYPE = pPetItem->getFunctionPetfoodType();
#endif //__FUNCTION_PETFOOD
#if __CONTENTS(__PET_MIXINGFORGE)
			const std::list<TYPE_ITEM_OPTION> &petMixingOption = pPetItem->GetPetMixingOptionList();
			std::list<TYPE_ITEM_OPTION>::const_iterator itr = petMixingOption.begin();
			
			for(;itr != petMixingOption.end(); ++itr)
			{
				petInfo.MIXINGOPTION.push_back( *itr );
			}
#endif //__PET_MIXINGFORGE
			gC_vs_ui.SetPetInfo(&petInfo);
		}
	}
}


void
UIMessageManager::Execute_UI_SCREENSHOT(int left, int right, void* void_ptr)
{
	MakeScreenShot();
}

void
UIMessageManager::Execute_UI_FORCEATTACK_SET(int left, int right, void* void_ptr)
{
	bool ForceAttack = g_pCGameUpdate->GetForceAttack();
	
	if (ForceAttack)
	{
		g_pSystemMessage->Add((*g_pGameStringTable)[STRING_MESSAGE_FORCE_ATTACK_DISABLE].GetString());
		g_pCGameUpdate->SetForceAttack(false);
	}
	else
	{
		g_pSystemMessage->Add((*g_pGameStringTable)[STRING_MESSAGE_FORCE_ATTACK_ENABLE].GetString());
		g_pCGameUpdate->SetForceAttack(true);
	}
}


// by svi
#if __CONTENTS(__AUTO_ATTACT)	//__AUTO_ATTACT
void
UIMessageManager::Execute_UI_AUTOATTACK_SET(int left, int right, void* void_ptr)
{
	bool AutoAttack = g_pUserOption->DoNotOneClickAttackOff;
	
	if (AutoAttack)
	{
#ifdef __DESIGNED_CHS
		g_pSystemMessage->Add("OneClick Attack has been enabled.");
#else
		g_pSystemMessage->Add("OneClick Attack has been enabled.");
#endif
		g_pUserOption->DoNotOneClickAttackOff = false;
	}
	else
	{
#ifdef __DESIGNED_CHS
		g_pSystemMessage->Add("OneClick Attack has been disabled.");
#else
		g_pSystemMessage->Add("OneClick Attack has been disabled.");
#endif
		g_pUserOption->DoNotOneClickAttackOff = true;
	}
}
#endif //__AUTO_ATTACT
// end

void
UIMessageManager::Execute_UI_UPDATE_AVAILABLE_SKILLS(int left, int right, void* void_ptr)
{
	g_pSkillAvailable->SetAvailableSkills();
}


void
UIMessageManager::Execute_UI_PLAYER_ACTION_TO_SEND_PACKET(int left, int right, void* void_ptr)
{
	g_pPlayer->ActionToSendPacket(true);
}

void
UIMessageManager::Execute_UI_RELOAD_INFO_FILE(int left, int right, void* void_ptr)
{
	InitInfomation();
	InitSound();
}

void
UIMessageManager::ChatCommand_Where(STRING_VEC& ChatCommandStr, LPCTSTR pOrgMsg, int right)
{
	if(ChatCommandStr.size() < 2)
	{
		return;
	}
	const char* str;
	std::vector<std::string>::iterator CommandPos = ChatCommandStr.begin();
	std::string strSum;

	std::fill_n(CommandPos, 1, "*nerde");

	for(; CommandPos != ChatCommandStr.end(); ++CommandPos)
	{
		strSum += *CommandPos;
		if(CommandPos+1 != (ChatCommandStr.end()))
		{
			strSum += ' ';
		}
		
	}
	str = strSum.c_str();

	const char *pMessage = str;

	CGSay _CGSay;
	_CGSay.setMessage( pMessage );	//pWansungString );
	_CGSay.setColor( right );
	g_pSocket->sendPacket( &_CGSay );

}

void
UIMessageManager::ChatCommand_Whisper(STRING_VEC& ChatCommandStr, LPCTSTR pOrgMsg, int right)
{
	if(ChatCommandStr.size() < 3)
	{
		return;
	}

	char szBuffer[128];
	strcpy(szBuffer, pOrgMsg);

	CToken strToken(szBuffer + 2);
	
	// [????]+[ ]+[????] ???????????????????????? ????.
	const char* pName		= strToken.GetToken();
	const char* pMessage	= strToken.GetEnd();									
	
	if (pName!=NULL && pMessage!=NULL)
	{									
		int nameLen = strlen(pName);
		
		//------------------------------------------------------
		// ID?????????????????????									
		//------------------------------------------------------
		if (nameLen<PlayerInfo::minIDLength || nameLen>PlayerInfo::maxIDLength)
		{
			char strTemp[256];
			sprintf(strTemp, (*g_pGameStringTable)[STRING_USER_REGISTER_ID_LENGTH].GetString(), 
				PlayerInfo::minIDLength,
				PlayerInfo::maxIDLength);
			
			UI_AddChatToHistory( strTemp, NULL, CLD_INFO, right );
		}
		//------------------------------------------------------
		// ???????????? ??????..?????
		//------------------------------------------------------
		else
		{
			if (g_pUserInformation->CharacterID==pName)
			{
				// ???????? ???????????????
				UI_AddChatToHistory( (*g_pGameStringTable)[STRING_MESSAGE_WHISPER_SELF].GetString(), NULL, CLD_INFO, right );
			}
			else
			{
				// ????????????????? : ID + ' '
				char strWhisperID[128];
				sprintf(strWhisperID, "%s ", pName);
				g_pUserInformation->WhisperID = strWhisperID;
				
				/*
				CGWhisper _CGWhisper;
				_CGWhisper.setName( pName );
				_CGWhisper.setMessage( pMessage );
				
				  g_pSocket->sendPacket( &_CGWhisper );
				*/
				g_pWhisperManager->SendWhisperMessage( pName, pMessage, right );
				
				
				char strMessage[128];
				char strName[128];
				//sprintf(temp, "[%s] <%s> %s", g_pUserInformation->CharacterID.GetString(), pName, pMessage);
				//UI_AddChatToHistory( temp );
				strcpy(strMessage, pMessage);
				// "[??????] ????????????> ???????????????????????????????.
				sprintf(strName, "[%s] %s", g_pUserInformation->CharacterID.GetString(), pName);
				UI_AddChatToHistory( strMessage, strName, CLD_WHISPER, right );
				
				// [??????] ??????????????
				//											__BEGIN_HELP_EVENT
				////												ExecuteHelpEvent( HE_CHAT_WHISPER );	
				//											__END_HELP_EVENT
			}
		}
	}
}

#if __CONTENTS(__GEAR_SWAP_CHANGE) // UIMessageManager::Execute_UI_GEAR_CHANGE_SEND_PACKET
void
UIMessageManager::Execute_UI_GEAR_CHANGE_SEND_PACKET(int left, int right, void* void_ptr)
{
	CGChangeGearSlot _ChageGearSlot;
	DWORD	dwGearID	= 0;

	switch(g_pPlayer->GetRace())
	{
	case RACE_SLAYER:
		dwGearID	= g_pSlayerGear->GetSendGearID();
		g_pSlayerGear->SetProcessingGearChange(true);
		break;

	case RACE_VAMPIRE:
		dwGearID	= g_pVampireGear->GetSendGearID();
		g_pVampireGear->SetProcessingGearChange(true);
		break;
				
	case RACE_OUSTERS:
		dwGearID	= g_pOustersGear->GetSendGearID();
		g_pOustersGear->SetProcessingGearChange(true);
		break;
	}

/*	DWORD	dwGearSize	= gC_vs_ui.GetGearSize();
	
	MItem* pItem	= NULL;
	for(int i = 0; i < dwGearSize; i++)
	{
		if(i	!= 0)	// ID 0???????? ???????? ??????.
		{
			g_pPlayer->RemoveAddon(i);
			pItem	=	g_pPlayer->GetGearItemIndexFromAddonInfo(i);

			if(pItem)
			{
				g_pPlayer->SetAddonItem(pItem);			
			}
		}
	}
*/
	_ChageGearSlot.setGearID(dwGearID);
	g_pSocket->sendPacket( &_ChageGearSlot );	

/*	switch(g_pPlayer->GetRace())
	{
	case RACE_SLAYER:
		g_pSlayerGear->SetProcessingGearChange(false);
		break;

	case RACE_VAMPIRE:
		g_pVampireGear->SetProcessingGearChange(false);
		break;
				
	case RACE_OUSTERS:
		g_pOustersGear->SetProcessingGearChange(false);
		break;
	}
*/
}
#endif	// __GEAR_SWAP_CHANGE



#if __CONTENTS(__TIPOJYU_CASTLE)	//UIMessageManager::Init() UI ???????????????????? ????
void
UIMessageManager::Execute_UI_TIPOJYU_MOVE_SEND_PACKET(int left, int right, void* void_ptr)
{
	CGInstanceDoungeonEnter _InstanceDoungeonEnter;

	_InstanceDoungeonEnter.setObjectID(left);				//NPC ID
	_InstanceDoungeonEnter.setSelectDoungeonIndex(right);

	g_pSocket->sendPacket( &_InstanceDoungeonEnter );
}
#endif	//__TIPOJYU_CASTLE

#if __CONTENTS(__ITEM_SHOW)			//???????????????? ????????
void
UIMessageManager::Execute_UI_ITEM_SHOW_AND_CREATE_ITEM(int left, int right, void* void_ptr)
{
	CGSay _CGSay;
	char Message[256];

	sprintf(Message,"*create %d %d",left,right);
	_CGSay.setMessage( Message );
	g_pSocket->sendPacket( &_CGSay );

	gC_vs_ui.CloseInventory();
	gC_vs_ui.HotKey_Inventory(true);
}

void
UIMessageManager::Execute_UI_ITEM_SHOW_AND_CLEAR_ITEM(int left, int right, void* void_ptr)
{
	CGSay _CGSay;
	char Message[256];

	sprintf(Message,"*command clearInventory");
	_CGSay.setMessage( Message );
	g_pSocket->sendPacket( &_CGSay );

	gC_vs_ui.CloseInventory();
	gC_vs_ui.HotKey_Inventory(true);
}
#endif	//__ITEM_SHOW

#if __CONTENTS(__MOVE_ZONE)
void
UIMessageManager::Execute_UI_MOVE_ZONE(int left, int right, void* void_ptr)
{
	CGSay _CGSay;
	char Message[256];
	
	sprintf(Message,"*warp %s",(char *)void_ptr);
	_CGSay.setMessage( Message );
	g_pSocket->sendPacket( &_CGSay );
	
	if(gpC_Gps != NULL)
	{
		if(gpC_Gps->GetGpsShow())
			gpC_Gps->NonActive();
	}
	gC_vs_ui.CloseWorldMap();
}
#endif //__MOVE_ZONE

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
void
UIMessageManager::Execute_UI_SELECT_WEEKITEM(int left, int right, void* void_ptr)
{	
	DEBUG_ADD("[UI] UI_SELECT_WEEKITEM");

	CGSelectWeekItem _CGSelectWeekItem;
	_CGSelectWeekItem.setSelectID(left);

	g_pSocket->sendPacket( &_CGSelectWeekItem );
}

void
UIMessageManager::Execute_UI_SELECT_ALL_WEEKITEM(int left, int right, void* void_ptr)
{	
	DEBUG_ADD("[UI] UI_SELECT_ALL_WEEKITEM");

	CGSelectAllWeekItem _CGSelectAllWeekItem;
	g_pSocket->sendPacket( &_CGSelectAllWeekItem );
}

void
UIMessageManager::Execute_UI_FINISH_PREMIUM_GIVEITEM_BUTTON(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_FINISH_PREMIUM_GIVEITEM_BUTTON");

	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	
	gC_vs_ui.FinishRequest_PremiumGiveItem();
}
#endif //__PREMIUM_GIVE_ITEM_UI

#if __CONTENTS(__REPAIRSYSTEM_RENEWAL)
void
UIMessageManager::Execute_UI_REQUEST_REPAIRINFO(int left, int right, void* void_ptr)
{
	DEBUG_ADD("[UI] Execute_UI_REQUEST_REPAIRINFO");

	if (g_Mode!=MODE_GAME)
	{
		DEBUG_ADD("Not Mode MODE_GAME");
		return;
	}
	MItem* p_selected_item =  (MItem *)void_ptr;

	CGRequestRepairInfo _CGRequestRepairInfo;
	if(p_selected_item)
		_CGRequestRepairInfo.setObjectID(p_selected_item->GetID());
	else
		_CGRequestRepairInfo.setObjectID(0);
	g_pSocket->sendPacket( &_CGRequestRepairInfo );	
//20090323?????? ????????UI_LockItemTrade??????????????????????????? 
//???? ?????????????? ???????????????????????????????? ?????????????????? ???????? ??????????? ????????.
	if(p_selected_item == NULL)
		UI_LockItemTrade();

}
#endif //__REPAIRSYSTEM_RENEWAL

#if __CONTENTS(__PET_MIXINGFORGE)
void
UIMessageManager::Execute_UI_CLOSE_PET_MIXING_FORGE(int left, int right, void *void_ptr)
{
	if(left == 0 )
	{
		gC_vs_ui.ClosePetMixingForge();
		g_pPlayer->ClearItemCheckBuffer();
		g_pTempInformation->SetMode( TempInformation::MODE_NULL );
	} else
	{
		g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[left].GetString() );		
	}
}

void 
UIMessageManager::Execute_UI_USE_PET_MIXING_FORGE(int left, int right, void* void_ptr)
{
	if( g_pPlayer->GetItemCheckBufferStatus() != MPlayer::ITEM_CHECK_BUFFER_USE_MIXING_ITEM ||
		g_pTempInformation->GetMode() != TempInformation::MODE_NULL )
		return;

	MItem *pFirstItem = g_pInventory->GetItemToModify( left );
	MItem *pSecondItem = g_pInventory->GetItemToModify( right );

	if( pFirstItem == NULL || pSecondItem == NULL )
	{
		return;
	}

	MItem* pMixingItem = g_pPlayer->GetItemCheckBuffer();
	
	CGMixItem _CGMixItem;
	
	_CGMixItem.setObjectID( pMixingItem->GetID() );
	_CGMixItem.setX( pMixingItem->GetGridX() );
	_CGMixItem.setY( pMixingItem->GetGridY() );
	_CGMixItem.setTargetObjectID( 0, pFirstItem->GetID() );
	_CGMixItem.setTargetObjectID( 1, pSecondItem->GetID() );
	
	g_pSocket->sendPacket( &_CGMixItem );
	g_pPlayer->SetItemCheckBuffer( pMixingItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY );
	
	g_pTempInformation->SetMode( TempInformation::MODE_ITEM_MIXING );
	g_pTempInformation->Value1 = pFirstItem->GetGridX();
	g_pTempInformation->Value2 = pFirstItem->GetGridY();
	
	g_pTempInformation->Value3 = pSecondItem->GetGridX();
	g_pTempInformation->Value4 = pSecondItem->GetGridY();
	
	g_pTempInformation->pValue = (void*) pMixingItem;

}
#endif //__PET_MIXINGFORGE


void
UIMessageManager::Execute_UI_DESTROY(int left, int right, void* void_ptr)
{
	Window* window = gpC_window_manager->GetWindowbyName((char*)void_ptr);
	if (window)
	{
		if (window->Running())
		{
			window->Finish();
			DeleteNew(window);
		}
		else
			DeleteNew(window);
	}
}

#if __CONTENTS(__FRIEND_ADDITION)
void
UIMessageManager::Execute_UI_FRIEND_INFO_DATA(int left, int right, void* void_ptr)
{
	// To do : ???????????? ???? ????????????????????????
	// void_ptr???????????????????? ?????? ????????????????

	CGFriendViewInfo _CGFriendInfoData;
	_CGFriendInfoData.SetPCName((char*)void_ptr);
	g_pSocket->sendPacket( &_CGFriendInfoData );
}

void
UIMessageManager::Execute_UI_FRIEND_SYSTEM_OPTION(int left, int right, void* void_ptr)
{
	// To do : ???? ???????????? ?????? ????
	// void_ptr???????? ??????????

	FriendGlobalOptionInfo* _OptionInfo = (FriendGlobalOptionInfo*)void_ptr;
	CGFriendUpdateGlobalOption _CGFriendGlobalOption;
	_CGFriendGlobalOption.SetFriendGlobalOptionInfo(*_OptionInfo);
	g_pSocket->sendPacket( &_CGFriendGlobalOption );
}

void
UIMessageManager::Execute_UI_FRIEND_INFO_OPTION(int left, int right, void* void_ptr)
{
	// To do : ???? ???????????? ?????? ????
	// void_ptr???????? ??????????

	typedef struct INFO_DATA
	{
		std::string						m_user_name;
		FriendIndividualOptionInfo		m_option_info;
	} InfoData;

	InfoData* _InfoData = (InfoData*)void_ptr;
	CGFriendUpdateIndividualOption _CGFriendIndividualOption;
	_CGFriendIndividualOption.SetFriendName(_InfoData->m_user_name);
	_CGFriendIndividualOption.SetFriendIndividualOptionInfo(_InfoData->m_option_info);
	g_pSocket->sendPacket( &_CGFriendIndividualOption );
}

void
UIMessageManager::Execute_UI_FRIEND_ADD(int left, int right, void* void_ptr)
{
	// To do : ???????????? ???? ????????????????????????
	// void_ptr????????????????????????????

	CGFriendReqToAdd _CGFriendAdd;
	_CGFriendAdd.setPCName((char*)void_ptr);
	DeleteNew(void_ptr);
	g_pSocket->sendPacket( &_CGFriendAdd );
}

void
UIMessageManager::Execute_UI_FRIEND_DEL(int left, int right, void* void_ptr)
{
	// To do : ?????????????? ????????????????????????
	// void_ptr??????????????????????????????

	CGFriendReqToDel _CGFriendDel;
	_CGFriendDel.setPCName((char*)void_ptr);
	DeleteNew(void_ptr);
	g_pSocket->sendPacket( &_CGFriendDel );
}

void
UIMessageManager::Execute_UI_FRIEND_RES_ADD(int left, int right, void* void_ptr)
{
	// To do : ???? ?????????????? ?????? ??????????????????
	// lef??????????
	// right?????????? 
	// void_ptr????????????????????

	CGFriendResToAdd _CGFriendResAdd;
	_CGFriendResAdd.SetResponse(FRIEND_ADD_RESPONSE_CODE_REJECT);
	if (left == 1)
		_CGFriendResAdd.SetResponse(FRIEND_ADD_RESPONSE_CODE_ACCEPT);
	else if (right == 1)
		_CGFriendResAdd.SetResponse(FRIEND_ADD_RESPONSE_CODE_REJECT);
	_CGFriendResAdd.SetPCName((char*)void_ptr);
	g_pSocket->sendPacket( &_CGFriendResAdd );
}
#endif //__FRIEND_ADDITION

void
UIMessageManager::Execute_UI_REMOVE_ITEM(int left, int right, void* void_ptr)
{
	g_pInventory->RemoveItem(left, right);
}

