//----------------------------------------------------------------------
// MPlayer.cpp
//----------------------------------------------------------------------
//
// m_nUsedActionInfo´Â ´ÙÀ½ turn¿¡ ¹Ù·Î ¼öÇàÇÏ°Ô µÇ´Â µ¿ÀÛÀÌ°í
//
// m_nNextUsedActionInfo´Â 
//          ÃßÀûÀÌ ³¡³ª°í ³ª¼­¾ß ¼öÇàÇÏ°Ô µÇ´Â µ¿ÀÛÀÌ´Ù.(bufferÀÇ ¿ªÇÒ)
//
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "crash_trace_tool.h"
#include <math.h>
#include <stdio.h>
#include "MZone.h"
#include "MTopView.h"
#include "MItem.h"
#include "MCreature.h"
#include "MInteractionObject.h"
#include "MLinearEffect.h"
#include "TempInformation.h"
#include "MParabolaEffect.h"
#include "MPlayer.h"
#include "MActionInfoTable.h"
#include "MEffectTarget.h"
#include "MEffectGeneratorTable.h"
#include "PacketDef.h" 
#include "ClientDef.h"
#include "MCreatureTable.h"
#include "MSoundTable.h"
#include "MItemTable.h"
#include "ClientConfig.h"
#include "MGameStringTable.h"
#include "MStatusManager.h"
#include "MItemOptionTable.h"
#include "TalkBox.h"
#include "UIDialog.h"
#include "MHelpManager.h"
#include "MPortal.h"
#include "PacketFunction.h"
#include "MGameTime.h"
#include "MObjectSelector.h"
#include "SoundDef.h"
#include "MItemFinder.h"
#include "packet\Cpackets\CGSelectTileEffect.h"
#include "packet\Cpackets\CGPartyInvite.h"
#include "EffectSpriteTypeDef.h"
#include "ModifyStatusManager.h"
#include "ClientCommunicationManager.h"
#include "MEventManager.h"
#include "MItemFinder.h"
#include "MZoneTable.h"

#include "Rpackets\RCPositionInfo.h"
#include "Rpackets\RCStatusHP.h"
#include "Rpackets\RCCharacterInfo.h"
#include "Cpackets\CGRequestInfo.h"
#include "Cpackets\CGUseItemFromGear.h"

#include "UserInformation.h"
#include "MParty.h"
#include "MJusticeAttackManager.h"
#include "RequestUserManager.h"
#include "MWarManager.h"

#include "MGuildType.h"
#include "ServerInfo.h"
#include "DebugInfo.h"
#include "MTestDef.h"
#include "RankBonusTable.h"
#include "RankBonusDef.h"
#include "MOustersGear.h"

#include "CPackets/CGAbsorbSoul.h"

#include "UIMessageManager.h"
#include "UIFunction.h"
#include "RequestClientPlayerManager.h"
#include "SystemAvailabilities.h"

#include "UserOption.h"
#include "OperatorOption.h"

#include "VS_UI_GameCommon2.h"

#include "Cpackets\CGPartyPosition.h"

// 2008.3.12 cheon ill zea - °è±Þ°æÇèÄ¡ º¯È­·® Ç¥½Ã¶§¹®
#include "ExperienceTable.h"

extern void UI_RunPetInfo(struct PETINFO *pPetInfo);
extern char g_CWD[_MAX_PATH];

#ifndef PHOENIX_MOVEMENT_AUDIT_ENABLED
#define PHOENIX_MOVEMENT_AUDIT_ENABLED 0
#endif

namespace
{
	// 2026-06-19 retarget deneyi bu sabiti 1 yapmıştı. VPS RTT'de her tile
	// GCMoveOK bekliyor → yürüyüş/motosiklet/yarasa takılması. Kullanılmıyor;
	// ActionMove referans eşiğine döndü (m_SendMove > MAX_CLIENT_MOVE).
	const int kPhoenixResponsiveMaxPendingMoves = 1;

	int Phoenix_GetConfiguredMaxClientMove()
	{
		if (g_pClientConfig == NULL || g_pClientConfig->MAX_CLIENT_MOVE < 1)
			return 1;
		return g_pClientConfig->MAX_CLIENT_MOVE;
	}

	int Phoenix_GetResponsiveMaxPendingMoves()
	{
		// Referans MPlayer.cpp: m_SendMove > g_pClientConfig->MAX_CLIENT_MOVE (default 6).
		// kPhoenixResponsiveMaxPendingMoves yalniz tarihsel kayit (2026-06-19 k=1 deneyi).
		static_assert(kPhoenixResponsiveMaxPendingMoves == 1, "June 19 pending-limit experiment value");
		return Phoenix_GetConfiguredMaxClientMove();
	}

#if PHOENIX_MOVEMENT_AUDIT_ENABLED
	void Phoenix_MovementAuditWrite(const char* stage, const char* detail, int x, int y, int destX, int destY, int dir, int sendMove)
	{
		char dataDir[_MAX_PATH];
		char logDir[_MAX_PATH];
		char logPath[_MAX_PATH];

		if (g_CWD[0] != '\0')
		{
			_snprintf(dataDir, sizeof(dataDir) - 1, "%s\\data", g_CWD);
			_snprintf(logDir, sizeof(logDir) - 1, "%s\\data\\log", g_CWD);
			_snprintf(logPath, sizeof(logPath) - 1, "%s\\data\\log\\mouse_click_audit.log", g_CWD);
		}
		else
		{
			_snprintf(dataDir, sizeof(dataDir) - 1, "data");
			_snprintf(logDir, sizeof(logDir) - 1, "data\\log");
			_snprintf(logPath, sizeof(logPath) - 1, "data\\log\\mouse_click_audit.log");
		}

		dataDir[sizeof(dataDir) - 1] = '\0';
		logDir[sizeof(logDir) - 1] = '\0';
		logPath[sizeof(logPath) - 1] = '\0';

		CreateDirectoryA(dataDir, NULL);
		CreateDirectoryA(logDir, NULL);

		FILE* fp = fopen(logPath, "a");
		if (fp == NULL)
			return;

		fprintf(fp,
			"tick=%lu move_stage=%s detail=%s x=%d y=%d dest=%d,%d dir=%d sendMove=%d\r\n",
			(unsigned long)GetTickCount(),
			stage != NULL ? stage : "-",
			detail != NULL ? detail : "-",
			x,
			y,
			destX,
			destY,
			dir,
			sendMove);
		fclose(fp);
	}
#else
#define Phoenix_MovementAuditWrite(stage, detail, x, y, destX, destY, dir, sendMove) ((void)0)
#endif
}

//#include "Client.h"
bool		GetMakeItemFitPosition(MItem* pItem, ITEM_CLASS itemClass, int itemType, POINT& fitPoint);

// [»õ±â¼ú]
void		SkillShadowDancing(MCreature* pUserCreature, MCreature* pTargetCreature, int skillID);
bool			IsCreatureMove( MCreature *pCreature );
bool			IsCreatureActionAttack( MCreature *pCreature );
bool			HasEffectStatusSummonSylph( MCreature* pCreature );
void			RemoveEffectStatusSummonSylph( MCreature* pCreature );
#if __CONTENTS(__FAST_TRANSFORTER)
bool			HasEffectStatusWingSylph( MCreature* pCreature );
void			RemoveEffectStatusWingSylph( MCreature* pCreature );
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
bool			HasEffectStatusUnicorn( MCreature* pCreature );
void			RemoveEffectStatusUnicorn( MCreature* pCreature );

bool			HasEffectStatusOusterNewWings1( MCreature* pCreature );
void			RemoveEffectStatusOusterNewWings1( MCreature* pCreature );

bool			HasEffectStatusOusterNewWings2( MCreature* pCreature );
void			RemoveEffectStatusOusterNewWings2( MCreature* pCreature );
#endif //__SECOND_TRANSFORTER
BYTE GetCreatureActionCountMax( const MCreature* pCreature, int action );

// #ifndef CONNECT_SERVER
	#include "MFakeCreature.h"
// #endif


#include "MTestDef.h"

#ifdef __METROTECH_TEST__
//	#define OUTPUT_DEBUG_PLAYER_ACTION
#endif



#include "VS_UI.h" // KJTINC
#include "VS_UI_Mouse_pointer.h"
#include "CImm.h"

extern	DWORD	g_CurrentTime;
extern	int		g_x;
extern	int		g_y;
extern	bool	g_bNetStatusGood;
extern BOOL					g_bLButtonDown;
extern BOOL					g_bRButtonDown;
extern BOOL					g_bCButtonDown;
extern bool g_bPreviousMove;

extern int					g_nZoneLarge;
extern int					g_nZoneSmall;
extern bool					g_bZonePlayerInLarge;
extern bool					g_bZoneSafe;
extern bool					g_bHolyLand;
extern POINT						g_SelectSector;

extern LONG					g_lGameRunBreakTime;

//#define	new			DEBUG_NEW

#define				TRADE_DISTANCE_NPC		4
#define				TRADE_DISTANCE_PC		2


//Test - Skill Cool Time Check TestÇÔ¼ö

//SkillTypeÀ¸·Î .. µ¿ÀÏÇÑ ½ºÅ³ÀÌ ¿¬¼Ó »ç¿ëµÉ ¶§, »ç¿ë½Ã°£ Â÷ÀÌ¸¦ SkillDelay.log¿¡ Ãâ·ÂÇØ ÁØ´Ù.
void TestCheckSkillCooltime (int SkillType, int WeaponSpeed)
{

	static DWORD _LastSkillTime = 0;
	static DWORD _LastSkillType = 0;

	if (_LastSkillType != SkillType)
	{
		_LastSkillType = SkillType;
		_LastSkillTime = g_CurrentTime;
	}
	else
	{
		ACTIONINFO_PACKET	packetType		= (*g_pActionInfoTable)[SkillType].GetPacketType();		//¾î¶² ¹æ½ÄÀÇ ½ºÅ³ÀÎ°¡?
		char strSkillInfo [256];
		switch (packetType)
		{
		case ACTIONINFO_PACKET_OTHER:
			wsprintf (strSkillInfo, "Skills used on others");
			break;
		case ACTIONINFO_PACKET_SELF:
			wsprintf (strSkillInfo, "Skills used on Self");
			break;
		case ACTIONINFO_PACKET_ZONE:
			wsprintf (strSkillInfo, "Skills used on Tile");
			break;
		default:
			wsprintf (strSkillInfo, "What could it be? %d", packetType);
			break;
		}
		const char *pSkillName = (*g_pActionInfoTable)[SkillType].GetName ();						//½ºÅ³ ÀÌ¸§
		int	StartFrame = (*g_pActionInfoTable)[SkillType].GetStartFrame( WeaponSpeed );	//½ÃÀÛ ÇÁ·¹ÀÓ

		std::ofstream file("SkillDelay.log", std::ios::out | std::ios::app);
		file << "*** SkillType = " << SkillType << "[" << pSkillName << "]" << " ***" << strSkillInfo << std::endl;
		file << "     WeaponSpeed=" << WeaponSpeed << "StartFrame =" << StartFrame << std::endl;
		file << "     Difftime=" << g_CurrentTime - _LastSkillTime << std::endl;
		
		file.close();

		_LastSkillTime = g_CurrentTime;
	}
}

//Test
void	MPlayer::SetWaitVerify(WAIT_VERIFY wv, TYPE_ACTIONINFO ai)
{
	m_WaitVerify=wv; m_WaitVerifyActionInfo=ai; 
}
void	MPlayer::SetWaitVerifyNULL()
{
	m_WaitVerify=WAIT_VERIFY_NULL; m_WaitVerifyActionInfo=ACTIONINFO_NULL; 
}

//----------------------------------------------------------------------
//
//							Global Player
//
//----------------------------------------------------------------------
MPlayer*			g_pPlayer = NULL;

//----------------------------------------------------------------------
// define Functions
//----------------------------------------------------------------------
// ¸· ½ÃÃ¼°¡ µÇ´Â ¼ø°£...
/*
#define GET_DYING_CREATURE( pCreature, id )								\
		if (pCreature==NULL)											\
		{																\
			MItem* pItem = g_pZone->GetItem( id );						\
																		\
			if (pItem!=NULL												\
				&& pItem->GetItemClass()==ITEM_CLASS_CORPSE)		\
			{															\
				pCreature = ((MCorpse*)pItem)->GetCreature();			\
																		\
				if (pCreature->GetActionCount() >= pCreature->GetActionCountMax())	\
				{														\
					pCreature = NULL;									\
				}														\
			}															\
		}
*/

//----------------------------------------------------------------------
// CHECK_REPEAT_LIMIT_TO_RETURN_FALSE
//----------------------------------------------------------------------
#define CHECK_REPEAT_LIMIT_TO_RETURN_FALSE(nActionInfo, repeatCount)		\
		if (nActionInfo < g_pActionInfoTable->GetSize())	\
		{													\
			if (repeatCount >= (*g_pActionInfoTable)[nActionInfo].GetRepeatLimit())	\
			{												\
				UnSetRepeatAction();						\
				return false;								\
			}												\
		}

//----------------------------------------------------------------------
// Æ÷¼Ç ÀÚµ¿ ¸Ô±â
//----------------------------------------------------------------------
#ifdef __METROTECH_TEST__

	int  g_iAutoHealPotion = 1;
	bool g_bAutoManaPotion = true;
	int	 g_iAutoReload = 1;
	bool g_bLight = false;
	int	 g_iSpeed = 0;

	void
	AutoUseSkill(int skill)
	{
		if (g_pSkillInfoTable != NULL
			&& g_pInventory!=NULL 
			&& g_pPlayer!=NULL
			&& g_pPlayer->IsSlayer()
			&& g_pPlayer->IsItemCheckBufferNULL()
			&& !g_pPlayer->IsInSafeSector())
		{
			bool bUse = false;
			int skill_id = ACTIONINFO_NULL;

			if(skill == 0)	//healing
			{
				const int hp_gap = g_pPlayer->GetMAX_HP()-g_pPlayer->GetHP();
				
				if(hp_gap > 0)
				{
					if(g_iAutoHealPotion == 2)	// ¿¡³ÊÁö ´Þ¸é ´Ü¸¸Å­ Ã¤¿î´Ù
					{
						if(g_pSkillAvailable->IsEnableSkill( MAGIC_CURE_CRITICAL_WOUNDS ) &&
							(*g_pSkillInfoTable)[MAGIC_CURE_CRITICAL_WOUNDS].IsEnable() && 
							(*g_pSkillInfoTable)[MAGIC_CURE_CRITICAL_WOUNDS].GetExpLevel()/2+30 <= hp_gap)
						{
							skill_id = MAGIC_CURE_CRITICAL_WOUNDS;
							bUse = true;
						}
						else
						if(g_pSkillAvailable->IsEnableSkill( MAGIC_CURE_SERIOUS_WOUNDS ) &&
							(*g_pSkillInfoTable)[MAGIC_CURE_SERIOUS_WOUNDS].IsEnable() && 
							(*g_pSkillInfoTable)[MAGIC_CURE_SERIOUS_WOUNDS].GetExpLevel()/4+30 <= hp_gap)
						{
							skill_id = MAGIC_CURE_SERIOUS_WOUNDS;
							bUse = true;
						}
						else
						if(g_pSkillAvailable->IsEnableSkill( MAGIC_CURE_LIGHT_WOUNDS ) &&
							(*g_pSkillInfoTable)[MAGIC_CURE_LIGHT_WOUNDS].IsEnable() && 
							(*g_pSkillInfoTable)[MAGIC_CURE_LIGHT_WOUNDS].GetExpLevel()/4+10 <= hp_gap)
						{
							skill_id = MAGIC_CURE_LIGHT_WOUNDS;
							bUse = true;
						}
					}
					else if(g_iAutoHealPotion == 3)	// ¿¡³ÊÁö ´Þ¸é Ã¤¿î´Ù
					{
						if(g_pSkillAvailable->IsEnableSkill( MAGIC_CURE_CRITICAL_WOUNDS ) &&
							(*g_pSkillInfoTable)[MAGIC_CURE_CRITICAL_WOUNDS].IsEnable())
						{
							skill_id = MAGIC_CURE_CRITICAL_WOUNDS;
							bUse = true;
						}
						else
						if(g_pSkillAvailable->IsEnableSkill( MAGIC_CURE_SERIOUS_WOUNDS ) &&
							(*g_pSkillInfoTable)[MAGIC_CURE_SERIOUS_WOUNDS].IsEnable())
						{
							skill_id = MAGIC_CURE_SERIOUS_WOUNDS;
							bUse = true;
						}
						else
						if(g_pSkillAvailable->IsEnableSkill( MAGIC_CURE_LIGHT_WOUNDS ) &&
							(*g_pSkillInfoTable)[MAGIC_CURE_LIGHT_WOUNDS].IsEnable())
						{
							skill_id = MAGIC_CURE_LIGHT_WOUNDS;
							bUse = true;
						}
					}
				}
			}

			if(skill == 1)	// create holy water
			{
				if(g_pSkillAvailable->IsEnableSkill( MAGIC_CREATE_HOLY_WATER ) &&
					(*g_pSkillInfoTable)[MAGIC_CREATE_HOLY_WATER].IsEnable() &&
					(*g_pSkillInfoTable)[MAGIC_CREATE_HOLY_WATER].IsAvailableTime())
				{
					skill_id = MAGIC_CREATE_HOLY_WATER;
					bUse = true;
				}
			}

			if(bUse == true && skill_id != ACTIONINFO_NULL)
			{
				// ½ºÅ³»ç¿ë
				g_pPlayer->UnSetRequestMode();
				
				TYPE_ACTIONINFO old_special = g_pPlayer->GetSpecialActionInfo();
				g_pPlayer->SetSpecialActionInfo(skill_id);
				g_pPlayer->SelfSpecialAction();
				g_pPlayer->SetSpecialActionInfo(old_special);
			
//				g_bPreviousMove = false;
			}
		}
	}

	//------------------------------------------------------------
	// Healing ¸Ô±â
	//------------------------------------------------------------
	void
	AutoUsePotion(int bHeal)	// 0:heal 1:mana 2:magazine
	{
		if (g_pInventory!=NULL 
			&& g_pPlayer!=NULL
			&& !g_pPlayer->IsVampire()
			&& g_pPlayer->IsItemCheckBufferNULL() 
			&& !gbl_item_lock)
		{
			// Healing
			int badClass = -1;
			int badType = -1;

			int currentValue;
			int maxValue; 
			float modifier = 1.0f;

			switch(bHeal)
			{
			case 0:	// heal
				currentValue = g_pPlayer->GetHP();
				maxValue = g_pPlayer->GetMAX_HP();
				break;

			case 1: // mana
				currentValue = g_pPlayer->GetMP();
				maxValue = g_pPlayer->GetMAX_MP();

				modifier = 1.0f + g_pPlayer->GetINT() / 300.0f;
				break;

			case 2: // magazine
			case 3: // magazine
			case 4: // magazien
			case 5: // magazine
				if(g_pCurrentMagazine == NULL)
					return;

				currentValue = g_pCurrentMagazine->GetNumber();
				maxValue = 1;
				break;
			}

			if (maxValue<=0) return;

			int lost = maxValue - currentValue;
			int percent = currentValue * 100 / maxValue;

			bool forceUse = false;
			
			ITEM_CLASS itemClass = ITEM_CLASS_POTION;
			if(g_pPlayer->IsOusters())
				itemClass = ITEM_CLASS_PUPA;

			int itemType;
			int maxItemType = 4;
			
			if(bHeal >= 2)
			{
				if(g_pSlayerGear != NULL && !g_pSlayerGear->GetItem(MSlayerGear::GEAR_SLAYER_LEFTHAND)->IsGunItem())return;
//				badType = (g_pSlayerGear->GetItem(MSlayerGear::GEAR_SLAYER_LEFTHAND)->GetItemClass()-ITEM_CLASS_SG)*2-1;
				badType = (g_pCurrentMagazine->GetItemType() & ~1) -1;
				itemClass = ITEM_CLASS_MAGAZINE;
				
				if(currentValue > bHeal-2)
					return;

				itemType = badType+2;
			}
			else
			{
				if (lost >= 200.0f * modifier)		itemType = 4;
				else if (lost >= 100.0f * modifier)	itemType = 3;
				else if (lost >= 51.0f * modifier)	itemType = 2;
				else if (lost >= 24.0f * modifier)	itemType = 1;
				else if (lost >= 10.0f * modifier)	itemType = 0;
				else								itemType = badType;
			}

			switch(bHeal)
			{
			case 0:	// Heal
				// HP´Â 50% ÀÌÇÏÀÏ¶§ ¸Ô´Â´Ù.
				forceUse = percent <= 50;
				break;

			case 1:	// Mana
				badType += 5;
				itemType += 5;
				maxItemType += 5;

				// Mana´Â ÇöÀç »ç¿ëÁßÀÎ ±â¼ú°ú ºñ±³ÇØ¼­ ¸Ô´Â´Ù.
				if (g_pSkillInfoTable!=NULL)
				{
					int skill = g_pPlayer->GetSpecialActionInfo();

					if (skill < g_pSkillInfoTable->GetSize())
					{
						forceUse = currentValue < 5
									|| (*g_pSkillInfoTable)[skill].GetMP()*2 >= currentValue;
					}
				}
				break;

			}
			
			if (itemType!=badType && itemClass!=badClass)
			{
				MItem* pItem = NULL;
				int type = itemType;

				do
				{
					pItem = g_pInventory->FindItem(itemClass , type );

					// reloadÀÏ¶§´Â ÀºÁ¦ÃÑ¾ËÀ» Ã£¾Æº¸ÀÚ
					if(bHeal >= 2 && pItem == NULL) pItem = g_pInventory->FindItem(itemClass , type +8);

					if (pItem!=NULL) break;					
				}
				while (--type > badType);

				// ¸î percent ÀÌÇÏÀÏ¶§´Â ±×³É ¸Ô¾î¾ß ÇÑ´Ù. ÃÑ¾ËÀº »©±¸
				if (bHeal != 2 && pItem==NULL && forceUse)
				{
					type = badType + 1;

					do
					{
						pItem = g_pInventory->FindItem( ITEM_CLASS_POTION, type );

						if (pItem!=NULL) break;					
					}
					while (++type <= maxItemType);
				}
			
				if (pItem!=NULL && g_pPlayer->IsItemCheckBufferNULL())
				{
					if(bHeal >= 2)
					{
						CGReloadFromInventory _CGReloadFromInventory;
						_CGReloadFromInventory.setObjectID( pItem->GetID() );
						_CGReloadFromInventory.setX( pItem->GetGridX() );
						_CGReloadFromInventory.setY( pItem->GetGridY() );

						g_pSocket->sendPacket( &_CGReloadFromInventory );
					}
					else
					{
						CGUsePotionFromInventory _CGUsePotionFromInventory;
						_CGUsePotionFromInventory.setObjectID( pItem->GetID() );
						_CGUsePotionFromInventory.setX( pItem->GetGridX() );
						_CGUsePotionFromInventory.setY( pItem->GetGridY() );

						g_pSocket->sendPacket( &_CGUsePotionFromInventory );
					}

					g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
				}
				else if (g_pQuickSlot!=NULL)
				{
					// º§Æ®¿¡¼­ Ã£±â
					type = itemType;

					do
					{
						MItemClassTypeFinder itemFinder( itemClass, type );

						pItem = g_pQuickSlot->FindItem( itemFinder );

						// reloadÀÏ¶§´Â ÀºÁ¦ÃÑ¾ËÀ» Ã£¾Æº¸ÀÚ
						if(bHeal >= 2 && pItem == NULL)
						{
							MItemClassTypeFinder itemFinder( itemClass, type +8 );

							pItem = g_pQuickSlot->FindItem( itemFinder );
						}

						if (pItem!=NULL) break;					
					}
					while (--type > badType);

					// ¸î percent ÀÌÇÏÀÏ¶§´Â ±×³É ¸Ô¾î¾ß ÇÑ´Ù. ÃÑ¾ËÀÏ ¶§´Â »©±¸
					if (bHeal != 2 && pItem==NULL && forceUse)
					{
						type = badType + 1;

						do
						{
							MItemClassTypeFinder itemFinder( ITEM_CLASS_POTION, type );

							pItem = g_pQuickSlot->FindItem( itemFinder );

							if (pItem!=NULL) break;					
						}
						while (++type <= maxItemType);
					}

					if (pItem!=NULL && g_pPlayer->IsItemCheckBufferNULL() )
					{
						if(bHeal >= 2)
						{
							g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);

							CGReloadFromQuickSlot _CGReloadFromQuickSlot;
							_CGReloadFromQuickSlot.setObjectID( pItem->GetID() );
							_CGReloadFromQuickSlot.setSlotID( pItem->GetItemSlot() );							

							g_pSocket->sendPacket( &_CGReloadFromQuickSlot );
						}
						else
						{
							g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);

							CGUsePotionFromQuickSlot _CGUsePotionFromQuickSlot;
							_CGUsePotionFromQuickSlot.setObjectID( pItem->GetID() );
							_CGUsePotionFromQuickSlot.setSlotID( pItem->GetItemSlot() );							

							g_pSocket->sendPacket( &_CGUsePotionFromQuickSlot );
						}

						g_pPlayer->SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_QUICKSLOT);

						UI_LockGear();
					}					
				}			
				
			}
		}
	}

#endif

bool	CanActionByZoneInfo()
{	
	if(g_pZone != NULL)
	{
		ZONETABLE_INFO *pZoneInfo = g_pZoneTable->Get( g_pZone->GetID() );
		
		if(pZoneInfo->CannotAttackInSafe == true)					// ¾ÈÀüÁö´ë ¾È¿¡¼­ °ø°ÝÇÒ ¼ö ¾øÀ¸¸é
		{			
			if ( g_pZone->GetSector(g_pPlayer->GetX(),g_pPlayer->GetY()).IsSafeComplete() )
				return true;
			else
			{
				if(g_pPlayer->IsSlayer() &&	g_pZone->GetSector(g_pPlayer->GetX(),g_pPlayer->GetY()).IsSafeSlayer() )
					return true;				
				if(g_pPlayer->IsVampire() && g_pZone->GetSector(g_pPlayer->GetX(),g_pPlayer->GetY()).IsSafeVampire() ) 
					return true;
				if(g_pPlayer->IsOusters() && g_pZone->GetSector(g_pPlayer->GetX(),g_pPlayer->GetY()).IsSafeOusters() ) 
					return true;
			}
		}
	}
	return false;
}

//----------------------------------------------------------------------
// ÆÄÆ¼¿øµé¿¡°Ô ³» ÁÂÇ¥¸¦ º¸³½´Ù.
//----------------------------------------------------------------------
void
SendPositionInfoToParty()
{
	//------------------------------------------------------
	// Áö¼ÓÀûÀ¸·Î ÁÂÇ¥¸¦ º¸³»´Â °æ¿ì
	//------------------------------------------------------		
	if (g_pParty->GetSize()!=0
		&& g_pClientCommunicationManager!=NULL)
	{
		static int oldX = -1;
		static int oldY = -1;
		static int oldZoneID = -1;

		int x		= g_pPlayer->GetX();
		int y		= g_pPlayer->GetY();
		int zoneID	= (g_bZonePlayerInLarge?g_nZoneLarge : g_nZoneSmall);

		// ÁÂÇ¥°¡ ´Þ¶óÁ³À¸¸é º¸³½´Ù.
		if (oldX!=x || oldY!=y || oldZoneID!=zoneID)			
		{
			if(g_pUserInformation->bKorean == true && 1==2)
			{
				RCPositionInfo _RCPositionInfo;

				// ÀÌ¸§À» ¾È º¸³»µµ IP·Î Ã¼Å©ÇÏ±â ¶§¸Þ µÈ´Ù.
				//_RCPositionInfo.setName( g_pUserInformation->CharacterID.GetString() );
				_RCPositionInfo.setZoneX( x );
				_RCPositionInfo.setZoneY( y );
				_RCPositionInfo.setZoneID( zoneID );

				// º¸³½ Á¤º¸ ±â¾ï
				oldX = x;
				oldY = y;
				oldZoneID = zoneID;

				for (int i=0; i<g_pParty->GetSize(); i++)
				{
					PARTY_INFO* pInfo = g_pParty->GetMemberInfo( i );
					
					if (pInfo!=NULL
						// ½Ã¾ß¿¡ ¾ø´Â °æ¿ì¿¡¸¸ À§Ä¡ Á¤º¸¸¦ º¸³½´Ù.
						// ÀÇ¹Ì°¡ ¾øÀ»Áöµµ.. - -;
						//&& !pInfo->bInSight
						)
					{
						int port = g_pClientConfig->CLIENT_COMMUNICATION_UDP_PORT;

						RequestUserInfo* pUserInfo = g_pRequestUserManager->GetUserInfo( pInfo->Name.GetString() );

						if (pUserInfo!=NULL)
						{
							if (pUserInfo->UDPPort!=0)
								port = pUserInfo->UDPPort;
						}

						g_pClientCommunicationManager->sendPacket( pInfo->IP, 
																port, 
																&_RCPositionInfo );
					}
				}			
			}
			else	// ÇÑ±¹¾î°¡ ¾Æ´Ò °æ¿ì
			{
				CGPartyPosition _CGPartyPosition;

				_CGPartyPosition.setZoneID ( zoneID );
				_CGPartyPosition.setXY( x, y );
				_CGPartyPosition.setHP( g_pPlayer->GetMAX_HP(), g_pPlayer->GetHP());

				g_pSocket->sendPacket( &_CGPartyPosition );
			}
		}
	}
}

//----------------------------------------------------------------------
// Send StatusInfo To Party
//----------------------------------------------------------------------
// ÆÄÆ¼¿øµé¿¡°Ô ³» »óÅÂ Á¤º¸¸¦ º¸³½´Ù. ¾ðÁ¦? - -;
//----------------------------------------------------------------------
void
SendStatusInfoToParty()
{
	static DWORD nextTime = g_CurrentTime;

	if (g_CurrentTime >= nextTime)
	{
		//------------------------------------------------------
		// Áö¼ÓÀûÀ¸·Î ÁÂÇ¥¸¦ º¸³»´Â °æ¿ì
		//------------------------------------------------------		
		if (g_pParty->GetSize()!=0
			&& g_pClientCommunicationManager!=NULL)
		{
			static int oldHP = -1;
			static int oldMaxHP = -1;
			
			HP_t HP		= g_pPlayer->GetHP();
			HP_t MaxHP	= g_pPlayer->GetMAX_HP();
			
			// ÁÂÇ¥°¡ ´Þ¶óÁ³À¸¸é º¸³½´Ù.
			if (oldHP!=HP || oldMaxHP!=MaxHP)
			{
				if(g_pUserInformation->bKorean == true && 1==2)
				{
					RCStatusHP _RCStatusHP;

					// ÀÌ¸§À» ¾È º¸³»µµ IP·Î Ã¼Å©ÇÏ±â ¶§¸Þ µÈ´Ù.
					//_RCStatusHP.setName( g_pUserInformation->CharacterID.GetString() );
					_RCStatusHP.setCurrentHP( HP );
					_RCStatusHP.setMaxHP( MaxHP );
					
					// º¸³½ Á¤º¸ ±â¾ï
					oldHP = HP;
					oldMaxHP = MaxHP;
					
					for (int i=0; i<g_pParty->GetSize(); i++)
					{
						PARTY_INFO* pInfo = g_pParty->GetMemberInfo( i );
						
						if (pInfo!=NULL
							// ½Ã¾ß¿¡ ¾ø´Â °æ¿ì¿¡¸¸ »óÅÂ Á¤º¸¸¦ º¸³½´Ù.
							// ÀÇ¹Ì°¡ ¾øÀ»Áöµµ.. - -;
							//&& !pInfo->bInSight
							)
						{
							int port = g_pClientConfig->CLIENT_COMMUNICATION_UDP_PORT;

							RequestUserInfo* pUserInfo = g_pRequestUserManager->GetUserInfo( pInfo->Name.GetString() );

							if (pUserInfo!=NULL)
							{
								if (pUserInfo->UDPPort!=0)
									port = pUserInfo->UDPPort;
							}

							g_pClientCommunicationManager->sendPacket( pInfo->IP, 
																	port, 
																	&_RCStatusHP );
						}
					}	
				}
				else // ÇÑ±¹¾î°¡ ¾Æ´Ñ °æ¿ì
				{
					int x		= g_pPlayer->GetX();
					int y		= g_pPlayer->GetY();
					int zoneID	= (g_bZonePlayerInLarge?g_nZoneLarge : g_nZoneSmall);

					CGPartyPosition _CGPartyPosition;

					_CGPartyPosition.setZoneID ( zoneID );
					_CGPartyPosition.setXY( x, y );
					_CGPartyPosition.setHP( g_pPlayer->GetMAX_HP(), g_pPlayer->GetHP());

					g_pSocket->sendPacket( &_CGPartyPosition );
				}
			}
		}

		// ½Ã¾ß¿¡ ¾ø´Â °æ¿ìÀÇ HPÀÌ±â ¶§¹®¿¡..
		// 5ÃÊ¿¡ ÇÑ¹ø °»½ÅÇØÁØ´Ù.
		nextTime = g_CurrentTime + g_pClientConfig->CLIENT_COMMUNICATION_STATUS_DELAY;
	}
}

//----------------------------------------------------------------------
// Send StatusInfo To Party
//----------------------------------------------------------------------
// ÆÄÆ¼¿øµé¿¡°Ô ³» »óÅÂ Á¤º¸¸¦ º¸³½´Ù. ¾ðÁ¦? - -;
//----------------------------------------------------------------------
void
SendCharacterInfoToParty()
{
	//------------------------------------------------------
	// Áö¼ÓÀûÀ¸·Î ÁÂÇ¥¸¦ º¸³»´Â °æ¿ì
	//------------------------------------------------------		
	if (g_pParty->GetSize()!=0
		&& g_pClientCommunicationManager!=NULL)
	{
		int guildID	= g_pPlayer->GetGuildNumber();
		if (guildID>0)
		{
			RCCharacterInfo _RCCharacterInfo;

			// ÀÌ¸§À» ¾È º¸³»µµ IP·Î Ã¼Å©ÇÏ±â ¶§¸Þ µÈ´Ù.
			//_RCStatusHP.setName( g_pUserInformation->CharacterID.GetString() );
			_RCCharacterInfo.setGuildID( guildID );
			
			for (int i=0; i<g_pParty->GetSize(); i++)
			{
				PARTY_INFO* pInfo = g_pParty->GetMemberInfo( i );
				
				if (pInfo!=NULL
					// ½Ã¾ß¿¡ ¾ø´Â °æ¿ì¿¡¸¸ »óÅÂ Á¤º¸¸¦ º¸³½´Ù.
					// ÀÇ¹Ì°¡ ¾øÀ»Áöµµ.. - -;
					//&& !pInfo->bInSight
					)
				{
					int port = g_pClientConfig->CLIENT_COMMUNICATION_UDP_PORT;

					RequestUserInfo* pUserInfo = g_pRequestUserManager->GetUserInfo( pInfo->Name.GetString() );

					if (pUserInfo!=NULL)
					{
						if (pUserInfo->UDPPort!=0)
							port = pUserInfo->UDPPort;
					}

					g_pClientCommunicationManager->sendPacket( pInfo->IP, 
															port, 
															&_RCCharacterInfo );
				}
			}
		}
	}
}

//----------------------------------------------------------------------
// Is Block All Direction
//----------------------------------------------------------------------
extern POINT g_DirectionValue[MAX_DIRECTION];
bool
IsBlockAllDirection()
{
	if (g_pPlayer!=NULL && g_pZone!=NULL)
	{
		int x = g_pPlayer->GetX();
		int y = g_pPlayer->GetY();

		for (int i=0; i<8; i++)
		{
			const POINT& pt = g_DirectionValue[i];

			int sx = x + pt.x;
			int sy = y + pt.y;

			if (g_pZone->CanMove( g_pPlayer->GetMoveType(), sx, sy ))
			{
				return false;
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------
// 
// contructor/destructor
//
//----------------------------------------------------------------------
MPlayer::MPlayer()
{	
	CT_Done("MPlayer::MPlayer ENTER");
	
	// ÃßÀû Á¤º¸
	m_fTrace				= FLAG_TRACE_NULL;
	m_TraceDistance			= 0;
	m_TraceObjectAction		= 0;
	m_fTraceBuffer			= FLAG_TRACE_NULL;

	m_BasicAttackDistance	= 1;

	// ´ÙÀ½ ÃßÀû Á¤º¸ 
	m_fNextTrace			= FLAG_TRACE_NULL;
	m_NextTraceID			= OBJECTID_NULL;
	m_NextTraceX			= SECTORPOSITION_NULL;
	m_NextTraceY			= SECTORPOSITION_NULL;
	m_NextTraceZ			= 0;
	m_NextTraceObjectAction	= 0;

	
	// ¸ñÀûÁö	
	m_DestX = SECTORPOSITION_NULL;
	m_DestY = SECTORPOSITION_NULL;

	// ´ÙÀ½ ¸ñÀûÁö
	m_NextDestX = SECTORPOSITION_NULL;
	m_NextDestY = SECTORPOSITION_NULL;

	m_bMoveRetargetPending = FALSE;
	m_RetargetDestX = SECTORPOSITION_NULL;
	m_RetargetDestY = SECTORPOSITION_NULL;
	m_RetargetTick = 0;

	// ¸ø °¬´ø °÷..
	m_BlockDestX = SECTORPOSITION_NULL;
	m_BlockDestY = SECTORPOSITION_NULL;

	// Server·Î º¸³½ ¸Þ¼¼Áö °³¼ö
	m_SendMove = 0;

	// Delay½Ã°£
	m_DelayTime	= 0;
	
	// attack mode
	m_AttackMode = ATTACK_MODE_NORMAL;

	// ¹æ±Ý ÁÖ¿î item
	m_ItemCheckBufferStatus = ITEM_CHECK_BUFFER_NULL;
	m_pItemCheckBuffer = NULL;
	m_dwSubItemIDCheckBuffer = OBJECTID_NULL;

	// ±â¼ú »ç¿ëÀÇ ¸ñÇ¥
	m_pEffectTarget = NULL;

	// server¿¡¼­ ±â´Ù¸®±â
	m_WaitVerify = WAIT_VERIFY_NULL;
	m_WaitVerifyActionInfo = ACTIONINFO_NULL;

	// °è¼Ó ÃßÀûÇÏ±â..
	m_bKeepTraceCreature = true;

	// Çàµ¿ ¹Ýº¹
	m_bRepeatAction		= FALSE;

	// ¹ìÆÄÀÌ¾î·Î º¯ÇÏ´Â ½Ã°£
	m_ConversionDelayTime = 0;

	m_nNoPacketUsedActionInfo = ACTIONINFO_NULL;

	m_bLockMode			= false;

	//-------------------------------------------------------
	// ºûÀÇ Å©±â ½Ã¾ß
	//-------------------------------------------------------
	m_TimeLightSight	= 0;
//	m_TimeLightSightX	= SECTORPOSITION_NULL;
//	m_TimeLightSightY	= SECTORPOSITION_NULL;	
	m_ItemLightSight	= 0;
	m_LightSight		= 0;

	// Ä³¸¯ÅÍ ½Ã¾ß
	m_Sight				= 0;
	m_PetDelayTime		= 0;

	m_SweepVice_Value	= 0;
	m_TempSelectPosX	= SECTORPOSITION_NULL;
	m_TempSelectPosY	= SECTORPOSITION_NULL;
	m_SweepViceX		= SECTORPOSITION_NULL;
	m_SweepViceY		= SECTORPOSITION_NULL;
	m_bShowAdamCloud	= false;

	m_ResurrectZoneID	= 0;

	m_byComboEndValue	= 1 ;
	m_bInvincible		= false;
	
	m_bEnforceResurrect	= false;

	m_SkillCastingX		= SECTORPOSITION_NULL;
	m_SkillCastingY		= SECTORPOSITION_NULL;
	
	m_BatColorSet			= 0xFFFF;
	m_AdvanceBatColorSet	= 0xFFFF;
  CT_Done("MPlayer constructor EXIT - SUCCESS");
}

MPlayer::~MPlayer()
{
	// À¸Çæ~~ ÇÊ»ìÀÇ ÄÚµå - -;;
	// Effect°¡ deleteµÇ¸é¼­
	// playerÀÇ EffectTargetÀ» Á¦°ÅÇÒ·Á°í g_pPlayer·Î Á¢±ÙÇÏ´Â ¹Ù¶÷¿¡..
	// ±¸Á¶¸¦ Á» ¹Ù²ã¾ßµÉÅÙµ¥.. T_T;; °íÄ¥ ½Ã°£ÀÌ.. - -;	
	g_pUserInformation->bCompetence = false;
	g_pUserInformation->bCompetenceShape = false;
	g_pPlayer = NULL;

	// remove priority std::queue 
	RemoveNodes();

	// remove list
	m_listDirection.clear();
	m_listSendDirection.clear();

	// ±â¼ú »ç¿ë ¸ñÇ¥
	if (m_pEffectTarget!=NULL)
	{
		MEffectTarget* pTempEffectTarget = m_pEffectTarget;
		m_pEffectTarget = NULL;

		delete pTempEffectTarget;
	}
}


//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// MPlayer°¡ Á¸ÀçÇÏ´Â ZoneÀ» ¼³Á¤ÇÑ´Ù.
//----------------------------------------------------------------------
void	
MPlayer::SetZone(MZone* pZone)
{
	// base class functions
	MCreature::SetZone(pZone);

}

//----------------------------------------------------------------------
// Player¸¦ ¸ØÃß°Ô ÇÑ´Ù.
//----------------------------------------------------------------------
void
MPlayer::SetStop()
{ 
	// ±æÃ£±â Á¦°Å
	// 2004, 9, 1, sobeit add start - ¼­¼­ Á×´Â°Å ¹æÁö?¤»¤»
	m_bKnockBack = 0;

	// 2004, 9, 1, sobeit add end

	m_listDirection.clear(); 

	// ÃßÀû ÁßÁö	
	m_fTrace		= FLAG_TRACE_NULL;
	//m_fNextTrace	= FLAG_TRACE_NULL;

	// Action ÁßÁö
	m_sX=0; 
	m_sY=0;

	int tempAction = m_MoveAction==ACTION_SLAYER_MOTOR_MOVE? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND;
	if(IsOusters() && HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) ))
		tempAction = ACTION_OUSTERS_FAST_MOVE_STAND;
#if __CONTENTS(__FAST_TRANSFORTER)
	else if(IsOusters() && HasEffectStatusWingSylph( dynamic_cast<MCreature*>(this) ))
		tempAction = ACTION_OUSTERS_WING_STAND;
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
	else if(IsOusters() && HasEffectStatusUnicorn( dynamic_cast<MCreature*>(this) ))
		tempAction = ACTION_OUSTERS_UNICORN_STAND;
	else if(IsOusters() && HasEffectStatusOusterNewWings1( dynamic_cast<MCreature*>(this) ))
		tempAction = ACTION_OUSTERS_UNICORN_STAND;
	else if(IsOusters() && HasEffectStatusOusterNewWings2( dynamic_cast<MCreature*>(this) ))
		tempAction = ACTION_OUSTERS_UNICORN_STAND;
#endif //__SECOND_TRANSFORTER
	m_Action		= tempAction;
	// 2004, 11, 3, sobeit modify start - m_ActionCount¸¦ ¹Ù²ï ¾×¼ÇÀÇ ¸Æ½º Ä«¿îÆ®·Î ¼¼ÆÃ ÇßÀ½ - ¾Æ¿ì½ºÅÍÁî °í½ºÆ® ¹ö±× ¶«½Ã..¤Ñ¤Ñ;
	m_ActionCount	= m_ActionCountMax; 
	//m_ActionCount	= (*g_pCreatureTable)[m_CreatureType].GetActionCount( m_Action );
	// 2004, 11, 3, sobeit modify end
	m_MoveCount		= m_MoveCountMax; 

	// ´ÙÀ½ µ¿ÀÛµµ ¾ø¾Ú
	m_bNextAction = false;
	m_NextX = SECTORPOSITION_NULL;
	m_NextY = SECTORPOSITION_NULL;
	
	//m_nNextUsedActionInfo = ACTIONINFO_NULL;
	//m_nUsedActionInfo = ACTIONINFO_NULL;

	
	// ¸ñÇ¥ Á¦°Å
	m_DestX			= SECTORPOSITION_NULL; 
	m_DestY			= SECTORPOSITION_NULL; 
	m_NextDestX		= SECTORPOSITION_NULL; 
	m_NextDestY		= SECTORPOSITION_NULL; 
}


//----------------------------------------------------------------------
// priority queueÀÎ m_pqDNodes¸¦ Áö¿î´Ù.
//----------------------------------------------------------------------
void
MPlayer::RemoveNodes()
{
	while (!m_pqDNodes.empty())
	{
		//DNode* pDNode = m_pqDNodes.top();
		m_pqDNodes.pop();
		//delete pDNode;		
	}


	// temp¸¦ Áö¿î´Ù.
	DNODE_LIST::iterator iNode = m_listDNodes.begin();

	while (iNode != m_listDNodes.end())
	{
		DNode* pNode = *iNode;
		delete pNode;
		iNode++;		
	}

	m_listDNodes.clear();
}

//----------------------------------------------------------------------
// (x,y)·Î ºÎÅÍ m_Dest(X,Y)±îÁöÀÇ °Å¸®¸¦ ±¸ÇÑ´Ù.
//----------------------------------------------------------------------
// °¡·Î,¼¼·Î,´ë°¢¼± ¸ðµÎ 1ÀÌ¹Ç·Î °¡Àå Å« °ªÀÌ °Å¸®ÀÌ°í
// ¸ñÇ¥¿Í °°Àº ÁÂÇ¥°¡ ÀÖ´Â°Ô ´õ ÁÁÀ¸¹Ç·Î ¾Æ´Ñ °æ¿ì +1ÀÌ µÇµµ·Ï ÇÑ´Ù.
//----------------------------------------------------------------------
int
MPlayer::CalculateDistance(int x, int y)
{
	//int xx = x-m_DestX,
	//	yy = y-m_DestY;

	//return abs(xx)+abs(yy);

	//return sqrt(xx*xx + yy*yy);
//	return xx*xx + yy*yy;

//	/*
	int xd = abs(x-m_DestX),
		yd = abs(y-m_DestY);	 

	int plus = !(m_DestX==x) + !(m_DestY==y);
		
	if (xd>yd)
	{
		return xd*xd + plus;
	}
	else return yd*yd + plus;

	return 0;
//	*/
}

//----------------------------------------------------------------------
// Get Destination()
//----------------------------------------------------------------------
void
MPlayer::GetDestination(POINT &dest)
{
	dest.x = m_DestX;
	dest.y = m_DestY;
}

//----------------------------------------------------------------------
// Set Move Position
//----------------------------------------------------------------------
bool
MPlayer::SetMovePosition(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)
{
	CT_BAT("SetMovePosition ENTER – target=(%d,%d) alive=%d underground=%d sleep=%d ghost=%d", sX, sY, m_bAlive, IsUndergroundCreature(), HasEffectStatus(EFFECTSTATUS_SLEEP), g_pPlayer->HasEffectStatus(EFFECTSTATUS_GHOST));
	Phoenix_MovementAuditWrite("MOVE_SET_POSITION_ENTER", "CLIENT_CLICK_TARGET", m_X, m_Y, sX, sY, m_Direction, m_SendMove);

	// »ì¾ÆÀÖÁö ¾ÊÀº °æ¿ì..
	if	(
		(
		!m_bAlive
		// °ü¼Ó¿¡ ÀÖ´Â °æ¿ì
		|| m_bInCasket
		// ¶¥ ¼Ó¿¡ ÀÖ´Â °æ¿ì
		|| IsUndergroundCreature()
		|| HasEffectStatus(EFFECTSTATUS_SLEEP)
		)
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
	{
		CT_BAT("SetMovePosition REJECT – NOT_ALIVE/UNDERGROUND/SLEEP (alive=%d, underground=%d, sleep=%d)", m_bAlive, IsUndergroundCreature(), HasEffectStatus(EFFECTSTATUS_SLEEP));
		Phoenix_MovementAuditWrite("MOVE_SET_POSITION_FAIL", "NOT_ALIVE_CASKET_UNDERGROUND_SLEEP", m_X, m_Y, sX, sY, m_Direction, m_SendMove);
		return false;
	}

	// ´ÙÀ½¿¡ »ç¿ëÇØ¾ßÇÒ ±â¼úÀÌ ÀÖ´Â °æ¿ì...
	//if (m_nUsedActionInfo!=ACTIONINFO_NULL)
	if (m_ActionCount>=m_ActionCountMax)
	{
		m_fTraceBuffer = FLAG_TRACE_NULL;
		m_TraceIDBuffer = OBJECTID_NULL;
		m_nNextUsedActionInfo = ACTIONINFO_NULL;

		//if (m_ActionCount==m_ActionCountMax)
		//{			
			m_nUsedActionInfo = ACTIONINFO_NULL;

			/*
			DEBUG_ADD("Clear m_nUsedActionInfo");
		}
		else
		{
			DEBUG_ADD_FORMAT("Not Clear : action=%d, %d/%d", (int)m_Action, (int)m_ActionCount, (int)m_ActionCountMax);
		}
		*/
		TraceNULL();
	}

	bool result = SetNextDestination(sX, sY);
	CT_BAT("SetMovePosition %s – SetNextDestination result=%s", result? "OK":"FAIL", result? "true":"false");
	Phoenix_MovementAuditWrite(result ? "MOVE_SET_POSITION_OK" : "MOVE_SET_POSITION_FAIL", "SET_NEXT_DESTINATION_RESULT", m_X, m_Y, sX, sY, m_Direction, m_SendMove);
	CT_BAT("SetMovePosition EXIT – returning %s", result? "true":"false");
	return result;
}

//----------------------------------------------------------------------
// Set Next Destination
//----------------------------------------------------------------------
// ´ÙÀ½¿¡ ÀÌµ¿ÇÒ ¸ñÇ¥ À§Ä¡¸¦ °áÁ¤ÇÑ´Ù.
// ¸ñÇ¥À§Ä¡°¡ °¥ ¼ö ¾ø´Â °÷ÀÎ °æ¿ì... ¾î¶»°Ô ÇÒ °ÍÀÎ°¡?
//----------------------------------------------------------------------
bool
MPlayer::SetNextDestination(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)
{
	CT_BAT("SetNextDestination ENTER – target=(%d,%d) alive=%d ghost=%d", sX, sY, m_bAlive, g_pPlayer->HasEffectStatus(EFFECTSTATUS_GHOST));
	//DEBUG_ADD_FORMAT("[Set] NextDestination (%d, %d) --> (%d, %d)", m_X, m_Y, sX, sY);
	Phoenix_MovementAuditWrite("MOVE_SET_NEXT_DEST_ENTER", "REQUEST_NEXT_DESTINATION", m_X, m_Y, sX, sY, m_Direction, m_SendMove);

	// »ì¾ÆÀÖÁö ¾ÊÀº °æ¿ì..
	if	(
		(!m_bAlive || HasEffectStatus(EFFECTSTATUS_SLEEP))
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
		
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
	{
		CT_BAT("SetNextDestination REJECT – NOT_ALIVE_OR_SLEEP (alive=%d, sleep=%d)", m_bAlive, HasEffectStatus(EFFECTSTATUS_SLEEP));
		Phoenix_MovementAuditWrite("MOVE_SET_NEXT_DEST_FAIL", "NOT_ALIVE_OR_SLEEP", m_X, m_Y, sX, sY, m_Direction, m_SendMove);
		return false;
	}

	// ´ÙÀ½¿¡ »ç¿ëÇØ¾ßÇÒ ±â¼úÀÌ ÀÖ´Â °æ¿ì...
	if (m_nUsedActionInfo!=ACTIONINFO_NULL)
	{
		m_fTraceBuffer = FLAG_TRACE_NULL;
		m_TraceIDBuffer = OBJECTID_NULL;
		m_nNextUsedActionInfo = ACTIONINFO_NULL;

		if (m_ActionCount>=m_ActionCountMax)
		{			
			
			m_nUsedActionInfo = ACTIONINFO_NULL;
			
			DEBUG_ADD("Clear m_nUsedActionInfo");
						
		}
		else
		{
			DEBUG_ADD_FORMAT("Not Clear : action=%d, %d/%d", (int)m_Action, (int)m_ActionCount, (int)m_ActionCountMax);
		}

		TraceNULL();
	}
	
	// ÃßÀû ÁßÀÌ ¾Æ´Ï¸é¼­
	// ÀÌ¹Ì ¸ñÇ¥À§Ä¡¿¡ ÀÖ´Â °æ¿ì´Â ±æÃ£±â ÇÊ¿ä¾ø´Ù.
	if (m_fTrace==FLAG_TRACE_NULL && sX==m_X && sY==m_Y)
	{
		
		Phoenix_MovementAuditWrite("MOVE_SET_NEXT_DEST_FAIL", "TARGET_IS_CURRENT_POSITION", m_X, m_Y, sX, sY, m_Direction, m_SendMove);
		return false;
	}

	// Àß¸øµÈ °ª
	// ÀÌÀü¿¡ ¸ø °¬´ø °÷..
	if (sX==SECTORPOSITION_NULL || sY==SECTORPOSITION_NULL
		|| sX==m_BlockDestX && sY==m_BlockDestY)
	{
		
		Phoenix_MovementAuditWrite("MOVE_SET_NEXT_DEST_FAIL", "NULL_OR_PREVIOUS_BLOCK_DEST", m_X, m_Y, sX, sY, m_Direction, m_SendMove);
		return false;
	}

	//---------------------------------------------------
	// [¹æ¹ý 1]
	// ¸ñÇ¥ÁöÁ¡À¸·Î °¥ ¼ö ¾ø´Ù¸é
	// ¸ñÇ¥ ÁöÁ¡°ú °¡Àå °¡±î¿î °÷À¸·Î ÀÌµ¿ÇÏ°Ô ÇÑ´Ù.
	//---------------------------------------------------
	/*
	TYPE_SECTORPOSITION destX = sX;
	TYPE_SECTORPOSITION	destY = sY;	

	//---------------------------------------------------
	// °¥ ¼ö ÀÖ´Â ¸ñÇ¥ÁöÁ¡À» ÁöÁ¤ÇÑ´Ù.
	//---------------------------------------------------
	while (!m_pZone->CanMove(m_MoveType, destX, destY))
	{
		// ¸ñÇ¥(sX,sY)¿¡¼­ ÇöÀç À§Ä¡·ÎÀÇ ¹æÇâ
		BYTE direction = MTopView::GetDirectionToPosition(sX, sY, m_X, m_Y);

		GetPositionToDirection(destX, destY, direction);

		// ¿ÏÀüÈ÷(!) °¥ ¼ö ¾ø´Â °÷ÀÌ´Ù.
		if (destX==m_X && destY==m_Y)
			return false;
	}
	m_NextDestX = destX; 
	m_NextDestY = destY; 
	*/

	if(m_fTrace==FLAG_TRACE_NULL)
	{
		BOOL bCanStand = m_pZone->CanMove(m_MoveType, sX,sY);
		CT_BAT("SetDestination CANMOVE – moveType=%d canMove=%d sector=(%d,%d)", m_MoveType, bCanStand, sX, sY);
		if(!bCanStand)
			
		{
			extern POINT g_DirectionValue[MAX_DIRECTION];
			
			POINT MovePoint;

			if( sY > GetY() )
				MovePoint.y = -1;
			else if( sY < GetY() )
				MovePoint.y = 1;
			else
				MovePoint.y = 0;

			if( sX > GetX() )
				MovePoint.x = -1;
			else if( sX < GetX() )
				MovePoint.x = 1;
			else
				MovePoint.x = 0;

			int limit = 20;	// À¯È¿°ª 20¹ø³»¿¡ ¸øÃ£À¸¸é Æ÷±â

			while( bCanStand == FALSE && !( GetX() == sX && GetY() == sY ) &&
				sX >= 0 && sY >= 0 
				&& sX < m_pZone->GetWidth() && sY < m_pZone->GetHeight()
				&& limit-- > 0
				)
			{
				if( GetX() != sX )sX += MovePoint.x;
				if( GetY() != sY )sY += MovePoint.y;
				bCanStand = m_pZone->CanMove( m_MoveType, sX,sY );
			};

			if( bCanStand == FALSE || GetX() == sX && GetY() == sY)
			{
				Phoenix_MovementAuditWrite("MOVE_SET_NEXT_DEST_FAIL", "NO_STANDABLE_NEAR_TARGET", m_X, m_Y, sX, sY, m_Direction, m_SendMove);
				CT_BAT("SetNextDestination EXIT – returning false");
				return false;
			}
		}
	}
//	else
	//---------------------------------------------------
	// [¹æ¹ý 2]
	// ÇöÀç À§Ä¡¿¡¼­ ¸ñÇ¥ ÁöÁ¡À¸·Î ÇÑ Ä­ ÀÌµ¿..
	//
	// ´Ü, ÃßÀû ÁßÀÎ °æ¿ì´Â... °ü°è¾ø°í.. 
	// ÀÌµ¿ Áß¿¡¼­¸¸...
	//---------------------------------------------------
//	if (m_fTrace==FLAG_TRACE_NULL && !m_pZone->CanMove(m_MoveType, sX, sY))
//	{
//		BYTE direction = MTopView::GetDirectionToPosition(m_X, m_Y, sX, sY);
//
//		sX = m_X;
//		sY = m_Y;
//
//		GetPositionToDirection(sX, sY, direction);
//
//		// »õ·Î¿î ¸ñÇ¥ ÁöÁ¡À¸·Î ¸ø °¡´Â °æ¿ì..
//		if (!m_pZone->CanMove(m_MoveType, sX, sY))
//		{
//			return false;
//		}
//	}



	m_NextDestX = sX;
	m_NextDestY = sY;
	Phoenix_MovementAuditWrite("MOVE_SET_NEXT_DEST_OK", "NEXT_DEST_SET", m_X, m_Y, m_NextDestX, m_NextDestY, m_Direction, m_SendMove);
	CT_BAT("SetNextDestination EXIT – returning true");
	return true;
}

//----------------------------------------------------------------------
// Reset Send Move
//----------------------------------------------------------------------
// server¿¡¼­ ±â´Ù¸®´Â messasge´Â ¾ø´Ù°í Ç¥½ÃÇÑ´Ù.
//----------------------------------------------------------------------
void
MPlayer::ResetSendMove()
{
	g_bNetStatusGood=true;
	m_SendMove = 0;
	m_listSendDirection.clear();
}

void
MPlayer::ClearMovementRetarget()
{
	m_bMoveRetargetPending = FALSE;
	m_RetargetDestX = SECTORPOSITION_NULL;
	m_RetargetDestY = SECTORPOSITION_NULL;
	m_RetargetTick = 0;
}

bool
MPlayer::CanImmediateRetargetMove(const char** reason)
{
	if (reason != NULL)
		*reason = "OK";

	if (m_pZone == NULL)
	{
		if (reason != NULL)
			*reason = "ZONE_NULL";
		return false;
	}

	if (((!m_bAlive || m_bInCasket || IsUndergroundCreature() || HasEffectStatus(EFFECTSTATUS_SLEEP))
		&& !HasEffectStatus(EFFECTSTATUS_GHOST)))
	{
		if (reason != NULL)
			*reason = "ALIVE_STATE_BLOCK";
		return false;
	}

	if (IsWaitVerify())
	{
		if (reason != NULL)
			*reason = "WAIT_VERIFY";
		return false;
	}

	if (m_bFastMove)
	{
		if (reason != NULL)
			*reason = "FAST_MOVE";
		return false;
	}

	if (IsRepeatAction())
	{
		if (reason != NULL)
			*reason = "REPEAT_ACTION";
		return false;
	}

	if (HasEffectStatus(EFFECTSTATUS_ETERNITY_PAUSE))
	{
		if (reason != NULL)
			*reason = "ETERNITY_PAUSE";
		return false;
	}

	if (m_SendMove > Phoenix_GetConfiguredMaxClientMove())
	{
		if (reason != NULL)
			*reason = "MAX_CLIENT_MOVE";
		return false;
	}

	return true;
}

bool
MPlayer::ApplyMovementRetarget(bool forceVisualOnly)
{
	if (!m_bMoveRetargetPending)
		return false;

	if (m_RetargetDestX == SECTORPOSITION_NULL || m_RetargetDestY == SECTORPOSITION_NULL)
	{
		Phoenix_MovementAuditWrite("MOVE_RETARGET_CLEAR", "NULL_TARGET", m_X, m_Y, m_RetargetDestX, m_RetargetDestY, m_Direction, m_SendMove);
		ClearMovementRetarget();
		return false;
	}

	m_DestX = m_RetargetDestX;
	m_DestY = m_RetargetDestY;
	m_NextDestX = SECTORPOSITION_NULL;
	m_NextDestY = SECTORPOSITION_NULL;

	if (m_fTrace == FLAG_TRACE_NULL)
		SetDirectionToPosition(m_DestX, m_DestY);

	bool pathReady = true;
	if (!forceVisualOnly && !m_bFastMove)
		pathReady = SetDestination(m_DestX, m_DestY);
	CT_BAT("SetDestination SUCCESS – dest set (%d,%d) pathReady=%d", m_DestX, m_DestY, pathReady);

	Phoenix_MovementAuditWrite(
		pathReady ? "MOVE_RETARGET_APPLIED" : "MOVE_RETARGET_PATH_FAIL",
		forceVisualOnly ? "VISUAL_ONLY" : "PATH_REFRESH",
		m_X,
		m_Y,
		m_DestX,
		m_DestY,
		m_Direction,
		m_SendMove);

	ClearMovementRetarget();
	return pathReady;
}

bool
MPlayer::RequestMovementRetarget(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)
{
	const char* rejectReason = NULL;
	if (!CanImmediateRetargetMove(&rejectReason))
	{
		CT_BAT("RequestMovementRetarget REJECT-IMMEDIATE – reason=%s sendMove=%d fastMove=%d waitVerify=%d", rejectReason ? rejectReason : "?", (int)m_SendMove, (int)m_bFastMove, (int)IsWaitVerify());
		Phoenix_MovementAuditWrite("MOVE_RETARGET_REJECT", rejectReason, m_X, m_Y, sX, sY, m_Direction, m_SendMove);
		ClearMovementRetarget();
		return false;
	}

	Phoenix_MovementAuditWrite("MOVE_RETARGET_REQUEST", "GROUND_CLICK", m_X, m_Y, sX, sY, m_Direction, m_SendMove);

	if (!SetMovePosition(sX, sY))
	{
		CT_BAT("RequestMovementRetarget REJECT – SetMovePosition FAILED");
		Phoenix_MovementAuditWrite("MOVE_RETARGET_REJECT", "SET_MOVE_POSITION_FAILED", m_X, m_Y, sX, sY, m_Direction, m_SendMove);
		ClearMovementRetarget();
		return false;
	}

	m_bMoveRetargetPending = TRUE;
	m_RetargetDestX = (m_NextDestX != SECTORPOSITION_NULL) ? m_NextDestX : sX;
	m_RetargetDestY = (m_NextDestY != SECTORPOSITION_NULL) ? m_NextDestY : sY;
	m_RetargetTick = GetTickCount();
	TYPE_SECTORPOSITION appliedDestX = m_RetargetDestX;
	TYPE_SECTORPOSITION appliedDestY = m_RetargetDestY;

	TraceNULL();
	SetNextActionToMove();

	bool applied = ApplyMovementRetarget(false);
	CT_BAT("RequestMovementRetarget %s – applied=%s", applied?"APPLIED":"VISUAL_ONLY", applied?"true":"false");
	Phoenix_MovementAuditWrite(
		applied ? "MOVE_RETARGET_ACCEPT" : "MOVE_RETARGET_ACCEPT_VISUAL_ONLY",
		"REQUEST_COMPLETE",
		m_X,
		m_Y,
		appliedDestX,
		appliedDestY,
		m_Direction,
		m_SendMove);

	CT_BAT("RequestMovementRetarget ENTER – target=(%d,%d) immediateCheck=%s", sX, sY, rejectReason?rejectReason:"OK");
	CT_BAT("RequestMovementRetarget EXIT – returning true");
	return true;
}

//----------------------------------------------------------------------
// Get DeadDelay Last
//----------------------------------------------------------------------
// Player°¡ Á×°í ³­ ÈÄ, ±â´Ù¸®´Â ½Ã°£..
//----------------------------------------------------------------------
DWORD			
MPlayer::GetDeadDelayLast() const
{
	int second = m_DeadDelayTime - g_CurrentTime;

	if (second < 0) 
	{
		return 0;
	}
	
	return second/1000;
}

//----------------------------------------------------------------------
// Get NextDestination()
//----------------------------------------------------------------------
void
MPlayer::GetNextDestination(POINT &dest)
{
	dest.x = m_NextDestX;
	dest.y = m_NextDestY;
}

//----------------------------------------------------------------------
// Get NextPosition()
//----------------------------------------------------------------------
// ±æÃ£±â¿¡ ÀÇÇØ¼­ Á¤ÇØÁø ±æµé Áß¿¡¼­
// ¹Ù·Î ´ÙÀ½¿¡ °¡¾ßÇÒ Sector¿¡ ´ëÇÑ ÁÂÇ¥¸¦ ³Ñ°ÜÁØ´Ù.
//
// 'ÇöÀçÀ§Ä¡'¿¡¼­ '´ÙÀ½ ¹æÇâ'¿¡ ´ëÇØ ÀûÀýÇÑ º¯È­¸¦ ´õÇØÁØ´Ù.
//----------------------------------------------------------------------
bool
MPlayer::GetNextPosition(POINT &next)
{
	// °¥ ±æÀÌ Á¤ÇØÁöÁö ¾ÊÀº °æ¿ì
	if (m_listDirection.empty())
	{
		//next.x = SECTORPOSITION_NULL;
		//next.y = SECTORPOSITION_NULL;
		return false;
	}

	
	// °¥ ±æÀÌ Á¤ÇØÁø °æ¿ì, ´ÙÀ½ ÁÂÇ¥¸¦ returnÇÑ´Ù.

	// ÇöÀç À§Ä¡¿¡¼­ ´ÙÀ½ ¹æÇâ¿¡ ´ëÇØ¼­ ¸ñÇ¥ ¼³Á¤
	next.x = m_X;
	next.y = m_Y;
	
	switch (m_listDirection.front())
	{
		case DIRECTION_LEFTDOWN		: next.x--;	next.y++;	break;
		case DIRECTION_RIGHTUP		: next.x++;	next.y--;	break;
		case DIRECTION_LEFTUP		: next.x--;	next.y--;	break;
		case DIRECTION_RIGHTDOWN	: next.x++;	next.y++;	break;
		case DIRECTION_LEFT			: next.x--;				break;
		case DIRECTION_DOWN			:			next.y++;	break;
		case DIRECTION_UP			:			next.y--;	break;
		case DIRECTION_RIGHT		: next.x++;				break;
	}

	return true;
}


//----------------------------------------------------------------------
// Set Destination(sX, sY)
//----------------------------------------------------------------------
// ¸ñÇ¥À§Ä¡¸¦ ÁöÁ¤ÇÏ´Â ¼ø°£¿¡ ¸ñÇ¥À§Ä¡±îÁöÀÇ
// Best Path¸¦ °áÁ¤ÇØ¼­ m_listDirection¿¡ ÀúÀåÇØµÐ´Ù.
//----------------------------------------------------------------------
bool
MPlayer::SetDestination(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)
{
	if(HasEffectStatus(EFFECTSTATUS_SLEEP)
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
		
		return false;

	// message : find path
	//DEBUG_ADD_FORMAT("........Find Path........");				

	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD_FORMAT("SetDest(%d, %d)", sX, sY);
	#endif

	//-------------------------------------------------------
	// »ì¾ÆÀÖÁö ¾ÊÀº °æ¿ì..
	//-------------------------------------------------------
	if (!m_bAlive)
	{
		#ifdef OUTPUT_DEBUG_PLAYER_ACTION
			DEBUG_ADD("dead");
		#endif
		return false;
	}

	//-------------------------------------------------------
	// if not empty, delete all elements
	//-------------------------------------------------------
	if (!m_listDirection.empty())
	{
		m_listDirection.clear();
	}

		// ¼º¹°À» µé¾úÀ»¶§´Â ÀÚ±âÆí ¾ÈÀüÁö´ë´Â ¸ø°£´Ù!
	bool bHasRelic = 
		( m_bEffectStatus[EFFECTSTATUS_HAS_VAMPIRE_RELIC] || m_bEffectStatus[EFFECTSTATUS_HAS_SLAYER_RELIC] ) && 
		(g_pZone->GetSector(sX, sY).IsSafeSlayer() && IsSlayer() ||
		g_pZone->GetSector(sX, sY).IsSafeVampire() && IsVampire() ||
		g_pZone->GetSector(sX, sY).IsSafeOusters() && IsOusters()
		);

	bool bHasBloodBible =
	(
	m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_ARMEGA] || m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_MIHOLE] ||
	m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_KIRO] ||m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_INI] ||
	m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_GREGORI] ||m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_CONCILIA] ||
	m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_LEGIOS] ||m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_HILLEL] ||
	m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_JAVE] ||m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_NEMA] ||
	m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_AROSA] ||m_bEffectStatus[EFFECTSTATUS_HAS_BLOOD_BIBLE_CHASPA] ) && 
		(g_pZone->GetSector(sX, sY).IsSafeSlayer() && IsSlayer() ||
		g_pZone->GetSector(sX, sY).IsSafeOusters() && IsOusters() ||
		g_pZone->GetSector(sX, sY).IsSafeVampire() && IsVampire()
		);

	if(bHasBloodBible)
	{
		g_pSystemMessage->Add((*g_pGameStringTable)[STRING_MESSAGE_CANNOT_MOVE_SAFETY_ZONE_BLOOD_BIBLE].GetString());
	}

	if(bHasRelic)
	{
		g_pSystemMessage->Add((*g_pGameStringTable)[STRING_MESSAGE_CANNOT_MOVE_SAFETY_ZONE_RELIC].GetString());
	}

	if( HasEffectStatus( EFFECTSTATUS_HAS_FLAG ) &&
		(g_pZone->GetSector(sX, sY).IsSafeSlayer() && IsSlayer() ||
		g_pZone->GetSector(sX, sY).IsSafeVampire() && IsVampire() ||
		g_pZone->GetSector(sX, sY).IsSafeOusters() && IsOusters()
		) )
	{
		bHasRelic = true;
		g_pSystemMessage->Add((*g_pGameStringTable)[STRING_MESSAGE_CANNOT_MOVE_SAFETY_ZONE_FLAG].GetString() );
	}

	//-------------------------------------------------------
	// ZoneÀÇ ¿µ¿ª ¹ÛÀÌ¸é check ¾ÈÇÑ´Ù.
	//-------------------------------------------------------
    // ----- EKLE BAŞLANGIÇ -----
    bool bReject = false;
    const char* reason = NULL;
	
    if (sX<0 || sY<0 || sX>=m_pZone->GetWidth() || sY>=m_pZone->GetHeight()) reason = "OOB";
	else if (g_pZone->GetSector(m_X, m_Y).HasSanctuary()) reason = "SANCTUARY";
    else if (bHasRelic) reason = "RELIC";
    else if (bHasBloodBible) reason = "BLOODBIBLE";
	if (reason != NULL)
    {
    {
        CT_BAT("SetDestination REJECT – %s (x=%d,y=%d)", reason, sX, sY);
        Phoenix_MovementAuditWrite("MOVE_SET_NEXT_DEST_FAIL", reason, m_X, m_Y, sX, sY, m_Direction, m_SendMove);
        m_MoveCount = m_MoveCountMax;
        m_DestX = SECTORPOSITION_NULL;
        m_DestY = SECTORPOSITION_NULL;
        return false;
    }
    // ----- EKLE BİTİŞ -----
	/*if (sX<0 || sY<0 
		|| sX>=m_pZone->GetWidth() || sY>=m_pZone->GetHeight()
		// [»õ±â¼ú] Sanctuary ·Î´Â ¸ø ¿òÁ÷ÀÎ´Ù.
		|| g_pZone->GetSector(m_X, m_Y).HasSanctuary()
		|| bHasRelic
		|| bHasBloodBible
		)
	{
		// Zone¿µ¿ª ¹ÛÀÌ¸é check ¾È ÇÒ °æ¿ì
		//m_ActionCount = m_ActionCountMax;
		m_MoveCount = m_MoveCountMax;
		m_DestX = SECTORPOSITION_NULL;
		m_DestY = SECTORPOSITION_NULL;

		#ifdef OUTPUT_DEBUG_PLAYER_ACTION
			DEBUG_ADD("outOfZone or Sanctuary");
		#endif

		return false;*/

		// ÇöÀç ÁøÇà ¹æÇâÀ¸·Î ÀÌµ¿ÇÏ°Ô ÇÏ´Â °æ¿ì
		/*
		POINT position = g_pTopView->GetSelectedSector(g_x, g_y);		
		BYTE direction = GetDirectionToPosition(position.x, position.y);		

		// ¹æÇâ¿¡ µû¸¥ À§Ä¡¸¦ °áÁ¤ÇÑ´Ù.		
		MCreature::GetNextPosition(direction, position);
		
		sX = position.x;
		sY = position.y;
		
		// ÇöÀç ÁøÇà ¹æÇâÀÌ ZoneÀ» ¹þ¾î³ª´Â °æ¿ì
		if (sX<0 || sY<0 
			|| sX>=m_pZone->GetWidth() || sY>=m_pZone->GetHeight()) 
		{
			return false;
		}
		*/
	}

	//-------------------------------------------------------
	// ÃßÀû ÁßÀÏ °æ¿ì, ÀÌ¹Ì »çÁ¤ °Å¸®¿¡ ÀÖÀ» ¶§...
	//-------------------------------------------------------
	if (m_fTrace!=FLAG_TRACE_NULL
		&& max(abs(m_TraceX-m_X), abs(m_TraceY-m_Y))<=m_TraceDistance)
	{
		//m_ActionCount = m_ActionCountMax;
		m_MoveCount = m_MoveCountMax;

		m_DestX = SECTORPOSITION_NULL;
		m_DestY = SECTORPOSITION_NULL;
	
		#ifdef OUTPUT_DEBUG_PLAYER_ACTION
			DEBUG_ADD("already in dist");
		#endif

		return false;
	}

	/*
	// priority std::queue test code
	std::ofstream file("test.txt", std::ios::app);
	CString str;

	m_pqDNodes.push( new DNode(0,0,0,1) );
	m_pqDNodes.push( new DNode(0,0,0,4) );
	m_pqDNodes.push( new DNode(0,0,0,3) );
	m_pqDNodes.push( new DNode(0,0,0,8) );
	m_pqDNodes.push( new DNode(0,0,0,2) );
	m_pqDNodes.push( new DNode(0,0,0,4) );
	m_pqDNodes.push( new DNode(0,0,0,7) );

	while (!m_pqDNodes.empty())
	{
		DNode*	pDNode = m_pqDNodes.top();
		m_pqDNodes.pop();

		str.Format("Pop : (%d,%d) , [%d], dist=%d", pDNode->x, pDNode->y, pDNode->direction, pDNode->distance);
		
		file << str << std::endl;
	}

	return;
	*/

	//--------------------------------------------------------------
	// °¥ ¼ö ÀÖ´Â °÷ÀÌ¸é
	//--------------------------------------------------------------
	// Best First Search
	//--------------------------------------------------------------
	BOOL	bCanStand = m_pZone->CanMove(m_MoveType, sX,sY);
	CT_BAT("SetDestination CANMOVE – moveType=%d canMove=%d sector=(%d,%d)", m_MoveType, bCanStand, sX, sY);

#ifdef __EXPO_CLIENT__
	bCanStand |= (BOOL)g_UserInformation.Invisible;
#endif

	//--------------------------------------------------------------
	// °¥ ¼ö ÀÖ´Â °÷ÀÌ°Å³ª
	// ÃßÀûÁßÀÌ¸é Best Search~·Î ±æÀ» Ã£´Â´Ù.
	//--------------------------------------------------------------
	CT_BAT("SetDestination DECISION: bCanStand=%d, fTrace=%d", (int)bCanStand, (int)m_fTrace);
	if (bCanStand || m_fTrace!=FLAG_TRACE_NULL) 
	{

		CT_BAT("SetDestination ACCEPTED - Pathfinding/Moving started");
		
		//-------------------------------------------------------
		// ¸ñÇ¥À§Ä¡¸¦ ÀúÀå
		//-------------------------------------------------------
		m_DestX = sX;
		m_DestY = sY;

		//-------------------------------------------------------
		// priority std::queue¸¦ ÃÊ±âÈ­ÇÑ´Ù.
		//-------------------------------------------------------
		RemoveNodes();	

		//-------------------------------------------------------
		// visited¸¦ ¸ðµÎ false·Î ÇØµÎ°í
		// ÇöÀçÀ§Ä¡ºÎÅÍ ¹æ¹®ÇÏÁö ¾ÊÀº °÷µéÀ» ¹æ¹®ÇÏ±â ½ÃÀÛÇÑ´Ù.
		//-------------------------------------------------------
		POINT firstSector = g_pTopView->GetFirstSector();
		//int x0 = m_DestX - g_pClientConfig->MAX_FINDPATH_WIDTH, 
		//	x1 = m_DestX + g_pClientConfig->MAX_FINDPATH_WIDTH+1, 
		//	y0 = m_DestY - g_pClientConfig->MAX_FINDPATH_HEIGHT,
		//	y1 = m_DestY + g_pClientConfig->MAX_FINDPATH_HEIGHT+1;
		int x0 = firstSector.x - 1, 
			x1 = firstSector.x + SECTOR_WIDTH + 5, 
			y0 = firstSector.y - 1,
			y1 = firstSector.y + SECTOR_HEIGHT + 5;

		if (x0 < 0)						
			x0 = 0;
		if (x1 > g_pZone->GetWidth())	
			x1 = g_pZone->GetWidth();
		
		if (y0 < 0)				
			y0 = 0;
		if (y1 > g_pZone->GetHeight())	
			y1 = g_pZone->GetHeight();
		
		int size = x1-x0;	// byte¼ö
		for (int i=y0; i<y1; i++)
		{			
			for (int j=x0; j<x1; j++)
			{
				//m_ppVisited[i][j] = false;
				g_pZone->UnSetVisitedFlag(j, i);
			}
			//memset( m_ppVisited[i]+x0, false, size );
		}
			

		//-------------------------------------------------------
		//
		// ÇöÀç À§Ä¡
		//
		//-------------------------------------------------------

		//-------------------------------------------------------
		// ¸ñÀûÁö¸¦ ÇâÇÑ ¹æÇâºÎÅÍ ±æÃ£±â¸¦ ÇÑ´Ù.		
		//-------------------------------------------------------
		static BYTE FirstDirection = GetDirectionToPosition(m_DestX, m_DestY);
		static TYPE_SECTORPOSITION previousDestX = SECTORPOSITION_NULL;
		static TYPE_SECTORPOSITION previousDestY = SECTORPOSITION_NULL;

		//-------------------------------------------------------
		// ¹æ±Ý ÀüÀÇ ±æÃ£±â Çß´ø°Å¶û ¸ñÇ¥ À§Ä¡°¡ ´Ù¸¥ °æ¿ì¿¡¸¸ 
		// ¸ñÀûÁö¸¦ ÇâÇÑ ¹æÇâÀ» ¼³Á¤ÇÑ´Ù.
		//-------------------------------------------------------
		if (previousDestX!=m_DestX || previousDestY!=m_DestY)
		{
			FirstDirection = GetDirectionToPosition(m_DestX, m_DestY);
			previousDestX = m_DestX;
			previousDestY = m_DestY;
		}
		//-------------------------------------------------------
		// °°Àº ¸ñÇ¥¸¦ ÇâÇØ ³ª¾Æ°¡´Â °æ¿ì´Â 
		// ÇöÀç PlayerÀÇ ¹æÇâºÎÅÍ »ìÆìº»´Ù.
		//-------------------------------------------------------
		else
		{
			FirstDirection = m_Direction;
		}
		
		DNode*	currentDNode = new DNode(
									m_X,
									m_Y,
									FirstDirection,
									CalculateDistance(m_X, m_Y),
									0
									);
	
		//-------------------------------------------------------
		// listDNodes¿¡ ³Ö¾î¼­ ³ªÁß¿¡ Áö¿öÁØ´Ù.
		//-------------------------------------------------------
		m_listDNodes.push_back( currentDNode );

		currentDNode->SetParent(NULL);
		m_pqDNodes.push(currentDNode);	
		
		POINT	next;
		int		dist;
		bool	bFound = false;		// ¾ÆÁ÷ µµ´ÞÇÏÁö ¾Ê¾Ò´Ù´Â ÀÇ¹Ì

		while (!bFound && !m_pqDNodes.empty())
		{
			//-------------------------------------------------------
			// Á¦ÀÏ ±¦ÂúÀº(!) Node¸¦ °ñ¶ó¼­(priority queueÀÌ´Ù)
			// ¾îµð·Î °¡¾ßÇÒÁö¿¡ ´ëÇØ¼­ °è»êÇØº»´Ù.
			//-------------------------------------------------------
			DNode* pDNode = m_pqDNodes.top();
			m_pqDNodes.pop();

			//-------------------------------------------------------
			// ¿ÏÀüÈ÷ µµÂøÇÑ °æ¿ì
			//-------------------------------------------------------
			if (pDNode->distance==0) 
			{
				m_pqDNodes.push(pDNode);
				bFound = true;
				break;
			}
			
			//-------------------------------------------------------
			// »çÁ¤ °Å¸® ÀÌ³»·Î Á¢±ÙÇÏ±â
			//-------------------------------------------------------
			if (m_fTrace!=FLAG_TRACE_NULL && 
				max(abs(pDNode->x-m_DestX), abs(pDNode->y-m_DestY))<=m_TraceDistance)
			{
				m_pqDNodes.push(pDNode);
				bFound = true;
				break;
			}			

			int Direction = pDNode->direction;

			//-------------------------------------------------------
			// 8¹æÇâÀ» ¸ðµÎ check
			//-------------------------------------------------------
			for (int i=0; i<8; Direction++, i++)
			{		
				if (Direction==8) Direction=0;

				next.x = pDNode->x;
				next.y = pDNode->y;

				switch (Direction)
				{
					case DIRECTION_LEFTDOWN		: next.x--;	next.y++;	break;
					case DIRECTION_RIGHTUP		: next.x++;	next.y--;	break;
					case DIRECTION_LEFTUP		: next.x--;	next.y--;	break;
					case DIRECTION_RIGHTDOWN	: next.x++;	next.y++;	break;
					case DIRECTION_LEFT			: next.x--;				break;
					case DIRECTION_DOWN			: next.y++;				break;
					case DIRECTION_UP			: next.y--;				break;
					case DIRECTION_RIGHT		: next.x++;				break;
				}

				//-------------------------------------------------------
				// ZoneÀÇ ¿µ¿ª ¹ÛÀÌ¸é check ¾ÈÇÑ´Ù.
				//-------------------------------------------------------
				if (next.x<0 || next.y<0 
					|| next.x>=m_pZone->GetWidth() || next.y>=m_pZone->GetHeight()) continue;

				//-------------------------------------------------------
				// °¬´ø °÷ÀÌ¸é ¾È °£´Ù.
				//-------------------------------------------------------
				if (g_pZone->IsVisitedFlag(next.x, next.y)) continue;

				// ÇÑ È­¸éÀ» ³Ñ¾î°¡´Â °æ¿ì´Â checkÇÏÁö ¾Ê´Â´Ù.				
				if (next.x<x0 || next.y<y0 
					|| next.x>=x1 || next.y>=y1) continue;

				//-------------------------------------------------------
				// °¥ ¼ö ÀÖÀ¸¸é pqueue¿¡ ³Ö´Â´Ù.
				//-------------------------------------------------------
				if (m_pZone->CanMove(m_MoveType, next.x, next.y)
					&& !bHasRelic && !bHasBloodBible

#ifdef __EXPO_CLIENT__
	|| g_UserInformation.Invisible
#endif
					
					)
				{		
					g_pZone->SetVisitedFlag(next.x, next.y);
//					m_ppVisited[next.y][next.x] = true;

					//-------------------------------------------------------
					// Node¸¦ ¸¸µé¾î¼­ priority std::queue¿¡ Ãß°¡ÇÑ´Ù.
					//-------------------------------------------------------
					// °Å¸® °è»ê
					dist = CalculateDistance(next.x, next.y);

					// Node¸¦ ¸¸µé¾î¼­ Ãß°¡
					DNode*	nextDNode = new DNode(
											next.x, 
											next.y, 
											Direction,
											dist,
											pDNode->step + 1);
					
				
					//-------------------------------------------------------
					// listDNodes¿¡ ³Ö¾î¼­ ³ªÁß¿¡ Áö¿öÁØ´Ù.
					//-------------------------------------------------------
					m_listDNodes.push_back( nextDNode );
					
					//CString str;
					//str.Format("NewNode : (%d,%d)-->(%d,%d) , [%d], dist=%d", next.x, next.y, m_Dest.x, m_Dest.y, i, dist);

					nextDNode->SetParent(pDNode);
					m_pqDNodes.push(nextDNode);					
				}

			
			}
		}

		//-------------------------------------------------------
		// Ã£¾ÒÀ» °æ¿ì ±æÀ» Á¤ÇØ¾ß ÇÑ´Ù.
		//-------------------------------------------------------
		if (bFound)
		{				
			//-------------------------------------------------------
			// m_pqDNodes·ÎºÎÅÍ list¸¦ Ã¤¿î´Ù. (parent¸¦ µû¶ó°¡¾ßÇÑ´Ù)
			//-------------------------------------------------------

			// ¸ñÇ¥À§Ä¡
			DNode* currentNode = m_pqDNodes.top();
			m_pqDNodes.pop();
			
			while (currentNode->pParent!=NULL)
			{
				m_listDirection.insert( m_listDirection.begin(), currentNode->direction);
				currentNode = currentNode->pParent;			
			}

			RemoveNodes();

			m_BlockDestX = SECTORPOSITION_NULL;
			m_BlockDestY = SECTORPOSITION_NULL;
		}
		//-------------------------------------------------------
		// ±æÀÌ ¾øÀ» °æ¿ì Á¤ÁöÇÑ´Ù.
		//-------------------------------------------------------
		else
		{
			// ÃßÀû ÁßÁö
			if (m_fTrace!=FLAG_TRACE_NULL)				
			{
				m_MoveCount = m_MoveCountMax;

				TraceNULL();

				// 2001.7.31 Ãß°¡
				m_nUsedActionInfo = ACTIONINFO_NULL;
				m_nNextUsedActionInfo = ACTIONINFO_NULL;
			}

			m_BlockDestX = m_DestX;
			m_BlockDestY = m_DestY;

			m_DestX = SECTORPOSITION_NULL;
			m_DestY = SECTORPOSITION_NULL;			
		}
	}
	// °¥ ¼ö ¾ø´Â °÷
	else 
	{
		//m_ActionCount = m_ActionCountMax;
		m_MoveCount = m_MoveCountMax;

		// ÀÌ¹ø¿¡ °¥ ¼ö ¾ø¾ú´ø °÷ ¼³Á¤
		//m_BlockDestX = m_DestX;
		//m_BlockDestY = m_DestY;

		m_DestX = SECTORPOSITION_NULL;
		m_DestY = SECTORPOSITION_NULL;		

		#ifdef OUTPUT_DEBUG_PLAYER_ACTION
			DEBUG_ADD("no way!");
		#endif
	// ----- EKLE BAŞLANGIÇ -----
				CT_BAT("SetDestination REJECT – CANMOVE false bCanStand=%d trace=%d", (int)bCanStand, (int)m_fTrace);
		Phoenix_MovementAuditWrite("MOVE_SET_NEXT_DEST_FAIL", "CANMOVE_FALSE", m_X, m_Y, sX, sY, m_Direction, m_SendMove);
    // ----- EKLE BİTİŞ -----
		return false;
	}

	/*
	else
	// ÀÌ ºÎºÐ¿¡ ¾à°£ÀÇ ¿À·ù°¡ ÀÖ´Â µí ÇÔ!! -_-;;;;
	//--------------------------------------------------------------
	// ¾Æ¿¹ °¥ ¼ö ¾ø´Â °÷ÀÌ¸é..
	//--------------------------------------------------------------
	// PlayerÀ§Ä¡¿¡¼­ ¸ñÇ¥ÁöÁ¡±îÁö Á÷¼±?À¸·Î °É¾î°¡´Ù°¡ 
	// Àå¾Ö¹°ÀÌ »ý±â¸é ¸ØÃá´Ù.
	// (m_X, m_Y)  --->  (sX, sY)
	//--------------------------------------------------------------		
	{
		int	stepX = sX - m_X,
			stepY = sY - m_Y,
			x = m_X,
			y = m_Y,
			signX, signY;

		//-------------------------------------------------------
		// if not empty, delete all elements
		//-------------------------------------------------------
		if (!m_listDirection.empty())
		{
			m_listDirection.clear();
		}
			
		
		while (1)
		{
			// ºÎÈ£¿¡ µû¶ó¼­..
			signX = (stepX==0)? 0 : (stepX<0)? -1 : 1;
			signY = (stepY==0)? 0 : (stepY<0)? -1 : 1;

			x += signX;
			y += signY;
				
			// °¥ ¼ö ÀÖ´Â °÷ÀÌ¸é list¿¡ ³Ö´Â´Ù.
			if (m_pZone->CanMove(x,y))
			{				
				m_listDirection.insert( m_listDirection.end(), DetermineDirection(stepX, stepY));
			}
			else 
			{
				// x,y ÇÑÄ­ ÀüÀÇ À§Ä¡¸¦ ¸ñÇ¥ÁöÁ¡À¸·Î Á¤ÇÑ´Ù.
				m_DestX = x - signX;
				m_DestY = y - signY;
				break;
			}	
			
			// ÇÑ Ä­ ¿òÁ÷¿´À½À» Ç¥½Ã
			stepX -= signX;
			stepY -= signY;
		}				
		
		return;
	}
	*/

	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD("find way");
	#endif

	return true;
}

//----------------------------------------------------------------------
// Self Special Action
//----------------------------------------------------------------------
// ÀÚ½ÅÇÑÅ× Æ¯¼ö±â¼ú »ç¿ë
//----------------------------------------------------------------------
bool	
MPlayer::SelfSpecialAction()
{
	if(HasEffectStatus(EFFECTSTATUS_SLEEP)
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
		return false;

	//-----------------------------------------------------
	// delay°¡ ÀÖÀ¸¸é ¾ÈµÈ´Ù.
	//-----------------------------------------------------
	if (!IsNotDelay() || HasEffectStatus( EFFECTSTATUS_ETERNITY_PAUSE ) )
	{
		return false;
	}

	// [»õ±â¼ú3] °ü ¼Ó¿¡ ÀÖ´Â °æ¿ì´Â Open casket¸¸ »ç¿ëÇÒ ¼ö ÀÖ´Ù.
	//if (IsInCasket())
//	{
//		m_nSpecialActionInfo = MAGIC_OPEN_CASKET;
//	}

	
	if (m_nSpecialActionInfo == ACTIONINFO_NULL
		// Hava araci (Hover/Heli) bindiginde ozel saldiri kullanilamaz.
		|| IsRide_Air_Transporter()
		// ¹º°¡ ±â¼ú »ç¿ëÀ» °ËÁ¤ ¹Þ¾Æ¾ß ÇÏ¸é »ç¿ëÇÒ ¼ö ¾ø´Ù.
		//|| m_WaitVerify != WAIT_VERIFY_NULL)	// 2001.8.20 ÁÖ¼®Ã³¸®
		|| IsInSafeSector() == 2
		// ¿ÏÀü ¾ÈÀüÁö´ë¿¡¼­´Â ±â¼úÀ» »ç¿ëÇÒ ¼ö ¾ø´Ù.
		)
		return false;
	
	// 2006.08.22 sjheon
	if(HasEffectStatus(EFFECTSTATUS_TRANSFORM_TO_WOLF))
	{
		if( !(m_nSpecialActionInfo == MAGIC_HOWL || m_nSpecialActionInfo == MAGIC_EAT_CORPSE || m_nSpecialActionInfo == MAGIC_UN_TRANSFORM))
			return false ;
	}
	else if(HasEffectStatus(EFFECTSTATUS_TRANSFORM_TO_BAT))
	{
		if( !(m_nSpecialActionInfo == MAGIC_UN_TRANSFORM))
			return false ;
	}	
	else if(HasEffectStatus(EFFECTSTATUS_TRANSFORM_TO_WERWOLF))
	{
		if( !(m_nSpecialActionInfo == SKILL_BITE_OF_DEATH	||
			  m_nSpecialActionInfo == MAGIC_RAPID_GLIDING	||
			  m_nSpecialActionInfo == SKILL_VIOLENT_PHANTOM ||
			  m_nSpecialActionInfo == SKILL_WILD_WOLF		||
			 // m_nSpecialActionInfo == SKILL_LETHAL_CLAW		||  //ÀÇÈË±äÉí¼¼ÄÜ MAGIC_DARKNESS
			  m_nSpecialActionInfo == MAGIC_BLOODY_ZENITH	||
			  m_nSpecialActionInfo == MAGIC_DARKNESS	||
			  m_nSpecialActionInfo == MAGIC_UN_TRANSFORM))
			return false ;
	}


//	|| HasEffectStatus(EFFECTSTATUS_TRANSFORM_TO_BAT) || 
//	   HasEffectStatus(EFFECTSTATUS_TRANSFORM_TO_WERWOLF))
//	{
//		SKILL_BITE_OF_DEATH				MAGIC_RAPID_GLIDING
//		if( !(m_nSpecialActionInfo == MAGIC_HOWL || m_nSpecialActionInfo == MAGIC_EAT_CORPSE || m_nSpecialActionInfo == MAGIC_UN_TRANSFORM))
//			return false ;
//	}

	// ÀÚ½Å¿¡°Ô »ç¿ëÇÒ ¼ö ÀÖ´Â °Íµé¸¸ »ç¿ëÇÑ´Ù.
	if ((*g_pActionInfoTable)[m_nSpecialActionInfo].IsTargetSelf())
	{	
		
		//-------------------------------------------------------
		// Áö·Ú ¸¸µé±â
		//-------------------------------------------------------
		// skillID            itemType
		// MINE_ANKLE_KILLER,	// 5
		// MINE_POMZ,			// 6
		// MINE_AP_C1,			// 7
		// MINE_DIAMONDBACK,	// 8
		// MINE_SWIFT_EX		// 9
		//
		// ÇÏµåÇÏµå...
		//			
		if (m_nSpecialActionInfo==SKILL_INSTALL_MINE
			|| m_nSpecialActionInfo>=MINE_ANKLE_KILLER 
				&& m_nSpecialActionInfo<=MINE_SWIFT_EX)
		{
			int					useSkill = SKILL_INSTALL_MINE;					

			//-------------------------------------------------------
			// ÇöÀç »ç¿ëÇÒ ¼ö ÀÖ´Â ±â¼úÀÎÁö Ã¼Å©..
			// Passive ½ºÅ³ÀÌ¸é »ç¿ë ¸øÇÏ°Ô..
			//-------------------------------------------------------
			if (g_pSkillAvailable->IsEnableSkill( (ACTIONINFO)useSkill )
				&& (*g_pSkillInfoTable)[useSkill].IsEnable()
				&& (*g_pSkillInfoTable)[useSkill].IsAvailableTime()
				&& !(*g_pSkillInfoTable)[useSkill].IsPassive()
				&& IsSlayer()
				&& !IsWaitVerify()
				&& IsItemCheckBufferNULL())
			{					

				ITEM_CLASS	itemClass = ITEM_CLASS_MINE;

				MItem* pItem = NULL;

				if (m_nSpecialActionInfo!=SKILL_INSTALL_MINE)
				{
					// Æ¯Á¤ typeÀÇ Áö·Ú¸¦ Ã£´Â´Ù.
					int itemType = m_nSpecialActionInfo-MINE_ANKLE_KILLER;

					MItemClassTypeFinder itemFinder(itemClass, itemType);
					pItem = ((MItemManager*)g_pInventory)->FindItem( itemFinder );
				}
				else
				{
					// Áö·Ú ¾Ï²¨³ª Ã£´Â´Ù.
					MItemClassFinder itemFinder(itemClass);
					pItem = ((MItemManager*)g_pInventory)->FindItem( itemFinder );
				}
				
				// ¾µ·Á´Â ÆøÅºÀÌ ÀÖ´Â °æ¿ì
				if (pItem!=NULL)
				{
					// Áö·Ú ¼³Ä¡ ÁØºñ..
					gC_vs_ui.StartInstallMineProgress( pItem->GetGridX(), pItem->GetGridY() );
				}									
			}

			// ¹Ýº¹µ¿ÀÛÀ» ¼³Á¤ÇÏÁö ¾Ê´Â´Ù.
			m_fNextTrace = FLAG_TRACE_NULL;
			UnSetRepeatAction();
			return false;	
		}

		//-------------------------------------------------------
		// ÇöÀç »ç¿ëÇÒ ¼ö ÀÖ´Â ±â¼úÀÎÁö Ã¼Å©..
		// Passive ½ºÅ³ÀÌ¸é »ç¿ë ¸øÇÏ°Ô..
		//-------------------------------------------------------
		if (!g_pSkillAvailable->IsEnableSkill( (ACTIONINFO)m_nSpecialActionInfo )
			|| !(*g_pSkillInfoTable)[m_nSpecialActionInfo].IsEnable()
			|| !(*g_pSkillInfoTable)[m_nSpecialActionInfo].IsAvailableTime()
			|| (*g_pSkillInfoTable)[m_nSpecialActionInfo].IsPassive())
		{
			m_fNextTrace = FLAG_TRACE_NULL;

			UnSetRepeatAction();

			return false;
		}

		if (IsStop()) // || m_Action!=ACTION_ATTACK)
		{
			//-------------------------------------------------------
			// Æ¯Á¤ÇÑ ±â¼úÀÎ °æ¿ìÀÇ Ã¼Å©
			//-------------------------------------------------------
			switch (m_nSpecialActionInfo)
			{
				//-------------------------------------------------------
				// Æ÷Å»
				//-------------------------------------------------------
				case MAGIC_BLOODY_MARK :
					if (IsVampire() && g_bHolyLand == false && g_pZoneTable->Get( m_pZone->GetID() )->CannotUseSpecialItem == false)
					{
						//----------------------------------------------------
						// Server¿¡ Á¢¼ÓÇÑ °æ¿ì
						//----------------------------------------------------
							//----------------------------------------------------
							// °ËÁõ ¹ÞÀ»°Ô ¾ø´Â °æ¿ì..
							//----------------------------------------------------
							if (!IsWaitVerify()
								&& IsItemCheckBufferNULL())
							{									
								// 2005, 3, 2, sobeit modify start - FindItem() ´ë½Å FindItemAll() »ç¿ë
								// »ç¿ë¾ÈµÈ(Marked=false) VampirePortalItemÀ» Ã£´Â´Ù.
								
								MItem* pSubInventory = NULL;
								MItem* pItem = ((MItemManager*)g_pInventory)->FindItemAll( MVampirePortalItemFinder( false ) , pSubInventory );
								
								if (pItem!=NULL && pItem->GetNumber()>0 && pItem->IsAffectStatus())
								{					
									DWORD SubInventoryItemID = 0;
									//----------------------------------------------------
									// Server¿¡ Á¢¼ÓÇÑ °æ¿ì
									//----------------------------------------------------
									CGSkillToInventory _CGSkillToInventory;
									_CGSkillToInventory.setObjectID( pItem->GetID() );
									_CGSkillToInventory.setX( pItem->GetGridX() );
									_CGSkillToInventory.setY( pItem->GetGridY() );
									_CGSkillToInventory.setSkillType( MAGIC_BLOODY_MARK );
									//_CGSkillToInventory.setCEffectID( 0 );	// -_-;;							

									if(NULL != pSubInventory)
									{
										SubInventoryItemID = pSubInventory->GetID();
									}
									_CGSkillToInventory.setInventoryItemObjectID( SubInventoryItemID );

									g_pSocket->sendPacket( &_CGSkillToInventory );

									//----------------------------------------------------
									// Inventory¿¡¼­ itemÀ» »ç¿ëÇÏ´Â °É °ËÁõ¹Þ±â¸¦ ±â´Ù¸°´Ù.
									//----------------------------------------------------
									SetWaitVerify( WAIT_VERIFY_SKILL_SUCCESS );
									SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY, SubInventoryItemID);

									g_pSkillInfoTable->SetNextAvailableTime(m_nSpecialActionInfo);
								}
								// 2005, 3, 2, sobeit modify end
							}
					}						
					
					return false;
				break;

				//-------------------------------------------------------
				// Æ÷Å»
				//-------------------------------------------------------
				case MAGIC_BLOODY_TUNNEL :					
					if (IsVampire() && g_bHolyLand == false && g_pZoneTable->Get( m_pZone->GetID() )->CannotUseSpecialItem == false)
					{
							//----------------------------------------------------
							// °ËÁõ ¹ÞÀ»°Ô ¾ø´Â °æ¿ì..
							//----------------------------------------------------
							if (!IsWaitVerify()
								&& IsItemCheckBufferNULL())
							{		
								// 2005, 3, 2, sobeit modify start - FindItem() ´ë½Å FindItemAll() »ç¿ë

								// »ç¿ëµÈ(Marked=true) VampirePortalItemÀ» Ã£´Â´Ù.
//								MItem* pItem = ((MItemManager*)g_pInventory)->FindItem( MVampirePortalItemFinder( true ) );
								
								MItem* pSubInventory = NULL;
								MItem* pItem = ((MItemManager*)g_pInventory)->FindItemAll( MVampirePortalItemFinder( true ) , pSubInventory );
								
								if (pItem!=NULL && pItem->GetNumber()>0 && pItem->IsAffectStatus())
								{				
									DWORD SubInventoryItemID = 0;
									CGSkillToInventory _CGSkillToInventory;
									_CGSkillToInventory.setObjectID( pItem->GetID() );
									_CGSkillToInventory.setX( pItem->GetGridX() );
									_CGSkillToInventory.setY( pItem->GetGridY() );
									_CGSkillToInventory.setSkillType( MAGIC_BLOODY_TUNNEL );
									//_CGSkillToInventory.setCEffectID( 0 );	// -_-;;							

									if(NULL != pSubInventory)
									{
										SubInventoryItemID = pSubInventory->GetID();
									}
									_CGSkillToInventory.setInventoryItemObjectID( SubInventoryItemID );

									g_pSocket->sendPacket( &_CGSkillToInventory );

									//----------------------------------------------------
									// Inventory¿¡¼­ itemÀ» »ç¿ëÇÏ´Â °É °ËÁõ¹Þ±â¸¦ ±â´Ù¸°´Ù.
									//----------------------------------------------------
									SetWaitVerify( MPlayer::WAIT_VERIFY_SKILL_SUCCESS );
									SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY, SubInventoryItemID);

									g_pSkillInfoTable->SetNextAvailableTime(m_nSpecialActionInfo);

								}
							}
					}						
					
					return false;
				break;

				//-------------------------------------------------------
				// Æ÷Å»
				//-------------------------------------------------------
				case SUMMON_HELICOPTER :
					if (IsSlayer() && g_bHolyLand == false && g_pZoneTable->Get( m_pZone->GetID() )->CannotUseSpecialItem == false)
					{
							//----------------------------------------------------
							// °ËÁõ ¹ÞÀ»°Ô ¾ø´Â °æ¿ì..
							//----------------------------------------------------
						
						
						// 2005, 3, 2, sobeit modify start - FindItem() ´ë½Å FindItemAll() »ç¿ë
						if (IsWaitVerifyNULL()
							&& IsItemCheckBufferNULL()
							&& g_pZone->GetHelicopter( GetID() )==NULL)				
						{		
							MItem* pSubInventory = NULL;
							MItem* pItem = ((MItemManager*)g_pInventory)->FindItemAll( MSlayerPortalItemFinder() , pSubInventory);
							
							if (pItem!=NULL && pItem->GetNumber()>0 && pItem->IsAffectStatus())
							{							
								DWORD SubInventoryItemID = 0;
								CGUseItemFromInventory _CGUseItemFromInventory;
								_CGUseItemFromInventory.setObjectID( pItem->GetID() );
								_CGUseItemFromInventory.setX( pItem->GetGridX() );
								_CGUseItemFromInventory.setY( pItem->GetGridY() );

								if(NULL != pSubInventory)
								{
									SubInventoryItemID = pSubInventory->GetID();
								}
								_CGUseItemFromInventory.setInventoryItemObjectID( SubInventoryItemID );
								
								g_pSocket->sendPacket( &_CGUseItemFromInventory );

								//----------------------------------------------------
								// Inventory¿¡¼­ itemÀ» »ç¿ëÇÏ´Â °É °ËÁõ¹Þ±â¸¦ ±â´Ù¸°´Ù.
								//----------------------------------------------------
								SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY, SubInventoryItemID);
							}
						}
						// 2005, 3, 2, sobeit modify end
					}						
					
					return false;
				break;

				//-------------------------------------------------------
				// ¼º¼ö ¸¸µé±â
				//-------------------------------------------------------
				case MAGIC_CREATE_HOLY_WATER :				
					if (
						IsSlayer()
						&& !IsWaitVerify()
						&& IsItemCheckBufferNULL())
					{					
						MItem* pItem = NULL;

						for( int i = 0; i <= 2; i++ )
						{
							pItem = g_pInventory->FindItem( ITEM_CLASS_WATER,  i);
							if( pItem != NULL )
								break;
						}

						if (pItem!=NULL)
						{
//							POINT fitPoint;			// ¼º¼ö°¡ µé¾î°¥ ÀÚ¸®

//							if (GetMakeItemFitPosition(pItem, 
//														ITEM_CLASS_HOLYWATER, 
//														pItem->GetItemType(), 
//														fitPoint))
							{
								CGSkillToInventory _CGSkillToInventory;
								_CGSkillToInventory.setObjectID( pItem->GetID() );
								_CGSkillToInventory.setX( pItem->GetGridX() );
								_CGSkillToInventory.setY( pItem->GetGridY() );
								_CGSkillToInventory.setTargetX( pItem->GetGridX() );
								_CGSkillToInventory.setTargetY( pItem->GetGridY() );
								_CGSkillToInventory.setSkillType( MAGIC_CREATE_HOLY_WATER );
								//_CGSkillToInventory.setCEffectID( 0 );	// -_-;;							

								g_pSocket->sendPacket( &_CGSkillToInventory );

								
								// ÀÏ´Ü(!) ±×³É ¾ø¾Ö°í º»´Ù.
								//(*g_pInventory).RemoveItem( pItem->GetID() );

								//----------------------------------------------------
								// Inventory¿¡¼­ itemÀ» »ç¿ëÇÏ´Â °É °ËÁõ¹Þ±â¸¦ ±â´Ù¸°´Ù.
								//----------------------------------------------------
								SetWaitVerify( MPlayer::WAIT_VERIFY_SKILL_SUCCESS );
								SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY );

								g_pSkillInfoTable->SetNextAvailableTime(MAGIC_CREATE_HOLY_WATER);

								//----------------------------------------------------
								// ±â¼ú »ç¿ë ½Ãµµ µ¿ÀÛ
								//----------------------------------------------------
								AddNewInventoryEffect( pItem->GetID(),
													MAGIC_CREATE_HOLY_WATER, //+ (*g_pActionInfoTable).GetMinResultActionInfo(),
													g_pClientConfig->FPS*3	// 3ÃÊ
												);
								// ¹Ýº¹µ¿ÀÛÀ» ¼³Á¤ÇÏÁö ¾Ê´Â´Ù.
								m_fNextTrace = FLAG_TRACE_NULL;
								UnSetRepeatAction();
							}
//							else
//							{
//								// g_pGameMessage( "ÀÚ¸®°¡ ¾øµû¾ß~" );
//							}
						}						
					}
					
					return false;					
				break;
			case SKILL_CREATE_HOLY_POTION :				
					if (
						IsSlayer()
						&& !IsWaitVerify()
						&& IsItemCheckBufferNULL())
					{					
						
						MItem* pItem = NULL;

						for( int i = 3; i <= 6; i++ )
						{
							pItem = g_pInventory->FindItem( ITEM_CLASS_WATER,  i);
							if( pItem != NULL )
								break;
						}

						if (pItem!=NULL)
						{
							POINT fitPoint;			// ¼º¼ö°¡ µé¾î°¥ ÀÚ¸®

							if (GetMakeItemFitPosition(pItem, 
														ITEM_CLASS_POTION, 
														(*g_pItemTable)[pItem->GetItemClass()][pItem->GetItemType()].Value3, 
														fitPoint))
							{
								CGSkillToInventory _CGSkillToInventory;
								_CGSkillToInventory.setObjectID( pItem->GetID() );
								_CGSkillToInventory.setX( pItem->GetGridX() );
								_CGSkillToInventory.setY( pItem->GetGridY() );
								_CGSkillToInventory.setTargetX( fitPoint.x );
								_CGSkillToInventory.setTargetY( fitPoint.y );
								_CGSkillToInventory.setSkillType( SKILL_CREATE_HOLY_POTION );
								//_CGSkillToInventory.setCEffectID( 0 );	// -_-;;							

								g_pSocket->sendPacket( &_CGSkillToInventory );

								
								// ÀÏ´Ü(!) ±×³É ¾ø¾Ö°í º»´Ù.
								//(*g_pInventory).RemoveItem( pItem->GetID() );

								//----------------------------------------------------
								// Inventory¿¡¼­ itemÀ» »ç¿ëÇÏ´Â °É °ËÁõ¹Þ±â¸¦ ±â´Ù¸°´Ù.
								//----------------------------------------------------
								SetWaitVerify( MPlayer::WAIT_VERIFY_SKILL_SUCCESS );
								SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY );

								g_pSkillInfoTable->SetNextAvailableTime(SKILL_CREATE_HOLY_POTION);

								//----------------------------------------------------
								// ±â¼ú »ç¿ë ½Ãµµ µ¿ÀÛ
								//----------------------------------------------------
								AddNewInventoryEffect( pItem->GetID(),
													SKILL_CREATE_HOLY_POTION, //+ (*g_pActionInfoTable).GetMinResultActionInfo(),
													g_pClientConfig->FPS*3	// 3ÃÊ
												);
								// ¹Ýº¹µ¿ÀÛÀ» ¼³Á¤ÇÏÁö ¾Ê´Â´Ù.
								m_fNextTrace = FLAG_TRACE_NULL;
								UnSetRepeatAction();
							}
							else
							{
								// g_pGameMessage( "ÀÚ¸®°¡ ¾øµû¾ß~" );
							}
						}						
					}
					
					return false;					
				break;

				//-------------------------------------------------------
				// ÆøÅº ¸¸µé±â
				// Áö·Ú ¸¸µé±â
				//-------------------------------------------------------
				case SKILL_MAKE_BOMB :
				case SKILL_MAKE_MINE :
					if (IsSlayer()
						&& !IsWaitVerify()
						&& IsItemCheckBufferNULL()
						&& !gC_vs_ui.IsInstallMineProgress())	// Áö·Ú ¸¸µå´Â ÁßÀÌ¸é..
						//|| gC_vs_ui.IsCreateMineProgress()	// Áö·Ú ¸¸µå´Â ÁßÀÌ¸é..
						//|| gC_vs_ui.IsCreateBombProgress()	// Áö·Ú ¸¸µå´Â ÁßÀÌ¸é..
					{					

						int					skillID;
						ITEM_CLASS	itemClass;
						int					minItemType;
						int					maxItemType;
						int					itemTypeModifier;
							
						// itemTypeÀÌ 0~4±îÁöÀÌ¸é ÆøÅºÀÌ´Ù.
						if (m_nSpecialActionInfo==SKILL_MAKE_BOMB)
						{
							skillID = SKILL_MAKE_BOMB;
							itemClass = ITEM_CLASS_BOMB;
							minItemType = 0;
							maxItemType = 4;
							itemTypeModifier = 0;
						}
						else
						{
							skillID = SKILL_MAKE_MINE;
							itemClass = ITEM_CLASS_MINE;
							minItemType = 5;
							maxItemType = 9;
							itemTypeModifier = -5;
						}

						MItemClassRangeTypeFinder itemFinder(ITEM_CLASS_BOMB_MATERIAL,
																minItemType, maxItemType);

						MItem* pItem = ((MItemManager*)g_pInventory)->FindItem( itemFinder );

						if (pItem!=NULL)
						{
							POINT fitPoint;			// µé¾î°¥ ÀÚ¸®

							if (GetMakeItemFitPosition(pItem, 
														itemClass, 
														pItem->GetItemType() + itemTypeModifier, 
														fitPoint))
							{
								// ¸¸µé±â ½ÃÀÛÇÏ´Â ¼ø°£...
								if (skillID==SKILL_MAKE_BOMB)
								{
									gC_vs_ui.StartCreateBombProgress( pItem->GetGridX(), pItem->GetGridY() );
								}
								else
								{
									gC_vs_ui.StartCreateMineProgress( pItem->GetGridX(), pItem->GetGridY() );
								}

								// UI¿¡¼­ ¸·´ë±â º¸¿©ÁÖ°í ³ª¼­
								// UI_ITEM_USE°¡ ³¯¾Æ¿À´Â ºÎºÐ¿¡¼­ Ã³¸®ÇÑ´Ù.
								
								// ¿¬¼ÓÀûÀ¸·Î »ç¿ëÇÏ±â À§ÇØ¼­
//								m_fNextTrace	= FLAG_TRACE_SELF;
//				
//								m_bRepeatAction = FALSE;
								
								// ¹Ýº¹µ¿ÀÛÀ» ¼³Á¤ÇÏÁö ¾Ê´Â´Ù.
								m_fNextTrace = FLAG_TRACE_NULL;
								UnSetRepeatAction();
								
								/*
								CGSkillToInventory _CGSkillToInventory;
								_CGSkillToInventory.setObjectID( pItem->GetID() );
								_CGSkillToInventory.setX( pItem->GetGridX() );
								_CGSkillToInventory.setY( pItem->GetGridY() );
								_CGSkillToInventory.setTargetX( fitPoint.x );
								_CGSkillToInventory.setTargetY( fitPoint.y );
								_CGSkillToInventory.setSkillType( skillID );
								//_CGSkillToInventory.setCEffectID( 0 );	// -_-;;							

								g_pSocket->sendPacket( &_CGSkillToInventory );

								
								// ÀÏ´Ü(!) ±×³É ¾ø¾Ö°í º»´Ù.
								//(*g_pInventory).RemoveItem( pItem->GetID() );

								//----------------------------------------------------
								// Inventory¿¡¼­ itemÀ» »ç¿ëÇÏ´Â °É °ËÁõ¹Þ±â¸¦ ±â´Ù¸°´Ù.
								//----------------------------------------------------
								SetWaitVerify( MPlayer::WAIT_VERIFY_SKILL_SUCCESS );
								SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY );

								(*g_pSkillInfoTable)[skillID].SetNextAvailableTime();

								//----------------------------------------------------
								// ±â¼ú »ç¿ë ½Ãµµ µ¿ÀÛ
								//----------------------------------------------------
								AddNewInventoryEffect( pItem->GetID(),
													skillID, //+ (*g_pActionInfoTable).GetMinResultActionInfo(),
													g_pClientConfig->FPS*3	// 3ÃÊ
												);
								*/
							}
							else
							{
								// g_pGameMessage( "ÀÚ¸®°¡ ¾øµû¾ß~" );
							}
						}						
					}
					
					return false;					
				break;

				//-------------------------------------------------------
				// ¹ÚÁã³ª ´Á´ë º¯½Å
				//-------------------------------------------------------
				case SKILL_TRANSFORM_TO_WERWOLF :
				case MAGIC_TRANSFORM_TO_WOLF :
				case MAGIC_TRANSFORM_TO_BAT :					
					//----------------------------------------------------
					//
					//					º¯½Å¿ë ¾ÆÀÌÅÛ - vampireÀÎ °æ¿ì
					//
					//----------------------------------------------------
					if(IsVampire() && GetAction() ==  ACTION_VAMPIRE_DRAIN) 
						return false ; 

					if (IsVampire() && g_pZoneTable->Get( m_pZone->GetID() )->CannotUseSpecialItem == false)
					{
							//----------------------------------------------------
							// °ËÁõ ¹ÞÀ»°Ô ¾ø´Â °æ¿ì..
							//----------------------------------------------------
							if (!IsWaitVerify()
								&& IsItemCheckBufferNULL())
							{		
								MItem* pItem = NULL;
								MItem* pSubInventory = NULL;

								if (m_nSpecialActionInfo==MAGIC_TRANSFORM_TO_WOLF)
								{
									// ´Á´ë ¹ßÅé Ã£±â
									pItem = g_pInventory->FindItem(ITEM_CLASS_VAMPIRE_ETC, 0);
								}
								else if ( m_nSpecialActionInfo == SKILL_TRANSFORM_TO_WERWOLF )
								{
									pItem = g_pInventory->FindItem(ITEM_CLASS_SKULL, 39);
								}
								else
								{
/*
#if __CONTENTS(__FAST_TRANSFORTER)
									pItem = g_pInventory->FindItemAll( MItemClassTypeFinder(ITEM_CLASS_VAMPIREWING_ITEM, 0), pSubInventory );
									// ¹ÚÁã ³¯°³ Ã£±â
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
									if(pItem == NULL)
										pItem = g_pInventory->FindItemAll( MItemClassTypeFinder(ITEM_CLASS_VAMPIREWING_ITEM, 1), pSubInventory );
#endif //__SECOND_TRANSFORTER
*/
#if __CONTENTS(__FAST_TRANSFORTER || __SECOND_TRANSFORTER)
									pItem = ((MItemManager*)g_pInventory)->FindItemOrderByIndex( MVampireWingItemFinder());
#endif
									if(pItem == NULL)
										pItem = g_pInventory->FindItemAll( MItemClassTypeFinder(ITEM_CLASS_VAMPIRE_ETC , 1), pSubInventory );
								}

								if (pItem!=NULL)
								{						
									DWORD dwSubInventoryID = 0;
									CGSkillToInventory _CGSkillToInventory;
									_CGSkillToInventory.setObjectID( pItem->GetID() );
									_CGSkillToInventory.setX( pItem->GetGridX() );
									_CGSkillToInventory.setY( pItem->GetGridY() );
									_CGSkillToInventory.setSkillType( m_nSpecialActionInfo );
									if(NULL != pSubInventory)
										dwSubInventoryID = pSubInventory->GetID();

									_CGSkillToInventory.setInventoryItemObjectID(dwSubInventoryID);

									//_CGSkillToInventory.setCEffectID( 0 );	// -_-;;							

									g_pSocket->sendPacket( &_CGSkillToInventory );


									// ÀÏ´Ü(!) ±×³É ¾ø¾Ö°í º»´Ù.
									//(*g_pInventory).RemoveItem( pItem->GetID() );

									//----------------------------------------------------
									// Inventory¿¡¼­ itemÀ» »ç¿ëÇÏ´Â °É °ËÁõ¹Þ±â¸¦ ±â´Ù¸°´Ù.
									//----------------------------------------------------
									SetWaitVerify( MPlayer::WAIT_VERIFY_SKILL_SUCCESS );
									SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY , dwSubInventoryID);

									g_pSkillInfoTable->SetNextAvailableTime(m_nSpecialActionInfo);

									//----------------------------------------------------
									// ±â¼ú »ç¿ë ½Ãµµ µ¿ÀÛ
									//----------------------------------------------------
									AddNewInventoryEffect( pItem->GetID(),
														m_nSpecialActionInfo, //+ (*g_pActionInfoTable).GetMinResultActionInfo(),
														g_pClientConfig->FPS*3	// 3ÃÊ
													);
								}
							}
					}						
					
					return false;
				break;

				//----------------------------------------------------
				//
				// º¯½Å Ç®±â
				// 
				//----------------------------------------------------
				case MAGIC_UN_TRANSFORM :
					if (IsVampire())
					{
						const MSector& sector = m_pZone->GetSector(m_X, m_Y);
						
						// 2005, 2, 5, sobeit modify start - IsWaitVerify Ãß°¡ 
						//if (sector.IsBlockGround())
						if (sector.IsBlockGround() || IsWaitVerify())
						// 2005, 2, 5, sobeit modify end
						{
							m_fNextTrace = FLAG_TRACE_NULL;

							UnSetRepeatAction();

							return false;
						}						
					}
				break;
				
				case SKILL_SUMMON_SYLPH :
					if( IsOusters() && !HasEffectStatus( EFFECTSTATUS_HAS_FLAG )
						//----------------------------------------------------
						// °ËÁõ ¹ÞÀ»°Ô ¾ø´Â °æ¿ì..
						//----------------------------------------------------
						&& IsWaitVerifyNULL()
						&& IsItemCheckBufferNULL()
						)
					{
						//MItem* pItem = g_pInventory->FindItem( ITEM_CLASS_OUSTERS_SUMMON_ITEM );
						
						// 2005, 3, 2, sobeit modify start - FindItem() ´ë½Å FindItemAll() »ç¿ë
						//MItem* pItem = ((MItemManager*)g_pInventory)->FindItem( MOustersSummonGemItemFinder() );
						
						MItem* pSubInventory = NULL;
/*						MItem* 
#if __CONTENTS(__FAST_TRANSFORTER)
							pItem = g_pInventory->FindItemAll( MItemClassTypeFinder(ITME_CLASS_OUSTERSWING_ITEM , 0), pSubInventory );
						if(pItem == NULL)
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
							pItem = g_pInventory->FindItemAll( MItemClassTypeFinder(ITME_CLASS_OUSTERSWING_ITEM , 1), pSubInventory );
						if(pItem == NULL)
#endif //__SECOND_TRANSFORTER
							pItem = ((MItemManager*)g_pInventory)->FindItemAll( MOustersSummonGemItemFinder(), pSubInventory );*/
						MItem*
#if __CONTENTS(__FAST_TRANSFORTER || __SECOND_TRANSFORTER)
							// by jin. 
							// 1. ½Å±Ô ÀÌµ¿¼ö´ÜÀÇ °æ¿ì, ¼­¸ó ¾ÆÀÌÅÛµé º¸´Ù ¿ì¼±ÇÑ´Ù. ¶ÇÇÑ, ½Å±Ô ÀÌµ¿¼ö´ÜÀÌ µÎ °¡Áö ÀÌ»ó Á¸ÀçÇÒ °æ¿ì, ¿ì¼±¼øÀ§°¡ ÇÊ¿äÇÏ´Ù.
							//	±âÁ¸ÀÇ FindItemAll ÇÔ¼ö´Â ½ÇÀç ÀÎº¥Åä¸® ³»ÀÇ ¹èÄ¡ ¼ø¼­°¡ ¾Æ´Ï¶ó ÃÖÃÊ ¼­¹ö¿¡¼­ ³»·Á¹ÞÀº ¼ø¼­¿¡ ÀÇÇØ ¿ì¼±¼øÀ§°¡ °áÁ¤µÈ´Ù.
							//	ÀÌ´Â, À¯ÀúµéÀÇ ºÒÆí»çÇ×ÀÌ µÉ ¼ö ÀÖÀ¸¹Ç·Î ÀÎº¥Åä¸® ³»ÀÇ ¹èÄ¡ ¼ø¼­¸¦ ±âÁØÀ¸·Î ¾Õ¿¡ ¹èÄ¡µÈ ¾ÆÀÌÅÛÀ» Ã£¾Æ ÁÖ´Â ÇÔ¼ö¸¦ »õ·Î ¸¸µé¾î »ç¿ëÇÑ´Ù.
							// 2. ½Å±Ô ÀÌµ¿¼ö´ÜÀº ¼­ºêÀÎº¥¿¡ ÀÖÀ» ¶§´Â »ç¿ëÀÌ ºÒ°¡´É ÇÏ´Ù. µû¶ó¼­, ¼­ºêÀÎº¥±îÁö Æ÷ÇÔÇÑ ¹èÄ¡ ¼ø¼­¿¡ ÀÇÇÑ ¾ÆÀÌÅÛ Ã£±â ÇÔ¼ö´Â »ç¿ëÇÏÁö ¸øÇÑ´Ù.
							//	ÈÄ¿¡, ¼­ºêÀÎº¥¿¡ ÀÖÀ» ´ëµµ »ç¿ëÀÌ °¡´ÉÇÏ°Ô ÇÒ ¶§´Â »ç¿ëÀÌ °¡´ÉÇÒ °ÍÀÌ¹Ç·Î ÁÖ¼® Ã³¸®ÇÏ¿© À¸·Î ³²°ÜµÐ´Ù. (FindItemAllOrderByIndex)
//							pItem = ((MItemManager*)g_pInventory)->FindItemAll( MOustersWingItemFinder(), pSubInventory );
//							pItem = ((MItemManager*)g_pInventory)->FindItemAllOrderByIndex( MOustersWingItemFinder(), pSubInventory);
							pItem = ((MItemManager*)g_pInventory)->FindItemOrderByIndex( MOustersWingItemFinder());
						if(pItem == NULL)
#endif
							pItem = ((MItemManager*)g_pInventory)->FindItemAll( MOustersSummonGemItemFinder(), pSubInventory );
						
						if( pItem != NULL )
						{
							if( (pItem->IsAffectStatus() && pItem->IsChargeItem() && pItem->GetNumber() > 0)
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
								|| (pItem->IsAffectStatus() && pItem->GetItemClass() == ITEM_CLASS_OUSTERSWING_ITEM )
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
								)
							{
								DWORD SubInventoryItemID = 0;

								CGUseItemFromInventory _CGUseItemFromInventory;
								_CGUseItemFromInventory.setObjectID( pItem->GetID() );
								_CGUseItemFromInventory.setX( pItem->GetGridX() );
								_CGUseItemFromInventory.setY( pItem->GetGridY() );
								
								if(NULL != pSubInventory)
								{
									SubInventoryItemID = pSubInventory->GetID();
								}
								_CGUseItemFromInventory.setInventoryItemObjectID( SubInventoryItemID );
								
								g_pSocket->sendPacket( &_CGUseItemFromInventory );
								
								SetItemCheckBuffer( pItem, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY, SubInventoryItemID);
								g_pSkillInfoTable->SetAvailableTime(SKILL_SUMMON_SYLPH, 4000);
							}
						} else
						{
							
							
						}
						// 2005, 3, 2, sobeit modify end
					}
					return false;
				break;
				//-----------------------------------------------------
				//
				// ÃßÀû
				//
				//-----------------------------------------------------
				case SKILL_SOUL_CHAIN :
					// ¹ìÆÄÀÌ¾î ½½·¹ÀÌ¾î ±¸ºÐÀÌ ¾ø´Ù.

					if (!IsWaitVerify())
					{
						// UI Ã¢À» ¶ç¿öÁØ´Ù.
						// UI Ã¢À» ¶ç¿î µÚ ¸Þ½ÃÁö°¡ ³¯¾Æ¿À¸é °Å±â¼­ sendpacket À» ÇØÁØ´Ù.
						
						gC_vs_ui.RunTraceWindow();
						g_pUIDialog->SetLockInputPCTalk();						
//						int skillID = SKILL_SOUL_CHAIN;
//						
//						if (skillID < MIN_RESULT_ACTIONINFO)
//						{
//							(*g_pSkillInfoTable)[skillID].SetAvailableTime();
//						}
						
					}
					return false;
					
					break;

				case SKILL_LOVE_CHAIN :
					//----------------------------------------------------
					// °ËÁõ ¹ÞÀ»°Ô ¾ø´Â °æ¿ì..
					//----------------------------------------------------
					if (!IsWaitVerify()	&& IsItemCheckBufferNULL() && CurPernalShop() != 1)
					{	
						ITEM_CLASS ItemClass;
						switch( GetRace() )
						{
						case RACE_SLAYER :
							ItemClass = ITEM_CLASS_COUPLE_RING;
							break;
						case RACE_VAMPIRE :
							ItemClass = ITEM_CLASS_VAMPIRE_COUPLE_RING;
							break;
						case RACE_OUSTERS :
							ItemClass = ITEM_CLASS_OUSTERS_HARMONIC_PENDENT; // ;
							break;
						default :
							ItemClass = ITEM_CLASS_COUPLE_RING;
							break;
						}
						MPlayerGear* pGear = GetGear();

//¿ö¾ÆÀÌÅÛÀ» °¡Áö°í ÀÖÀ¸¸é Ä¿ÇÃÃ¼ÀÎÀº »ç¿ëÇÒ¼ö ¾ø´Ù.
						MItem*	pWarItem = g_pInventory->FindItem(ITEM_CLASS_WAR_ITEM);
						if(pWarItem!=NULL)
							break;

						MItemClassFinder itemFinder( ItemClass );
						
						
						MItem *pItem = pGear->FindItem( itemFinder );
						if(pItem == NULL || !pItem->IsAffectStatus())
							return false;

						CGUseItemFromGear _CGUseItemFromGear;

						_CGUseItemFromGear.setObjectID ( pItem->GetID() );
						_CGUseItemFromGear.setPart ( pItem->GetItemSlot() );
						
						g_pSocket->sendPacket( &_CGUseItemFromGear );

						SetItemCheckBuffer( pItem, ITEM_CHECK_BUFFER_USE_FROM_GEAR );
						return false;
					}
					break;
			}			

			if( (*g_pActionInfoTable)[m_nSpecialActionInfo].GetParentActionInfo() != ACTIONINFO_NULL )
				m_nSpecialActionInfo = (*g_pActionInfoTable)[m_nSpecialActionInfo].GetParentActionInfo();

			// »ç¿ëÇÑ ±â¼ú·Î ¼³Á¤
			m_nUsedActionInfo	= m_nSpecialActionInfo;

			// ÃßÀû Á¤º¸ ÃÊ±âÈ­
			SetTraceID( m_ID );
			m_fTrace	= FLAG_TRACE_SELF;
			m_TraceX	= m_X;//SECTORPOSITION_NULL;
			m_TraceY	= m_Y;//SECTORPOSITION_NULL;
			m_TraceZ	= 0;

			// ÇöÀç ÃßÀûÇÏ´Â ´ë»ó¿¡ ´ëÇØ¼­ ±â¾ïÇØµÐ´Ù.
			m_fTraceBuffer	= m_fTrace;	

			//------------------------------------------------------------
			// BasicActionÀÌ ¹¹³Ä¿¡ µû¶ó¼­..
			// ÇöÀç PlayerÀÇ ´É·Â¿¡ µû¶ó¼­ TraceDistance°¡ ´Þ¶óÁú °ÍÀÌ´Ù.
			//------------------------------------------------------------
			
			m_TraceDistance		= 0;
			
			//------------------------------------------------------------
			// ±â¼ú »ç¿ë ActionÇ¥Çö
			SetNextAction( GetActionInfoAction(m_nUsedActionInfo, true) );

			// bufferingÀ» ¾ø¾Ø´Ù.
			m_fNextTrace			= FLAG_TRACE_NULL;	

			//------------------------------------------------------------
			// messageÃâ·Â
			//------------------------------------------------------------
			DEBUG_ADD_FORMAT("Self SpecialAction");					
		}
		else
		{
			//------------------------------------------------------------
			// ¹Ýº¹ actionÇÏ´Â ÁßÀÌ ¾Æ´Ñ °æ¿ì¿¡ ´ÙÀ½ µ¿ÀÛ buffering
			//------------------------------------------------------------
			if (m_bRepeatAction)
			{
				m_fNextTrace	= FLAG_TRACE_NULL;

				return false;
			}
			else
			{
				m_fNextTrace	= FLAG_TRACE_SELF;
				
				m_bRepeatAction = FALSE;

				return false;
			}
		}

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Keep Trace Creature
//----------------------------------------------------------------------
// ÃßÀûÇÏ´ø Creature¸¦ °è¼Ó ÃßÀûÇÑ´Ù.
//----------------------------------------------------------------------
void
MPlayer::KeepTraceCreature()
{
	if (IsInDarkness())
	{
		TraceNULL();			
		SetStop();			
		UnSetRepeatAction();
			
		return;			
	}

	// ÃßÀûÇÏ´Â CreatureÀÇ Á¤º¸¸¦ ÀÐ¾î¿Â´Ù.
	MCreature*	pCreature = m_pZone->GetCreature( m_TraceID );

	//GET_DYING_CREATURE( pCreature, m_TraceID );

	bool	bTraceTimer = false;

	if( m_TraceTimer != 0 )
	{
		if( ( GetTickCount() - m_TraceTimer ) / 1000 > g_pClientConfig->TRACE_CHARACTER_LIMIT_TIME )
		{
			if( pCreature != NULL &&					// µ¿Á·ÀÏ °æ¿ì¸¸.
				(pCreature->IsSlayer() && IsSlayer()) || 
				(pCreature->IsVampire() && IsVampire()) ||
				(pCreature->IsOusters() && IsOusters())
				)
			{
				bTraceTimer = true;
				m_TraceTimer = 0;
			}			
		}
	}
	
	//-------------------------------------------------------
	// ÃßÀûÇÏ´Â Creature°¡ »ç¶óÁ³À» °æ¿ì --> ÃßÀû ÁßÁö
	// ³»°¡ SlayerÀÎ °æ¿ì´Â Darkness¾È¿¡ µé¾î°£ Ä³¸¯À» ÂÑ¾Æ°¥ ¼ö ¾ø´Ù.
	//-------------------------------------------------------
	if ((pCreature==NULL || 
		pCreature->IsInDarkness() && !pCreature->IsNPC() && 
		(!IsVampire() && !HasEffectStatus( EFFECTSTATUS_LIGHTNESS) ||
		IsVampire() && g_pZone->GetID() == 3001)
		||bTraceTimer
		|| pCreature->IsInGroundElemental() && pCreature->IsOusters() && !g_pPlayer->IsOusters())
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_FLAME_SIGH)
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
	{
		// ÃßÀû ÁßÁö
		TraceNULL();
			
		// ¸ØÃá´Ù.
		SetStop();	
		
		UnSetRepeatAction();
		m_TraceTimer = 0;
			
		return;			
	}
	
	if(pCreature)
	{
		if (max(abs(pCreature->GetX()-m_X), 
			abs(pCreature->GetY()-m_Y)) > 1)
		{		
			TraceCreatureToBasicAction( m_TraceID, false );	
			
			g_pTopView->SetSelectedSectorNULL();
		}	
	}
}


//----------------------------------------------------------------------
// Get ActionInfo Range
//----------------------------------------------------------------------
// nActionInfoÀÇ »ç¿ë °¡´É °Å¸®´Â?
//----------------------------------------------------------------------
int		
MPlayer::GetActionInfoRange(TYPE_ACTIONINFO nActionInfo)
{
	if( nActionInfo == SKILL_HEAD_SHOT )
		return 3;
	
	if( (*g_pActionInfoTable)[nActionInfo].GetParentActionInfo() != ACTIONINFO_NULL )
		nActionInfo = (*g_pActionInfoTable)[nActionInfo].GetParentActionInfo();

	//-------------------------------------------------------------
	// ±âº» actionÀÇ Àû¿ëÀ» ¹Þ´Â°¡?
	//-------------------------------------------------------------
	if ((*g_pActionInfoTable)[nActionInfo].IsAffectCurrentWeaponRange())
	{
		return m_BasicAttackDistance;
		//nActionInfo = m_nBasicActionInfo;
	}
	
	SKILLINFO_NODE& skillInfo = (*g_pSkillInfoTable)[nActionInfo];	

	int minRange = skillInfo.minRange;
	int maxRange = skillInfo.maxRange;
	int skillLevel = skillInfo.GetExpLevel();
	
#ifdef __METROTECH_TEST__
	if(g_bLight)
		return maxRange;
#endif

	int maxLevel = 100;	// ½½·¹ÀÌ¾î´Â ½ºÅ³·¹º§À» 100±îÁö ¿Ã¸°´Ù.
	
	if(IsOusters())
		maxLevel = 30;	// ¾Æ¿ì½ºÅÍÁî´Â ½ºÅ³·¹º§À» 30±îÁö ¿Ã¸°´Ù.

	// Æ¯Á¤ ½ºÅ³¿¡ ´ëÇÑ Range °è»ê
	
	switch( nActionInfo )
	{
	case MAGIC_RAPID_GLIDING :
		return ( min(6, (2+ (GetDEX() / 50)) ) );
		break;
	
	case MAGIC_BLOODY_ZENITH :
		return min(6, 2 + GetSTR()/80 + GetDEX()/120 + GetINT()/240 );
		break;	
	
	case SKILL_SOUL_REBIRTH:
		return min(5, 2+(*g_pSkillInfoTable)[SKILL_SOUL_REBIRTH_MASTERY].GetExpLevel()/10);
		break;
	}
	
	return (int)(minRange + (maxRange - minRange) * skillLevel/maxLevel);

}

//----------------------------------------------------------------------
// Trace Creature To BasicAction
//----------------------------------------------------------------------
// ±âº»°ø°ÝÀ¸·Î(L-Click) ÇÑ ¸íÀÇ Creature¸¦ ÃßÀûÇÏ¿© °ø°ÝÇÑ´Ù.
// ¸ØÃçÀÖ´Â »óÅÂ°¡ ¾Æ´Ï¸é.. Next¿¡ ¼³Á¤ÇØµÐ´Ù.
//----------------------------------------------------------------------
bool
MPlayer::TraceCreatureToBasicAction(TYPE_OBJECTID id, bool bForceAttack, bool bClick)
{
	DEBUG_ADD_FORMAT("[NPC-TCB] ENTER id=%d bForceAttack=%d bClick=%d playerCT=%d",
		(int)id, (int)bForceAttack, (int)bClick, (int)m_CreatureType);
//	if(bForceAttack && !g_pDXInput->KeyDown(DIK_LSHIFT))
//	{
//		bForceAttack = false;
//	}

	if((HasEffectStatus(EFFECTSTATUS_SLEEP) || HasEffectStatus( EFFECTSTATUS_BLOCK_HEAD ) )
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
		return false;

	//-----------------------------------------------------
	// delay°¡ ÀÖÀ¸¸é ¾ÈµÈ´Ù.
	//-----------------------------------------------------
	if (!IsNotDelay() 
		|| IsInCasket()	// [»õ±â¼ú3]
		|| HasEffectStatus( EFFECTSTATUS_ETERNITY_PAUSE )
		|| IsInDarkness() && 
		!HasEffectStatus( EFFECTSTATUS_LIGHTNESS )
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_FLAME_SIGH)
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		 )
	{
		return false;
	}

	if (m_nBasicActionInfo == ACTIONINFO_NULL)
		return false;

	if (m_nUsedActionInfo!=ACTIONINFO_NULL)
		return false;

	if (IsStop()) // || m_Action!=ACTION_ATTACK)
	{		
		// Zone¿¡ Á¸ÀçÇÏ´Â CreatureÀÎÁö checkÇÑ´Ù.
		MCreature*	pCreature = m_pZone->GetCreature(id);
		if(pCreature == NULL)
		{
			MFakeCreature *pFakeCreature = (MFakeCreature *)m_pZone->GetFakeCreature(id);
			if(pFakeCreature != NULL && pFakeCreature->GetOwnerID() != OBJECTID_NULL)	// ÆêÀÌ´ç
			{
				MPetItem *pPetItem = pFakeCreature->GetPetItem();
				if(pPetItem != NULL)
				{
					PETINFO petInfo;
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
				
						for(; itr!=optionList.end() && *itr < g_pItemOptionTable->GetSize(); itr++)
						{
							petInfo.OPTION_LIST.push_back( *itr );
						}
					}
#endif //__NEW_PET_INCUBUS

					petInfo.ITEM_TYPE = pPetItem->GetItemType();
					petInfo.NAME = pPetItem->GetPetName();
					petInfo.ITEM_ID = pPetItem->GetID();
					petInfo.ATTACK = pPetItem->IsCanAttack();
					petInfo.NICK_NAME = pFakeCreature->GetNickName();
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
					UI_RunPetInfo(&petInfo);
				}
				return false;
			}
		}

		// 2004,5, 28 sobeit add start - ´Ù¸¥ Ä³¸¯ÅÍ Á¤º¸
		if(g_pTopView->GetRequestMode() == MRequestMode::REQUEST_INFO)
		{
			SetTraceID( id );
			SetRequestMode( g_pTopView->GetRequestMode() );
			BasicActionToCreature();				
			SetTraceID( NULL );
			return false;	
		}	
		// 2004,5, 28 sobeit add end
		//GET_DYING_CREATURE( pCreature, id );

		// creature°¡ ¾ø´Â °æ¿ì
		// creature°¡ ÀÌ¹Ì Á×Àº °æ¿ì
		// ³»°¡ SlayerÀÎ °æ¿ì´Â Darkness¾È¿¡ µé¾î°£ Ä³¸¯À» ÂÑ¾Æ°¥ ¼ö ¾ø´Ù.	
		
		if( pCreature != NULL )
		{
			if( pCreature->GetObjectType() == MObject::TYPE_CREATURE )
			{
				if( IsSlayer() && g_pZone->GetSector( GetX(), GetY() ).IsSafeSlayer() && 
					pCreature->IsSlayer() && !g_pZone->GetSector( pCreature->GetX(), pCreature->GetY() ).IsSafeSlayer() && pCreature->IsPlayerOnly() ||
					IsVampire() && g_pZone->GetSector( GetX(), GetY() ).IsSafeVampire() && 
					pCreature->IsVampire() && !g_pZone->GetSector( pCreature->GetX(), pCreature->GetY() ).IsSafeVampire() && pCreature->IsPlayerOnly() ||
					IsOusters() && g_pZone->GetSector( GetX(), GetY() ).IsSafeOusters() && 
					pCreature->IsOusters() && !g_pZone->GetSector( pCreature->GetX(), pCreature->GetY() ).IsSafeOusters() && pCreature->IsPlayerOnly()
					)
				{
					//						UnSetLockMode();
					UnSetRepeatAction();
					//						g_pTopView->SetSelectedNULL();
					return false;
				}
			}
		}
		
//		bool bOusters = IsOusters();
//		MItem *pOustersItem = g_pOustersGear->GetItem(MOustersGear::GEAR_OUSTERS_RIGHTHAND);
		if (pCreature==NULL
			|| pCreature->IsDead()
			|| pCreature->IsInDarkness() && !pCreature->IsNPC() && 
			( !IsVampire() && !HasEffectStatus( EFFECTSTATUS_LIGHTNESS ) ||
			IsVampire() &&g_pZone->GetID() == 3001)
			|| pCreature->IsInGroundElemental() && pCreature->IsOusters() && !g_pPlayer->IsOusters()
			&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_FLAME_SIGH)
			&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
			&& !g_bLight
#endif
			)
		{
			UnSetRepeatAction();
			
			m_fTraceBuffer	= FLAG_TRACE_NULL;	
			m_fNextTrace	= FLAG_TRACE_NULL;
			return false;
		}


		bool	bTraceCreatureToForceAttack;
		bool	bKeepTraceCreature;

		bool bOusters = IsOusters();
		MItem *pOustersItem = g_pOustersGear->GetItem(MOustersGear::GEAR_OUSTERS_RIGHTHAND);

		//------------------------------------------------
		// RequestÇÒ·Á°í ÂÑ¾Æ°¡´Â °æ¿ì
		//------------------------------------------------
		// FAZ 4 (2026-05-05): SafeSpriteType0 + g_pCreatureSpriteTable bound guard
		int _reqSprT0 = SafeSpriteType0(m_CreatureType);
		bool _reqIsPlayerOnly = (_reqSprT0 > 0 && g_pCreatureSpriteTable != NULL &&
			_reqSprT0 < (int)g_pCreatureSpriteTable->GetSize() &&
			(*g_pCreatureSpriteTable)[_reqSprT0].IsPlayerOnlySprite());
		if (g_pTopView->IsRequestMode()
			// Player¸¸ µÈ´Ù.
			//&& pCreature->GetCreatureType()<=CREATURETYPE_VAMPIRE_FEMALE
			&& _reqIsPlayerOnly
			// °ü ¼Ó¿¡ ÀÖ´Â °æ¿ì´Â ¾ÈµÈ´Ù.
			&& !pCreature->IsInCasket()
			// ¼û¾î ÀÖ´Â °æ¿ì´Â ¾ÈµÈ´Ù.
			&& !pCreature->IsUndergroundCreature()
			&& pCreature->GetCreatureType() != CREATURETYPE_SLAYER_OPERATOR
			&& pCreature->GetCreatureType() != CREATURETYPE_VAMPIRE_OPERATOR
			&& pCreature->GetCreatureType() != CREATURETYPE_OUSTERS_OPERATOR

			// ³»°¡ ¹ÚÁã³ª ´Á´ë°¡ ¾Æ´Ñ °æ¿ì
			//&& m_CreatureType!=CREATURETYPE_BAT
			//&& m_CreatureType!=CREATURETYPE_WOLF
			)
		{
			bTraceCreatureToForceAttack = false;
			bKeepTraceCreature			= false;
			m_bNextForceAttack			= false;
			
			SetRequestMode( g_pTopView->GetRequestMode() );
		}
		//------------------------------------------------
		// ¿ÀÅä¹ÙÀÌ¸¦ Å¸°í ÀÖÀ¸¸é °ø°ÝÀÌ ¾ÈµÈ´Ù.
		// ¹ÚÁã , °í½ºÆ® ÀÎ °æ¿ì °ø°ÝÀÌ ¾ÈµÈ´Ù.
		//------------------------------------------------
		else if (IsRide_Air_Transporter()
				|| m_MoveDevice == MOVE_DEVICE_SUMMON_SYLPH || HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) )
				|| m_CreatureType==CREATURETYPE_BAT
				|| m_CreatureType==CREATURETYPE_VAMPIRE_GHOST
#if __CONTENTS(__FAST_TRANSFORTER)
				|| (m_MoveDevice == MOVE_DEVICE_WING_SYLPH && HasEffectStatusWingSylph(dynamic_cast<MCreature*>(this)))
				|| m_CreatureType == CREATURETYPE_FLITTERMOUSE
				|| m_CreatureType == CREATURETYPE_NEDE
				|| m_CreatureType == CREATURETYPE_KLTL
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
				|| (m_CreatureType == MOVE_DEVICE_WING_SYLPH && HasEffectStatusUnicorn(dynamic_cast<MCreature*>(this)))
				|| (m_CreatureType == MOVE_DEVICE_WING_SYLPH && HasEffectStatusOusterNewWings1(dynamic_cast<MCreature*>(this)))
				|| (m_CreatureType == MOVE_DEVICE_WING_SYLPH && HasEffectStatusOusterNewWings2(dynamic_cast<MCreature*>(this)))
				|| m_CreatureType == CREATURETYPE_SHAPE_OF_DEMON
#endif //__SECOND_TRANSFORTER
				|| !(bOusters && pOustersItem!=NULL && pOustersItem->GetItemClass() == ITEM_CLASS_OUSTERS_CHAKRAM || !bOusters)
				|| CanActionByZoneInfo()
				)
		{
			bTraceCreatureToForceAttack = false;
			bKeepTraceCreature			= true;	
			
			UnSetRequestMode();
		}
		//------------------------------------------------
		// °­Á¦ °ø°ÝÀÎ°¡?
		//------------------------------------------------		
		else if (bForceAttack)
		{
//			if(! (IsSlayer() && g_pZone->GetSector( GetX(), GetY() ).IsSafeSlayer() && pCreature->IsSlayer() && pCreature->IsPlayerOnly()|| 
//				IsVampire() && g_pZone->GetSector( GetX(), GetY() ).IsSafeVampire() && pCreature->IsVampire() && pCreature->IsPlayerOnly()) )
				bTraceCreatureToForceAttack = true;

			bKeepTraceCreature			= false;			

			UnSetRequestMode();
		}
		//------------------------------------------------
		// ¾Æ´Ï¸é.. Á¾Á·¿¡ µû¶ó¼­..
		//------------------------------------------------
		else
		{
			bTraceCreatureToForceAttack = CanAttackTribe( pCreature )
										// ¹ìÆÄÀÌ¾îÀÎ °æ¿ì¿¡´Â Guild¿¡ µû¶ó¼­ °ø°ÝÇÒ ¼ö ÀÖ°Å³ª..
										|| IsVampire() && CanAttackGuild( pCreature )
										|| g_pObjectSelector->IsWarEnemy( pCreature );
									//	|| m_pZone->IsFreePKZone();			// -_- ¹Ù³ªÅø


			// Á¤´ç¹æÀ§°¡ µÇ´Â°¡?
			bTraceCreatureToForceAttack |= g_pJusticeAttackManager->HasCreature( pCreature->GetName() );

			// °è¼Ó ÂÑ¾Æ°¡´Â°¡? - °ø°ÝÀÌ ¾Æ´Ï¶ó¸é..
			bKeepTraceCreature			= !bTraceCreatureToForceAttack;	
		}

		// 2005, 1, 8, sobeit add start - ½ÂÁ÷ ½½·¡ÀÌ¾î Áß ÈúÁ÷ ÀÎÃ¦Àº ÀÏ¹Ý °ø°ÝÀ» ÇÒ ¼ö ¾ø´Ù. µ¿ÀÛÀÌ ¾ø´Ù..-_-
		// 2006.10.18 sjheon  ¿©±â¿¡ ÁÖ¼®Ã³¸® 
		
		if(IsAdvancementClass())
		{
			const MCreatureWear::ADDON_INFO& TempaddonInfo = GetAddonInfo(ADDON_RIGHTHAND);
			MCreature*	pCreature = m_pZone->GetCreature(id);
			if(pCreature != NULL)
			{
				if(!pCreature->IsNPC() && IsSlayer() && (!TempaddonInfo.bAddon || HasEffectStatus(EFFECTSTATUS_INSTALL_MK1) || TempaddonInfo.ItemClass == ITEM_CLASS_CROSS || TempaddonInfo.ItemClass == ITEM_CLASS_MACE)
					&& bTraceCreatureToForceAttack )
				{
					bTraceCreatureToForceAttack = false;//½ûÖ¹×ó¼ü¹¥»÷  by rallser
					bKeepTraceCreature			= true;	//ÔÊÐí×ó¼üµã»÷¹ÖÎï ¿¿½üÄ¿±ê by rallser
			
					UnSetRequestMode();
					//return false;
				}
			}
		}
		// 2005, 1, 8, sobeit add end
		//------------------------------------------------
		// ÃÑ °ø°ÝÀÏ °æ¿ì 
		// --> ÅºÃ¢ÀÌ ¼³Á¤µÇ¾î ÀÖ´Â °æ¿ì¸¦ Ã¼Å©ÇÏ¸é µÈ´Ù.
		//------------------------------------------------
		if (bKeepTraceCreature)
		{
			// ÃßÀûÇÏ´Â °æ¿ì Ã¼Å©ÇÒ °Í...
		}
		// NPC°¡ ¾Æ´Ï¶ó¸é °ø°ÝÇÒ·Á´Â°Çµ¥... ÃÑ¾Ë Ã¼Å©ÇØ¾ß ÇÑ´Ù.
		else if (!pCreature->IsNPC())
		{
			if(!IsRequestMode() && IsInSafeSector() == 2)
				return false;
			
			// g_pCurrentMagazineÀ» È®½ÇÈ÷ NULL·Î ¼³Á¤ÇÏ¸é µÈ´Ù.
			//if (IsSlayer())
			{
			//	const MItem* pWeapon = g_pSlayerGear->GetItem( MSlayerGear::GEAR_SLAYER_RIGHTHAND );
				
				//------------------------------------------------
				// ÃÑ¾Ë Ã¼Å©
				//------------------------------------------------
			//	if (pWeapon!=NULL					// ¹«±â°¡ ÀÖ°í
			//		&& pWeapon->IsGunItem()			// ÃÑÀÌ°í
				//	&&	

				if (
					(IsOusters() && 
					g_pOustersGear->GetItem( MOustersGear::GEAR_OUSTERS_RIGHTHAND ) != NULL &&
					g_pOustersGear->GetItem( MOustersGear::GEAR_OUSTERS_RIGHTHAND )->GetItemClass() == ITEM_CLASS_OUSTERS_CHAKRAM || 
					!IsOusters() ) 
					&& 	(*g_pActionInfoTable)[m_nBasicActionInfo].IsWeaponTypeGunAny() 
					)
				{
					if (!IsRequestMode() 
						
						&& g_pCurrentMagazine!=NULL)		// ÃÑ¾ËÀÌ ÇÊ¿ä ¾ø´Â °æ¿ì
					{
						if (g_pCurrentMagazine->GetNumber()==0)	// ÃÑ¾ËÀÌ ¾ø´Â °æ¿ì
						{
							// ÃÑÀ» »ç¿ëÇÏ´Âµ¥ ÅºÃ¢ÀÌ ¾ø´Â °æ¿ì
							PlaySound( SOUND_ITEM_NO_MAGAZINE );

							m_fNextTrace	= FLAG_TRACE_NULL;

							// ÅºÃ¢ ¾øÀ»¶§ delay
							m_DelayTime	= g_CurrentTime + GetActionInfoDelay(m_nBasicActionInfo);

							// [µµ¿ò¸»] ÃÑ¾Ë ´Ù ½èÀ» ¶§
//							__BEGIN_HELP_EVENT
//								ExecuteHelpEvent( HE_ITEM_EMPTY_BULLET );
//							__END_HELP_EVENT

							m_fNextTrace = FLAG_TRACE_NULL;

							UnSetRepeatAction();

						
							return false;
						}
					}
				}
			}
			
			// »ç¿ëÇÑ ±â¼ú·Î ¼³Á¤
			m_nNextUsedActionInfo	= GetBasicActionInfo(); //m_nBasicActionInfo;
		}
		
		// ÃßÀû Á¤º¸ ¼³Á¤
		SetTraceID( id );

		if( !bForceAttack && bClick )
		{
			m_TraceTimer = GetTickCount();
		}
		
		m_fTrace	= FLAG_TRACE_CREATURE_BASIC;
		m_TraceX	= pCreature->GetX();
		m_TraceY	= pCreature->GetY();
		m_TraceZ	= pCreature->GetZ();
		
		//------------------------------------------------------------
		// °­Á¦ °ø°ÝÀÎ°¡?
		// °è¼Ó ÃßÀûÇÏ´Â°¡?
		//------------------------------------------------------------
		m_bTraceCreatureToForceAttack	= bTraceCreatureToForceAttack;
		m_bKeepTraceCreature			= bKeepTraceCreature;

		//------------------------------------------------------------
		// ÃßÀû ¸ñÇ¥ °Å¸®..
		//------------------------------------------------------------
		// BasicActionÀÌ ¹¹³Ä¿¡ µû¶ó¼­..
		// ÇöÀç PlayerÀÇ ´É·Â¿¡ µû¶ó¼­ TraceDistance°¡ ´Þ¶óÁú °ÍÀÌ´Ù.
		//
		// µû¶ó°¡´Â °ÍÀÌ¸é 1ÀÌ°í
		// ¾Æ´Ï¸é ±âº» °ø°Ý¿¡ µû¶ó¼­..
		//
		// NPCÀÎ °æ¿ì´Â °Å¸®°¡ ´Þ¶óÁú ¼ö ÀÖ´Ù.
		//------------------------------------------------------------
		if (m_bKeepTraceCreature)
		{
			m_TraceDistance = 1;
		}
		else
		{
			m_TraceDistance		= m_BasicAttackDistance;	
		}

		// NPC 
		if (pCreature->IsNPC())
		{
			if(pCreature->GetCreatureType() == 659)	// ´ëÁöÁ¤·ÉÀÇ »ÔÀº ÇÑÄ­
				m_TraceDistance		= 1;	
			else
				m_TraceDistance		= TRADE_DISTANCE_NPC;	
		}
		// TradeÇÒ·Á°í ÂÑ¾Æ°¡´Â °æ¿ì
		else if (IsRequestMode())
		{
			m_TraceDistance		= TRADE_DISTANCE_PC;	
		}

		// ÇöÀç ÃßÀûÇÏ´Â ´ë»ó¿¡ ´ëÇØ¼­ ±â¾ïÇØµÐ´Ù.
		m_fTraceBuffer	= m_fTrace;	
		m_TraceIDBuffer = id;

		//------------------------------------------------------------

		if( HasEffectStatus( EFFECTSTATUS_BLOCK_HEAD ) )
			m_bTraceCreatureToForceAttack = false;


		// ÃßÀûÇÏ´Â ±æ ¼³Á¤
		SetAction( m_MoveAction );
		SetNextDestination(m_TraceX, m_TraceY);

		// bufferingÀ» ¾ø¾Ø´Ù.
		m_fNextTrace			= FLAG_TRACE_NULL;	

		//------------------------------------------------------------
		// messageÃâ·Â
		//------------------------------------------------------------
		DEBUG_ADD_FORMAT("Trace Creature To BasicAction : ID=%d, (%d, %d)", id, m_TraceX, m_TraceY);				

		//------------------------------------------------------------
		// return falseÇÏ¸é ¹Ýº¹ actionÀ» ¾ÈÇÏ°Ô µÈ´Ù.
		//------------------------------------------------------------
		if (m_bKeepTraceCreature)// || m_bTraceCreatureToForceAttack)
		{
			return false;
		}

		// ¼±ÅÃµÈ sector ¾ø¾Ö±â
		g_pTopView->SetSelectedSectorNULL();
			

		return true;
	}
	else
	{
		//------------------------------------------------------------
		// ¹Ýº¹ actionÇÏ´Â ÁßÀÌ ¾Æ´Ñ °æ¿ì¿¡ ´ÙÀ½ µ¿ÀÛ buffering
		//------------------------------------------------------------
		if (m_bRepeatAction)
		{
			m_fNextTrace	= FLAG_TRACE_NULL;
			
			return false;
		}
		else
		{
			m_fNextTrace	= FLAG_TRACE_CREATURE_BASIC;
			m_NextTraceID	= id;
			m_bNextForceAttack = bForceAttack;

			m_TraceIDBuffer = id;

			m_bRepeatAction = FALSE;

			m_fTraceBuffer = FLAG_TRACE_NULL;

			return true;
		}
	}

	return true;
}


//----------------------------------------------------------------------
// Trace Creature To SpecialAction
//----------------------------------------------------------------------
// Æ¯¼ö°ø°ÝÀ¸·Î(R-Click) ÇÑ ¸íÀÇ Creature¸¦ ÃßÀûÇÏ¿© °ø°ÝÇÑ´Ù.
// ¸ØÃçÀÖ´Â »óÅÂ°¡ ¾Æ´Ï¸é.. Next¿¡ ¼³Á¤ÇØµÐ´Ù.
//----------------------------------------------------------------------
// [return°ª]
//
//  ´Ù¸¥ Creature¿¡°Ô »ç¿ëÇÏÁö ¸øÇÏ´Â °æ¿ì¸¸ false¸¦ returnÇÑ´Ù. 
//
//----------------------------------------------------------------------
bool	
MPlayer::TraceCreatureToSpecialAction(TYPE_OBJECTID id, bool bForceAttack)
{
	SetActionDelay( 0 );

	if((HasEffectStatus(EFFECTSTATUS_SLEEP) || HasEffectStatus( EFFECTSTATUS_BLOCK_HEAD ) )
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
		return false;

//	if(bForceAttack && !g_pDXInput->KeyDown(DIK_LSHIFT))
//	{
//		bForceAttack = false;
//	}
		
	//-----------------------------------------------------
	// delay°¡ ÀÖÀ¸¸é ¾ÈµÈ´Ù.
	//-----------------------------------------------------
	if (!IsNotDelay() 
		|| HasEffectStatus( EFFECTSTATUS_ETERNITY_PAUSE )
		|| IsInCasket()	// [»õ±â¼ú3]
		|| IsInDarkness() && (!HasEffectStatus( EFFECTSTATUS_LIGHTNESS ) || g_pZone->GetID() == 3001)
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_FLAME_SIGH)
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
	{
		return false;
	}

	if (m_nSpecialActionInfo == ACTIONINFO_NULL
		// Hava araci (Hover/Heli) bindiginde ozel saldiri kullanilamaz.
		|| IsRide_Air_Transporter()
		// ¹º°¡ ±â¼ú »ç¿ëÀ» °ËÁ¤ ¹Þ¾Æ¾ß ÇÏ¸é »ç¿ëÇÒ ¼ö ¾ø´Ù.
		//|| m_WaitVerify != WAIT_VERIFY_NULL)	// 2001.8.20 ÁÖ¼®Ã³¸®
		|| IsInSafeSector() == 2
		// ¿ÏÀü ¾ÈÀüÁö´ë¿¡¼­´Â ±â¼úÀ» »ç¿ëÇÒ ¼ö ¾ø´Ù.
		|| CanActionByZoneInfo()
		|| HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) )				// ´Þ¶ó´ó±â¸é ¸øÇÑ´ç~ 
#if __CONTENTS(__FAST_TRANSFORTER)
		|| HasEffectStatusWingSylph( dynamic_cast<MCreature*>(this) )
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
		|| HasEffectStatusUnicorn( dynamic_cast<MCreature*>(this) )
		|| HasEffectStatusOusterNewWings1( dynamic_cast<MCreature*>(this) )
		|| HasEffectStatusOusterNewWings2( dynamic_cast<MCreature*>(this) )
#endif //__SECOND_TRANSFORTER
		)
	{
		m_fNextTrace = FLAG_TRACE_NULL;

		UnSetRepeatAction();

		return false;
	}

		
	// ´Ù¸¥ ±â¼úÀ» »ç¿ëÁßÀÎ °æ¿ì
	if (m_nUsedActionInfo!=ACTIONINFO_NULL)
		return false;

	// 2006.08.22 sjheon
	if(HasEffectStatus(EFFECTSTATUS_TRANSFORM_TO_WOLF))
	{
		if( !(m_nSpecialActionInfo == MAGIC_HOWL || m_nSpecialActionInfo == MAGIC_EAT_CORPSE || m_nSpecialActionInfo == MAGIC_UN_TRANSFORM))
			return false ;
	}
	else if(HasEffectStatus(EFFECTSTATUS_TRANSFORM_TO_BAT))
	{
		if( !(m_nSpecialActionInfo == MAGIC_UN_TRANSFORM))
			return false ;
	}	
	
	//-------------------------------------------------------
	// Å¸ÀÎ¿¡°Ô »ç¿ëÇÒ ¼ö ÀÖ´Â °Íµé¸¸ »ç¿ëÇÑ´Ù.
	//-------------------------------------------------------
	
	if ((*g_pActionInfoTable)[m_nSpecialActionInfo].IsTargetOther())
	{
		int useSkill = m_nSpecialActionInfo;
		int originalSkill = m_nSpecialActionInfo;

//		// 2004, 6, 9 sobeit add start - Å×½ºÆ®, ÈíÇ÷ ¼º°ø ¸Þ¼¼Áö ¿Â´ã¿¡ µ¿ÀÛÇÏ°Ô..-_-
//		if(m_nSpecialActionInfo == SKILL_BLOOD_DRAIN)
//		{
//			m_fNextTrace = FLAG_TRACE_NULL;
//			//UnSetRepeatAction();
//			if(g_pSkillAvailable->IsEnableSkill( (ACTIONINFO)m_nSpecialActionInfo ) && m_TraceID != -1)
//			{
//				CGBloodDrain _CGBloodDrain;
//				_CGBloodDrain.setObjectID( m_TraceID );
//				g_pSocket->sendPacket( &_CGBloodDrain );
//				DEBUG_ADD("ÈíÇ÷ ¸Þ¼¼Áö º¸³¿");
//			}
//			return false;
//		}
//		// 2004, 6, 9 sobeit add end - Å×½ºÆ®, ÈíÇ÷ ¼º°ø ¸Þ¼¼Áö ¿Â´ã¿¡ µ¿ÀÛÇÏ°Ô..-_-
	
		if( m_nSpecialActionInfo == SKILL_BLITZ_SLIDING_ATTACK ||
			m_nSpecialActionInfo == SKILL_BLAZE_WALK_ATTACK
			)
		{
			m_fNextTrace = FLAG_TRACE_NULL;
			UnSetRepeatAction();

			return false;
		}

		int grade = 0;
		
		if( IsOusters() )
		{
			grade = (*g_pSkillInfoTable)[originalSkill].GetExpLevel() / 15;
			DEBUG_ADD_FORMAT("OUSTERS LEVEL : %d %d",originalSkill,(*g_pSkillInfoTable)[originalSkill].GetExpLevel());
		}


		if( grade>0 && (*g_pActionInfoTable)[originalSkill].IsUseActionStep())
			useSkill = (*g_pActionInfoTable)[originalSkill].GetActionStep( grade - 1 );
		
		// Sjheon 2005.07.01 Combo ±â¼ú Ã³¸® Add
		/*if((*g_pActionInfoTable)[originalSkill].GetComboAttackSkill())  
		{
			m_ComboCnt = GetComboEndValue();
			//OutputDebugString("TraceCreatureToSpecialAction()\n") ; 
		}
		else
		{
			//m_ComboCnt = 0 ;
			if(originalSkill != SKILL_HALO  && originalSkill != SKILL_SHARP_HAIL && originalSkill != SKILL_DRAGON_TORNADO &&
               originalSkill != SKILL_HIT_CONVERT  && originalSkill != SKILL_SWORD_OF_THOR && originalSkill != SKILL_BLITZ_SLIDING &&
			   originalSkill != SKILL_MENTAL_SWORD  && originalSkill != SKILL_BLAZE_WALK && originalSkill != SKILL_BLITZ_SLIDING &&
			   originalSkill != SKILL_SHADOW_DANCING  && originalSkill != SKILL_POWER_OF_LAND && originalSkill != SKILL_TYPHOON
			)
				m_ComboCnt = 0 ;


			if( grade>0 && (*g_pActionInfoTable)[originalSkill].IsUseActionStep())
				useSkill = (*g_pActionInfoTable)[originalSkill].GetActionStep( grade - 1 );
		}*/
		// Sjheon 2005.07.01 Combo ±â¼ú Ã³¸® End


		// 2004, 9, 20, sobeit add start
		if((*g_pActionInfoTable)[useSkill].IsMasterySkillStep()) 
		{
			int TempSkill = (*g_pActionInfoTable)[useSkill].GetActionStep( (*g_pActionInfoTable)[useSkill].GetMasterySkillStep() );
			if(g_pSkillAvailable->IsEnableSkill( (ACTIONINFO)TempSkill ))
				useSkill = TempSkill;
		}
		// 2004, 9, 20, sobeit add end
		if( (*g_pActionInfoTable)[useSkill].GetParentActionInfo() != ACTIONINFO_NULL )
			originalSkill = (*g_pActionInfoTable)[useSkill].GetParentActionInfo();
		
		DEBUG_ADD_FORMAT("SKILL : %d %d", originalSkill, useSkill );
		//-------------------------------------------------------
		// ÇöÀç »ç¿ëÇÒ ¼ö ÀÖ´Â ±â¼úÀÎÁö Ã¼Å©..
		// Passive ½ºÅ³ÀÌ¸é »ç¿ë ¸øÇÏ°Ô..
		//-------------------------------------------------------
		if (!g_pSkillAvailable->IsEnableSkill( (ACTIONINFO)originalSkill )
			|| !(*g_pSkillInfoTable)[originalSkill].IsEnable()
			|| !(*g_pSkillInfoTable)[originalSkill].IsAvailableTime()
			|| (*g_pSkillInfoTable)[originalSkill].IsPassive())
		{
			m_fNextTrace = FLAG_TRACE_NULL;

			UnSetRepeatAction();

			return false;
		}
		
		if (IsStop()) // || m_Action!=ACTION_ATTACK)
		{
			// Zone¿¡ Á¸ÀçÇÏ´Â CreatureÀÎÁö checkÇÑ´Ù.
			MCreature*	pCreature = m_pZone->GetCreature(id);

			//GET_DYING_CREATURE( pCreature, id );
			
			// creature°¡ ¾ø´Â °æ¿ì
			// ³»°¡ SlayerÀÎ °æ¿ì´Â Darkness¾È¿¡ µé¾î°£ Ä³¸¯À» ÂÑ¾Æ°¥ ¼ö ¾ø´Ù.
			if (pCreature==NULL 
				// ½ÃÃ¼ÇÑÅ× ¾µ ¼ö ÀÖ´Â ½ºÅ³ Á¤º¸¸¦ Ãß°¡ÇØ¾ßÇÑ´Ù. FUCK!
				|| pCreature->IsDead()
				&& originalSkill!=MAGIC_RESURRECT && originalSkill!=SKILL_TRANSFUSION
				&& originalSkill!=SKILL_SOUL_REBIRTH 
				&& originalSkill != SKILL_BLOODY_WINGS
				|| pCreature->IsInGroundElemental() && pCreature->IsOusters() && !g_pPlayer->IsOusters()
				// 2004, 10, 14, sobiet modify start - ¶óÀÌÆ®´Ï½º°É·ÈÀ»¶§ ´ÙÅ©´Ï½º ¾È¿¡ ³à¼® °ø°ÝÀÌ ¾ÈµÈ´Ù°í ÇØ¼­ ¼öÁ¤..
//				|| pCreature->IsInDarkness() && !pCreature->IsNPC() && 
//				!(IsVampire() && !HasEffectStatus( EFFECTSTATUS_LIGHTNESS )  ||
//				g_pZone->GetID() != 3001 && IsVampire() )
//				&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST ) 
				|| pCreature->IsInDarkness() && !pCreature->IsNPC() && !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST ) && !g_pPlayer->HasEffectStatus( EFFECTSTATUS_FLAME_SIGH) &&
				(!IsVampire() && !HasEffectStatus( EFFECTSTATUS_LIGHTNESS ) || g_pZone->GetID() == 3001 && IsVampire() )
				// 2004, 10, 14, sobiet modify end- ¶óÀÌÆ®´Ï½º°É·ÈÀ»¶§ ´ÙÅ©´Ï½º ¾È¿¡ ³à¼® °ø°ÝÀÌ ¾ÈµÈ´Ù°í ÇØ¼­ ¼öÁ¤..
#ifdef __METROTECH_TEST__
				&& !g_bLight
#endif
				|| (*g_pActionInfoTable)[originalSkill].GetUser() == FLAG_ACTIONINFO_USER_SLAYER && (!pCreature->IsSlayer() || !pCreature->IsPlayerOnly())
				|| (*g_pActionInfoTable)[originalSkill].GetUser() == FLAG_ACTIONINFO_USER_VAMPIRE && (!pCreature->IsVampire() || !pCreature->IsPlayerOnly())
				|| (*g_pActionInfoTable)[originalSkill].GetUser() == FLAG_ACTIONINFO_USER_OUSTERS && (!pCreature->IsOusters() || !pCreature->IsPlayerOnly())
				|| (*g_pActionInfoTable)[originalSkill].GetUser() == FLAG_ACTIONINFO_USER_MONSTER && (!pCreature->IsVampire() || pCreature->IsPlayerOnly())

				|| IsSlayer() && !(*g_pActionInfoTable)[originalSkill].IsUserSlayer() && pCreature->IsSlayer() && pCreature->IsPlayerOnly() && !bForceAttack
				|| IsVampire() && !(*g_pActionInfoTable)[originalSkill].IsUserVampire() && pCreature->IsVampire() && pCreature->IsPlayerOnly() && !bForceAttack				
				|| IsOusters() && !(*g_pActionInfoTable)[originalSkill].IsUserOusters() && pCreature->IsOusters() && pCreature->IsPlayerOnly() && !bForceAttack				
				) 
			{
				// ¿©±â±îÁö ¿À¸é ACTIONINFO_NULLÀÎµ¥..
				// UnSetRepeatAction()¿¡¼­ Á¦´ë·Î Ã³¸®ÇÏ±â À§ÇØ¼­.. - -;;
				// ¾ÏÆ° Äá°¡·ç´Ù.. - -;;
				if (IsRepeatAction())
				{
					m_nUsedActionInfo = originalSkill;

					UnSetRepeatAction();

					m_nUsedActionInfo = ACTIONINFO_NULL;
				}
				else
				{
					UnSetRepeatAction();
				}

				m_fNextTrace = FLAG_TRACE_NULL; 
				m_fTraceBuffer	= FLAG_TRACE_NULL;	
				return false;
			}

			if( pCreature != NULL )
			{
				if( pCreature->GetObjectType() == MObject::TYPE_CREATURE )
				{
					if( IsSlayer() && g_pZone->GetSector( GetX(), GetY() ).IsSafeSlayer() && 
						pCreature->IsSlayer() && !g_pZone->GetSector( pCreature->GetX(), pCreature->GetY() ).IsSafeSlayer() && pCreature->IsPlayerOnly() ||
						IsVampire() && g_pZone->GetSector( GetX(), GetY() ).IsSafeVampire() && 
						pCreature->IsVampire() && !g_pZone->GetSector( pCreature->GetX(), pCreature->GetY() ).IsSafeVampire() && pCreature->IsPlayerOnly() ||
						IsOusters() && g_pZone->GetSector( GetX(), GetY() ).IsSafeOusters() && 
						pCreature->IsOusters() && !g_pZone->GetSector( pCreature->GetX(), pCreature->GetY() ).IsSafeOusters() && pCreature->IsPlayerOnly())
					{
//						UnSetLockMode();
						UnSetRepeatAction();
//						g_pTopView->SetSelectedNULL();
						return false;
					}
				}
			}

			bool bTraceCreatureToForceAttack = true;
			bool bKeepTraceCreature			= false;			

			//-------------------------------------------------------
			// NPCÀÎ °æ¿ì.. ±×³É ÃßÀûÇÏ°Ô ÇÑ´Ù.
			//-------------------------------------------------------
			if (pCreature->IsNPC())
			{
				TraceCreatureToBasicAction(id, false);				

				return false;
			}
			else
				if (bForceAttack)
				{
					bTraceCreatureToForceAttack = true;
					bKeepTraceCreature			= false;			
					
					UnSetRequestMode();
				}


			//-------------------------------------------------------
			// Æ¯Á¤ ±â¼ú¿¡ ´ëÇÑ Ã¼Å© - µû·Î »©¾ßµÇ´Âµ¥. ´Ã ±×·¸µíÀÌ.. --;
			//-------------------------------------------------------
			switch (originalSkill)
			{
				//-------------------------------------------------------
				// SKILL_TRANSFUSION
				//-------------------------------------------------------
				case SKILL_TRANSFUSION :

					//--------------------------------------------------
					// »ç¿ëÇÒ ¼ö ¾ø´Â °æ¿ì
					//--------------------------------------------------
					// ³» HP°¡ 30º¸´Ù ÀûÀº °æ¿ì
					// ´Ù¸¥ ±æµåÀÎ °æ¿ì
					// »ó´ëÀÇ HP°¡ fullÀÎ °æ¿ì
					if (GetHP() < 30
						|| pCreature->IsSlayer()
							//|| GetGuildNumber()!=pCreature->GetGuildNumber()	
							// vampireÀÇ default ±æµå
							//&& GetGuildNumber()!=0
						//	&& pCreature->GetGuildNumber()!=0
						|| pCreature->GetHP()==pCreature->GetMAX_HP()
						|| pCreature->IsUndergroundCreature()
						|| pCreature->IsFlyingCreature() )
					{
						m_fNextTrace = FLAG_TRACE_NULL;

						UnSetRepeatAction();

						return false;
					}
				break;

				//-------------------------------------------------------
				// SKILL_THROW_BOMB
				//-------------------------------------------------------
				case SKILL_THROW_BOMB :
				{
					MItem* pItem = g_pInventory->FindItem( ITEM_CLASS_BOMB );
								
					if (pItem==NULL)
					{
						m_fNextTrace = FLAG_TRACE_NULL;
						UnSetRepeatAction();
						return false;
					}

					useSkill = pItem->GetUseActionInfo();
				}
				break;
			
			}

			//-------------------------------------------------------
			// Æ¯Á¤ÇÑ »óÈ² Ã¼Å©
			//-------------------------------------------------------
			if ((*g_pActionInfoTable)[originalSkill].HasOption())
			{
				//-------------------------------------------------------
				// bless °É·Á¾ß »ç¿ëÇÏ´Â ±â¼ú
				//-------------------------------------------------------
				if ((*g_pActionInfoTable)[originalSkill].IsOptionUseWithBless())
				{
					if (!m_bEffectStatus[EFFECTSTATUS_BLESS])
					{
							m_fNextTrace = FLAG_TRACE_NULL;

							UnSetRepeatAction();

							return false;
					}
				}
			}
			//-------------------------------------------------------
			// itemÀ» »ç¿ëÇÏ´Â ±â¼úÀÌ¸é itemÀ» Ã¼Å©ÇØ¾ß ÇÑ´Ù.
			//-------------------------------------------------------
			WORD fWeaponType = (*g_pActionInfoTable)[originalSkill].GetWeaponType();
			if (fWeaponType	& FLAG_ACTIONINFO_WEAPON_HOLY_WATER)
			{
				MItem* pItem = g_pInventory->FindItem( ITEM_CLASS_HOLYWATER );

				if (pItem==NULL)
				{
					m_fNextTrace = FLAG_TRACE_NULL;

					UnSetRepeatAction();

					return false;
				}

				// holy water°¡ ÀÖ´Â °æ¿ì¶ó¸é..
				// µýÁþ ¸øÇÏµµ·Ï... item °íÁ¤..
				// ±â¼ú ³¡³ª°í Ç®¾î¾ß ÇÑ´Ù.
				//SetItemCheckBuffer( pItem, ITEM_CHECK_BUFFER_SKILL_FROM_ITEM );
			}

			//-------------------------------------------------------
			// itemÀ» »ç¿ëÇÏ´Â ±â¼úÀÌ¸é itemÀ» Ã¼Å©ÇØ¾ß ÇÑ´Ù.
			//-------------------------------------------------------
			if (fWeaponType & FLAG_ACTIONINFO_WEAPON_BOMB)
			{
				MItem* pItem = g_pInventory->FindItem( ITEM_CLASS_BOMB );

				if (pItem==NULL)
				{
					m_fNextTrace = FLAG_TRACE_NULL;

					UnSetRepeatAction();

					return false;
				}

				// holy water°¡ ÀÖ´Â °æ¿ì¶ó¸é..
				// µýÁþ ¸øÇÏµµ·Ï... item °íÁ¤..
				// ±â¼ú ³¡³ª°í Ç®¾î¾ß ÇÑ´Ù.
				//SetItemCheckBuffer( pItem, ITEM_CHECK_BUFFER_SKILL_FROM_ITEM );
			}

			//-------------------------------------------------------
			// ÃÑÀ» »ç¿ëÇÏ´Â ±â¼úÀÌ¸é 
			// ÃÑ¾ËÀ» Ã¼Å©ÇØ¾ß ÇÑ´Ù.
			//-------------------------------------------------------
			if ((*g_pActionInfoTable)[originalSkill].IsWeaponTypeGunAny())
			{
					if (g_pCurrentMagazine==NULL)
					{
						// ÃÑÀ» »ç¿ëÇÏ´Âµ¥ ÅºÃ¢ÀÌ ¾ø´Â °æ¿ì
						PlaySound( SOUND_ITEM_NO_MAGAZINE );

						// ÅºÃ¢ ¾øÀ»¶§ delay
							m_DelayTime	= g_CurrentTime + GetActionInfoDelay(m_nBasicActionInfo);

						m_fNextTrace	= FLAG_TRACE_NULL;
						return false;					
					}
					else
					{
						if (g_pCurrentMagazine->GetNumber()==0)
						{
							// ÃÑÀ» »ç¿ëÇÏ´Âµ¥ ÅºÃ¢ÀÌ ¾ø´Â °æ¿ì
							PlaySound( SOUND_ITEM_NO_MAGAZINE );

							// ÅºÃ¢ ¾øÀ»¶§ delay
							m_DelayTime	= g_CurrentTime + GetActionInfoDelay(m_nBasicActionInfo);

							if (IsRepeatAction())
							{
								m_nUsedActionInfo = originalSkill;

								UnSetRepeatAction();

								m_nUsedActionInfo = ACTIONINFO_NULL;
							}
							else
							{
								UnSetRepeatAction();
							}

							m_fNextTrace	= FLAG_TRACE_NULL;

							// [µµ¿ò¸»] ÃÑ¾Ë ´Ù ½èÀ» ¶§
//							__BEGIN_HELP_EVENT
//								ExecuteHelpEvent( HE_ITEM_EMPTY_BULLET );
//							__END_HELP_EVENT
							

							return false;
						}
					}
			}

			// creature°¡ ÀÌ¹Ì Á×Àº °æ¿ì
			//if (pCreature->IsDead()) return;

			// »ç¿ëÇÑ ±â¼ú·Î ¼³Á¤
			m_nNextUsedActionInfo	= useSkill;

			// ÃßÀû Á¤º¸ ¼³Á¤
			SetTraceID( id );
			m_fTrace	= FLAG_TRACE_CREATURE_SPECIAL;		
			m_TraceX	= pCreature->GetX();
			m_TraceY	= pCreature->GetY();
			m_TraceZ	= pCreature->GetZ();

			// ÃßÀû´ë»óÀÌ µÇ´Â Ä³¸¯ÅÍÀÇ Á¾Á·
			//m_TraceCreatureTribe = pCreature->GetCreatureTribe();

			// ÇöÀç ÃßÀûÇÏ´Â ´ë»ó¿¡ ´ëÇØ¼­ ±â¾ïÇØµÐ´Ù.
			m_fTraceBuffer	= m_fTrace;	
			m_TraceIDBuffer = id;

			//------------------------------------------------------------
			// SpecialActionÀÌ ¹¹³Ä¿¡ µû¶ó¼­.. 
			// ÇöÀç PlayerÀÇ ´É·Â¿¡ µû¶ó¼­ TraceDistance°¡ ´Þ¶óÁú °ÍÀÌ´Ù.
			//------------------------------------------------------------
			//------------------------------------------------------------
			// °­Á¦ °ø°ÝÀÎ°¡?
			// °è¼Ó ÃßÀûÇÏ´Â°¡?
			//------------------------------------------------------------
			m_bTraceCreatureToForceAttack	= bTraceCreatureToForceAttack;
			m_bKeepTraceCreature			= bKeepTraceCreature;
			
			m_TraceDistance		= GetActionInfoRange(m_nNextUsedActionInfo);
			
			//------------------------------------------------------------

			// ÃßÀûÇÏ´Â ±æ ¼³Á¤
			SetAction( m_MoveAction );
			SetNextDestination(m_TraceX, m_TraceY);

			// bufferingÀ» ¾ø¾Ø´Ù.
			m_fNextTrace			= FLAG_TRACE_NULL;

			// °è¼Ó ÂÑ¾Æ°¡´Â °ÍÀ» ÁßÁö
			m_bKeepTraceCreature	= false;
		
			//------------------------------------------------------------
			// messageÃâ·Â
			//------------------------------------------------------------
			DEBUG_ADD_FORMAT("Trace Creature To SpecialAction: ID=%d, (%d, %d)", id, m_TraceX, m_TraceY);					

			// ¼±ÅÃµÈ sector ¾ø¾Ö±â
			g_pTopView->SetSelectedSectorNULL();
		}
		else
		{
			//------------------------------------------------------------
			// ¹Ýº¹ actionÇÏ´Â ÁßÀÌ ¾Æ´Ñ °æ¿ì¿¡ ´ÙÀ½ µ¿ÀÛ buffering
			//------------------------------------------------------------
			if (m_bRepeatAction)
			{
				m_fNextTrace	= FLAG_TRACE_NULL;

				return false;
			}
			else
			{
				m_fNextTrace	= FLAG_TRACE_CREATURE_SPECIAL;
				m_bNextForceAttack = bForceAttack;
				m_NextTraceID	= id;

				m_TraceIDBuffer = id;

				m_bRepeatAction = FALSE;

				m_fTraceBuffer = FLAG_TRACE_NULL;

				return true;//false;
			}
		}

		return true;
	}
	
	// Target to creature°¡ ¾Æ´Ï¶ó´Â ÀÇ¹Ì.
	return false;
}


//----------------------------------------------------------------------
// Trace Sector To BasicAction
//----------------------------------------------------------------------
// ±âº»°ø°ÝÀ¸·Î(L-Click)  Sector(sX,sY)¸¦ °ø°ÝÇÑ´Ù.
// ¸ØÃçÀÖ´Â »óÅÂ°¡ ¾Æ´Ï¸é.. Next¿¡ ¼³Á¤ÇØµÐ´Ù.
//----------------------------------------------------------------------
bool	
MPlayer::TraceSectorToBasicAction(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)
{
	//-----------------------------------------------------
	// delay°¡ ÀÖÀ¸¸é ¾ÈµÈ´Ù.
	//-----------------------------------------------------
	if (!IsNotDelay() 
		|| HasEffectStatus ( EFFECTSTATUS_ETERNITY_PAUSE )
		|| IsInCasket()	// [»õ±â¼ú3]
		|| g_bZoneSafe)
	{
		return false;
	}

	// ÁÂÇ¥°¡ Àß¸ø ÀÔ·ÂµÈ °æ¿ì
	if (sX==SECTORPOSITION_NULL || sY==SECTORPOSITION_NULL)
		return false;
	
	if (m_nBasicActionInfo == ACTIONINFO_NULL
		// Hava araci bindiginde saldiri yapilamaz.
		|| IsRide_Air_Transporter()
		|| CanActionByZoneInfo())
	{
		return false;
	}
		
	if (IsStop()) // || m_Action!=ACTION_ATTACK)
	{		
		// »ç¿ëÇÑ ±â¼ú·Î ¼³Á¤
		m_nNextUsedActionInfo	= GetBasicActionInfo(); //m_nBasicActionInfo;
		
		// ÃßÀû Á¤º¸ ¼³Á¤
		m_fTrace	= FLAG_TRACE_SECTOR_BASIC;
		m_TraceX	= sX;
		m_TraceY	= sY;
		m_TraceZ	= 0;
		
		// ÇöÀç ÃßÀûÇÏ´Â ´ë»ó¿¡ ´ëÇØ¼­ ±â¾ïÇØµÐ´Ù.
		m_fTraceBuffer	= m_fTrace;	

		//------------------------------------------------------------
		// BasicActionÀÌ ¹¹³Ä¿¡ µû¶ó¼­..
		// ÇöÀç PlayerÀÇ ´É·Â¿¡ µû¶ó¼­ TraceDistance°¡ ´Þ¶óÁú °ÍÀÌ´Ù.
		//------------------------------------------------------------
		
		m_TraceDistance		= GetActionInfoRange(m_nNextUsedActionInfo);
		
		//------------------------------------------------------------		

		// ÃßÀûÇÏ´Â ±æ ¼³Á¤
		SetAction( m_MoveAction );
		SetNextDestination(m_TraceX, m_TraceY);

		// bufferingÀ» ¾ø¾Ø´Ù.
		m_fNextTrace			= FLAG_TRACE_NULL;
		
		// °è¼Ó ÂÑ¾Æ°¡´Â °ÍÀ» ÁßÁö
		m_bKeepTraceCreature	= false;

		//------------------------------------------------------------
		// messageÃâ·Â
		//------------------------------------------------------------
		#ifdef	OUTPUT_DEBUG
			DEBUG_ADD_FORMAT("Trace Sector To BasicAction: (%d, %d)", sX, sY);				
		#endif
	}
	else
	{
		//------------------------------------------------------------
		// ¹Ýº¹ actionÇÏ´Â ÁßÀÌ ¾Æ´Ñ °æ¿ì¿¡ ´ÙÀ½ µ¿ÀÛ buffering
		//------------------------------------------------------------
		if (m_bRepeatAction)
		{
			m_fNextTrace	= FLAG_TRACE_NULL;

			return false;
		}
		else
		{
			m_fNextTrace	= FLAG_TRACE_SECTOR_BASIC;
			m_NextTraceX	= sX;
			m_NextTraceY	= sY;

			m_bRepeatAction = FALSE;

			return false;
		}
	}

	return true;
}

//----------------------------------------------------------------------
// Trace Sector To SpecialAction
//----------------------------------------------------------------------
// Æ¯¼ö°ø°ÝÀ¸·Î(R-Click)  Sector(sX,sY)¸¦ °ø°ÝÇÑ´Ù.
// ¸ØÃçÀÖ´Â »óÅÂ°¡ ¾Æ´Ï¸é.. Next¿¡ ¼³Á¤ÇØµÐ´Ù.
//----------------------------------------------------------------------
// [return°ª]
//
//  Zone¿¡ »ç¿ëÇÏÁö ¸øÇÏ´Â °æ¿ì¸¸ false¸¦ returnÇÑ´Ù. 
//
//----------------------------------------------------------------------
bool	
MPlayer::TraceSectorToSpecialAction(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)
{
	if((HasEffectStatus(EFFECTSTATUS_SLEEP) || HasEffectStatus( EFFECTSTATUS_BLOCK_HEAD ) )
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
		return false;
	
	//-----------------------------------------------------
	// delay°¡ ÀÖÀ¸¸é ¾ÈµÈ´Ù.
	//-----------------------------------------------------
	if (!IsNotDelay() 
		|| HasEffectStatus( EFFECTSTATUS_ETERNITY_PAUSE )
		|| IsInCasket()	// [»õ±â¼ú3]
		|| g_bZoneSafe
		|| CanActionByZoneInfo()
		)
	{
		return false;
	}

	// ÁÂÇ¥°¡ Àß¸ø ÀÔ·ÂµÈ °æ¿ì
	if (sX==SECTORPOSITION_NULL || sY==SECTORPOSITION_NULL)
		return false;

	
	if(m_nSpecialActionInfo != ACTIONINFO_NULL)
	{
		MActionInfo& rActionInfo = (*g_pActionInfoTable)[m_nSpecialActionInfo];

		if(rActionInfo.GetCastingTime())
		{
			m_SkillCastingX	= sX;
			m_SkillCastingY	= sY;
		}
	}
	
	if (m_nSpecialActionInfo == ACTIONINFO_NULL		
		// Hava araci bindiginde skill kullanilamaz.
		|| IsRide_Air_Transporter()
		// ¹º°¡ ±â¼ú »ç¿ëÀ» °ËÁ¤ ¹Þ¾Æ¾ß ÇÏ¸é »ç¿ëÇÒ ¼ö ¾ø´Ù.
		//|| m_WaitVerify != WAIT_VERIFY_NULL)	// 2001.8.20 ÁÖ¼®Ã³¸®
		|| IsInSafeSector() == 2
		// ¿ÏÀü ¾ÈÀüÁö´ë¿¡¼­´Â ±â¼úÀ» »ç¿ëÇÒ ¼ö ¾ø´Ù.		
		)
	{
		m_fNextTrace = FLAG_TRACE_NULL;

		UnSetRepeatAction();

		return false;
	}
	// 2004, 9, 18, sobeit add start - ÅÍ·¿ ÆÄÀÌ¾î´Â »çÁ¤°Å¸® ¹þ¾î³ª¹È ¹«½Ã ¤Ì.¤Ì 
	if (m_nSpecialActionInfo==SKILL_TURRET_FIRE)
	{
		m_TraceDistance		= GetActionInfoRange(m_nSpecialActionInfo);
		if (max(abs(m_X-sX), abs(m_Y-sY)) > m_TraceDistance || !g_pSkillAvailable->IsEnableSkill( SKILL_TURRET_FIRE ))
		{
			m_fNextTrace = FLAG_TRACE_NULL;
			UnSetRepeatAction();
			InstallTurretStopAttack();
			return false;
		}
	}
if (m_nSpecialActionInfo==SKILL_CLINT_ADV2_HALCYON_MK1)
	{
		m_TraceDistance		= GetActionInfoRange(m_nSpecialActionInfo);
		if (max(abs(m_X-sX), abs(m_Y-sY)) > m_TraceDistance || !g_pSkillAvailable->IsEnableSkill( SKILL_CLINT_ADV2_HALCYON_MK1 ))
		{
			m_fNextTrace = FLAG_TRACE_NULL;
	        UnSetRepeatAction();
			//UnSetRequestMode();
		//	InstallTurretStopAttack();
			return false;
		}
	}
if (m_nSpecialActionInfo==SKILL_ADV2_HALCYON_MK1_NAPALN_BULLET || m_nSpecialActionInfo==SKILL_CLIENT_ADV2_HALCYON_MK1)
	{
		m_TraceDistance		= GetActionInfoRange(m_nSpecialActionInfo);
		if (max(abs(m_X-sX), abs(m_Y-sY)) > m_TraceDistance || !g_pSkillAvailable->IsEnableSkill( SKILL_ADV2_HALCYON_MK1_NAPALN_BULLET ) || !g_pSkillAvailable->IsEnableSkill( SKILL_CLIENT_ADV2_HALCYON_MK1 ))
		{
			m_fNextTrace = FLAG_TRACE_NULL;
			UnSetRepeatAction();
			//UnSetRequestMode();
			//	InstallTurretStopAttack();
			return false;
		}
	}
	// 2004, 9, 18, sobeit add end

	// ´Ù¸¥ ±â¼úÀ» »ç¿ëÁßÀÎ °æ¿ì
	if (m_nUsedActionInfo!=ACTIONINFO_NULL)
		return false;

	// 2004, 9, 21, sobeit add start - Å¸°Ù Å©¸®ÃÄ ½ºÅ³ÀÌ Å¸°Ù Á¸ ½ºÅ³·Î »ç¿ëÇÒ ¼ö ÀÖ°Ô
	bool bAbleTargetSector = false;
//	if(m_nSpecialActionInfo == SKILL_DESTRUCTION_SPEAR || m_nSpecialActionInfo == SKILL_DESTRUCTION_SPEAR_MASTERY)
//	{
//		if(g_pSkillAvailable->IsEnableSkill( SKILL_DESTRUCTION_SPEAR_MASTERY ))
//			bAbleTargetSector = true;
//	}
//	else 
	if(m_nSpecialActionInfo == SKILL_ICE_LANCE || m_nSpecialActionInfo == SKILL_ICE_LANCE_MASTERY)
	{
		if(g_pSkillAvailable->IsEnableSkill( SKILL_ICE_LANCE_MASTERY ))
			bAbleTargetSector = true;
	}

	// 2006.08.22 sjheon
	if(HasEffectStatus(EFFECTSTATUS_TRANSFORM_TO_WOLF))
	{
		if( !(m_nSpecialActionInfo == MAGIC_HOWL || m_nSpecialActionInfo == MAGIC_EAT_CORPSE || m_nSpecialActionInfo == MAGIC_UN_TRANSFORM))
			return false ;
	}
	else if(HasEffectStatus(EFFECTSTATUS_TRANSFORM_TO_BAT))
	{
		if( !(m_nSpecialActionInfo == MAGIC_UN_TRANSFORM))
			return false ;
	}	

	// 2004, 9, 21, sobeit add end
	// Zone¿¡ »ç¿ëÇÒ ¼ö ÀÖ´Â °Íµé¸¸ »ç¿ëÇÑ´Ù.
	if ((*g_pActionInfoTable)[m_nSpecialActionInfo].IsTargetZone() || bAbleTargetSector)
	{
		int grade = 0;
		int		useSkill = m_nSpecialActionInfo;

		if( IsOusters() )
		{
			grade = (*g_pSkillInfoTable)[useSkill].GetExpLevel() / 15;
		}
		
		if( (*g_pActionInfoTable)[useSkill].IsUseActionStep() && grade > 0 )
			useSkill = (*g_pActionInfoTable)[useSkill].GetActionStep( grade - 1 ); 			
			
		// 2004, 9, 20, sobeit add start
		if((*g_pActionInfoTable)[useSkill].IsMasterySkillStep())
		{
			int TempSkill = (*g_pActionInfoTable)[useSkill].GetActionStep( (*g_pActionInfoTable)[useSkill].GetMasterySkillStep() );
			if(g_pSkillAvailable->IsEnableSkill( (ACTIONINFO)TempSkill ))
				useSkill = TempSkill;
		}
		// 2004, 9, 20, sobeit add end
		
//		if (useSkill==SKILL_TURRET_FIRE)
//		{
//			if (max(abs(m_X-sX), abs(m_Y-sY)) 
//				> m_TraceDistance)
//			{
//				m_fNextTrace = FLAG_TRACE_NULL;
//				UnSetRepeatAction();
//				return false;
//			}
//		}
		//-------------------------------------------------------
		// ÆøÅº °ñ¶ó¼­ ´øÁö±â
		//-------------------------------------------------------
		if (useSkill==SKILL_THROW_BOMB)
		{
			//-------------------------------------------------------
			// ÇöÀç »ç¿ëÇÒ ¼ö ÀÖ´Â ±â¼úÀÎÁö Ã¼Å©..
			// Passive ½ºÅ³ÀÌ¸é »ç¿ë ¸øÇÏ°Ô..
			//-------------------------------------------------------
			if (IsSlayer()
				&& !IsWaitVerify()
				&& IsItemCheckBufferNULL())
			{											
				ITEM_CLASS	itemClass = ITEM_CLASS_BOMB;

				MItemClassFinder itemFinder(itemClass);
				MItem* pItem = ((MItemManager*)g_pInventory)->FindItem( itemFinder );
							
				if (pItem==NULL)
				{
					m_fNextTrace = FLAG_TRACE_NULL;
					UnSetRepeatAction();
					return false;
				}

				useSkill = pItem->GetUseActionInfo();				
			}
		}

		
		//-------------------------------------------------------
		// ÇöÀç »ç¿ëÇÒ ¼ö ÀÖ´Â ±â¼úÀÎÁö Ã¼Å©..SKILL_CLINT_ADV2_HALCYON_MK1
		// Passive ½ºÅ³ÀÌ¸é »ç¿ë ¸øÇÏ°Ô..
		//-------------------------------------------------------
		if (
			(!g_pSkillAvailable->IsEnableSkill( (ACTIONINFO)m_nSpecialActionInfo ) && m_nSpecialActionInfo != SKILL_TURRET_FIRE)
			|| !(*g_pSkillInfoTable)[m_nSpecialActionInfo].IsEnable()
			|| !(*g_pSkillInfoTable)[m_nSpecialActionInfo].IsAvailableTime()
			|| (*g_pSkillInfoTable)[m_nSpecialActionInfo].IsPassive())
		{
			m_fNextTrace = FLAG_TRACE_NULL;

			UnSetRepeatAction();

			return false;
		}
		if (
			(!g_pSkillAvailable->IsEnableSkill( (ACTIONINFO)m_nSpecialActionInfo ) && m_nSpecialActionInfo != SKILL_CLINT_ADV2_HALCYON_MK1)
			|| !(*g_pSkillInfoTable)[m_nSpecialActionInfo].IsEnable()
			|| !(*g_pSkillInfoTable)[m_nSpecialActionInfo].IsAvailableTime()
			|| (*g_pSkillInfoTable)[m_nSpecialActionInfo].IsPassive())
		{
			m_fNextTrace = FLAG_TRACE_NULL;

			UnSetRepeatAction();

			return false;
		}																													  
		if (
			(!g_pSkillAvailable->IsEnableSkill( (ACTIONINFO)m_nSpecialActionInfo ) && m_nSpecialActionInfo != SKILL_ADV2_HALCYON_MK1_NAPALN_BULLET)
			|| !(*g_pSkillInfoTable)[m_nSpecialActionInfo].IsEnable()
			|| !(*g_pSkillInfoTable)[m_nSpecialActionInfo].IsAvailableTime()
			|| (*g_pSkillInfoTable)[m_nSpecialActionInfo].IsPassive())
		{
			m_fNextTrace = FLAG_TRACE_NULL;

			UnSetRepeatAction();

			return false;
		}
		if (
			(!g_pSkillAvailable->IsEnableSkill( (ACTIONINFO)m_nSpecialActionInfo ) && m_nSpecialActionInfo != SKILL_CLIENT_ADV2_HALCYON_MK1)
			|| !(*g_pSkillInfoTable)[m_nSpecialActionInfo].IsEnable()
			|| !(*g_pSkillInfoTable)[m_nSpecialActionInfo].IsAvailableTime()
			|| (*g_pSkillInfoTable)[m_nSpecialActionInfo].IsPassive())
		{
			m_fNextTrace = FLAG_TRACE_NULL;

			UnSetRepeatAction();

			return false;
		}
		if (IsStop()) // || m_Action!=ACTION_ATTACK)
		{	

			// ¾Æ¿ì½ºÅÍÁî ½ÃÃ¼ ¿À¸¥Å¬¸¯À¸·Î ¹Ù·Î Èí¿µÇÏ°Ô ÇÏ´Â ÄÚµå ÁÖ¼® Ã³¸® 
/*
#ifdef __METROTECH_TEST__
			if (IsOusters())
			{
				bool bAbsorbSoul = true;
				
				MItem* pItem = g_pZone->GetItem( g_pZone->GetItemID( sX, sY ) );
				
				if (pItem==NULL 
					|| pItem->GetItemClass()!=ITEM_CLASS_CORPSE)
				{
					bAbsorbSoul = false;
					
					if(pItem == NULL)
					{
						MCreature *pCreature = g_pZone->GetCreature(g_pZone->GetCreatureID(sX, sY));
						
						if(pCreature != NULL && pCreature->HasEffectStatus(EFFECTSTATUS_COMA))
						{
							bAbsorbSoul = true;
						}
					}
				}
				
				else if(pItem != NULL )
				{
					// ¼º¹°Àº ¾È¸Ô°Ô ÇÏ±â
					MCreature* pCreature = ((MCorpse*)pItem)->GetCreature();
					if(pCreature != NULL)
					{
						if(pCreature->GetCreatureType() >= 371 && pCreature->GetCreatureType() <= 376 || pCreature->GetCreatureType() >= 560 && pCreature->GetCreatureType() <= 563 || pCreature->GetCreatureType() >= 526 && pCreature->GetCreatureType() <= 549 || pCreature->GetCreatureType() == 670 || pCreature->GetCreatureType() == 672 || pCreature->GetCreatureType() == 673)
							bAbsorbSoul = false;
					}
				}
				if(bAbsorbSoul)
					useSkill = SKILL_ABSORB_SOUL;
			}
#endif
			*/
			
			int targetX	= sX;
			int targetY = sY;

			//-------------------------------------------------------
			// Æ¯Á¤ÇÑ ±â¼úÀÎ °æ¿ìÀÇ Ã¼Å©
			//-------------------------------------------------------
			switch (useSkill)
			{
				// Æ®·£½º Ç»ÀüÀº Å¸ÀÏ±â¼ú·Î Ã¼Å©µÇÀÖÁö¸¸ ½ÇÁ¦·Î »ç¿ëÇÒ ¼ö ´Â ¾ø´Ù
				case SKILL_TRANSFUSION:
				case SKILL_WILD_TYPHOON :
				case SKILL_BLOODY_WINGS:	
					return false;
					break;

				//-------------------------------------------------------
				// Èí¿µ
				//-------------------------------------------------------
				// ¹Ù´Ú¿¡ ½ÃÃ¼°¡ ¾ø´Â °æ¿ì´Â ±â¼ú »ç¿ë ¾ÈÇÑ´Ù.
				//-------------------------------------------------------
				case SKILL_ABSORB_SOUL :
					if (IsOusters())
					{
						MItem* pItem = g_pZone->GetItem( g_pZone->GetItemID( sX, sY ) );
						
						if (pItem==NULL 
							|| pItem->GetItemClass()!=ITEM_CLASS_CORPSE)
						{
							if(pItem == NULL)
							{
								MCreature *pCreature = g_pZone->GetCreature(g_pZone->GetCreatureID(sX, sY));

								if(pCreature != NULL && pCreature->HasEffectStatus(EFFECTSTATUS_COMA))
									break;
							}

							m_fNextTrace = FLAG_TRACE_NULL;
							
							UnSetRepeatAction();
							
							return false;
						}
						
						// ¼º¹°Àº ¾È¸Ô°Ô ÇÏ±â
						MCreature* pCreature = ((MCorpse*)pItem)->GetCreature();
						if(pCreature != NULL)
						{
							if(pCreature->GetCreatureType() >= 371 && pCreature->GetCreatureType() <= 376 || pCreature->GetCreatureType() >= 560 && pCreature->GetCreatureType() <= 563 || pCreature->GetCreatureType() >= 526 && pCreature->GetCreatureType() <= 549 || pCreature->GetCreatureType() == 670 || pCreature->GetCreatureType() == 672 || pCreature->GetCreatureType() == 673)
								return false;
						}
						
					}
					else
					{
						m_fNextTrace = FLAG_TRACE_NULL;
						
						UnSetRepeatAction();
						
						return false;
					}
					break;

				//-------------------------------------------------------
				// ´Á´ëÀÎ °æ¿ì ½ÃÃ¼ ¸Ô±â
				//-------------------------------------------------------
				// ¹Ù´Ú¿¡ ½ÃÃ¼°¡ ¾ø´Â °æ¿ì´Â ±â¼ú »ç¿ë ¾ÈÇÑ´Ù.
				//-------------------------------------------------------
				case SKILL_BITE_OF_DEATH :
					if (m_CreatureType != CREATURETYPE_WER_WOLF )
					{
						return false;
					}
					break;
				case MAGIC_EAT_CORPSE :
					if (m_CreatureType==CREATURETYPE_WOLF)
					{
						MItem* pItem = g_pZone->GetItem( g_pZone->GetItemID( sX, sY ) );

						if (pItem==NULL 
							|| pItem->GetItemClass()!=ITEM_CLASS_CORPSE)
						{
							m_fNextTrace = FLAG_TRACE_NULL;

							UnSetRepeatAction();

							return false;
						}

						// ¼º¹°Àº ¾È¸Ô°Ô ÇÏ±â
						MCreature* pCreature = ((MCorpse*)pItem)->GetCreature();
						if(pCreature != NULL)
						{
							if(pCreature->GetCreatureType() >= 371 && pCreature->GetCreatureType() <= 376 || pCreature->GetCreatureType() >= 560 && pCreature->GetCreatureType() <= 563 || pCreature->GetCreatureType() >= 526 && pCreature->GetCreatureType() <= 549 || pCreature->GetCreatureType() == 670 || pCreature->GetCreatureType() == 672 || pCreature->GetCreatureType() == 673)
								return false;
						}
					
					}
					else
					{
						m_fNextTrace = FLAG_TRACE_NULL;

						UnSetRepeatAction();

						return false;
					}
				break;
//				// 2004, 8, 30, sobeit add start - sweep vice test
//				case SKILL_SWEEP_VICE_1:
//					if(!GetSweepViewValue()) //½ºÀ¬¹ÙÀÌ½º¸¦ ¾ÆÁ÷ »ç¿ëÁßÀÌ ¾Æ´Ò¶§
//					{
//						SetSweepViewValue(1);
//						g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);
//						m_TraceX	= 	g_SelectSector.x;
//						m_TraceY	=	g_SelectSector.y;
//						ExecuteActionInfoFromMainNode(SKILL_CLIENT_SWEEP_VICE,g_SelectSector.x, g_SelectSector.y, 0,g_pPlayer->GetDirection(),0	,	
//							g_SelectSector.x, g_SelectSector.y, 0, 0xffff, NULL, false);
//						UnSetRepeatAction();
//					//	return false ;
//					}
//					else
//					{
//						if(!g_bRButtonDown)
//						{
//							if(2 == GetSweepViewValue())
//								useSkill = SKILL_SWEEP_VICE_3;
//							else if(3 == GetSweepViewValue())
//								useSkill = SKILL_SWEEP_VICE_5;
//						}
//						else 
//							return false;
//					}
//					break;
				case SKILL_TURRET_FIRE :
					if (!HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
						return false;
					break;
				case SKILL_CLINT_ADV2_HALCYON_MK1 :
					if (!HasEffectStatus(EFFECTSTATUS_INSTALL_MK1))
						return false;
					break;
				case SKILL_ADV2_HALCYON_MK1_NAPALN_BULLET :
				case SKILL_CLIENT_ADV2_HALCYON_MK1:
					if (!HasEffectStatus(EFFECTSTATUS_INSTALL_MK1))
						return false;
					break;

			}

	

// 2004, 8, 30, sob
	

			WORD fWeaponType = (*g_pActionInfoTable)[useSkill].GetWeaponType();
			
			//------------------------------------------------
			// ÆøÅºÀÎ °æ¿ì.. ·¹º§¿¡ µû¶ó¼­ ÁÂÇ¥ ¾î±ß³ª°Ô
			//------------------------------------------------
			if (fWeaponType	& FLAG_ACTIONINFO_WEAPON_BOMB)
			{
				int skillLevel = (*g_pSkillInfoTable)[SKILL_THROW_BOMB].GetExpLevel();

				int hitRoll = 50 + skillLevel/2;

				if (rand()%100 < hitRoll)
				{
					// Á¦´ë·Î ¸ÂÀº °æ¿ì
				}
				else
				{
					if (rand()%100 < 25
						&& skillLevel < 30)		// 2Å¸ÀÏ ±ÙÃ³
					{
						// ºú¸Â¾Æ¾ß ÇÑ´Ù.
						targetX += rand()%5 - 2;
						targetY += rand()%5 - 2;
					}
					else						// 1Å¸ÀÏ ±ÙÃ³
					{
						// ºú¸Â¾Æ¾ß ÇÑ´Ù.
						targetX += rand()%3 - 1;
						targetY += rand()%3 - 1;
					}
				}
			}

			//------------------------------------------------
			// ÃÑ¾Ë Ã¼Å©
			//------------------------------------------------
			if ((*g_pActionInfoTable)[useSkill].IsWeaponTypeGunAny())
			{
				if (!IsRequestMode() && g_pCurrentMagazine!=NULL)		// ÃÑ¾ËÀÌ ÇÊ¿ä ¾ø´Â °æ¿ì
				{
					if (g_pCurrentMagazine->GetNumber()==0)	// ÃÑ¾ËÀÌ ¾ø´Â °æ¿ì
					{
						// ÃÑÀ» »ç¿ëÇÏ´Âµ¥ ÅºÃ¢ÀÌ ¾ø´Â °æ¿ì
						PlaySound( SOUND_ITEM_NO_MAGAZINE );

						m_fNextTrace	= FLAG_TRACE_NULL;

						

						// [µµ¿ò¸»] ÃÑ¾Ë ´Ù ½èÀ» ¶§
//						__BEGIN_HELP_EVENT
//							ExecuteHelpEvent( HE_ITEM_EMPTY_BULLET );
//						__END_HELP_EVENT

						m_fNextTrace = FLAG_TRACE_NULL;

						UnSetRepeatAction();
						// 2004, 9, 23, sobeit add start - ÅÍ·¿ °ø°Ý ³¡³ª´Â µ¿ÀÛ
					if(HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
						{
							InstallTurretStopAttack();
						}
						else
							// ÅºÃ¢ ¾øÀ»¶§ delay
						m_DelayTime	= g_CurrentTime + GetActionInfoDelay(m_nBasicActionInfo);
						// 2004, 9, 23, sobeit add end
						return false;
					}
				}
			}

			// »ç¿ëÇÑ ±â¼ú·Î ¼³Á¤
			m_nNextUsedActionInfo	= useSkill;

			// ÃßÀû Á¤º¸ ¼³Á¤
			m_fTrace	= FLAG_TRACE_SECTOR_SPECIAL;
			m_TraceX	= targetX;
			m_TraceY	= targetY;
			m_TraceZ	= 0;

			// ÇöÀç ÃßÀûÇÏ´Â ´ë»ó¿¡ ´ëÇØ¼­ ±â¾ïÇØµÐ´Ù.
			m_fTraceBuffer	= m_fTrace;	
			
			//------------------------------------------------------------
			// BasicActionÀÌ ¹¹³Ä¿¡ µû¶ó¼­..
			// ÇöÀç PlayerÀÇ ´É·Â¿¡ µû¶ó¼­ TraceDistance°¡ ´Þ¶óÁú °ÍÀÌ´Ù.
			//------------------------------------------------------------
			
			m_TraceDistance		= GetActionInfoRange(m_nNextUsedActionInfo);
			
			//------------------------------------------------------------		

			// ÃßÀûÇÏ´Â ±æ ¼³Á¤
			SetAction( m_MoveAction );
			SetNextDestination(m_TraceX, m_TraceY);

			// bufferingÀ» ¾ø¾Ø´Ù.
			m_fNextTrace			= FLAG_TRACE_NULL;

			// °è¼Ó ÂÑ¾Æ°¡´Â °ÍÀ» ÁßÁö
			m_bKeepTraceCreature	= false;
			
			//------------------------------------------------------------
			// messageÃâ·Â
			//------------------------------------------------------------
			DEBUG_ADD_FORMAT("Trace Sector To SpecialAction: (%d, %d)", sX, sY);					
			if(useSkill == SKILL_TELEPORT || useSkill == SKILL_WARP_PORTAL)
			{
				return false;
			}
		}
		else
		{

			//------------------------------------------------------------
			// ¹Ýº¹ actionÇÏ´Â ÁßÀÌ ¾Æ´Ñ °æ¿ì¿¡ ´ÙÀ½ µ¿ÀÛ buffering
			//------------------------------------------------------------
			if (m_bRepeatAction)
			{
				m_fNextTrace	= FLAG_TRACE_NULL;

				return false;
			}
			else
			{
				DEBUG_ADD_FORMAT("Trace Sector To SpecialAction: Buffering");					
				
				m_fNextTrace	= FLAG_TRACE_SECTOR_SPECIAL;
				m_NextTraceX	= sX;
				m_NextTraceY	= sY;

				m_bRepeatAction = FALSE;

				// 200411, 12, 21, sobeit modify start - ÀÌµ¿Áß ½ºÅ³ »ç¿ë½Ã ¹Ýº¹ÀÌ ¾ÈµÇ´Â Çö»ó ¼öÁ¤
				//return false;
				return true;
				// 200411, 12, 21, sobeit modify end
			}
		}

		return true;
	}

	// Zone¿¡ »ç¿ëÇÏ´Â°Ô ¾Æ´Ï¶ó´Â ÀÇ¹Ì
	return false;
}

//----------------------------------------------------------------------
// Trace Item
//----------------------------------------------------------------------
// ItemÀ¸·Î ´Ù°¡°¡¼­ ÁÖ¿ö¾ß ÇÑ´Ù.
// ¸ØÃçÀÖ´Â »óÅÂ°¡ ¾Æ´Ï¸é.. Next¿¡ ¼³Á¤ÇØµÐ´Ù.
//----------------------------------------------------------------------
bool	
MPlayer::TraceItem(TYPE_OBJECTID id)
{
	//-----------------------------------------------------
	// delay°¡ ÀÖÀ¸¸é ¾ÈµÈ´Ù.
	//-----------------------------------------------------
	if (!IsNotDelay() 
		|| HasEffectStatus( EFFECTSTATUS_ETERNITY_PAUSE )
		|| IsInCasket()	// [»õ±â¼ú3]
		|| IsInDarkness() && (!HasEffectStatus( EFFECTSTATUS_LIGHTNESS )||g_pZone->GetID() == 3001)
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_FLAME_SIGH)
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
	{
		return false;
	}

	if (IsStop()) // || m_Action!=ACTION_ATTACK)
	{
		// Zone¿¡ Á¸ÀçÇÏ´Â ItemÀÎÁö checkÇÑ´Ù.
		MItem*	pItem = m_pZone->GetItem(id);

		// itemÀÌ zone¿¡ ¾ø´Â °æ¿ì
		if (pItem==NULL
			|| g_pZone->GetSector(pItem->GetX(), pItem->GetY()).HasDarkness() && 
			( !IsVampire() && (!HasEffectStatus( EFFECTSTATUS_LIGHTNESS ) || 
			IsVampire() && g_pZone->GetID() == 3001) )
			&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_FLAME_SIGH)
			&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
			&& !g_bLight
#endif
			) 
		{
			return false;
		}
	
		//-------------------------------------------------------------
		// Item ÁÝ±â °¡´É °Å¸®¿¡ ÀÌ¹Ì ÀÖ´Ù¸é..
		//-------------------------------------------------------------
		if (max(abs(pItem->GetX()-m_X), abs(pItem->GetY()-m_Y)) <= 1)
		{
			DEBUG_ADD("Pickup Item in Trace Dist.");

			TraceNULL();
			
			// ÃßÀûÀ» ¿Ï·áÇßÀ¸¹Ç·Î ItemÀ» ÁÝ´Â´Ù.
			PickupItem( pItem );	
			
			return true;
		}


		// ÃßÀû Á¤º¸ ¼³Á¤
		SetTraceID( id );
		m_fTrace	= FLAG_TRACE_ITEM;
		m_TraceX	= pItem->GetX();
		m_TraceY	= pItem->GetY();
		m_TraceZ	= 0;

		// ÇöÀç ÃßÀûÇÏ´Â ´ë»ó¿¡ ´ëÇØ¼­ ±â¾ïÇØµÐ´Ù.
		m_fTraceBuffer	= m_fTrace;	
		

		//------------------------------------------------------------		
		// ÇöÀç PlayerÀÇ ´É·Â¿¡ µû¶ó¼­ TraceDistance°¡ ´Þ¶óÁú±î?
		//------------------------------------------------------------
		
		m_TraceDistance		= 1;
		
		//------------------------------------------------------------

		// ÃßÀûÇÏ´Â ±æ ¼³Á¤
		SetAction( m_MoveAction );
		SetNextDestination(m_TraceX, m_TraceY);

		// bufferingÀ» ¾ø¾Ø´Ù.
		m_fNextTrace			= FLAG_TRACE_NULL;
	
		// °è¼Ó ÂÑ¾Æ°¡´Â °ÍÀ» ÁßÁö
		m_bKeepTraceCreature	= false;

		//------------------------------------------------------------
		// messageÃâ·Â
		//------------------------------------------------------------
		DEBUG_ADD_FORMAT("Trace Item: %d", id);				
	}
	else
	{
		//------------------------------------------------------------
		// ¹Ýº¹ actionÇÏ´Â ÁßÀÌ ¾Æ´Ñ °æ¿ì¿¡ ´ÙÀ½ µ¿ÀÛ buffering
		//------------------------------------------------------------
		if (m_bRepeatAction)
		{
			m_fNextTrace	= FLAG_TRACE_NULL;

			return false;
		}
		else
		{
			m_fNextTrace	= FLAG_TRACE_ITEM;
			m_NextTraceID	= id;

			m_bRepeatAction = FALSE;

			return false;
		}
	}

	return true;
}

//----------------------------------------------------------------------
// Trace InteractionObject
//----------------------------------------------------------------------
// InteractionObject·Î ´Ù°¡°¡¼­ currentActionÀ» º¯°æ½ÃÅ²´Ù.
// --> ´Ù°¡°¬À»¶§, currentActionÀÌ 
//     "ÀÌ¹Ì" º¯°æµÇ¾îÀÖÀ¸¸é º¯°æÇÏÁö ¾Ê¾Æ¾ß ÇÑ´Ù.
//
// ¸ØÃçÀÖ´Â »óÅÂ°¡ ¾Æ´Ï¸é.. Next¿¡ ¼³Á¤ÇØµÐ´Ù.
//----------------------------------------------------------------------
bool	
MPlayer::TraceInteractionObject(TYPE_OBJECTID id, BYTE currentAction)
{
	//-----------------------------------------------------
	// delay°¡ ÀÖÀ¸¸é ¾ÈµÈ´Ù.
	//-----------------------------------------------------
	if (!IsNotDelay()
		|| HasEffectStatus( EFFECTSTATUS_ETERNITY_PAUSE )
		|| IsInCasket())	// [»õ±â¼ú3]		
	{
		return false;
	}

	//------------------------------------------------
	// ¿ÀÅä¹ÙÀÌ¸¦ Å¸°í ÀÖÀ¸¸é ¹®À» ¿­ ¼ö ¾ø´Ù.
	//------------------------------------------------
	if (m_MoveDevice == MOVE_DEVICE_RIDE)
	{
		return false;
	}

	if (IsStop()) // || m_Action!=ACTION_ATTACK)
	{
		// Zone¿¡ Á¸ÀçÇÏ´Â ItemÀÎÁö checkÇÑ´Ù.
		MImageObject*	pImageObject = m_pZone->GetImageObject(id);

		// ImageObject°¡ ¾øÀ¸¸é return
		if (pImageObject==NULL) return false;

		// InteractionObject°¡ ¾Æ´Ï¸é return
		if (pImageObject->GetObjectType()!=MObject::TYPE_INTERACTIONOBJECT)
			return false;	 	

		// ÃßÀû Á¤º¸ ¼³Á¤
		SetTraceID( id );
		m_fTrace			= FLAG_TRACE_INTERACTIONOBJECT;
		m_TraceObjectAction	= currentAction;
		m_TraceX			= pImageObject->GetX();
		m_TraceY			= pImageObject->GetY();
		m_TraceZ			= 0;

		// ÇöÀç ÃßÀûÇÏ´Â ´ë»ó¿¡ ´ëÇØ¼­ ±â¾ïÇØµÐ´Ù.
		m_fTraceBuffer	= m_fTrace;	

		//------------------------------------------------------------		
		// ÇöÀç PlayerÀÇ ´É·Â¿¡ µû¶ó¼­ TraceDistance°¡ ´Þ¶óÁú±î?
		//------------------------------------------------------------
		
		m_TraceDistance		= 1;
		
		//------------------------------------------------------------

		// ÃßÀûÇÏ´Â ±æ ¼³Á¤
		SetAction( m_MoveAction );
		SetNextDestination(m_TraceX, m_TraceY);

		// bufferingÀ» ¾ø¾Ø´Ù.
		m_fNextTrace			= FLAG_TRACE_NULL;
	
		// °è¼Ó ÂÑ¾Æ°¡´Â °ÍÀ» ÁßÁö
		m_bKeepTraceCreature	= false;

		//------------------------------------------------------------
		// messageÃâ·Â
		//------------------------------------------------------------
		DEBUG_ADD_FORMAT("Trace InteractionObject: %d", id);				
	}
	else
	{
		//------------------------------------------------------------
		// ¹Ýº¹ actionÇÏ´Â ÁßÀÌ ¾Æ´Ñ °æ¿ì¿¡ ´ÙÀ½ µ¿ÀÛ buffering
		//------------------------------------------------------------
		if (m_bRepeatAction)
		{
			m_fNextTrace	= FLAG_TRACE_NULL;

			return false;
		}
		else
		{
			m_fNextTrace			= FLAG_TRACE_INTERACTIONOBJECT;
			m_NextTraceID			= id;
			m_NextTraceObjectAction	= currentAction;

			m_bRepeatAction = FALSE;

			return false;
		}
	}

	return true;
}

//----------------------------------------------------------------------
// Trace Effect
//----------------------------------------------------------------------
// SelectableEffect¿¡ Á¢±ÙÇÑ´Ù.
//----------------------------------------------------------------------
bool	
MPlayer::TraceEffect(TYPE_OBJECTID id)
{
	//-----------------------------------------------------
	// delay°¡ ÀÖÀ¸¸é ¾ÈµÈ´Ù.
	//-----------------------------------------------------
	if (!IsNotDelay()
		|| HasEffectStatus( EFFECTSTATUS_ETERNITY_PAUSE )
		|| IsInCasket())	// [»õ±â¼ú3]
	{
		return false;
	}

	//------------------------------------------------
	// ¿ÀÅä¹ÙÀÌ¸¦ Å¸°í ÀÖÀ¸¸é ¹®À» ¿­ ¼ö ¾ø´Ù.
	//------------------------------------------------
	if (m_MoveDevice == MOVE_DEVICE_RIDE)
	{
		return false;
	}

	if (IsStop()) // || m_Action!=ACTION_ATTACK)
	{
		MEffect*	pEffect = m_pZone->GetEffect( id );

		if (pEffect==NULL)
		{
			pEffect = m_pZone->GetGroundEffect( id );

			if (pEffect==NULL)
			{
				return false;
			}
		}
		
		if (!pEffect->IsSelectable())
			return false;	 	

		// ÃßÀû Á¤º¸ ¼³Á¤
		SetTraceID( id );
		m_fTrace			= FLAG_TRACE_EFFECT;
		//m_TraceObjectAction	= currentAction;
		m_TraceX			= pEffect->GetX();
		m_TraceY			= pEffect->GetY();
		m_TraceZ			= 0;

		// ÇöÀç ÃßÀûÇÏ´Â ´ë»ó¿¡ ´ëÇØ¼­ ±â¾ïÇØµÐ´Ù.
		m_fTraceBuffer	= m_fTrace;	

		//------------------------------------------------------------		
		// ÇöÀç PlayerÀÇ ´É·Â¿¡ µû¶ó¼­ TraceDistance°¡ ´Þ¶óÁú±î?
		//------------------------------------------------------------
		
		m_TraceDistance		= 1;
		
		//------------------------------------------------------------

		// ÃßÀûÇÏ´Â ±æ ¼³Á¤
		SetAction( m_MoveAction );
		SetNextDestination(m_TraceX, m_TraceY);

		// bufferingÀ» ¾ø¾Ø´Ù.
		m_fNextTrace			= FLAG_TRACE_NULL;
	
		// °è¼Ó ÂÑ¾Æ°¡´Â °ÍÀ» ÁßÁö
		m_bKeepTraceCreature	= false;

		//------------------------------------------------------------
		// messageÃâ·Â
		//------------------------------------------------------------
		DEBUG_ADD_FORMAT("Trace Effect: %d", id);				
	}
	else
	{
		//------------------------------------------------------------
		// ¹Ýº¹ actionÇÏ´Â ÁßÀÌ ¾Æ´Ñ °æ¿ì¿¡ ´ÙÀ½ µ¿ÀÛ buffering
		//------------------------------------------------------------
		if (m_bRepeatAction)
		{
			m_fNextTrace	= FLAG_TRACE_NULL;

			return false;
		}
		else
		{
			m_fNextTrace			= FLAG_TRACE_EFFECT;
			m_NextTraceID			= id;
			//m_NextTraceObjectAction	= currentAction;

			m_bRepeatAction = FALSE;

			return false;
		}
	}

	return true;
}



//----------------------------------------------------------------------
// Set Trace Position
//----------------------------------------------------------------------
// Creature°¡ ÀÖ´Â sector´Â ÀÌ¹Ì Creature°¡ ÀÖ±â ¶§¹®¿¡ °¥ ¼ö ¾ø´Ù.
// ±×·¯¹Ç·Î, Creature¿¡ °¡Àå ±ÙÁ¢ÇÏ¸é¼­ ÇöÀç Player¿¡¼­ °¡±î¿î 
// Sector¸¦ ¼±ÅÃÇØ¾ß ÇÑ´Ù.
//----------------------------------------------------------------------
/*
void
MPlayer::SetTraceDestination()
{
	// checkÇÏ´Â ¹æÇâÀÇ ¼ø¼­¸¦ µÐ´Ù.
	int		stepX = m_X - m_TraceX,
			stepY = m_Y - m_TraceY;
	int		signX = (stepX==0)? 0 : (stepX<0)? -1 : 1,
			signY = (stepY==0)? 0 : (stepY<0)? -1 : 1;

	int		check[8];

	// signX,signY·Î 
	switch (DetermineDirection(signX, signY))
	{
		case RIGHT :
			
		break;
	}
	

	//-------------------------------------------------------
	// 8¹æÇâÀ» ¸ðµÎ check
	//-------------------------------------------------------
	for (int i=0; i<8; i++)
	{		
		switch (i)
		{
			case DIRECTION_LEFT			: next.x--;	next.y++;	break;
			case DIRECTION_RIGHT		: next.x++;	next.y--;	break;
			case DIRECTION_UP			: next.x--;	next.y--;	break;
			case DIRECTION_DOWN			: next.x++;	next.y++;	break;
			case DIRECTION_LEFTUP		: next.x--;				break;
			case DIRECTION_LEFTDOWN		: next.y++;				break;
			case DIRECTION_RIGHTUP		: next.y--;				break;
			case DIRECTION_RIGHTDOWN	: next.x++;				break;
		}

		//-------------------------------------------------------
		// ZoneÀÇ ¿µ¿ª ¹ÛÀÌ¸é check ¾ÈÇÑ´Ù.
		//-------------------------------------------------------
		if (next.x<0 || next.y<0 
					|| next.x>=m_pZone->GetWidth() || next.y>=m_pZone->GetHeight()) continue;

		//-------------------------------------------------------
		// °¥ ¼ö ÀÖÀ¸¸é ¼±ÅÃÇÑ´Ù.
		//-------------------------------------------------------
		if (m_pZone->CanMove(next.x, next.y))
		{				
		}
	}
}
*/			

//----------------------------------------------------------------------
// Can Attack Tribe
//----------------------------------------------------------------------
// ¾î¶² Á¾Á·À» °ø°ÝÇÒ ¼ö ÀÖ´Â AttackModeÀÎÁö¸¦ ¾Ë¾Æº»´Ù.
//----------------------------------------------------------------------
bool
MPlayer::CanAttackTribe( enum CREATURETRIBE to ) const
{
	switch ( m_AttackMode )
	{
		//--------------------------------------------
		// ÆòÈ­»óÅÂ : ¾Æ¹«µµ °ø°Ý ¾È ÇÑ´Ù.
		//--------------------------------------------
		case ATTACK_MODE_PEACE :
			return false;
		break;

		//--------------------------------------------
		// °ø°Ý»óÅÂ : ¾Æ¹«³ª °ø°Ý ÇÑ´Ù.
		//--------------------------------------------
		case ATTACK_MODE_AGGRESS :
			return true;
		break;

		//--------------------------------------------
		// º¸Åë : Àû¸¸ °ø°ÝÇÑ´Ù.
		//--------------------------------------------
		// Àû = Tribe°¡ °°Áö ¾ÊÀº °æ¿ì..
		// NPC´Â °ø°Ý ¸øÇÑ´Ù.
		case ATTACK_MODE_NORMAL :
			return GetCreatureTribe()!=to;// && to!=CREATURETRIBE_NPC;
		break;		
	}

	return false;
}

//----------------------------------------------------------------------
// Can Attack Guild
//----------------------------------------------------------------------
// ¾î¶² guild¸¦ °ø°ÝÇÒ ¼ö ÀÖ´Â AttackModeÀÎÁö¸¦ ¾Ë¾Æº»´Ù.
//----------------------------------------------------------------------
bool	
MPlayer::CanAttackGuild( const MCreature* pCreature ) const
{
	switch ( m_AttackMode )
	{
		//--------------------------------------------
		// ÆòÈ­»óÅÂ : ¾Æ¹«µµ °ø°Ý ¾È ÇÑ´Ù.
		//--------------------------------------------
		case ATTACK_MODE_PEACE :
			return false;
		break;

		//--------------------------------------------
		// °ø°Ý»óÅÂ : ¾Æ¹«³ª °ø°Ý ÇÑ´Ù.
		//--------------------------------------------
		case ATTACK_MODE_AGGRESS :
			return true;
		break;

		//--------------------------------------------
		// º¸Åë : Àû¸¸ °ø°ÝÇÑ´Ù.
		//--------------------------------------------
		// Àû = Guild°¡ °°Áö ¾ÊÀº °æ¿ì..
		case ATTACK_MODE_NORMAL :
		{
			int gn = pCreature->GetGuildNumber();

			return m_GuildNumber!=gn
					// ÇöÀç´Â »ç¿ëÀÚ³¢¸®´Â °ø°ÝÇÏ¸é ¾ÈµÇ±â ¶§¹®¿¡..
					// »ç¿ëÀÚ ±æµå°¡ ¾Æ´Ñ °æ¿ì¸¸ °ø°ÝÇÒ ¼ö ÀÖ´Ù.
					// ³ªÁß¿¡´Â ÀÌ°Å ºüÁ®¾ßµÇÁö ¾ÊÀ»±î?

					// vampireÀÎ °æ¿ì. ÀÏ´Ü vampire¸¸ AttackByGuild¸¦ »ç¿ëÇÏ±â ¶§¹®
					&& gn!=GUILDID_VAMPIRE_DEFAULT && gn<MIN_USER_GUILDID
					|| GetCreatureTribe()!=pCreature->GetCreatureTribe();
					// && to!=CREATURETRIBE_NPC;
		}
		break;		
	}

	return false;
}

//----------------------------------------------------------------------
// Action In Trace Distance
//----------------------------------------------------------------------
// Çàµ¿ÀÌ °¡´ÉÇÑ °Å¸®¿¡ ÀÖÀ» ¶§ÀÇ Çàµ¿ Ã³¸®
//
// return true : °Å¸®¿¡ ÀÖÀ» ¶§
//		  false : °Å¸® ¹Û.
//----------------------------------------------------------------------
bool
MPlayer::ActionInTraceDistance()
{
	int nextAction;
	if (m_nUsedActionInfo==ACTIONINFO_NULL)
	{
		if (m_MoveDevice==MOVE_DEVICE_RIDE)
		{
			if (IsSlayer())
			{
				nextAction = ACTION_SLAYER_MOTOR_STAND;				
			}
			else
			{
				// Äá°¡·ç~~..
				//nextAction = ACTION_STAND;
			}
		}
		//-----------------------------------------------------
		// °É¾î´Ù´Ò¶§
		//-----------------------------------------------------
		else if( m_MoveDevice == MOVE_DEVICE_SUMMON_SYLPH )
		{
			if( IsOusters() )
			{
				nextAction = ACTION_OUSTERS_FAST_MOVE_STAND;
			}
		}
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
		else if(m_MoveDevice == MOVE_DEVICE_WING_SYLPH)
		{
			if( IsOusters() )
			{
#if __CONTENTS(__SECOND_TRANSFORTER)
				if(0 == GetWingItemType())
#endif //__SECOND_TRANSFORTER
					nextAction = ACTION_OUSTERS_WING_STAND;
#if __CONTENTS(__SECOND_TRANSFORTER)
				else if(1 == GetWingItemType())
					nextAction = ACTION_OUSTERS_UNICORN_STAND;
				else if(2 == GetWingItemType())
					nextAction = ACTION_OUSTERS_UNICORN_STAND;
				else if(3 == GetWingItemType())
					nextAction = ACTION_OUSTERS_UNICORN_STAND;
#endif //__SECOND_TRANSFORTER
			}
		}
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
		else
		{
			nextAction = ACTION_STAND;
		}
	}
	else
	{
		nextAction = GetActionInfoAction(m_nUsedActionInfo);
	}

	//-------------------------------------------------------
	//
	// [ ÃßÀû ¿Ï·á ] 
	//
	// : »çÁ¤ °Å¸® ÀÌ³»ÀÎ °æ¿ì --> ÃßÀû ÁßÁö --> ÃßÀû Action
	//
	//-------------------------------------------------------			
	if (max(abs(m_X-m_TraceX), abs(m_Y-m_TraceY)) 
		<= m_TraceDistance)
	{	
		//----------------------------------------------------
		//
		// »çÁ¤°Å¸®¿¡ Á¢±ÙÇßÀ¸¹Ç·Î Action ModeÀüÈ¯				
		//
		//----------------------------------------------------
				
		//----------------------------------------------------
		// ÃßÀûÀÌ ¿Ï·áµÆÀ¸¹Ç·Î ÀûÀýÇÑ Çàµ¿À» ÃëÇÏ°Ô ÇÑ´Ù.
		// m_NextAction¿¡ ÀûÀýÇÑ Çàµ¿À» ¼³Á¤..
		//----------------------------------------------------
		//---------------------
		// ÀÚ½Å¿¡°Ô Çàµ¿
		//---------------------
		if (m_fTrace & FLAG_TRACE_SELF)
		{
			//---------------------------------------------------------------
			// ÇöÀç ¼±ÅÃµÈ Æ¯¼ö±â¼ú¿¡ µû¶ó¼­..
			//---------------------------------------------------------------					
			//m_ActionCount = m_ActionCountMax;
			m_MoveCount = m_MoveCountMax;					
			SetNextAction(nextAction);

			//--------------------
			// message
			//--------------------
			DEBUG_ADD("Do Self SpecialAction");							
			
			return true;
		}
		//----------------------------------------------------
		//
		//					±âº» Çàµ¿
		//
		//----------------------------------------------------
		else if (m_fTrace & FLAG_TRACE_CREATURE_BASIC)
		{
			//---------------------------------------------------------------
			// ÇöÀç »ç¿ëÁßÀÎ ¹«±â¿¡ µû¶ó¼­...
			//---------------------------------------------------------------
			// pCreature
			// m_TraceX, m_TraceY°¡ ¸ñÀûÁö
			//---------------------------------------------------------------
			//m_ActionCount = m_ActionCountMax;
			m_MoveCount = m_MoveCountMax;

			BasicActionToCreature();				

			return true;
		}
		//----------------------------------------------------
		//
		//						Æ¯¼ö Çàµ¿
		//
		//----------------------------------------------------
		else if (m_fTrace & FLAG_TRACE_CREATURE_SPECIAL)
		{
			//---------------------------------------------------------------
			// Slot¿¡ ¼³Á¤µÈ Æ¯¼ö°ø°Ý(°ø°ÝÀÌ ¾Æ´Ò¼öµµ ÀÖ°ÚÁö)¿¡ µû¶ó¼­..
			//---------------------------------------------------------------
			// pCreature
			// m_TraceX, m_TraceY°¡ ¸ñÀûÁö
			//---------------------------------------------------------------
			//m_ActionCount = m_ActionCountMax;
			m_MoveCount = m_MoveCountMax;
			SetNextAction(nextAction);

			/*
			#ifdef	CONNECT_SERVER
				CGAttackNormal _CGAttackNormal;
				_CGAttackNormal.setX( m_X );
				_CGAttackNormal.setY( m_Y );
				_CGAttackNormal.setDir( m_Direction );
				_CGAttackNormal.setObjectID( m_TraceID );
				g_pSocket->sendPacket( &_CGAttackNormal );

				#ifdef	OUTPUT_DEBUG
					DEBUG_ADD( _CGAttackNormal.toString().c_str() );							
				#endif
			#endif
			*/

			//--------------------
			// message
			//--------------------
			DEBUG_ADD_FORMAT("Special Attack : %d", m_TraceID);							
			
			return true;
		}
		//----------------------------------------------------
		//
		//				±âº» Çàµ¿ -> Sector
		//
		//----------------------------------------------------
		else if (m_fTrace & FLAG_TRACE_SECTOR_BASIC)
		{
			//---------------------------------------------------------------
			// ÇöÀç »ç¿ëÁßÀÎ ¹«±â¿¡ µû¶ó¼­...
			//---------------------------------------------------------------
			// pCreature
			// m_TraceX, m_TraceY°¡ ¸ñÀûÁö
			//---------------------------------------------------------------
			//m_ActionCount = m_ActionCountMax;
			m_MoveCount = m_MoveCountMax;
			// FAZ 4 (2026-05-11 it.2) Bug 11: Sector-basic gun-attack mag guard
			// nextAction = GetActionInfoAction(m_nUsedActionInfo) line 5294'te hesaplandi.
			// m_nUsedActionInfo gun-type ise + magazine NULL/0 ise gun-attack animasyonu OYNAMA.
			bool bug11_sb_skipFire = false;
			if (m_nUsedActionInfo >= 0 && m_nUsedActionInfo < (int)g_pActionInfoTable->GetSize() &&
				(*g_pActionInfoTable)[m_nUsedActionInfo].IsWeaponTypeGunAny())
			{
				if (g_pCurrentMagazine == NULL ||
					(g_pCurrentMagazine != NULL && g_pCurrentMagazine->GetNumber() == 0))
				{
					bug11_sb_skipFire = true;
					CT_Done("AITD-SB: Bug11 skip gun fire (mag empty) action=%d", (int)m_nUsedActionInfo);
					PlaySound(SOUND_ITEM_NO_MAGAZINE);
					m_DelayTime = g_CurrentTime + GetActionInfoDelay(m_nBasicActionInfo);
					m_fNextTrace = FLAG_TRACE_NULL;
					m_nUsedActionInfo = ACTIONINFO_NULL;
					UnSetRepeatAction();
					SetNextAction(ACTION_STAND);
				}
			}
			if (!bug11_sb_skipFire)
			{
				SetNextAction(nextAction);
			}

			/*
			#ifdef	CONNECT_SERVER
				CGAttackNormal _CGAttackNormal;
				_CGAttackNormal.setX( m_X );
				_CGAttackNormal.setY( m_Y );
				_CGAttackNormal.setDir( m_Direction );
				_CGAttackNormal.setObjectID( m_TraceID );
				g_pSocket->sendPacket( &_CGAttackNormal );

				#ifdef	OUTPUT_DEBUG
					DEBUG_ADD( _CGAttackNormal.toString().c_str() );							
				#endif
			#endif
			*/

			//--------------------
			// message
			//--------------------
			DEBUG_ADD_FORMAT("Basic Attack Sector : (%d, %d)", m_TraceX, m_TraceY);
			
			return true;
		}
		//----------------------------------------------------
		//
		//				Æ¯¼ö Çàµ¿ -> Sector
		//
		//----------------------------------------------------
		else if (m_fTrace & FLAG_TRACE_SECTOR_SPECIAL)
		{
			//---------------------------------------------------------------
			// Slot¿¡ ¼³Á¤µÈ Æ¯¼ö°ø°Ý(°ø°ÝÀÌ ¾Æ´Ò¼öµµ ÀÖ°ÚÁö)¿¡ µû¶ó¼­..
			//---------------------------------------------------------------
			// pCreature
			// m_TraceX, m_TraceY°¡ ¸ñÀûÁö
			//---------------------------------------------------------------
			//m_NextAction = ACTION_SHOOT;
			//m_ActionCount = m_ActionCountMax; 
			m_MoveCount = m_MoveCountMax;
			SetNextAction(nextAction);

			/*
			#ifdef	CONNECT_SERVER
				CGAttackNormal _CGAttackNormal;
				_CGAttackNormal.setX( m_X );
				_CGAttackNormal.setY( m_Y );
				_CGAttackNormal.setDir( m_Direction );
				_CGAttackNormal.setObjectID( m_TraceID );
				g_pSocket->sendPacket( &_CGAttackNormal );

				#ifdef	OUTPUT_DEBUG
					DEBUG_ADD( _CGAttackNormal.toString().c_str() );							
				#endif
			#endif
			*/

			//--------------------
			// message
			//--------------------
			DEBUG_ADD_FORMAT("Special Attack Sector : (%d, %d)", m_TraceX, m_TraceY);							
			//*/

			return true;
		}
		//----------------------------------------------------
		//
		//					Item ÃßÀû
		//
		//----------------------------------------------------
		else if (m_fTrace & FLAG_TRACE_ITEM)
		{
			MItem*	pItem = m_pZone->GetItem( m_TraceID );

			// ÃßÀûÀ» ¿Ï·áÇßÀ¸¹Ç·Î ItemÀ» ÁÝ´Â´Ù.
			if (pItem!=NULL
				&& (!g_pZone->GetSector(pItem->GetX(), pItem->GetY()).HasDarkness() || IsVampire() && g_pZone->GetID() != 3001 || 
				!IsVampire() && HasEffectStatus( EFFECTSTATUS_LIGHTNESS )
				&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_FLAME_SIGH)
				&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
				&& !g_bLight
#endif
				||!IsOusters())
				)
			{
				PickupItem( pItem );
			}

			return true;
		}
		//----------------------------------------------------
		//
		//			InventoryÀÇ Item¿¡ ±â¼ú »ç¿ë
		//
		//----------------------------------------------------
		else if (m_fTrace & FLAG_TRACE_INVENTORY)
		{
			MItem* pItem = g_pInventory->GetItemToModify( m_TraceID );

			// ÃßÀûÀ» ¿Ï·áÇßÀ¸¹Ç·Î ItemÀ» ÁÝ´Â´Ù.
			if (pItem!=NULL)
			{
				// ActionEffect()¸¦ È£ÃâÇÏ±â À§ÇØ¼­...
				m_MoveCount = m_MoveCountMax;
				SetNextAction(nextAction);
			}
			else
			{
				DEBUG_ADD_FORMAT("No such Item in Inventory: id=%d", m_TraceID);
			}
			
			return true;
		}
		//----------------------------------------------------
		//
		//			EffectÃßÀû
		//
		//----------------------------------------------------
		else if (m_fTrace & FLAG_TRACE_EFFECT)
		{
			MEffect* pEffect = m_pZone->GetEffect( m_TraceID );

			if (pEffect==NULL)
			{
				pEffect = m_pZone->GetGroundEffect( m_TraceID );

				if (pEffect==NULL)
				{
					return true;
				}
			}

			if (pEffect->IsSelectable())
			{		
					MEffectTarget* pEffectTarget = pEffect->GetEffectTarget();

					if (pEffectTarget!=NULL)
					{
						bool bSendPacket = true;
						if (pEffectTarget->GetEffectTargetType()==MEffectTarget::EFFECT_TARGET_PORTAL)
						{					
							if (IsSlayer())
							{
								bSendPacket = false;
							}
						}
						
						MPortalEffectTarget* pPortalEffectTarget
							= dynamic_cast<MPortalEffectTarget*>( pEffectTarget );

						if ( bSendPacket && pPortalEffectTarget != NULL )
						{
							int serverID = pEffectTarget->GetServerID();

							CGSelectTileEffect _CGSelectTileEffect;
							_CGSelectTileEffect.setEffectObjectID( serverID );
							
							g_pSocket->sendPacket( &_CGSelectTileEffect );
						}
					}
			}

			return true;
		}	
		//----------------------------------------------------
		//
		//			InteractionObjectÃßÀû
		//
		//----------------------------------------------------
		else if (m_fTrace & FLAG_TRACE_INTERACTIONOBJECT)
		{
			MImageObject* pImageObject = m_pZone->GetImageObject( m_TraceID );

			if (pImageObject->GetObjectType()==MObject::TYPE_INTERACTIONOBJECT)
			{
				MInteractionObject*	pInteractionObject = (MInteractionObject*)pImageObject;
				pInteractionObject->SetNextAction( m_TraceObjectAction );
				
			}

			return true;
		}				

		return false;
	}
	//-------------------------------------------------------
	// ´õ ÃßÀûÇØ¾ß ÇÏ´Â °æ¿ì
	//-------------------------------------------------------
	else
	{
		//------------------------------------------
		// ÃßÀûÇÒ ±æÀÌ ¾ø´Â °æ¿ì --> ±æÃ£±â
		//------------------------------------------
		if (m_listDirection.empty())
		{
			//---------------------
			// ±âº» Çàµ¿
			//---------------------
			if (m_fTrace & FLAG_TRACE_CREATURE_BASIC)
			{
				TraceCreatureToBasicAction( m_TraceID, m_bTraceCreatureToForceAttack );
			}
			//---------------------
			// Æ¯¼ö Çàµ¿
			//---------------------
			else  if (m_fTrace & FLAG_TRACE_CREATURE_SPECIAL)
			{
				TraceCreatureToSpecialAction( m_TraceID, m_bTraceCreatureToForceAttack );
			}
			//---------------------
			// ±âº» Çàµ¿ -> Sector
			//---------------------
			else if (m_fTrace & FLAG_TRACE_SECTOR_BASIC)
			{
				TraceSectorToBasicAction( m_TraceX, m_TraceY );
			}
			//---------------------
			// Æ¯¼ö Çàµ¿ -> Sector
			//---------------------
			else if (m_fTrace & FLAG_TRACE_SECTOR_SPECIAL)
			{
				// 2004, 9, 18, sobeit add start - ÅÍ·¿ ÆÄÀÌ¾î´Â »çÁ¤°Å¸® ¹þ¾î³ª¹È ¹«½Ã ¤Ì.¤Ì 
				if (m_nUsedActionInfo==SKILL_TURRET_FIRE)
				{
					m_fNextTrace = FLAG_TRACE_NULL;
					UnSetRepeatAction();
					InstallTurretStopAttack();
					//SetAction(ACTION_STAND);
					return false;
				}
				else if (m_nUsedActionInfo==SKILL_CLINT_ADV2_HALCYON_MK1)
				{
					m_fNextTrace = FLAG_TRACE_NULL;
					UnSetRepeatAction();
					//InstallTurretStopAttack();
					//SetAction(ACTION_STAND);
					return false;
				}
				else if (m_nUsedActionInfo==SKILL_ADV2_HALCYON_MK1_NAPALN_BULLET || m_nUsedActionInfo==SKILL_CLIENT_ADV2_HALCYON_MK1)
				{
					m_fNextTrace = FLAG_TRACE_NULL;
					UnSetRepeatAction();
					//InstallTurretStopAttack();
					//SetAction(ACTION_STAND);
					return false;
				}
				// 2004, 9, 18, sobeit add end
				TraceSectorToSpecialAction( m_TraceX, m_TraceY );
			}
			//---------------------
			// Item ÃßÀû
			//---------------------
			else if (m_fTrace & FLAG_TRACE_ITEM)
			{
				TraceItem( m_TraceID );
			}
			//---------------------
			// Inventory¿¡ ±â¼ú »ç¿ë
			//---------------------
			else if (m_fTrace & FLAG_TRACE_INVENTORY)
			{
				TraceInventoryItem( m_TraceID );
			}
			//---------------------
			// EffectÃßÀû
			//---------------------
			else if (m_fTrace & FLAG_TRACE_EFFECT)
			{
				TraceEffect( m_TraceID );
			}
			//---------------------
			// InteractionObjectÃßÀû
			//---------------------
			else if (m_fTrace & FLAG_TRACE_INTERACTIONOBJECT)
			{
				TraceInteractionObject( m_TraceID, m_TraceObjectAction );
			}

			// ÀÌµ¿~ÇÏ¸é¼­ ÃßÀû..
			//SetAction( m_MoveAction );
		}


		//m_NextAction = m_MoveAction;
		SetNextAction( m_MoveAction );
	}

	return false;
}

//----------------------------------------------------------------------
// Move
//----------------------------------------------------------------------
// MCreature::Move¿Í ºñ½ÁÇÏÁö¸¸ MPlayer::Move´Â 
//    m_listDirection¿¡¼­ ¹æÇâÀ» ÇÏ³ª¾¿ ÀÐ¾î¼­ ¿òÁ÷ÀÌ°Ô µÈ´Ù.
// 
// ´Ü, ±æÀ» °¡´Ù°¡ ¸·È÷¸é ´Ù½Ã ±æÀ» Á¤ÇØ¾ßÇÑ´Ù(SetDest...)
//----------------------------------------------------------------------
void	
MPlayer::ActionMove()
{
	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD("ActionMove");
	#endif

	//----------------------------------------------------
	//
	// ´ÙÀ½ Sector¿¡ µµ´ÞÇÑ °æ¿ì	
	//
	//----------------------------------------------------
	// m_listDirection¿¡¼­ ¹æÇâÀ» ÇÏ³ª ÀÐ¾î¿Í¼­ 
	// checkÇØº¸°í ¿òÁ÷ÀÎ´Ù.
	//----------------------------------------------------
	if( HasEffectStatus( EFFECTSTATUS_TRYING ) )
	{
		MZone::EFFECT_MAP::const_iterator itr = g_pZone->GetGroundEffects();
		bool bFind = false;

		for(int i = 0; i < g_pZone->GetGroundEffectCount(); itr++,i++ )
		{
			const MEffect* pEffect = itr->second;
			
			if( GetX() == pEffect->GetX() &&
				GetY() == pEffect->GetY() &&
				pEffect->GetFrameID() == EFFECTSPRITETYPE_REGEN_TOWER_GROUND )
			{
				bFind = true;
				break;
			}
		}
		if( bFind == false )
			RemoveEffectStatus( EFFECTSTATUS_TRYING );
	}

	if (m_MoveCount==0)//m_sX==0 && m_sY==0)
	{
		TYPE_SECTORPOSITION	x, y;

		CT_BAT("ActionMove STEP-ENTRY: ctype=%d pos=(%d,%d) dest=(%d,%d) nextDest=(%d,%d) dirList=%d sendMove=%d fastMove=%d moveCountMax=%d moveDevice=%d",
			(int)m_CreatureType, (int)m_X, (int)m_Y, (int)m_DestX, (int)m_DestY, (int)m_NextDestX, (int)m_NextDestY,
			(int)m_listDirection.size(), (int)m_SendMove, (int)m_bFastMove, (int)m_MoveCountMax, (int)m_MoveDevice);

		//-------------------------------------------------------------
		// fast moveÀÎ °æ¿ì´Â nextX, nextY¿¡ ÁÂÇ¥°¡ ÀÖ´Ù.
		//-------------------------------------------------------------
		if (m_bFastMove)
		{
			x = m_NextX;
			y = m_NextY;

			m_NextX = SECTORPOSITION_NULL;					

			//m_CurrentDirection = 
			m_Direction = GetDirectionToPosition( x, y );

		}
		//-------------------------------------------------------------
		// ÀÏ¹ÝÀûÀÎ ¿òÁ÷ÀÓ
		//-------------------------------------------------------------
		else
		{
			//----------------------------------------------------
			// ¸ñÇ¥À§Ä¡°¡ ¾øÀ¸¸é ¿òÁ÷ÀÌÁö ¾Ê´Â´Ù.
			//----------------------------------------------------
			if (m_DestX==SECTORPOSITION_NULL && m_NextDestX==SECTORPOSITION_NULL)
			{
				DEBUG_ADD("### Don't need to move any more - Move Stop! ###");
				Phoenix_MovementAuditWrite("MOVE_ACTION_STOP", "NO_DESTINATION", m_X, m_Y, m_DestX, m_DestY, m_Direction, m_SendMove);
				CT_BAT("ActionMove RETURN: NO_DESTINATION (dest ve nextDest bos)");

				m_MoveCount = m_MoveCountMax;
				return;
			}

				//sprintf(g_pDebugMessage->GetCurrent(), "ListSize = %d", m_listDirection.size());
			
			//----------------------------------------------------
			// 
			// 1. ±æÃ£±â¸¦ ÇØ¼­ ÀÌµ¿ÇÏ°Ô ÇÏ°í..
			// 2. Server¿¡¼­ °ËÁõ ¹Þ±â À§ÇØ Send..
			//
			//----------------------------------------------------
			// Server·ÎºÎÅÍ ±â´Ù¸®´Â messageÀÇ °³¼ö Ãâ·Â		
			if (m_SendMove != 0  && g_bNetStatusGood)
			{
				static int lastSendCount = 0;

				if (lastSendCount != m_SendMove)
				{
					DEBUG_ADD_FORMAT("Waiting MoveOK = %d", m_SendMove);						
					
					lastSendCount = m_SendMove;
				}
			}

			//----------------------------------------------------
			//
			//
			//                 ÃßÀû ÁßÀÌ¸é..
			//
			//
			//----------------------------------------------------		
			if (m_fTrace != FLAG_TRACE_NULL)
			{				
				//----------------------------------------------------	
				// Creature ÃßÀû ÁßÀÌ¸é..
				// Creature°¡ »ç¶óÁö°Å³ª
				// ÃßÀû ¸ñÇ¥ÁÂÇ¥°¡ ¹Ù²ð ¼ö ÀÖÀ¸¹Ç·Î checkÇØÁØ´Ù.
				//----------------------------------------------------	
				if ((m_fTrace & FLAG_TRACE_CREATURE_BASIC) 
					|| (m_fTrace & FLAG_TRACE_CREATURE_SPECIAL))
				{
					// ÃßÀûÇÏ´Â CreatureÀÇ Á¤º¸¸¦ ÀÐ¾î¿Â´Ù.
					MCreature*	pCreature = m_pZone->GetCreature( m_TraceID );				

					//GET_DYING_CREATURE( pCreature, m_TraceID );

					//-------------------------------------------------------
					// ÃßÀûÇÏ´Â Creature°¡ »ç¶óÁ³À» °æ¿ì --> ÃßÀû ÁßÁö
					// ³»°¡ SlayerÀÎ °æ¿ì´Â Darkness¾È¿¡ µé¾î°£ Ä³¸¯À» ÂÑ¾Æ°¥ ¼ö ¾ø´Ù.					
					//-------------------------------------------------------								
					if (pCreature==NULL 
						|| pCreature->IsInDarkness() && !pCreature->IsNPC() && 
						(!IsVampire() && !HasEffectStatus( EFFECTSTATUS_LIGHTNESS )  || 
						g_pZone->GetID() == 3001 && IsVampire() ||
						pCreature->IsOusters() && pCreature->IsInGroundElemental() && !g_pPlayer->IsOusters())
						&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_FLAME_SIGH)
						&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )						
#ifdef __METROTECH_TEST__
						&& !g_bLight
#endif
						) 
					{
						if (IsRepeatAction())
						{
							m_fNextTrace = FLAG_TRACE_NULL;

							// ¹öÆ°À» ¶¼¾úÀ¸¹Ç·Î Çàµ¿ ¹Ýº¹À» Ãë¼ÒÇÑ´Ù.
							UnSetRepeatAction();
							//TraceNextNULL();
						}
						else
						{
							// ÃßÀû ÁßÁö
							TraceNULL();
								
							// ¸ØÃá´Ù.
							SetStop();					
						}

						#ifdef OUTPUT_DEBUG_PLAYER_ACTION
							DEBUG_ADD("creature lost");
						#endif
							
						return;			
					}
				
					//-------------------------------------------------------			
					// ÃßÀûÇÒ·Á´Â CreatureÀÇ ÁÂÇ¥°¡ ´Þ¶óÁ³À» °æ¿ì
					//-------------------------------------------------------			
					if (pCreature->GetX()!=m_TraceX ||
						pCreature->GetY()!=m_TraceY)
					{										
						m_NextDestX = pCreature->GetX();
						m_NextDestY = pCreature->GetY();
						// Z´Â °ü°è¾ø´Ù.
					}

					m_TraceX	= pCreature->GetX();
					m_TraceY	= pCreature->GetY();
					m_TraceZ	= pCreature->GetZ();
				}
				//----------------------------------------------------	
				// Item ÃßÀû ÁßÀÌ¸é..
				// ItemÀÌ »ç¶óÁú ¼ö ÀÖÀ¸¹Ç·Î checkÇØÁØ´Ù.
				//----------------------------------------------------
				else if (m_fTrace & FLAG_TRACE_ITEM)
				{
					// ÃßÀûÇÏ´Â CreatureÀÇ Á¤º¸¸¦ ÀÐ¾î¿Â´Ù.
					MItem*	pItem = m_pZone->GetItem( m_TraceID );

					//-------------------------------------------------------
					// ÃßÀûÇÏ´Â ItemÀÌ »ç¶óÁ³À» °æ¿ì --> ÃßÀû ÁßÁö
					//-------------------------------------------------------
					if (pItem==NULL
						|| g_pZone->GetSector(pItem->GetX(), pItem->GetY()).HasDarkness()&& 
						(!IsVampire() && (!HasEffectStatus( EFFECTSTATUS_LIGHTNESS ) ||
						IsVampire() &&g_pZone->GetID() == 3001) )
						&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_FLAME_SIGH)
						&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
						&& !g_bLight
#endif
						) 
					{
						// ÃßÀû ÁßÁö
						TraceNULL();
							
						// ¸ØÃá´Ù.
						SetStop();					
							
						#ifdef OUTPUT_DEBUG_PLAYER_ACTION
							DEBUG_ADD("item lost");
						#endif

						return;			
					}
				}

				//-------------------------------------------------------
				// Çàµ¿ °¡´É °Å¸®¿¡ ÀÖ´ÂÁö ÆÇ´ÜÇÑ´Ù.
				//-------------------------------------------------------
				if (ActionInTraceDistance())
				{
					// ±â¼ú »ç¿ë~~
					if (m_nNextUsedActionInfo!=ACTIONINFO_NULL)
					{
						m_nUsedActionInfo = m_nNextUsedActionInfo;
						m_nNextUsedActionInfo = ACTIONINFO_NULL;

						// FAZ 4 (2026-05-11) Bug 11: Gun mermisi yoksa atış animasyonu OYNAMA
						// SKILL_ATTACK_GUN_*/IsWeaponTypeGunAny() + Magazine==0 ise
						// PlaySound(NO_MAGAZINE) + SetNextAction(ACTION_STAND) + RepeatAction iptal
						// Repeat-fire akışında trace setup'tan sonra mermi bitti ise burada yakalanır.
						// Race-parity: Slayer Gun + Ousters Chakram (gun-type aksiyon kullanmaz; chakram melee).
						bool bug11_skipFire = false;
						if (m_nUsedActionInfo >= 0 && m_nUsedActionInfo < (int)g_pActionInfoTable->GetSize() &&
							(*g_pActionInfoTable)[m_nUsedActionInfo].IsWeaponTypeGunAny())
						{
							if (g_pCurrentMagazine == NULL ||
								(g_pCurrentMagazine != NULL && g_pCurrentMagazine->GetNumber() == 0))
							{
								bug11_skipFire = true;
								PlaySound(SOUND_ITEM_NO_MAGAZINE);
								m_DelayTime = g_CurrentTime + GetActionInfoDelay(m_nBasicActionInfo);
								m_fNextTrace = FLAG_TRACE_NULL;
								m_nUsedActionInfo = ACTIONINFO_NULL;
								UnSetRepeatAction();
								SetNextAction(ACTION_STAND);
							}
						}

						// ´ÙÀ½ action¼³Á¤
						if (!bug11_skipFire)
						{
						m_bNextAction = true;
						// 2004, 11, 23, sobeit add start - ¾Æ..°¨±â°É·Á¼­..´«¿¡ ¾Ï°ÍµÎ ¾Èµé¾î¿Â´Ù..¤Ì.¤Ì
						if(m_nUsedActionInfo == SKILL_SET_AFIRE || m_nUsedActionInfo == SKILL_ADV2_SUDDEN_BLAST || m_nUsedActionInfo == SKILL_ACCELERATE_CHARGE_1 || m_nUsedActionInfo == SKILL_ACCELERATE_CHARGE_2 || m_nUsedActionInfo == SKILL_SET_AFIRE_2)
						{
							if(max(abs(m_X-m_TraceX), abs(m_Y-m_TraceY))<2)
								SetNextAction(ACTION_ATTACK);
							else
								SetNextAction(ACTION_STAND);
						}
						else
						// 2004, 11, 23, sobeit add end
							SetNextAction(GetActionInfoAction( m_nUsedActionInfo ));
						}
					}

					// ´õ ÀÌ»ó °¥ ÇÊ¿ä°¡ ¾ø´Ù.
					m_listDirection.clear();

					// ActionÁßÁö
					//m_ActionCount = m_ActionCountMax;
					m_MoveCount = m_MoveCountMax;

					// Player°¡ ¹Ù¶óº¸´Â ¹æÇâÀÌ ÇöÀç ÀûÀÌ ÀÖ´Â ¹æÇâÀÌ µÇµµ·Ï ÇÑ´Ù.				
					SetDirectionToPosition( m_TraceX, m_TraceY );
					
							
					//----------------------------------------------------			
					// °è¼Ó ÃßÀûÇÏ´Â °æ¿ì°¡ ¾Æ´Ï¸é ÃßÀû ÁßÁö
					//----------------------------------------------------
					if (m_bKeepTraceCreature)
					{
						int fTrace = m_fTrace;
						
						// Á¤Áö
						SetStop();

						m_fTrace = fTrace;
					}	
					else
					{
						TraceNULL();

						// Á¤Áö
						SetStop();
					}
				}
			
			}

			//----------------------------------------------------
			// ±æÃ£±â BufferingÇß´ø°É ¼³Á¤ÇÑ´Ù.
			//----------------------------------------------------
			if (m_NextDestX!=SECTORPOSITION_NULL)
			{
				// ±æÃ£±â
				//SetDestination(m_NextDestX, m_NextDestY);
				m_DestX		= m_NextDestX;	// TEST
				m_DestY		= m_NextDestY;	// TEST
				m_NextDestX = SECTORPOSITION_NULL;
				m_NextDestY = SECTORPOSITION_NULL;			
			}

			//----------------------------------------------------
			// ±æÃ£±â : ÇÑ Tile ÀÌµ¿½Ã¸¶´Ù ±æÃ£±â¸¦ ÇÑ´Ù.
			//----------------------------------------------------
			if (m_DestX!=SECTORPOSITION_NULL)
				SetDestination(m_DestX, m_DestY);

			//----------------------------------------------------
			// Server¿¡¼­ °ËÁõ¹Þ±â À§ÇØ¼­ Send
			//----------------------------------------------------
			//SetAction( m_MoveAction );

			
			//----------------------------------------------------
			// °ËÁõ¹ÞÁö ¾ÊÀº ¿òÁ÷ÀÓÀÌ 
			// Á¤ÇØÁø ¼ýÀÚ ÀÌÇÏÀÏ °æ¿ì¿¡¸¸ ¿òÁ÷ÀÌ°Ô ÇÑ´Ù.
			//----------------------------------------------------
			// Referans: m_SendMove > MAX_CLIENT_MOVE. Phoenix 2026-06-19 `>= 1`
			// yapınca client her tile'da GCMoveOK bekledi; LAN'da görünmez,
			// VPS RTT'de yürüyüş ve FastMove (bat/ghost/motorcycle) takılıyor.
			if (m_SendMove > Phoenix_GetResponsiveMaxPendingMoves())
			{
				g_bNetStatusGood = false;
				Phoenix_MovementAuditWrite("MOVE_ACTION_BLOCK", "MAX_CLIENT_MOVE", m_X, m_Y, m_DestX, m_DestY, m_Direction, m_SendMove);
				CT_BAT("ActionMove RETURN: PENDING_MOVE_LIMIT — sendMove=%d limit=%d (server MoveOK gelmiyor!)", (int)m_SendMove, (int)Phoenix_GetResponsiveMaxPendingMoves());

				#ifdef OUTPUT_DEBUG_PLAYER_ACTION
					DEBUG_ADD("max client move");
				#endif

				return;
			}
			else
			{
				g_bNetStatusGood = true;
			}

			//----------------------------------------------------
			// °¥ ±æÀÌ ¾ø´Â °æ¿ì
			//----------------------------------------------------
			if (m_listDirection.empty())
			{
				//--------------------------------------------
				// ¸ñÀûÁö¿¡ µµ´ÞÇÑ °æ¿ì..
				//--------------------------------------------
				if (m_DestX==m_X && m_DestY==m_Y)
				{
					m_DestX = SECTORPOSITION_NULL;
					m_DestY = SECTORPOSITION_NULL;
					Phoenix_MovementAuditWrite("MOVE_ACTION_STOP", "DEST_REACHED_NO_PATH", m_X, m_Y, m_DestX, m_DestY, m_Direction, m_SendMove);
					CT_BAT("ActionMove RETURN: DEST_REACHED_NO_PATH pos=(%d,%d)", (int)m_X, (int)m_Y);

					#ifdef OUTPUT_DEBUG_PLAYER_ACTION
						DEBUG_ADD("dest reached");
					#endif

					return;
				}

				#ifdef OUTPUT_DEBUG_PLAYER_ACTION
					DEBUG_ADD("no way");
				#endif

				Phoenix_MovementAuditWrite("MOVE_ACTION_BLOCK", "NO_PATH_DIRECTIONS", m_X, m_Y, m_DestX, m_DestY, m_Direction, m_SendMove);
				CT_BAT("ActionMove RETURN: NO_PATH_DIRECTIONS — dirList bos ama dest=(%d,%d) pos=(%d,%d)", (int)m_DestX, (int)m_DestY, (int)m_X, (int)m_Y);
				return;
			}

			
			//----------------------------------------------------	
			//
			// ¹æÇâ´ë·Î ¿òÁ÷ÀÎ´Ù.
			//
			//----------------------------------------------------			
			//----------------------------------------------------
			// m_listDirection¿¡¼­ ¹æÇâÀ» ÇÏ³ª ÀÐ¾î¿Â´Ù.
			//----------------------------------------------------
			m_Direction = m_listDirection.front();
			m_listDirection.pop_front();	
				
			//----------------------------------------------------
			// ´ÙÀ½¿¡ ±æÃ£±â¸¦ ÇØ¾ßÇÒ °æ¿ì...
			//----------------------------------------------------
			if (m_NextDestX!=SECTORPOSITION_NULL)
			{
				//m_NextAction = m_MoveAction;
				SetNextAction( m_MoveAction );
			}

			//----------------------------------------------------
			// ÀÐ¾î¿Â ¹æÇâÀ¸·Î ÇÑ Sector¸¦ ÁøÇàÇÏ°í
			// cX,cY, sX,sY¸¦ ´Ù½Ã ÁöÁ¤ÇØÁà¾ß ÇÑ´Ù.
			//----------------------------------------------------

			//----------------------------------------------------
			// ÀÌµ¿ °¡´ÉÇÑÁö check
			//----------------------------------------------------
			x = m_X;
			y = m_Y;

			switch (m_Direction)
			{
				case DIRECTION_LEFTDOWN		: x--;	y++;	break;
				case DIRECTION_RIGHTUP		: x++;	y--;	break;
				case DIRECTION_LEFTUP		: x--;	y--;	break;
				case DIRECTION_RIGHTDOWN	: x++;	y++;	break;
				case DIRECTION_LEFT			: x--;			break;
				case DIRECTION_DOWN			:		y++;	break;
				case DIRECTION_UP			:		y--;	break;
				case DIRECTION_RIGHT		: x++;			break;
			}
		}

		//--------------------------------------------------------------------
		// ¿òÁ÷ÀÏ ¼ö ÀÖÀ» °æ¿ì		
		//--------------------------------------------------------------------
		BOOL bCanMoveStep = m_pZone->CanMove(m_MoveType, x,y);
		CT_BAT("ActionMove CanMove: moveType=%d from=(%d,%d) to=(%d,%d) canMove=%d fastMove=%d",
			(int)m_MoveType, (int)m_X, (int)m_Y, (int)x, (int)y, (int)bCanMoveStep, (int)m_bFastMove);
		if (bCanMoveStep
			|| m_bFastMove		// Fast MoveÀÎ °æ¿ì´Â ¹«Á¶°Ç ÀÌµ¿ °¡´É..

#ifdef __EXPO_CLIENT__
			|| g_UserInformation.Invisible
#endif
			)
		{
			//---------------------------------------------------------------
			// Fast MoveÀÎ °æ¿ì..
			//---------------------------------------------------------------
			if (m_bFastMove)
			{
				// nothing
			}
			//---------------------------------------------------------------
			// ÀÏ¹ÝÀûÀÎ ¿òÁ÷ÀÓ --> °ËÁõ ÇÊ¿ä
			//---------------------------------------------------------------
			else
			{
				//----------------------------------------------------
				// Áö·Ú ¼³Ä¡ÁßÀÌ¸é Ãë¼ÒÇÏ±â
				//----------------------------------------------------
				if (gC_vs_ui.IsInstallMineProgress())
				{
					gC_vs_ui.EndInstallMineProgress();
				}

				CGMove _CGMove;
				_CGMove.setX( m_X );
				_CGMove.setY( m_Y );
				_CGMove.setDir( m_Direction );
				Phoenix_MovementAuditWrite("MOVE_CGMOVE_SEND", "CLIENT_SENT_CGMOVE", m_X, m_Y, m_DestX, m_DestY, m_Direction, m_SendMove);
				g_pSocket->sendPacket( &_CGMove );

				// Server·Î º¸³½ ¹æÇâÀ» ±â¾ïÇØµÐ´Ù.
				m_listSendDirection.push_back( m_Direction );
				m_SendMove++;
				Phoenix_MovementAuditWrite("MOVE_CGMOVE_PENDING", "SENDMOVE_INCREMENTED", m_X, m_Y, m_DestX, m_DestY, m_Direction, m_SendMove);
				

				//----------------------------------------------------
				// NPC¿¡°Ô ¸» °Ç »óÅÂ¿¡¼­ ¿òÁ÷ÀÌ´Â °æ¿ì..			
				//----------------------------------------------------
				if (m_WaitVerifyActionInfo==WAIT_VERIFY_NPC_ASK)
				{
					MCreature* pCreature = g_pZone->GetCreature( g_pPCTalkBox->GetNPCID() );

					if (pCreature!=NULL)
					{
						//----------------------------------------------------
						// °Å¸®°¡ ¸Ö¾îÁö¸é ´ëÈ­ ÁßÁö´ç.
						//----------------------------------------------------
						if ((max(abs(m_X-pCreature->GetX()), abs(m_Y-pCreature->GetY()))) > TRADE_DISTANCE_NPC)
						{
							// °ËÁõ Á¦°Å
							SetWaitVerifyNULL();

							// Dialog ²¨ÁØ´Ù.
							g_pUIDialog->ClosePCTalkDlg();
						}
					}
				}
			}

			//-----------------------
			// ½Ã¾ß ¹Ù²ãÁÖ±â
			//-----------------------
			//m_pZone->MoveLightSight(m_X, m_Y, m_LightSight, m_Direction);
			//m_pZone->UnSetLightSight(m_X, m_Y, m_LightSight);
			//m_pZone->SetLightSight(x, y, m_LightSight);

			// ÀÌÀü ÁÂÇ¥ ±â¾ï
			TYPE_SECTORPOSITION ox = m_X;
			TYPE_SECTORPOSITION oy = m_Y;

		// zoneÀÇ sectorÀÇ Á¤º¸¸¦ ¹Ù²ãÁØ´Ù.
		CT_BAT("ActionMove STEP-EXEC: to=(%d,%d) MovePosition cagriliyor", (int)x, (int)y);
		MovePosition( x, y );
		Phoenix_MovementAuditWrite("MOVE_LOCAL_POSITION", "MOVE_POSITION_APPLIED", m_X, m_Y, m_DestX, m_DestY, m_Direction, m_SendMove);

			//CheckInDarkness();

			//--------------------------------------------
			//
			// ÀÌµ¿ÇÑ °÷ÀÌ 'Æ÷Å»'ÀÎ °æ¿ì
			//
			//--------------------------------------------
			if (IsSlayer())
			{
				const MSector& sector = m_pZone->GetSector(x, y);
				if (sector.IsPortal())
				{
					int numPortal = sector.GetPortalSize();		
					
					if (numPortal!=0)
					{
						PORTAL_LIST::const_iterator	iPortal = sector.GetPortalBegin();

						PORTAL_INFO portalInfo = *iPortal;			

						//--------------------------------------------
						// ¸ÖÆ¼ Æ÷Å»ÀÌ¸é elevator¸¦ ¶ç¿î´Ù.
						//--------------------------------------------
						if (portalInfo.Type==MPortal::TYPE_MULTI_PORTAL)
						{
							DEBUG_ADD("[MPlayer] Run Elevator");
							
							gC_vs_ui.RunElevator();

							// ¸ñÀûÁö Ç¥½Ã¸¦ ¾ø¾Ø´Ù.
							g_pTopView->SetSelectedSectorNULL();

							// ¸ñÀûÁö ¾ø¾Ú..
							m_DestX		= SECTORPOSITION_NULL;
							m_DestY		= SECTORPOSITION_NULL;	
							m_NextDestX = SECTORPOSITION_NULL;
							m_NextDestY = SECTORPOSITION_NULL;	
						}

						iPortal++;
					}
				}
			}

			//--------------------------------------------
			// 5 FrameÀÌ ÀÖ´Ù°í ÇÒ °æ¿ì
			//--------------------------------------------
			//
			// [1] UP,DOWN,LEFT,RIGHTÀÏ ¶§,
			//
			//     Xº¯È­ : 0 16 32 48 64   (+-16)
			//     Yº¯È­ : 0  8 16 24 32   (+-8)
			// 
			//
			// [2] ´ë°¢¼±(LEFTUP,LEFTDOWN,RIGHTUP,RIGHTDOWN)À¸·Î ¿òÁ÷ÀÏ¶§,
			//
			//     Xº¯È­ : 0  8 16 24 32   (+-8)
			//     Yº¯È­ : 0  4  8 12 16   (+-4)
			//
			//--------------------------------------------
			// sX,sY : ¿òÁ÷¿©¾ßÇÒ ÀüÃ¼ pixel(ÇÑ TILE)
			// cX,cY : ÀÌµ¿ÇÏ´Â ´ÜÀ§ pixel
			//--------------------------------------------		
			//--------------------------------------------
			// Fast Move ÀÎ °æ¿ì
			//--------------------------------------------
			if (m_bFastMove)
			{
				// Á÷¼± °Å¸®·Î ¿òÁ÷ÀÏ¶§ÀÇ pixel°Å¸®
				// ex) ¿ÞÂÊ 2 --> 1 : ( 2 - 1 ) * TILE_X
				//     À§ÂÊ 2 --> 1 : ( 2 - 1 ) * TILE_Y
				m_sX = (ox - m_X) * TILE_X;
				m_sY = (oy - m_Y) * TILE_Y;
					
				//------------------------------------------------
				// ½ÇÁ¦ ÇÑ Å¸ÀÏ ÀÌµ¿ ¼ÓµµÀÇ ¹ÝÀÇ ¼Óµµ¿¡ ¸ñÀûÁö±îÁö ÀÌµ¿ÇÑ´Ù.
				//------------------------------------------------
				// ÇÑ Å¸ÀÏ ÀÌµ¿ÇÒ¶§ÀÇ Frame ¼ö
				int moveTimes_div_2 = (*g_pCreatureTable)[m_CreatureType].MoveTimes >> 1;
				// 2005, 1, 5, sobeit add start
				if(HasEffectStatus(EFFECTSTATUS_BIKE_CRASH))
				{
//					moveTimes_div_2 = moveTimes_div_2/2;
					m_MoveCountMax = m_MoveCountMax*2;
				}
				// 2005, 1, 5, sobeit add end
				m_cX = -m_sX / moveTimes_div_2;
				m_cY = -m_sY / moveTimes_div_2;

				m_MoveCount += 2;

				if (m_MoveCount > m_MoveCountMax)
				{
					m_MoveCount = m_MoveCountMax;
				}
			}
			//--------------------------------------------	
			// Á¤»ó ÀÌµ¿
			//--------------------------------------------
			else
			{	
				m_DirectionMoved = m_Direction;

				m_sX = m_sXTable[m_DirectionMoved];
				m_sY = m_sYTable[m_DirectionMoved];

				//????????????????????????????????????????????????????????
				//???                                                  ???
				//???   ÇÑ FrameÀ» ÀÌµ¿...ÇØ¾ßÇÏ´Â°¡?? ¸»¾Æ¾ß ÇÏ´Â°¡   ???
				//???                                                  ???
				//????????????????????????????????????????????????????????
				int moveTimes_1;
				
				#ifdef OUTPUT_DEBUG
					if (m_CreatureType >= g_pCreatureTable->GetSize())
					{
						DEBUG_ADD_FORMAT("[Error] CreatureType is exceed MAX. id=%d, type=%d", m_ID, m_CreatureType);
						
						SetStop();

						return;
					}
				#endif

				if (m_MoveDevice==MOVE_DEVICE_WALK)
				{
					moveTimes_1 = (*g_pCreatureTable)[m_CreatureType].MoveTimes-1;
				}
				else if (m_MoveDevice==MOVE_DEVICE_RIDE)
				{
					moveTimes_1 = (*g_pCreatureTable)[m_CreatureType].MoveTimesMotor-1;
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
//½½·¹ÀÌ¾îÀÇ ½Å±ÔÀÌµ¿¼ö´Ü(ÇïÆÄÀÌ¾î)À» Å¸°í ÀÖÀ» °æ¿ì -- moveTimes¸¦ °­Á¦·Î 1ÁÙÀÎ´Ù. Áï, °­Á¦·Î 4ÇÁ·¹ÀÓÀÚ¸®¸¦ 3ÇÁ·¹ÀÓÀ¸·Î...
					if(IsRide_Fast_Transforter())
						moveTimes_1--;
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
				}
				else if ((m_MoveDevice==MOVE_DEVICE_SUMMON_SYLPH || HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this)))
#if __CONTENTS(__FAST_TRANSFORTER)
					||(m_MoveDevice==MOVE_DEVICE_WING_SYLPH && HasEffectStatusWingSylph( this ))
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
					||(m_MoveDevice==MOVE_DEVICE_WING_SYLPH && HasEffectStatusUnicorn(this))
					||(m_MoveDevice==MOVE_DEVICE_WING_SYLPH && HasEffectStatusOusterNewWings1(this))
					||(m_MoveDevice==MOVE_DEVICE_WING_SYLPH && HasEffectStatusOusterNewWings2(this))
#endif //__SECOND_TRANSFORTER
					)
				{
					moveTimes_1 = (*g_pCreatureTable)[m_CreatureType].MoveTimesMotor-1;
				}
			else
			{
				DEBUG_ADD_FORMAT("[Error] What is MoveType of this Player??? id=%d", m_ID);
				CT_BAT("ActionMove RETURN: UNKNOWN_MOVE_DEVICE — device=%d SetStop cagrildi", (int)m_MoveDevice);

				SetStop();

				return;
			}

				m_MoveTableCount = 0;
				m_cX = m_cXTable[moveTimes_1][m_DirectionMoved][m_MoveTableCount];
				m_cY = m_cYTable[moveTimes_1][m_DirectionMoved][m_MoveTableCount];
			}

			m_sX += m_cX;
			m_sY += m_cY;

			m_MoveCount++;

			// ´ÙÀ½¿¡ ÀÌµ¿ÇÒ count¸¦ ÁöÁ¤ÇÑ´Ù.
			m_NextMoveCount = (*g_pCreatureTable)[m_CreatureType].MoveRatio;

			//------------------------------------------------
			// Ä³¸¯ÅÍÀÇ MoveAction¿¡ ¸Â´Â Sound¸¦ Ãâ·ÂÇØÁØ´Ù.
			//------------------------------------------------
			TYPE_SOUNDID soundID = SOUNDID_NULL;
			if (m_MoveDevice == MOVE_DEVICE_RIDE)
			{
				soundID = GetRideMotorcycleSoundID(MOTORCYCLE_SOUND_MOVE);
			}
			if (soundID == SOUNDID_NULL)
			{
				soundID = (*g_pCreatureTable)[m_CreatureType].GetActionSound( m_MoveAction );
			}

			if (soundID!=SOUNDID_NULL)
			{
				PlaySound( soundID,
							false,
							m_X,
							m_Y);
			}
		}	
	//--------------------------------------------
	// ¿òÁ÷ÀÏ ¼ö ¾øÀ» °æ¿ì
	//--------------------------------------------
	// ±æÃ£±â¸¦ ´Ù½Ã ÇØÁØ´Ù.
	else
	{
		m_NextDestX = m_DestX;
		m_NextDestY = m_DestY;
		m_listDirection.empty();
		CT_BAT("ActionMove BLOCKED-TILE: to=(%d,%d) canMove=0 — zone reddetti, nextDest=(%d,%d) olarak yeniden yol istenecek",
			(int)x, (int)y, (int)m_NextDestX, (int)m_NextDestY);


		DEBUG_ADD("´Ù¸¥ Ä³¸¯ÅÍ¿¡ ÀÇÇØ¼­ BlockµÆÀ½.");
	}

		// ´ÙÀ½ Sector±îÁö ¿òÁ÷ÀÎ´Ù.
		if (!m_listDirection.empty())
		{
			//m_NextAction = m_MoveAction;
			SetNextAction( m_MoveAction );		
		}
	}
	//----------------------------------------------------
	//
	// ¾ÆÁ÷ ´ÙÀ½ Sector±îÁö ´ú µµ´ÞÇÑ °æ¿ì
	//
	//----------------------------------------------------
	else
	{
		// ÇÑ FrameÀ» ÀÌµ¿ÇÑ´Ù.
		/*
		switch (m_MoveDevice)
		{
			case MOVE_DEVICE_NULL :			
				m_sX += m_cXTableDeviceNull[m_Direction][m_ActionCount];
				m_sY += m_cYTableDeviceNull[m_Direction][m_ActionCount];
			break;

			case MOVE_DEVICE_MOTOR1 :						
				m_sX += m_cXTableDeviceMotor1[m_Direction][m_ActionCount];
				m_sY += m_cYTableDeviceMotor1[m_Direction][m_ActionCount];
			break;
		}
		*/

		#ifdef OUTPUT_DEBUG_PLAYER_ACTION
			DEBUG_ADD("going");
		#endif

		// ÀÌµ¿ÇÒ count°¡ µÆÀ»¶§¸¸ ÀÌµ¿ÇÑ´Ù.
		if (m_MoveCount>=m_NextMoveCount)
		{			
			//--------------------------------------------
			// Fast Move ÀÎ °æ¿ì
			//--------------------------------------------
			if (m_bFastMove)
			{
				// 2001.8.22 Ãß°¡ - ³È³È
				if (m_sX!=0)	m_sX += m_cX;
				if (m_sY!=0)	m_sY += m_cY;		

				m_MoveCount += 2;

				if (m_MoveCount > m_MoveCountMax)
				{
					m_MoveCount = m_MoveCountMax;
				}

				// ´ÙÀ½¿¡ ÀÌµ¿ÇÒ count¸¦ ÁöÁ¤ÇÑ´Ù.
				m_NextMoveCount += (*g_pCreatureTable)[m_CreatureType].MoveRatio;

				if (m_MoveCount+1 >= m_MoveCountMax
					&& m_nNoPacketUsedActionInfo==ACTIONINFO_NULL)
				{
					//----------------------------------------------------------
					// À¸À¸Èì.. Å×½ºÆ® ÄÚµå.
					//----------------------------------------------------------
					// fast move ÈÄ¿¡ ÇÑ¹ø ¶§¸®±â.. - -
					// Flash Sliding
					//----------------------------------------------------------
					//m_TraceID	= id;		
					MCreature* pCreature = g_pZone->GetCreature( m_TraceID );

					//GET_DYING_CREATURE( pCreature, m_TraceID );

					if (pCreature!=NULL 
						&& m_nSpecialActionInfo != MAGIC_RAPID_GLIDING 
						&& m_nSpecialActionInfo != SKILL_TELEPORT
						&& m_nSpecialActionInfo != SKILL_WARP_PORTAL
						&& !HasEffectStatus(EFFECTSTATUS_BIKE_CRASH)
						)		//-_- ÇöÀç ¼±ÅÃµÈ°Ô...·Î..
					{
//						_MinTrace(" SpecialActionInfo : %d UsedAction : %d NextUsedActionInfo :%d\n",
//							m_nSpecialActionInfo, m_nUsedActionInfo, m_nNextUsedActionInfo);
						#ifdef OUTPUT_DEBUG
							DEBUG_ADD("[FastMovePosition] set next action to attack");
						#endif

						UnSetRequestMode();

						// »ç¿ëÇÑ ±â¼ú·Î ¼³Á¤
						
						//---------------------------------------------------
						// ¿ìÈÄÈÊ..
						//---------------------------------------------------
						MActionResult* pResult = new MActionResult;

						// °á°ú Ç¥Çö
						int ActionInfo = GetBasicActionInfo() ; //m_nBasicActionInfo;
						if( m_nSpecialActionInfo == SKILL_BLITZ_SLIDING || m_nSpecialActionInfo == SKILL_BLAZE_WALK )
						{
							m_fTrace	= FLAG_TRACE_CREATURE_SPECIAL;
							if(m_nSpecialActionInfo == SKILL_BLITZ_SLIDING )
								m_nNextUsedActionInfo	= SKILL_BLITZ_SLIDING_ATTACK;
							else
								return;
							m_TraceX	= pCreature->GetX();
							m_TraceY	= pCreature->GetY();
							m_TraceZ	= pCreature->GetZ();
							
							m_bTraceCreatureToForceAttack	= false;
							m_bKeepTraceCreature			= false;
							m_TraceDistance		= m_BasicAttackDistance;
							
							m_bNextAction = true;
							SetNextAction(GetActionInfoAction( m_nNextUsedActionInfo ));
							SetDirection( MTopView::GetDirectionToPosition( GetX(), GetY(), pCreature->GetX(), pCreature->GetY() ) );
							return;
						}
						if(m_nSpecialActionInfo == SKILL_SET_AFIRE)
						{
							m_nNextUsedActionInfo = SKILL_SET_AFIRE;
							m_nNoPacketUsedActionInfo = m_nNextUsedActionInfo;
							m_NextAction = ACTION_ATTACK;
							
							return;
						}
						if(m_nSpecialActionInfo == SKILL_SET_AFIRE_2)
						{
							m_nNextUsedActionInfo = SKILL_SET_AFIRE_2;
							m_nNoPacketUsedActionInfo = m_nNextUsedActionInfo;
							m_NextAction = ACTION_ATTACK;

							return;
						}
						if(m_nSpecialActionInfo == SKILL_ADV2_SUDDEN_BLAST)
						{
							m_nNextUsedActionInfo = SKILL_ADV2_SUDDEN_BLAST;
							m_nNoPacketUsedActionInfo = m_nNextUsedActionInfo;
							m_NextAction = ACTION_ATTACK;
							
							return;
						}
						if(m_nSpecialActionInfo == SKILL_ACCELERATE_CHARGE_1)
						{
							m_nNextUsedActionInfo = SKILL_ACCELERATE_CHARGE_1;
							m_nNoPacketUsedActionInfo = m_nNextUsedActionInfo;
							m_NextAction = ACTION_ATTACK;
						}
						if(m_nSpecialActionInfo == SKILL_ACCELERATE_CHARGE_2)
						{
							m_nNextUsedActionInfo = SKILL_ACCELERATE_CHARGE_2;
							m_nNoPacketUsedActionInfo = m_nNextUsedActionInfo;
							m_NextAction = ACTION_ATTACK;
						}
//						if(m_nSpecialActionInfo == MAGIC_BLOODY_ZENITH)
//						{
//							SetBloodyZenith(16);
//						}

						m_nNextUsedActionInfo	= GetBasicActionInfo() ; //m_nBasicActionInfo;

						pResult->Add( new MActionResultNodeActionInfo(
														ActionInfo,
														m_ID,
														pCreature->GetID(), 
														pCreature->GetX(),
														pCreature->GetY(),
														0 ) 
										);	

						// PlayerÀÇ ÇöÀç ÁøÇà ÁßÀÎ ±â¼úÀÇ °á°ú·Î Ãß°¡½ÃÅ²´Ù.
						if( m_nUsedActionInfo == SKILL_BLITZ_SLIDING_ATTACK ||
							m_nUsedActionInfo == SKILL_BLAZE_WALK_ATTACK
							)
							m_nUsedActionInfo = GetBasicActionInfo(); //m_nBasicActionInfo;
						
						int tempActionInfo = m_nUsedActionInfo;
						m_nUsedActionInfo = GetBasicActionInfo(); //m_nBasicActionInfo;	// ¸·°¡´Â ÄÚµå - -;
						
						// ¸·°£´Ù ~~  --;
						if (m_listEffectTarget.size()==0)
						{
							m_pEffectTarget = new MEffectTarget( 0 );
							m_pEffectTarget->NewEffectID();

							m_listEffectTarget.push_back( m_pEffectTarget );							
						}

						PacketAddActionResult( 0, pResult );

						m_nUsedActionInfo = tempActionInfo;
						//---------------------------------------------------
						// ¿ìÈÄÈÄÈÊ... - -; 2001.12.10¿¡ Ãß°¡.. ¤»¤».
						//---------------------------------------------------
						
						// ÃßÀû Á¤º¸ ¼³Á¤						
						m_fTrace	= FLAG_TRACE_CREATURE_BASIC;
						m_TraceX	= pCreature->GetX();
						m_TraceY	= pCreature->GetY();
						m_TraceZ	= pCreature->GetZ();

						m_bTraceCreatureToForceAttack	= true;
						m_bKeepTraceCreature			= false;
						m_TraceDistance		= m_BasicAttackDistance;

						m_bNextAction = true;
						// FAZ 4 (2026-05-11 it.2) Bug 11: SelfSpecialAction/FastMove gun guard
						// m_nNextUsedActionInfo gun-type ise + magazine NULL/0 ise gun-attack OYNAMA.
						bool bug11_ssa_skipFire = false;
						if (m_nNextUsedActionInfo >= 0 && m_nNextUsedActionInfo < (int)g_pActionInfoTable->GetSize() &&
							(*g_pActionInfoTable)[m_nNextUsedActionInfo].IsWeaponTypeGunAny())
						{
							if (g_pCurrentMagazine == NULL ||
								(g_pCurrentMagazine != NULL && g_pCurrentMagazine->GetNumber() == 0))
							{
								bug11_ssa_skipFire = true;
								CT_Done("SSA: Bug11 skip gun fire (mag empty) action=%d", (int)m_nNextUsedActionInfo);
								PlaySound(SOUND_ITEM_NO_MAGAZINE);
								m_DelayTime = g_CurrentTime + GetActionInfoDelay(m_nBasicActionInfo);
								m_fNextTrace = FLAG_TRACE_NULL;
								m_nNextUsedActionInfo = ACTIONINFO_NULL;
								UnSetRepeatAction();
								SetNextAction(ACTION_STAND);
							}
						}
						if (!bug11_ssa_skipFire)
						{
							SetNextAction(GetActionInfoAction( m_nNextUsedActionInfo ));
						}
						SetDirection( MTopView::GetDirectionToPosition( GetX(), GetY(), pCreature->GetX(), pCreature->GetY() ) );

						// ´ÙÀ½¿¡ ÇÑ ¹ø ÆÐÅ¶À» ¾È º¸³½´Ù.
						m_nNoPacketUsedActionInfo = m_nNextUsedActionInfo;
					}
					else
					{
						#ifdef OUTPUT_DEBUG
							DEBUG_ADD_FORMAT("[FastMovePosition] there is no such creature.id=%d", m_TraceID);
						#endif
					}
				}
	
			}
			//--------------------------------------------
			// ÀÏ¹ÝÀûÀÎ ÀÌµ¿ÀÇ °æ¿ì
			//--------------------------------------------
			else
			{
				int moveTimes_1;
							
				if (m_MoveDevice==MOVE_DEVICE_WALK)
				{
					moveTimes_1 = (*g_pCreatureTable)[m_CreatureType].MoveTimes-1;
				}
				else if (m_MoveDevice==MOVE_DEVICE_RIDE)
				{	
					moveTimes_1 = (*g_pCreatureTable)[m_CreatureType].MoveTimesMotor-1;
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
					if(IsRide_Fast_Transforter())
					moveTimes_1--;
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
				}
				else if ((m_MoveDevice==MOVE_DEVICE_SUMMON_SYLPH || HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this)))
#if __CONTENTS(__FAST_TRANSFORTER)
					||(m_MoveDevice==MOVE_DEVICE_WING_SYLPH && HasEffectStatusWingSylph( dynamic_cast<MCreature*>(this)))
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
					||(m_MoveDevice==MOVE_DEVICE_WING_SYLPH && HasEffectStatusUnicorn( dynamic_cast<MCreature*>(this)))
					||(m_MoveDevice==MOVE_DEVICE_WING_SYLPH && HasEffectStatusOusterNewWings1( dynamic_cast<MCreature*>(this)))
					||(m_MoveDevice==MOVE_DEVICE_WING_SYLPH && HasEffectStatusOusterNewWings2( dynamic_cast<MCreature*>(this)))
#endif //__SECOND_TRANSFORTER
					)
				{
					moveTimes_1 = (*g_pCreatureTable)[m_CreatureType].MoveTimesMotor-1;
				}
				else
				{
					DEBUG_ADD_FORMAT("[Error] What is MoveType of this Creature??? id=%d", m_ID);					
					SetStop();					
					return;
				}

				m_MoveTableCount++;
				// 2004, 12, 22, sobeit add start - max °ª ³Ñ¾î°¡¸é ¾û¶×ÇÑ ÁÂÇ¥°¡ µé¾î°£´Ù.
				if(m_MoveTableCount > moveTimes_1)
					m_MoveTableCount = moveTimes_1;
				// 2004, 12, 22, sobeit add end
				m_cX = m_cXTable[moveTimes_1][m_DirectionMoved][m_MoveTableCount];
				m_cY = m_cYTable[moveTimes_1][m_DirectionMoved][m_MoveTableCount];

				// 2001.8.22 Ãß°¡ - ³È³È
				if (m_sX!=0)	m_sX += m_cX;
				if (m_sY!=0)	m_sY += m_cY;		
			
				// ´ÙÀ½¿¡ ÀÌµ¿ÇÒ count¸¦ ÁöÁ¤ÇÑ´Ù.
				m_NextMoveCount += (*g_pCreatureTable)[m_CreatureType].MoveRatio;

				m_MoveCount++;
			}
		}	
		else
		{
			m_MoveCount++;
		}
		
		//m_ActionCount++;
		// frameÀ» ¹Ù²ãÁØ´Ù. ¸¶Áö¸· Frame±îÁö °¬À¸¸é 0¹øÂ° FrameÀ¸·Î ¹Ù²Û´Ù.
		//if (++m_ActionCount == (*m_pFrames)[m_Action][m_Direction].GetCount())
		//	m_ActionCount = 0;

		//if (m_ActionCount == (m_ActionCountMax>>1))
		//if (m_MoveCount == (m_MoveCountMax>>1))
		//{				
		//	SetLightSight( m_LightSight );
		//}
	}

	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD("moveok");
	#endif
}

//----------------------------------------------------------------------
// Attach CastingEffect
//----------------------------------------------------------------------
void	
MPlayer::AttachCastingEffect(TYPE_ACTIONINFO nUsedActionInfo, BOOL bForceAttach)
{
	MCreature::AttachCastingEffect(nUsedActionInfo, bForceAttach);

	// ÄÊ..
}

//----------------------------------------------------------------------
// Affect UsedActionInfo
//----------------------------------------------------------------------
// m_nUsedActionInfo¸¦ ½ÇÇà½ÃÅ²´Ù.
//----------------------------------------------------------------------
void
MPlayer::AffectUsedActionInfo(TYPE_ACTIONINFO nUsedActionInfo)
{
	if (nUsedActionInfo==ACTIONINFO_NULL)
	{
		return;
	}

	switch(nUsedActionInfo)
	{
	case SKILL_ACID_STORM:
		{
			if((*g_pRankBonusTable)[RANK_BONUS_WIDE_STORM].GetStatus() == RankBonusInfo::STATUS_LEARNED)
				nUsedActionInfo = SKILL_ACID_STORM_WIDE;
		}
		break;
		
	case SKILL_POISON_STORM:
		{
			if((*g_pRankBonusTable)[RANK_BONUS_WIDE_STORM].GetStatus() == RankBonusInfo::STATUS_LEARNED)
				nUsedActionInfo = SKILL_POISON_STORM_WIDE;
		}
		break;

	case SKILL_KASAS_ARROW:
		break;

	case SKILL_LARSLASH :
		
		break;					

	case SKILL_SWEEP_VICE_1:
	case SKILL_SWEEP_VICE_3:
	case SKILL_SWEEP_VICE_5:
		if(g_pSkillAvailable->IsEnableSkill( SKILL_SWEEP_VICE_1 ))
		{
		}
		else
			return;
		break;
	}
	POINT point;	
#ifdef __METROTECH_TEST__
//	if(g_bLight)
//	{
//		int plusX = GetX()-m_TraceX;
//		int plusY = GetY()-m_TraceY;
//		int moveX = 0, moveY = 0;
//		if(plusX != 0)
//			moveX = plusX/abs(plusX);
//		if(plusY != 0)
//			moveY = plusY/abs(plusY);
//		
//		g_pPlayer->UnSetRequestMode();
//		
//		g_SelectSector.x = GetX()+moveX;
//		g_SelectSector.y = GetY()+moveY;
//		if(abs(g_SelectSector.x - m_TraceX) > GetActionInfoRange(nUsedActionInfo))
//			g_SelectSector.x = GetX();
//		if(abs(g_SelectSector.y - m_TraceY) > GetActionInfoRange(nUsedActionInfo))
//			g_SelectSector.y = GetY();
//		
//		// ´ÙÀ½ ¸ñÇ¥À§Ä¡·Î ¼³Á¤ÇÑ´Ù
//		g_pPlayer->TraceNULL();
//		
//		if (g_pPlayer->SetMovePosition(g_SelectSector.x, g_SelectSector.y))
//		{
//			g_pPlayer->SetNextActionToMove();
//		}
//		
//		// ´ÙÀ½ °¥ °÷ÀÌ ¾øÀ¸¸é
//		POINT point;
//		g_pPlayer->GetNextDestination( point );		
//		if (point.x==SECTORPOSITION_NULL || point.y==SECTORPOSITION_NULL)
//		{
//			// ÇöÀç °¡°í ÀÖ´Â °÷ÀÌ ¾øÀ¸¸é
//			g_pPlayer->GetDestination( point );
//			if (point.x==SECTORPOSITION_NULL || point.y==SECTORPOSITION_NULL)
//			{
//				//-_-;;
//			}
//			else
//			{
//				g_pTopView->SetSelectedSector( point );
//			}
//		}
//		else
//		{
//			g_pTopView->SetSelectedSector( point );
//		}		
//	}
#endif

	//-----------------------------------------------------
	// ÀÌ ±â¼ú¿¡¼­ »ç¿ëÇÒ EffectTarget
	//-----------------------------------------------------
	MEffectTarget* pEffectTarget = NULL;

	DEBUG_ADD_FORMAT("[MPlayer-StartEffect] ActionInfo=%d, ActionCount=%d/%d", nUsedActionInfo, m_ActionCount, m_ActionCountMax);
	
	//--------------------------------------------------------
	// Casting ActionInfoÀÎ°¡?
	//--------------------------------------------------------
	BOOL bCastingAction = (*g_pActionInfoTable)[nUsedActionInfo].IsCastingAction();

	//--------------------------------------------------------
	// ¸ñÇ¥ À§Ä¡ PixelÁÂÇ¥
	//--------------------------------------------------------
	point = MTopView::MapToPixel(m_TraceX, m_TraceY);
	//point.x += m_sX;
	//point.y += m_sY;
	//point.x += TILE_X_HALF;
	
	//--------------------------------------------------------
	//
	//					Effect ¸ñÇ¥ ¼³Á¤
	//Z
	//--------------------------------------------------------
	//MEffectTarget* m_pEffectTarget = new MEffectTarget( (*g_pActionInfoTable)[nUsedActionInfo].GetSize() );
	// 
	//--------------------------------------------------------
	// casting actionÀÎ °æ¿ì
	//--------------------------------------------------------
	if (bCastingAction)
	{
		// casting¿ë EffectTargetÀ» »ý¼ºÇÑ´Ù.
		pEffectTarget = new MEffectTarget( (*g_pActionInfoTable)[nUsedActionInfo].GetSize() );
		pEffectTarget->NewEffectID();

		if( nUsedActionInfo == CASTING_FIRE || nUsedActionInfo == CASTING_WATER ||
			nUsedActionInfo == CASTING_GROUND || nUsedActionInfo == CASTING_FIRE2 ||
			nUsedActionInfo == CASTING_GROUND2 || nUsedActionInfo == CASTING_WATER2 )
			pEffectTarget->Set( point.x, point.y, m_TraceZ, GetID() );
		else
			pEffectTarget->Set( point.x, point.y, m_TraceZ, m_TraceID );

		//--------------------------------------------------------
		// castingAction¿¡¼­µµ delay°¡ Àû¿ëµÇÁö¸¸,
		// castingActionÀÎ °æ¿ì´Â delay¸¦ Á¦°ÅÇÏ¸é ¾ÈµÈ´Ù.
		//--------------------------------------------------------
		// ÀÌ°Å °ËÁõÇÒ ¹æ¹ýÀÌ ¾ø´Ù.
		//if (m_DelayActionInfo==nUsedActionInfo) - -;
		{
			pEffectTarget->SetDelayFrame( m_EffectDelayFrame );
			m_DelayActionInfo	= ACTIONINFO_NULL;
			m_EffectDelayFrame	= 0;
		}

	}
	//--------------------------------------------------------
	// casting actionÀÌ ¾Æ´Ñ.. ½ÇÁ¦ ±â¼úÀÎ °æ¿ì..
	//--------------------------------------------------------
	else
	{
		//--------------------------------------------------------
		// ½ÃÀÛ ±â¼úÀÌ¸é.. ÀÌ¹Ì new¸¦ ÇßÀ¸¹Ç·Î
		// °á°ú ±â¼úÀÎ °æ¿ì¿¡¸¸ EffectTargetÀ» »ý¼ºÇÑ´Ù.
		//--------------------------------------------------------
		if (nUsedActionInfo < (*g_pActionInfoTable).GetMinResultActionInfo())
		{
		}
		else
		{
			if (m_pEffectTarget != NULL)
			{
				DEBUG_ADD_FORMAT("RemoveEffectTarget: action=(%d/%d) ActionInfo=%d, pEffectTarget=%x", m_ActionCount, m_ActionCountMax, nUsedActionInfo, m_pEffectTarget);
				
				RemoveEffectTarget( m_pEffectTarget->GetEffectID() );
				MEffectTarget* pTempEffectTarget = m_pEffectTarget;
				m_pEffectTarget = NULL;
				
				// deleteµÇ¸é¼­ m_pEffectTarget¿¡ Á¢±ÙÀÌ µÈ´Ù - -;
				delete pTempEffectTarget;				
			}

			//----------------------------------------------------------
			// °á°ú¸¦ ÇÏ³ª »ý¼º½ÃÄÑ µÐ´Ù.
			//----------------------------------------------------------
			m_pEffectTarget = new MEffectTarget( (*g_pActionInfoTable)[nUsedActionInfo].GetSize() );
			m_pEffectTarget->NewEffectID();

			// ³ªÁß¿¡ Áö¿öÁÖ±â À§ÇØ¼­..
			AddEffectTarget( m_pEffectTarget );
			//m_listEffectTarget.push_back( pEffectTarget );

			DEBUG_ADD_FORMAT("NewEffectTarget: action=(%d/%d) ActionInfo=%d, pEffectTarget=%x", m_ActionCount, m_ActionCountMax, nUsedActionInfo, m_pEffectTarget);
		}
		

		if (m_pEffectTarget==NULL)
		{	
			DEBUG_ADD_FORMAT("EffectTarget is NULL, newEffectTarget : %x", m_pEffectTarget);
			
			m_pEffectTarget = new MEffectTarget( (*g_pActionInfoTable)[nUsedActionInfo].GetSize() );
			m_pEffectTarget->NewEffectID();

			DEBUG_ADD("new EffectTarget OK");
		}

//		if( nUsedActionInfo == MAGIC_ACID_TOUCH || nUsedActionInfo == MAGIC_BLOODY_NAIL ||
//			nUsedActionInfo == SKILL_FLOURISH || nUsedActionInfo == SKILL_ABSORB_SOUL)
		if(nUsedActionInfo != ACTIONINFO_NULL && (*g_pActionInfoTable)[nUsedActionInfo].IsAttachSelf() )
		{
			//m_pEffectTarget->Set( point.x, point.y, m_TraceZ, m_TraceID );
			m_pEffectTarget->Set( point.x, point.y, m_TraceZ, GetID() );
		} else
		{
			m_pEffectTarget->Set( point.x, point.y, m_TraceZ, m_TraceID );
		}

		//--------------------------------------------------------
		// Action¿¡ ¸Â´Â Sound Ãâ·Â
		//--------------------------------------------------------
		// °á°úÀÎ °æ¿ì¸¸ ¿©±â¼­ »ç¿îµå Ãâ·ÂÇÑ´Ù.
		if (nUsedActionInfo >= g_pActionInfoTable->GetMinResultActionInfo())
		{
			int soundID = (*g_pCreatureTable)[m_CreatureType].GetActionSound( 
									GetActionInfoAction(nUsedActionInfo,  false , true) );

			PlaySound( soundID,	false, m_X, m_Y	);

			//-------------------------------------------------
			// Force Feel
			//-------------------------------------------------
			if (g_pUserOption->UseForceFeel && gpC_Imm!=NULL && gpC_Imm->IsDevice()
				&& soundID < g_pSoundTable->GetSize())
			{
				DEBUG_ADD_FORMAT("ForceAction-AffectUsedActionInfo(%d, %s)", soundID, strrchr((*g_pSoundTable)[soundID].Filename.GetString(), '\\'));
		
				gpC_Imm->ForceAction( soundID );
			}
		}

		//--------------------------------------------------------
		// Áö¼Ó ½Ã°£ ¼³Á¤
		//--------------------------------------------------------
		if (m_DelayActionInfo==nUsedActionInfo)
		{
			m_pEffectTarget->SetDelayFrame( m_EffectDelayFrame );
			m_DelayActionInfo	= ACTIONINFO_NULL;
			m_EffectDelayFrame	= 0;
		}

		// ÀúÀåÇØµÎ°í
		pEffectTarget = m_pEffectTarget;
		
		// Áö¿ò.. 
		m_pEffectTarget = NULL;
	}

	// ÁøÇàÁßÀÎ Effect·Î¼­ ±â¾ïÇØµÐ´Ù.		
	//AddEffectTarget( m_pEffectTarget );
	
	// °á°ú¿¡ ´ëÇÑ Á¤º¸¸¦ ¼³Á¤ - delete´Â °á°ú¸¦ Àû¿ëÇÒ¶§..
	/*
	MActionResult* pResult = new MActionResult;
	pResult->Add( new MActionResultNodeActionInfo( m_ID, m_TraceID, nUsedActionInfo ) );
	
	m_pEffectTarget->SetResult( pResult );
	*/
	

	//--------------------------------------------------------
	//
	//					½ÃÀÛ À§Ä¡¸¦ °áÁ¤ÇÑ´Ù.
	//
	//--------------------------------------------------------
	int x,y,z, direction;
	x = 0;
	y = 0;
	z = 0;
	direction = 0;

	//--------------------------------------------------------
	// User À§Ä¡¿¡¼­ ½ÃÀÛÇÏ´Â °æ¿ì
	//--------------------------------------------------------
	if ((*g_pActionInfoTable)[nUsedActionInfo].IsStartUser())
	{
		point = MTopView::MapToPixel(m_X, m_Y);
		point.x += m_sX;
		point.y += m_sY;
		//point.x += TILE_X_HALF;

		x			= point.x;
		y			= point.y;
		z			= m_Z+1;//+60;			
	}
	//--------------------------------------------------------
	// Target À§Ä¡¿¡¼­ ½ÃÀÛÇÏ´Â °æ¿ì
	//--------------------------------------------------------
	else if ((*g_pActionInfoTable)[nUsedActionInfo].IsStartTarget())
	{
		x			= point.x;
		y			= point.y;
		z			= m_Z;//+60;			
	}

	//--------------------------------------------------------
	// °øÁß¿¡¼­ ½ÃÀÛÇÏ´Â °æ¿ì
	//--------------------------------------------------------
	if ((*g_pActionInfoTable)[nUsedActionInfo].IsStartSky())
	{
		z	= m_Z + (*g_pActionInfoTable)[nUsedActionInfo].GetValue();

		//direction	= DIRECTION_DOWN;
	}
	//--------------------------------------------------------
	// Áö»ó¿¡¼­ ½ÃÀÛÇÏ´Â °æ¿ì
	//--------------------------------------------------------
	//else
	//{
	if( nUsedActionInfo == SKILL_ABSORB_SOUL )
	{
		// m_TraceID ´Â Á¸ÀÇ ½ÃÃ¼ÀÌ¹Ç·Î ¾ÆÀÌÅÛÀÌ´Ù.		
		direction = g_pTopView->GetDirectionToPosition( 
			GetX(), GetY(),m_TraceX, m_TraceY);
	}
	else
		direction	= m_Direction;
	//}

	DEBUG_ADD_FORMAT("[MPlayer-StartEffect] Before Generate");
	
	//--------------------------------------------------------
	//
	//                   Effect»ý¼º		
	//
	//--------------------------------------------------------
	g_pEffectGeneratorTable->Generate(
			x,y,z,				// ½ÃÀÛ À§Ä¡
			direction, 		// ¹æÇâ
			1,					// power
			nUsedActionInfo,		//	ActionInfoTableÁ¾·ù,
			pEffectTarget		// ¸ñÇ¥ Á¤º¸
			, GetActionGrade()
	);
	ClearActionGrade();

	DEBUG_ADD_FORMAT("[MPlayer-StartEffect] AffectUsedActionInfo OK");
}

//----------------------------------------------------------------------
// Action to Send Packet
//----------------------------------------------------------------------
// packetÀ» º¸³½´ç...
//----------------------------------------------------------------------
void
MPlayer::ActionToSendPacket(bool bImmediately)
{
	CT_Done("ATSP: ENTER bImmediately=%d m_nUsedActionInfo=%d m_fTraceBuffer=0x%X m_TraceID=%d", (int)bImmediately, (int)m_nUsedActionInfo, (unsigned)m_fTraceBuffer, (int)m_TraceID);

	//--------------------------------------------------------
	// °á°ú Çàµ¿ÀÌ ¾Æ´Ñ °æ¿ì¸¸ packetÀ» º¸³½´Ù.
	//--------------------------------------------------------
	if ((m_nUsedActionInfo >= g_pActionInfoTable->GetMinResultActionInfo() ||
		 m_nUsedActionInfo == ACTIONINFO_NULL))
	{
		CT_Done("ATSP: EARLY RETURN (result-action or NULL action)");
		return;
	}

	// FAZ 4 (2026-05-11) Bug 11: ActionToSendPacket de mermi yoksa erken donus
	// Generic gun attack actions (SKILL_ATTACK_GUN_AR/SR/SG/SMG) icin
	// magazine NULL veya 0 ise paket gondermez + RepeatAction iptal + sound.
	// (PLASMA_ROCKET_LAUNCHER ve SATELLITE_BOMB switch icinde zaten korunuyor.)
	if (m_nUsedActionInfo >= 0 && m_nUsedActionInfo < (int)g_pActionInfoTable->GetSize() &&
		(*g_pActionInfoTable)[m_nUsedActionInfo].IsWeaponTypeGunAny())
	{
		if (g_pCurrentMagazine == NULL ||
			(g_pCurrentMagazine != NULL && g_pCurrentMagazine->GetNumber() == 0))
		{
			CT_Done("ATSP: Bug11 skip gun fire (mag empty) action=%d", (int)m_nUsedActionInfo);
			PlaySound(SOUND_ITEM_NO_MAGAZINE);
			m_DelayTime = g_CurrentTime + GetActionInfoDelay(m_nBasicActionInfo);
			m_fNextTrace = FLAG_TRACE_NULL;
			UnSetRepeatAction();
			m_nUsedActionInfo = ACTIONINFO_NULL;
			SetNextAction(ACTION_STAND);
			return;
		}
	}

	switch(m_nUsedActionInfo)
	{
	case SKILL_ACID_STORM_WIDE:
		m_nUsedActionInfo = SKILL_ACID_STORM;
		break;

	case SKILL_POISON_STORM_WIDE:
		m_nUsedActionInfo = SKILL_POISON_STORM;
		break;
	case SKILL_BURNING_SOUL_CHARGING:
		if(IsBurningSol()) 
			return;
		break;
	case SKILL_SWEEP_VICE_1:
	case SKILL_SWEEP_VICE_3:
	case SKILL_SWEEP_VICE_5: 
//		if(!GetSweepViewValue()) //½ºÀ¬¹ÙÀÌ½º¸¦ ¾ÆÁ÷ »ç¿ëÁßÀÌ ¾Æ´Ò¶§
//		{
//			SetSweepViewValue(1);
//			g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);
//			ExecuteActionInfoFromMainNode(SKILL_CLIENT_SWEEP_VICE,g_SelectSector.x, g_SelectSector.y, 0,g_pPlayer->GetDirection(),0	,	
//				g_SelectSector.x, g_SelectSector.y, 0, 0xffff, NULL, false);
//			return;
//		}
		if(g_bRButtonDown/*||!GetSweepViewValue()*/) 
			return;
			
		if(2 == GetSweepViewValue() || 3 == GetSweepViewValue())
			m_fTraceBuffer = FLAG_TRACE_SECTOR_SPECIAL;
		break;
	case SKILL_PLASMA_ROCKET_LAUNCHER:
#if __CONTENTS(__SECOND_RARE_SKILL)
	case SKILL_XRL:
#endif //__SECOND_RARE_SKILL
		if(g_pCurrentMagazine != NULL)
		 {
			 int MagazineCount = g_pCurrentMagazine->GetNumber(); 
			 if(MagazineCount>0)
			 {
				 g_pCurrentMagazine->SetNumber(MagazineCount-1);
			 }
			 else
				 return;
		 }
		else return;
		break;
	case SKILL_SATELLITE_BOMB:
		if(g_pCurrentMagazine != NULL)
		 {
			 int MagazineCount = g_pCurrentMagazine->GetNumber(); 
			 if(MagazineCount > 9)
			 {
				 g_pCurrentMagazine->SetNumber(MagazineCount-10);
			 }
			 else
				 return;
		 }
		else return;
	    break;
	case SKILL_SATELLITE_BOMB_2:
		if(g_pCurrentMagazine != NULL)
		 {
			 int MagazineCount = g_pCurrentMagazine->GetNumber(); 
			 if(MagazineCount > 9)
			 {
				 g_pCurrentMagazine->SetNumber(MagazineCount-10);
			 }
			 else
				 return;
		 }
		else return;
	}

	/*
	if(m_nUsedActionInfo != MAGIC_TRANSFORM_TO_WOLF &&
	   m_nUsedActionInfo != MAGIC_TRANSFORM_TO_BAT &&
	   m_nUsedActionInfo != SKILL_TRANSFORM_TO_WERWOLF)
	{
		const int TransformDelay = 500;
		(*g_pSkillInfoTable)[MAGIC_TRANSFORM_TO_WOLF].SetAvailableTime(TransformDelay);
		(*g_pSkillInfoTable)[MAGIC_TRANSFORM_TO_BAT].SetAvailableTime(TransformDelay);
		(*g_pSkillInfoTable)[SKILL_TRANSFORM_TO_WERWOLF].SetAvailableTime(TransformDelay);
	}
	*/
	

	/*
	// [ÀûÀýÇÑ Å¸ÀÌ¹Ö]¿¡ .. 
	BOOL	bActionStand	= (m_Action==ACTION_STAND || IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND);
	int		actionCountMax_1 = m_ActionCountMax-1;
	BOOL	bEndAction		= (m_ActionCount==actionCountMax_1);// || bActionStand;
	int		StartFrame		= (*g_pActionInfoTable)[m_nUsedActionInfo].GetStartFrame( m_WeaponSpeed );

	int		packetSendTiming = min(actionCountMax_1, StartFrame);
	BOOL	bStartAction	= (m_ActionCount==packetSendTiming);	// packetSendTimingÀÌ ¿ø·¡´Â 0ÀÌ¾ú´Ù.
	
	if (bActionStand)
	{
		bEndAction = bStartAction;
	}
	*/

	BOOL	bActionStand	= (m_Action==ACTION_STAND || IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND || IsOusters() && m_Action == ACTION_OUSTERS_FAST_MOVE_STAND
#if __CONTENTS(__FAST_TRANSFORTER)
		|| IsOusters() && m_Action == ACTION_OUSTERS_WING_STAND
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
		|| IsOusters() && m_Action == ACTION_OUSTERS_UNICORN_STAND
#endif //__SECOND_TRANSFORTER
		);
	int		actionCountMax_1 = m_ActionCountMax-1;
	BOOL	bEndAction		= (m_ActionCount==actionCountMax_1) || bActionStand;
	int		StartFrame		= (*g_pActionInfoTable)[m_nUsedActionInfo].GetStartFrame( m_WeaponSpeed );

	bool	bStartAction	= (m_ActionCount==0);	
//	BOOL	bStartAction	= (m_ActionCount == StartFrame && !IsGunShotGuidance());
	

	// Effect°¡ ½ÃÀÛµÇ´Â °æ¿ì´Â..
	// (1) StartFrameÀÎ °æ¿ì
	// (2) ¸¶Áö¸· ActionFrameÀÎ °æ¿ì
	bool	bStartEffect = m_ActionCount==StartFrame || 
							StartFrame >= m_ActionCountMax && bEndAction;
	
	//BOOL	bCastingEffect	= GetActionInfoCastingStartFrame(m_nUsedActionInfo)==m_ActionCount;
		// ¹Ýº¹ actionÀÇ ½ÃÀÛ frame
	bStartAction |= m_bRepeatAction 
					&& (*g_pActionInfoTable)[m_nUsedActionInfo].IsUseRepeatFrame()
					&& m_RepeatCount!=0
					&& m_ActionCount == GetActionInfoRepeatStartFrame( m_nUsedActionInfo );

	bStartAction |= bImmediately;

	//----------------------------------------------------------------------
	// [ TEST CODE ]
	//----------------------------------------------------------------------
	// »ó´çÈ÷ À§ÇèÇÑ ÁþÀÌÁö¸¸.. - -;;
	// Å©°Ô ¹®Á¦´Â ¾øÀ» µí...
	//----------------------------------------------------------------------
	// EffectTargetÀº castingµ¿ÀÛÀÌ ³¡³­ µÚ¿¡ »ý¼ºµÇ´Âµ¥,
	// EffectTargetÀÇ ID¸¦ casting½ÃÀÛ¶§¿¡ server·Î º¸³»¾ß ÇÏ±â ¶§¹®¿¡
	// ÀÓ½Ã·Î... EffectTargetÀ» ¸ÕÀú »ý¼º½ÃÄÑµÎ°í »ç¿ëÇß´Ù.
	//----------------------------------------------------------------------
	// À½. data member·Î ¹Ù²å´Ù. - -;;
	//static MEffectTarget* 

	
	#ifdef OUTPUT_DEBUG
			//DEBUG_ADD_FORMAT("action=(%d/%d) ActionInfo=%d, pEffectTarget=%x", m_ActionCount, m_ActionCountMax, m_nUsedActionInfo, m_pEffectTarget);
	#endif

	//----------------------------------------------------------
	//
	//					µ¿ÀÛ ½ÃÀÛ ½ÃÁ¡..
	//
	//----------------------------------------------------------
	if (//bStartEffect)
		bStartAction || m_nUsedActionInfo == SKILL_BURNING_SOUL_LAUNCH)
	{			

		//----------------------------------------------------------
		// PacketÀ» ¾È º¸³»´Â °æ¿ìÀÌ´Ù.
		//----------------------------------------------------------
		if (m_nNoPacketUsedActionInfo==m_nUsedActionInfo)
		{
			m_nNoPacketUsedActionInfo = ACTIONINFO_NULL;

			int soundID = (*g_pCreatureTable)[m_CreatureType].GetActionSound( 
											GetActionInfoAction(m_nUsedActionInfo,  false , true) );

			//------------------------------------------------
			// Ä³¸¯ÅÍÀÇ Action¿¡ ¸Â´Â Sound¸¦ Ãâ·ÂÇØÁØ´Ù.
			//------------------------------------------------
			PlaySound( soundID,	false, m_X, m_Y	);

			//-------------------------------------------------
			// Force Feel
			//-------------------------------------------------
			if (g_pUserOption->UseForceFeel && gpC_Imm!=NULL && gpC_Imm->IsDevice())				
			{				
				if ((*g_pActionInfoTable)[m_nUsedActionInfo].IsWeaponTypeGunAny())
				{
					soundID = SOUND_SLAYER_ATTACK_AR;
				}

				if (soundID < g_pSoundTable->GetSize())
				{
					DEBUG_ADD_FORMAT("ForceAction-ActionToSendPacket(%d, %s)", soundID, strrchr((*g_pSoundTable)[soundID].Filename.GetString(), '\\'));
					gpC_Imm->ForceAction( soundID );
				}
			}

			return;
		}

		//----------------------------------------------------------------------
		// ÀÌ ´Ü°è¿¡¼­ PacketÀ» º¸³»¾ß ÇÑ´Ù.
		//----------------------------------------------------------------------
		// ´Ü, ActionEffect()°¡ È£ÃâµÆ´Ù°í ÇØ¼­ ¸ðµç °æ¿ì¿¡ 
		// ´Ù PacketÀü¼ÛÀÌ ÀÏ¾î³ª´Â °ÍÀº ¾Æ´Ï´Ù.
		//
		// Server¿¡¼­ ¹Þ¾Æ¼­ Player°¡ ¾î¶² ActionÀ» ÃëÇÒ °æ¿ìµµ ÀÖ±â ¶§¹®ÀÌ´Ù.
		//
		// ActionEffect()°¡ ºÒ¸®±â Àü¿¡..
		// PacketÀ» º¸³¾Áö ¸»Áö¸¦ °áÁ¤ÇÏ´Â º¯¼ö°¡ ÇÊ¿äÇÒ °ÍÀÌ´Ù.
		//
		// (!) Packet¿¡´Â »ç¿ëÇÑ ActionInfo¿¡ ´ëÇÑ Instance ID¸¦ ºÙ¿©Áà¾ß ÇÑ´Ù.
		//     °á°ú¸¦ ¹Þ¾ÒÀ» ¶§ check¸¦ À§ÇØ¼­ÀÌ´Ù.
		//     ±×¸®°í, °¡Àå ÃÖ±ÙÀÇ Instance ID¸¦ ÇÏ³ª¸¸ ±â¾ïÇÑ´Ù.		
		//----------------------------------------------------------------------

		// °á°ú¸¦ º¸¿©ÁÖ´Â ActionÀÌ ¾Æ´Ï°í
		// ½ÇÁ¦·Î Player°¡ »ç¿ëÇÑ ActionÀÏ °æ¿ì¿¡¸¸ PacketÀ» º¸³½´Ù.
		if (m_nUsedActionInfo < (*g_pActionInfoTable).GetMinResultActionInfo())
		{

			//--------------------------------------------------------
			// ±â¼ú »ç¿ë ½Ã°£ Delay¸¦ ¼³Á¤ÇØÁØ´Ù.
			// ½ÃÀÛ ±â¼úÀÎ °æ¿ì¿¡¸¸..
			//--------------------------------------------------------
			if (m_Action==ACTION_STAND)
			{
				// Á¤Áö µ¿ÀÛ ±â¼úÀº 0.3ÃÊÀÇ delay¸¦ °¡Áø´Ù.
				m_DelayTime	= g_CurrentTime + 300;		 //ÄÚÖÃCD
			}
			else if (m_bRepeatAction)
			{				
			}
			else
			{
				m_DelayTime	= g_CurrentTime 
								+ GetActionInfoDelay(m_nUsedActionInfo);
								// [ÀûÀýÇÑ Å¸ÀÌ¹Ö]¿¡ º¸³¾¶§´Â - ÇØ¾ß ÇÑ´Ù.
								// - (m_ActionCount<<6);	// Áö³ª°£ ActionCount¸¸Å­ delay¸¦ »©ÁØ´Ù.
			}

		//----------------------------------------------------------
		//
		//			¼­¹ö¿¡ Á¢¼ÓÇØ ÀÖ´Â °æ¿ì..
		//
		//----------------------------------------------------------

			// 
			if (m_pEffectTarget != NULL)
			{
				DEBUG_ADD_FORMAT("RemoveEffectTarget: action=(%d/%d) ActionInfo=%d, pEffectTarget=%x", m_ActionCount, m_ActionCountMax, m_nUsedActionInfo, m_pEffectTarget);
				
				RemoveEffectTarget( m_pEffectTarget->GetEffectID() );

				MEffectTarget* pTempEffectTarget = m_pEffectTarget;
				m_pEffectTarget = NULL;
				
				delete pTempEffectTarget;			
				pTempEffectTarget = NULL;
			}

			//----------------------------------------------------------
			// °á°ú¸¦ ÇÏ³ª »ý¼º½ÃÄÑ µÐ´Ù.
			//----------------------------------------------------------
			m_pEffectTarget = new MEffectTarget( (*g_pActionInfoTable)[m_nUsedActionInfo].GetSize() );
			
			DEBUG_ADD_FORMAT("NewEffectTarget: action=(%d/%d) ActionInfo=%d, pEffectTarget=%x", m_ActionCount, m_ActionCountMax, m_nUsedActionInfo, m_pEffectTarget);
			
			//---------------------------------------------------------
			// ÁøÇàÁßÀÎ °á°ú·Î¼­ Ãß°¡ÇÑ´Ù.
			//----------------------------------------------------------
			AddEffectTarget( m_pEffectTarget );

			//----------------------------------------------------------
			// »õ·Î¿î EffectID¸¦ ¹ß±Þ?¹Þ´Â´Ù.
			//----------------------------------------------------------
			m_pEffectTarget->NewEffectID();

			
			if (m_fTraceBuffer!=FLAG_TRACE_NULL)
			{
				//----------------------------------------------------------
				// ´ÙÀ½¿¡ ÀÌ ±â¼úÀ» »ç¿ëÇÒ ¼ö ÀÖ´Â ½Ã°£ ¼³Á¤
				//----------------------------------------------------------
				int originalActionInfo = m_nUsedActionInfo;
				if( (*g_pActionInfoTable)[m_nUsedActionInfo].GetParentActionInfo() != ACTIONINFO_NULL )
					originalActionInfo = (*g_pActionInfoTable)[m_nUsedActionInfo].GetParentActionInfo();
				g_pSkillInfoTable->SetNextAvailableTime(originalActionInfo);

				//----------------------------------------------------------
				// º¸³»´Â packet Á¾·ù ¼³Á¤
				//----------------------------------------------------------
				ACTIONINFO_PACKET	packetType		= (*g_pActionInfoTable)[m_nUsedActionInfo].GetPacketType();
				
//Test - 080125		: Skill Cool Time Check
//				if (! (m_fTraceBuffer & FLAG_TRACE_CREATURE_BASIC) )
					//±âº»°ø°ÝÀÌ ¾Æ´Ò ¶§¸¸ È£ÃâÇØ ÁÖÀÚ.
//					TestCheckSkillCooltime (m_nUsedActionInfo, m_WeaponSpeed);		
//

				//----------------------------------------------------------
				//
				//			»ó´ë¿¡°Ô »ç¿ëÇÏ´Â °ÍÀÌ¸é..
				//
				//----------------------------------------------------------
				// ±âº» °ø°Ý
				//----------------------------------------------------------
				if (m_fTraceBuffer & FLAG_TRACE_CREATURE_BASIC)
				{
					MCreature* pCreature = g_pZone->GetCreature(m_TraceID);
					CT_Done("ATK: enter, TraceID=%d pCreature=%p g_pObjSel=%p", (int)m_TraceID, (void*)pCreature, (void*)g_pObjectSelector);

					if(pCreature && g_pObjectSelector->CanAttack(pCreature))
					{
						CT_Done("ATK: CanAttack=true, building CGAttack pkt step=%d", (int)m_ActionStep);
						CGAttack _CGAttack;
						_CGAttack.setX( m_X );
						_CGAttack.setY( m_Y );
						_CGAttack.setDir( m_Direction );
						_CGAttack.setObjectID( m_TraceID );
						_CGAttack.setActionStep( m_ActionStep );

						CT_Done("ATK: pre sendPacket(CGAttack)");
						g_pSocket->sendPacket( &_CGAttack );
						CT_Done("ATK: post sendPacket(CGAttack) OK");
					}
					else { CT_Done("ATK: SKIP (pCreature=%p)", (void*)pCreature); }

					// [ TEST CODE ] - -;;
					// [ TEST CODE ] - -;;
					// [ TEST CODE ] - -;;
					/*
					#if defined(OUTPUT_DEBUG) && defined(__DEBUG_OUTPUT__) && defined(_DEBUG)
						if (g_DXInput.KeyDown(DIK_LMENU))
						{
							for (int i=0; i<200; i++)
							{
								g_pSocket->sendPacket( &_CGAttack );
							}
						}
					#endif
					*/

				}
				//----------------------------------------------------------
				//
				// Blood drainÀÎ °æ¿ì
				//
				//----------------------------------------------------------
				else if (packetType==ACTIONINFO_PACKET_BLOOD_DRAIN)
				{
					CGBloodDrain _CGBloodDrain;
					_CGBloodDrain.setObjectID( m_TraceID );
					g_pSocket->sendPacket( &_CGBloodDrain );
			//		DEBUG_ADD("ÈíÇ÷ ¸Þ¼¼Áö º¸³¿");									
					// 2001.8.20 ÁÖ¼®Ã³¸®
//					SetWaitVerify( WAIT_VERIFY_SKILL_SUCCESS, m_nUsedActionInfo );
				}
				//----------------------------------------------------------
				//
				// Èí¿µÀÎ °æ¿ì
				//
				//----------------------------------------------------------
				else if (packetType==ACTIONINFO_PACKET_ABSORB_SOUL)
				{
					CGAbsorbSoul _CGAbsorbSoul;
					_CGAbsorbSoul.setObjectID( g_pZone->GetItemID( m_TraceX, m_TraceY ) );
					_CGAbsorbSoul.setTargetZoneX( m_TraceX );
					_CGAbsorbSoul.setTargetZoneY( m_TraceY );

					int larvaSize = (*g_pItemTable)[ITEM_CLASS_LARVA].GetSize();
					MItem* pLarvaItem = NULL;
					for(int i = 0; i < larvaSize; i++)
					{
						 pLarvaItem= g_pInventory->FindItem( ITEM_CLASS_LARVA, i );
						if(pLarvaItem != NULL)
							break;
					}

					if(pLarvaItem == NULL)
					{
						_CGAbsorbSoul.setInvenObjectID(OBJECTID_NULL);
						_CGAbsorbSoul.setInvenX(255);
						_CGAbsorbSoul.setInvenY(255);
						_CGAbsorbSoul.setTargetInvenX(255);
						_CGAbsorbSoul.setTargetInvenY(255);
					}
					else
					{
						_CGAbsorbSoul.setInvenObjectID(pLarvaItem->GetID());
						_CGAbsorbSoul.setInvenX(pLarvaItem->GetGridX());
						_CGAbsorbSoul.setInvenY(pLarvaItem->GetGridY());

						MOustersPupa pupa;

						// ¶ó¹Ù¿Í °°Àº Å¸ÀÔÀ¸·Î
						pupa.SetItemType(pLarvaItem->GetItemType());

						POINT p;
						g_pInventory->GetFitPosition(&pupa, p);
						_CGAbsorbSoul.setTargetInvenX(p.x);
						_CGAbsorbSoul.setTargetInvenY(p.y);

						// (!!!) °ËÁõ packetÀ» ¹ÞÀ»¶§±îÁö itemÀ» ¸ø ¿òÁ÷ÀÌµµ·Ï ÇØ¾ßÇÑ´Ù.
						// Item°ü·Ã ÇàÀ§ Áß´Ü
						SetItemCheckBuffer(pLarvaItem, ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY);
						
						// ±â¼ú »ç¿ë Áß´Ü
						// 2001.8.20 ÁÖ¼®Ã³¸®
						// 2001.10.26 ÁÖ¼®ÇØÁ¦ - ItemÀº °ËÁõµÇ¾î¾ß ÇÑ´Ù.
						SetWaitVerify( WAIT_VERIFY_SKILL_SUCCESS, m_nUsedActionInfo );
					}

					g_pSocket->sendPacket( &_CGAbsorbSoul );

					
					// 2001.8.20 ÁÖ¼®Ã³¸®
					SetWaitVerify( WAIT_VERIFY_SKILL_SUCCESS, m_nUsedActionInfo );
				}
				//----------------------------------------------------------
				//
				// ´Ù¸¥ »ç¶÷¿¡°Ô »ç¿ëÇÏ´Â packetÀ» º¸³»´Â °æ¿ì
				//
				//----------------------------------------------------------
				else if	(packetType==ACTIONINFO_PACKET_OTHER
						||	(m_fTraceBuffer & FLAG_TRACE_CREATURE_SPECIAL))
				{
					#ifndef __EXPO_CLIENT__

						//-----------------------------------------------
						// itemÀ» »ç¿ëÇØ¼­ skillÀ» »ç¿ëÇÏ´Â °æ¿ì
						//-----------------------------------------------					
						if ((*g_pActionInfoTable)[m_nUsedActionInfo].GetWeaponType()
							& FLAG_ACTIONINFO_WEAPON_HOLY_WATER)
						{						
							MItem* pUsingItem = g_pInventory->FindItem( ITEM_CLASS_HOLYWATER );
							
							//-----------------------------------------------
							// »ç¿ëÇÒ itemÀÌ ¾ø´Â °æ¿ì
							//-----------------------------------------------
							if (pUsingItem==NULL)
							{
								//-------------------------------------------------
								// Skill Icon Ã¼Å© - ÇÊ¿äÇÒ·Á³ª.. - -;
								//-------------------------------------------------
								g_pSkillAvailable->SetAvailableSkills();

								// ÃßÀû ÁßÁö
								TraceNULL();

								//------------------------------------------
								// Á¦ÀÚ¸®¿¡¼­ Èçµé°Å¸®´Â ¸ð½À
								//------------------------------------------
								SetAction( ACTION_STAND );

								m_ActionCount++;

								m_nUsedActionInfo = ACTIONINFO_NULL;
	
								return;
							}

							CGThrowItem	_CGThrowItem;
							_CGThrowItem.setObjectID( pUsingItem->GetID() );	// item id
							_CGThrowItem.setTargetObjectID( m_TraceID );	//
							_CGThrowItem.setX( pUsingItem->GetGridX() );
							_CGThrowItem.setY( pUsingItem->GetGridY() );

							g_pSocket->sendPacket( &_CGThrowItem );						
							
							//--------------------------------------------------
							// 1°³ ÀÌ»óÀÎ °æ¿ì¿¡´Â °³¼ö¸¸ ÁÙÀÎ´Ù.
							//--------------------------------------------------
							if (pUsingItem->GetNumber()>1)
							{
								pUsingItem->SetNumber( pUsingItem->GetNumber() - 1 );
							}
							//--------------------------------------------------
							// 1°³¸¸ ³²Àº °æ¿ì´Â ItemÀ» ¿ÏÀüÈ÷ Á¦°ÅÇÑ´Ù
							//--------------------------------------------------
							else
							{
								MItem* pRemovedItem = g_pInventory->RemoveItem( pUsingItem->GetGridX(), pUsingItem->GetGridY() );

								if (pRemovedItem!=NULL)
								{
									// itemÁ¤º¸ Á¦°Å
									UI_RemoveDescriptor( (void*)pRemovedItem );

									delete pRemovedItem;								
								}
								else
								{
									DEBUG_ADD_FORMAT("[Error] Cannot Remove Item to use Skill. id=%d", pUsingItem->GetID());
								}

								//-------------------------------------------------
								// Skill Icon Ã¼Å©
								//-------------------------------------------------
								g_pSkillAvailable->SetAvailableSkills();
							}
						}
						//-----------------------------------------------
						// ÆøÅºÀ» »ç¿ëÇØ¼­ skillÀ» »ç¿ëÇÏ´Â °æ¿ì
						//-----------------------------------------------					
						if ((*g_pActionInfoTable)[m_nUsedActionInfo].GetWeaponType()
							& FLAG_ACTIONINFO_WEAPON_BOMB)
						{		
							int itemType;

							MItem* pUsingItem = NULL;

							if (m_nUsedActionInfo==SKILL_THROW_BOMB)
							{
								pUsingItem = g_pInventory->FindItem( ITEM_CLASS_BOMB );
							}
							else
							{
								itemType = m_nUsedActionInfo - BOMB_SPLINTER;

								pUsingItem = g_pInventory->FindItem( ITEM_CLASS_BOMB, itemType );
							}
							
							//-----------------------------------------------
							// »ç¿ëÇÒ itemÀÌ ¾ø´Â °æ¿ì
							//-----------------------------------------------
							if (pUsingItem==NULL)
							{
								//-------------------------------------------------
								// Skill Icon Ã¼Å© - ÇÊ¿äÇÒ·Á³ª.. - -;
								//-------------------------------------------------
								g_pSkillAvailable->SetAvailableSkills();

								// ÃßÀû ÁßÁö
								TraceNULL();

								//------------------------------------------
								// Á¦ÀÚ¸®¿¡¼­ Èçµé°Å¸®´Â ¸ð½À
								//------------------------------------------
								SetAction( ACTION_STAND );

								m_ActionCount++;

								m_nUsedActionInfo = ACTIONINFO_NULL;
	
								return;
							}

							CGThrowBomb	cgThrowBomb;

							cgThrowBomb.setZoneX( m_TraceX );
							cgThrowBomb.setZoneY( m_TraceY );
							cgThrowBomb.setBombX( pUsingItem->GetGridX() );
							cgThrowBomb.setBombY( pUsingItem->GetGridY() );
							cgThrowBomb.setAttackSlayerFlag( m_TraceID!=OBJECTID_NULL );

							g_pSocket->sendPacket( &cgThrowBomb );						
							
							//--------------------------------------------------
							// 1°³ ÀÌ»óÀÎ °æ¿ì¿¡´Â °³¼ö¸¸ ÁÙÀÎ´Ù.
							//--------------------------------------------------
							if (pUsingItem->GetNumber()>1)
							{
								pUsingItem->SetNumber( pUsingItem->GetNumber() - 1 );
							}
							//--------------------------------------------------
							// 1°³¸¸ ³²Àº °æ¿ì´Â ItemÀ» ¿ÏÀüÈ÷ Á¦°ÅÇÑ´Ù
							//--------------------------------------------------
							else
							{
								MItem* pRemovedItem = g_pInventory->RemoveItem( pUsingItem->GetGridX(), pUsingItem->GetGridY() );

								if (pRemovedItem!=NULL)
								{
									// itemÁ¤º¸ Á¦°Å
									UI_RemoveDescriptor( (void*)pRemovedItem );

									delete pRemovedItem;								
								}
								else
								{
									DEBUG_ADD_FORMAT("[Error] Cannot Remove Item to use Skill. id=%d", pUsingItem->GetID());
								}

								//-------------------------------------------------
								// Skill Icon Ã¼Å©
								//-------------------------------------------------
								g_pSkillAvailable->SetAvailableSkills();
							}
						}
						else
						{
							int SkillType = m_nUsedActionInfo; 
							if( (*g_pActionInfoTable)[SkillType].GetParentActionInfo() != ACTIONINFO_NULL )
								SkillType = (*g_pActionInfoTable)[SkillType].GetParentActionInfo();
							// 2005.07.13 Combo Skill Add Sjheon	
							//if(!(*g_pActionInfoTable)[SkillType].GetComboAttackSkill())  
							//	m_ComboCnt = 0  ;
							//else
							//	m_ComboCnt = GetComboEndValue(); 
							// 2005.07.13 Combo Skill End Sjheon


							if(SkillType == SKILL_TELEPORT || SkillType == SKILL_CHARGING_ATTACK )//|| SkillType == SKILL_WARP_PORTAL)//|| SkillType == SKILL_ADV2_SUDDEN_BLAST)
							{
								g_pPlayer->SetDelay(1000);  
							}

							CGSkillToObject _CGSkillToObject;
							_CGSkillToObject.setSkillType( SkillType );
							_CGSkillToObject.setCEffectID( m_pEffectTarget->GetEffectID() );			
							_CGSkillToObject.setTargetObjectID( m_TraceID );
							_CGSkillToObject.setActionStep(GetActionStep());

								//_CGSkillToObject.setCombo(m_ComboCnt); // 2005.07.13  // ÄÞº¸ Ä«¿îÆ® 	
							g_pSocket->sendPacket( &_CGSkillToObject );	

//Test - 071207
/*
#ifdef __DEBUG_OUTPUT__
				static test_LastTime = 0;
				static test_LastSkillType = 0;

				if (test_LastSkillType == SkillType)
				{
					char test_SkillName[128];
					switch                           (SkillType)
					{
					case 378: wsprintf (test_SkillName, " µµ : ºí·¹ÀÌµåÄÞº¸(Blade Combo) : 10 ~ 10"); break;
					case 405: wsprintf (test_SkillName, " µµ : ¹ü Å©·¡½¬ ¿öÅ©(Bomb Crash Walk) : 300 ~ 300"); break;
					case 254: wsprintf (test_SkillName, " µµ : ºí·¹ÀÌÁî ¿öÅ©(Blaze Walk) : 30 ~ 50"); break;
					case 129: wsprintf (test_SkillName, " µµ : ¹®¶óÀÌÆ® ¼¼¹ö(Moonlight Sever) : 8 ~ 8"); break;
					case 27: wsprintf (test_SkillName, " µµ : Åä³×ÀÌµµ ¼¼¹ö(Tornado Server) : 10 ~ 20"); break;
					case 26: wsprintf (test_SkillName, " µµ : Çã¸®ÄÉÀÎ ÄÞº¸(Hurricane Combo) : 10 ~ 10"); break;
					case 21: wsprintf (test_SkillName, " µµ : ¿ÍÀÏµå ½º¸Å½¬(Wild Smash) : 10 ~ 10"); break;
					case 20: wsprintf (test_SkillName, " µµ : Æ®¸®ÇÃ ºê·¹ÀÌÅ©(Triple Break) : 10 ~ 10"); break;
					case 19: wsprintf (test_SkillName, " µµ : ½ºÆÄÀÌ·² ½½·¹ÀÌ(Spiral Slay) : 10 ~ 10"); break;
					case 18: wsprintf (test_SkillName, " µµ : ½Ì±Û ºí·Î¿ì(Single Blow) : 10 ~ 10"); break;

					case 404: wsprintf (test_SkillName, " °Ë : »þÀÎ ½º¿öµå(Shine Swork) : 100 ~ 450"); break;
					case 377: wsprintf (test_SkillName, " °Ë : ¼Òµå ÄÞº¸(Sword Combo) : 10 ~ 10"); break;
					case 253: wsprintf (test_SkillName, " °Ë : ºí¸®Ã÷ ½½¶óÀÌµù(Blitz Sliding) : 20 ~ 40"); break;
					case 12: wsprintf (test_SkillName, " °Ë : ¶óÀÌÆ®´× ÇÚµå(Lightning Hand) : 4 ~ 4"); break;
					case 14: wsprintf (test_SkillName, " °Ë : ¼Òµå ¿þÀÌºê(SWORD Wave) : 10 ~ 10"); break;
					case 13: wsprintf (test_SkillName, " °Ë : ½º³×ÀÌÅ© ÄÞº¸(Snake Combo) : 10 ~ 10"); break;
					case 7: wsprintf (test_SkillName, " °Ë : ·¹ÀÎº¸¿ì ½½·¡¼Å(Rainbow Slasher) : 10 ~ 10"); break;
					case 10: wsprintf (test_SkillName, " °Ë : Å©·Î½º Ä«¿îÅÍ(Cross Counter) : 20 ~ 50"); break;
					case 8: wsprintf (test_SkillName, " °Ë : ½ã´õ ½ºÆÄÅ©(Thunder Spark) : 10 ~ 10"); break;
					case 6: wsprintf (test_SkillName, " °Ë : Æ®¸®ÇÃ ½½·¡¼Å(Triple Slasher) : 10 ~ 10"); break;
					case 5: wsprintf (test_SkillName, " °Ë : ´õºí ÀÓÆÑÆ®(Double Impact) : 10 ~ 10"); break;

					case 406: wsprintf (test_SkillName, " ÃÑ : ¼¼ÅÍ¶óÀÌÆ® ¹ü(Satellite Bomb) : 100 ~ 450"); break;

					case 417: wsprintf (test_SkillName, " °øÅë : °¡Á× ¹þ±â±â(Tanning) : 400 ~ 400"); break;

					case 379: wsprintf (test_SkillName, " ¹ìÆÄ : ¹ìÆÄÀÌ¾î ÄÞº¸(Vampire Combo) : 10 ~ 10"); break;
					case 328: wsprintf (test_SkillName, " ¹ìÆÄ : ¹ÙÀÌ¾î·±Æ® ÆÒÅÒ(Violent Phantom) : 0 ~ 0"); break;
					case 414: wsprintf (test_SkillName, " ¹ìÆÄ : ÇÇÀÇ ÀúÁÖ(Blood Curse) : 450 ~ 450"); break;
					case 276: wsprintf (test_SkillName, " ¹ìÆÄ : Å»·Ð ¿Àºê Å©·Î¿ì(Talon OF Crow) : 6 ~ 6"); break;
					case 93: wsprintf (test_SkillName, " ¹ìÆÄ : ºí·¯µð ³×ÀÏ(Bloody Nail) : 5 ~ 20"); break;
					case 85: wsprintf (test_SkillName, " ¹ìÆÄ : ¾Ö½Ãµå ÅÍÄ¡(Acid Touch) : 6 ~ 6"); break;
					case 199: wsprintf (test_SkillName, " ¹ìÆÄ : ÇÚÁî ¿Àºê À§Áî´ý(Hands OF Wisdon) : 6 ~ 6"); break;
					case 80: wsprintf (test_SkillName, " ¹ìÆÄ : Æ÷ÀÌÁî³Ê½º ÇÚÁî(Poisonous Hands) : 6 ~ 6"); break;

					case 365: wsprintf (test_SkillName, " ¾Æ¿ì : ÇìÀÏ·Î(Halo) : 150 ~ 50"); break;
					case 380: wsprintf (test_SkillName, " ¾Æ¿ì : ¾Æ¿ì½ºÅÍÁî ÄÞº¸(Ousters Combo) : 10 ~ 10"); break;
					case 397: wsprintf (test_SkillName, " ¾Æ¿ì : ÈÖµÎ¸£±â(Brandish) : 50 ~ 200"); break;
					case 219: wsprintf (test_SkillName, " ¾Æ¿ì : ÇÃ·¯¸®½¬(Flourish) : 10 ~ 10"); break;
					case 400: wsprintf (test_SkillName, " ¾Æ¿ì : ÆÄÀÌ¾î ¿þÀÌºê(Fire Wave) : 50 ~ 200"); break;
					case 223: wsprintf (test_SkillName, " ¾Æ¿ì : ¹é½ºÅÇ(Back Stab) : 10 ~ 10"); break;
					case 304: wsprintf (test_SkillName, " ¾Æ¿ì : µð½ºÅÏ½º ºí¸®Ã÷(Distance Blitz) : 10 ~ 70"); break;
					case 225: wsprintf (test_SkillName, " ¾Æ¿ì : °¨¸¶ ÃÏ(Cammna Chop) : 10 ~ 40"); break;
					case 246: wsprintf (test_SkillName, " ¾Æ¿ì : Èí¿µ(Absorb Soul) : 0 ~ 0"); break;
					case 221: wsprintf (test_SkillName, " ¾Æ¿ì : »þÇÁ ¶ó¿îµå(Sharp Round) : 10 ~ 10"); break;
					case 296: wsprintf (test_SkillName, " ¾Æ¿ì : ½ÃÇÁÆ® ºê·¹ÀÌÅ©(Shift Break) : 10 ~ 50"); break;
					case 297: wsprintf (test_SkillName, " ¾Æ¿ì : ¿öÅÍ ½Çµå(Water Shield) : 10 ~ 10"); break;
					case 298: wsprintf (test_SkillName, " ¾Æ¿ì : µð½ºÆ®·°¼Ç ½ºÇÇ¾î(Destruction Spear) : 10 ~ 50"); break;
					case 299: wsprintf (test_SkillName, " ¾Æ¿ì : ºí·¹½º ÆÄÀÌ¾î(Bless Fire) : 10 ~ 10"); break;
					case 300: wsprintf (test_SkillName, " ¾Æ¿ì : ÆäÀÌÆ² ½º´ÏÅ©(Fatal Snick) : 10 ~ 10"); break;
					case 301: wsprintf (test_SkillName, " ¾Æ¿ì : »÷µå Å©·Î½º(Sand Cross) : 10 ~ 10"); break;
					case 302: wsprintf (test_SkillName, " ¾Æ¿ì : ´õÅ· ¿ù·¯ÇÁ(Ducking Wallop) : 20 ~ 100"); break;
					case 303: wsprintf (test_SkillName, " ¾Æ¿ì : Â÷Â¡ ¾îÅÃ(Charging Attack) : 50 ~ 100"); break;
					case 399: wsprintf (test_SkillName, " ¾Æ¿ì : À½ÆÄ°ø°Ý(Sound Wave) : 50 ~ 200"); break;
					
					default : wsprintf (test_SkillName, " ¹Ì µî·Ï"); break;
					}
					int		StartFrame		= (*g_pActionInfoTable)[m_nUsedActionInfo].GetStartFrame( m_WeaponSpeed );

					std::ofstream file("SkillDelay.log", std::ios::out | std::ios::app);
					file << "*** SkillType=" << SkillType << test_SkillName << " ***" << std::endl;//"LastTime=" << test_LastTime << ", CurrentTime=" << g_CurrentTime << std::endl;
					file << "     WeaponSpeed=" << m_WeaponSpeed << "StartFrame =" << StartFrame << std::endl;
					file << "     Difftime=" << g_CurrentTime - test_LastTime << std::endl;
					
					file.close();
					test_LastTime = g_CurrentTime;
				}
				else
				{
					test_LastSkillType = SkillType;
					test_LastTime = g_CurrentTime;
				}
#endif
*/							
							//char cstrOutput[256] ; 
							//wsprintf(cstrOutput , "GCSkillToObjectOK1Handler _Seder() %d\n" ,SkillType  ) ; 

							//OutputDebugString(cstrOutput) ; 
						}

					#else
					// ÀÓ½Ã·Î..
					// BOMB±â¼úÀº targetÀÌ object¿Í tileÀÌ¹Ç·Î..
					// ¹«Á¶°Ç tile·Î º¸³»ÁØ´Ù.
						if (m_nUsedActionInfo==BOMB_TWISTER)
						{
							int SkillType = m_nUsedActionInfo;
							if( (*g_pActionInfoTable)[SkillType].GetParentActionInfo() != ACTIONINFO_NULL )
								SkillType = (*g_pActionInfoTable)[SkillType].GetParentActionInfo();

							CGSkillToTile _CGSkillToTile;
							_CGSkillToTile.setSkillType( SkillType ); 
							_CGSkillToTile.setCEffectID( m_pEffectTarget->GetEffectID() );
							_CGSkillToTile.setX( m_TraceX );
							_CGSkillToTile.setY( m_TraceY );
							
							g_pSocket->sendPacket( &_CGSkillToTile );
						}
						else
						{
							int SkillType = m_nUsedActionInfo;
							if( (*g_pActionInfoTable)[SkillType].GetParentActionInfo() != ACTIONINFO_NULL )
								SkillType = (*g_pActionInfoTable)[SkillType].GetParentActionInfo();

							if(SkillType == SKILL_TELEPORT || SkillType == SKILL_CHARGING_ATTACK )// || SkillType == SKILL_WARP_PORTAL)//|| SkillType == SKILL_ADV2_SUDDEN_BLAST)
							{
								g_pPlayer->SetDelay(1000);
							}

							CGSkillToObject _CGSkillToObject;
							_CGSkillToObject.setSkillType( SkillType );
							_CGSkillToObject.setCEffectID( m_pEffectTarget->GetEffectID() );			
							_CGSkillToObject.setTargetObjectID( m_TraceID );
							_CGSkillToObject.setSkillStep(GetSkillStep());
							
							g_pSocket->sendPacket( &_CGSkillToObject );
						}
					#endif
						
					// 2001.8.20 ÁÖ¼®Ã³¸®
					SetWaitVerify( WAIT_VERIFY_SKILL_SUCCESS, m_nUsedActionInfo );
				}
				//----------------------------------------------------------
				//
				//			ÀÚ½Å¿¡°Ô »ç¿ëÇÏ´Â °ÍÀÌ¸é..
				//
				//----------------------------------------------------------
				else if (packetType==ACTIONINFO_PACKET_SELF
						|| (m_fTraceBuffer & FLAG_TRACE_SELF))
				{
					//----------------------------------------------------------
					// OLD_PACKET
					//----------------------------------------------------------
					///*
					int originalActionInfo = m_nUsedActionInfo;
					if( (*g_pActionInfoTable)[originalActionInfo].GetParentActionInfo() != ACTIONINFO_NULL )
						originalActionInfo = (*g_pActionInfoTable)[originalActionInfo].GetParentActionInfo();

					CGSkillToSelf _CGSkillToSelf;
					_CGSkillToSelf.setSkillType( originalActionInfo );
					_CGSkillToSelf.setCEffectID( m_pEffectTarget->GetEffectID() );
					g_pSocket->sendPacket( &_CGSkillToSelf );
				// 2001.8.20 ÁÖ¼®Ã³¸®
					SetWaitVerify( WAIT_VERIFY_SKILL_SUCCESS, m_nUsedActionInfo );
				//*/

				}
				//----------------------------------------------------------
				//
				//				Tile¿¡ »ç¿ëÇÏ´Â °ÍÀÌ¸é..
				//
				//----------------------------------------------------------
				else if (packetType==ACTIONINFO_PACKET_ZONE
						|| (m_fTraceBuffer & FLAG_TRACE_SECTOR_BASIC) 
						|| (m_fTraceBuffer & FLAG_TRACE_SECTOR_SPECIAL))
				{
					//----------------------------------------------------------
					// OLD_PACKET
					//----------------------------------------------------------
					///*
					int SkillType = m_nUsedActionInfo;
					if( (*g_pActionInfoTable)[SkillType].GetParentActionInfo() != ACTIONINFO_NULL )
						SkillType = (*g_pActionInfoTable)[SkillType].GetParentActionInfo();

					if(SkillType == SKILL_TELEPORT || SkillType == SKILL_CHARGING_ATTACK  )//|| SkillType == SKILL_WARP_PORTAL)//|| SkillType == SKILL_ADV2_SUDDEN_BLAST)
					{
						g_pPlayer->SetDelay(1000);
					}
					else if(SkillType == SKILL_SWEEP_VICE_1)
					{
						if(1 == GetSweepViewValue())
							SkillType = SKILL_SWEEP_VICE_1;
						else if(2 == GetSweepViewValue())
							SkillType = SKILL_SWEEP_VICE_3;
						else if(3 == GetSweepViewValue())
							SkillType = SKILL_SWEEP_VICE_5;
						m_TraceX = m_SweepViceX;
						m_TraceY = m_SweepViceY;
					}
					else 	if(SkillType == SKILL_BURNING_SOUL_LAUNCH)
					{
						m_TraceX = m_TempSelectPosX;
						m_TraceY = m_TempSelectPosY;
					}

					CGSkillToTile _CGSkillToTile;	
					_CGSkillToTile.setSkillType( SkillType );
					_CGSkillToTile.setCEffectID( m_pEffectTarget->GetEffectID() );
					_CGSkillToTile.setX( m_TraceX );
					_CGSkillToTile.setY( m_TraceY );
					g_pSocket->sendPacket( &_CGSkillToTile );

// 					char szBuf[256];
// 					sprintf(szBuf, "Send CGSkillToTile: TX = %d, TY = %d\n", m_TraceX, m_TraceY);
// 					OutputDebugString(szBuf);

					// 2001.8.20 ÁÖ¼®Ã³¸®
					SetWaitVerify( WAIT_VERIFY_SKILL_SUCCESS, m_nUsedActionInfo );
					//*/
				}
				//----------------------------------------------------------
				//
				//				Inventory¿¡ »ç¿ëÇÏ´Â °ÍÀÌ¸é..
				//
				//----------------------------------------------------------
				else if (packetType==ACTIONINFO_PACKET_ITEM
						|| (m_fTraceBuffer & FLAG_TRACE_INVENTORY))
				{
					MItem* pItem = g_pInventory->GetItemToModify( m_TraceID );

					if (pItem!=NULL && IsItemCheckBufferNULL())
					{
						// inventoryÀÇ Item¿¡ ±â¼úÀ» »ç¿ëÇÑ´Ù°í packetÃ³¸®¸¦ ÇØÁÖ¸é µÈ´Ù.
						CGSkillToInventory _CGSkillToInventory;
						
						_CGSkillToInventory.setSkillType( m_nUsedActionInfo );
						//_CGSkillToInventory.setCEffectID( m_pEffectTarget->GetEffectID() );
						_CGSkillToInventory.setObjectID( m_TraceID );
						_CGSkillToInventory.setX( pItem->GetGridX() );
						_CGSkillToInventory.setY( pItem->GetGridY() );

							
						// (!!!) °ËÁõ packetÀ» ¹ÞÀ»¶§±îÁö itemÀ» ¸ø ¿òÁ÷ÀÌµµ·Ï ÇØ¾ßÇÑ´Ù.
						// Item°ü·Ã ÇàÀ§ Áß´Ü
						SetItemCheckBuffer(pItem, ITEM_CHECK_BUFFER_SKILL_TO_INVENTORY);
						
						// ±â¼ú »ç¿ë Áß´Ü
						// 2001.8.20 ÁÖ¼®Ã³¸®
						// 2001.10.26 ÁÖ¼®ÇØÁ¦ - ItemÀº °ËÁõµÇ¾î¾ß ÇÑ´Ù.
						SetWaitVerify( WAIT_VERIFY_SKILL_SUCCESS, m_nUsedActionInfo );
					}
					
				}

				
			}		
		}


		//----------------------------------------------------------------------
		// Packet °á°ú¸¦ ¹ÞÀ» °æ¿ì¿¡..
		//----------------------------------------------------------------------
		// Client¿¡¼­ º¸³½ Instance ID¸¦ °°ÀÌ ¹Þ¾Æ¼­..
		// ±× ID°¡ ÇöÀç ÀúÀåµÈ InstanceID¿Í °°À¸¸é .. ¾ÆÁ÷ ±× ActionInfo°¡
		// ÁøÇàÁßÀÌ¶ó´Â ÀÇ¹ÌÀÌ´Ù. PlayerÀÇ ActionInfo°¡ ³¡³¯¶§´Â
		// Instance ID¸¦ 0À¸·Î? ÇØ¾ßÇÏÁö ¾ÊÀ»±î..
		//
		// °°À¸¸é.. Server¿¡¼­ ³¯¾Æ¿Â ActionInfoÀÇ °á°ú¸¦ ±â¾ï½ÃÄÑµÎ°í  
		//          ³ªÁß¿¡~~~ Ãâ·ÂÇÏ°í..(ActionInfo°¡ ³¡³­ÈÄ¿¡)
		// ´Ù¸£¸é.. Áï½Ã Ãâ·ÂÇÏ¸é µÈ´Ù.
		//
		// ÀÌ·¸°Ô °­Á¦·Î ½Ã°£Â÷¸¦ µÎ¾î¼­.. PacketÃ³¸®¿¡ µû¸¥ Èå¸§À» ¸¸µé¾î¾ß ÇÑ´Ù.
		//----------------------------------------------------------------------

		// castingµ¿ÀÛÀÌ ¿ø·¡´Â ¿©±â¼­ Ç¥ÇöµÆ´Ù. - -;

		//------------------------------------------------
		// ±â¼úÀÇ µ¿ÀÛ¿¡ ¸Â´Â sound¸¦ Ãâ·ÂÇØÁØ´Ù.
		//------------------------------------------------
		//g_Sound.Play( g_SoundTable[(*g_pActionInfoTable)[m_nUsedActionInfo].GetSoundID()].pDSBuffer );
		//PlaySound( (*g_pActionInfoTable)[m_nUsedActionInfo].GetSoundID() ,
		//			false,
		//			m_X, m_Y);	

		//------------------------------------------------
		// Ä³¸¯ÅÍÀÇ Action¿¡ ¸Â´Â Sound¸¦ Ãâ·ÂÇØÁØ´Ù.
		//------------------------------------------------
		/*int soundID  = 0 ; 

		int 	UseAction = (*g_pActionInfoTable)[m_nUsedActionInfo].GetAction();
		
		if(UseAction == ACTION_MAGIC && IsAdvancementClass())
		{
			if(!(*g_pActionInfoTable)[m_nUsedActionInfo].IsTargetSelf())
			{
				soundID = (*g_pCreatureTable)[m_CreatureType].GetActionSound(UseAction );
			}
		}
		else
		{
			//int soundID = (*g_pCreatureTable)[m_CreatureType].GetActionSound( 
			//							GetActionInfoAction(m_nUsedActionInfo) );

			soundID = (*g_pCreatureTable)[m_CreatureType].GetActionSound( 
										GetActionInfoAction(m_nUsedActionInfo) );

		}*/
		
		int soundID = (*g_pCreatureTable)[m_CreatureType].GetActionSound( 
										GetActionInfoAction(m_nUsedActionInfo,  false , true) );


		if(!HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET) && !m_bBurningSol )
			PlaySound( soundID,	false, m_X, m_Y	);

		//-------------------------------------------------
		// Force Feel
		//-------------------------------------------------
		if (g_pUserOption->UseForceFeel && gpC_Imm!=NULL && gpC_Imm->IsDevice())				
		{				
			if ((*g_pActionInfoTable)[m_nUsedActionInfo].IsWeaponTypeGunAny())
			{
				soundID = SOUND_SLAYER_ATTACK_AR;
			}

			if (soundID < g_pSoundTable->GetSize())
			{
				DEBUG_ADD_FORMAT("ForceAction-ActionToSendPacket(%d, %s)", soundID, strrchr((*g_pSoundTable)[soundID].Filename.GetString(), '\\'));
				gpC_Imm->ForceAction( soundID );
			}
		}
	}
}

//----------------------------------------------------------------------
// Set ServerPosition
//----------------------------------------------------------------------
void		
MPlayer::SetServerPosition(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)
{
	//--------------------------------------------------------
	// ¸ðµÎ Á¦°Å..
	//--------------------------------------------------------
	m_listSendDirection.clear();

	m_SendMove = 0;
	
	g_bNetStatusGood = true;

	//--------------------------------------------------------
	// Server¿¡¼­ °ËÁõµÈ À§Ä¡¸¦ ±â¾ïÇØµÐ´Ù.
	//--------------------------------------------------------
	m_ServerX	= sX;
	m_ServerY	= sY;

	//--------------------------------------------------------
	// (m_ServerX, m_ServerY)¿¡¼­ºÎÅÍ 
	// PlayerÀÇ ½Ã¾ß¿¡ Æ÷ÇÔµÇÁö ¾Ê´Â Creature¸¦ Zone¿¡¼­ Á¦°Å½ÃÅ²´Ù.
	//--------------------------------------------------------
	if (m_pZone!=NULL)
	{
		m_pZone->KeepObjectInSight(m_ServerX, m_ServerY, m_Sight);
	}

	SendPositionInfoToParty();
}

//----------------------------------------------------------------------
// Action Effect
//----------------------------------------------------------------------
// µ¿ÀÛÀÌ ¸ðµÎ ³¡³¯¶§¿¡ EffectÈ¿°ú¸¦ ¼öÇàÇÑ´Ù.
//----------------------------------------------------------------------
void	
MPlayer::ActionEffect()
{
	/*
	// [ÀûÀýÇÑ Å¸ÀÌ¹Ö]¿¡ ÆÐÅ¶À» º¸³»´Â ÄÚµå
	// 
	BOOL	bActionStand	= (m_Action==ACTION_STAND || IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND);
	int		actionCountMax_1 = m_ActionCountMax-1;
	BOOL	bEndAction		= (m_ActionCount==actionCountMax_1);// || bActionStand;
	int		StartFrame		= (*g_pActionInfoTable)[m_nUsedActionInfo].GetStartFrame( m_WeaponSpeed );
	
	// µ¿ÀÛ ³¡ºÎºÐ¿¡¼­ ÆÐÅ¶ º¸³½´Ù. 2001.8.1
	int		packetSendTiming = min(actionCountMax_1, StartFrame);
	BOOL	bStartAction	= (m_ActionCount==packetSendTiming);	// packetSendTimingÀÌ ¿ø·¡´Â 0ÀÌ¾ú´Ù.
	
	if (bActionStand)
	{
		bEndAction = bStartAction;
	}
	*/

	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD("ActionEffect");
	#endif
/*
#if __CONTENTS(__GEAR_SWAP_CHANGE)		//MPlayer::ActionEffect()
		bool	bGearChange	= false;
		switch(g_pPlayer->GetRace())
		{
		case RACE_SLAYER:
			bGearChange	= g_pSlayerGear->GetProcessingGearChange();
			break;	

		case RACE_VAMPIRE:
			bGearChange	= g_pVampireGear->GetProcessingGearChange();
			break;
				
		case RACE_OUSTERS:
			bGearChange	= g_pOustersGear->GetProcessingGearChange();
			break;
		}

		if(bGearChange)
			return;
#endif
*/
	// 2002.3.12
	// packetÀ» º¸³»¾ßÇÏ´Â ½ÃÀÛ µ¿ÀÛÀÇ ±â¼úÀÌ¸é..
	if (m_nUsedActionInfo < g_pActionInfoTable->GetMinResultActionInfo())
	{
		// °Å¸®°¡ µÇ´Â °æ¿ì¸¸ ActionEffect¸¦ ¼öÇàÇÑ´Ù.		
		if (max(abs(m_X-m_TraceX), abs(m_Y-m_TraceY)) 
			<= m_TraceDistance)
		{
			#ifdef OUTPUT_DEBUG_PLAYER_ACTION
				DEBUG_ADD("inDist");
			#endif
		}
		else
		{
			#ifdef OUTPUT_DEBUG_PLAYER_ACTION
				DEBUG_ADD("not inDist");
			#endif

			// °Å¸®°¡ ¾È µÇ¸é? 
			//if (m_listDirection.empty())
			{
				if (m_DestX != SECTORPOSITION_NULL
					&& m_DestY != SECTORPOSITION_NULL)
				{
					// ÇöÀç °¡°í ÀÖ´Â ¸ñÇ¥°¡ ÀÖ´Â °æ¿ì
				}
				else if (abs((int)m_X-m_TraceX) < SECTOR_WIDTH
						&& abs((int)m_Y-m_TraceY) < SECTOR_HEIGHT)
				{
					// ÃßÀûÇÏ´Â °æ¿ì
					m_DestX = m_TraceX;
					m_DestY = m_TraceY;
				}
				
				if (SetDestination(m_TraceX, m_TraceY))
				{
					#ifdef OUTPUT_DEBUG_PLAYER_ACTION
						DEBUG_ADD("NewWayOK");
					#endif

					// °¥ ±æÀÌ ÀÖ´Â °æ¿ì
					m_nNextUsedActionInfo = m_nUsedActionInfo;
					m_nUsedActionInfo = ACTIONINFO_NULL;
					
					SetAction( ACTION_STAND );
				}
				else
				{				
					// °¥±æÀÌ ¾ø´Â °æ¿ì
					// ¶Ç ÀÌ·± ÄÚµå°¡ ´Ã¾ú´Ù. ÂÁ.. ½Ï ¶â¾î°íÃÄ¾ßµÇ´Âµ¥ --;
					SetAction(((m_MoveDevice==MOVE_DEVICE_WALK)? ACTION_STAND : ACTION_SLAYER_MOTOR_STAND));
					SetNextAction(ACTION_STAND);
					m_fTraceBuffer = 0;
					m_fNextTrace = 0;
					m_bTraceCreatureToForceAttack = false;
					SetStop();
					UnSetRepeatAction();
					TraceNULL();
					m_nUsedActionInfo = ACTIONINFO_NULL;
					m_nNextUsedActionInfo = ACTIONINFO_NULL;

					#ifdef OUTPUT_DEBUG_PLAYER_ACTION
						DEBUG_ADD("Stop!");
					#endif

					return;
				}

				return;
			}
		}
	}

	BOOL	bActionStand	= (m_Action==ACTION_STAND || IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND);
	int		actionCountMax_1 = m_ActionCountMax-1;
	BOOL	bEndAction		= (m_ActionCount==actionCountMax_1) || bActionStand;
	int		StartFrame		= (*g_pActionInfoTable)[m_nUsedActionInfo].GetStartFrame( m_WeaponSpeed );

	BOOL	bStartAction	= (m_ActionCount == 0 && !IsGunShotGuidance());
	bool	bTest			= IsGunShotGuidance();
//	BOOL	bStartAction	= (m_ActionCount == StartFrame && !IsGunShotGuidance());
	
	// Effect°¡ ½ÃÀÛµÇ´Â °æ¿ì´Â..
	// (1) StartFrameÀÎ °æ¿ì
	// (2) ¸¶Áö¸· ActionFrameÀÎ °æ¿ì

	BOOL	bStartEffect	= m_ActionCount==StartFrame || StartFrame >= m_ActionCountMax && bEndAction || m_nUsedActionInfo == 518;// || m_nUsedActionInfo == 578;

	BOOL	bRepeatAction	= m_bRepeatAction 
							  && (*g_pActionInfoTable)[m_nUsedActionInfo].IsUseRepeatFrame()
							  && m_RepeatCount != 0
							  && m_ActionCount == GetActionInfoRepeatStartFrame( m_nUsedActionInfo );

	// ¹Ýº¹ actionÀÇ ½ÃÀÛ frame
	bStartAction |= bRepeatAction;

	//BOOL	bCastingEffect	= GetActionInfoCastingStartFrame(m_nUsedActionInfo)==m_ActionCount;
	

	//----------------------------------------------------------------------
	// [ TEST CODE ]
	//----------------------------------------------------------------------
	// »ó´çÈ÷ À§ÇèÇÑ ÁþÀÌÁö¸¸.. - -;;
	// Å©°Ô ¹®Á¦´Â ¾øÀ» µí...
	//----------------------------------------------------------------------
	// EffectTargetÀº castingµ¿ÀÛÀÌ ³¡³­ µÚ¿¡ »ý¼ºµÇ´Âµ¥,
	// EffectTargetÀÇ ID¸¦ casting½ÃÀÛ¶§¿¡ server·Î º¸³»¾ß ÇÏ±â ¶§¹®¿¡
	// ÀÓ½Ã·Î... EffectTargetÀ» ¸ÕÀú »ý¼º½ÃÄÑµÎ°í »ç¿ëÇß´Ù.
	//----------------------------------------------------------------------
	// À½. data member·Î ¹Ù²å´Ù. - -;;
	//static MEffectTarget* 

	
	#ifdef OUTPUT_DEBUG
			//DEBUG_ADD_FORMAT("action=(%d/%d) ActionInfo=%d, pEffectTarget=%x", m_ActionCount, m_ActionCountMax, m_nUsedActionInfo, m_pEffectTarget);
	#endif

	int SkillInfo = m_nUsedActionInfo;
	if(SkillInfo == SKILL_SWEEP_VICE_1)
	{
		if(!GetSweepViewValue() && g_bRButtonDown) //½ºÀ¬¹ÙÀÌ½º¸¦ ¾ÆÁ÷ »ç¿ëÁßÀÌ ¾Æ´Ò¶§
		{
			if(g_pSkillAvailable->IsEnableSkill( SKILL_SWEEP_VICE_1)
				&&!IsDead()
				&&!(*g_pSkillInfoTable)[SKILL_SWEEP_VICE_1].GetAvailableTimeLeft()
				)
			{
				SetSweepViewValue(1);
				g_SelectSector = g_pTopView->GetSelectedSector(g_x, g_y);

				m_TempSelectPosX = g_SelectSector.x;
				m_TempSelectPosY = g_SelectSector.y;
				m_SweepViceX = m_TraceX;
				m_SweepViceY = m_TraceY;
				ExecuteActionInfoFromMainNode(SKILL_CLIENT_SWEEP_VICE,m_SweepViceX, m_SweepViceY, 0,g_pPlayer->GetDirection(),0	,	
						m_SweepViceX, m_SweepViceY, 0, 0xffff, NULL, false);
			}
			return;
		}
		else if(g_bRButtonDown)
			return;

	//	bStartEffect = true;

		if(1 == GetSweepViewValue())
			SkillInfo = SKILL_SWEEP_VICE_1;
		else if(2 == GetSweepViewValue())
			SkillInfo = SKILL_SWEEP_VICE_3;
		else if(3 == GetSweepViewValue())
			SkillInfo = SKILL_SWEEP_VICE_5;
		else
		{
			SkillInfo = SKILL_SWEEP_VICE_1;
			SetSweepViewValue(0);
		}
	}
	
	//----------------------------------------------------------
	//
	//					µ¿ÀÛ ½ÃÀÛ ½ÃÁ¡..
	//
	//----------------------------------------------------------

	// Effect°¡ ºÙÀ»¶§ packetÀ» º¸³½´Ù.
	// µ¿ÀÛ ½ÃÀÛÇÒ¶§ packetÀ» º¸³½´Ù.
	if(bStartAction)
	{	
		const MActionInfo* pUsedActionInfo = NULL;
		if(m_nUsedActionInfo != ACTIONINFO_NULL)
			pUsedActionInfo = &(*g_pActionInfoTable)[m_nUsedActionInfo];
		
		bool bNoCastingTime = true;
//		bool bCastingAction	= false;
		WORD wCastingTime	= 0;

		if(pUsedActionInfo)
		{
//			bCastingAction	= pUsedActionInfo->IsCastingAction();
			wCastingTime	= pUsedActionInfo->GetCastingTime();
			bNoCastingTime	= /*!bCastingAction || */wCastingTime == 0;
		}

		if(bNoCastingTime)
		{
			if(HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
			{
				if(bStartEffect && g_pCurrentMagazine != NULL)
				{
					int MagazineCount = g_pCurrentMagazine->GetNumber();
					if(MagazineCount > 0)
					{
						g_pCurrentMagazine->SetNumber(MagazineCount - 1);
						ActionToSendPacket();
					}
				}
			}
			//if(HasEffectStatus(EFFECTSTATUS_INSTALL_MK1))
			//{
			//	if(bStartEffect && g_pCurrentMagazine != NULL)
			//	{
			//		int MagazineCount = g_pCurrentMagazine->GetNumber();
			//		if(MagazineCount > 0)
			//		{
			//			g_pCurrentMagazine->SetNumber(MagazineCount - 1);
			//			ActionToSendPacket();
			//		}
		//		}
		//	}
			else
			{	
				ActionToSendPacket();
			}
		}
		else
		{
			if(!gC_vs_ui.IsSkillCastingProgress())
			{
// 				m_SkillCastingX = m_TraceX;
// 				m_SkillCastingY = m_TraceY;
				gC_vs_ui.StartSkillCastingProgress(
					m_nUsedActionInfo, wCastingTime * 100);
			}
		}
	}

	//-------------------------------------------------------------
	//
	// Casting effect
	//
	//-------------------------------------------------------------
	AttachCastingEffect( SkillInfo );

	//-------------------------------------------------------------
	// Á¤Áö µ¿ÀÛÀÌ¸é..
	// ¹Ù·Î effect¸¦ º¸¿©ÁØ´Ù.
	//-------------------------------------------------------------

	// [ÀûÀýÇÑ Å¸ÀÌ¹Ö] ¸Â°Ô º¸³¾·Á¸é ÀÌ°Å ÁÖ¼®Ã³¸® ÇØ¾ßÇÑ´Ù.
	if (bActionStand)
	{
		m_ActionCount = actionCountMax_1;
	}

	//----------------------------------------------------------
	// action count Áõ°¡
	//----------------------------------------------------------
	// ¿Ö ¿©±â¼­ Áõ°¡ÇÏ³Ä¸é?
	// ¹Ø¿¡¼­.. actionCount°¡ ¹Ù²ð ¼ö ÀÖ±â ¶§¹®¿¡..
	//----------------------------------------------------------
	m_ActionCount++; 


	//--------------------------------------------------------	
	// ÀÓ½Ã·Î ÈíÇ÷µ¿ÀÛ ÁßÁöÇÏ´Â°Å..
	//--------------------------------------------------------	
	if (m_bStopBloodDrain)
	{
		StopBloodDrain();
	}
	if (m_bStopAbsorbSoul)
	{
		StopAbsorbSoul();
	}

		
	//----------------------------------------------------------
	//
	//					Effect ½ÃÀÛ
	//
	//----------------------------------------------------------
	// affect ÈÄ¿¡ ÀÌ°Ô ¹Ù²ð ¼öµµ ÀÖ´Ù T_T;
	TYPE_ACTIONINFO currentUsedActionInfo = SkillInfo;

	if (bStartEffect)
	{
		AffectUsedActionInfo(SkillInfo);

	}
	




	//----------------------------------------------------------
	//
	//					Actionµ¿ÀÛÀÌ ³¡³¯ ¶§
	//
	//----------------------------------------------------------
	if (bEndAction)
		//&& (*g_pActionInfoTable)[ m_nUsedActionInfo ].GetSize()!=0)		
	{
		//--------------------------------------------------------
		// ÃßÀû Á¤º¸¸¦ ¾ø¾ÖÁØ´Ù.
		//--------------------------------------------------------
		//m_TraceID	= OBJECTID_NULL;
		//m_Action	= ACTION_STAND;

		//--------------------------------------------------------
		// ±â¼ú »ç¿ë ½Ã°£ Delay¸¦ ¼³Á¤ÇØÁØ´Ù.
		// ½ÃÀÛ ±â¼úÀÎ °æ¿ì¿¡¸¸..
		//--------------------------------------------------------
		// [ TEST CODE ]
		//if (//currentUsedActionInfo!=ACTIONINFO_NULL && 
		//	currentUsedActionInfo < (*g_pActionInfoTable).GetMinResultActionInfo())
		//{
		//	m_DelayTime	= g_CurrentTime + GetActionInfoDelay(currentUsedActionInfo);
		//}

		//m_bAffectUsedActionInfo = true;


		//------------------------------------------------------------
		// ¹Ýº¹ Çàµ¿ Ã¼Å©..
		//------------------------------------------------------------
			///*
		if (CheckRepeatAction())
		{
			CheckBufferAction();
		}

		// 2002.3.24 checkRepeatAction() À§¿¡ ÀÖ´ø°É ¿©±â·Î ³»·È´Ù.
		m_nUsedActionInfo = ACTIONINFO_NULL;		

		//*/
	}	

	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD("AEEnd");
	#endif
}


//----------------------------------------------------------------------
// ½Ã¾ß ¼öÁ¤
//----------------------------------------------------------------------
//void	
//MPlayer::SetLightSight(char LightSight)
//{
//	// ½Ã¾ß°¡ ¼³Á¤µÇ¾î ÀÖÀ» ¶§, ½Ã¾ß Á¦°Å..
////	if (m_TimeLightSightX != SECTORPOSITION_NULL)
////	{
//////		m_pZone->UnSetLight(m_LightSightX, m_LightSightY, m_LightSight);	
////	}
//
//	// ¼³Á¤
//	m_TimeLightSight = LightSight;
//
//	// ¼³Á¤µÈ ½Ã¾ß¿¡ ´ëÇÑ ÁÂÇ¥°ªÀ» Set..	
////	m_TimeLightSightX = m_X;
////	m_TimeLightSightY = m_Y;
//
//	// »õ·Î¿î ½Ã¾ß ¹üÀ§ ÁöÁ¤.		
////	m_pZone->SetLight(m_X, m_Y, m_LightSight);			
//}

//----------------------------------------------------------------------
// ½Ã¾ß »èÁ¦
//----------------------------------------------------------------------
//void	
//MPlayer::UnSetLightSight()
//{
//	// ½Ã¾ß°¡ ¼³Á¤µÇ¾î ÀÖÀ» ¶§, ½Ã¾ß Á¦°Å..
////	if (m_LightSightX != SECTORPOSITION_NULL)
////	{
////		m_pZone->UnSetLight(m_LightSightX, m_LightSightY, m_LightSight);	
////	}
//	
//	//m_LightSight = 0;
//
////	m_LightSightX = SECTORPOSITION_NULL;
////	m_LightSightY = SECTORPOSITION_NULL;
//}
//
//----------------------------------------------------------------------
// Set ItemCheckBuffer
//----------------------------------------------------------------------
void	
MPlayer::SetItemCheckBuffer(MItem* pItem, enum ITEM_CHECK_BUFFER status, TYPE_OBJECTID SubItem)
{
	m_pItemCheckBuffer = pItem;

	m_dwSubItemIDCheckBuffer = SubItem;

	m_ItemCheckBufferStatus = status;	

	#ifdef OUTPUT_DEBUG
	DEBUG_ADD_FORMAT("[SetItemCheckBuffer] status %d", status);
//		switch (status)
//		{
//			case ITEM_CHECK_BUFFER_PICKUP_TO_INVENTORY :
//				DEBUG_ADD("ITEM_CHECK_BUFFER_PICKUP_TO_INVENTORY");
//			break;
//
//			case ITEM_CHECK_BUFFER_PICKUP_TO_MOUSE :
//				DEBUG_ADD("ITEM_CHECK_BUFFER_PICKUP_TO_MOUSE");
//			break;
//
//			case ITEM_CHECK_BUFFER_DROP_TO_ZONE :
//				DEBUG_ADD("ITEM_CHECK_BUFFER_DROP_TO_ZONE");
//			break;
//		}
	#endif

	UI_LockItem();
	UI_LockGear();
	UI_LockItemTrade();
}

//----------------------------------------------------------------------
// Clear ItemCheckBuffer
//----------------------------------------------------------------------
void
MPlayer::ClearItemCheckBuffer()
{
	DEBUG_ADD("Set Item Check Buffer to NULL");
	
	m_pItemCheckBuffer = NULL;

	m_dwSubItemIDCheckBuffer = OBJECTID_NULL;

	m_ItemCheckBufferStatus = ITEM_CHECK_BUFFER_NULL;

	// 2004, 8, 27, sobeit add start - °ü¼ÒÈ¯ »óÅÂ¿¡¼­ ÀÎº¥¾ÆÀÌÅÛ »ç¿ëÈÄ Âø¿ë¾ÆÀÌÅÛ ¹þ¾ú´Ù ÀÔ¾ú´Ù ÇÏ¸é 
	// Å¬¶óÀÌ¾ðÆ® ÂÊ¿¡¼­ °ü»óÅÂ°¡ Ç®¸°´Ù.-_- µû¶ó¼­ °ü ¼ÒÈ¯ÀÏ¶© UI_UnlockGear()À» È£ÃâÇÏÁö ¾Ê´Â´Ù.
	if(!IsInCasket())
		UI_UnlockGear();
	// 2004, 8, 27, sobeit add end - 
	UI_UnlockItem();
	UI_UnlockItemTrade();
}

//----------------------------------------------------------------------
// Çàµ¿ °áÁ¤
//----------------------------------------------------------------------
void	
MPlayer::SetAction(BYTE action)
{
//	if ( m_nSpecialActionInfo == MAGIC_BLOODY_ZENITH && m_Action == ACTION_ATTACK &&
//		m_ActionCount >= m_ActionCountMax && GetActionDelay() > 0)
//	{
//		SetActionDelay( GetActionDelay() - 1 );
//		m_ActionCount = m_ActionCountMax - 2;
//		return;
//	}	
	// 2004, 10, 6, sobeit add start

	if(HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET)  && (action == ACTION_DAMAGED || action == ACTION_DRAINED))
		return;
	// 2004, 10, 6, sobeit add end
	if(HasEffectStatus(EFFECTSTATUS_SLEEP)
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
	{
//		return;
		action = ACTION_STAND;
	}

	// ¹«Á¶°Ç ²¨ÁØ´Ù.
	m_bFastMove = false;
	//m_bFastMove = false;
	
	//-----------------------------------------------------------------
	// ÇöÀç ¸ð½ÀÀÇ action¿¡ ¸Â´Â °æ¿ì. 2001.10.5
	//-----------------------------------------------------------------
	if (action < GetActionMax())
	{
		//-----------------------------------------------------
		// ÀüÅõ ÁØºñ ÀÚ¼¼
		//-----------------------------------------------------
		UpdateBattleStand(action);
		
		//-----------------------------------------------------
		// ¿ÀÅä¹ÙÀÌ Å¸°í ÀÖÀ»¶§
		//-----------------------------------------------------
		if (m_MoveDevice==MOVE_DEVICE_RIDE)
		{
			if (IsSlayer())
			{
				if (action==ACTION_STAND)
				{
					action = ACTION_SLAYER_MOTOR_STAND;
				}
				else if (action==ACTION_MOVE)
				{
					action = ACTION_SLAYER_MOTOR_MOVE;
				}
			}
		}
		//-----------------------------------------------------
		// °É¾î´Ù´Ò¶§
		//-----------------------------------------------------
		else
		{
			if (IsSlayer())
			{
				if (action==ACTION_SLAYER_MOTOR_STAND)
				{
					action = ACTION_STAND;
				}
				else if (action==ACTION_SLAYER_MOTOR_MOVE)
				{
					action = ACTION_MOVE;
				}
			}
		}

		if( IsOusters() && (HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) ) || m_MoveDevice==MOVE_DEVICE_SUMMON_SYLPH))
		{
			if (action==ACTION_STAND)
			{
				action = ACTION_OUSTERS_FAST_MOVE_STAND;
			} 
			else if( action == ACTION_MOVE )
			{
				action = ACTION_OUSTERS_FAST_MOVE_MOVE;
			}
		}

#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
		if( IsOusters() && (HasEffectStatusWingSylph(dynamic_cast<MCreature*>(this)) && m_MoveDevice==MOVE_DEVICE_WING_SYLPH)
#if __CONTENTS(__SECOND_TRANSFORTER)
			|| (HasEffectStatusUnicorn(dynamic_cast<MCreature*>(this)) && m_MoveDevice==MOVE_DEVICE_WING_SYLPH)
			|| (HasEffectStatusOusterNewWings1(dynamic_cast<MCreature*>(this)) && m_MoveDevice==MOVE_DEVICE_WING_SYLPH)
			|| (HasEffectStatusOusterNewWings2(dynamic_cast<MCreature*>(this)) && m_MoveDevice==MOVE_DEVICE_WING_SYLPH)
#endif //__SECOND_TRANSFORTER
			)
		{
			if (action==ACTION_STAND)
			{
#if __CONTENTS(__SECOND_TRANSFORTER)
				if(0 == GetWingItemType())
#endif //__SECOND_TRANSFORTER
					action = ACTION_OUSTERS_WING_STAND;
#if __CONTENTS(__SECOND_TRANSFORTER)
				else if(1 == GetWingItemType())
					action = ACTION_OUSTERS_UNICORN_STAND;
				else if(2 == GetWingItemType())
					action = ACTION_OUSTERS_UNICORN_STAND;
				else if(3 == GetWingItemType())
					action = ACTION_OUSTERS_UNICORN_STAND;
#endif //__SECOND_TRANSFORTER
			} 
			else if( action == ACTION_MOVE )
			{
#if __CONTENTS(__SECOND_TRANSFORTER)
				if(0 == GetWingItemType())
#endif //__SECOND_TRANSFORTER
					action = ACTION_OUSTERS_WING_MOVE;
#if __CONTENTS(__SECOND_TRANSFORTER)
				else if(1 == GetWingItemType())
					action = ACTION_OUSTERS_UNICORN_MOVE;
				else if(2 == GetWingItemType())
					action = ACTION_OUSTERS_UNICORN_MOVE;
				else if(3 == GetWingItemType())
					action = ACTION_OUSTERS_UNICORN_MOVE;
#endif //__SECOND_TRANSFORTER
			}
		}
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
		#ifdef OUTPUT_DEBUG
			if (IsSlayer())
			{
				// slayerÀÎ °æ¿ì
				if (action >= GetActionMax())
				{
					DEBUG_ADD_FORMAT("[Error] Slayer's Action exceed MAX : id=%d, action=%d", m_ID, action);
					
					return;
				}
			}
			else if (IsVampire())
			{
				if (action >= GetActionMax())
				{
					DEBUG_ADD_FORMAT("[Error] Vampire's Action exceed MAX : id=%d, action=%d", m_ID, action);
					
					return;
				}
			}
			else if (IsOusters())
			{
				if (action >= GetActionMax())
				{
					DEBUG_ADD_FORMAT("[Error] Ousters's Action exceed MAX : id=%d, action=%d", m_ID, action);
					
					return;
				}
			}
			else if (IsNPC())
			{
				if (action >= GetActionMax())
				{
					DEBUG_ADD_FORMAT("[Error] NPC's Action exceed MAX : id=%d, action=%d", m_ID, action);
					
					return;
				}
			}
			else
			{
				// - -;;
				DEBUG_ADD("[Error] What is this Creature????");
				
				return;
			}
		#endif

		m_bNextAction = false;

		//------------------------------------------------
		// MoveÀÎ °æ¿ì
		//------------------------------------------------
		if (action==m_MoveAction)
		{
			CT_BAT("SetAction MOVE-BRANCH: action=%d m_Action=%d m_MoveAction=%d ctype=%d IsStop=%d moveCount=%d/%d",
				(int)action, (int)m_Action, (int)m_MoveAction, (int)m_CreatureType, (int)IsStop(), (int)m_MoveCount, (int)m_MoveCountMax);
			// ±×³É ¼­ÀÖ´Â °æ¿ì¸é... ³¡µ¿ÀÛÀ¸·Î ¸¸µç´Ù.
			// ActionÀÌ ³¡³µ´Ù°í Ç¥½ÃÇØÁÖ±â À§ÇØ¼­..
			if (m_Action==ACTION_STAND
				|| IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND
				|| IsOusters() && m_Action==ACTION_OUSTERS_FAST_MOVE_STAND
#if __CONTENTS(__FAST_TRANSFORTER)
				|| IsOusters() && m_Action==ACTION_OUSTERS_WING_STAND
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
				|| IsOusters() && m_Action==ACTION_OUSTERS_UNICORN_STAND
#endif //__SECOND_TRANSFORTER
				)
			{
				m_ActionCount = 0;
				m_ActionCountMax = 0;
			}

			//------------------------------------------------
			// Á¤ÁöµÈ °æ¿ì¸¸ ¿òÁ÷ÀÎ´Ù.
			//------------------------------------------------
			if (IsStop())
			{
				m_Action = action;
				//SetAction( action );

				SetNextAction(ACTION_STAND);

				// Move Ã¹ µ¿ÀÛ
				m_MoveCount = 0;
				m_NextMoveCount = 0;

				// Á¤Áö  :  m_sX = 0; m_sY = 0;
				// ¿òÁ÷ÀÌ´Â Á¾·ù¿¡ µû¶ó¼­..
				//m_MoveCountMax = (*g_pCreatureTable)[m_CreatureType].GetActionCount( action );
				m_MoveCountMax = GetCreatureActionCountMax( this, action );
				CT_BAT("SetAction MOVE-START: ctype=%d action=%d m_MoveCountMax=%d (0 ise hareket asla baslamaz!)", (int)m_CreatureType, (int)action, (int)m_MoveCountMax);
					//g_pTopView->m_CreatureFPK[(*g_pCreatureTable)[m_CreatureType].FrameID]
					//									[action][m_Direction].GetSize();		
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
				//¸¸ÀÏ ½Å±ÔÀÌµ¿¼ö´Ü(ÇïÆÄÀÌ¾î)À» Å¸°í ÀÖ´Ù¸é... 
				if (m_MoveDevice==MOVE_DEVICE_RIDE && IsRide_Fast_Transforter())
				{
						m_MoveCountMax -= 1;
				}
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
				//---------------------------------------------
				// UI¿¡¼­ gearÀÇ item ÀÌµ¿À» ¸øÇÏ°Ô ÇÑ´Ù. 
				//---------------------------------------------
				//gC_vs_ui.LockItem();
				gC_vs_ui.LockGear();				
			}
			//------------------------------------------------
			// ¿òÁ÷ÀÌ°í ÀÖ´Â µµÁßÀÌ¸é BufferingÇÑ´Ù.
			//------------------------------------------------
			else
			{
				CT_BAT("SetAction MOVE-BUFFERED: IsStop=false — move bufferlandi (m_Action=%d count=%d/%d moveCount=%d/%d)",
					(int)m_Action, (int)m_ActionCount, (int)m_ActionCountMax, (int)m_MoveCount, (int)m_MoveCountMax);
				//---------------------------------------------
				// UI¿¡¼­ gear item ÀÌµ¿À» ÇÒ ¼ö ÀÖµµ·Ï ÇÑ´Ù.
				//---------------------------------------------
				//gC_vs_ui.UnlockItem();
				gC_vs_ui.UnlockGear();

				// buffering
				SetNextAction(action);
			}
		}
		//------------------------------------------------
		// ActionÀÎ °æ¿ì
		//------------------------------------------------
		else
		{
			//------------------------------------------------
			// extremeÀº Æ¯Á¤ µ¿ÀÛ¿¡¼­ Ç®¸°´Ù.
			//------------------------------------------------
//			if (HasEffectStatus(EFFECTSTATUS_EXTREME))
//			{
//				if (action==ACTION_MAGIC
//					|| action==ACTION_DIE
//					|| action==ACTION_VAMPIRE_DRAIN
//					|| action==ACTION_DRAINED)
//				{
//					RemoveEffectStatus(EFFECTSTATUS_EXTREME);
//				}
//			}

			if (// ¿ÀÅä¹ÙÀÌ Å¸°í ÀÖÀ»¶§´Â action ¾È º¸¿©ÁØ´Ù.
				m_MoveDevice==MOVE_DEVICE_RIDE && action!=ACTION_SLAYER_MOTOR_STAND
				// damagedÀÎ °æ¿ì..
				|| action==ACTION_DAMAGED 						
						&& (// °ø°Ý¹Þ´Â ÁßÀÌ ¾Æ´Ï°Å³ª Á¤Áö µ¿ÀÛÀÌ ¾Æ´Ï¸é °ø°Ý ¹Þ´Â ¸ð½À Ç¥Çö ¾È ÇÑ´Ù
							m_Action!=ACTION_DAMAGED && m_Action!=ACTION_STAND && m_Action!=ACTION_MOVE

							// ÈíÇ÷ ´çÇÏ´Â µ¿ÀÛ ÁßÀÌ°Å³ª 
	//						m_Action==ACTION_DRAINED
							// ¹ìÆÄÀÌ¾îÀÏ¶§, ÈíÇ÷ÇÏ´Â µ¿ÀÛ Áß¿¡´Â damaged¸¦ ¾È º¸¿©ÁØ´Ù.
	//						|| IsVampire() && m_Action==ACTION_VAMPIRE_DRAIN
							)
				)
			{
				SetNextAction(ACTION_STAND);
				return;
			}

			//---------------------------------------------
			// UI¿¡¼­ gear item ÀÌµ¿À» ÇÒ ¼ö ÀÖµµ·Ï ÇÑ´Ù.
			//---------------------------------------------
			//gC_vs_ui.UnlockItem();
			if(!IsInCasket())
				gC_vs_ui.UnlockGear();		

			// µ¿ÀÛ ¼³Á¤		
			SetNextAction(ACTION_STAND);

			//m_ActionCountMax = (*g_pCreatureTable)[m_CreatureType].GetActionCount( action );
			m_ActionCountMax = GetCreatureActionCountMax( this, action );

			if (m_ActionCountMax==0)
			{
				// µ¿ÀÛÀÌ ¾ø´Â °æ¿ì
				m_Action = ((m_MoveDevice==MOVE_DEVICE_WALK)? ACTION_STAND : ACTION_SLAYER_MOTOR_STAND);
				//SetAction( ((m_MoveDevice==MOVE_DEVICE_WALK)? ACTION_STAND : ACTION_SLAYER_MOTOR_STAND) );
				m_ActionCount = 0;		
				m_RepeatCount = 0;
				//m_ActionCountMax = (*g_pCreatureTable)[m_CreatureType].GetActionCount( m_Action );
				m_ActionCountMax = GetCreatureActionCountMax( this, m_Action );
			}
			else
			{
				//---------------------------------------------
				// actionÀ» ¹Ýº¹ÇÏ´Â °æ¿ì
				//---------------------------------------------
				if (m_nUsedActionInfo!=ACTIONINFO_NULL 
					&& m_bRepeatAction 
					&& (*g_pActionInfoTable)[m_nUsedActionInfo].IsUseRepeatFrame())// m_WeaponSpeed ))
				{
					m_Action = action; 
					//SetAction( action );

					// ¹Ýº¹È¸¼ö Á¦ÇÑÀÌ °É¸®¸é..
					if (m_RepeatCount >= (*g_pActionInfoTable)[m_nUsedActionInfo].GetRepeatLimit())
					{		
						m_RepeatCount = 0;

						// ¹«Á¶°Ç Ã¹ µ¿ÀÛºÎÅÍ..
						m_ActionCount = 0;
					}
					else
					{
						//---------------------------------------------
						// °è¼Ó ¹Ýº¹ÇÏ´Â °æ¿ì
						//---------------------------------------------
						BOOL bSlayer = IsSlayer();

						if (m_RepeatCount!=0 
							
							&& action!=ACTION_STAND 
							&& (!bSlayer || bSlayer && action!=ACTION_SLAYER_MOTOR_STAND))
						{
							// count ¼³Á¤..
							//m_ActionCount = (*g_pActionInfoTable)[m_nUsedActionInfo].GetRepeatStartFrame( m_WeaponSpeed );
							m_ActionCount = GetActionInfoRepeatStartFrame( m_nUsedActionInfo );
						}
						//---------------------------------------------
						// Ã³À½ ¹Ýº¹À» ½ÃÀÛÇÒ·Á´Â °æ¿ì
						//---------------------------------------------
						else
						{
							m_ActionCount = 0;				
						}
					}

					//m_ActionCountMax = (*g_pActionInfoTable)[m_nUsedActionInfo].GetRepeatEndFrame( m_WeaponSpeed );			
					m_ActionCountMax = GetActionInfoRepeatEndFrame( m_nUsedActionInfo );
				}
				//---------------------------------------------
				// ÀÏ¹ÝÀûÀÎ °æ¿ì..
				//---------------------------------------------
				else
				{			
					if (m_ActionCount!=m_ActionCountMax
						&& (action==ACTION_STAND && m_Action==ACTION_STAND
							|| IsSlayer() && action==ACTION_SLAYER_MOTOR_STAND && m_Action==ACTION_SLAYER_MOTOR_STAND
							|| IsOusters() && action==ACTION_OUSTERS_FAST_MOVE_STAND && m_Action==ACTION_OUSTERS_FAST_MOVE_STAND
#if __CONTENTS(__FAST_TRANSFORTER)
							|| IsOusters() && action==ACTION_OUSTERS_WING_MOVE && m_Action==ACTION_OUSTERS_WING_STAND
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
							|| IsOusters() && action==ACTION_OUSTERS_UNICORN_MOVE && m_Action==ACTION_OUSTERS_UNICORN_STAND
#endif //__SECOND_TRANSFORTER
							)
							)
					{
						// °°Àº Á¤Áö µ¿ÀÛÀÌ¸é ¼³Á¤ÇÏÁö ¾Ê´Â´Ù.
						//m_ActionCount = 0;
						//m_ActionCountMax = (*g_pCreatureTable)[m_CreatureType].GetActionCount( m_Action );
					}
					else
					{
						m_Action = action;
						//SetAction( action );

						// count ¼³Á¤..
						m_ActionCount = 0;
						
							//g_pTopView->m_CreatureFPK[(*g_pCreatureTable)[m_CreatureType].FrameID]
							//												[m_Action][m_Direction].GetSize();
					}			
				}			
			}
		}		
	}


	// µ¿ÀÛÀ» ½ÃÀÛÇÒ¶§ ºÎ°¡ÀûÀ¸·Î ºÙ´Â Effect µé.
	// ¾îµò°¡·Î »©¾ß ÇÏ´Âµ¥.-_-
	if( HasEffectStatus( EFFECTSTATUS_REDIANCE ) )
	{
		if( 
			( m_Action == ACTION_SLAYER_SWORD_2 || m_Action == ACTION_SLAYER_SWORD_2_SLOW || m_Action == ACTION_SLAYER_SWORD_2_FAST ) 
			)
		{
			MAttachEffect *pEffect = CreateAttachEffect( EFFECTSPRITETYPE_NEW_REDIANCE_MALE_NORMAL, 0, 0 );
			if( pEffect != NULL )
			{
				pEffect->SetDirection( GetDirection() );
				pEffect->SetLink( ACTIONINFO_NULL, NULL );							
				pEffect->SetAttachCreatureID( GetID() );
			}
		} 
		else if(
			( m_Action == ACTION_SLAYER_SWORD || m_Action == ACTION_SLAYER_SWORD_FAST || m_Action == ACTION_SLAYER_SWORD_SLOW ) )
		{
			MAttachEffect *pEffect = CreateAttachEffect( EFFECTSPRITETYPE_NEW_REDIANCE_MALE_NORMAL_ATTACK_NORMAL, 0, 0 );
			if( pEffect != NULL )
			{
				pEffect->SetDirection( GetDirection() );
				pEffect->SetLink( ACTIONINFO_NULL, NULL );							
				pEffect->SetAttachCreatureID( GetID() );
			}
		}				
	}
	//------------------------------
	// Á¤ÁöÇÑ »óÅÂÀÌ¸é
	//------------------------------
	/*
	if (IsStop())
	{	
		m_Action = action; 
		m_NextAction = ACTION_STAND;

		// CreatureÀÇ Á¾·ù¿¡ µû¶ó Action¿¡ ´ëÇÑ frame¼ö°¡ ´Þ¶ó¾ß ÇÏ³ª?
		switch (m_Action)
		{
			case ACTION_MOVE :
				switch (m_MoveDevice)
				{
					case MOVE_DEVICE_NULL :			
						m_ActionCountMax = ACTIONCOUNT_MOVE;
					break;

					case MOVE_DEVICE_MOTOR1 :						
						m_ActionCountMax = ACTIONCOUNT_MOVE_HALF;
					break;
				}
			break;

			default :
				m_ActionCountMax = g_pTopView->m_CreatureFPK[(*g_pCreatureTable)[m_CreatureType].FrameID]
															[m_Action][m_Direction].GetSize();
		}

		m_ActionCount = 0;
	}
	//------------------------------
	// ¿òÁ÷ÀÌ°í ÀÖ´Â »óÅÂÀÌ¸é
	//------------------------------
	else
	{
		// buffering
		m_NextAction = action;
	}	
	*/
}

//----------------------------------------------------------------------
// Set AddonItem
//----------------------------------------------------------------------
// AddonItemÀ» ÀåÂøÇÑ´Ù.
//----------------------------------------------------------------------
bool
MPlayer::SetAddonItem( MItem* pItem )
{
	DEBUG_ADD("MPlayer::SetAddonItem");
	CT_Done("MPSAI: ENTER this=%p pItem=%p", (void*)this, (void*)pItem);

	//-------------------------------------------------
	// Á¦´ë·Î ÀåÂøÇÑ °æ¿ì
	//-------------------------------------------------
	bool baseResult = MCreatureWear::SetAddonItem( pItem );
	CT_Done("MPSAI: post MCreatureWear::SetAddonItem result=%d", (int)baseResult);
	if (baseResult)
	{
		//-------------------------------------------------
		// ÀåÂøÇÑ ItemÀÌ 
		// ±âº»°ø°Ý¿ë ¾ÆÀÌÅÛÀÌ¶ó¸é...
		// ±âº»°ø°ÝActionInfo°¡ ¹Ù²î¾î¾ß ÇÑ´Ù.
		//-------------------------------------------------
		if (pItem->IsBasicWeapon())
		{
			//-------------------------------------------------
			// °ø°Ý °¡´É°Å¸® ¼³Á¤
			// ÃÑÀÎ °æ¿ì¸¸ ´Ù¸£°í.. 
			// ³ª¸ÓÁö´Â ¸ðµÎ ±ÙÁ¢°ø°ÝÀÌ¹Ç·Î 1ÀÌ´Ù.							
			//-------------------------------------------------
			//if (pItem->IsGunItem())
			//{
				SetBasicAttackDistance( pItem->GetReach() );
			//}
			//else
			//{								
			//	SetBasicAttackDistance( 1 );
			//}

			//-------------------------------------------------
			// Skill Icon Ã¼Å©
			//-------------------------------------------------
			//(*g_pSkillAvailable).SetAvailableSkills();
		}

		CT_Done("MPSAI: EXIT true (Player path)");
		return true;
	}

	CT_Done("MPSAI: EXIT false (Player path)");
	return false;
}

//----------------------------------------------------------------------
// Remove AddonItem
//----------------------------------------------------------------------
// AddonItemÀ» ÀåÂøÇØÁ¦ÇÑ´Ù.
//----------------------------------------------------------------------
bool
MPlayer::RemoveAddonItem( MItem* pItem )
{
	DEBUG_ADD("MCreatureWear::RemoveAddonItem");
	
	//-------------------------------------------------
	// Á¦´ë·Î ÇØÁ¦µÈ °æ¿ì
	//-------------------------------------------------
	if (MCreatureWear::RemoveAddonItem( pItem ))
	{
		//-------------------------------------------------
		// ÇØÁ¦ÇÑ ItemÀÌ 
		// ±âº»°ø°Ý¿ë ¾ÆÀÌÅÛÀÌ¶ó¸é...
		// ±âº»°ø°ÝActionInfo°¡ ¸Ç¼Õ °ø°ÝÀ¸·Î ¹Ù²î¾î¾ß ÇÑ´Ù.
		//-------------------------------------------------
		if (pItem->IsBasicWeapon())
		{
			// ±âº» ¸Ç¼Õ °ø°Ý µ¿ÀÛÀ¸·Î ÀüÈ¯ÇÑ´Ù.
			SetBasicActionInfo( SKILL_ATTACK_MELEE );


			// °ø°Ý °¡´É°Å¸® ¼³Á¤
			SetBasicAttackDistance( 1 );

			//-------------------------------------------------
			// Skill Icon Ã¼Å©
			//-------------------------------------------------
			//(*g_pSkillAvailable).SetAvailableSkills();
		}

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Set Next Action
//----------------------------------------------------------------------
// ´ÙÀ½¿¡ ÇÒ Çàµ¿ ¼³Á¤.. ÇöÀç ±×³É ¼­ÀÖÀ¸¸é(STAND) ¹Ù·Î action ¼³Á¤
// ¾Æ´Ï¸é, buffering
//----------------------------------------------------------------------
void	
MPlayer::SetNextAction(BYTE action)
{ 	
	if(HasEffectStatus(EFFECTSTATUS_SLEEP)
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		)
	{
//		return;
		action = ACTION_STAND;
	}

	if(action == ACTION_STAND)
	{
		action = ((m_MoveAction==ACTION_SLAYER_MOTOR_MOVE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND);
		if(IsOusters() && HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) ))
			action = ACTION_OUSTERS_FAST_MOVE_STAND;
#if __CONTENTS(__FAST_TRANSFORTER)
		else if(IsOusters() && HasEffectStatusWingSylph( dynamic_cast<MCreature*>(this) ))
			action = ACTION_OUSTERS_WING_STAND;
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
		else if(IsOusters() && HasEffectStatusUnicorn( dynamic_cast<MCreature*>(this) ))
			action = ACTION_OUSTERS_UNICORN_STAND;
		else if(IsOusters() && HasEffectStatusOusterNewWings1( dynamic_cast<MCreature*>(this) ))
			action = ACTION_OUSTERS_UNICORN_STAND;
		else if(IsOusters() && HasEffectStatusOusterNewWings2( dynamic_cast<MCreature*>(this) ))
			action = ACTION_OUSTERS_UNICORN_STAND;
#endif //__SECOND_TRANSFORTER
	}
	else if(action == ACTION_MOVE)
	{
		if(IsOusters() && HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) ))
			action = ACTION_OUSTERS_FAST_MOVE_MOVE;
#if __CONTENTS(__FAST_TRANSFORTER)
		else if(IsOusters() && HasEffectStatusWingSylph( dynamic_cast<MCreature*>(this) ))
			action = ACTION_OUSTERS_WING_MOVE;
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
		else if(IsOusters() && HasEffectStatusUnicorn( dynamic_cast<MCreature*>(this) ))
			action = ACTION_OUSTERS_UNICORN_MOVE;
		else if(IsOusters() && HasEffectStatusOusterNewWings1( dynamic_cast<MCreature*>(this) ))
			action = ACTION_OUSTERS_UNICORN_MOVE;
		else if(IsOusters() && HasEffectStatusOusterNewWings2( dynamic_cast<MCreature*>(this) ))
			action = ACTION_OUSTERS_UNICORN_MOVE;
#endif //__SECOND_TRANSFORTER
	}

	if (m_Action==ACTION_STAND || IsOusters() && m_Action==ACTION_OUSTERS_FAST_MOVE_STAND
		|| IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND
#if __CONTENTS(__FAST_TRANSFORTER)
		|| IsOusters() && m_Action==ACTION_OUSTERS_WING_STAND
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
		|| IsOusters() && m_Action==ACTION_OUSTERS_UNICORN_STAND
#endif //__SECOND_TRANSFORTER
		) 
	{
		//SetAction( action );
		m_ActionCount = m_ActionCountMax;
	}

	//else
	//{
		m_NextAction = action;
	//}
}

//----------------------------------------------------------------------
// Set NextAction To Move
//----------------------------------------------------------------------
// ´ÙÀ½ action¿¡´Â ÀÌµ¿ÇÑ´Ù.
//----------------------------------------------------------------------
void
MPlayer::SetNextActionToMove()
{
	Phoenix_MovementAuditWrite("MOVE_SET_NEXT_ACTION", "SET_NEXT_ACTION_TO_MOVE", m_X, m_Y, m_DestX, m_DestY, m_Direction, m_SendMove);
	CT_BAT("SetNextActionToMove: m_Action=%d m_MoveAction=%d actionCount=%d/%d moveCount=%d/%d ctype=%d",
		(int)m_Action, (int)m_MoveAction, (int)m_ActionCount, (int)m_ActionCountMax, (int)m_MoveCount, (int)m_MoveCountMax, (int)m_CreatureType);

	if (m_Action==ACTION_STAND || IsOusters() && m_Action == ACTION_OUSTERS_FAST_MOVE_STAND
#if __CONTENTS(__FAST_TRANSFORTER)
		|| IsOusters() && m_Action == ACTION_OUSTERS_WING_STAND
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
		|| IsOusters() && m_Action == ACTION_OUSTERS_UNICORN_STAND
#endif //__SECOND_TRANSFORTER
		|| IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND) 
	{
		//SetAction( action );
		m_ActionCount = m_ActionCountMax;
	}
	//else
	//{
		SetNextAction(m_MoveAction);
	//}
}

//----------------------------------------------------------------------
// Set Alive
//----------------------------------------------------------------------
// »ì¾Ò´Ù~ 
//----------------------------------------------------------------------
void	
MPlayer::SetAlive()
{
	m_bAlive = true;

	// item ÀÌµ¿ ÇØÁ¦
	UI_UnlockItem();
	UI_UnlockGear();
	UI_UnlockItemTrade();

	SetAction( ACTION_STAND );

	//-------------------------------------------------------
	// Effect»óÅÂµé Á¦°Å
	//-------------------------------------------------------
	/*
	if (g_pEffectStatusTable!=NULL)
	{
		for (int e=0; e<g_pEffectStatusTable->GetSize(); e++)
		{
			RemoveEffectStatus( (EFFECTSTATUS)e );
		}
	}
	*/

	if (g_pTopView!=NULL)
	{
		g_pTopView->SetFadeEnd();
	}

	if (m_MoveType == CREATURE_FLYING)
	{
		// FAZ 4 (2026-05-03): bounds guard (Empty global table OOB pattern)
		if (g_pCreatureTable != NULL
			&& m_CreatureType >= 0
			&& m_CreatureType < g_pCreatureTable->GetSize())
		{
			m_Z = (*g_pCreatureTable)[m_CreatureType].FlyingHeight;
		}
		else
		{
			m_Z = 0;
		}
	}
}

//----------------------------------------------------------------------
// Set Dead
//----------------------------------------------------------------------
// Á×¾ú´Ù~ 
// ServerPositionÀ¸·Î °¡¼­ Á×ÀÚ..
//----------------------------------------------------------------------
void	
MPlayer::SetDead()
{
	// »ì¾Æ ÀÖ´Â °æ¿ì¸¸ Á×ÀÎ´Ù. -_-;;
	if (m_bAlive)
	{
		if(HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
		{
			RemoveEffectStatus(EFFECTSTATUS_INSTALL_TURRET);
			SetAction( ACTION_STAND );
		}

		if(HasEffectStatus(EFFECTSTATUS_INSTALL_MK1))
		{
			RemoveEffectStatus(EFFECTSTATUS_INSTALL_MK1);
			SetAction( ACTION_STAND );
		} // MK1²âÊÔ

		//Add By Hays 2013 3  10
		if(HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) ) )
		{
			RemoveEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) );
			SetAction( ACTION_STAND );
		}
#if __CONTENTS(__FAST_TRANSFORTER)
		if(HasEffectStatusWingSylph( dynamic_cast<MCreature*>(this) ) )
		{
			RemoveEffectStatusWingSylph( dynamic_cast<MCreature*>(this) );
			SetAction( ACTION_STAND );
		}
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
		if(HasEffectStatusUnicorn( dynamic_cast<MCreature*>(this) ) )
		{
			RemoveEffectStatusUnicorn( dynamic_cast<MCreature*>(this) );
			SetAction( ACTION_STAND );
		}
		if(HasEffectStatusOusterNewWings1( dynamic_cast<MCreature*>(this) ) )
		{
			RemoveEffectStatusOusterNewWings1( dynamic_cast<MCreature*>(this) );
			SetAction( ACTION_STAND );
		}
		if(HasEffectStatusOusterNewWings2( dynamic_cast<MCreature*>(this) ) )
		{
			RemoveEffectStatusOusterNewWings2( dynamic_cast<MCreature*>(this) );
			SetAction( ACTION_STAND );
		}
#endif //__SECOND_TRANSFORTER
		if(HasEffectStatus(EFFECTSTATUS_GLACIER))
		{ 
			RemoveEffectStatus( EFFECTSTATUS_GLACIER );
			SetAction(ACTION_STAND);
			ClearAttachEffect();
		}
		g_pUIMessageManager->Execute(UI_FINISH_LEVELUP_BUTTON, 0, 0, NULL);
		
		StopDrain();
		StopAbsorb();

		// item ÀÌµ¿ ¸øÇÏ°Ô..
		UI_LockItem();
		UI_LockGear();
		UI_LockItemTrade();

		SetStop();	
//		StopCauseCriticalWounds();

		MovePosition( m_ServerX, m_ServerY );

		DEBUG_ADD_FORMAT("PlayerDeadPosition(%d, %d)", m_ServerX, m_ServerY);

		// Á¤Áö
		TraceNULL();

		// Á×À» ¶§ÀÇ ActionInfo
		m_nUsedActionInfo	= (*g_pCreatureTable)[m_CreatureType].DeadActionInfo;

		// FAZ 4 (2026-05-11) Bug 3.b ROOT FIX — PC death animasyonu oynamiyordu.
		// 3 ROOT CAUSE:
		// 1) DeadActionInfo == ACTIONINFO_NULL ise (creature.inf'te tanimsizsa)
		//    else branch yoktu → SetAction hic cagrilmiyor, m_Action ACTION_STAND
		//    kaliyor, player ayakta oluyor.
		// 2) DeadActionInfo dolu olsa bile GetActionInfoAction action.inf
		//    "Not Defined" entry'ler icin smart fallback ile ACTION_ATTACK (yumruk)
		//    dondurebiliyor (Bug 6.b'nin ayni root cause'u). Burada
		//    ACTION_DIE'a guarantee force ediyoruz.
		// 3) Asagidaki SetNextAction(ACTION_STAND) call'i death animasyonunu
		//    bir sonraki tick'te eziyor — onu kaldiriyoruz (m_bAlive=false sonrasi
		//    SetAction(ACTION_STAND) zaten line 5009 guard'i ile drop ediliyor
		//    ama m_NextAction state'i bozulmasin).
		// 3 irk paritesi: SetDead virtual degil (MPlayer hepsi icin ortak),
		// degisiklik Slayer+Vampire+Ousters hepsini ayni etkiler.
		CrashTrace_Impl("[Bug3.b][PSD] ENTER m_bAlive=%d m_CreatureType=%d DeadActionInfo=%d",
		                (int)m_bAlive, (int)m_CreatureType, (int)m_nUsedActionInfo);

		int dieAction = ACTION_DIE;
		if (m_nUsedActionInfo != ACTIONINFO_NULL)
		{
			dieAction = GetActionInfoAction(m_nUsedActionInfo);
			// action.inf Not-Defined → smart fallback ACTION_ATTACK/STAND/DAMAGED
			// donebilir. Death animasyonu icin yalniz ACTION_DIE kabul edilebilir.
			if (dieAction == ACTION_STAND || dieAction == ACTION_DAMAGED
			    || dieAction == ACTION_ATTACK || dieAction == ACTION_MOVE) {
				CrashTrace_Impl("[Bug3.b][PSD] dieAction=%d invalid, force ACTION_DIE",
				                dieAction);
				dieAction = ACTION_DIE;
			}
		}
		else
		{
			CrashTrace_Impl("[Bug3.b][PSD] DeadActionInfo NULL, force ACTION_DIE");
		}

		// Death animasyonunu daima oynat (ACTIONINFO_NULL durumunda da).
		// SetAction(ACTION_DIE) ile m_Action hemen DIE'a gecer; animasyon
		// frame'leri ActionCountMax dolana kadar oynar, sonra son frame'de
		// kalir (corpse separate entity).
		SetAction(dieAction);
		CrashTrace_Impl("[Bug3.b][PSD] post SetAction dieAction=%d m_Action=%d",
		                dieAction, (int)m_Action);

		// Trace bilgisi — kendinde
		m_NextTraceID			= m_ID;
		m_NextTraceX			= m_X;
		m_NextTraceY			= m_Y;
		m_NextTraceZ			= 0;

		SetTraceID( m_ID );
		m_TraceX			= m_X;
		m_TraceY			= m_Y;
		m_TraceZ			= 0;

		// Delay ½Ã°£
		m_DeadDelayTime	= g_CurrentTime + g_pClientConfig->DELAY_PLAYER_DEAD;


		//-------------------------------------------------------
		// º¸ÀÌ°Ô ÇÏ±â
		//-------------------------------------------------------
		SetVisibleSoon();

		// Á×À»¶§ ¹ú¶± ¼­´Â°Å ¾ø¾Ö±â À§ÇØ¼­.. È®ÀÎ»ç»ì.. - -; 2001.7.23
		m_bNextAction = false;
		m_bKeepTraceCreature = false;
		// FAZ 4 (2026-05-11): Eski kod `SetNextAction(ACTION_STAND)` -> bu da
		// death animasyon sirasinda problemli. SetAction internal'i zaten
		// m_NextAction'i STAND'a reset ediyor (line 5331). m_bAlive=false
		// guard'i (line 5015) sayesinde ACTION_STAND attempt'leri block edilir.
		// Ekstra SetNextAction call'i KALDIRILDI — gereksiz state churning.
		UnSetRepeatAction();

		// HP
		StopRecoveryHP();
		StopRecoveryMP();
		SetStatus( MODIFY_CURRENT_HP, 0 );

		// LockMode ²ô±â
		UnSetLockMode();
	//	UnBlazeWalk2SetLockMode();
		// À½¾ÇÀ» ´À¸®°Ô...
		//g_DXMusic.SetCurrentTempo( g_DXMusic.GetOriginalTempo() * 0.8f );

		m_Z = 0;
		// È®ÀÎ.. 
		UI_CloseSelectWayPoint();

		// Logout½Ã°£ Á¦°Å
		g_pUserInformation->LogoutTime = 0;	
		
	#if __CONTENTS(__080405_FIREST_UI_UPDATE)
		g_pUserInformation->QuitExitTime = 0;
	#endif //__080405_FIREST_UI_UPDATE

		SetWaitVerifyNULL();

		g_pEventManager->RemoveEvent(EVENTID_LOGOUT);
		g_pEventManager->RemoveEvent(EVENTID_LOVECHAIN);

		// Á×¾ú´Ù.
		m_bAlive = false;
		m_bInCasket = false;
	
		//-------------------------------------------------------
		// Effect»óÅÂµé Á¦°Å
		//-------------------------------------------------------
		if (g_pEffectStatusTable!=NULL)
		{
			for (int e=0; e<g_pEffectStatusTable->GetSize(); e++)
			{
				// ÈíÇ÷Àº Á¦°ÅÇÏÁö ¾Ê´Â´Ù.
				if (e!=EFFECTSTATUS_BLOOD_DRAIN && 
					// 2004, 11, 16, sobeit add start - °ø¼ºÀü °ø¼ºÃø ³à¼®µé »óÅÂ°¡ ÇÊ¿äÇØ¼­ »èÁ¦ ¾ÈÇÔ..
					e!=EFFECTSTATUS_SIEGE_ATTACKER_1 &&	
					e!=EFFECTSTATUS_SIEGE_ATTACKER_2 &&
					e!=EFFECTSTATUS_SIEGE_ATTACKER_3 &&
					e!=EFFECTSTATUS_SIEGE_ATTACKER_4 &&
					e!=EFFECTSTATUS_SIEGE_ATTACKER_5 &&
					// 2004, 11, 16, sobeit add end
					HasEffectStatus((EFFECTSTATUS)e))
				{
					RemoveEffectStatus( (EFFECTSTATUS)e );
				}
			}
		}
		ClearAttachEffect();

//		g_pTempInformation->SetMode( TempInformation::MODE_NULL );

		//---------------------------------------------------------------
		// ³» Á¤º¸¸¦ ÆÄÆ¼¿¡°Ô º¸³»ÁØ´Ù.
		//---------------------------------------------------------------
		SendStatusInfoToParty();	

		// 2004, 5, 7 sobeit add start - Á×¾úÀ» ¶§ µµ¿ò¸»
		ExecuteHelpEvent(HELP_EVENT_DIE);
		// 2004, 5, 7 sobeit add end
	}
}

//----------------------------------------------------------------------
// Check Repeat Action
//----------------------------------------------------------------------
bool
MPlayer::CheckRepeatAction()
{
	//----------------------------------------------------------------------
	// ¹Ýº¹ actionÀÌ ¾ÈµÇ´Â °æ¿ì
	//----------------------------------------------------------------------
	if (m_bKeepTraceCreature || IsRequestMode() || IsInDarkness())// || m_bTraceCreatureToForceAttack)
	{
		UnSetRepeatAction();
		return false;
	}

	//----------------------------------------------------------------------
	// actionÀÇ ¸¶Áö¸· µ¿ÀÛÀÌ°Å³ª Á¤Áöµ¿ÀÛÀÎ °æ¿ì¿¡ ¹Ýº¹ actionÀ» Àû¿ëÇÑ´Ù.
	//----------------------------------------------------------------------
	if (m_ActionCount>=m_ActionCountMax-1 
			|| m_Action==ACTION_STAND 
			|| IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND)
	{
		static DWORD repeatFrameIncTurn = 0;

		//------------------------------------------------------------
		// ¹Ýº¹ Çàµ¿ÀÌ ¼³Á¤µÈ °æ¿ì
		//------------------------------------------------------------
		if (m_bRepeatAction)				
		{
			//------------------------------------------------------------
			// ÀÚ½Å¿¡°Ô Æ¯¼ö ±â¼ú
			//------------------------------------------------------------
			if (m_fTraceBuffer & FLAG_TRACE_SELF)
			{
				//SelfSpecialAction();
				m_fNextTrace = m_fTraceBuffer;

				if (repeatFrameIncTurn!=g_CurrentFrame)
				{
					repeatFrameIncTurn = g_CurrentFrame;
					m_RepeatCount ++;
				}

				//CHECK_REPEAT_LIMIT_TO_RETURN_FALSE( m_nUsedActionInfo, m_RepeatCount )

				return true;
			}				
			//------------------------------------------------------------
			// ±âº» Çàµ¿			
			//------------------------------------------------------------
			else if (m_fTraceBuffer & FLAG_TRACE_CREATURE_BASIC)
			{	
				//TraceCreatureToBasicAction( m_TraceID, m_bTraceCreatureToForceAttack );
				m_fNextTrace = m_fTraceBuffer;

				m_NextTraceID  = m_TraceIDBuffer;				
				
				m_bNextForceAttack = m_bTraceCreatureToForceAttack;

				if (repeatFrameIncTurn!=g_CurrentFrame)
				{
					repeatFrameIncTurn = g_CurrentFrame;
					m_RepeatCount ++;
				}

				//CHECK_REPEAT_LIMIT_TO_RETURN_FALSE( m_nUsedActionInfo, m_RepeatCount )

				return true;
			}				
			//------------------------------------------------------------
			// Æ¯¼ö Çàµ¿
			//------------------------------------------------------------
			else  if (m_fTraceBuffer & FLAG_TRACE_CREATURE_SPECIAL)
			{
				//TraceCreatureToSpecialAction( m_TraceID );
				m_fNextTrace = m_fTraceBuffer;

				m_NextTraceID = m_TraceIDBuffer;

				m_bNextForceAttack = m_bTraceCreatureToForceAttack;

				if (repeatFrameIncTurn!=g_CurrentFrame)
				{
					repeatFrameIncTurn = g_CurrentFrame;
					m_RepeatCount ++;
				}
				
				//CHECK_REPEAT_LIMIT_TO_RETURN_FALSE( m_nUsedActionInfo, m_RepeatCount )				
				if (m_nUsedActionInfo < g_pActionInfoTable->GetSize())
				{
					// ¹Ýº¹È¸¼ö Á¦ÇÑ¿¡ °É¸®¸é..
					if (m_RepeatCount >= (*g_pActionInfoTable)[m_nUsedActionInfo].GetRepeatLimit())
					{		
						// µ¿ÀÛ ³¡À¸·Î º¸³½´Ù.
						//m_RepeatCount = 0;

						BOOL bSlayer = IsSlayer();

						if ((*g_pActionInfoTable)[m_nUsedActionInfo].IsUseRepeatFrame() //m_WeaponSpeed )
							&& m_Action!=ACTION_STAND 
							&& (!bSlayer || bSlayer && m_Action!=ACTION_SLAYER_MOTOR_STAND)
							&& m_ActionCountMax!=0 )	// ÀÌ°É·Î µÉ·Á³ª.. - -;
						{	
							//m_ActionCountMax = (*g_pCreatureTable)[m_CreatureType].GetActionCount( m_Action );
							m_ActionCountMax = GetCreatureActionCountMax( this, m_Action );

							DEBUG_ADD_FORMAT("unsetrepeat: actionCount=%d / %d", m_ActionCount, m_ActionCountMax);		
						}						
					}												
				}
				
				return true;
			}
			//------------------------------------------------------------
			// Æ¯¼ö Çàµ¿ -> Sector
			//------------------------------------------------------------
			else if (m_fTraceBuffer & FLAG_TRACE_SECTOR_SPECIAL)
			{
				//TraceSectorToSpecialAction( m_TraceX, m_TraceY );
				m_fNextTrace = m_fTraceBuffer;

				//m_NextTraceX = m_TraceX;
				//m_NextTraceY = m_TraceY;

				//------------------------------------------------------------
				// ÇöÀçÀÇ mouse ÁÂÇ¥¸¦ ÀÐ¾î¼­ mapÀÇ ÁÂÇ¥·Î ¹Ù²Û´Ù.
				//------------------------------------------------------------
				// ¿ì¾¾ -_-;; ¿Ö ¿©±â¼­ ¸¶¿ì½º ÁÂÇ¥¸¦ ÀÐ¾î¿À³Ä°í ”î
				if(!gC_vs_ui.IsSkillCastingProgress())
				{
					POINT temp = g_pTopView->ScreenToPixel(g_x, g_y);
					temp = MTopView::PixelToMap( temp.x, temp.y );
						
					m_NextTraceX = temp.x;
					m_NextTraceY = temp.y;
				}
				else
				{
					m_NextTraceX = m_SkillCastingX;
					m_NextTraceY = m_SkillCastingY;
				}

				if (repeatFrameIncTurn!=g_CurrentFrame)
				{
					repeatFrameIncTurn = g_CurrentFrame;
					m_RepeatCount ++;
				}
				
				//CHECK_REPEAT_LIMIT_TO_RETURN_FALSE( m_nUsedActionInfo, m_RepeatCount )

				return true;
			}		
		}
		//------------------------------------------------------------
		// ¹Ýº¹ Çàµ¿ÀÌ ¼³Á¤ ¾È µÈ °æ¿ì
		//------------------------------------------------------------
		else
		{
			//------------------------------------------------------------
			// bufferingµÈ ´ÙÀ½ Çàµ¿ÀÌ ÀÖ´Ù¸é..
			//------------------------------------------------------------
			if (m_fNextTrace!=FLAG_TRACE_NULL)
			{
				//m_fTraceBuffer = m_fNextTrace;
				return true;
			}			
		}
	}
	else
	{
	}

	return false;
}

//----------------------------------------------------------------------
// Check BufferAction
//----------------------------------------------------------------------
// ´ÙÀ½¿¡ Ã³¸®ÇØ¾ßÇÒ bufferingµÈ actionÀÌ ÀÖ´ÂÁö¸¦ ¾Ë¾Æº»´Ù.
// ÀÖÀ» °æ¿ì´Â return true
// ¾øÀ» °æ¿ì´Â return false
//----------------------------------------------------------------------
bool
MPlayer::CheckBufferAction()
{
	//DEBUG_ADD("Before CheckBufferAction");
	
	//------------------------------------------		
	//     ÃßÀûÇØ¾ß ÇÏ´Â °æ¿ì..		
	//------------------------------------------
	if (m_fNextTrace != FLAG_TRACE_NULL)
	{			
		//SetAction( (m_MoveAction==ACTION_SLAYER_MOTOR_MOVE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND );

		//---------------------
		// ÀÚ½Å¿¡°Ô Æ¯¼ö ±â¼ú
		//---------------------
		if (m_fNextTrace & FLAG_TRACE_SELF)
		{
			SelfSpecialAction();
		}				
		//---------------------
		// ±âº» Çàµ¿			
		//---------------------
		if (m_fNextTrace & FLAG_TRACE_CREATURE_BASIC)
		{
			TraceCreatureToBasicAction( m_NextTraceID, m_bNextForceAttack );
		}				
		//---------------------
		// Æ¯¼ö Çàµ¿
		//---------------------
		else  if (m_fNextTrace & FLAG_TRACE_CREATURE_SPECIAL)
		{
			TraceCreatureToSpecialAction( m_NextTraceID, m_bNextForceAttack );
		}
		//---------------------
		// ±âº» Çàµ¿ -> Sector
		//---------------------
		else if (m_fNextTrace & FLAG_TRACE_SECTOR_BASIC)
		{
			TraceSectorToBasicAction( m_NextTraceX, m_NextTraceY );
		}
		//---------------------
		// Æ¯¼ö Çàµ¿ -> Sector
		//---------------------
		else if (m_fNextTrace & FLAG_TRACE_SECTOR_SPECIAL)
		{
			TraceSectorToSpecialAction( m_NextTraceX, m_NextTraceY );
		}
		//---------------------
		// Item ÃßÀû
		//---------------------
		else if (m_fNextTrace & FLAG_TRACE_ITEM)
		{
			TraceItem( m_NextTraceID );
		}
		//---------------------
		// InventoryÀÇ Item ¿¡ ±â¼ú »ç¿ë
		//---------------------
		else if (m_fNextTrace & FLAG_TRACE_INVENTORY)
		{
			TraceInventoryItem( m_NextTraceID );
		}
		//---------------------
		// EffectÃßÀû
		//---------------------
		else if (m_fNextTrace & FLAG_TRACE_EFFECT)
		{
			TraceEffect( m_NextTraceID );
		}
		//---------------------
		// InteractionObjectÃßÀû
		//---------------------
		else if (m_fNextTrace & FLAG_TRACE_INTERACTIONOBJECT)
		{
			TraceInteractionObject( m_NextTraceID, m_NextTraceObjectAction );
		}

		//DEBUG_ADD("After CheckBufferAction : true");
		
		return true;
	}

	//------------------------------------------		
	//     BufferingµÈ ´ÙÀ½ Çàµ¿
	//------------------------------------------
	BOOL bSlayer = IsSlayer();

	if (m_bNextAction || 
		m_NextAction!=ACTION_STAND 
		&& (!IsOusters() || IsOusters() && m_NextAction!=ACTION_OUSTERS_FAST_MOVE_STAND
#if __CONTENTS(__FAST_TRANSFORTER)
		|| IsOusters() && m_NextAction!=ACTION_OUSTERS_WING_STAND
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
		|| IsOusters() && m_NextAction!=ACTION_OUSTERS_UNICORN_STAND
#endif //__SECOND_TRANSFORTER
		)// && m_fNextTrace==FLAG_TRACE_NULL)
		&& (!bSlayer || bSlayer && m_NextAction!=ACTION_SLAYER_MOTOR_STAND))// && m_fNextTrace==FLAG_TRACE_NULL)
	{
		//SetSpecialActionInfo(SKILL_UNTERFELDWEBEL_FIRE) ; 
		SetAction( m_NextAction);  
		
		
		//SetAction( ACTION_ADVANCEMENT_SLAYER_COMBO_SWARD_SLOW);	
		CT_Done("After CheckBufferAction : true - SetAction");
		return true;
	}

	//DEBUG_ADD("After CheckBufferAction - false");
	
	return false;
}

//----------------------------------------------------------------------
// UnSet Repeat Action
//----------------------------------------------------------------------
void			
MPlayer::UnSetRepeatAction()				
{ 
	// ¹Ýº¹ÀÌ ÁßÁöµÇ¸é ´ÙÀ½ bufferingÇàµ¿µµ ¾ø¾Ö¾ß µÉ±î?

	m_bRepeatAction = FALSE; 

	m_RepeatCount = 0; 
	
	
	m_nNextUsedActionInfo = ACTIONINFO_NULL;

	BOOL bSlayer = IsSlayer();

	//m_fTraceBuffer = FLAG_TRACE_NULL;
	/*
	#ifdef OUTPUT_DEBUG
		if (g_pDebugMessage)
			{
				if (m_nUsedActionInfo != ACTIONINFO_NULL)
				{
					DEBUG_ADD_FORMAT("UnSetRepeatAction:ai=%d, ir=%d, at=%d, sm=%d, acm=%d",
			
					(int)m_nUsedActionInfo, 
					(int)(*g_pActionInfoTable)[m_nUsedActionInfo].IsUseRepeatFrame(),
					(int)m_Action!=ACTION_STAND,
					(int)(!bSlayer || bSlayer && m_Action!=ACTION_SLAYER_MOTOR_STAND),
					(int)m_ActionCountMax!=0);
				}
				else
				{
					DEBUG_ADD("UnSetRepeatAction: m_nUsedActionInfo is ACTIONINFO_NULL");
				}
			}
	#endif
	*/

	if (m_nUsedActionInfo!=ACTIONINFO_NULL 
		&& (*g_pActionInfoTable)[m_nUsedActionInfo].IsUseRepeatFrame() //m_WeaponSpeed )
		&& m_Action!=ACTION_STAND 
		&& (!bSlayer || bSlayer && m_Action!=ACTION_SLAYER_MOTOR_STAND)
		&& m_ActionCountMax!=0 )	// ÀÌ°É·Î µÉ·Á³ª.. - -;
	{	
		//m_ActionCountMax = (*g_pCreatureTable)[m_CreatureType].GetActionCount( m_Action );		
		m_ActionCountMax = GetCreatureActionCountMax( this, m_Action );

		DEBUG_ADD_FORMAT("unsetrepeat: actionCount=%d / %d", m_ActionCount, m_ActionCountMax);		
	}



	//m_fNextTrace = FLAG_TRACE_NULL; 
}

//----------------------------------------------------------------------
// Add EffectStatus
//----------------------------------------------------------------------
bool
MPlayer::AddEffectStatus(EFFECTSTATUS status, DWORD delayFrame)
{
	__BEGIN_HELP_EVENT
		switch (status)
		{
		case EFFECTSTATUS_SUMMON_SYLPH_GREEN :
		case EFFECTSTATUS_SUMMON_SYLPH_ORANGE :
		case EFFECTSTATUS_SUMMON_SYLPH_RED :
		case EFFECTSTATUS_SUMMON_SYLPH_YELLOW :
		case EFFECTSTATUS_SUMMON_SYLPH_WHITE :
		case EFFECTSTATUS_SUMMON_SYLPH_BROWN :
		case EFFECTSTATUS_SUMMON_SYLPH_LILAC :
		case EFFECTSTATUS_SUMMON_SYLPH_BLACK :
		case EFFECTSTATUS_SUMMON_SYLPH:
		case EFFECTSTATUS_SUMMON_WISP_RED:
		case EFFECTSTATUS_SUMMON_WISP_YELLOW:
		case EFFECTSTATUS_SUMMON_WISP_GREEN:
			SetMoveDevice(MOVE_DEVICE_SUMMON_SYLPH);
			break;
#if __CONTENTS(__FAST_TRANSFORTER)
		case EFFECTSTATUS_CLIENT_WING_SYLPH:
#if __CONTENTS(__SECOND_TRANSFORTER)
		case EFFECTSTATUS_CLIENT_UNICORN:
		case EFFECTSTATUS_OusterWings_New_1://Ä§ÁéÐÂ×øÆï
		case EFFECTSTATUS_OusterWings_New_2:
#endif //__SECOND_TRANSFORTER
			SetMoveDevice(MOVE_DEVICE_WING_SYLPH);
			break;
#endif //__FAST_TRANSFORTER
			//--------------------------------------------------
			// Bless
			//--------------------------------------------------
			case EFFECTSTATUS_BLESS :
				// [µµ¿ò¸»] striking
//				ExecuteHelpEvent( HE_EFFECT_BLESS );
			break;
		//	case EFFECTSTATUS_INSTALL_MK1:
			//		g_pSkillAvailable->SetAvailableSkills();
			//		SetGroundCreature();
		//		break;
			//--------------------------------------------------
			// Striking
			//--------------------------------------------------
			case EFFECTSTATUS_STRIKING :
				// [µµ¿ò¸»] striking
//				ExecuteHelpEvent( HE_EFFECT_STRIKING );
			break;

			//--------------------------------------------------
			// Green Poison
			//--------------------------------------------------
			case EFFECTSTATUS_GREEN_POISON :
			case EFFECTSTATUS_POISON :
				// [µµ¿ò¸»] green poison
//				ExecuteHelpEvent( HE_EFFECT_GREEN_POISON );
			break;

			//--------------------------------------------------
			// Purple Wall
			//--------------------------------------------------
			//case EFFECTSTATUS_PURPLE_WALL :
			case EFFECTSTATUS_CURSE_PARALYSIS :
				// [µµ¿ò¸»] purple wall
//				ExecuteHelpEvent( HE_EFFECT_PURPLE_WALL );
			break;

			//--------------------------------------------------
			// Yellow Poison
			//--------------------------------------------------
			case EFFECTSTATUS_YELLOW_POISON :
			case EFFECTSTATUS_YELLOW_POISON_TO_CREATURE :
				// [µµ¿ò¸»] yellow poison
//				ExecuteHelpEvent( HE_EFFECT_YELLOW_POISON );
			break;
		
			//--------------------------------------------------
			// BLOOD DRAIN 
			//--------------------------------------------------
			case EFFECTSTATUS_BLOOD_DRAIN :
			{
				// 2004, 5, 6 sobeit add start [µµ¿ò¸»] ÈíÇ÷´çÇßÀ»¶§
				ExecuteHelpEvent( HELP_EVENT_DRAIN_BLOOD );				
				// 2004, 5, 6 sobeit add end
			}
			break;		

			//--------------------------------------------------
			// OBSERVING_EYE [»õ±â¼ú6]
			//--------------------------------------------------
			case EFFECTSTATUS_OBSERVING_EYE :
				// ¾ÆÁ÷ ¾È °É·ÁÀÖ´Â °æ¿ì
				if (!m_bEffectStatus[EFFECTSTATUS_OBSERVING_EYE])
				{
					CalculateLightSight();
				}
			break;

			case EFFECTSTATUS_GHOST:
				SetFlyingCreature();
//				g_pUserInformation->bCompetence = true;
			break;
			case EFFECTSTATUS_REQUEST_RESURRECT:
				// »ì¾ÆÀÖÀ¸¸é ÀÌÆÑÆ®¸¦ ¹«½ÃÇÑ´Ù.
				if(IsDead())	gC_vs_ui.RequestResurrectSetButton(4);
				else			return true;
			break;	
		}
	__END_HELP_EVENT

	//--------------------------------------------------
	// ¹ìÆÄÀÌ¾î·Î º¯ÇÏ´Â effectÀÌ¸é.. ½Ã°£ ¼³Á¤À» ÇØµÐ´Ù.
	//--------------------------------------------------
	switch (status)
	{
		//--------------------------------------------------
		// ÈíÇ÷
		//--------------------------------------------------
		case EFFECTSTATUS_BLOOD_DRAIN :
		{
			DWORD conversionTime = delayFrame * 1000 / 16;// / g_pClientConfig->FPS;

			SetConversionDelay( conversionTime );

			DEBUG_ADD_FORMAT( "[BloodDrained] %ld --> %ld", g_CurrentTime, m_ConversionDelayTime );

//			delayFrame *= 16;
//			UI_SetHP( GetHP(), GetMAX_HP() );
		}
		break;

		//--------------------------------------------------
		// hallu
		//--------------------------------------------------
		case EFFECTSTATUS_HALLUCINATION :
			g_pZone->ChangeToHalluCreature();			
		break;

		case EFFECTSTATUS_LIGHTNESS :
		case EFFECTSTATUS_YELLOW_POISON :
		case EFFECTSTATUS_FLARE :
		case EFFECTSTATUS_YELLOW_POISON_TO_CREATURE :
//		case EFFECTSTATUS_BLINDNESS:
			CalculateSight();
			break;
		case EFFECTSTATUS_CAN_ENTER_GDR_LAIR:
			delayFrame = delayFrame*10;
			break;
		case EFFECTSTATUS_INSTALL_TURRET:
//			g_pSkillAvailable->SetAvailableSkills();
			SetDelay(1000);
			gC_vs_ui.UnselectSkill();
		//	gC_vs_ui.SelectSkill( SKILL_CLINT_ADV2_HALCYON_MK1 );
			break;
		case EFFECTSTATUS_INSTALL_MK1:
			gC_vs_ui.SelectSkill( SKILL_CLINT_ADV2_HALCYON_MK1 );
			break;
		case EFFECTSTATUS_HARPOON_BOMB:
			SetEnforceResurrect(true);
			break;
	}

	bool re = MCreature::AddEffectStatus( status, delayFrame );
	//--------------------------------------------------
	// UI¿¡ ¾Ë¸°´Ù. ¿Ö ÀÌ°É re·Î ÆÇ´ÜÇÏ¸é »à»ì³ªÁö-_-; 2002.5.5 ¾¦°« SKILL_CLINT_ADV2_HALCYON_MK1
	//--------------------------------------------------
	if(!(IsSlayer() && status == EFFECTSTATUS_CAUSE_CRITICAL_WOUNDS))
		UI_AddEffectStatus( status, delayFrame );
	
	if(status == EFFECTSTATUS_INSTALL_TURRET)
	{
		g_pSkillAvailable->SetAvailableSkills();
		gC_vs_ui.SelectSkill( SKILL_TURRET_FIRE );
	}
	CheckRegen();
	if(status == EFFECTSTATUS_INSTALL_MK1)
	{
		g_pSkillAvailable->SetAvailableSkills();
		gC_vs_ui.SelectSkill( SKILL_CLINT_ADV2_HALCYON_MK1 );
	}
	CheckRegen();
	return re; 
}

//----------------------------------------------------------------------
// Remove EffectStatus
//----------------------------------------------------------------------
bool		
MPlayer::RemoveEffectStatus(EFFECTSTATUS status)
{
	bool bOK = MCreatureWear::RemoveEffectStatus( status );

	//--------------------------------------------------
	// UI¿¡ ¾Ë¸°´Ù.
	//--------------------------------------------------
	UI_RemoveEffectStatus( status );

	switch (status)
	{		
	case EFFECTSTATUS_SUMMON_SYLPH_GREEN :
	case EFFECTSTATUS_SUMMON_SYLPH_ORANGE :
	case EFFECTSTATUS_SUMMON_SYLPH_RED :
	case EFFECTSTATUS_SUMMON_SYLPH_YELLOW :
	case EFFECTSTATUS_SUMMON_SYLPH_WHITE :
	case EFFECTSTATUS_SUMMON_SYLPH_BROWN :
	case EFFECTSTATUS_SUMMON_SYLPH_LILAC :
	case EFFECTSTATUS_SUMMON_SYLPH_BLACK :
	case EFFECTSTATUS_SUMMON_SYLPH:
	case EFFECTSTATUS_SUMMON_WISP_RED:
	case EFFECTSTATUS_SUMMON_WISP_YELLOW:
	case EFFECTSTATUS_SUMMON_WISP_GREEN:
		SetMoveDevice(MOVE_DEVICE_WALK);
		if(GetWaitVerify() == MPlayer::WAIT_VERIFY_SYLPH_SUMMON_GETOFF)
			SetWaitVerifyNULL();
		break;
#if __CONTENTS(__FAST_TRANSFORTER)
	case EFFECTSTATUS_CLIENT_WING_SYLPH:
		SetMoveDevice(MOVE_DEVICE_WALK);
		if(GetWaitVerify() == MPlayer::WAIT_VERIFY_SYLPH_SUMMON_GETOFF)
			SetWaitVerifyNULL();
		break;
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
	case EFFECTSTATUS_CLIENT_UNICORN:

	case EFFECTSTATUS_OusterWings_New_1://Ä§ÁéÐÂ×øÆï
	case EFFECTSTATUS_OusterWings_New_2:

		SetMoveDevice(MOVE_DEVICE_WALK);
		if(GetWaitVerify() == MPlayer::WAIT_VERIFY_SYLPH_SUMMON_GETOFF)
			SetWaitVerifyNULL();
		break;
#endif //__SECOND_TRANSFORTER
		//--------------------------------------------------
		// hallu
		//--------------------------------------------------
		case EFFECTSTATUS_HALLUCINATION :
			g_pZone->RemoveHalluCreature();			
		break;

		//--------------------------------------------------
		// ¹ìÆÄÀÌ¾î·Î º¯ÇÏ´Â effectÀÌ¸é.. ½Ã°£ ¼³Á¤ Á¦°Å
		//--------------------------------------------------
		case EFFECTSTATUS_BLOOD_DRAIN :
			UnSetConversionDelay();

			UI_SetHP( GetHP(), GetMAX_HP() );		
		break;

		//--------------------------------------------------
		// Á×´Ù°¡ »ì¾Æ³­ °æ¿ì
		//--------------------------------------------------		
		case EFFECTSTATUS_COMA :
			// °ËÁõ Á¦°Å
			if (g_pTempInformation->GetMode()==TempInformation::MODE_WAIT_RESURRECT)
			{
				g_pTempInformation->SetMode(TempInformation::MODE_NULL);
			}

			if (GetWaitVerify()==WAIT_VERIFY_RESURRECT)
			{
				SetWaitVerifyNULL();
			}

			gC_vs_ui.FinishRequestDie();
			gC_vs_ui.FinishRequestResurrect();
			gC_vs_ui.CloseRequestShrineMinimap();
		break;

		//--------------------------------------------------
		// OBSERVING_EYE [»õ±â¼ú6]
		//--------------------------------------------------
		case EFFECTSTATUS_OBSERVING_EYE :
			CalculateStatus();
		break;

		case EFFECTSTATUS_GHOST:
			// FAZ 4 (2026-05-03): bounds guard (Empty global table OOB pattern)
			if (g_pCreatureTable != NULL
				&& m_CreatureType >= 0
				&& m_CreatureType < g_pCreatureTable->GetSize())
			{
				if (!(*g_pCreatureTable)[m_CreatureType].bFlyingCreature)	// ¹ÚÁãÀÎ °æ¿ì
					SetGroundCreature();
			}
			else
			{
				SetGroundCreature();
			}
			break;
		case EFFECTSTATUS_WILL_OF_LIFE :
			if(IsVampire() )
				MCreature::CheckRegen();
			break;
		case EFFECTSTATUS_RAGE_OF_BLOOD :
			if(IsVampire() )
				MCreature::CheckRegen();
			break; //by rallser
		case EFFECTSTATUS_FLARE :
		case EFFECTSTATUS_YELLOW_POISON :
		case EFFECTSTATUS_YELLOW_POISON_TO_CREATURE :
		case EFFECTSTATUS_LIGHTNESS :
		case EFFECTSTATUS_FLAME_SIGH :
			// 2004, 6, 21 sobeit add start - about blindness
//		case EFFECTSTATUS_BLINDNESS:
			// 2004, 6, 21 sobeit add end - about blindness
			if( bOK )
				CalculateSight();
			break;
//		case EFFECTSTATUS_GLACIER:
//			if (m_bAlive)
//			{
//				SetStop();
//				StopCauseCriticalWounds();
//			}			
//		break;

			//Add By Hays 2013  3   10
		case EFFECTSTATUS_INSTALL_MK1:
			g_pSkillAvailable->SetAvailableSkills();
			ExecuteActionInfoFromMainNode(RESULT_SKILL__ADV2_HALCYON_MK1,GetX(), GetY(), 0,0,GetID()	,	
				GetX(), GetY(), 0, 0, NULL, false);
		//	SetDelay(1000);
		//	SetGroundCreature();
			break;

		case EFFECTSTATUS_INSTALL_TURRET:
			g_pSkillAvailable->SetAvailableSkills();
			ExecuteActionInfoFromMainNode(RESULT_SKILL_INSTALL_TURRET,GetX(), GetY(), 0,0,GetID()	,	
				GetX(), GetY(), 0, 0, NULL, false);
			SetDelay(1000);
			break;
		case EFFECTSTATUS_HARPOON_BOMB:
			// ÇÃ·¹ÀÌ¾î°¡ Á×Áö ¾Ê¾ÒÀ» °æ¿ì¿¡¸¸ ÇØÁ¦
			if(!IsDead()) SetEnforceResurrect(false);
			break;
	}
	
	return bOK;
}

//----------------------------------------------------------------------
// Set CreatureType
//----------------------------------------------------------------------
void	
MPlayer::SetCreatureType(TYPE_CREATURETYPE type)
{
	MCreatureWear::SetCreatureType(type);

	//---------------------------------------------------
	// Skill IconÀ» ´Ù½Ã Ã¼Å©ÇÑ´Ù.
	//---------------------------------------------------
	if (g_pSkillAvailable!=NULL)
	{
		g_pSkillAvailable->SetAvailableSkills();		
	}	
	if(type==CREATURETYPE_SLAYER_OPERATOR
		|| type==CREATURETYPE_VAMPIRE_OPERATOR
		|| type==CREATURETYPE_OUSTERS_OPERATOR)
		g_pUserInformation->bCompetenceShape = true;
	else
		g_pUserInformation->bCompetenceShape = false;
}

//----------------------------------------------------------------------
// UpdateConversionTime
//----------------------------------------------------------------------
void
MPlayer::UpdateConversionTime()
{
	//--------------------------------------------------------
	// ¹ìÆÄÀÌ¾î·Î º¯ÇÏ´Â EffectÃ¼Å©
	// ÄÚµå ¸¶±¸ Ãß°¡ÇÏ±â.. Ä«Ä«...- -;
	//--------------------------------------------------------

	// ÀüÀï ³²Àº ½Ã°£ Ã¼Å©
	
	if (IsSlayer())
	{
		if ((int)m_ConversionDelayTime > (int)g_CurrentTime)
		{
			//--------------------------------------------------------
			// ¹ìÆÄÀÌ¾î·Î º¯ÇÏ´Â ³²Àº ½Ã°£À» UI¿¡ ¼³Á¤ÇÑ´Ù.
			//--------------------------------------------------------
			// msec --> minÀ¸·Î ¹Ù²Û´Ù.   * GetTimeRatio / 1000 / 60 
			int changeTime = ((int)m_ConversionDelayTime - (int)g_CurrentTime)/* * g_pGameTime->GetTimeRatio()*/ / 60000 ;
			g_char_slot_ingame.CHANGE_VAMPIRE = changeTime;

			static DWORD enableBlinkTime = 0;	// ±ôºý°Å·Áµµ µÇ´Â ½Ã°£
			
			//const DWORD sixHour = 6*60*60*1000;

			// { ³²Àº½Ã°£, ±ôºýÀÌ´Â ½Ã°£ }
			const int maxBlink = 12;
			const DWORD blinkValue[maxBlink][2] = 
			{
				{ 4*60*60*1000, 6*60*1000 },	// 4½Ã°£ ÀÌ»ó --> 5ºÐ¸¶´Ù			
				{ 3*60*60*1000, 4*60*1000 },	// 3½Ã°£ ÀÌ»ó --> 4ºÐ¸¶´Ù			
				{ 2*60*60*1000, 3*60*1000 },	// 2½Ã°£ ÀÌ»ó --> 3ºÐ¸¶´Ù			
				{ 1*60*60*1000, 2*60*1000 },	// 1½Ã°£ ÀÌ»ó --> 2ºÐ¸¶´Ù
				{ 1*60*60*1000, 1*60*1000 },	// 10ºÐ ÀÌ»ó --> 1ºÐ
				{ 3*60*1000, 30*1000 },			// 3ºÐ ÀÌ»ó --> 30ÃÊ
				{ 2*60*1000, 10*1000 },			// 2ºÐ ÀÌ»ó --> 10ÃÊ
				{ 1*60*1000, 5*1000 },			// 1ºÐ ÀÌ»ó --> 5ÃÊ
				{ 30*60*1000, 3*1000 },			// 30ÃÊ ÀÌ»ó --> 3ÃÊ
				{ 10*1000, 2*1000 },			// 10ÃÊ ÀÌ»ó --> 2ÃÊ
				{ 10*1000, 2*1000 },			// 5ÃÊ ÀÌ»ó --> 1ÃÊ
				{ 0*1000, 300 }					// 0ÃÊ ÀÌ»ó --> 0.3ÃÊ
			};

			//--------------------------------------------------------
			// ¸¶Áö¸·¿¡ ±ôºýÀÌ°í ³ª¼­.. 
			// ´Ù½Ã ±ôºýÀÏ ¼ö ÀÖ´Â ½Ã°£ÀÎÁö Ã¼Å©ÇÑ´Ù.
			//--------------------------------------------------------
			if (g_CurrentTime > enableBlinkTime)
			{
				// º¯ÇÏ±â±îÁö ³²Àº ½Ã°£.. /1000ÇÏ¸é 'ÃÊ'·Î ³ª¿Â´Ù.
				DWORD timeGap = m_ConversionDelayTime - g_CurrentTime;
				
				// ¹«Á¶°Ç Ãâ·Â. 6½Ã°£ÀÌ ¾È ³²Àº °æ¿ì¿¡¸¸ effectÃâ·Â
				if (1)//timeGap < sixHour)
				{
					BOOL bBlink = FALSE;

					for (int i=0; i<maxBlink; i++)
					{
						if (timeGap > blinkValue[i][0])
						{
							g_pTopView->SetFadeStart(25, 31, 2, 31,0,0);

							// ´ÙÀ½¿¡ ±ôºýÀÏ ¼ö ÀÖ´Â ½Ã°£À» ¼³Á¤ÇÑ´Ù.
							enableBlinkTime = g_CurrentTime + blinkValue[i][1];

							bBlink = TRUE;

							break;
						}
					}

					if (!bBlink)
					{
						UnSetConversionDelay();
					}
				}
			}
		}
		else
		{
			m_ConversionDelayTime = 0;

			//--------------------------------------------------------
			// ¹ìÆÄÀÌ¾î·Î º¯ÇÏ´Â ³²Àº ½Ã°£À» UI¿¡ ¼³Á¤ÇÑ´Ù.
			//--------------------------------------------------------
			g_char_slot_ingame.CHANGE_VAMPIRE = 0;
		}
	}
}

//----------------------------------------------------------------------
// ´ÙÀ½ µ¿ÀÛÀ» ÃëÇÑ´Ù.
//----------------------------------------------------------------------
void
MPlayer::Action()
{
	UpdateActionStep();

	if( !HasEffectStatus( EFFECTSTATUS_FIRE_ELEMENTAL ) && !HasEffectStatus( EFFECTSTATUS_WATER_ELEMENTAL ) && GetElementalID() != OBJECTID_NULL && g_pZone != NULL)
	{
		g_pZone->RemoveFakeCreature(GetElementalID());
	}

	// ÆêÀÌ ÀÖ°í Æê µ¿ÀÛÇÑÁö 1ÃÊ ³Ñ¾úÀ»¶§
	if( GetPetID() != OBJECTID_NULL)
	{
		MFakeCreature *pFakeCreature = (MFakeCreature *)g_pZone->GetFakeCreature(GetPetID());
		if(pFakeCreature != NULL)
		{
			MPetItem *pPetItem = pFakeCreature->GetPetItem();
			if(pPetItem != NULL)
			{
				const int petLevelMax = 50;
				int petLevel = pPetItem->GetNumber();
				int petDelayTime = 0;
				if(pPetItem->GetItemType() >= 3 && pPetItem->GetItemType() <= 5)// 2 Â÷ ÆêÀÌ¸é
					petDelayTime = 3200+((petLevelMax-petLevel)*2800)/petLevelMax;
				else
					petDelayTime = 1200+((petLevelMax-petLevel)*2800)/petLevelMax;
#if __CONTENTS(__EXPERT_PET_CHANGER)
//
				if( g_char_slot_ingame.bExpertPetUse)
				{
						petDelayTime = max(petDelayTime*0.5, 900);
				}
#endif //__EXPERT_PET_CHANGER
				if(m_PetDelayTime+petDelayTime < GetTickCount() )
				{
					// Pet daima treasure'i olan cesetleri dener (TreasureCount > 0).
					// CanCutHead client secimini engellemez; kafatasi extract server'da
					// CGDissectionCorpseHandler + canCutHead() ile korunur.
					MItem *pItem = g_pZone->GetCorpseKilledByMe(0);

					if(pItem != NULL)
					{
						CGDissectionCorpse _CGDissectionCorpse;
						_CGDissectionCorpse.setX( pItem->GetX() );
						_CGDissectionCorpse.setY( pItem->GetY() );
						_CGDissectionCorpse.setPet( true );
						_CGDissectionCorpse.setObjectID( pItem->GetID() );

						g_pSocket->sendPacket( &_CGDissectionCorpse );

						// Tekrar denemesin. CutHead yoksa server skull'da durur;
						// kalan kafatasi manuel dissection ile alinabilir.
						DEBUG_ADD_FORMAT("[PETPET] dissect cutHead=%d", (int)pPetItem->IsCanCutHead());
						pItem->SetNumber( 0 );
					}
					
					m_PetDelayTime = GetTickCount();
				}
			}
		}
	}

	// ³Ê¹« ¿À·¡ ¹Ýº¹ ½ÃÅ°Áö ¾Ê°Ô
	if( m_nUsedActionInfo == SKILL_BLITZ_SLIDING_ATTACK || m_nUsedActionInfo == SKILL_BLAZE_WALK_ATTACK )
		UnSetRepeatAction();

	//if( m_nUsedActionInfo == MAGIC_METEOR)
	//{
	//		if(GetActionCountMax() != 0)
	//		{
	//			m_ActionCount = g_CurrentFrame % GetActionCountMax();
	//		}
	//}
	//	//if( GetAction() != 0)
	////{
	////		if(GetActionCountMax() != 0)
	////		{
	////			m_ActionCount = g_CurrentFrame % GetActionCountMax();
	////		}
	////}
	if(IsRepeatAction())
	{
		if(m_RepeatTimer + g_pClientConfig->REPEAT_TIME < GetTickCount())
			UnSetRepeatAction();
	}
	// ³Ê¹« ¿À·¡ ¹Ýº¹ ½ÃÅ°Áö ¾Ê°Ô
	if(IsLockMode())
	{
		if(m_LockTimer + g_pClientConfig->LOCK_TIME < GetTickCount())
			UnSetLockMode();
	}
	//if (m_ActionCount==m_ActionCountMax)
	//{
	//	int a =0;
	//}
	
	//----------------------------------------------------------------------
	// ÆÄÆ¼¿øµé¿¡°Ô ³ªÀÇ »óÅÂÁ¤º¸¸¦ º¸³½´Ù.
	//----------------------------------------------------------------------
	SendStatusInfoToParty();
	
#ifdef OUTPUT_DEBUG_PLAYER_ACTION
	DEBUG_ADD_FORMAT("[MPLayer::Action] Action[%d]=%d/%d. Move=%d/%d"
					, m_Action, m_ActionCount, m_ActionCountMax, m_MoveCount, m_MoveCountMax);	
#endif
	
	/* Test¹öÀü¿¡¼­ ÀÚµ¿À¸·Î ¹°¾à ¸Ô´Â ±â´É »èÁ¦ -_-
	// Å×½ºÆ®°¡ ¿ÀÈ÷·Á ´õ Èûµé¾î¼­·ò!
#ifdef __METROTECH_TEST__
	if(!IsVampire())
	{
		if (g_iAutoHealPotion == 1 && m_RecoveryHPTimes==0)	AutoUsePotion(0);		// HP
		if (g_bAutoManaPotion && m_RecoveryMPTimes==0)	AutoUsePotion(1);		// MP
		if (g_iAutoReload)	AutoUsePotion(2+g_iAutoReload-1);					// Magazine
//		if (g_iAutoHealPotion == 2 || g_iAutoHealPotion == 3)  AutoUseSkill(0);									// Healing
//		if (g_bHolyWater)  AutoUseSkill(1);									// CreateHolyWater
	}
#endif
	*/

	/*
	// [ TEST CODE ]
	// EffectTarget¿¡ °á°ú Ãß°¡ÇÏ±â Test
	if (m_listEffectTarget.size()!=0 && m_ActionCount==m_ActionCountMax)
	{
		MActionResult* pResult = new MActionResult;
		pResult->Add( new MActionResultNodeActionInfo( 1, 1001, GUIDANCE_FIRE ) );
		
		// °á°ú¿Í °ü·ÃµÈ Effect°¡ ¾ø´Â °æ¿ì..
		EFFECTTARGET_LIST::iterator iEffectTarget = m_listEffectTarget.begin();
		MEffectTarget* pEffectTarget = *iEffectTarget;
		
		BYTE id = pEffectTarget->GetEffectID();

		if (!AddActionResult(id, pResult))
		{
			// °á°ú¸¦ ¹Ù·Î ½ÇÇàÇÏ°í..
			// ¸Þ¸ð¸®¿¡¼­ Áö¿î´Ù.
			delete pResult;
		}
		
	}
	*/

	#ifdef OUTPUT_DEBUG
		if (m_ActionCount > m_ActionCountMax)
		{
			DEBUG_ADD("So Terrible!");
		}
	#endif

	//--------------------------------------------------------
	// µµ¿ò¸» --;
	//--------------------------------------------------------
//	__BEGIN_HELP_EVENT
//		static DWORD nextHelpTime = g_CurrentTime+20*1000;	// 20ÃÊ µÚ Ã³À½À¸·Î ¸Þ¼¼Áö Ãâ·Â
//		if (g_CurrentTime > nextHelpTime)
//		{
//			ExecuteHelpEvent( HE_TIME_PASSED );
//
//			nextHelpTime = g_CurrentTime + 20*1000;	// 20ÃÊ
//		}
//	__END_HELP_EVENT

	//--------------------------------------------------------
	// Status »óÅÂ º¯È­	- HP, MP º¯È­
	//--------------------------------------------------------
	UpdateStatus();	
	
	//--------------------------------------------------------
	// ¹ìÆÄÀÌ¾î·Î º¯ÇÏ´Â EffectÃ¼Å©
	//--------------------------------------------------------
	UpdateConversionTime();
	
	//--------------------------------------------------------
	// Ã¤ÆÃ String Scroll ½ÃÅ³ ½Ã°£
	//--------------------------------------------------------
	if (m_NextChatFadeTime < g_CurrentTime)
	{
		FadeChatString();
	}

	//--------------------------------------------------------
	// Æ¯¼öÇÑ È¿°úµé
	//--------------------------------------------------------
	if(UpDateInstallTurret())
		return;
	UpDateInstallMK1();
	UpdateInvisible();
	UpdateCasket();
	UpdateCutHeight();
	UpdateTurning();
	UpdateCauseCriticalWounds();
//	UpdateBloodyZenith();
	UpdateGunShotGuidance();
	UpdateFakeDie();
	UpdateGhost();

	UpdateDarkness();
	UpdateGunFiled();

	UpdateSweepVice();
	UpdateBurningSol();		

	UpdateBikeCrash();
	//--------------------------------------------------------
	// ¹æÇâ ÀüÈ¯À» ÀÚ¿¬½º·´°Ô
	//--------------------------------------------------------
	ChangeNearDirection();
	//m_CurrentDirection = m_Direction;

	//--------------------------------------------------------
	// ±â¼ú¿¡ µû¸¥ °á°ú Ç¥Çö Ã¼Å©
	//--------------------------------------------------------
	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD("beforeCheckListEffect");
	#endif
	
	if (m_nUsedActionInfo < g_pActionInfoTable->GetMinResultActionInfo())
	{		
		int numTarget = m_listEffectTarget.size();

		if (numTarget!=0)
		{
			//------------------------------------------------------------
			// ¹º°¡ ±â¼úÀ» »ç¿ëÇÏ°í ÀÖ´Ù¸é 
			// ±× ±â¼úÀÇ °á°ú ½ÃÁ¡¿¡¼­ °á°ú¸¦ º¸¿©ÁÖ°Ô ÇÒ·Á°í...
			//------------------------------------------------------------
			BOOL	bActionStand	= (m_Action==ACTION_STAND || IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND);
			//int		actionCountMax_1 = m_ActionCountMax-1;
			//BOOL	bEndAction		= (m_ActionCount==actionCountMax_1) || bActionStand;
			int		StartFrame		= (*g_pActionInfoTable)[m_nUsedActionInfo].GetStartFrame( m_WeaponSpeed );

			BOOL	bStartResult	= bActionStand 
										|| (m_ActionCount >= StartFrame)
										|| m_ActionCount >= m_ActionCountMax;

			bStartResult |= m_bRepeatAction 
							&& (*g_pActionInfoTable)[m_nUsedActionInfo].IsUseRepeatFrame()
							&& m_RepeatCount!=0
							&& m_ActionCount >= GetActionInfoRepeatStartFrame( m_nUsedActionInfo );

			// °á°ú¸¦ ÀüºÎ ½ÇÇàÇØ¹ö¸°´Ù.
			if (bStartResult)
			{
				EFFECTTARGET_LIST::iterator iEffectTarget = m_listEffectTarget.begin();
		
				while (iEffectTarget != m_listEffectTarget.end())
				{
					MEffectTarget* pEffectTarget = *iEffectTarget;

					if (pEffectTarget->IsResultTime())// || pEffectTarget->IsEnd())
					{
						MActionResult* pResult = pEffectTarget->GetResult();

						if (pResult!=NULL)
						{
							// ½ÇÇàÇÑ °á°ú´Â ¾ø¾Ø´Ù.
							int numTargetBeforeExecute = m_listEffectTarget.size();

							pEffectTarget->SetResultNULL();

							pResult->Execute();

							delete pResult;

							int numTargetAfterExecute = m_listEffectTarget.size();
							
							// pResult->Execute()¿¡¼­ m_listEffectTargetÀÌ Á¦°ÅµÉ ¼ö°¡ ÀÖ´Ù - -;
							// Äá°¡·ç ±¸Á¶... ¼öÁ¤ÀÌ ÇÊ¿äÇÏ´Ù - -;;
							if (numTargetBeforeExecute!=numTargetAfterExecute)
							{
								// Ã³À½ºÎÅÍ ´Ù½Ã Ã¼Å©
								iEffectTarget = m_listEffectTarget.begin();
								continue;
							}

							// delete´Â ¿ÜºÎ¿¡¼­ ÇÑ´Ù...
							// ¾Æ¸¶(-_-;;) Effect¿¡ ºÙ¿©¼­ Effect Á¦°ÅµÉ¶§ Áö¿ì´Â µí..
							// ±×·¯³ª.. º°·Î ±â¾ïÀÌ ¾È³ª´Âµ¥.. ¤Ñ.¤Ñ;;
							// ÀÏ´Ü Áö¿ì¸é ´Ù¿îµÇ´Âµ¥. ¾È Áö¿ì¸é ´Ù¿î¾ÈµÇ´Â°Éº¸¸é.. ¸Â´Âµí.. - -;;
							//delete pEffectTarget;
						}
					}

					iEffectTarget++;
				}		

				m_listEffectTarget.clear();
			}
		}
	}

	#ifdef OUTPUT_DEBUG_PLAYER_ACTION
		DEBUG_ADD("bcleok");
	#endif

	//--------------------------------------------------------
	// ±â¼ú »ç¿ëÈÄÀÇ delay°¡ ¸ðµÎ ³¡³­ »óÅÂÀÌ¸é...
	//--------------------------------------------------------	
	if (!m_bTurning	// [»õ±â¼ú]
		&& !HasEffectStatus(EFFECTSTATUS_CURSE_PARALYSIS)
		&& !HasEffectStatus(EFFECTSTATUS_STUN)
		&& !HasEffectStatus(EFFECTSTATUS_CURSE_OF_BLOOD)
		&& !HasEffectStatus(EFFECTSTATUS_FREEZE)
		&& !IsCauseCriticalWounds()
//		&& !IsBloodyZenith()
		&& !IsGunShotGuidance()
		&& !HasEffectStatus(EFFECTSTATUS_SOUL_CHAIN)
		&& !HasEffectStatus(EFFECTSTATUS_TRAP_TRIGGERED)
		&& !HasEffectStatus(EFFECTSTATUS_TRAPPED) 
		&& !HasEffectStatus(EFFECTSTATUS_unknow660)
		//&& !HasEffectStatus(EFFECTSTATUS_unknow231_21)
		//&& !HasEffectStatus(EFFECTSTATUS_994)
//		&& g_pUserInformation->LogoutTime == 0
		|| g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		|| g_bLight
#endif
		)// && !HasEffectStatus(EFFECTSTATUS_CAUSE_CRITICAL_WOUNDS))	//IsNotDelay())
	{
		//--------------------------------------------------------
		// ´ÙÀ½ bufferingµÈ actioninfo
		//--------------------------------------------------------
		/*
		if (m_nNextUsedActionInfo!=ACTIONINFO_NULL
			&& m_nUsedActionInfo==ACTIONINFO_NULL)
		{
			m_nUsedActionInfo = m_nNextUsedActionInfo;
			m_nNextUsedActionInfo = ACTIONINFO_NULL;
		}
		*/
		//--------------------------------------------------------
		// ¹«½¼ effect°¡ °É·ÁÀÖ´Ù¸é 2¹è ´À¸®°Ô ¿òÁ÷ÀÎ´Ù. // IsInFuryOfGnome À» ÀÌµ¿ , °ø¼Óµµ ´À·ÁÁö°Ô ¸¸µé¾î¾ß ÇÑ´Ù.
		//--------------------------------------------------------
		bool bAttackSlow = false, bMoveSlow = false;
		if (HasEffectStatus( EFFECTSTATUS_HAS_SLAYER_RELIC )       || HasEffectStatus( EFFECTSTATUS_HAS_VAMPIRE_RELIC ) || HasEffectStatus(EFFECTSTATUS_ABERRATION) ||
			HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_GREGORI )|| HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_NEMA ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_LEGIOS ) || HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_MIHOLE ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_AROSA )  || HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_ARMEGA ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_INI )    || HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_JAVE ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_CHASPA ) || HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_CONCILIA ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_KIRO )   || HasEffectStatus( EFFECTSTATUS_HAS_BLOOD_BIBLE_HILLEL ) ||
			HasEffectStatus( EFFECTSTATUS_AMBER_OF_GUARD ) || HasEffectStatus( EFFECTSTATUS_AMBER_OF_AVENGER ) ||
			HasEffectStatus( EFFECTSTATUS_AMBER_OF_IMMORTAL ) || HasEffectStatus( EFFECTSTATUS_AMBER_OF_CURSE ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_CASTLE_SYMBOL_5 ) || HasEffectStatus( EFFECTSTATUS_HAS_CASTLE_SYMBOL_6 ))
		{
			bAttackSlow = true; bMoveSlow = true;
		}
		if (IsCreatureActionAttack( this ) && (	HasEffectStatus( EFFECTSTATUS_ICE_OF_SOUL_STONE ) || (IsInFuryOfGnome() && !IsOusters())))
		{
			bAttackSlow = true;
		}
			
		if (IsCreatureMove( dynamic_cast<MCreature*>(this) ) && HasEffectStatus( EFFECTSTATUS_JABBING_VEIN ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER ) || HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_2 ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_3 ) || HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_4 ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_5 ) || HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_6 ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_7 ) || HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_8 ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_9 ) || HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_10 ) ||
			HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_11 ) || HasEffectStatus( EFFECTSTATUS_HAS_SWEEPER_12 ) ||
			HasEffectStatus( EFFECTSTATUS_NATURAL_PEACE ) || HasEffectStatus( EFFECTSTATUS_ICE_FIELD_TO_CREATURE ) ||
			HasEffectStatus( EFFECTSTATUS_FROZEN_ARMOR_TO_ENEMY )|| HasEffectStatus( EFFECTSTATUS_INSTALL_MK1 ))
		{
			bMoveSlow = true;
		}

			/*|| (IsInFuryOfGnome() && !IsOusters()) // Ç»¸®¿Àºê³ë¿ò ÀÌ¼Ó ´À·ÁÁö´Â È¿°ú »èÁ¦ - 2006.11.21 by chyaya*/
		if(!g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
			&& !g_bLight
#endif
			&& (IsCreatureActionAttack(this) && bAttackSlow == true) || (IsCreatureMove(dynamic_cast<MCreature*>(this)) && bMoveSlow == true)
			)			
		{
			if (g_CurrentFrame & 0x01)
			{
				return;
			}
		}
		

		//--------------------------------------------------------
		// KnockBack Ã³¸® 2001.10.9
		//--------------------------------------------------------
		if (m_bKnockBack > 0)
		{
			m_sX += m_cX;
			m_sY += m_cY;

			// knockBack ´Ùx µÆÀ¸¸é Á¤Áöµ¿ÀÛ.
			if (--m_bKnockBack==0)
			{
				m_sX = 0;
				m_sY = 0;

				SetAction( ACTION_STAND );

				return;
			}			
		}
		//--------------------------------------------------------
		// FastMoveÁß¿¡´Â actionÀ» ÃëÇÏÁö ¾Ê´Â´Ù. 2001.8.10
		//--------------------------------------------------------
		else if (!m_bFastMove)
		{
			//--------------------------------------------------------
			// Lock Mode »ç¿ë
			//--------------------------------------------------------
			if (IsLockMode() && !IsInDarkness())
			{
				MCreature*	pCreature  = m_pZone->GetCreature(GetTraceID());

				int creatureID;

				// ³­ ÇÑ³ð¸¸ ÆÐ
				if( pCreature == NULL ||
					(pCreature != NULL && (pCreature->IsDead() || pCreature->GetName() == NULL)) 
					)
					creatureID = FindEnemy();
				else
				{
					creatureID = GetTraceID();

					//20070618 by diesirace 
					//±â¼úÀ» ¾µ¶§´Â ¸ØÃç ÀÖ´Â »óÅÂ ¹º°¡ ±â¼úÀ» ¾²´Â »óÅÂ ÀÌµ¿ÇÏ´Â »óÅÂ
					//µîµîÀÇ »óÅÂÀÌ°í µû¶ó¼­ ´ÙÀ½ÀÌ µÇ¾ß ½ÃÀüÀÌ °¡´ÉÇÑ°ÍÀÌ µÊÀ¸·Î ¸ðµç Çàµ¿Àº 
					//´ÙÀ½ÀÌ µÇ¾ß ½ÇÇàÀÌ µÈ´Ù. µû¶ó¼­ ÀÚ±â ÀÚ½Å¿¡°Ô ±â¼úÀ» 
					//·è ¸ðµå ÀÏ¶§ ÀÚ±â ÀÚ½Å¿¡°Ô ±â¼úÀ» ¾µ¶§ ¾´´Ù°í ¸í½Ã ÇÔÀ¸·Î½á ´ÙÀ½ Å¸°ÙÀÌ µÇ´Â ÀûÀ»
					//ÁöÁ¤Çß´ø Å©¸®Ã³·Î µ¹¸±¼ö ÀÖ´Ù. µû¶ó¼­ ÀÚ»ì¹ö±×µµ °íÃÄ Áø´Ù.
					if(creatureID == g_pPlayer->GetID())
					{
						m_fNextTrace	= FLAG_TRACE_SELF;
						m_NextTraceID	= creatureID;
					}
				}
				
				

				//--------------------------------------------------------
				// »çÁ¤°Å¸® ÀÌ³»¿¡ ÀûÀÌ ¾ø´Â °æ¿ì
				//--------------------------------------------------------
				if (creatureID == OBJECTID_NULL)
				{
					m_listDirection.clear();

					UnSetRepeatAction();
					TraceNULL();
					m_fTraceBuffer	= FLAG_TRACE_NULL;	
					m_fNextTrace	= FLAG_TRACE_NULL;

					// ±æÃ£±â Çß´ø°É ¾ø¾ÖÁØ´Ù.
					m_DestX		= SECTORPOSITION_NULL;
					m_DestY		= SECTORPOSITION_NULL;
					m_NextDestX	= SECTORPOSITION_NULL;
					m_NextDestY	= SECTORPOSITION_NULL;

					g_pTopView->SetSelectedNULL();
				}
				//--------------------------------------------------------
				// ÀûÀÌ ÀÖ´Â °æ¿ì
				//--------------------------------------------------------
				else
				{
					int bBasicAction = g_bLButtonDown;
					int bSpecialAction = g_bRButtonDown;

					//---------------------------------------------------------------
					// ±âº» Çàµ¿
					//-------------------------------------------------------------
					if (bBasicAction)
					{
						if (TraceCreatureToBasicAction( 
									creatureID, 
									true ))		// °­Á¦ °ø°Ý
						{
							//----------------------------------
							// ¹Ýº¹ actionÀÎ °æ¿ì¸¸ ¹Ýº¹ ¼³Á¤
							//----------------------------------
							//if (m_nBasicActionInfo!=ACTIONINFO_NULL 
							//	&& (*g_pActionInfoTable)[m_nBasicActionInfo].IsUseRepeatFrame()
							//	&& !IsRepeatAction())
							//{
							//	SetRepeatAction();
							//}

							g_pTopView->SetSelectedCreature( creatureID );			
							gpC_mouse_pointer->SetCursorAttack();					

							//g_bPreviousMove = false;
						}
					}
					//-------------------------------------------------------------
					// Æ¯¼ö Çàµ¿
					//-------------------------------------------------------------
					else if (bSpecialAction)
					{
						if (TraceCreatureToSpecialAction( creatureID, true ))
						{
							//----------------------------------
							// ¹Ýº¹ actionÀÎ °æ¿ì¸¸ ¹Ýº¹ ¼³Á¤
							//----------------------------------
							//if (m_nSpecialActionInfo!=ACTIONINFO_NULL 
							//	&& (*g_pActionInfoTable)[m_nSpecialActionInfo].IsUseRepeatFrame()
							//	&& !IsRepeatAction()
							//	)
							//{
								SetRepeatAction();
								g_bPreviousMove = false;
							//}							

							g_pTopView->SetSelectedCreature( creatureID );			
							gpC_mouse_pointer->SetCursorAttack();					

							//g_bPreviousMove = false;
						}
					}					

					// ¸ñÀûÁö Ç¥½Ã¸¦ ¾ø¾Ø´Ù.
					g_pTopView->SetSelectedSectorNULL();
				}
				
			}
			//--------------------------------------------------------
			// ÇÇ Èê¸®´Â°Å Ã¼Å©
			//--------------------------------------------------------
			if (m_bAlive)
			{
				CheckDropBlood();
			}

			// delayTimeÀº ¾ø´Â °É·Î settingÇÑ´Ù.
			//m_DelayTime	= 0;
			//----------------------------------------------------
			// °è¼Ó ÃßÀûÇÏ´Â °æ¿ì
			// À½.. ÁöÀúºÐÇÑ ÄÚµå°¡ ´Ã¾î³­´Ù... - -;;
			//----------------------------------------------------
			if (m_bKeepTraceCreature && (m_fTrace & FLAG_TRACE_CREATURE_BASIC))
			{							
				KeepTraceCreature();		
			}
			
			//------------------------------------------		
			//     BufferingµÈ ´ÙÀ½ Çàµ¿
			//------------------------------------------
			BOOL bSlayer = IsSlayer();
			if (m_bNextAction ||
				(!IsOusters() || IsOusters() && m_NextAction!=ACTION_OUSTERS_FAST_MOVE_STAND 
#if __CONTENTS(__FAST_TRANSFORTER)
				&& m_NextAction!=ACTION_OUSTERS_WING_STAND
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
				&& m_NextAction!=ACTION_OUSTERS_UNICORN_STAND
#endif //__SECOND_TRANSFORTER
				)
				&& m_NextAction!=ACTION_STAND 
				&& (!bSlayer || bSlayer && m_NextAction!=ACTION_SLAYER_MOTOR_STAND)
				&& m_NextAction!=m_MoveAction)
			{
				SetAction( m_NextAction );
				
				#ifdef OUTPUT_DEBUG
					if (m_ActionCount > m_ActionCountMax)
					{
						DEBUG_ADD("So Terrible! return1");
					}
				#endif

				return;
			}

			//------------------------------------------
			// [ TEST CODE ]
			//------------------------------------------
			// ¿òÁ÷ÀÌ´Â µµÁß¿¡..
			// ÃßÀûÇÏ´Â °æ¿ì ÃßÀû¸ñÇ¥°Å¸®°¡ µÇ´Â °æ¿ì¿¡´Â..
			// ¹Ù·Î ÀÌ¹ø¿¡.. actionÀ» ¼öÇàÇÏ°Ô ÇØ¾ßÇÑ´Ù.
			// ±×·¡¼­, return false
			if (m_NextAction==m_MoveAction &&
				m_fTrace != FLAG_TRACE_NULL &&
				(max(abs(m_X-m_TraceX), abs(m_Y-m_TraceY))) <= m_TraceDistance)
			{
				SetAction( m_NextAction );
			}

			//------------------------------------------
			// CastingTimeÀÌ ¾øÀ¸¸é
			// Action ¼öÇà.. ´ÙÀ½ FrameÀ¸·Î...
			//------------------------------------------
			if (m_ActionCount < m_ActionCountMax)
			{	
				if(m_nUsedActionInfo != ACTIONINFO_NULL)
				{
					ActionEffect();
				}
				else
				{
					//m_ActionCount--;

					// ÀûÀýÇÑ Action ¼öÇà
					//switch (m_Action)
					//

					BOOL bSlayer = IsSlayer();

						// Á¤Áö µ¿ÀÛ
					if (m_Action==ACTION_STAND || IsOusters() && m_Action==ACTION_OUSTERS_FAST_MOVE_MOVE
						|| bSlayer && m_Action==ACTION_SLAYER_MOTOR_STAND
#if __CONTENTS(__FAST_TRANSFORTER)
						|| IsOusters() && m_Action==ACTION_OUSTERS_WING_MOVE
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
						|| IsOusters() && m_Action==ACTION_OUSTERS_UNICORN_MOVE
#endif //__SECOND_TRANSFORTER
						)
					{
												
						m_ActionCount++;						

						//------------------------------------------------------------
						// ¹Ýº¹ Çàµ¿ Ã¼Å©..
						//------------------------------------------------------------
						if (CheckRepeatAction())
						{
							if (CheckBufferAction())
							{
								if (m_nNextUsedActionInfo != ACTIONINFO_NULL)
								{
									// ¾Æ.. Çò°¥·Á¶ó.. - -;
									m_nUsedActionInfo = m_nNextUsedActionInfo;
									m_nNextUsedActionInfo = ACTIONINFO_NULL;							

									if (ActionInTraceDistance())
									{
										if (m_nNextUsedActionInfo != ACTIONINFO_NULL)
										{
											// ¾Æ.. Çò°¥·Á¶ó.. - -;
											m_nUsedActionInfo = m_nNextUsedActionInfo;
											m_nNextUsedActionInfo = ACTIONINFO_NULL;
										}
										else
										{
											// ´õ ÀÌ»ó °¥ ÇÊ¿ä°¡ ¾ø´Ù.
											m_listDirection.clear();

											// ÀÌµ¿ ÁßÁö
											m_MoveCount = m_MoveCountMax;

											// Player°¡ ¹Ù¶óº¸´Â ¹æÇâÀÌ ÇöÀç ÀûÀÌ ÀÖ´Â ¹æÇâÀÌ µÇµµ·Ï ÇÑ´Ù.				
											SetDirectionToPosition( m_TraceX, m_TraceY );

											SetAction( m_NextAction );

											//ActionToSendPacket();
										}
									}
								}
								#ifdef OUTPUT_DEBUG
									if (m_ActionCount > m_ActionCountMax)
									{
										DEBUG_ADD("So Terrible! return2");
									}
								#endif

								return;
							}
						}					

						/*
						else if (m_Action==ACTION_MOVE 
								|| bSlayer && ACTION_SLAYER_MOTOR_MOVE)
						{
							// Move´Â µû·Î Ã³¸®ÇÑ´Ù.
							//ActionMove();					
						}
						*/					
					}
					else
					{
					//default :
						if (m_nUsedActionInfo==ACTIONINFO_NULL)
						{	
							m_ActionCount++;

							if (m_bStopBloodDrain)
							{
								StopBloodDrain();
							}							
							if (m_bStopAbsorbSoul)
							{
								StopAbsorbSoul();
							}							
						}
						else
						{
							ActionEffect();
						}
					}
				}
			}
		}
		


		//------------------------------------------
		// ActionÀÌ ³¡³­ »óÅÂÀÌ¸é
		//------------------------------------------
		if (m_bAlive)
		{
			// CT_BAT: bat move debug — hareket beklentisi varken 250ms throttle ile dispatch durumu
			if (m_DestX!=SECTORPOSITION_NULL || m_NextDestX!=SECTORPOSITION_NULL || m_NextAction==m_MoveAction || m_bMoveRetargetPending)
			{
				static DWORD s_lastActionDispatchLog = 0;
				DWORD nowTick = GetTickCount();
				if (nowTick - s_lastActionDispatchLog >= 250)
				{
					s_lastActionDispatchLog = nowTick;
					CT_BAT("Action DISPATCH-STATE: ctype=%d m_Action=%d next=%d moveAction=%d aCount=%d/%d mCount=%d/%d dest=(%d,%d) nextDest=(%d,%d) dirList=%d sendMove=%d bNextAction=%d retargetPending=%d",
						(int)m_CreatureType, (int)m_Action, (int)m_NextAction, (int)m_MoveAction,
						(int)m_ActionCount, (int)m_ActionCountMax, (int)m_MoveCount, (int)m_MoveCountMax,
						(int)m_DestX, (int)m_DestY, (int)m_NextDestX, (int)m_NextDestY,
						(int)m_listDirection.size(), (int)m_SendMove, (int)m_bNextAction, (int)m_bMoveRetargetPending);
				}
			}
			// ºÎµîÈ£ Ãß°¡ 2001.10.7
			if (m_ActionCount>=m_ActionCountMax) //if (IsStop())
			{
				//--------------------------------------------------------
				// ¸ñÇ¥ Å¸ÀÏ¿¡ µµÂøÇÑ »óÅÂ¿¡¼­..
				// °è¼Ó ¼­ ÀÖ°Å³ª.. ´Ù °ÉÀº °æ¿ì´Â.. 
				// Á¦ÀÚ¸®¿¡¼­ Èçµé°Å¸®´Â ¸ð½ÀÀ» Ç¥ÇöÇØÁØ´Ù.
				//--------------------------------------------------------
				// ºÎµîÈ£ Ãß°¡ 2001.10.7
				if (m_MoveCount>=m_MoveCountMax)
				{					
					//if (m_bRepeatAction && m_RepeatCount!=0 && (*g_pActionInfoTable)[m_nUsedActionInfo].IsRepeatFrame())
					//{						
					//	int a =0;
					//}
					//------------------------------------------------------------
					// ¹Ýº¹ Çàµ¿ Ã¼Å©..
					//------------------------------------------------------------
					//if (
						bool bCheckRepeat = CheckRepeatAction();
						//)
					//{
					//}

					if ((!m_bRepeatAction || !bCheckRepeat)
						&& m_fNextTrace != FLAG_TRACE_NULL)
					{			
						if (m_ActionCount>=m_ActionCountMax)
						{
							SetAction( ACTION_STAND );
						}
					}

					if (CheckBufferAction()) 
					{
						/*
						if (m_NextAction==m_MoveAction &&
							//m_fTrace != FLAG_TRACE_NULL &&
							(max(abs(m_X-m_TraceX), abs(m_Y-m_TraceY))) <= m_TraceDistance)
						{
							SetAction( m_NextAction );
						}
						*/
						if (m_nNextUsedActionInfo != ACTIONINFO_NULL)
						{
							// ¾Æ.. Çò°¥·Á¶ó.. - -;
							m_nUsedActionInfo = m_nNextUsedActionInfo;
							m_nNextUsedActionInfo = ACTIONINFO_NULL;							

							if (ActionInTraceDistance())
							{
								if (m_nNextUsedActionInfo != ACTIONINFO_NULL)
								{
									// ¾Æ.. Çò°¥·Á¶ó.. - -;
									m_nUsedActionInfo = m_nNextUsedActionInfo;
									m_nNextUsedActionInfo = ACTIONINFO_NULL;
								}
								else
								{
									// ´õ ÀÌ»ó °¥ ÇÊ¿ä°¡ ¾ø´Ù.
									m_listDirection.clear();

									// ÀÌµ¿ ÁßÁö
									m_MoveCount = m_MoveCountMax;

									// Player°¡ ¹Ù¶óº¸´Â ¹æÇâÀÌ ÇöÀç ÀûÀÌ ÀÖ´Â ¹æÇâÀÌ µÇµµ·Ï ÇÑ´Ù.				
									SetDirectionToPosition( m_TraceX, m_TraceY );

									SetAction( m_NextAction );

									//ActionToSendPacket();
								}
							}							
						}

						#ifdef OUTPUT_DEBUG
							if (m_ActionCount > m_ActionCountMax)
							{
								DEBUG_ADD("So Terrible! return3");
							}
						#endif

						return;
					}

					//------------------------------------------
					// Á¦ÀÚ¸®¿¡¼­ Èçµé°Å¸®´Â ¸ð½À
					//------------------------------------------
					if (m_bRepeatAction)// && m_RepeatCount!=0)// && (*g_pActionInfoTable)[m_nUsedActionInfo].IsRepeatFrame())
					{						
					}
					else
					{
						SetAction( ACTION_STAND );
					}
					
				}
				//--------------------------------------------------------
				// °È´Ù°¡ ´Ù¸¥ actionÀ» º¸¿©Áá°Å³ª °è¼Ó °È´ø ÁßÀÌ´Ù.
				// ´Ù½Ã °È´Â´Ù.
				//--------------------------------------------------------
				else
				{
					if (m_MoveCount==0)
					{
						CT_BAT("Action DISPATCH->ActionMove: ctype=%d moveCount=%d/%d dest=(%d,%d) dirList=%d sendMove=%d",
							(int)m_CreatureType, (int)m_MoveCount, (int)m_MoveCountMax, (int)m_DestX, (int)m_DestY, (int)m_listDirection.size(), (int)m_SendMove);
					}
					m_Action = m_MoveAction;
					ActionMove();
#if __CONTENTS(__HERO_SYSTEM)
					if (HasEffectStatus(EFFECTSTATUS_SLAYER_HERO) ||
						HasEffectStatus(EFFECTSTATUS_VAMPIRE_HERO) ||
						HasEffectStatus(EFFECTSTATUS_OUSTERS_HERO) ||
						HasEffectStatus(EFFECTSTATUS_LEVELWAR_SLAYER_HERO) ||
						HasEffectStatus(EFFECTSTATUS_LEVELWAR_VAMPIRE_HERO) ||
						HasEffectStatus(EFFECTSTATUS_LEVELWAR_OUSTERS_HERO)
						)
					{
						ActionMove();
					}
#endif //__HERO_SYSTEM

					//----------------------------------------------------
					// ÀÌµ¿ÀÌ ¸ðµÎ ³¡³µÀ¸¸é..
					// Á¦ÀÚ¸®¿¡¼­ Èçµé°Å¸®´Â ¸ð½À 
					//----------------------------------------------------
					if (m_MoveCount==m_MoveCountMax)
					{		
						if (m_fNextTrace != FLAG_TRACE_NULL)
						{		
							if (m_ActionCount>=m_ActionCountMax)
							{
								SetAction( ACTION_STAND );
							}
						}

						if (CheckBufferAction()) 
						{
							if (m_nNextUsedActionInfo != ACTIONINFO_NULL)
							{
								// ¾Æ.. Çò°¥·Á¶ó.. - -;
								m_nUsedActionInfo = m_nNextUsedActionInfo;
								m_nNextUsedActionInfo = ACTIONINFO_NULL;							

								if (ActionInTraceDistance())
								{
									if (m_nNextUsedActionInfo != ACTIONINFO_NULL)
									{
										// ¾Æ.. Çò°¥·Á¶ó.. - -;
										m_nUsedActionInfo = m_nNextUsedActionInfo;
										m_nNextUsedActionInfo = ACTIONINFO_NULL;
									}
									else
									{
										// ´õ ÀÌ»ó °¥ ÇÊ¿ä°¡ ¾ø´Ù.
										m_listDirection.clear();

										// ÀÌµ¿ ÁßÁö
										m_MoveCount = m_MoveCountMax;

										// Player°¡ ¹Ù¶óº¸´Â ¹æÇâÀÌ ÇöÀç ÀûÀÌ ÀÖ´Â ¹æÇâÀÌ µÇµµ·Ï ÇÑ´Ù.				
										SetDirectionToPosition( m_TraceX, m_TraceY );

										SetAction( m_NextAction );

										//ActionToSendPacket();
									}
								}
							}

							#ifdef OUTPUT_DEBUG
								if (m_ActionCount > m_ActionCountMax)
								{
									DEBUG_ADD("So Terrible! return4");
								}
							#endif

							return;
						}

						//------------------------------------------
						// Á¦ÀÚ¸®¿¡¼­ Èçµé°Å¸®´Â ¸ð½À
						//------------------------------------------				
						if (m_ActionCount>=m_ActionCountMax)
						{
							SetAction( ACTION_STAND );
						}
					}
				}			
			}	
		}
	}

	
	//--------------------------------------------------------
	// Delay°¡ ÀÖ´Â °æ¿ì...  
	// »ì¾Æ ÀÖ´Â °æ¿ì¿¡¸¸ --> Á¤Áöµ¿ÀÛÀ» º¸¿©ÁØ´Ù.
	//--------------------------------------------------------
	/*
	else if (m_bAlive)
	{
		if (m_ActionCount < m_ActionCountMax)
		{	
			if (m_Action==ACTION_STAND 
				|| IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND)
			{
				m_ActionCount++;
			}		

			// actionÀÌ ³¡³µÀ¸¸é..
			if (m_ActionCount==m_ActionCountMax) 
			{
				// ´Ù½Ã Á¤Áö µ¿ÀÛ
				SetAction( (m_MoveAction==ACTION_SLAYER_MOTOR_MOVE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND );
			}		hflh
		}
		else
		{
			SetAction( (m_MoveAction==ACTION_SLAYER_MOTOR_MOVE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND );
		}
	}
	*/
	
}

//----------------------------------------------------------------------
// Add EffectTarget
//----------------------------------------------------------------------
// ÁøÇàÁßÀÎ EffectTargetÀ» Ãß°¡ÇÑ´Ù.
//----------------------------------------------------------------------
void
MPlayer::AddEffectTarget(MEffectTarget* pEffectTarget)
{
	// ±âÁ¸¿¡²¨ ´Ù Áö¿ö¹ö¸°´Ù. ¤»¤»
	m_listEffectTarget.clear();

	// »õ°Å ÇÏ³ª Ãß°¡
	m_listEffectTarget.push_back( pEffectTarget );
}

//----------------------------------------------------------------------
// Remove EffectTarget
//----------------------------------------------------------------------
// ÁøÇàÁßÀÌ´ø Effect°¡ ³¡³­ °æ¿ì...
// ±×¿Í °ü·ÃÀÖ´Â TargetÀ» Á¦°ÅÇÑ´Ù.
//----------------------------------------------------------------------
void
MPlayer::RemoveEffectTarget(BYTE id)
{
	EFFECTTARGET_LIST::iterator iEffectTarget = m_listEffectTarget.begin();

	// id°¡ °°Àº °æ¿ì¸¦ Ã£¾Æ¼­ Á¦°ÅÇÑ´Ù.
	// listÀÌ±â ¶§¹®¿¡.. ´Ù ºñ±³ÇØ¾ß ÇÏÁö¸¸...
	// ½ÇÁ¦·Î Player°¡ »ç¿ëÁßÀÎ ±â¼úÀÌ ¸î°³~ ¹Û¿¡ ¾ÈµÇ±â ¶§¹®¿¡...
	while (iEffectTarget != m_listEffectTarget.end())
	{
		// id°¡ °°Àº °æ¿ì
		if ((*iEffectTarget)->GetEffectID() == id)
		{
			// ¸Þ¸ð¸®¿¡¼­ Á¦°Å´Â ¿ÜºÎ¿¡¼­ ÇÑ´Ù.
			//delete (*iEffectTarget);

			// list¿¡¼­ Á¦°ÅÇÑ´Ù.			
			m_listEffectTarget.erase( iEffectTarget );

			break;
		}

		iEffectTarget++;
	}
}

//----------------------------------------------------------------------
// Packet Back
//----------------------------------------------------------------------
// ÀÌµ¿ ÁßÀÌ¸é..
// ¹Ù·Î ÀüÀÇ Ä­À¸·Î BackÇÑ´Ù.
// °ø°Ý ´çÇßÀ»¶§ÀÇ Ã³¸®ÀÌ´Ù.
// ÇØÅ·ÀÇ À§ÇèÀÌ ÀÖÁö ¾ÊÀ»±î.. ÈìÈì.. - -;;
//----------------------------------------------------------------------
/*
void	
MPlayer::PacketBack()
{
	// Á¤ÁöµÈ »óÅÂÀÌ¸é ÇÊ¿ä°¡ ¾ø´Ù.
	if (m_sX==0 && m_sY==0)
		return;

	// ¹Ý´ë¹æÇâ.. 	
	//m_sX = -m_sX;
	//m_sY = -m_sY;
	//m_cX = -m_cX;
	//m_cY = -m_cY;
	//m_ActionCount = m_ActionCountMax - m_ActionCount;
	

	// Á¤ÁöµÈ »óÅÂ°¡ ¾Æ´Ï¸é.. Á¤Áö ½ÃÅ²´Ù.
	SetStop();

	// ¹Ù·Î ÀüÀÇ À§Ä¡´Â...
	// ÇöÀç À§Ä¡¿¡¼­ ÁøÇà¹æÇâÀÇ ¹Ý´ë¹æÇâÀ¸·Î µÇµ¹¾Æ °¡¸é µÈ´Ù.
	m_Direction = GetCounterDirection( m_Direction );

	// ¹æÇâÀ¸·Î ¿òÁ÷ÀÎ ÁÂÇ¥¸¦ ¾ò´Â´Ù.
	GetPositionToDirection(m_X, m_Y, m_Direction);

	// BackÀÌµ¿À» Çß´Ù°¡ Server·Î º¸³½´Ù.
	#ifdef	CONNECT_SERVER
		CGMove _CGMove;
		_CGMove.setX( m_X );
		_CGMove.setY( m_Y );
		_CGMove.setDir( m_Direction );
		g_pSocket->sendPacket( &_CGMove );

		#if defined(OUTPUT_DEBUG) && defined(__DEBUG_OUTPUT__)
				sprintf(g_pDebugMessage->GetCurrent(), "[Send] [Back]%s", _CGMove.toString().c_str());
				g_pDebugMessage->Next();
		#endif

		// Server·Î º¸³½ ¹æÇâÀ» ±â¾ïÇØµÐ´Ù.
		m_listSendDirection.push_back( m_Direction );
		m_SendMove++;
		
	#endif
}
*/

//----------------------------------------------------------------------
// Packet MoveOK
//----------------------------------------------------------------------
// Server¿¡¼­ MOVE_OK¸¦ ¹ÞÀº °æ¿ì
//
// ÇöÀçÀ§Ä¡¿¡¼­ direction¹æÇâÀÎ (x,y) sector·Î ÀÌµ¿°¡´ÉÇÏ´Ù´Â ÀÇ¹ÌÀÌ´Ù.
//----------------------------------------------------------------------
void
MPlayer::PacketMoveOK(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, BYTE direction)
{
	CT_BAT("PacketMoveOK ENTRY: server=(%d,%d) dir=%d sendMove=%d pos=(%d,%d) dest=(%d,%d) ctype=%d",
		(int)sX, (int)sY, (int)direction, (int)m_SendMove, (int)m_X, (int)m_Y, (int)m_DestX, (int)m_DestY, (int)m_CreatureType);
	// 2004, 9, 16, sobeit add start - ÅÍ·¿ ¹æÇâ ¹Ù²Ù±â, »¬±î..¤Ñ¤Ñ;
//	if(HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
//	{
//		SetCurrentDirection(direction);
//		return;
//	}
	// 2004, 9, 16, sobeit add end
	// 2004, 9, 21, sobeit add start - ¾Æ´ã ºÏ ±¸¸§
	m_bShowAdamCloud = false;
	if(NULL != g_pZone && g_pZone->GetID() == 74)
	{
		if(sY<60)
			m_bShowAdamCloud = true;
	}
	// 2004, 9, 21, sobeit add end
	//--------------------------------------------------------
	// ±â¾ïÇØµÐ °Í Áß¿¡¼­..
	// °ËÁõ¹ÞÀº ¹æÇâÀ» Á¦°ÅÇÑ´Ù.
	//--------------------------------------------------------
	if (!m_listSendDirection.empty())
	{
		m_listSendDirection.pop_front();
	}

	//--------------------------------------------------------
	// MoveError´ÙÀ½¿¡ MoveOK°¡ ¹Ù·Î ³¯¾Æ¿À´Â °æ¿ì
	//--------------------------------------------------------
	if (m_SendMove==0)
	{
		//--------------------------------------------------------
		// MoveError´ÙÀ½¿¡ ¹Ù·Î MoveOK°¡ ³¯¾Æ¿À´Â °æ¿ì°¡ ÀÖ¾ú´Ù.
		// ÀÌ °æ¿ì.. (¾Æ¸¶ - -;) ÁÂÇ¥°¡ »ß²ý~ÇØ¼­ 
		// °è¼Ó ´ÙÀ½ºÎÅÍ ÁÂÇ¥ °ËÁõÀ» ¸ø¹Þ´Â°Å °°´Ù.
		// ±×·¡¼­.. ÀÓ½Ã(?) ´ëÃ¥À¸·Î... Server¿¡¼­ °ËÁõµÈ ÁÂÇ¥·Î ¹Ù·Î ÀÌµ¿½ÃÅ²´Ù.
		//--------------------------------------------------------
		MovePosition( sX, sY );

		// ÀÌµ¿À» ÁßÁöÇÑ´Ù.
		m_sX = 0;	
		m_sY = 0;

		m_MoveCount		= m_MoveCountMax;
		m_ActionCount	= m_ActionCountMax; 

		// ±æÃ£±â Á¦°Å
		m_listDirection.empty();			

		//----------------------------------------------------
		// Creature¸¦ °è¼Ó ÃßÀûÇÏ´ø ÁßÀÌ¸é..
		//----------------------------------------------------
		if (m_bKeepTraceCreature						// °è¼Ó ÃßÀûÇÏ´Â °æ¿ì
			&& (m_fTrace & FLAG_TRACE_CREATURE_BASIC)	// creature¸¦ ÃßÀûÇÏ´Â °æ¿ì
			&& !m_bTraceCreatureToForceAttack)			// ±×³É µû¶ó°¡´Â °æ¿ì
		{							
			KeepTraceCreature();		
		}
		//----------------------------------------------------
		// º¸Åë ÀÌµ¿½Ã¿¡ ÁßÁöµÈ °æ¿ì
		//----------------------------------------------------
		else
		{		
			TraceNULL();	

			// ±æÃ£±â Çß´ø°É ¾ø¾ÖÁØ´Ù.
			m_DestX		= SECTORPOSITION_NULL;
			m_DestY		= SECTORPOSITION_NULL;
			m_NextDestX	= SECTORPOSITION_NULL;
			m_NextDestY	= SECTORPOSITION_NULL;
		}
	}
	//--------------------------------------------------------
	// Á¤»óÀûÀÎ MoveOKÀÎ °æ¿ì
	//--------------------------------------------------------
	else if (m_SendMove>0)
	{
		m_SendMove--;		
	}

	g_bNetStatusGood = true;

	//--------------------------------------------------------
	// Server¿¡¼­ °ËÁõµÈ À§Ä¡¸¦ ±â¾ïÇØµÐ´Ù.
	//--------------------------------------------------------
	//POINT nextPoint;
	//MCreature::GetNextPosition(direction, nextPoint);
//	m_ServerX	= nextPoint.x;
//	m_ServerY	= nextPoint.y;

	m_ServerX	= sX;
	m_ServerY	= sY;


	//--------------------------------------------------------
	// (m_ServerX, m_ServerY)¿¡¼­ºÎÅÍ 
	// PlayerÀÇ ½Ã¾ß¿¡ Æ÷ÇÔµÇÁö ¾Ê´Â Creature¸¦ Zone¿¡¼­ Á¦°Å½ÃÅ²´Ù.
	//--------------------------------------------------------
	if (m_pZone!=NULL)
	{
		m_pZone->KeepObjectInSight(m_ServerX, m_ServerY, m_Sight);
	}

	SendPositionInfoToParty();

	return;
}

//----------------------------------------------------------------------
// Packet MoveNO
//----------------------------------------------------------------------
// Server¿¡¼­ MOVE_NO¸¦ ¹ÞÀº °æ¿ì
//----------------------------------------------------------------------
void
MPlayer::PacketMoveNO()
{
	CT_BAT("PacketMoveNO ENTRY: SERVER MOVE REJECT — pos=(%d,%d) dest=(%d,%d) sendMove=%d ctype=%d",
		(int)m_X, (int)m_Y, (int)m_DestX, (int)m_DestY, (int)m_SendMove, (int)m_CreatureType);
	//m_listSendDirection.front();
	if (!m_listSendDirection.empty())
	{
		m_listSendDirection.pop_front();
	}

	m_SendMove = 0;

	//------------------------------------------------------------
	// ÇöÀç À§Ä¡·Î ¿Ã ¼ö ÀÖ´Â ±æÀÌ ÀÖ´ÂÁö »ìÆìº»´Ù.
	//
	// ±æÀÌ ¾ø´Â °æ¿ìÀÌ°Å³ª ±æÀÌ ³Ñ ±ä °æ¿ì?´Â 
	//		ÃÖ±ÙÀÇ °ËÁõ¹ÞÀº À§Ä¡(ServerX,Y)·Î ÀÌµ¿ÇÑ´Ù.
	//
	// ±æÀÌ ÀÖ´Ù¸é, Ã£Àº À§Ä¡±îÁöÀÇ ÀÌµ¿ packetÀ» º¸³½´Ù.
	//------------------------------------------------------------
	//if (!SetDestination( m_X, m_Y ) || m_listDirection.empty()	// ±æÀÌ ¾ø´Â °æ¿ì
		//|| m_SendMove > g_pClientConfig->MAX_CLIENT_MOVE)		// ±æÀÌ ³Ñ ±ä °æ¿ì
	{
		// Zone¿¡¼­ ÇöÀç ÀÖ´Â Sector¿¡¼­ Á¦°ÅÇÏ°í 
		// Server¿¡¼­ °ËÁõµÈ ÃÖ±ÙÀÇ Sector·Î ÀÌµ¿ÇÑ´Ù.
		MovePosition( m_ServerX, m_ServerY );
		

		// ÀÌµ¿À» ÁßÁöÇÑ´Ù.
		m_sX = 0;	
		m_sY = 0;
		
		m_MoveCount		= m_MoveCountMax;
		//m_ActionCount	= m_ActionCountMax; 

		//----------------------------------------------------
		// Creature¸¦ °è¼Ó ÃßÀûÇÏ´ø ÁßÀÌ¸é..
		//----------------------------------------------------
		if (m_bKeepTraceCreature						// °è¼Ó ÃßÀûÇÏ´Â °æ¿ì
			&& (m_fTrace & FLAG_TRACE_CREATURE_BASIC)	// creature¸¦ ÃßÀûÇÏ´Â °æ¿ì
			&& !m_bTraceCreatureToForceAttack			// ±×³É µû¶ó°¡´Â °æ¿ì
			&& m_ActionCount>=m_ActionCountMax)			// Æ¯º°ÇÑ µ¿ÀÛ¾øÀÌ ¿òÁ÷ÀÌ°í ÀÖ´ø °æ¿ì
		{							
			KeepTraceCreature();		
		}
		//----------------------------------------------------
		// º¸Åë ÀÌµ¿½Ã¿¡ ÁßÁöµÈ °æ¿ì
		//----------------------------------------------------
		else
		{		
			// ±æÃ£±â Çß´ø°É ¾ø¾ÖÁØ´Ù.
			m_listDirection.empty();

			/*
			// ´Ù ÁÖ¼®Ã³¸®ÇÏ¸é ¿ØÁö µÉ°Å°°´Ù... - -;  2001.8.1		
			m_DestX		= SECTORPOSITION_NULL;
			m_DestY		= SECTORPOSITION_NULL;
			m_NextDestX	= SECTORPOSITION_NULL;
			m_NextDestY	= SECTORPOSITION_NULL;

			TraceNULL();	

			// 2001.7.27 Ãß°¡
			UnSetRepeatAction();	
			*/
		}

		// 2001.8.22
		if (m_Action==m_MoveAction)
		{
			// ÀÌµ¿ ÁßÀÌ¸é ±×³É Á¤Áö
			/*
			SetStop();

			m_DestX		= SECTORPOSITION_NULL;
			m_DestY		= SECTORPOSITION_NULL;
			m_NextDestX	= SECTORPOSITION_NULL;
			m_NextDestY	= SECTORPOSITION_NULL;

			TraceNULL();	

			UnSetRepeatAction();			
			*/
			// ±æÃ£±â Á¦°Å
			m_listDirection.clear(); 

			// ÃßÀû ÁßÁö	
			//m_fTrace		= FLAG_TRACE_NULL;
			//m_fNextTrace	= FLAG_TRACE_NULL;

			// Action ÁßÁö
			//m_Action		= (m_MoveAction==ACTION_SLAYER_MOTOR_MOVE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND;
			//m_ActionCount	= m_ActionCountMax; 	
			m_MoveCount		= m_MoveCountMax-1; 
			m_NextMoveCount = m_MoveCount;

			// ´ÙÀ½ µ¿ÀÛµµ ¾ø¾Ú
			m_bNextAction = false;
			m_NextX = SECTORPOSITION_NULL;
			m_NextY = SECTORPOSITION_NULL;
			
			//m_nNextUsedActionInfo = ACTIONINFO_NULL;
			//m_nUsedActionInfo = ACTIONINFO_NULL;

			
			// ¸ñÇ¥ Á¦°Å
			m_DestX			= SECTORPOSITION_NULL; 
			m_DestY			= SECTORPOSITION_NULL; 
			m_NextDestX		= SECTORPOSITION_NULL; 
			m_NextDestY		= SECTORPOSITION_NULL; 
		}
		// 2001.8.6
		// ÀÓ½Ã ´ëÃ¥.. - ¿òÁ÷ÀÌ´Ù°¡ ºÎµúÇô¼Å Æ¨±â´Â °æ¿ì..
		else if (m_ActionCount>=m_ActionCountMax)
		{
			if (m_Action==m_MoveAction)
			{
				/*
				SetStop();

				m_DestX		= SECTORPOSITION_NULL;
				m_DestY		= SECTORPOSITION_NULL;
				m_NextDestX	= SECTORPOSITION_NULL;
				m_NextDestY	= SECTORPOSITION_NULL;

				TraceNULL();	

				UnSetRepeatAction();
				*/
				m_sX = 0;//-m_cX;
				m_sY = 0;//-m_cY;

				m_MoveCount = m_MoveCountMax - 1;
				m_NextMoveCount = m_MoveCount;
			}
		}

		//m_SendMove = 0;
		// ¿©±â¼­ ¹º°¡ SX, SY°ªÀÌ ÀÌ»óÇÏ°Ô µÇ´Âµ¥~~!!!!!!!!!!!!!!!!
	}
	/*
	// ÇÑÄ­ Â÷ÀÌ·Î ´ëÃ¼·Î ¸·Çô¹ö¸®±â ¶§¹®¿¡
	// º°·Î ÀÇ¹Ì°¡ ¾ø¾î¼­ ÁÖ¼®Ã³¸®ÇØµ×´Ù.
	// ±×¸®°í.. Å×½ºÆ®µµ ÇÑ¹øµµ ¸øÇØº» ÄÚµåÀÌ´Ù. - -;
	else
	{
		DEBUG_ADD_FORMAT("[MoveNo] Find New Way. num=%d", m_listDirection.size());

		//----------------------------------------------------	
		//
		// (m_ServerX, m_ServerY)¿¡¼­ 
		//	(m_X, m_Y)±îÁö ¹æÇâ´ë·Î ¿òÁ÷ÀÎ´Ù.
		//
		//----------------------------------------------------
		int x = m_ServerX;
		int y = m_ServerY;

		while (!m_listDirection.empty())
		{
			//----------------------------------------------------
			// m_listDirection¿¡¼­ ¹æÇâÀ» ÇÏ³ª ÀÐ¾î¿Â´Ù.
			//----------------------------------------------------
			int direction = m_listDirection.front();
			m_listDirection.pop_front();	
		
			switch (direction)
			{
				case DIRECTION_LEFTDOWN		: x--;	y++;	break;
				case DIRECTION_RIGHTUP		: x++;	y--;	break;
				case DIRECTION_LEFTUP		: x--;	y--;	break;
				case DIRECTION_RIGHTDOWN	: x++;	y++;	break;
				case DIRECTION_LEFT			: x--;			break;
				case DIRECTION_DOWN			:		y++;	break;
				case DIRECTION_UP			:		y--;	break;
				case DIRECTION_RIGHT		: x++;			break;
			}

			//----------------------------------------------------
			// Server¿¡¼­ °ËÁõÀ» ¹Þ¾Æ¾ß ÇÑ´Ù.
			//----------------------------------------------------
			#ifdef	CONNECT_SERVER
				CGMove _CGMove;
				_CGMove.setX( x );
				_CGMove.setY( y );
				_CGMove.setDir( direction );
				g_pSocket->sendPacket( &_CGMove );
				
				// Server·Î º¸³½ ¹æÇâÀ» ±â¾ïÇØµÐ´Ù.
				m_listSendDirection.push_back( direction );
				m_SendMove++;				
			#endif
		}
	}
	*/
}


//----------------------------------------------------------------------
// Packet SpecialAction Result
//----------------------------------------------------------------------
// this Creature´Â Damage¸¦ ¹Þ¾ÒÀ¸¹Ç·Î
// ÀÌµ¿ ÁßÀÌ¸é ¹æ±Ý ÀüÀÇ Tile·Î Back½ÃÄÑ¾ß ÇÑ´Ù.
//
// 0. ¸ðµç µ¿ÀÛÀ» ÁßÁöÇÏ°í..
// 1. Action --> Damage¹Þ´Â µ¿ÀÛ
// 2. Damage Ç¥½Ã
//----------------------------------------------------------------------
void		
MPlayer::PacketSpecialActionResult(TYPE_ACTIONINFO nResultActionInfo, TYPE_OBJECTID id, TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, BYTE temp)
{
	//--------------------------------------------------
	// Á×¾úÀ¸¸é return
	//--------------------------------------------------
	if (!m_bAlive)
		return;

	//----------------------------------------------------------------------
	// °á°ú actionÀÌ ¾Æ´Ñ °æ¿ì..  - ÀÇ¹Ì°¡ ÀÖ³ª?? Èì..
	//----------------------------------------------------------------------
	if (nResultActionInfo < (*g_pActionInfoTable).GetMinResultActionInfo()
		// fast moveÁß¿¡´Â actionÀ» º¸¿©ÁÖÁö ¾Ê´Â´Ù... // 2001.8.10
		// ÇÏÁö¸¸ º¸¿©ÁÙ ÇÊ¿ä°¡ ÀÖ´Â °Íµµ ÀÖ´Ù --;;
		|| (m_bFastMove &&
			(nResultActionInfo != RESULT_SKILL_BOMB_CRASH_WALK	&&
			 nResultActionInfo != RESULT_SKILL_BLOODS_SYMPOSION_ATTACK ))
		|| m_bKnockBack)
	{
		return;
	}

	//----------------------------------------------------------------------
	// ¹æ¾î¸· È¿°ú.. - -;
	//----------------------------------------------------------------------
	int actionInfoAction = GetActionInfoAction(nResultActionInfo);
	
	if (actionInfoAction==ACTION_DAMAGED)
	{
		//if (HasEffectStatus(EFFECTSTATUS_AURA_PRISM))
		{
		//	nResultActionInfo = REFLECT_AURA_PRISM;
		//	actionInfoAction = GetActionInfoAction(nResultActionInfo);
		}
		//else 
			if (HasEffectStatus(EFFECTSTATUS_AURA_SHIELD))
		{
			nResultActionInfo = REFLECT_AURA_PRISM;//REFLECT_AURA_SHIELD;
			actionInfoAction = GetActionInfoAction(nResultActionInfo);
		}

		//-------------------------------------------------
		// Force Feel
		//-------------------------------------------------
//		if (gpC_Imm!=NULL && gpC_Imm->IsDevice())
//		{
//			gpC_Imm->ForceAction( CImm::FORCE_ACTION_DAMAGED );
//		}
	}

	//-------------------------------------------------
	// ¹Ýº¹ action ¶§¹®¿¡ ±â¾ïÇØµÐ´Ù.
	//-------------------------------------------------
	int oldTraceID = m_TraceID;

	//----------------------------------------------------------------------
	// ±âÁ¸¿¡ »ç¿ëÇÒ·Á´Â °ÍÀÌ ÀÖ¾ú´Ù¸é...
	//----------------------------------------------------------------------
	///*
	m_nNextUsedActionInfo = ACTIONINFO_NULL;
	TraceNextNULL();

	if (m_nUsedActionInfo!=ACTIONINFO_NULL)
	{
		BOOL	bStartActionInfo = m_nUsedActionInfo < g_pActionInfoTable->GetMinResultActionInfo();
		BOOL	bSentPacket		= (m_ActionCount>0);
		int		StartFrame		= (*g_pActionInfoTable)[m_nUsedActionInfo].GetStartFrame( m_WeaponSpeed );

		// Effect°¡ ½ÃÀÛµÇ´Â °æ¿ì´Â..
		// (1) StartFrameÀÎ °æ¿ì
		// (2) ¸¶Áö¸· ActionFrameÀÎ °æ¿ì
		BOOL	bStartedEffect = m_ActionCount>=StartFrame;

		//--------------------------------------------------
		// ÀÌ¹Ì effect°¡ ºÙÀº°Ô ¾Æ´Ï¶ó¸é ºÙÀÎ´Ù.
		//--------------------------------------------------		
		if (!bStartActionInfo						// °á°ú ÆÐÅ¶ÀÌ°Å³ª..
			|| (bSentPacket && !bStartedEffect))	// ÆÐÅ¶À» º¸³Â´Âµ¥ Effect°¡ ¾È ºÙÀº °æ¿ì..
		{
			TYPE_ACTIONINFO oldUsedActionInfo = m_nUsedActionInfo;

			m_nUsedActionInfo = ACTIONINFO_NULL;

			AffectUsedActionInfo(oldUsedActionInfo);
			
			// AffectUsedActionInfo¿¡ ÀÇÇØ¼­ »ý±ä °á°ú Ç¥Çö..
			if (m_nUsedActionInfo != ACTIONINFO_NULL)
			{
				AttachCastingEffect( m_nUsedActionInfo, TRUE );
				AffectUsedActionInfo(m_nUsedActionInfo);
			}
		}			
		//--------------------------------------------------		
		// ¹Ù·Î ÀÌÀüÀÇ Effect¸¦ Ãâ·Â ¾È ÇÏ´Â °æ¿ì.
		//--------------------------------------------------		
		else
		{
			m_nUsedActionInfo = ACTIONINFO_NULL;			
		}
		
		//-------------------------------------------------------------
		// castingEffect°¡ ¾ÆÁ÷ Ãâ·Â ¾ÈµÆÀ¸¸é Ãâ·Â½ÃÅ²´Ù.
		//-------------------------------------------------------------
		if (m_nUsedActionInfo!=ACTIONINFO_NULL
			&& GetActionInfoCastingStartFrame(m_nUsedActionInfo) >= m_ActionCount)
		{
			AttachCastingEffect( m_nUsedActionInfo, TRUE );		
		}
	
	}
	//*/

	// ÀÌµ¿À» ÁßÁöÇÑ´Ù.
	//m_sX = 0;	
	//m_sY = 0;
	// Back½ÃÅ²´Ù.	
	////SetStop();
	//PacketBack();		
	
	//m_ActionCount = m_ActionCountMax; 

	// ÃßÀû ÁßÁö
	//-------------------------------------------------
	// 2000.09.22 ¿¡ m_fTrace¸¸ NULL·Î ¹Ù²Þ..
	//-------------------------------------------------
	//TraceNULL();


	m_fTrace = FLAG_TRACE_NULL;


	SetTraceID( id );
	m_TraceX	= sX;
	m_TraceY	= sY;

	// ±æÃ£±â Çß´ø°É ¾ø¾ÖÁØ´Ù.
	m_listDirection.empty();
	m_DestX		= m_X;//SECTORPOSITION_NULL;
	m_DestY		= m_Y;//SECTORPOSITION_NULL;
	m_NextDestX	= SECTORPOSITION_NULL;
	m_NextDestY	= SECTORPOSITION_NULL;

	// nResultActionInfo¿¡ ÇØ´çÇÏ´Â ActionInfo¸¦ Ã£¾Æ¾ß ÇÑ´Ù.
	// ¿ø·¡ActionInfo + MIN_RESULT_ACTIONINFO¸¦ ÇÏ¸é µÈ´Ù.
	//	m_nUsedActionInfo	= nResultActionInfo;	// + (*g_pActionInfoTable).GetMinResultActionInfo()
	//SetAction( (*g_pActionInfoTable)[m_nUsedActionInfo].GetAction() );

	// ÀÌ·¸°Ô ÇØ¹ö¸®¸é..
	// ÇöÀç ÁøÇàÇÏ´Â Tile±îÁö °¡¼­.. °ø°ÝÀ» ¹Þ±â ¶§¹®¿¡
	// ÇÑ Ä­ ¶³¾îÁ®¼­ °ø°Ý¹Þ´Â °æ¿ì°¡ ¹ß»ýÇÑ´Ù.
	// ¾î¶»°Ô ÇØ¾ßÇÒ±î??
	//m_NextAction = (*g_pActionInfoTable)[m_nUsedActionInfo].GetAction();

	//SetNextAction( GetActionInfoAction(m_nUsedActionInfo) );
	//m_bNextAction = true;

	if(!m_bFastMove)
	{

		// 2001.05.21 Ãß°¡
		// ±â¼ú µ¿ÀÛ¿¡¼­ ACTION_STAND´Â º¸¿©ÁÖÁö ¾Ê´Â´Ù.
		if (actionInfoAction!=ACTION_STAND)
		{
			SetAction( actionInfoAction );
		}

		if( temp != 0)
			SetActionGrade( temp );

	}

	// ¹Ù·Î Àû¿ë
	//-------------------------------------------------------------
	//
	// Casting effect
	//
	//-------------------------------------------------------------
	//-------------------------------------------------------------
	// Casting effect
	//-------------------------------------------------------------
	AttachCastingEffect( nResultActionInfo, TRUE );		
	AffectUsedActionInfo( nResultActionInfo );


	//-------------------------------------------------
	// ¹Ýº¹ action ¶§¹®¿¡...
	//-------------------------------------------------
	SetTraceID( oldTraceID );


	//--------------------------------------------------------
	// ±â¼úµ¿ÀÛ ¿¡ ¸Â´Â »çµÐµå (³²ÀÚ / ¿©ÀÚ ±¸º° )¸¦ Ãâ·ÂÇØÁØ´Ù. 
	//--------------------------------------------------------
	// 2005.08.12 Sjheon ½ÃÀü½Ã ³²ÀÚ / ¿©ÀÚ / ¾Æ¿ì½ºÅÍÁî ¿¡ µû¸® ½ÃÀü »ç¿îµå °¡  ´Þ¶óÁø´Ù.
	int soundID = 0 ; 
	if(IsOusters())
	{
		soundID = (*g_pActionInfoTable)[nResultActionInfo].GetMaleSoundID() ; 
	}
	else 
	{	
		if(IsMale())
			soundID = (*g_pActionInfoTable)[nResultActionInfo].GetMaleSoundID() ; 
		else
			soundID = (*g_pActionInfoTable)[nResultActionInfo].GetFemaleSoundID() ; 
	}

	if(soundID != SOUNDID_NULL)
		PlaySound( soundID ,	false, sX, sY	);
	//--------------------------------------------------------

	// ÀÚ½Å¿¡°Ô »ç¿ë?..
	//-------------------------------------------------
	// 2000.09.22 ¿¡ ÁÖ¼® Ã³¸®.. -_-;
	//-------------------------------------------------
	//m_TraceID = m_ID;

	// Test Code
	/*
	if (rand()%2)
		SetChatString( "Shit!" );
	else
		SetChatString( "Oops!" );
	*/
}

//----------------------------------------------------------------------
// Packet Add ActionResult
//----------------------------------------------------------------------
// Player°¡ »ç¿ëÇÑ Action°ú °ü·ÃµÈ °á°ú¸¦ Server·Î ºÎÅÍ ¹Þ¾Æ¼­
// ¿ÜºÎ¿¡¼­ ActionResult·Î »ý¼ºÇÑ °ÍÀ» 
// PlayerÀÇ ActionResultList¿¡ Ãß°¡ÇÑ´Ù.
//
// Áï, InstanceID°¡ idÀÎ MEffectTargetÀÇ Result¸¦ ¼³Á¤ÇÑ´Ù.
//----------------------------------------------------------------------
// return°ªÀÌ 
//     trueÀÌ¸é Ãß°¡µÇ¾ú´Ù´Â ÀÇ¹Ì
//     falseÀÌ¸é °ü·ÃµÈ EffectTargetÀÌ ¾øÀ¸¹Ç·Î 
//               °á°ú¸¦ ¹Ù·Î ½ÇÇàÇØÁà¾ß ÇÑ´Ù.
//----------------------------------------------------------------------
bool		
MPlayer::PacketAddActionResult(WORD effectID, MActionResult* pActionResult)
{
	/*
	MActionResult* pResult = new MActionResult;
	pResult->Add( new MActionResultNodeActionInfo( m_ID, m_TraceID, m_nUsedActionInfo ) );
		
	pEffectTarget->SetResult( pResult );
	*/
	
	// Ã³¸®ÇÒ °á°ú°¡ ¾ø´Â °æ¿ì
	if (pActionResult==NULL)
	{
		return false;
	}


	if (m_nUsedActionInfo < g_pActionInfoTable->GetMinResultActionInfo())
	{
		//------------------------------------------------------------
		// ÁøÇàÁßÀÎ EffectÀÇ EffectTargetÀÌ ÀÖÀ» °æ¿ì¿¡
		//------------------------------------------------------------
		if (m_listEffectTarget.size()!=0)
		{
			// ¸¶Áö¸· EffectTargetÀÇ °á°ú¸¦ È®ÀÎÇØº»´Ù..
			MEffectTarget* pEffectTarget = m_listEffectTarget.back();

			// ÀÌ¹Ì °á°ú°¡ ÀÖ´Â °æ¿ì¶ó¸é..
			MActionResult* pResult = pEffectTarget->GetResult();

			if (pResult!=NULL)
			{
				pEffectTarget->SetResultNULL();
			}

			// »õ·Î¿î °á°ú¸¦ ´ëÀÔÇØÁØ´Ù.
			pEffectTarget->SetResult( pActionResult );

			if (pResult!=NULL)
			{
				// ±âÁ¸ÀÇ °á°ú¸¦ ½ÇÇà½ÃÄÑ ¹ö¸°´Ù.
				pResult->Execute();

				delete pResult;
			}			

			return true;

			/*
			EFFECTTARGET_LIST::iterator iEffectTarget = m_listEffectTarget.begin();

			while (iEffectTarget != m_listEffectTarget.end())
			{
				//------------------------------------------------------------
				// id°¡ °°Àº °æ¿ì
				//------------------------------------------------------------
				// EffectID¸¦ Ã¼Å©ÇØ¾ßÇÏÁö¸¸,
				// ÇöÀç(!)´Â .. ±â¼ú ÇÏ³ª ¾²°í ÇÏ³ª °ËÁõ¹Þ±â ¶§¹®¿¡
				// EffectID¸¦ Ã¼Å© ¾ÈÇØµµ µÈ´Ù.
				//------------------------------------------------------------
				// ¹«Á¶°Ç ÀÌ¹ø Çàµ¿¿¡ ¸ÂÃç¼­ °á°ú¸¦ Ç¥ÇöÇØÁÙ ¿¹Á¤ÀÌ¹Ç·Î
				// °á°ú¸¦ ³Ö¾îµÐ´Ù.
				//if ((*iEffectTarget)->GetEffectID() == effectID)
				{
					// EffectTargetÀÇ °á°ú·Î Ãß°¡½ÃÅ²´Ù.
					// ÀÌ¹Ì °á°ú°¡ ÀÖ´Â °æ¿ì..(?)
					// ±× ÀüÀÇ °á°ú¸¦ ½ÇÇà½ÃÄÑ¹ö¸°´Ù.
					if ((*iEffectTarget)->IsExistResult())
					{
						(*iEffectTarget)->GetResult()->Execute();
						//break; // ¿ø·¡´Â.. - -;
					}

					// »õ·Î¿î°É setÇÏ¸é ±âÁ¸¿¡°Ç deleteµÈ´Ù.
					(*iEffectTarget)->SetResult( pActionResult );

					// Á¦´ë·Î ¼³Á¤ µÈ °æ¿ì
					return true;
				}

				iEffectTarget++;
			}
			*/
		}
	}

	//------------------------------------------------------------
	// ÁøÇàÁßÀÎ EffectÀÇ EffectTargetÀÌ ¾øÀ» °æ¿ì¿¡
	//------------------------------------------------------------			
	// °á°ú¸¦ ¹Ù·Î ½ÇÇàÇÏ°í..
	// ¸Þ¸ð¸®¿¡¼­ Áö¿î´Ù.
	// (!) °á°ú ½ÇÇàÀ» À§ÇÑ Manager class°¡ ÇÊ¿äÇÏ´Ù.
	pActionResult->Execute();

	delete pActionResult;
	
	return false;	
}

//----------------------------------------------------------------------
// On Attacking
//----------------------------------------------------------------------
// ¹Ýº¹ Çàµ¿ Áß¿¡ °ø°ÝÇÏ´Â °æ¿ì..
//----------------------------------------------------------------------
bool			
MPlayer::OnAttacking() const				
{ 
	return m_bRepeatAction && m_bTraceCreatureToForceAttack; 
}

//----------------------------------------------------------------------
// Basic Action To Creature
//----------------------------------------------------------------------
// ÃßÀûÀ» ¿Ï·áÇÑ °æ¿ì.. Creature¿¡°Ô ±âº»actionÀ» ÃëÇÑ´Ù.
//----------------------------------------------------------------------
void
MPlayer::BasicActionToCreature()
{
	DEBUG_ADD_FORMAT("[NPC-BCA] ENTER traceID=%d playerCT=%d", (int)m_TraceID, (int)m_CreatureType);
	if (IsInDarkness())
	{
		DEBUG_ADD("[NPC-BCA] return: IsInDarkness");
		return;
	}

	MCreature* pCreature = g_pZone->GetCreature( m_TraceID );

	if (pCreature==NULL)// || pCreature->IsDead())
	{
		DEBUG_ADD_FORMAT("[NPC-BCA] return: pCreature NULL for traceID=%d", (int)m_TraceID);
		return;
	}
	if(pCreature->CurPernalShop() == 1) // by csm 12/4
	{
		DEBUG_ADD("[NPC-BCA] return: PernalShop=1");
		return;
	}

	//---------------------------------------------------------------
	// ´ë»óÀÌ NPCÀÎ °æ¿ì..
	//---------------------------------------------------------------
	if (pCreature->IsNPC())
	{
		DEBUG_ADD_FORMAT("[NPC-BCA] target IS NPC type=%d", (int)pCreature->GetCreatureType());
		// 2026-05-12 ROOT CAUSE FIX: AC=0 newbie characters (Slayer/Vampire/Ousters)
		// CreatureType = OPERATOR olarak baslar. Eski check `IsPlayerOnlySprite()`
		// data-driven, OPERATOR sprite flag'ine bagimli. Newbie character'lar
		// IsPlayerOnly=false donerse NPC click sessizce kayboluyordu — CGNPCTalk
		// gonderilmiyordu.
		//
		// Eski yorum: "BAT veya WOLF olmayan durumda transaction possible"
		// — niyet: transformed formlari (Vampire Bat/Wolf, Ousters elemental,
		//   Slayer transform) engelle, regular forma izin ver.
		//
		// Yeni cozum: data-driven flag yerine explicit transformed-form list.
		// Tum normal formlar (MALE/FEMALE/OPERATOR + 3 race) gecer; sadece
		// bilinen transform listesi engellenir.
		bool _isTransformedForm = (m_CreatureType == CREATURETYPE_BAT
			|| m_CreatureType == CREATURETYPE_WOLF
			|| m_CreatureType == CREATURETYPE_VAMPIRE_GHOST
#if __CONTENTS(__FAST_TRANSFORTER)
			|| m_CreatureType == CREATURETYPE_FLITTERMOUSE
			|| m_CreatureType == CREATURETYPE_NEDE
			|| m_CreatureType == CREATURETYPE_KLTL
#endif //__FAST_TRANSFORTER
			|| m_CreatureType == CREATURETYPE_SHAPE_OF_DEMON);
		bool _canTalkToNPC = !_isTransformedForm;
		DEBUG_ADD_FORMAT("[NPC-BCA] canTalkToNPC=%d (playerCT=%d transformed=%d)",
			(int)_canTalkToNPC, (int)m_CreatureType, (int)_isTransformedForm);
		if (_canTalkToNPC)
		{
			if(pCreature->GetCreatureType() == 659)	// ´ëÁöÁ¤·ÉÀÇ »ÔÀÌ¸é UI¶ç¿î´Ù
				UI_RunHorn(g_pZone->GetID());
			else
			{
#if __CONTENTS(__MENEGROTH)
				if(pCreature->HasEffectStatus(EFFECTSTATUS_MENEGROTH_CABRACAM_ALTAR))
				{
					int creatureType = pCreature->GetCreatureType();
					if (creatureType > 948 && creatureType < 1021)
					{
					}
				}
				else
#endif //__MENEGROTH
				if (IsWaitVerifyNULL())
				{
					DEBUG_ADD("[NPC-BCA] SENDING CGNPCTalk");
					// NPC Ã³¸® packetÀ» º¸³½´Ù.
					CGNPCTalk _CGNPCTalk;
					_CGNPCTalk.setObjectID( m_TraceID );

					g_pSocket->sendPacket( &_CGNPCTalk );

					SetWaitVerify( WAIT_VERIFY_NPC_ASK );
				}
				else
				{
					DEBUG_ADD_FORMAT("[NPC-BCA] WaitVerify blocks send (wv=%d)", (int)GetWaitVerify());
				}
			}
		}
		else
		{
			DEBUG_ADD_FORMAT("[NPC-BCA] transformed-form skip CGNPCTalk (playerCT=%d)",
				(int)m_CreatureType);
		}

		// ÇÊ¿äÇÏ·Á³ª.. - -;
		m_bTraceCreatureToForceAttack = false;
		m_bKeepTraceCreature = false;

		// Á¤ÁöÇÑ´Ù.
		SetNextAction(ACTION_STAND);
		m_nUsedActionInfo = ACTIONINFO_NULL;
		m_nNextUsedActionInfo = ACTIONINFO_NULL;
		m_fNextTrace = FLAG_TRACE_NULL;
		UnSetRepeatAction();
	}
	//---------------------------------------------------------------
	// RequestÇÒ·Á´Â °æ¿ì
	//---------------------------------------------------------------
	else if (IsRequestMode())
	{
		switch (m_RequestMode)
		{
			//---------------------------------------------------------------
			// Trade ÇÒ·Á´Â °æ¿ì
			//---------------------------------------------------------------
			case REQUEST_TRADE :
			{
				if (IsWaitVerifyNULL() 
					&& g_pTempInformation->GetMode() == TempInformation::MODE_NULL
					&& (IsSlayer() && pCreature->IsSlayer() || 
					IsVampire() && pCreature->IsVampire() || 
					IsOusters() && pCreature->IsOusters())
					)
				{
						// ±³È¯ ½ÅÃ»
						CGTradePrepare _CGTradePrepare;
						_CGTradePrepare.setTargetObjectID( m_TraceID );
						_CGTradePrepare.setCode( CG_TRADE_PREPARE_CODE_REQUEST );

						g_pSocket->sendPacket( &_CGTradePrepare );

					SetWaitVerify( WAIT_VERIFY_TRADE );
					
					// temp informationµµ ¼³Á¤.. (À½..¾à°£ ÇãÁ¢ÇÏ´ç.- -;)
					g_pTempInformation->SetMode(TempInformation::MODE_TRADE_REQUEST);
					g_pTempInformation->Value1 = m_TraceID;

					//-----------------------------------------------------
					// ±³È¯ Ãë¼ÒÇÒ±î?
					//-----------------------------------------------------
					UI_RunExchangeCancel( pCreature->GetName() );
				}				
			}
			break;

			//---------------------------------------------------------------
			// Party ÇÒ·Á´Â °æ¿ì
			//---------------------------------------------------------------
			case REQUEST_PARTY :
			{
				if (IsWaitVerifyNULL() 
					&& g_pTempInformation->GetMode() == TempInformation::MODE_NULL
					&& g_pParty!=NULL
					// ³» ÆÄÆ¼¿øÀÌ ¾Æ´Ñ °æ¿ì¿¡¸¸ ½ÅÃ» °¡´ÉÇÏ´Ù.
					&& !g_pParty->HasMember( pCreature->GetName() )
					&& (IsSlayer() && pCreature->IsSlayer() || IsVampire() && pCreature->IsVampire() || IsOusters() && pCreature->IsOusters())
					&& g_pSystemAvailableManager->IsAvailablePartySystem()
					)
				{
						// ÆÄÆ¼ ½ÅÃ»
						CGPartyInvite _CGPartyInvite;
						_CGPartyInvite.setTargetObjectID( m_TraceID );
						_CGPartyInvite.setCode( CG_PARTY_INVITE_REQUEST );

						g_pSocket->sendPacket( &_CGPartyInvite );

					SetWaitVerify( WAIT_VERIFY_PARTY );

					// temp informationµµ ¼³Á¤.. (À½..¾à°£ ÇãÁ¢ÇÏ´ç.- -;)
					g_pTempInformation->SetMode(TempInformation::MODE_PARTY_REQUEST);
					g_pTempInformation->Value1 = m_TraceID;

					//-----------------------------------------------------
					// ±³È¯ Ãë¼ÒÇÒ±î?
					//-----------------------------------------------------
					UI_RunPartyCancel( pCreature->GetName() );
				}				
			}
			break;

			//---------------------------------------------------------------
			// OtherInfo º¼·Á´Â °æ¿ì
			//---------------------------------------------------------------
			case REQUEST_INFO:
			{
//				MFakeCreature *pFakeCreature = (MFakeCreature *)g_pZone->GetFakeCreature(m_TraceID);
//				if(pFakeCreature != NULL && pFakeCreature->GetOwnerID() != OBJECTID_NULL)	// ÆêÀÌ´ç
//				{
//					UI_RunPetInfo(pFakeCreature->GetPetItem());
//				}
//				else
				if (IsWaitVerifyNULL() 
					&& g_pTempInformation->GetMode() == TempInformation::MODE_NULL
					&& (IsSlayer() && pCreature->IsSlayer() || 
					IsVampire() && pCreature->IsVampire() || 
					IsOusters() && pCreature->IsOusters() ||
					GetCreatureType() == CREATURETYPE_SLAYER_OPERATOR ||
					GetCreatureType() == CREATURETYPE_VAMPIRE_OPERATOR ||
					GetCreatureType() == CREATURETYPE_OUSTERS_OPERATOR ||
					GetCompetence() == 0 )
					)
				{
						const char* str = gC_vs_ui.GetInputString();
						if(strlen(str)>0)
						{
							if(str[0] == '*')
							{
								gC_vs_ui.AddInputString(pCreature->GetName());
								break;

							}
						}
						// Á¤º¸ ½ÅÃ»
						CGRequestInfo _CGRequestInfo;
						_CGRequestInfo.setValue( m_TraceID );
						_CGRequestInfo.setCode( CGRequestInfo::REQUEST_CHARACTER_INFO );

						g_pSocket->sendPacket( &_CGRequestInfo );

						MCreature *pCreature = g_pZone->GetCreature(m_TraceID);
						
						if(pCreature != NULL)
						{
							UI_RunOtherInfo(pCreature);

							if (g_pProfileManager!=NULL 
								&& g_pRequestUserManager!=NULL
								&& g_pRequestClientPlayerManager!=NULL)
								//&& !g_pProfileManager->HasProfile(pName)
								//&& !g_pProfileManager->HasProfileNULL(pName)
								//&& !g_pRequestUserManager->HasRequestingUser(pName)
								//&& !g_pRequestClientPlayerManager->HasConnection(pName)
								//&& !g_pRequestClientPlayerManager->HasTryingConnection(pName))
							{
								g_pProfileManager->RequestProfile(pCreature->GetName());
							}
						}


				}
			}
			break;
	
		}

		// ÇÊ¿äÇÏ·Á³ª.. - -;
		m_bTraceCreatureToForceAttack = false;
		m_bKeepTraceCreature = false;

		// Á¤ÁöÇÑ´Ù.
		SetNextAction(ACTION_STAND);
		m_nUsedActionInfo = ACTIONINFO_NULL;
		m_nNextUsedActionInfo = ACTIONINFO_NULL;
		m_fNextTrace = FLAG_TRACE_NULL;
		UnSetRepeatAction();

		UnSetRequestMode();
	}	
	//---------------------------------------------------------------
	// ¾Æ´Ï¸é.. °ø°ÝÀÌ°Å³ª ¸ØÃã..
	//---------------------------------------------------------------
	else
	{
		//---------------------------------------------------------------
		// °­Á¦ attackÀÌ°Å³ª..
		// ÃßÀû´ë»óÀÌ µÇ´Â Ä³¸¯ÅÍÀÇ Á¾Á·À» º¸°í °ø°ÝÇÒÁö¸¦ °áÁ¤ÇÑ´Ù.
		//---------------------------------------------------------------
		bool bOusters = IsOusters();
		MItem *pOustersItem = NULL;
		if( bOusters) 
			pOustersItem = g_pOustersGear->GetItem(MOustersGear::GEAR_OUSTERS_RIGHTHAND);

		if (m_bTraceCreatureToForceAttack
			&& (bOusters && pOustersItem != NULL && pOustersItem->GetItemClass() == ITEM_CLASS_OUSTERS_CHAKRAM
				|| !bOusters)
			)
		{
			// FAZ 4 (2026-05-11 it.2) Bug 11: BasicActionToCreature da mermi guard
			// m_nBasicActionInfo gun-type ise + magazine NULL/0 ise SetNextAction(GunAttack) YAPMA
			// Bu yol: ForceAttack akisinda (force-attack mode + sol click hold) animasyonu
			// dogrudan baslatiyordu — magazine kontrolu yoktu. CT marker ile teyit.
			bool bug11_bca_skipFire = false;
			if (m_nBasicActionInfo >= 0 && m_nBasicActionInfo < (int)g_pActionInfoTable->GetSize() &&
				(*g_pActionInfoTable)[m_nBasicActionInfo].IsWeaponTypeGunAny())
			{
				if (g_pCurrentMagazine == NULL ||
					(g_pCurrentMagazine != NULL && g_pCurrentMagazine->GetNumber() == 0))
				{
					bug11_bca_skipFire = true;
					CT_Done("BCA: Bug11 skip gun fire (mag empty) basicAI=%d", (int)m_nBasicActionInfo);
					PlaySound(SOUND_ITEM_NO_MAGAZINE);
					m_DelayTime = g_CurrentTime + GetActionInfoDelay(m_nBasicActionInfo);
					m_fNextTrace = FLAG_TRACE_NULL;
					m_fTraceBuffer = FLAG_TRACE_NULL;
					m_nUsedActionInfo = ACTIONINFO_NULL;
					m_nNextUsedActionInfo = ACTIONINFO_NULL;
					UnSetRepeatAction();
					SetNextAction(ACTION_STAND);
				}
			}

			if (!bug11_bca_skipFire)
			{
				SetNextAction(GetActionInfoAction(m_nBasicActionInfo));
			}

			//--------------------
			// message
			//--------------------
			DEBUG_ADD_FORMAT("Basic Action : %d", m_TraceID);

			//
			//m_bTraceCreatureToForceAttack = false;
		}
		else
		{
			DEBUG_ADD_FORMAT("Basic Action : %d - But don't attack", m_TraceID);

			// °ø°Ý ¾ÈÇÑ´Ù.
			m_nUsedActionInfo = ACTIONINFO_NULL;

			//---------------------------------
			// °è¼Ó ÃßÀûÇØ¾ß ÇÏ¸é..
			//---------------------------------
			if (m_bKeepTraceCreature)
			{
				return;
			}
		}
	}
	/*
	#ifdef	CONNECT_SERVER
		CGAttackNormal _CGAttackNormal;
		_CGAttackNormal.setX( m_X );
		_CGAttackNormal.setY( m_Y );
		_CGAttackNormal.setDir( m_Direction );
		_CGAttackNormal.setObjectID( m_TraceID );
		g_pSocket->sendPacket( &_CGAttackNormal );

		#ifdef	OUTPUT_DEBUG
			DEBUG_ADD( _CGAttackNormal.toString().c_str() );							
		#endif
	#endif
	*/	
}
//void
//MPlayer::BasicActionToCreature()
//{
//	if (IsInDarkness())
//	{
//		return;
//	}
//
//	MCreature* pCreature = g_pZone->GetCreature( m_TraceID );
//
//	if (pCreature==NULL)// || pCreature->IsDead())
//	{
//		return;
//	}
//
//	//---------------------------------------------------------------
//	// ´ë»óÀÌ NPCÀÎ °æ¿ì..
//	//---------------------------------------------------------------
//	if (pCreature->IsNPC())
//	{
//		// ¹ÚÁã³ª ´Á´ë°¡ ¾Æ´Ñ °æ¿ì.. °Å·¡ °¡´ÉÇÏ´Ù.
//		if (//m_CreatureType!=CREATURETYPE_BAT
//			//&& m_CreatureType!=CREATURETYPE_WOLF)
//			(*g_pCreatureSpriteTable)[(*g_pCreatureTable)[m_CreatureType].SpriteType].IsPlayerOnlySprite())
//		{		
//			if(pCreature->GetCreatureType() == 659)	// ´ëÁöÁ¤·ÉÀÇ »ÔÀÌ¸é UI¶ç¿î´Ù
//				UI_RunHorn(g_pZone->GetID());
//			else
//				
//				if (IsWaitVerifyNULL())
//				{
//					// NPC Ã³¸® packetÀ» º¸³½´Ù.
//					CGNPCTalk _CGNPCTalk;
//					_CGNPCTalk.setObjectID( m_TraceID );
//
//					g_pSocket->sendPacket( &_CGNPCTalk );
//
//					SetWaitVerify( WAIT_VERIFY_NPC_ASK );
//				}
//		}
//
//		// ÇÊ¿äÇÏ·Á³ª.. - -;
//		m_bTraceCreatureToForceAttack = false;
//		m_bKeepTraceCreature = false;
//
//		// Á¤ÁöÇÑ´Ù.
//		SetNextAction(ACTION_STAND);
//		m_nUsedActionInfo = ACTIONINFO_NULL;
//		m_nNextUsedActionInfo = ACTIONINFO_NULL;
//		m_fNextTrace = FLAG_TRACE_NULL;
//		UnSetRepeatAction();
//	}
//	//---------------------------------------------------------------
//	// RequestÇÒ·Á´Â °æ¿ì
//	//---------------------------------------------------------------
//	else if (IsRequestMode())
//	{
//		switch (m_RequestMode)
//		{
//			//---------------------------------------------------------------
//			// Trade ÇÒ·Á´Â °æ¿ì
//			//---------------------------------------------------------------
//			case REQUEST_TRADE :
//			{
//				if (IsWaitVerifyNULL() 
//					&& g_pTempInformation->GetMode() == TempInformation::MODE_NULL
//					&& (IsSlayer() && pCreature->IsSlayer() || 
//					IsVampire() && pCreature->IsVampire() || 
//					IsOusters() && pCreature->IsOusters())
//					)
//				{
//						// ±³È¯ ½ÅÃ»
//						CGTradePrepare _CGTradePrepare;
//						_CGTradePrepare.setTargetObjectID( m_TraceID );
//						_CGTradePrepare.setCode( CG_TRADE_PREPARE_CODE_REQUEST );
//
//						g_pSocket->sendPacket( &_CGTradePrepare );
//
//					SetWaitVerify( WAIT_VERIFY_TRADE );
//					
//					// temp informationµµ ¼³Á¤.. (À½..¾à°£ ÇãÁ¢ÇÏ´ç.- -;)
//					g_pTempInformation->SetMode(TempInformation::MODE_TRADE_REQUEST);
//					g_pTempInformation->Value1 = m_TraceID;
//
//					//-----------------------------------------------------
//					// ±³È¯ Ãë¼ÒÇÒ±î?
//					//-----------------------------------------------------
//					UI_RunExchangeCancel( pCreature->GetName() );
//				}				
//			}
//			break;
//
//			//---------------------------------------------------------------
//			// Party ÇÒ·Á´Â °æ¿ì
//			//---------------------------------------------------------------
//			case REQUEST_PARTY :
//			{
//				if (IsWaitVerifyNULL() 
//					&& g_pTempInformation->GetMode() == TempInformation::MODE_NULL
//					&& g_pParty!=NULL
//					// ³» ÆÄÆ¼¿øÀÌ ¾Æ´Ñ °æ¿ì¿¡¸¸ ½ÅÃ» °¡´ÉÇÏ´Ù.
//					&& !g_pParty->HasMember( pCreature->GetName() )
//					&& (IsSlayer() && pCreature->IsSlayer() || IsVampire() && pCreature->IsVampire() || IsOusters() && pCreature->IsOusters())
//					&& g_pSystemAvailableManager->IsAvailablePartySystem()
//					)
//				{
//						// ÆÄÆ¼ ½ÅÃ»
//						CGPartyInvite _CGPartyInvite;
//						_CGPartyInvite.setTargetObjectID( m_TraceID );
//						_CGPartyInvite.setCode( CG_PARTY_INVITE_REQUEST );
//
//						g_pSocket->sendPacket( &_CGPartyInvite );
//
//					SetWaitVerify( WAIT_VERIFY_PARTY );
//
//					// temp informationµµ ¼³Á¤.. (À½..¾à°£ ÇãÁ¢ÇÏ´ç.- -;)
//					g_pTempInformation->SetMode(TempInformation::MODE_PARTY_REQUEST);
//					g_pTempInformation->Value1 = m_TraceID;
//
//					//-----------------------------------------------------
//					// ±³È¯ Ãë¼ÒÇÒ±î?
//					//-----------------------------------------------------
//					UI_RunPartyCancel( pCreature->GetName() );
//				}				
//			}
//			break;
//
//			//---------------------------------------------------------------
//			// OtherInfo º¼·Á´Â °æ¿ì
//			//---------------------------------------------------------------
//			case REQUEST_INFO:
//			{
////				MFakeCreature *pFakeCreature = (MFakeCreature *)g_pZone->GetFakeCreature(m_TraceID);
////				if(pFakeCreature != NULL && pFakeCreature->GetOwnerID() != OBJECTID_NULL)	// ÆêÀÌ´ç
////				{
////					UI_RunPetInfo(pFakeCreature->GetPetItem());
////				}
////				else
//				if (IsWaitVerifyNULL() 
//					&& g_pTempInformation->GetMode() == TempInformation::MODE_NULL
//					&& (IsSlayer() && pCreature->IsSlayer() || 
//					IsVampire() && pCreature->IsVampire() || 
//					IsOusters() && pCreature->IsOusters() ||
//					GetCreatureType() == CREATURETYPE_SLAYER_OPERATOR ||
//					GetCreatureType() == CREATURETYPE_VAMPIRE_OPERATOR ||
//					GetCreatureType() == CREATURETYPE_OUSTERS_OPERATOR ||
//					GetCompetence() == 0 )
//					)
//				{
//						const char* str = gC_vs_ui.GetInputString();
//						if(strlen(str)>0)
//						{
//							if(str[0] == '*')
//							{
//								gC_vs_ui.AddInputString(pCreature->GetName());
//								break;
//
//							}
//						}
//						// Á¤º¸ ½ÅÃ»
//						CGRequestInfo _CGRequestInfo;
//						_CGRequestInfo.setValue( m_TraceID );
//						_CGRequestInfo.setCode( CGRequestInfo::REQUEST_CHARACTER_INFO );
//
//						g_pSocket->sendPacket( &_CGRequestInfo );
//
//						MCreature *pCreature = g_pZone->GetCreature(m_TraceID);
//						
//						if(pCreature != NULL)
//						{
//							UI_RunOtherInfo(pCreature);
//
//							if (g_pProfileManager!=NULL 
//								&& g_pRequestUserManager!=NULL
//								&& g_pRequestClientPlayerManager!=NULL)
//								//&& !g_pProfileManager->HasProfile(pName)
//								//&& !g_pProfileManager->HasProfileNULL(pName)
//								//&& !g_pRequestUserManager->HasRequestingUser(pName)
//								//&& !g_pRequestClientPlayerManager->HasConnection(pName)
//								//&& !g_pRequestClientPlayerManager->HasTryingConnection(pName))
//							{
//								g_pProfileManager->RequestProfile(pCreature->GetName());
//							}
//						}
//
//
//				}
//			}
//			break;
//	
//		}
//
//		// ÇÊ¿äÇÏ·Á³ª.. - -;
//		m_bTraceCreatureToForceAttack = false;
//		m_bKeepTraceCreature = false;
//
//		// Á¤ÁöÇÑ´Ù.
//		SetNextAction(ACTION_STAND);
//		m_nUsedActionInfo = ACTIONINFO_NULL;
//		m_nNextUsedActionInfo = ACTIONINFO_NULL;
//		m_fNextTrace = FLAG_TRACE_NULL;
//		UnSetRepeatAction();
//
//		UnSetRequestMode();
//	}	
//	//---------------------------------------------------------------
//	// ¾Æ´Ï¸é.. °ø°ÝÀÌ°Å³ª ¸ØÃã..
//	//---------------------------------------------------------------
//	else
//	{
//		//---------------------------------------------------------------
//		// °­Á¦ attackÀÌ°Å³ª..
//		// ÃßÀû´ë»óÀÌ µÇ´Â Ä³¸¯ÅÍÀÇ Á¾Á·À» º¸°í °ø°ÝÇÒÁö¸¦ °áÁ¤ÇÑ´Ù.
//		//---------------------------------------------------------------
//		bool bOusters = IsOusters();
//		MItem *pOustersItem = NULL;
//		if( bOusters) 
//			pOustersItem = g_pOustersGear->GetItem(MOustersGear::GEAR_OUSTERS_RIGHTHAND);
//
//		if (m_bTraceCreatureToForceAttack 
//			&& (bOusters && pOustersItem != NULL && pOustersItem->GetItemClass() == ITEM_CLASS_OUSTERS_CHAKRAM
//				|| !bOusters)
//			)
//		{
//			SetNextAction(GetActionInfoAction(m_nBasicActionInfo));
//
//			//--------------------
//			// message
//			//--------------------
//			DEBUG_ADD_FORMAT("Basic Action : %d", m_TraceID);					
//			
//			// 
//			//m_bTraceCreatureToForceAttack = false;
//		}
//		else
//		{
//			DEBUG_ADD_FORMAT("Basic Action : %d - But don't attack", m_TraceID);					
//			
//			// °ø°Ý ¾ÈÇÑ´Ù.
//			m_nUsedActionInfo = ACTIONINFO_NULL;
//			
//			//---------------------------------
//			// °è¼Ó ÃßÀûÇØ¾ß ÇÏ¸é..
//			//---------------------------------
//			if (m_bKeepTraceCreature)
//			{
//				return;
//			}
//		}
//	}
//	/*
//	#ifdef	CONNECT_SERVER
//		CGAttackNormal _CGAttackNormal;
//		_CGAttackNormal.setX( m_X );
//		_CGAttackNormal.setY( m_Y );
//		_CGAttackNormal.setDir( m_Direction );
//		_CGAttackNormal.setObjectID( m_TraceID );
//		g_pSocket->sendPacket( &_CGAttackNormal );
//
//		#ifdef	OUTPUT_DEBUG
//			DEBUG_ADD( _CGAttackNormal.toString().c_str() );							
//		#endif
//	#endif
//	*/	
//}

//----------------------------------------------------------------------
// Pickup Item
//----------------------------------------------------------------------
// ItemÁÝ±â¸¦ ¼öÇàÇÑ´Ù.
//----------------------------------------------------------------------
void
MPlayer::PickupItem(MItem* pItem)
{
	//----------------------------------------------------
	// °ËÁõ ¹ÞÀ» ¾ÆÀÌÅÛÀÌ ¾ø°í..
	// ÇöÀç µé°í ÀÖ´Â ItemÀÌ ¾ø´Â °æ¿ì
	//----------------------------------------------------

	if( HasEffectStatusSummonSylph( dynamic_cast<MCreature*>(this) ) 
#if __CONTENTS(__FAST_TRANSFORTER)
		|| HasEffectStatusWingSylph(dynamic_cast<MCreature*>(this))
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
		|| HasEffectStatusUnicorn(dynamic_cast<MCreature*>(this))
		|| HasEffectStatusOusterNewWings1(dynamic_cast<MCreature*>(this))
		|| HasEffectStatusOusterNewWings2(dynamic_cast<MCreature*>(this))
#endif //__SECOND_TRANSFORTER
		)
		return;

	if (IsItemCheckBufferNULL() 
		&& gpC_mouse_pointer->GetPickUpItem()==NULL)
	{
		//----------------------------------------------------------------
		// À§Ä¡ °ËÁõÀÌ µÈ »óÅÂ
		//----------------------------------------------------------------
		//if (m_SendMove==0)
		{
			// [ TEST CODE ]
			// Item Á¤º¸¸¦ ÀÐ¾î¿Í¼­ ItemÁÝ±âÇÒ¶§ Sound¸¦ ³½´Ù.
			// ÀÌ ºÎºÐÀº ³ªÁß¿¡.. 
			// Server Packet¿¡¼­ È®½ÇÈ÷ "ÁÖ¿ü´Ù"¶ó°í ÇÒ ¶§
			// ÇÏ´Â °ÍÀÌ ÁÁÀ» °Í °°±âµµ ÇÏ´Ù.
			//MItem*	pItem = m_pZone->GetItem( m_TraceID );

			//g_Sound.Play( g_SoundTable[ g_ItemTable[pItem->GetItemType()].PickupSoundID ].pDSBuffer );
			//PlaySound( g_ItemTable[pItem->GetItemClass()][pItem->GetItemType()].TileSoundID ,
			//			false,
			//			m_X, m_Y);

			// item class ºñ±³ÇÏÁö ¸»°í..
			// item class¿¡´Ù°¡ virtual pickup()À» µÎ´Â°Ô ³´Áö ½ÍÀºµ¥.
			// ±ÍÂú¾î¶û.. -_-;;

			//----------------------------------------------------------------
			//
			//	 MotorcycleÀÎ °æ¿ì --> Å¸±â
			//
			//----------------------------------------------------------------
			if (pItem->GetItemClass()==ITEM_CLASS_MOTORCYCLE)
			{	
				if (IsSlayer()
					&& m_CreatureType!=CREATURETYPE_SLAYER_OPERATOR)
				{
					RideMotorcycle( (MMotorcycle*)pItem );
				}
			}	
			
			//----------------------------------------------------------------
			//
			// ½ÃÃ¼ÀÎ °æ¿ì --> Looting
			//
			//----------------------------------------------------------------
			else if (pItem->GetItemClass()==ITEM_CLASS_CORPSE)
			{				
				//----------------------------------------------------
				// Server¿¡ Á¢¼ÓÇØ ÀÖÀ» ¶§,
				//----------------------------------------------------
					MCreature* pCreature = ((MCorpse*)pItem)->GetCreature();

					if ( pCreature != NULL &&
						(
						pCreature->GetActionCount() >= pCreature->GetActionCountMax() || 
						pCreature->GetCreatureType() == 672 
						)
						)
					{	
						if (pItem->GetNumber()>0 || 
							pCreature->GetCreatureType() >= 371 && pCreature->GetCreatureType() <= 376 || 
							pCreature->GetCreatureType() >= 560 && pCreature->GetCreatureType() <= 563 || 
							pCreature->GetCreatureType() >= 526 && pCreature->GetCreatureType() <= 549 ||
							pCreature->GetCreatureType() == 670 ||			// ±ê´ë¸é
							pCreature->GetCreatureType() == 672	||			// ½ºÀ§ÆÛ º¸°ü´ëÀÌ¸é
							pCreature->GetCreatureType() == 673
							)
						{
							// ¹ÚÁã°¡ ¾Æ´Ñ °æ¿ì¿¡¸¸ º¸³½´Ù.
							// ¿ÀÅä¹ÙÀÌ¸¦ Å¸Áö ¾ÊÀº º¸Åë °ÉÀ½ÀÎ °æ¿ì¿¡¸¸
							// ¼®È­ ¾È°É·ÈÀ»¶§
							if (m_CreatureType!=CREATURETYPE_BAT
								&& m_CreatureType!=CREATURETYPE_VAMPIRE_GHOST
#if __CONTENTS(__FAST_TRANSFORTER)
								&& m_CreatureType != CREATURETYPE_FLITTERMOUSE
								&& m_CreatureType != CREATURETYPE_NEDE
								&& m_CreatureType != CREATURETYPE_KLTL
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
								&& m_CreatureType != CREATURETYPE_SHAPE_OF_DEMON
#endif //__SECOND_TRANSFORTER
								&& m_MoveDevice==MOVE_DEVICE_WALK
								&& !m_bEffectStatus[EFFECTSTATUS_CURSE_PARALYSIS])
							{
								CGDissectionCorpse _CGDissectionCorpse;
								_CGDissectionCorpse.setX( pItem->GetX() );
								_CGDissectionCorpse.setY( pItem->GetY() );
								_CGDissectionCorpse.setObjectID( pItem->GetID() );

								g_pSocket->sendPacket( &_CGDissectionCorpse );

								if(pItem->GetNumber() > 0)
									pItem->SetNumber( pItem->GetNumber()-1 );
							}
						}
						// itemÀÌ ¾ø´Â °æ¿ì¿£..
						else
						{
							// ´Á´ë¶ó¸é ½ÃÃ¼ ¸Ô±â
							// ¼º¹°ÀÌ ¾Æ´Ñ °æ¿ì¿¡¸¸, ¼öÈ£¼º´ÜÀÌ ¾Æ´Ñ °æ¿ì¿¡¸¸.
							if ( m_CreatureType==CREATURETYPE_WOLF && 
								!(
								pCreature->GetCreatureType() >= 371 && pCreature->GetCreatureType() <= 376 || 
								pCreature->GetCreatureType() == 670	||			// ±ê´ë¸é
								pCreature->GetCreatureType() == 672 ||			// ½ºÀ§ÆÛ º¸°ü´ëÀÌ¸é
								pCreature->GetCreatureType() == 673 ||
								pCreature->GetCreatureType() >= 526 && pCreature->GetCreatureType() <= 549 || 
								pCreature->GetCreatureType() >= 560 && pCreature->GetCreatureType() <= 563
								)								
							   )
							{
								// »ç¿ëÇÑ ±â¼ú·Î ¼³Á¤
								m_nNextUsedActionInfo	= MAGIC_EAT_CORPSE;

								// ÃßÀû Á¤º¸ ¼³Á¤
								m_fTraceBuffer = m_fTrace = FLAG_TRACE_SECTOR_SPECIAL;
								m_TraceX	= pItem->GetX();
								m_TraceY	= pItem->GetY();
								m_TraceZ	= 0;

								m_TraceDistance		= GetActionInfoRange(m_nNextUsedActionInfo);

								SetAction( m_MoveAction );
								SetNextDestination(m_TraceX, m_TraceY);

								m_fNextTrace			= FLAG_TRACE_NULL;
								
								m_bKeepTraceCreature	= false;
							}							
						}						
					}
			}

			//----------------------------------------------------------------
			//
			// µ·ÀÎ °æ¿ì --> ¹«Á¶°Ç inventory·Î..
			//
			//----------------------------------------------------------------
			else if (pItem->GetItemClass()==ITEM_CLASS_MONEY   || pItem->GetItemClass()== ITEM_CLASS_CHECK_MONEY )
			{
				PickupMoney( (MMoney*)pItem );	
			}
			
			else //if (m_CreatureType!=CREATURETYPE_BAT || m_MoveDevice!=MOVE_DEVICE_WALK)
			{
				//----------------------------------------------------------------
				// ¼³Ä¡µÈ Áö·ÚÀÎ °æ¿ì´Â ÁÖÀ» ¼ö ¾ø´Ù.
				//----------------------------------------------------------------
				if (pItem->GetItemClass()==ITEM_CLASS_MINE)
				{
					if (((MMine*)pItem)->IsInstalled())
					{
						return;
					}
				}				
				//----------------------------------------------------------------
				//
				//	 Event ¾ÆÀÌÅÛÀÎ °æ¿ì
				//
				//----------------------------------------------------------------
//				else if (pItem->GetItemClass()==ITEM_CLASS_EVENT_GIFT_BOX)
//				{
//					// °°Àº typeÀÇ ¾ÆÀÌÅÛÀÌ ÀÖ´ÂÁö Ã¼Å©ÇÑ´Ù.
//					if (NULL != g_pInventory->FindItem( ITEM_CLASS_EVENT_GIFT_BOX, pItem->GetItemType() ))
//					{
//						// ÀÖ´Ù¸é ¸ø ÁÝ´Â´Ù.
//						return;
//					}
//				}

				//----------------------------------------------------------------
				//
				//	Gear¿¡ µé¾î°¥ ¼ö ÀÖ´Â ItemÀÎ °æ¿ì..
				//
				//----------------------------------------------------------------
				if (pItem->IsGearItem())
				{					
					//------------------------------------------
					// gear³ª Inventory°¡ ¿­·ÁÀÖ´Ù¸é --> mouse·Î
					//------------------------------------------
					if (gC_vs_ui.GetGearOpenState() ||
						gC_vs_ui.GetInventoryOpenState())
					{							
						PickupItemToMouse( pItem );
					}
					//------------------------------------------
					// ¾Æ´Ï¸é 
					//------------------------------------------
					else
					{
						// gear¿¡ ³Ö¾îº¸°í µé¾î°¥ ¼ö ¾ø´Ù¸é
						// inventory¿¡ ³Ö¾îº»´Ù.

						// µÑ ´Ù ¾È µé¾î°¡¸é ¸ø Áý´Â´Ù.

						// ÀÏ´ÜÀº.. --;
						PickupItemToInventory( pItem );									
					}
				}
				//----------------------------------------------------------------
				//
				//	Inventory¿¡ µé¾î°¥ ¼ö ÀÖ´Â ItemÀÎ °æ¿ì..
				//
				//----------------------------------------------------------------
				else if (pItem->IsInventoryItem())
				{												
					//----------------------------------------------------------------
					// inventory°¡ ¿­·ÁÀÖ´Ù¸é --> mouse·Î
					//----------------------------------------------------------------
					if (gC_vs_ui.GetInventoryOpenState())
					{							
						PickupItemToMouse( pItem );
					}
					//----------------------------------------------------------------
					// ¾Æ´Ï¸é ±×³É inventory·Î
					//----------------------------------------------------------------
					else
					{
						if (pItem->IsQuickItem()
							&& PickupItemToQuickslot( pItem ))
						{
							// quickslot¿¡ µé¾î°¥ ¼ö ÀÖ¾î¼­ µé¾î°¡µµ µÇ´Â °æ¿ì´Ù.
							// ³É³É..
						}
						else
						{
							PickupItemToInventory( pItem );
						}
					}	
				}
			}
		}
		//----------------------------------------------------------------
		// À§Ä¡ °ËÁõÀÌ ¾ÈµÈ »óÅÂ
		//----------------------------------------------------------------
		//DEBUG_ADD_FORMAT("Cannot Pickup item (Position not verified) sendMove=%d", m_SendMove);		
	}
	#ifdef OUTPUT_DEBUG
	else
	{
		if (gpC_mouse_pointer->GetPickUpItem()!=NULL)
		{
			DEBUG_ADD_FORMAT("[UI] Already Handle a Item id=%d", gpC_mouse_pointer->GetPickUpItem()->GetID());
		}
	}
	#endif
	
}

//----------------------------------------------------------------------
// Pickup Item to Inventory
//----------------------------------------------------------------------
// ZoneÀÇ ItemÀ» ÁÖ¿ö¼­ inventory¿¡ ³Ö´Â´Ù.
//----------------------------------------------------------------------
void
MPlayer::PickupItemToInventory(MItem* pItem)
{
	//------------------------------------------
	// Inventory¿¡ µé¾î°¥ ÀÚ¸®°¡ ÀÖ´Â °æ¿ì
	// : Server¿¡¼­ °ËÁõ¹Þ±â À§ÇÑ packetÀ» º¸³½´Ù.
	//------------------------------------------
	
	// sjheon 2005.05.03  ±âÁ¸¿¡ Âü ¾ÆÀÌÅÆÀÌ ÀÖÀ»°æ¿ì ¾ÆÀÌÅÛÀ» ÁÝÁö ¸øÇÏ°Ô ÇÑ´Ù.Add
	if(pItem->GetItemClass() == ITEM_CLASS_EVENT_ITEM && (pItem->GetItemType() >= 32 && pItem->GetItemType() <= 36))
	{
		MItem* pItemCharm = NULL;
		for(int iItemType = 0 ;  iItemType <  5  ; iItemType++)
		{
			pItemCharm = g_pInventory->FindItem(pItem->GetItemClass() , iItemType + 32);
			if(pItemCharm) return ;
		}
	}
	// sjheon 2005.05.03  ±âÁ¸¿¡ Âü ¾ÆÀÌÅÆÀÌ ÀÖÀ»°æ¿ì ¾ÆÀÌÅÛÀ» ÁÝÁö ¸øÇÏ°Ô ÇÑ´Ù.End
	else if( pItem->GetItemClass() == ITEM_CLASS_QUEST_ITEM && pItem->GetItemType() == 4) {
		MItem *pSubInven = NULL;
		if( g_pInventory->FindItemAll(MItemClassTypeFinder(pItem->GetItemClass() , 4), pSubInven) )
			return;
	}

	POINT fitPoint;
	//------------------------------------------
	// ÀÌÀü¿¡ Ã³¸®ÇÒ·Á´Â itemÀÌ ¾ø´Â °æ¿ì..
	//------------------------------------------
	if (IsItemCheckBufferNULL())
	{
		
		if (g_pInventory->GetFitPosition(pItem, fitPoint) && 
			(pItem->IsSlayerItem() && IsSlayer() || 
			pItem->IsVampireItem() && IsVampire() || 
			pItem->IsOustersItem() && IsOusters()))
		{
			CGAddZoneToInventory _CGAddZoneToInventory;
			
			_CGAddZoneToInventory.setObjectID( pItem->GetID() );
			_CGAddZoneToInventory.setZoneX( pItem->GetX() );
			_CGAddZoneToInventory.setZoneY( pItem->GetY() );
			_CGAddZoneToInventory.setInvenX( fitPoint.x );
			_CGAddZoneToInventory.setInvenY( fitPoint.y );

			g_pSocket->sendPacket( &_CGAddZoneToInventory );

			// ÁÖ¿ï·Á´Â itemÀ» ±â¾ïÇÑ´Ù.
			pItem->SetGridXY( fitPoint.x, fitPoint.y );
			SetItemCheckBuffer(pItem, ITEM_CHECK_BUFFER_PICKUP_TO_INVENTORY);											
		}
		else
		{
			//------------------------------------------------------------
			// inventory¿¡ ÀûÀýÇÑ °ø°£ÀÌ ¾ø¾î¼­ ÁÖ¿ï ¼ö ¾ø´Â °æ¿ìÀÌ´Ù.				
			//------------------------------------------------------------
			pItem->SetDropping();

			// 2004, 5, 7 , sobeit add start - ÀÎº¥¿¡ ÀÚ¸®°¡ ¾øÀ¸¸é º¸°üÇÔ »ç¶ó°í µµ¿ò¸» º¸¿©ÁÜ
			ExecuteHelpEvent( HELP_EVENT_STORAGE_BUY );
			// 2004, 5, 6, sobeit add end
		}
	}
	//------------------------------------------
	// ¾ÆÁ÷ ´Ù¸¥ itemÀ» Ã³¸® ÁßÀÌ±â ¶§¹®¿¡
	// itemÀ» ÁÝÁö ¸øÇÏ´Â °æ¿ìÀÌ´Ù.
	//------------------------------------------
	else
	{
		// messageÃâ·Â
		// "¾ÆÁ÷ itemÀ» ÁÖ¿ï ¼ö ¾ø½À´Ï´Ù."
	}
}

//----------------------------------------------------------------------
// Pickup Money
//----------------------------------------------------------------------
// ZoneÀÇ ItemÀ» ÁÝ´Â´Ù.
//----------------------------------------------------------------------
void
MPlayer::PickupMoney(MMoney* pItem)
{
		//------------------------------------------
		// ÀÌÀü¿¡ Ã³¸®ÇÒ·Á´Â itemÀÌ ¾ø´Â °æ¿ì..
		//------------------------------------------
		if (IsItemCheckBufferNULL())
		{		
			CGPickupMoney _CGPickupMoney;
			
			_CGPickupMoney.setObjectID( pItem->GetID() );
			_CGPickupMoney.setZoneX( pItem->GetX() );
			_CGPickupMoney.setZoneY( pItem->GetY() );
		
			g_pSocket->sendPacket( &_CGPickupMoney );

			// ÁÖ¿ï·Á´Â itemÀ» ±â¾ïÇÑ´Ù.
			SetItemCheckBuffer(pItem, ITEM_CHECK_BUFFER_PICKUP_MONEY);											
		
		}
		//------------------------------------------
		// ¾ÆÁ÷ ´Ù¸¥ itemÀ» Ã³¸® ÁßÀÌ±â ¶§¹®¿¡
		// itemÀ» ÁÝÁö ¸øÇÏ´Â °æ¿ìÀÌ´Ù.
		//------------------------------------------
		else
		{
			// messageÃâ·Â
			// "¾ÆÁ÷ itemÀ» ÁÖ¿ï ¼ö ¾ø½À´Ï´Ù."
		}
}

//----------------------------------------------------------------------
// Pickup Item to Mouse
//----------------------------------------------------------------------
// ZoneÀÇ ItemÀ» ÁÖ¿ö¼­ Mouse¿¡ ºÙÀÎ´Ù. - -
//----------------------------------------------------------------------
void
MPlayer::PickupItemToMouse(MItem* pItem)
{
	//------------------------------------------
	// Server¿¡ Á¢¼ÓµÈ °æ¿ì´Â..
	//------------------------------------------
	// itemÀ» ÁÖ¿ï ¼ö ÀÖ´ÂÁö server¿¡¼­ 
	// °ËÁõÀ» ¹Þ¾Æ¾ß ÇÑ´Ù.
	//------------------------------------------
	
	// sjheon 2005.05.03  ±âÁ¸¿¡ Âü ¾ÆÀÌÅÆÀÌ ÀÖÀ»°æ¿ì ¾ÆÀÌÅÛÀ» ÁÝÁö ¸øÇÏ°Ô ÇÑ´Ù.Add
	if(pItem->GetItemClass() == ITEM_CLASS_EVENT_ITEM && (pItem->GetItemType() >= 32 && pItem->GetItemType() <= 36))
	{
		MItem* pItemCharm = NULL;
		for(int iItemType = 0 ;  iItemType <  5  ; iItemType++)
		{
			pItemCharm = g_pInventory->FindItem(pItem->GetItemClass() , iItemType + 32);
			if(pItemCharm) return ;
		}
	}
	// sjheon 2005.05.03  ±âÁ¸¿¡ Âü ¾ÆÀÌÅÆÀÌ ÀÖÀ»°æ¿ì ¾ÆÀÌÅÛÀ» ÁÝÁö ¸øÇÏ°Ô ÇÑ´Ù.End 


	// Áª ¾ÆÀÌÅÛµµ ÇÑ°³ ÀÌ»ó ¸ø ÁÝ°Ô ÇÑ´Ù.
	if( pItem->GetItemClass() == ITEM_CLASS_QUEST_ITEM && pItem->GetItemType() == 4) {
		MItem *pSubInven = NULL;
		if( g_pInventory->FindItemAll(MItemClassTypeFinder(pItem->GetItemClass() , 4), pSubInven) )
			return;
	}

	//*
	// chyaya
	if( pItem->GetItemClass() == ITEM_CLASS_COMMON_QUEST_ITEM ) {
		MItem *pSubInven = NULL;
		POINT FitPoint;
		if( false == GetMakeItemFitPosition(
			pItem, pItem->GetItemClass(), pItem->GetItemType(), FitPoint ) )
			return;
	}
	/**/

	//------------------------------------------
	// ÀÌÀü¿¡ Ã³¸®ÇÒ·Á´Â itemÀÌ ¾ø´Â °æ¿ì..
	//------------------------------------------
	if (IsItemCheckBufferNULL())
	{
		

		if(pItem->IsSlayerItem() && IsSlayer() || 
			pItem->IsVampireItem() && IsVampire() ||
			pItem->IsOustersItem() && IsOusters())
		{
			CGAddZoneToMouse _CGAddZoneToMouse;
			
			_CGAddZoneToMouse.setObjectID( pItem->GetID() );
			_CGAddZoneToMouse.setZoneX( pItem->GetX() );
			_CGAddZoneToMouse.setZoneY( pItem->GetY() );
			
			g_pSocket->sendPacket( &_CGAddZoneToMouse );
			
			// ÁÖ¿ï·Á´Â itemÀ» ±â¾ïÇÑ´Ù.
			SetItemCheckBuffer(pItem, ITEM_CHECK_BUFFER_PICKUP_TO_MOUSE);
		}
	}								
	//------------------------------------------
	// itemÀ» ÁÝÁö ¸øÇÏ´Â °æ¿ìÀÌ´Ù.
	//------------------------------------------
	else
	{
		// messageÃâ·Â
		// "¾ÆÁ÷ itemÀ» ÁÖ¿ï ¼ö ¾ø½À´Ï´Ù."
	}
}

//----------------------------------------------------------------------
// Pickup Item to Quickslot
//----------------------------------------------------------------------
// ZoneÀÇ ItemÀ» ÁÖ¿ö¼­ Mouse¿¡ ºÙÀÎ´Ù. - -
//----------------------------------------------------------------------
bool
MPlayer::PickupItemToQuickslot(MItem* pItem)
{
	//------------------------------------------------------------------
	// ÀÏ´Ü QuickSlot¿¡ µé¾î°¥ ¼ö ÀÖ´ÂÁö¸¦ È®ÀÎÇØ¾ß ÇÑ´Ù.
	//------------------------------------------------------------------
	if ( (g_pQuickSlot!=NULL&&IsSlayer()) || ((g_pArmsBand1!=NULL||g_pArmsBand2!=NULL)&&IsOusters())
		&& IsItemCheckBufferNULL()
		&& g_pTempInformation->GetMode()==TempInformation::MODE_NULL
		&& (pItem->IsSlayerItem() && IsSlayer() || 
		pItem->IsVampireItem() && IsVampire() || 
		pItem->IsOustersItem() && IsOusters()))
	{
		BOOL FirstArmsband = TRUE;
		int slot;

		//---------------------------------------------------------
		// QuickSlotÀÇ ¾îµð¿¡ µé¾î°¥ ¼ö ÀÖÀ»±î?
		//---------------------------------------------------------
		if (IsSlayer() && !g_pQuickSlot->FindSlotToAddItem(pItem, slot))
		{
			return false;
		} 
		else if( IsOusters() )
		{
			// Ã¹¹øÂ°²¨¸ÕÀú µÚÁö°í ¾øÀ¸¸é µÎ¹øÂ°²¬ µÚÁø´Ù.
			
			if( g_pArmsBand1 == NULL ||  !g_pArmsBand1->FindSlotToAddItem( pItem, slot ) )
			{
				if( g_pArmsBand2 == NULL || !g_pArmsBand2->FindSlotToAddItem( pItem,slot ) )
					return false;
				else FirstArmsband = FALSE;
			} else
			{
				FirstArmsband = TRUE;
			}
		}

			//---------------------------------------------------------
			// ÀÏ´Ü ¸¶¿ì½º·Î °¥ ¼ö ÀÖ´Â°É °ËÁõ¹Þ¾Æ¾ß ÇÑ´Ù...
			//---------------------------------------------------------
			CGAddZoneToMouse _CGAddZoneToMouse;

			_CGAddZoneToMouse.setObjectID( pItem->GetID() );
			_CGAddZoneToMouse.setZoneX( pItem->GetX() );
			_CGAddZoneToMouse.setZoneY( pItem->GetY() );

			g_pSocket->sendPacket( &_CGAddZoneToMouse );

			//---------------------------------------------------------
			// slot¿¡ µé¾î°¥ ¼ö ÀÖ´Ù.
			//---------------------------------------------------------
			g_pTempInformation->SetMode(TempInformation::MODE_TRADE_VERIFY_PICKUP_TO_QUICKSLOT);
			g_pTempInformation->Value1 = FirstArmsband ? slot : slot + 3;
			g_pTempInformation->pValue = (void*)pItem;

			SetItemCheckBuffer(pItem, ITEM_CHECK_BUFFER_PICKUP_TO_QUICKSLOT);


		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Pickup Item to Mouse
//----------------------------------------------------------------------
// ZoneÀÇ ItemÀ» ÁÖ¿ö¼­ Mouse¿¡ ºÙÀÎ´Ù. - -
//----------------------------------------------------------------------
void
MPlayer::RideMotorcycle(MMotorcycle* pMotorcycle)
{
	if (pMotorcycle==NULL)
	{
		DEBUG_ADD("[Error] Player::Motorcycle to ride is NULL");
		
		return;
	}

	//------------------------------------------
	// °È°í ÀÖ´Â ÁßÀÎ °æ¿ì ¿ÀÅä¹ÙÀÌ¿¡ Å» ¼ö ÀÖ´Ù.
	//------------------------------------------
	if (m_MoveDevice==MOVE_DEVICE_WALK)
	{
		CGRideMotorCycle _CGRideMotorCycle;

		_CGRideMotorCycle.setObjectID( pMotorcycle->GetID() );
		_CGRideMotorCycle.setX( pMotorcycle->GetX() );
		_CGRideMotorCycle.setY( pMotorcycle->GetY() );

		g_pSocket->sendPacket( &_CGRideMotorCycle );

		// MotorcycleÀ» Å¸±â À§ÇØ °ËÁõ¹Þ±â¸¦ ±â´Ù¸°´Ù.
		SetWaitVerify( WAIT_VERIFY_MOTORCYCLE_GETON );
	}
	//------------------------------------------
	// ÀÌ¹Ì ¿ÀÅä¹ÙÀÌ¸¦ Å¸°í ÀÖ´Â °æ¿ìÀÌ´Ù.
	//------------------------------------------
	else
	{
		// ¿ÀÅä¹ÙÀÌ Å» ¼ö ¾ø´Ù. 
	}
}

//----------------------------------------------------------------------
// Skill To Inventory Item
//----------------------------------------------------------------------
// Inventory¿¡ ÀÖ´Â item¿¡ ±â¼úÀ» »ç¿ëÇÑ´Ù.
// ¿©±â¼­´Â castingµ¿ÀÛ¸¸ ½ÃÀÛÇÏ°Ô ÇÏ¸é µÈ´Ù.
//
// °á°ú´Â packetÀ» ¹Þ¾Æ¼­ Ã³¸®..
// °á°ú´Â (global) AddNewInventoryEffect(...)¿¡¼­ º¸¿©Áö°Ô ÇÏ¸é µÈ´Ù.
//----------------------------------------------------------------------
bool
MPlayer::TraceInventoryItem(TYPE_OBJECTID id)
{
	//-----------------------------------------------------
	// delay°¡ ÀÖÀ¸¸é ¾ÈµÈ´Ù.
	//-----------------------------------------------------
	if (!IsNotDelay()
		|| HasEffectStatus( EFFECTSTATUS_ETERNITY_PAUSE ))
	{
		return false;
	}

	//--------------------------------------------------------
	// »ç¿ëÇÒ ¼ö ¾ø´Â °æ¿ì
	//--------------------------------------------------------
	if (m_nSpecialActionInfo == ACTIONINFO_NULL
		// Hava araci (Hover/Heli) bindiginde ozel saldiri kullanilamaz.
		|| IsRide_Air_Transporter()
		// ¹º°¡ ±â¼ú »ç¿ëÀ» °ËÁ¤ ¹Þ¾Æ¾ß ÇÏ¸é »ç¿ëÇÒ ¼ö ¾ø´Ù.
		//|| m_WaitVerify != WAIT_VERIFY_NULL)	// 2001.8.20 ÁÖ¼®Ã³¸®
		)
	{
		return false;
	}
	
	//--------------------------------------------------------
	// ´Ù¸¥ ±â¼úÀ» »ç¿ëÁßÀÎ °æ¿ì
	//--------------------------------------------------------
	if (m_nUsedActionInfo!=ACTIONINFO_NULL)
		return false;

	// 2006.08.22 sjheon
	if(HasEffectStatus(EFFECTSTATUS_TRANSFORM_TO_WOLF))
	{
		if( !(m_nSpecialActionInfo == MAGIC_HOWL || m_nSpecialActionInfo == MAGIC_EAT_CORPSE || m_nSpecialActionInfo == MAGIC_UN_TRANSFORM))
			return false ;
	}
	else if(HasEffectStatus(EFFECTSTATUS_TRANSFORM_TO_BAT))
	{
		if( !(m_nSpecialActionInfo == MAGIC_UN_TRANSFORM))
			return false ;
	}	

	//--------------------------------------------------------
	// ÇöÀç ±â¼úÀÌ item¿¡ »ç¿ëÇÏ´Â°Ô ¸Â³ª?
	//--------------------------------------------------------
	if ((*g_pActionInfoTable)[m_nSpecialActionInfo].IsTargetItem())
	{
		//-------------------------------------------------------
		// ÇöÀç »ç¿ëÇÒ ¼ö ÀÖ´Â ±â¼úÀÎÁö Ã¼Å©..
		// Passive ½ºÅ³ÀÌ¸é »ç¿ë ¸øÇÏ°Ô..
		//-------------------------------------------------------
		if (!g_pSkillAvailable->IsEnableSkill( (ACTIONINFO)m_nSpecialActionInfo )
			|| !(*g_pSkillInfoTable)[m_nSpecialActionInfo].IsEnable()
			|| !(*g_pSkillInfoTable)[m_nSpecialActionInfo].IsAvailableTime()
			|| (*g_pSkillInfoTable)[m_nSpecialActionInfo].IsPassive())
		{
			m_fNextTrace = FLAG_TRACE_NULL;

			UnSetRepeatAction();

			return false;
		}

		// Á¤Áö »óÅÂ¿¡¼­¸¸ »ç¿ëÇÒ ¼ö ÀÖ´Ù.
		if (IsStop() && IsItemCheckBufferNULL()) // || m_Action!=ACTION_ATTACK)
		{
			//--------------------------------------------------------
			// pItemÀÌ inventory¿¡ ÀÖ´Â itemÀÎÁö Ã¼Å©ÇÑ´Ù.
			//--------------------------------------------------------
			const MItem* pItem = ((MItemManager)(*g_pInventory)).GetItem( id );
			
			if (pItem==NULL)
			{
				return false;
			}

			// »ç¿ëÇÑ ±â¼ú·Î ¼³Á¤
			m_nNextUsedActionInfo	= m_nSpecialActionInfo;

			// ÃßÀû Á¤º¸ ¼³Á¤
			SetTraceID( id );
			m_fTrace	= FLAG_TRACE_INVENTORY;
			m_TraceX	= pItem->GetGridX();
			m_TraceY	= pItem->GetGridY();
			m_TraceZ	= 0;

			// ÇöÀç ÃßÀûÇÏ´Â ´ë»ó¿¡ ´ëÇØ¼­ ±â¾ïÇØµÐ´Ù.
			m_fTraceBuffer	= m_fTrace;	
			

			//------------------------------------------------------------		
			// Inventory Item¿¡ ´ëÇØ¼­´Â ÀÇ¹Ì ¾ø´Ù. - -;
			//------------------------------------------------------------	
			m_TraceDistance		= 1;			
			SetAction( m_MoveAction );
			SetNextDestination(m_X, m_Y);

			// bufferingÀ» ¾ø¾Ø´Ù.
			m_fNextTrace			= FLAG_TRACE_NULL;
		
			// °è¼Ó ÂÑ¾Æ°¡´Â °ÍÀ» ÁßÁö
			m_bKeepTraceCreature	= false;

			//------------------------------------------------------------
			// messageÃâ·Â
			//------------------------------------------------------------
			DEBUG_ADD_FORMAT("Trace Item: %d", id);				
		}
		else
		{
			//------------------------------------------------------------
			// ¹Ýº¹ actionÇÏ´Â ÁßÀÌ ¾Æ´Ñ °æ¿ì¿¡ ´ÙÀ½ µ¿ÀÛ buffering
			//------------------------------------------------------------
			if (m_bRepeatAction)
			{
				m_fNextTrace	= FLAG_TRACE_NULL;

				return false;
			}
			else
			{
				m_fNextTrace	= FLAG_TRACE_INVENTORY;
				m_NextTraceID	= id;

				m_bRepeatAction = FALSE;

				return false;
			}
		}

		return true;
	}

	// item¿¡ »ç¿ëÇÏ´Â ±â¼úÀÌ ¾Æ´Ï¶ó´Â ÀÇ¹Ì
	return false;
}

//----------------------------------------------------------------------
// Change To Slayer
//----------------------------------------------------------------------
// slayer·Î º¯½ÅÇÏ´Âµ¥..
// ÀÌ¹Ì slayerÀÌ¸é.. return false
//----------------------------------------------------------------------
bool	
MPlayer::ChangeToSlayer()
{
	if (MCreature::ChangeToSlayer())
	{
		m_ConversionDelayTime = 0;

		g_pCurrentMagazine = NULL;
		
		//-----------------------------------------------------
		// º¸°üÇÔµµ ³¯·ÁÁØ´Ù.
		//-----------------------------------------------------
		if (g_pStorage!=NULL)
		{
			delete g_pStorage;
			g_pStorage = NULL;
		}

		//-----------------------------------------------------
		// ¹øÂ½
		//-----------------------------------------------------
		g_pTopView->SetFadeStart(1, 31, 6,  5,5,31);

		UI_ChangeInterfaceRace(RACE_SLAYER);

		SetWaitVerifyNULL();

		ResetSendMove();
		
		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Change To Vampire
//----------------------------------------------------------------------
// vampire·Î º¯½ÅÇÏ´Âµ¥..
// ÀÌ¹Ì vampireÀÌ¸é.. return false
//----------------------------------------------------------------------
bool
MPlayer::ChangeToVampire()
{
	if (MCreature::ChangeToVampire())
	{
		m_ConversionDelayTime = 0;

		g_pCurrentMagazine = NULL;
		

		//-----------------------------------------------------
		// º¸°üÇÔµµ ³¯·ÁÁØ´Ù.
		//-----------------------------------------------------
		if (g_pStorage!=NULL)
		{
			delete g_pStorage;
			g_pStorage = NULL;
		}

		g_pTopView->SetFadeStart(1, 31, 6,  31,0,0);

		UI_ChangeInterfaceRace(RACE_VAMPIRE);

		SetWaitVerifyNULL();

		ResetSendMove();

		DEBUG_ADD("MPlayer::Change to Vampire OK");
		
		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Set Status
//----------------------------------------------------------------------
// Ä³¸¯ÅÍÀÇ »óÅÂ°ªÀ» ¹Ù²Û´Ù.
//----------------------------------------------------------------------
void	
MPlayer::SetStatus(DWORD n, DWORD value)
{ 
	if (n >= MODIFY_MAX)
	{
		DEBUG_ADD_FORMAT("[Error] Modify Part is Wrong : part=%d, value=%d", n, value);
		
		return;
	}

	MODIFY_VALUE modifyValue;
	modifyValue.oldValue = m_Status[n];
	modifyValue.newValue = value;
	
	/* ÀÏº»¿¡ µé¾î°£°Å -_- ³ªÁß¿¡ ÇÊ¿äÇÒ±îºÁ
	if( GetWaitVerify() == WAIT_VERIFY_LOGOUT	&&
		n == MODIFY_CURRENT_HP					&&
		modifyValue.oldValue > modifyValue.newValue ) {

		g_pSystemMessage->Add("ÀûÀÇ °ø°Ý¿¡ ÀÇÇØ ·Î±×¾Æ¿ôÀÌ Ãë¼ÒµÇ¾ú½À´Ï´Ù.");
		g_pEventManager->RemoveEvent(EVENTID_LOGOUT);
		g_pUserInformation->LogoutTime = 0;

		SetWaitVerify(WAIT_VERIFY_NULL);
	}
	*/
	
	// ÀÏ´Ü °ª º¯°æ
	m_Status[n] = value; 
	
	// º¯°æµÈ °ª°ú °ü·ÃµÈ Ã³¸®
	g_pModifyStatusManager->Execute( n, (void*)&modifyValue );

	// ´Ù½Ã.. - -;
	m_Status[n] = modifyValue.newValue;


	if(g_pUserInformation->attrOperator.GetValue() &&
	   g_pOperatorOption->bShowModifyHP)
	{
		/////////////////////////////////////////////////////////////////////////
		// 2006.11.10 chyaya - ÇÇ º¯È­·® Ç¥½Ã! (³ªÁß¿¡ Á¤¸®ÇØ¾ßÇÔ)
		if(n == MODIFY_CURRENT_HP)
		{
			int modifyHP = modifyValue.newValue - modifyValue.oldValue;

			if(modifyHP != 0)
			{
				char szNameBuf[64];
				sprintf(szNameBuf, "%s(%d,%d)",
					g_pUserInformation->CharacterID.GetString(), GetX(), GetY());

				HP_t curHP = GetHP();
				HP_t maxHP = max(GetMAX_HP() - GetSilverDamage(), curHP);

				char szInfoBuf[128];
				sprintf(szInfoBuf, "MAX/CUR=%4d/%4d, %s=%d",
					curHP, maxHP, (modifyHP>0)?"HEAL":"DAM", abs(modifyHP));

				if(curHP == 0)
					strcat(szInfoBuf, " (DIE)");
				else if(modifyHP > 0 && curHP - modifyHP == 0)
					strcat(szInfoBuf, " (CREATE)");

				DWORD color = (modifyHP > 0) ? RGB_GREEN : RGB_RED;

				UI_AddChatToHistory(szInfoBuf, szNameBuf, 6, color);
			}
		}
		//
		/////////////////////////////////////////////////////////////////////////
	}

	if(g_pUserInformation->attrOperator.GetValue() &&
	   g_pOperatorOption->bShowModifyRANKEXP)
	{
		/////////////////////////////////////////////////////////////////////////
		// 2008.3.11 cheon ill zea - °è±Þ°æÇèÄ¡ º¯È­·® Ç¥½Ã!
		if(n == MODIFY_RANK_EXP_REMAIN)
		{
			int modifyRANKEXP = modifyValue.newValue - modifyValue.oldValue;

			if(modifyRANKEXP != 0)
			{
				char szNameBuf[256];
			
				sprintf(szNameBuf, "%s(%d,%d)",
				g_pUserInformation->CharacterID.GetString(), GetX(), GetY());

				DOUBLE oldRANKEXP = (DOUBLE)modifyValue.oldValue;
				DOUBLE curRANKEXP = (DOUBLE)modifyValue.newValue;

				__int64	goal_exp = max(1, g_pExperienceTable->GetRankInfo(g_char_slot_ingame.GRADE, g_eRaceInterface).GoalExp);
		
				char szInfoBuf[128];
				sprintf(szInfoBuf, "(srt=%4d) (old=%4d) (-%d) (cur=%4d) (%0.2f%%)",
					(int)goal_exp, (int)oldRANKEXP, abs((int)modifyRANKEXP), (int)curRANKEXP,
					(goal_exp-curRANKEXP)*100/goal_exp);

				if(curRANKEXP == 0)
					strcat(szInfoBuf, " (UPGRADE)");

				DWORD color = RGB_GOLD;
				UI_AddChatToHistory(szInfoBuf, szNameBuf, 6, color);
			}
		}
		//
		/////////////////////////////////////////////////////////////////////////
	}

	//20070705 by diesirace
	if(g_pUserInformation->attrOperator.GetValue() &&
		g_pOperatorOption->bShowModifyEXP)
	{
		if((n >= MODIFY_SWORD_DOMAIN_EXP_REMAIN && 
			n <= MODIFY_GUN_DOMAIN_EXP_REMAIN && 
			((n - MODIFY_SWORD_DOMAIN_EXP_REMAIN)%3 == 0)) ||
			n == MODIFY_VAMP_EXP_REMAIN || 
			n == MODIFY_OUSTERS_EXP_REMAIN || 
			n == MODIFY_ADVANCEMENT_CLASS_GOAL_EXP)
		{
			int modifyEXP = modifyValue.oldValue - modifyValue.newValue;
			// µµ¸ÞÀÎ ·¾ÀÌ ÇÊ¿äÇÑ ³à¼®ÀÎÁö ÆÇº°
			bool isDomain = (g_pPlayer->GetRace() == RACE_SLAYER && 
				n >= MODIFY_SWORD_DOMAIN_EXP_REMAIN && 
				n <= MODIFY_GUN_DOMAIN_EXP_REMAIN);

			if(modifyEXP > 0)
			{
				char szNameBuf[64];
				sprintf(szNameBuf, "%s(%d,%d)",
					g_pUserInformation->CharacterID.GetString(), GetX(), GetY());
				
				int remainEXP = m_Status[n];
				char szInfoBuf[128];
				sprintf(szInfoBuf, "REMAIN =%6d GETEXP =%6d ",
					remainEXP, modifyEXP);
				if(isDomain)
				{
					char* domainName[] = {"(Sword)", "(Blade)", "(Heal)", "(Enchant)", "(Gun)"};
					//°¢ °æÇèÄ¡º°·Î 3°¡Áö Á¾·ù°¡ ÀÖ¾î¼­ ºÎµîÈ£·Î Á¶°ÇÀ» Ã¼Å©ÇØ¼­ 
					//°¢ µµ¸ÞÀÎ ¸¶´Ù µðÆÄÀÎ Á¾·ù°¡ 3°¡ÁöÀÌ±â ¶§¹®¿¡ 3À¸·Î ³ª´«´Ù.
					int domainNameNum = (n - MODIFY_SWORD_DOMAIN_EXP_REMAIN)/3;
					strcat(szInfoBuf, domainName[domainNameNum]);
				}

				DWORD color = RGB_GOLD;
				UI_AddChatToHistory(szInfoBuf, szNameBuf, 6, color);
			}

			//·¾¾÷ÇÒ¶§ ¸ÊÀÌµ¿ ÇÒ¶§
			else if(modifyEXP < 0)
			{
				char szNameBuf[64];
				sprintf(szNameBuf, "%s(%d,%d)",
					g_pUserInformation->CharacterID.GetString(), GetX(), GetY());
				
				int remainEXP = m_Status[n];
				char szInfoBuf[128];
				if(!(modifyEXP + modifyValue.newValue))
				{
					sprintf(szInfoBuf, "REMAIN =%6d GETEXP =%6d (CREATE)",
					remainEXP, modifyValue.oldValue);
				}
				else
				{
					sprintf(szInfoBuf, "REMAIN =%6d GETEXP =%6d (LEVELUP)",
						remainEXP, modifyValue.oldValue);
				}
				DWORD color = RGB_GOLD;
				UI_AddChatToHistory(szInfoBuf, szNameBuf, 6, color);
			}
		}
	}
}


//----------------------------------------------------------------------
// Affect Item Option
//----------------------------------------------------------------------
// ¾ÆÀÌÅÛ ¿É¼Ç Àû¿ë
//----------------------------------------------------------------------
void
MPlayer::AffectItemOption(const MItem* pItem)
{
	CT_Done("AIO: ENTER pItem=%p", (void*)pItem);
	DWORD& DV				= m_Status[MODIFY_DEFENSE];
	DWORD& PV				= m_Status[MODIFY_PROTECTION];
	DWORD& TOHIT			= m_Status[MODIFY_TOHIT];
	DWORD& MinDAM			= m_Status[MODIFY_MIN_DAMAGE];
	DWORD& MaxDAM			= m_Status[MODIFY_MAX_DAMAGE];
	DWORD& AttackSpeed		= m_Status[MODIFY_ATTACK_SPEED];

	DWORD& ElementalFire	= m_Status[MODIFY_ELEMENTAL_FIRE];
	DWORD& ElementalWater	= m_Status[MODIFY_ELEMENTAL_WATER];
	DWORD& ElementalEarth	= m_Status[MODIFY_ELEMENTAL_EARTH];
	DWORD& ElementalWind	= m_Status[MODIFY_ELEMENTAL_WIND];

	//----------------------------------------------------------
	// ¼öÄ¡°¡ Àû¿ëµÇ´Â itemÀÎ °æ¿ì¸¸..
	//----------------------------------------------------------
	CT_Done("AIO: pre IsAffectStatus (virtual)");
	bool bAffect = (pItem != NULL) ? pItem->IsAffectStatus() : false;
	CT_Done("AIO: post IsAffectStatus=%d", (int)bAffect);
	if (pItem != NULL && bAffect)
	{
		CT_Done("AIO: pre GetItemClass (virtual)");
		int itemClass = pItem->GetItemClass();
		CT_Done("AIO: post GetItemClass=%d", itemClass);
		if(itemClass == ITEM_CLASS_OUSTERS_WRISTLET || itemClass == ITEM_CLASS_OUSTERS_STONE)
		{
			ITEMTABLE_INFO iInfo = (*g_pItemTable)[pItem->GetItemClass()][pItem->GetItemType()];
			if(iInfo.ElementalType == ITEMTABLE_INFO::ELEMENTAL_TYPE_FIRE)
				ElementalFire += iInfo.Elemental;
			else if(iInfo.ElementalType == ITEMTABLE_INFO::ELEMENTAL_TYPE_WATER)
				ElementalWater += iInfo.Elemental;
			else if(iInfo.ElementalType == ITEMTABLE_INFO::ELEMENTAL_TYPE_EARTH)
				ElementalEarth += iInfo.Elemental;
			else if(iInfo.ElementalType == ITEMTABLE_INFO::ELEMENTAL_TYPE_WIND)
				ElementalWind += iInfo.Elemental;
			
		}
		//---------------------------------------------------
		// ±âº»ÀûÀÎ ¹æ¾î¼öÄ¡ Áõ°¡
		//---------------------------------------------------
		CT_Done("AIO: pre GetDefenseValue (virtual)");
		int def = pItem->GetDefenseValue();
		CT_Done("AIO: post GetDefenseValue=%d", def);

		if (def != -1)
		{
			DV += def;
		}

		CT_Done("AIO: pre GetProtectionValue (virtual)");
		int pro = pItem->GetProtectionValue();
		CT_Done("AIO: post GetProtectionValue=%d", pro);

		if (pro != -1)
		{
			PV += pro;
		}

		CT_Done("AIO: pre GetItemOptionList (virtual)");
		const std::list<TYPE_ITEM_OPTION>& optionList = pItem->GetItemOptionList();
		CT_Done("AIO: post GetItemOptionList size=%d", (int)optionList.size());

		// FAZ 4: g_pItemOptionTable bounds check (option index OOB crash önle)
		const int aioOptTableSize1 = (g_pItemOptionTable ? g_pItemOptionTable->GetSize() : 0);
		std::list<TYPE_ITEM_OPTION>::const_iterator optionListItr = optionList.begin();

		while(optionListItr != optionList.end())
		{
			if ((int)*optionListItr < 0 || (int)*optionListItr >= aioOptTableSize1) {
				CT_Done("AIO-OL: option=%d OOB (size=%d), SKIP", (int)*optionListItr, aioOptTableSize1);
				optionListItr++;
				continue;
			}

			ITEMOPTION_INFO& optionInfo = (*g_pItemOptionTable)[*optionListItr];
			//---------------------------------------------------
			// ºÎ°¡ÀûÀÎ Option
			//---------------------------------------------------
			switch ((ITEMOPTION_TABLE::ITEMOPTION_PART)optionInfo.Part)
			{
			case ITEMOPTION_TABLE::PART_DEFENSE		: DV		+= optionInfo.PlusPoint; break;
			case ITEMOPTION_TABLE::PART_TOHIT		: TOHIT		+= optionInfo.PlusPoint; break;
			case ITEMOPTION_TABLE::PART_PROTECTION	: PV		+= optionInfo.PlusPoint; break;
			//case ITEMOPTION_INFO::PART_DAMAGE		: MinDAM	+= optionInfo.PlusPoint; 
			//										  MaxDAM	+= optionInfo.PlusPoint; break;
				
			//case ITEMOPTION_TABLE::PART_ATTACK_SPEED : AttackSpeed += optionInfo.PlusPoint; break;
				
			case ITEMOPTION_TABLE::PART_VISION :	SetItemLightSight(GetItemLightSight() + optionInfo.PlusPoint); break;
			}
			optionListItr++;
		}
		
		CT_Done("AIO: pre GetItemDefaultOptionList (virtual) — class=%d type=%d", (int)pItem->GetItemClass(), (int)pItem->GetItemType());
		const std::list<TYPE_ITEM_OPTION>& defaultList = pItem->GetItemDefaultOptionList();
		CT_Done("AIO: post GetItemDefaultOptionList ref=%p size=%d", (void*)&defaultList, (int)defaultList.size());
		std::list<TYPE_ITEM_OPTION>::const_iterator itr = defaultList.begin();
		std::list<TYPE_ITEM_OPTION>::const_iterator endItr = defaultList.end();
		CT_Done("AIO: post begin/end iterators captured");

		// FAZ 4 (2026-05-02): g_pItemOptionTable bounds check (memory'deki
		// "Empty global table OOB pattern" — option index >= table size crash).
		const int aioOptTableSize = (g_pItemOptionTable ? g_pItemOptionTable->GetSize() : 0);
		CT_Done("AIO-DL: g_pItemOptionTable size=%d", aioOptTableSize);

		int aioDefIter = 0;
		while(itr != endItr)
		{
			CT_Done("AIO-DL: iter %d pre *itr deref", aioDefIter);
			TYPE_ITEM_OPTION option = *itr;
			CT_Done("AIO-DL: iter %d post deref option=%d, pre OptionTable bounds check", aioDefIter, (int)option);

			if ((int)option < 0 || (int)option >= aioOptTableSize) {
				CT_Done("AIO-DL: iter %d option=%d OOB (size=%d), SKIP", aioDefIter, (int)option, aioOptTableSize);
				itr++;
				aioDefIter++;
				continue;
			}

			ITEMOPTION_INFO& optionInfo = (*g_pItemOptionTable)[option];
			CT_Done("AIO-DL: iter %d post OptionTable lookup Part=%d", aioDefIter, (int)optionInfo.Part);
			//---------------------------------------------------
			// ºÎ°¡ÀûÀÎ Option
			//---------------------------------------------------
			switch ((ITEMOPTION_TABLE::ITEMOPTION_PART)optionInfo.Part)
			{
			case ITEMOPTION_TABLE::PART_DEFENSE		: DV		+= optionInfo.PlusPoint; break;
			case ITEMOPTION_TABLE::PART_TOHIT		: TOHIT		+= optionInfo.PlusPoint; break;
			case ITEMOPTION_TABLE::PART_PROTECTION	: PV		+= optionInfo.PlusPoint; break;
			//case ITEMOPTION_INFO::PART_DAMAGE		: MinDAM	+= optionInfo.PlusPoint; 
			//										  MaxDAM	+= optionInfo.PlusPoint; break;
				
			//case ITEMOPTION_TABLE::PART_ATTACK_SPEED : AttackSpeed += optionInfo.PlusPoint; break;
				
			case ITEMOPTION_TABLE::PART_VISION :	SetItemLightSight(GetItemLightSight() + optionInfo.PlusPoint); break;
			}
			CT_Done("AIO-DL: iter %d post switch, pre itr++", aioDefIter);

			itr++;
			CT_Done("AIO-DL: iter %d post itr++", aioDefIter);
			aioDefIter++;
		}
		CT_Done("AIO: post DefaultOptionList loop iters=%d", aioDefIter);
	}
	CT_Done("AIO: EXIT");
}



//----------------------------------------------------------------------
// Check Status
//----------------------------------------------------------------------
// ¼öÄ¡ º¯ÇÏ´Â°Å Ã¼Å©
//----------------------------------------------------------------------
void
MPlayer::CalculateStatus()
{
	CT_Done("CS: ENTER race=%d", (int)GetRace());
	//-----------------------------------------------------------------
	// item Âø¿ëÇÑ°Å¿¡ µû¶ó¼­...
	//-----------------------------------------------------------------
	MItemManager* pGear;

	int weaponSpeed = 0;
	int weaponTohit = 0;

	switch(GetRace())
	{
	case RACE_SLAYER:
		{
			CT_Done("CS: SLAYER branch");
			pGear = g_pSlayerGear;

			CT_Done("CS: pre g_pSlayerGear->GetItem(RIGHTHAND)");
			const MItem* pWeapon = g_pSlayerGear->GetItem( MSlayerGear::GEAR_SLAYER_RIGHTHAND );
			CT_Done("CS: post GetItem pWeapon=%p", (void*)pWeapon);

			SKILLDOMAIN domain = MAX_SKILLDOMAIN;
			int domainLevel = 0;

			//------------------------------------------------------
			// ÃÑµé¾ú´ÂÁö Ã¼Å©
			//------------------------------------------------------
			CT_Done("CS: pre pWeapon NULL/IsAffectStatus check");
			bool bWpnAffect = (pWeapon != NULL) ? pWeapon->IsAffectStatus() : false;
			CT_Done("CS: bWpnAffect=%d", (int)bWpnAffect);
			if (pWeapon!=NULL && bWpnAffect)
			{
				CT_Done("CS: pre IsGunItem (virtual)");
				bool bIsGun = pWeapon->IsGunItem();
				CT_Done("CS: post IsGunItem=%d, pre GetItemClass (virtual)", (int)bIsGun);
				int weaponClass = pWeapon->GetItemClass();
				CT_Done("CS: post GetItemClass=%d", weaponClass);
				if (bIsGun)
				{
					domain = SKILLDOMAIN_GUN;
					CT_Done("CS: pre GetToHit (virtual)");
					weaponTohit = pWeapon->GetToHit();
					CT_Done("CS: post GetToHit=%d", weaponTohit);
				}
				else if (weaponClass==ITEM_CLASS_SWORD)
				{
					domain = SKILLDOMAIN_SWORD;
				}
				else if (weaponClass==ITEM_CLASS_BLADE)
				{
					domain = SKILLDOMAIN_BLADE;
				}
				else if (weaponClass==ITEM_CLASS_CROSS)
				{
					domain = SKILLDOMAIN_HEAL;
				}
				else if (weaponClass==ITEM_CLASS_MACE)
				{
					domain = SKILLDOMAIN_ENCHANT;
				}

				if (domain < MAX_SKILLDOMAIN)
				{
					CT_Done("CS: pre SkillManager[%d] domainLevel", (int)domain);
					domainLevel = (*g_pSkillManager)[domain].GetDomainLevel();
					CT_Done("CS: post domainLevel=%d", domainLevel);
				}


				//----------------------------------------------------
				// ¹«±âÀÇ ±âº» ¼Óµµ
				//----------------------------------------------------
				CT_Done("CS: pre weaponSpeed branch");
				if(pWeapon->IsAffectStatus()) {
					weaponSpeed = pWeapon->GetOriginalSpeed();
					CT_Done("CS: post weaponSpeed=%d", weaponSpeed);
				}
			}
			CT_Done("CS: pre SetCurrentWeaponDomain");
			g_StatusManager.SetCurrentWeaponDomain( domain, domainLevel );
			CT_Done("CS: post SetCurrentWeaponDomain");
		}
		break;

	case RACE_VAMPIRE:
		{
			pGear = g_pVampireGear;
			
			const MItem* pWeapon = g_pVampireGear->GetItem( MVampireGear::GEAR_VAMPIRE_RIGHTHAND );
			
			if (pWeapon!=NULL)
			{
				//----------------------------------------------------
				// ¹«±âÀÇ ±âº» ¼Óµµ
 				//----------------------------------------------------			
				if(pWeapon->IsAffectStatus())
					weaponSpeed = pWeapon->GetOriginalSpeed();
			}
			g_StatusManager.SetCurrentWeaponDomain( SKILLDOMAIN_VAMPIRE, GetLEVEL() );
		}
		break;

	case RACE_OUSTERS:
		{
			pGear = g_pOustersGear;
			
			const MItem* pWeapon = g_pOustersGear->GetItem( MOustersGear::GEAR_OUSTERS_RIGHTHAND );
			
			if (pWeapon!=NULL)
			{
				//----------------------------------------------------
				// ¹«±âÀÇ ±âº» ¼Óµµ
				//----------------------------------------------------			
				if(pWeapon->IsAffectStatus())
					weaponSpeed = pWeapon->GetOriginalSpeed();
			}
			g_StatusManager.SetCurrentWeaponDomain( SKILLDOMAIN_OUSTERS, GetLEVEL() );
		}
		break;
	}

	//-----------------------------------------------------------------
	// GearÀÇ ¸ðµç item¿¡ ´ëÇØ¼­..
	//
	// DAM, AC, TOHIT, CC option¸¸ Àû¿ë½ÃÅ²´Ù.
	//-----------------------------------------------------------------
	g_StatusManager.Set(GetSTR(), GetDEX(), GetINT());

	DWORD& DV				= m_Status[MODIFY_DEFENSE];
	DWORD& PV				= m_Status[MODIFY_PROTECTION];
	DWORD& TOHIT			= m_Status[MODIFY_TOHIT];
	DWORD& MinDAM			= m_Status[MODIFY_MIN_DAMAGE];
	DWORD& MaxDAM			= m_Status[MODIFY_MAX_DAMAGE];
	DWORD& AttackSpeed		= m_Status[MODIFY_ATTACK_SPEED];

	//-----------------------------------------------------------------
	// DV, PV, TOHIT´Â °è»êÀ» ³¡³½ ÈÄ º¹±¸ÇØ¾ßÇÑ´Ù.
	//-----------------------------------------------------------------
	DWORD OldDV		= DV;
	DWORD OldPV		= PV;
	DWORD OldTOHIT	= TOHIT;
	
	DV			= g_StatusManager.GetDefense();
	PV			= g_StatusManager.GetProtection();
	TOHIT		= g_StatusManager.GetTOHIT() + weaponTohit;
	//int CC		= g_StatusManager.GetCC();
	//MinDAM		= m_Status[MODIFY_MIN_DAMAGE];	//g_StatusManager.GetMinDAM();
	//MaxDAM		= m_Status[MODIFY_MAX_DAMAGE];	//g_StatusManager.GetMaxDAM();
//	AttackSpeed = g_StatusManager.GetAttackSpeed() + weaponSpeed;
//
//	if((*g_pRankBonusTable)[RANK_BONUS_HAWK_WING].GetStatus() == RankBonusInfo::STATUS_LEARNED)
//		AttackSpeed += (*g_pRankBonusTable)[RANK_BONUS_HAWK_WING].GetPoint();
//
//	if((*g_pRankBonusTable)[RANK_BONUS_CROW_WING].GetStatus() == RankBonusInfo::STATUS_LEARNED)
//		AttackSpeed += (*g_pRankBonusTable)[RANK_BONUS_CROW_WING].GetPoint();
//	
//	if((*g_pRankBonusTable)[RANK_BONUS_SWIFT_ARM].GetStatus() == RankBonusInfo::STATUS_LEARNED)
//		AttackSpeed += (*g_pRankBonusTable)[RANK_BONUS_SWIFT_ARM].GetPoint();


	//-----------------------------------------------------------------
	// ¾ÆÀÌÅÛ¿¡ ÀÇÇØ¼­ ½Ã¾ß°¡ ¹à¾ÆÁö´Â°Å Ã¼Å©.
	//-----------------------------------------------------------------
	SetItemLightSight(0);

	// Pet Bonus
	if( GetPetID() != OBJECTID_NULL)
	{
		MFakeCreature *pFakeCreature = (MFakeCreature *)g_pZone->GetFakeCreature(GetPetID());
		if(pFakeCreature != NULL)
		{
			MPetItem *pPetItem = pFakeCreature->GetPetItem();
			if(pPetItem != NULL)
			{
				ITEMOPTION_TABLE::ITEMOPTION_PART attr = (ITEMOPTION_TABLE::ITEMOPTION_PART)pPetItem->GetEnchantLevel();
				int PlusPoint = pPetItem->GetSilver();

				switch(attr)
				{
				case ITEMOPTION_TABLE::PART_VISION :		SetItemLightSight(GetItemLightSight() + PlusPoint); break;
				}

				AffectItemOption(pPetItem);
			}
		}
	}

	DWORD& ElementalFire	= m_Status[MODIFY_ELEMENTAL_FIRE];
	DWORD& ElementalWater	= m_Status[MODIFY_ELEMENTAL_WATER];
	DWORD& ElementalEarth	= m_Status[MODIFY_ELEMENTAL_EARTH];
	DWORD& ElementalWind	= m_Status[MODIFY_ELEMENTAL_WIND];

	ElementalFire	= 0;
	ElementalWater	= 0;
	ElementalEarth	= 0;
	ElementalWind	= 0;

	CT_Done("CS: pre pGear->SetBegin");
	pGear->SetBegin();
	CT_Done("CS: post SetBegin, entering loop");

	int csGearIter = 0;
	while (pGear->IsNotEnd())
	{
		CT_Done("CS-GL: iter %d pre Get (virtual)", csGearIter);
		const MItem* pItem = pGear->Get();
		CT_Done("CS-GL: iter %d post Get pItem=%p, pre AffectItemOption", csGearIter, (void*)pItem);

		AffectItemOption(pItem);
		CT_Done("CS-GL: iter %d post AffectItemOption, pre Next", csGearIter);

		pGear->Next();
		CT_Done("CS-GL: iter %d post Next", csGearIter);
		csGearIter++;
	}
	CT_Done("CS: post gear loop iters=%d", csGearIter);
	
	// ÇÇÀÇ °è¾à ¿É¼Ç Àû¿ë add by chaya
	for(int i = 0; i < 3; ++i)
	{
		POINT ptContractOfBlood = { 8, i << 1 };

		const MItem* pSlot0 = g_pInventory->GetItem(ptContractOfBlood.x + 1, ptContractOfBlood.y);
		const MItem* pSlot1 = g_pInventory->GetItem(ptContractOfBlood.x + 1, ptContractOfBlood.y + 1);

		if(pSlot0 && pSlot1 &&
		   pSlot0->GetID() == pSlot1->GetID() &&
		   pSlot0->GetItemClass() == ITEM_CLASS_CONTRACT_OF_BLOOD &&
		   pSlot1->GetItemClass() == ITEM_CLASS_CONTRACT_OF_BLOOD)
//		   pItem->GetX() == ptContractOfBlood.x && pItem->GetY() == ptContractOfBlood.y)
			AffectItemOption(pSlot0);
	}

	// 2004, 9, 20, sobeit add start - Á¤·É ·¹º§ ¿Ã·ÁÁÖ´Â °è±Þ½ºÅ³ÀÌ ÀÖ³×..
	if((*g_pRankBonusTable)[RANK_BONUS_SALAMANDERS_KNOWLEDGE].GetStatus() == RankBonusInfo::STATUS_LEARNED)
		ElementalFire ++;
	if((*g_pRankBonusTable)[RANK_BONUS_UNDINES_KNOWLEDGE].GetStatus() == RankBonusInfo::STATUS_LEARNED)
		ElementalWater ++;
	if((*g_pRankBonusTable)[RANK_BONUS_GNOMES_KNOWLEDGE].GetStatus() == RankBonusInfo::STATUS_LEARNED)
		ElementalEarth ++;
	// 2004, 9, 20, sobeit add end
	// Á¤·É ¼öÄ¡ ÁöÁ¤
	SetStatus(MODIFY_ELEMENTAL_FIRE, ElementalFire);
	SetStatus(MODIFY_ELEMENTAL_WATER, ElementalWater);
	SetStatus(MODIFY_ELEMENTAL_EARTH, ElementalEarth);
	SetStatus(MODIFY_ELEMENTAL_WIND, ElementalWind);
	if(IsOusters())
		g_pSkillAvailable->SetAvailableSkills();

	//-----------------------------------------------------------------
	// ¹«±â ¼Óµµ ÁöÁ¤
	//-----------------------------------------------------------------
	SetStatus(MODIFY_ATTACK_SPEED, AttackSpeed);

	//-----------------------------------------------------------------
	// DAM, AC, TOHI, CC option¸¸ Àû¿ë½ÃÅ²´Ù.
	//-----------------------------------------------------------------
	// 2001.9.28 Á¦°Å
	//SetStatus(MODIFY_DEFENSE, DV);
	//SetStatus(MODIFY_PROTECTION, PV);
	//SetStatus(MODIFY_TOHIT, TOHIT);
//	SetStatus(MODIFY_CARRYWEIGHT, CC);	
	
	//-----------------------------------------------------------------
	// DV, PV, TOHIT¸¦ º¹±¸
	//-----------------------------------------------------------------
	DV		= OldDV;
	PV		= OldPV;
	TOHIT	= OldTOHIT;

	//-----------------------------------------------------------------
	// DAM¸¸ µû·Î Ã¼Å©.. ³È³È..
	//-----------------------------------------------------------------	
//	m_Status[MODIFY_MIN_DAMAGE] = MinDAM; 
//	m_Status[MODIFY_MAX_DAMAGE] = MaxDAM; 
	int maxDAM, minDAM;
	int maxSilverDAM = 0, minSilverDAM = 0;

	//-----------------------------------------------------------------
	// SlayerÀÎ °æ¿ì --> ¹«±â damage Àû¿ë
	//-----------------------------------------------------------------
	switch(GetRace())
	{
		case RACE_SLAYER:
		{
			const MItem* pWeapon = g_pSlayerGear->GetItem( MSlayerGear::GEAR_SLAYER_RIGHTHAND );

			if (pWeapon==NULL || !pWeapon->IsAffectStatus())
			{
				minDAM = MinDAM;
				maxDAM = MaxDAM;
			}
			else //if (pWeapon->IsAffectStatus())
			{
				minDAM = MinDAM + pWeapon->GetMinDamage();
				maxDAM = MaxDAM + pWeapon->GetMaxDamage();		

				//-----------------------------------------------------------------
				// ½ÊÀÚ°¡³ª ¸ÞÀÌ½ºÀÎ °æ¿ì  : Àºµ¥¹ÌÁö +10%
				//-----------------------------------------------------------------
				if (pWeapon->GetItemClass()==ITEM_CLASS_MACE
					|| pWeapon->GetItemClass()==ITEM_CLASS_CROSS)
				{
					minSilverDAM = pWeapon->GetMinDamage() / 10;
					maxSilverDAM = pWeapon->GetMaxDamage() / 10;
				}

				//-----------------------------------------------------------------
				// Àºµµ±ÝÀÌ µÇ¾î ÀÖ´Â °æ¿ì : Àºµ¥¹ÌÁö +10%
				// ÃÑÀÎ °æ¿ì ÀºÃÑ¾Ë Ã¼Å©
				//-----------------------------------------------------------------
				if (!pWeapon->IsGunItem() && pWeapon->GetSilver() > 0 ||
					pWeapon->IsGunItem() && g_pCurrentMagazine &&
					g_pCurrentMagazine->IsSilverMagazine())
						
				{
					minSilverDAM += pWeapon->GetMinDamage() / 10;
					maxSilverDAM += pWeapon->GetMaxDamage() / 10;
				}
			}
		}
	break;

	case RACE_VAMPIRE:
		//-----------------------------------------------------------------
		// ¹ìÆÄÀÌ¾î..
		//-----------------------------------------------------------------
		{
			const MItem* pWeapon = g_pVampireGear->GetItem( MVampireGear::GEAR_VAMPIRE_RIGHTHAND );

			if (pWeapon==NULL || !pWeapon->IsAffectStatus())
			{
				minDAM = MinDAM;
				maxDAM = MaxDAM;
			}
			else //if (pWeapon->IsAffectStatus())
			{
				minDAM = MinDAM + pWeapon->GetMinDamage();
				maxDAM = MaxDAM + pWeapon->GetMaxDamage();		

			}
		}
		break;

	case RACE_OUSTERS:
		//-----------------------------------------------------------------
		// ¾Æ¿ì½ºÅÍÁî..
		//-----------------------------------------------------------------
		{
			const MItem* pWeapon = g_pOustersGear->GetItem( MOustersGear::GEAR_OUSTERS_RIGHTHAND );
			
			if (pWeapon==NULL || !pWeapon->IsAffectStatus() || pWeapon->GetItemClass() == ITEM_CLASS_OUSTERS_WRISTLET)
			{
				minDAM = MinDAM;
				maxDAM = MaxDAM;
			}
			else //if (pWeapon->IsAffectStatus())
			{
				minDAM = MinDAM + pWeapon->GetMinDamage();
				maxDAM = MaxDAM + pWeapon->GetMaxDamage();		
				
			}
		}
		break;
	}

	UI_SetCharInfoDAM( maxDAM, minDAM );
	
	g_char_slot_ingame.SILVER_DAM2	= minSilverDAM;
	g_char_slot_ingame.SILVER_DAM	= maxSilverDAM;

	// °ÔÀÓ Áß¿¡´Â ¼­¹ö¿¡¼­ º¸³»ÁÖ±â ¶§¹®¿¡..
	CT_Done("CS: pre UI_SetCharInfo* (g_Mode=%d)", (int)g_Mode);
	if (g_Mode == MODE_WAIT_PCLIST)
	{
		UI_SetCharInfoDefense( DV );
		UI_SetCharInfoProtection( PV );
		UI_SetCharInfoTOHIT( TOHIT );
	}
	else
	{
		UI_SetCharInfoDefense( GetDefense() );
		UI_SetCharInfoProtection( GetProtection() );
		UI_SetCharInfoTOHIT( GetTOHIT() );
	}
	CT_Done("CS: EXIT");
}

//----------------------------------------------------------------------
// Fast Move Position
//----------------------------------------------------------------------
// (m_X, m_Y) --> (x, y)
//----------------------------------------------------------------------
bool
MPlayer::FastMovePosition(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y, bool server)
{	
	if (g_pTopView!=NULL)
	{
		g_pTopView->SetSelectedSectorNULL();
	}

	if (MCreature::FastMovePosition( x, y ))
	{
		if(server == true)
			SetServerPosition( x, y);
		// 2001.8.8  °è¼Ó ÃßÀûÇÏ°Ô ÇØº¸±â À§ÇÑ ÁÖ¼®Ã³¸®
		// ±×·¯³ª.. ¹º°¡ ¹®Á¦°¡ ÀÖ¾î¼­.. ´Ù½Ã... - -;;
		//m_nUsedActionInfo = ACTIONINFO_NULL;
		//m_Action		= (m_MoveAction==ACTION_SLAYER_MOTOR_MOVE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND;
		{
		m_fNextTrace = FLAG_TRACE_NULL;

		m_bKeepTraceCreature = false;
		m_Action		= (m_MoveAction==ACTION_SLAYER_MOTOR_MOVE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND;

		UnSetRepeatAction();
		//*/

		// ´Ù °ËÁõµÆ´Ù°í º»´Ù.
		ResetSendMove();

		// 2001.8.10¿¡ Ãß°¡
		m_nUsedActionInfo = ACTIONINFO_NULL;
		}

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// KnockBackPosition
//----------------------------------------------------------------------
bool	
MPlayer::KnockBackPosition(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y)
{
	if (MCreature::KnockBackPosition( x, y ))
	{
		// 2001.8.8  °è¼Ó ÃßÀûÇÏ°Ô ÇØº¸±â À§ÇÑ ÁÖ¼®Ã³¸®
		// ±×·¯³ª.. ¹º°¡ ¹®Á¦°¡ ÀÖ¾î¼­.. ´Ù½Ã... - -;;
		//m_nUsedActionInfo = ACTIONINFO_NULL;
		//m_Action		= (m_MoveAction==ACTION_SLAYER_MOTOR_MOVE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND;

		///*
		//m_fNextTrace = FLAG_TRACE_NULL;

		//m_bKeepTraceCreature = false;
		//m_Action		= (m_MoveAction==ACTION_SLAYER_MOTOR_MOVE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND;

		//UnSetRepeatAction();
		//*/

		// ´Ù °ËÁõµÆ´Ù°í º»´Ù.
		ResetSendMove();

		// 2001.8.10¿¡ Ãß°¡
		m_nUsedActionInfo = ACTIONINFO_NULL;

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Find Enemy
//----------------------------------------------------------------------
// ±ÙÃ³ÀÇ ÀûÀ» Ã£´Â´Ù.
// ÀûÀÌ ¾ø´Ù¸é OBJECTID_NULLÀ» returnÇÑ´Ù.
//----------------------------------------------------------------------
int		
MPlayer::FindEnemy()
{
	//---------------------------------------------------------------
	// ÇöÀç L, R ButtonÀÌ ´­¸° »óÅÂ·Î LockModeÀÇ Çàµ¿À» ÆÇ´ÜÇÑ´Ù.
	// º°·ç ¾È ±ò²ûÇÏÁö¸¸.. ¤»¤»..
	//---------------------------------------------------------------
	int bBasicAction = g_bLButtonDown;
	int bSpecialAction = g_bRButtonDown;
	int actionDistance = 0;

	//---------------------------------------------------------------
	// ±âº» Çàµ¿
	//-------------------------------------------------------------
	if (bBasicAction)
	{
		actionDistance = m_BasicAttackDistance;		
	}
	//-------------------------------------------------------------
	// Æ¯¼ö Çàµ¿
	//-------------------------------------------------------------
	else if (bSpecialAction)
	{
		// Æ¯¼ö ±â¼úÀÌ ¼³Á¤µÇÁö ¾ÊÀº °æ¿ì
		if (m_nSpecialActionInfo==ACTIONINFO_NULL)
		{
			return NULL;
		}
		actionDistance = GetActionInfoRange( m_nSpecialActionInfo );
	}

	//-------------------------------------------------------------
	// ZoneÀÇ ¸ðµç Creature¸¦ °Ë»öÇÑ´Ù.
	//-------------------------------------------------------------
	int	numCreature = g_pZone->GetCreatureNumber();

	MZone::CREATURE_MAP::const_iterator iCreature = g_pZone->GetCreatureBegin();
		
	int targetCreatureID	= OBJECTID_NULL;
	int targetMinDistance	= 99999;

	for (int i=0; i<numCreature; i++)
	{
		MCreature* pCreature = iCreature->second;

		//--------------------------------------------------
		// NPCµµ ¾Æ´Ï°í, ³»°¡ ¾Æ´Ñ °æ¿ì..
		//--------------------------------------------------
		if (!pCreature->IsNPC() 
			&& pCreature->GetID()!=m_ID)
		{
			 

			int tx = pCreature->GetServerX();
			int ty = pCreature->GetServerY();

			int tdistance = max(abs(tx-m_X), abs(ty-m_Y));

			//--------------------------------------------------
			// ÀûÀÌ°í..
			// Çàµ¿ °¡´É °Å¸®ÀÌ¸é...
			//--------------------------------------------------
			if (g_pObjectSelector->CanAttack(pCreature)
				&& tdistance <= actionDistance)
				//&& g_pObjectSelector->IsAblePKbyZone(pCreature))					
			{
				//*
				//--------------------------------------------------
				// °¡Àå °¡±î¿î ³ð Ä£´Ù. -chyaya
				//--------------------------------------------------
				if(tdistance < targetMinDistance)
				{
					targetMinDistance	= tdistance;
					targetCreatureID	= pCreature->GetID();
				}
				/*/
				//--------------------------------------------------
				// Ã³À½ ¼±ÅÃµÈ Ä³¸¯ÅÍÀÎ °æ¿ì
				//--------------------------------------------------
				if (targetCreatureID==OBJECTID_NULL)
				{
					targetCreatureID = pCreature->GetID();
				}
				//--------------------------------------------------
				// Ã³À½ ¼±ÅÃµÈ Ä³¸¯ÅÍ°¡ ¾Æ´Ñ °æ¿ì
				//--------------------------------------------------
				else
				{
					//--------------------------------------------------
					// ¹Ý¹ÝÀÇ È®·ü·Î ¸ñÇ¥¸¦ ´Ù½Ã ¼³Á¤ÇÑ´Ù.
					//--------------------------------------------------
					if (rand()%2)
					{
						targetCreatureID = pCreature->GetID();	
					}
					//--------------------------------------------------
					// ´Ù½Ã ¼³Á¤µÇÁö ¾ÊÀº °æ¿ì
					// ±âÁ¸¿¡ ¼³Á¤µÈ Ä³¸¯ÅÍ¸¦ ¼±ÅÃÇÑ´Ù.
					//--------------------------------------------------
					else
					{
						return targetCreatureID;
					}
				}
				//*/
			}
		}

		iCreature++;
	}	
	return targetCreatureID;
}

//----------------------------------------------------------------------
// Stop Blood Drain
//----------------------------------------------------------------------
// m_bStopBloodDrainÀÌ¸é ÀÌ°É È£ÃâÇØ¾ß ÇÑ´Ù.
//----------------------------------------------------------------------
void	
MPlayer::StopBloodDrain()
{
	if (m_Action==ACTION_VAMPIRE_DRAIN
		&& IsVampire()
		&& m_ActionCountMax>60)
	{
		if (m_ActionCount>9 && m_ActionCount<60)
		{
			m_ActionCount = 60;

			// delay ´Ù½Ã ¼³Á¤
			//m_DelayTime	= g_CurrentTime + (m_ActionCountMax-m_ActionCount)*60;		
			
			UnSetStopBloodDrain();
//			char TempBuffer[128];
//			sprintf(TempBuffer, "StopBD: D:%d, m:%d, C:%d", m_DelayTime,m_bStopBloodDrain, m_ActionCount);
//			g_pSystemMessage->Add(TempBuffer);

			//DEBUG_ADD_FORMAT("[Player] StopBloodDrain1(%d)", m_bStopBloodDrain);

		}	
	}
	else
	{
		//UnSetStopBloodDrain();
		//DEBUG_ADD_FORMAT("[Player] StopBloodDrain2(%d), action=%d(%d)", m_bStopBloodDrain, m_Action, m_ActionCountMax);
	}
}

//----------------------------------------------------------------------
// Stop Absorb Soul
//----------------------------------------------------------------------
// m_bStopAbsorbSoulÀÌ¸é ÀÌ°É È£ÃâÇØ¾ß ÇÑ´Ù.
//----------------------------------------------------------------------
void	
MPlayer::StopAbsorbSoul()
{
	if (m_Action==ACTION_OUSTERS_DRAIN
		&& IsOusters()
		&& m_ActionCountMax>30)
	{
		if (m_ActionCount>9 && m_ActionCount<30)
		{
			m_ActionCount = 30;
			
			ATTACHEFFECT_LIST::iterator itr = m_listEffect.begin();
			ATTACHEFFECT_LIST::iterator endItr = m_listEffect.end();
			
			while(itr != endItr)
			{
				MAttachEffect*	pEffect = *itr;
				
				if(pEffect->GetEffectSpriteType() == EFFECTSPRITETYPE_ABSORB_SOUL)
				{
					bool bUseEffectSprite = pEffect->IsEffectSprite();
					
					// flagÁ¦°Å
					if (bUseEffectSprite)
					{
						m_bAttachEffect[pEffect->GetEffectSpriteType()] = false;	// flagÁ¦°Å
					}
					
					delete pEffect;
					m_listEffect.erase(itr);
					break;
				}
				itr++;
			}
			
			UnSetStopAbsorbSoul();
		}	
	}
	else
	{
		//UnSetStopBloodDrain();
	}
}

//----------------------------------------------------------------------
// Check In Darkness
//----------------------------------------------------------------------
// darkness°¡ ³ªÅ¸³ª°í/»ç¶óÁö´Â ¼ø°£
//            player°¡ ¿òÁ÷ÀÌ´Â ¼ø°£À» Ã¼Å©ÇÏ¸é µÈ´Ù.
//----------------------------------------------------------------------
void
MPlayer::CheckInDarkness()
{
	const bool eligible =
		g_pZone != NULL &&
		((!IsVampire() &&
		  (!HasEffectStatus(EFFECTSTATUS_LIGHTNESS) ||
		   g_pZone->GetID() == 3001)) ||
		 (g_pZone->GetID() == 3001 && IsVampire())) &&
		!g_pPlayer->HasEffectStatus(EFFECTSTATUS_FLAME_SIGH) &&
		!g_pPlayer->HasEffectStatus(EFFECTSTATUS_GHOST)
#ifdef __METROTECH_TEST__
		&& !g_bLight
#endif
		&& !HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET) &&
		!HasEffectStatus(EFFECTSTATUS_INSTALL_MK1);

	if (eligible) {
		const MSector &sector = g_pZone->GetSector(m_X, m_Y);
		const bool hasDark = sector.HasDarkness();
		const int countBefore = m_DarknessCount;

		if (hasDark) {
			PlaceInDarkness();
		} else {
			PlaceNotInDarkness();
		}

		static DWORD s_LastDarknessCheckTrace = 0;
		static bool s_LastHasDark = false;
		static bool s_LastInDark = false;
		const DWORD now = timeGetTime();
		const bool inDarkAfter = IsInDarkness();
		const bool stateChanged =
			hasDark != s_LastHasDark || inDarkAfter != s_LastInDark ||
			countBefore != m_DarknessCount;

		if (stateChanged || now - s_LastDarknessCheckTrace > 1000) {
			s_LastDarknessCheckTrace = now;
			s_LastHasDark = hasDark;
			s_LastInDark = inDarkAfter;
			CT_BD("[DARKNESS] CheckInDarkness xy=(%d,%d) zone=%d race=%d "
			      "eligible=1 hasDark=%d inDarkBefore=%d inDarkAfter=%d "
			      "countBefore=%d countAfter=%d inc=%d action=%s",
			      (int)m_X, (int)m_Y, g_pZone->GetID(),
			      (int)GetRace(), hasDark ? 1 : 0,
			      countBefore >= 0 ? 1 : 0, inDarkAfter ? 1 : 0, countBefore,
			      m_DarknessCount, m_DarknessCountInc,
			      hasDark ? "PlaceInDarkness" : "PlaceNotInDarkness");
		}
	} else {
		const int countBefore = m_DarknessCount;
		PlaceNotInDarkness();

		static DWORD s_LastDarknessExemptTrace = 0;
		const DWORD now = timeGetTime();
		if (countBefore != m_DarknessCount ||
		    now - s_LastDarknessExemptTrace > 2000) {
			s_LastDarknessExemptTrace = now;
			CT_BD("[DARKNESS] CheckInDarkness xy=(%d,%d) zone=%d race=%d "
			      "eligible=0 -> PlaceNotInDarkness countBefore=%d countAfter=%d",
			      (int)m_X, (int)m_Y, g_pZone ? g_pZone->GetID() : -1,
			      (int)GetRace(), countBefore, m_DarknessCount);
		}
	}
}

void
MPlayer::CheckInGunFiled()
{
	if (g_pZone!=NULL && 
		( !IsSlayer() && g_pZone->GetID() == 3001 || 
		g_pZone->GetID() == 3001 && IsSlayer())
		&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
		)
	{
		const MSector& sector = g_pZone->GetSector( m_X, m_Y );

		if (sector.HasGunFiled())
		{
			PlaceInGunFiled();
		}
		else
		{		
			// darkness´Â »ç¶óÁ³´Ù.
			PlaceNotInGunFiled();
		}
	}
	else
	{
		PlaceNotInGunFiled();
	}
}
//----------------------------------------------------------------------
// Set SpecialActionInfo
//----------------------------------------------------------------------
void			
MPlayer::SetSpecialActionInfo( TYPE_ACTIONINFO n )	
{ 
	
	if (g_pActionInfoTable!=NULL  
		&& n<g_pActionInfoTable->GetMinResultActionInfo())
	{
		m_nSpecialActionInfo = n; 
		
		// ¹Ýº¹ Áß¿¡´Â..
		// ±â¼úÀÇ TargetÀÌ ¹Ù²î´Â °æ¿ì°¡ ÀÖ´Ù.
		if (IsRepeatAction())
		{
			if (m_fTraceBuffer != FLAG_TRACE_NULL
				// ±âº» µ¿ÀÛ Áß¿¡´Â °ü°è¾ø´Ù.
				&& !(m_fTraceBuffer & FLAG_TRACE_CREATURE_BASIC)
				&& !(m_fTraceBuffer & FLAG_TRACE_SECTOR_BASIC)
				&& !(m_fTraceBuffer & FLAG_TRACE_ITEM)
				&& !(m_fTraceBuffer & FLAG_TRACE_INTERACTIONOBJECT)
				&& !(m_fTraceBuffer & FLAG_TRACE_EFFECT)
				)
			{			
				// ±â¼ú »ç¿ëÁßÀÎ °æ¿ì..
				BOOL bOther = ((*g_pActionInfoTable)[n].IsTargetOther());
				BOOL bSelf = ((*g_pActionInfoTable)[n].IsTargetSelf());
				BOOL bZone = ((*g_pActionInfoTable)[n].IsTargetZone());
				BOOL bItem = ((*g_pActionInfoTable)[n].IsTargetItem());

				// Á¤»óÀûÀÎ °æ¿ì...
				if (((m_fTraceBuffer & FLAG_TRACE_SELF) && bSelf)
					|| ((m_fTraceBuffer & FLAG_TRACE_CREATURE_SPECIAL) && bOther)
					|| ((m_fTraceBuffer & FLAG_TRACE_SECTOR_SPECIAL) && bZone)
					|| ((m_fTraceBuffer & FLAG_TRACE_INVENTORY) && bItem)
					)
				{				
				}
				// ºñÁ¤»óÀûÀÎ °æ¿ì.. target ¼³Á¤À» ¹Ù²ã¾ß ÇÑ´Ù.
				else
				{
					if (bOther)
					{
						m_fTraceBuffer = FLAG_TRACE_CREATURE_SPECIAL;
					}
					else if (bSelf)
					{
						m_fTraceBuffer = FLAG_TRACE_SELF;
					}
					else if (bZone)
					{
						m_fTraceBuffer = FLAG_TRACE_SECTOR_SPECIAL;
					}
					else if (bItem)
					{
						m_fTraceBuffer = FLAG_TRACE_INVENTORY;
					}
				}
			}		
		}
	}
	
//	CT_Done("DASfadsf");  
	//if((*g_pActionInfoTable)[m_nSpecialActionInfo].GetComboAttackSkill())
	//	gC_vs_ui.SetSelectedAttackComboSkill(TRUE); 
	//else
	//	gC_vs_ui.SetSelectedAttackComboSkill(); 
	
}

//----------------------------------------------------------------------
// Is In SafeSector
//----------------------------------------------------------------------
BOOL
MPlayer::IsInSafeSector() const
{
	if(g_pWarManager->IsExist(m_pZone->GetID()))
		return FALSE;

	if (m_pZone!=NULL)
	{
		if(m_pZone->GetSector(m_X, m_Y).IsSafeComplete() || g_bZoneSafe)
			return 2;

		if (IsSlayer())
		{
			return (m_pZone->GetSector(m_X, m_Y).IsSafeSlayer())?1:0;
		}
		else
		if (IsVampire())
		{
			return (m_pZone->GetSector(m_X, m_Y).IsSafeVampire())?1:0;
		} else
		if (IsOusters() )
		{
			return (m_pZone->GetSector(m_X, m_Y).IsSafeOusters())?1:0;
		}
	}

	return FALSE;
}

//-------------------------------------------------------------------------
// Soul Chain File
//-------------------------------------------------------------------------
void	
MPlayer::SetCannotTrace()
{
	gC_vs_ui.SetCannotTrace();
	g_pUIDialog->SetLockInputPCTalk();
}

void	MPlayer::SetTimeLightSight(char s)
{
	// Paket BYTE degeri char'a signed tasindiginda (orn. 182 -> -74) LightSight bozulur.
	if (s < 0)
		m_TimeLightSight = 0;
	else
		m_TimeLightSight = s; 
	CalculateLightSight();
}

void	MPlayer::SetItemLightSight(int s)
{
	m_ItemLightSight = s; 
	CalculateLightSight();
}

void	MPlayer::CalculateLightSight()
{
//	CalculateSight();
	m_LightSight = GetTimeLightSight() + GetItemLightSight();

	// [»õ±â¼ú6] observing eye°¡ °É·ÁÀÖÀ¸¸é ½Ã¾ß°¡ ³Ð¾îÁø´Ù.
	if (m_bEffectStatus[EFFECTSTATUS_OBSERVING_EYE])
	{
		m_LightSight +=  1 + (*g_pSkillInfoTable)[SKILL_OBSERVING_EYE].GetLevel() / 50;
	}
	// Pet Bonus
	/*
	if( GetPetID() != OBJECTID_NULL)
	{
		MFakeCreature *pFakeCreature = (MFakeCreature *)g_pZone->GetFakeCreature(GetPetID());
		if(pFakeCreature != NULL)
		{
			MPetItem *pPetItem = pFakeCreature->GetPetItem();
			if(pPetItem != NULL)
			{
				ITEMOPTION_TABLE::ITEMOPTION_PART attr = (ITEMOPTION_TABLE::ITEMOPTION_PART)pPetItem->GetEnchantLevel();
				if(attr == ITEMOPTION_TABLE::PART_VISION)
				{
					m_LightSight += pPetItem->GetSilver();
				}
			}
		}
	}
	*/
	
	if(g_pSkillAvailable->IsEnableSkill(SKILL_HOLYLAND_BLOOD_BIBLE_GREGORI))
	{
		m_LightSight += 5;
	}
	
	if(g_pSkillAvailable->IsEnableSkill(SKILL_SWEEPER_BONUS_12))
	{
		m_LightSight += 5;
	}

	if(IsFlyingCreature())
	{
		m_LightSight += 3;
	}
	
	if (m_LightSight < 0)
		m_LightSight = 0;
	
	// 2004, 6, 24, sobeit add start
	if(HasEffectStatus(EFFECTSTATUS_BLINDNESS))
	{
		//m_LightSight = 1;
		PlaceInDarkness(true);
		PlaceInGunFiled(true);
	}
	if(HasEffectStatus(EFFECTSTATUS_LIGHT) ||
	   HasEffectStatus(EFFECTSTATUS_LIGHTNESS))
	{
		m_LightSight += 6;
	}
}

void	MPlayer::SetUndergroundCreature()
{
	MCreature::SetUndergroundCreature();
	CalculateLightSight();
}

void	MPlayer::SetGroundCreature()
{
	MCreature::SetGroundCreature();
	CalculateLightSight();
}

void	MPlayer::SetFlyingCreature()
{
	MCreature::SetFlyingCreature();
	CalculateLightSight();
}

void	MPlayer::CalculateSight()
{
	// -_----; ½Ã¾ß ¼¼ÆÃ ¤»¤»¤» ¹ö±×°¡ ¸¹¾Æ¼­ °Á ¸ÇµéÀÚ

	//-----------------------------------------------------------------
	// item Âø¿ëÇÑ°Å¿¡ µû¶ó¼­...
	//-----------------------------------------------------------------
	MItemManager* pGear = GetGear();

	//-----------------------------------------------------------------
	// SlayerÀÎ °æ¿ì
	//-----------------------------------------------------------------	
	if(pGear == NULL )
		return;

	SetItemLightSight(0);

	pGear->SetBegin();

	while (pGear->IsNotEnd())
	{
		const MItem* pItem = pGear->Get();

		//----------------------------------------------------------
		// ¼öÄ¡°¡ Àû¿ëµÇ´Â itemÀÎ °æ¿ì¸¸..
		//----------------------------------------------------------
		if (pItem->IsAffectStatus())
		{	
			const std::list<TYPE_ITEM_OPTION>& optionList = pItem->GetItemOptionList();
			std::list<TYPE_ITEM_OPTION>::const_iterator optionListItr = optionList.begin();
			while(optionListItr != optionList.end())
			{				
				ITEMOPTION_INFO& optionInfo = (*g_pItemOptionTable)[*optionListItr];
				//---------------------------------------------------
				// ºÎ°¡ÀûÀÎ Option
				//---------------------------------------------------
				switch ((ITEMOPTION_TABLE::ITEMOPTION_PART)optionInfo.Part)
				{
				case ITEMOPTION_TABLE::PART_VISION :	
					SetItemLightSight(GetItemLightSight() + optionInfo.PlusPoint); 
					break;
				}
				optionListItr++;
			}

			std::list<TYPE_ITEM_OPTION>::const_iterator itr = pItem->GetItemDefaultOptionList().begin();

			while(itr != pItem->GetItemDefaultOptionList().end())
			{
				TYPE_ITEM_OPTION option = *itr;
				ITEMOPTION_INFO& optionInfo = (*g_pItemOptionTable)[option];
				//---------------------------------------------------
				// ºÎ°¡ÀûÀÎ Option
				//---------------------------------------------------
				switch ((ITEMOPTION_TABLE::ITEMOPTION_PART)optionInfo.Part)
				{
				case ITEMOPTION_TABLE::PART_VISION :	
					SetItemLightSight(GetItemLightSight() + optionInfo.PlusPoint); 
					break;
				}
				itr++;
			}
		}
		pGear->Next();
	}
	if( //HasEffectStatus(EFFECTSTATUS_LIGHTNESS) || 
		g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
		|| g_bLight
#endif
		)
	{
		// ÃÖ°­~~
		SetItemLightSight( GetItemLightSight() + 15 );
	} else
	{
		if( HasEffectStatus( EFFECTSTATUS_FLARE ) && IsVampire() || 
			HasEffectStatus( EFFECTSTATUS_YELLOW_POISON ) && IsSlayer() ||
			HasEffectStatus( EFFECTSTATUS_YELLOW_POISON_TO_CREATURE ) )//||
//			HasEffectStatus( EFFECTSTATUS_BLINDNESS ))
			SetItemLightSight( 1 );
	}
}
// 2004, 8, 30, sobeit add start - sweep vice test
void
MPlayer::UpdateSweepVice()
{
	if(GetSweepViewValue())	// ½ºÀ¬ ¹ÙÀÌ½º ÁßÀÏ‹š
	{
		//if(IsDead() || m_nUsedActionInfo != SKILL_SWEEP_VICE_1 || m_SweepViceX == SECTORPOSITION_NULL ||	m_SweepViceY == SECTORPOSITION_NULL	)
		if(IsDead() ||!IsSlayer() || m_SweepViceX == SECTORPOSITION_NULL ||	m_SweepViceY == SECTORPOSITION_NULL	)
		{
			g_pZone->RemoveSwapViceType();
			SetSweepViewValue(0);
			return;
		}
		POINT CurrentPos = g_pTopView->GetSelectedSector(g_x, g_y);
		int TempVal = 0;
		int Distance = max(abs(m_TempSelectPosX - CurrentPos.x),abs(m_TempSelectPosY - CurrentPos.y));

		if(Distance<2)
		{
			TempVal = 1;
			if(GetSweepViewValue()!=TempVal)
			{
				g_pZone->ChangeSwapViceType(m_SweepViceX, m_SweepViceY, EFFECTSPRITETYPE_SWEEP_VICE_PRECASTING_1);
				PlaySound( SOUND_SLAYER_SWEEPVICE_RESIZE );
			}
				
		}
		else if(Distance<3)
		{
			TempVal = 2;
			if(GetSweepViewValue()!=TempVal)
			{
				g_pZone->ChangeSwapViceType(m_SweepViceX, m_SweepViceY, EFFECTSPRITETYPE_SWEEP_VICE_PRECASTING_2);
				PlaySound( SOUND_SLAYER_SWEEPVICE_RESIZE );
			}
		}
		else
		{
			TempVal = 3;
			if(GetSweepViewValue()!=TempVal)
			{
				g_pZone->ChangeSwapViceType(m_SweepViceX, m_SweepViceY, EFFECTSPRITETYPE_SWEEP_VICE_PRECASTING_3);
				PlaySound( SOUND_SLAYER_SWEEPVICE_RESIZE );
			}
		}
		SetSweepViewValue(TempVal);
	}
}

bool 
MPlayer::CheckRbuttonUpSkill()
{
	if(HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
	{
		m_fNextTrace = FLAG_TRACE_NULL;
		UnSetRepeatAction();
		InstallTurretStopAttack();
	}
	if(HasEffectStatus(EFFECTSTATUS_INSTALL_MK1))
	{
		m_fNextTrace = FLAG_TRACE_NULL;
		UnSetRepeatAction();
		//InstallTurretStopAttack();
	}
	if(GetSweepViewValue())
	{
		g_pZone->RemoveSwapViceType();
		m_nUsedActionInfo = SKILL_SWEEP_VICE_1;
		ActionEffect();
		SetSweepViewValue(0);
		m_nUsedActionInfo = SKILL_SWEEP_VICE_1;
		m_TempSelectPosX = SECTORPOSITION_NULL;
		m_TempSelectPosY = SECTORPOSITION_NULL;
		m_SweepViceX = SECTORPOSITION_NULL;
		m_SweepViceY = SECTORPOSITION_NULL;

		return true;
	}
//	else if(IsBurningSol())
//	{
//		return true;
//	}
	return false;
}
// 2004, 8, 30, sobeit add end - sweep vice test



bool MPlayer::UpdateBurningSol()
{
	if(m_bBurningSol)
	{
		if(IsDead()||!IsSlayer())
		{
			SetBurningSol(0);
			StopBurningSol();
			SetStop();
			m_nUsedActionInfo = ACTIONINFO_NULL;
			m_fNextTrace = FLAG_TRACE_NULL;
			UnSetRepeatAction();
		//	m_ActionCount = m_ActionCountMax;
			SetAction(ACTION_DIE);
			return false ;
		}
		else if(HasEffectStatus(EFFECTSTATUS_CURSE_PARALYSIS)
			|| m_MoveDevice == MOVE_DEVICE_RIDE)
		{
			SetBurningSol(0);
			StopBurningSol();
			//SetStop();
			m_nUsedActionInfo = ACTIONINFO_NULL;
			m_fNextTrace = FLAG_TRACE_NULL;
			UnSetRepeatAction();
			RemoveEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_1);
			RemoveEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_2);
			RemoveEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_3);
			RemoveEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_4);
			return false;
		}
		if(m_TempSelectPosX == SECTORPOSITION_NULL || m_TempSelectPosY == SECTORPOSITION_NULL)
		{
			//SetBurningSol(0);
			StopBurningSol();
			m_nUsedActionInfo = ACTIONINFO_NULL;
			m_fNextTrace = FLAG_TRACE_NULL;
//			UnSetRepeatAction();
			//m_ActionCount = m_ActionCountMax;
			SetAction(ACTION_STAND);
			return false;
		}
		int bCheckFrame = 0;
		if(!IsAdvancementClass())
			bCheckFrame = 3;
		else
			bCheckFrame = 7;

		m_bBurningSolCount++;

		//if(g_bRButtonDown)
		if(HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_1)||
			HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_2)||
			HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_3)||
			HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_4))
		{
			if(m_bBurningSolCount>=bCheckFrame)
				m_bBurningSolCount = bCheckFrame;
		}
		POINT  TempPos = g_pTopView->GetSelectedSector(g_x, g_y);
		m_TempSelectPosX = TempPos.x;
		m_TempSelectPosY = TempPos.y;
		m_Direction = MTopView::GetDirectionToPosition(g_pPlayer->GetX(), g_pPlayer->GetY(),m_TempSelectPosX, m_TempSelectPosY);
		ChangeNearDirection();
		//else
		if(!g_bRButtonDown)
		{
			if(m_bBurningSolCount>=bCheckFrame  )
			{
//				POINT  TempPos = g_pTopView->GetSelectedSector(g_x, g_y);
//				m_TempSelectPosX = TempPos.x;
//				m_TempSelectPosY = TempPos.y;
//				m_CurrentDirection = MTopView::GetDirectionToPosition(g_pPlayer->GetX(), g_pPlayer->GetY(),m_TempSelectPosX, m_TempSelectPosY);
//				

				bool ShowLunchEffect = true;
				int TempDistance = GetActionInfoRange(SKILL_BURNING_SOUL_LAUNCH);
				if (max(abs(m_X-m_TempSelectPosX), abs(m_Y-m_TempSelectPosY)) > TempDistance)
				{
					ShowLunchEffect = false;
				}
				
				if(HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_1))
				{
					if(ShowLunchEffect)
						ExecuteActionInfoFromMainNode(SKILL_CLIENT_BURNING_SOL_1,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
								m_TempSelectPosX,m_TempSelectPosY, 0, 10, NULL, false);	
					RemoveEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_1);
				}
				else if(HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_2))
				{
					if(ShowLunchEffect)
						ExecuteActionInfoFromMainNode(SKILL_CLIENT_BURNING_SOL_2,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
								m_TempSelectPosX, m_TempSelectPosY, 0, 10, NULL, false);	
					RemoveEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_2);
				}
				else if(HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_3))
				{
					if(ShowLunchEffect)
						ExecuteActionInfoFromMainNode(SKILL_CLIENT_BURNING_SOL_3,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
								m_TempSelectPosX, m_TempSelectPosY, 0, 10, NULL, false);	
					RemoveEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_3);
				}
				else if(HasEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_4))
				{
					if(ShowLunchEffect)
						ExecuteActionInfoFromMainNode(SKILL_CLIENT_BURNING_SOL_4,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),	g_pPlayer->GetID(),	
								m_TempSelectPosX, m_TempSelectPosY, 0, 10, NULL, false);	
					RemoveEffectStatus(EFFECTSTATUS_BURNING_SOL_CHARGE_4);
				}

				m_nUsedActionInfo = SKILL_BURNING_SOUL_LAUNCH;
				m_fTraceBuffer = FLAG_TRACE_SECTOR_SPECIAL;
				ActionToSendPacket();
				m_nNextUsedActionInfo = ACTIONINFO_NULL;
				//SetBurningSol(0);
				StopBurningSol();
				
				UnSetRepeatAction();
				m_TempSelectPosX = SECTORPOSITION_NULL;
				m_TempSelectPosY = SECTORPOSITION_NULL;
			}
		}
		m_ActionCount = m_bBurningSolCount;
			
		return true;
	#ifdef OUTPUT_DEBUG					
		DEBUG_ADD_FORMAT("[UpdateBurningSol] ActionCount  : %d", m_bBurningSolCount);
	#endif
	}
	return false;
}


bool MPlayer::UpDateInstallTurret()
{
	if(HasEffectStatus(EFFECTSTATUS_INSTALL_TURRET))
	{
		if(!IsNotDelay())
		{
			m_Action = ACTION_STAND;
			//m_CurrentDirection = 2; 
			return true;
		}
		if(IsAlive() && (m_Action == ACTION_STAND))
		{
			if (m_bInstallTurretDirect != m_CurrentDirection)
			{
				if( ((m_bInstallTurretDirect - m_CurrentDirection + 8)%8) < 4 )
				{
					m_bInstallTurretCount = ((m_bInstallTurretCount-1)+16)%16;
				}
				else
				{
					m_bInstallTurretCount = (m_bInstallTurretCount+1)%16;
				}

				if(!(m_bInstallTurretCount&0x01))
					m_bInstallTurretDirect = ((m_bInstallTurretCount /2) +2 )%8;
			}
			return true;
		}

		
		SetInstallTurretDirect(m_CurrentDirection);
		SetInstallTurretCount(((m_CurrentDirection+6)%8)*2);
	}
	return false;
}
bool MPlayer::UpDateInstallMK1()
{
	if(HasEffectStatus(EFFECTSTATUS_INSTALL_MK1))
	{
		//if(!IsNotDelay())
	//	{
		//m_Action = ACTION_STAND;
			//m_CurrentDirection = 2; 
			return false;
	//	}
    //int a=0;
	}
	return false;
} //MK1²âÊÔ
// 2005, 1, 4, sobeit add start
void	
MPlayer::UseWildWolf_Corpse(MItem *pItem)
{
	if(pItem == NULL) return;
	m_TraceDistance = GetActionInfoRange(SKILL_WILD_WOLF);
	if (max(abs(m_X-pItem->GetX()), abs(m_Y-pItem->GetY())) <= m_TraceDistance && g_pSkillAvailable->IsEnableSkill( SKILL_WILD_WOLF ))
	{
		SetAction( ACTION_MAGIC );
		CGSkillToObject _CGSkillToObject;
		_CGSkillToObject.setSkillType( SKILL_WILD_WOLF );
		_CGSkillToObject.setCEffectID( 0 );			
		_CGSkillToObject.setTargetObjectID( pItem->GetID() );
		g_pSocket->sendPacket( &_CGSkillToObject );				
	}
}
// 2005, 1, 4, sobeit add end



// 2005.06.30 combo Skill State  My Creature  ¸¸ Àû¿ë  Sjheon
void	
MPlayer::AddComboEndValue()
{
	m_byComboEndValue  ; 
	m_byComboEndValue  = (m_byComboEndValue & 3) ; ///%  4) ; 
	++m_byComboEndValue ; 
	gC_vs_ui.SetComboCnt(m_byComboEndValue);
}

// 2005.06.30 combo Skill State  My Creature ¸¸ Àû¿ë Sjheon

void
MPlayer::UpdateActionStep()
{
	if(m_ActionCount == m_ActionCountMax - 1)
	{
		m_ActionStep = (++m_ActionStep) % 3;

//		OutputDebugString("UpdateSkillStep()\n");
	}
}

void 
MPlayer::RecordCharName(TYPE_OBJECTID id)
{
	std::ofstream RecordCharName("RecordCharName.txt",std::ios::app);
	RecordCharName << m_pZone->GetCreature(id)->GetName() << std::endl;
	RecordCharName.close();
}

#if __CONTENTS(__FRIEND_ADDITION)
void
MPlayer::RequestModeExec()
{
	if (IsInDarkness())
		return;
	
	MCreature* pCreature = g_pZone->GetCreature( m_TraceID );
	
	if (pCreature==NULL)
		return;
	//if (pCreature->CurPernalShop() == 1)  °ÚÌ¯²»ÄÜ²é¿´×°±¸
	//	return;
	if (pCreature->IsNPC())
		return;

	if (IsRequestMode())
	{
		switch (m_RequestMode)
		{
			//---------------------------------------------------------------
			// Trade ÇÒ·Á´Â °æ¿ì
			//---------------------------------------------------------------
			case REQUEST_TRADE :
			{
				if (IsWaitVerifyNULL() 
					&& g_pTempInformation->GetMode() == TempInformation::MODE_NULL
					&& (IsSlayer() && pCreature->IsSlayer() || 
					IsVampire() && pCreature->IsVampire() || 
					IsOusters() && pCreature->IsOusters())
					)
				{
					// ±³È¯ ½ÅÃ»
					CGTradePrepare _CGTradePrepare;
					_CGTradePrepare.setTargetObjectID( m_TraceID );
					_CGTradePrepare.setCode( CG_TRADE_PREPARE_CODE_REQUEST );

					g_pSocket->sendPacket( &_CGTradePrepare );

					SetWaitVerify( WAIT_VERIFY_TRADE );
					
					// temp informationµµ ¼³Á¤.. (À½..¾à°£ ÇãÁ¢ÇÏ´ç.- -;)
					g_pTempInformation->SetMode(TempInformation::MODE_TRADE_REQUEST);
					g_pTempInformation->Value1 = m_TraceID;

					//-----------------------------------------------------
					// ±³È¯ Ãë¼ÒÇÒ±î?
					//-----------------------------------------------------
					UI_RunExchangeCancel( pCreature->GetName() );
				}				
			}
			break;

			//---------------------------------------------------------------
			// Party ÇÒ·Á´Â °æ¿ì
			//---------------------------------------------------------------
			case REQUEST_PARTY :
			{
				if (IsWaitVerifyNULL() 
					&& g_pTempInformation->GetMode() == TempInformation::MODE_NULL
					&& g_pParty!=NULL
					// ³» ÆÄÆ¼¿øÀÌ ¾Æ´Ñ °æ¿ì¿¡¸¸ ½ÅÃ» °¡´ÉÇÏ´Ù.
					&& !g_pParty->HasMember( pCreature->GetName() )
					&& (IsSlayer() && pCreature->IsSlayer() || IsVampire() && pCreature->IsVampire() || IsOusters() && pCreature->IsOusters())
					&& g_pSystemAvailableManager->IsAvailablePartySystem()
					)
				{
					// ÆÄÆ¼ ½ÅÃ»
					CGPartyInvite _CGPartyInvite;
					_CGPartyInvite.setTargetObjectID( m_TraceID );
					_CGPartyInvite.setCode( CG_PARTY_INVITE_REQUEST );

					g_pSocket->sendPacket( &_CGPartyInvite );

					SetWaitVerify( WAIT_VERIFY_PARTY );

					// temp informationµµ ¼³Á¤.. (À½..¾à°£ ÇãÁ¢ÇÏ´ç.- -;)
					g_pTempInformation->SetMode(TempInformation::MODE_PARTY_REQUEST);
					g_pTempInformation->Value1 = m_TraceID;

					//-----------------------------------------------------
					// ±³È¯ Ãë¼ÒÇÒ±î?
					//-----------------------------------------------------
					UI_RunPartyCancel( pCreature->GetName() );
				}				
			}
			break;

			//---------------------------------------------------------------
			// OtherInfo º¼·Á´Â °æ¿ì
			//---------------------------------------------------------------
			case REQUEST_INFO:
			{
				if (IsWaitVerifyNULL() 
					&& g_pTempInformation->GetMode() == TempInformation::MODE_NULL
					&& (IsSlayer() && pCreature->IsSlayer() || 
					IsVampire() && pCreature->IsVampire() || 
					IsOusters() && pCreature->IsOusters() ||
					GetCreatureType() == CREATURETYPE_SLAYER_OPERATOR ||
					GetCreatureType() == CREATURETYPE_VAMPIRE_OPERATOR ||
					GetCreatureType() == CREATURETYPE_OUSTERS_OPERATOR ||
					GetCompetence() == 0 )
					)
				{
					const char* str = gC_vs_ui.GetInputString();
					if(strlen(str)>0)
					{
						if(str[0] == '*')
						{
							gC_vs_ui.AddInputString(pCreature->GetName());
							break;

						}
					}
					// Á¤º¸ ½ÅÃ»
					CGRequestInfo _CGRequestInfo;
					_CGRequestInfo.setValue( m_TraceID );
					_CGRequestInfo.setCode( CGRequestInfo::REQUEST_CHARACTER_INFO );

					g_pSocket->sendPacket( &_CGRequestInfo );

					MCreature *pCreature = g_pZone->GetCreature(m_TraceID);
					
					if(pCreature != NULL)
					{
						UI_RunOtherInfo(pCreature);

						if (g_pProfileManager!=NULL 
							&& g_pRequestUserManager!=NULL
							&& g_pRequestClientPlayerManager!=NULL)
						{
							g_pProfileManager->RequestProfile(pCreature->GetName());
						}
					}
				}
			}
			break;
		}
		UnSetRequestMode();
	}	
}
#endif //__FRIEND_ADDITION
