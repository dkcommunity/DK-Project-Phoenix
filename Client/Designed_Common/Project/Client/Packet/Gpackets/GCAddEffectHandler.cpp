//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAddEffectHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCAddEffect.h"
#include "ClientDef.h"
#include "PacketFunction.h"
#include "MEffectStatusDef.h"
#include "SkillDef.h"
#include "MTopView.h"

#include "OperatorOption.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAddEffectHandler::execute ( GCAddEffect * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	//------------------------------------------------------
	// 디버그 메시지 보기
	//------------------------------------------------------
	if(g_pUserInformation->attrOperator.GetValue() &&
	   g_pOperatorOption->bShowAddEffect)
	{	
		MCreature*	pTargetCreature = g_pZone->GetCreature(pPacket->getObjectID());
		DWORD		delayFrame		= ConvertDurationToFrame( pPacket->getDuration() );

		const char* pName;
		if(pTargetCreature)	pName = pTargetCreature->GetName();
		else				pName = "?";

		char szBuf[128];
		sprintf(szBuf, "targetName=%s, cid=%d, eid=%d, duration=%d, delayFrame=%d",
				pName, pPacket->getObjectID(), pPacket->getEffectID(),
				pPacket->getDuration(), delayFrame);

		UI_AddChatToHistory(szBuf, "Recv GCAddEffect", 6, RGB(255, 255, 0));
	}

	//------------------------------------------------------
	// Zone이 아직 생성되지 않은 경우
	//------------------------------------------------------
	if (g_pZone==NULL)
	{
		// message
		DEBUG_ADD("[Error] Zone is Not Init.. yet.");				
	}	
	//------------------------------------------------------
	// 정상.. 
	//------------------------------------------------------
	else
	{
		int CreatureID = pPacket->getObjectID();
		
		MCreature* pCreature = g_pZone->GetCreature(CreatureID);

		// 성물보관대 얻어오기 위해서
		if (pCreature == NULL)
		{
			MItem *selectedItem = g_pZone->GetItem(CreatureID);
			if(selectedItem != NULL)
			{
				if(selectedItem->GetItemClass() == ITEM_CLASS_CORPSE)
				{
					MCorpse *pCorpse = (MCorpse *)selectedItem;
					pCreature = pCorpse->GetCreature();
				}
			}
		}

		if (pCreature==NULL)
		{
			DEBUG_ADD_FORMAT("[Error] the Creature is Not Exist. id=%d", pPacket->getObjectID());			
		}
		else
		{
#if __CONTENTS(__QUEST_RENEWAL)
			DWORD status = (EFFECTSTATUS)pPacket->getEffectID();

			if(status	== EFFECTSTATUS_QUEST_FINISH)
			{
				switch(g_pPlayer->GetRace())
				{
				case RACE_SLAYER:
					status	= EFFECTSTATUS_CLIENT_QUEST_FINISH_SLAYER;
					break;
				case RACE_VAMPIRE:
					status	= EFFECTSTATUS_CLIENT_QUEST_FINISH_VAMPIRE;
					break;	
				case RACE_OUSTERS:
					status	= EFFECTSTATUS_CLIENT_QUEST_FINISH_OUSTERS;
					break;
				}
			}
#else
			EFFECTSTATUS status = (EFFECTSTATUS)pPacket->getEffectID();
#endif
			if(status >= EFFECTSTATUS_MAX)
				return;

			// FAZ 4 (2026-05-05): g_pEffectStatusTable bounds-check.
			// EFFECTSTATUS_MAX enum vs runtime table size uyumsuz olabilir
			// (table boş veya kısa init). OOB → garbage SoundID/ActionInfo → crash.
			if (g_pEffectStatusTable == NULL ||
				(int)status >= (int)g_pEffectStatusTable->GetSize())
			{
				DEBUG_ADD_FORMAT("[GCAddEffect] EffectStatus OOB: status=%d table_size=%d",
					(int)status,
					(g_pEffectStatusTable != NULL) ? (int)g_pEffectStatusTable->GetSize() : -1);
				return;
			}

			if( (*g_pEffectStatusTable)[ status ].SoundID != SOUNDID_NULL )
			{
				PlaySound( (*g_pEffectStatusTable)[ status ].SoundID );
			}

			// 임시로...
			// 둠이면 무시..
			// extreme이면 무시...
			if (status == EFFECTSTATUS_BACK_STAB_3		|| status == EFFECTSTATUS_ICE_OF_SOUL_STONE			||
				status == EFFECTSTATUS_TRAP_TRIGGERED	|| status == EFFECTSTATUS_TRAPPED					||
				status == EFFECTSTATUS_TRYING			|| status == EFFECTSTATUS_GROUND_ELEMENTAL_CENTER)
			{
				// 3단계는 관련 actioninfo 가 있다.
				TYPE_ACTIONINFO		ActionInfo = (*g_pEffectStatusTable)[ status ].ActionInfo;
				if( ActionInfo != ACTIONINFO_NULL )
				{
					DWORD delayFrame = 18;
					if( status == EFFECTSTATUS_ICE_OF_SOUL_STONE )
						delayFrame = 19;

					if( status == EFFECTSTATUS_TRYING || status == EFFECTSTATUS_GROUND_ELEMENTAL_CENTER)
						delayFrame = 0xFFFF;

					if( status == EFFECTSTATUS_TRAP_TRIGGERED || status == EFFECTSTATUS_TRAPPED )
						delayFrame = ConvertDurationToFrame( pPacket->getDuration() );
					
					ExecuteActionInfoFromMainNode(
						ActionInfo,						
						pCreature->GetX(), pCreature->GetY(), 0,
						pCreature->GetDirection(),						
						pCreature->GetID(),
						pCreature->GetX(), pCreature->GetY(), 0,						
						delayFrame,						
						NULL, status == EFFECTSTATUS_TRAP_TRIGGERED
						);
				}
			}
			// 2004, 9, 3, sobeit add start
			else
			if(status == EFFECTSTATUS_TURRET_LASER && g_pPlayer) // 공성전 트랩중..공격측에 안보이는 광선
			{
				if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_1) || 
					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_2) || 
					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_3) || 
					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_4) || 
					g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_5) )
					return;
			}
			// 2004, 9, 3, sobeit add end
			if (status != EFFECTSTATUS_CURSE_DOOM				&&
				status != EFFECTSTATUS_CAUSE_CRITICAL_WOUNDS	&&
				status != EFFECTSTATUS_BACK_STAB_3
				//&& status!=EFFECTSTATUS_EXTREME
				)
			{
				// delay계산
				DWORD delayFrame = ConvertDurationToFrame( pPacket->getDuration() );

				// EffectStatus를 추가한다.
#if __CONTENTS(__QUEST_RENEWAL) 
				pCreature->AddEffectStatus((EFFECTSTATUS)status, delayFrame);
#else
				pCreature->AddEffectStatus(status, delayFrame);
#endif
			}
			
			switch( status )
			{
			case EFFECTSTATUS_SLAYER_TRYING_1:                   // 320
			case EFFECTSTATUS_SLAYER_TRYING_2:                   // 321
			case EFFECTSTATUS_SLAYER_TRYING_3:                   // 322
			case EFFECTSTATUS_VAMPIRE_TRYING_1:                  // 323
			case EFFECTSTATUS_VAMPIRE_TRYING_2:                  // 324
			case EFFECTSTATUS_VAMPIRE_TRYING_3:                  // 325
			case EFFECTSTATUS_OUSTERS_TRYING_1:                  // 326
			case EFFECTSTATUS_OUSTERS_TRYING_2:                  // 327
			case EFFECTSTATUS_OUSTERS_TRYING_3:                  // 328
				{
					ExecuteActionInfoFromMainNode(
						REGEN_TOWER_FLASH_BEAD,
						pCreature->GetX(), pCreature->GetY(), 0,
						DIRECTION_DOWN,
						pCreature->GetID(),
						pCreature->GetX(), pCreature->GetY(), 0,
						7,
						NULL );
						
				}
				break;
			}
		}
	}

#endif

	__END_CATCH
}
