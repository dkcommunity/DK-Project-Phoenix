//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSkillToObjectOK1Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "crash_trace_tool.h"  // 2026-05-21 CT_ACT: Skill->Action->Effect/Sound trace
#include "GCSkillToObjectOK1.h"
#include "ClientDef.h"
#include "MActionInfoTable.h"
#include "PacketFunction2.h"

// [새기술2]
#include "SkillDef.h"	
#include "MFakeCreature.h"
#include "MTopView.h"

void	SkillShadowDancing(MCreature* pUserCreature, MCreature* pTargetCreature, int skillID);
extern void Add_RocketRuncher(MCreature* UserCreature, MCreature* TargetCreature);
extern void Add_XRL_RocketRuncher(MCreature* UserCreature, MCreature* TargetCreature);
extern void Add_Wild_Wolf(MCreature* UserCreature, MCreature* TargetCreature, bool IsEatCorpse);
extern void Add_FakeCreature_Wild_Wolf_Strike(MCreature* UserCreature, MCreature* TargetCreature);

extern bool g_bPreviousMove;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCSkillToObjectOK1Handler::execute ( GCSkillToObjectOK1 * pPacket , Player * pPlayer )

{
	__BEGIN_TRY

	// 2026-05-21 CT_ACT: skill->action->effect/sound dispatch entry trace.
	// Gun Shot Guidance (ve diger skill'ler) yanlis ses+effect root cause hunt.
	// Doru methodlar: getSkillType (SkillID), getCEffectID, getTargetObjectID.
	CT_ACT("GCSkillToObjectOK1::execute Skill=%d CEffID=%d TgtObj=%d Dur=%d Grade=%d",
	       (int)pPacket->getSkillType(), (int)pPacket->getCEffectID(),
	       (int)pPacket->getTargetObjectID(), (int)pPacket->getDuration(),
	       (int)pPacket->getGrade());

#ifdef __GAME_CLIENT__
		
	//------------------------------------------------------
	// 디버그 메시지 보기
	//------------------------------------------------------
// 	if(g_pUserInformation->attrOperator.GetValue() &&
// 	   g_pOperatorOption->bShowAddEffect)
// 	{	
// 		MCreature*	pTargetCreature = g_pZone->GetCreature(pPacket->getObjectID());
// 		DWORD		delayFrame		= ConvertDurationToFrame( pPacket->getDuration() );
// 
// 		const char* pName;
// 		if(pTargetCreature)	pName = pTargetCreature->GetName();
// 		else				pName = "?";
// 
// 		char szBuf[128];
// 		sprintf(szBuf, "targetName=%s, cid=%d, eid=%d, duration=%d, delayFrame=%d",
// 				pName, pPacket->getObjectID(), pPacket->getEffectID(),
// 				pPacket->getDuration(), delayFrame);
// 
// 		UI_AddChatToHistory(szBuf, "Recv GCAddEffect", 6, RGB(255, 255, 0));
// 	}
		
	//------------------------------------------------------------------
	// Player가 기다리던 skill의 성공유무를 검증받았다.
	//------------------------------------------------------------------
	if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_SKILL_SUCCESS)
	{		
		g_pPlayer->SetWaitVerifyNULL();
	}
	else
	{
		DEBUG_ADD("[Error] Player is not WaitVerifySkillSuccess");
	}

	//------------------------------------------------------------------
	// Player가 Skill을 성공시킨 경우에 날아오는 Packet이므로
	// 결과를 반영시켜야 한다.
	//------------------------------------------------------------------
	// 상태값을 바꾼다.
	//------------------------------------------------------------------
	AffectModifyInfo(g_pPlayer, pPacket);
  
	//------------------------------------------------------------------
	// UI에 보이는 것을 바꿔준다.
	// 비교연산하는거보다 이게 더 빠르지 않을까.. 음.. - -;
	//------------------------------------------------------------------
	//UI_SetHP( g_pPlayer->GetHP(), g_pPlayer->GetMAX_HP() );
	//UI_SetMP( g_pPlayer->GetMP(), g_pPlayer->GetMAX_MP() );

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
		MCreature* pCreature = g_pZone->GetCreature( pPacket->getTargetObjectID() );

		// [ TEST CODE ]
		// 진행중인 Effect이면 Result로 추가해야 한다. 
		// Result를 생성하고..
		// g_pPlayer->>AddActionResult( ... )

		// 2005, 1, 3, sobeit add start
		if(pPacket->getSkillType() == SKILL_WILD_WOLF)
		{
			if(pCreature != NULL)
				Add_Wild_Wolf(g_pPlayer, pCreature, pPacket->getGrade()?true:false);
			else
			{
				MItem* pItem = g_pZone->GetItem(pPacket->getTargetObjectID());
				if(pItem != NULL && pItem->GetItemClass() == ITEM_CLASS_CORPSE)
				{
					// release build mismatch korumasi
					MCorpse* pCorpse = dynamic_cast<MCorpse*>(pItem);
					if (pCorpse != NULL)
					{
						pCreature = pCorpse->GetCreature();
						if(pCreature != NULL)
						{
							Add_Wild_Wolf(g_pPlayer, pCreature, pPacket->getGrade()?true:false);
						}
					}
				}
			}
		}

		// 2005, 1, 3, sobeit add end
		// Creature에게 Damage 입힘
		if (pCreature != NULL)
		{	
			// 자기 자신이 보낸 SkillStep을 구지 세팅해줄 필요 없다.
			//pCreature->SetSkillStep(pPacket->getSkillStep());

			int skillID = pPacket->getSkillType();
			//char cstrOutput[256] ; 
			//wsprintf(cstrOutput , "GCSkillToObjectOK1Handler() %d\n" ,skillID  ) ; 

			//OutputDebugString(cstrOutput) ; 
			
			if( g_pActionInfoTable->GetSize() <= skillID )
			{
				DEBUG_ADD_FORMAT("[Error] Exceed SkillType %d",skillID);
				
				SendBugReport("[Error:GCSTOOK1H] Exceed SkillType %d", skillID );
				return;
			}
			// 2004, 11, 13, sobeit add start 
			if(skillID == SKILL_PLASMA_ROCKET_LAUNCHER)
			{
				Add_RocketRuncher(g_pPlayer, pCreature);
			}
#if __CONTENTS(__SECOND_RARE_SKILL)
			else if(skillID == SKILL_XRL)
			{
				Add_XRL_RocketRuncher(g_pPlayer, pCreature);
			}
#endif //__SECOND_RARE_SKILL
			// 2004, 11, 13, sobeit add end
//			// 2005, 1, 3, sobeit add start
			else if(skillID == SKILL_HEAL_PASS)
			{
				ExecuteActionInfoFromMainNode(SKILL_HEAL_PASS,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),pCreature->GetID()	,	
					pCreature->GetX(), pCreature->GetY(), 0, 20 , NULL, false);
				
			}
			//else if(skillID == SKILL_ATTRACT_SWORD_CRASH_1)
			//{
			//	ExecuteActionInfoFromMainNode(SKILL_CLIENT_ATTRACT_CIRCLE,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),pCreature->GetID()	,	
			//		pCreature->GetX(), pCreature->GetY(), 0, 20 , NULL, false);
			//}
			else if(skillID == SKILL_BLAZE_WALK_2)
			{

				if(g_pPlayer->IsBlazeWalk2Mode())
				{
					g_pTopView->GetSelectedCreature();
					g_pPlayer->SetRepeatAction(); 								
					g_bPreviousMove = false;
				}

			}
			else if(skillID == SKILL_ADV2_WILD_WOLF_STRIKE)
			{
				Add_FakeCreature_Wild_Wolf_Strike(g_pPlayer,pCreature);
			}
//			// 2005, 1, 3, sobeit add end
			// 현재 무기의 적용을 받는 기술이면..
			if ((*g_pActionInfoTable)[skillID].IsAffectCurrentWeaponAction())
			{
				skillID = g_pPlayer->GetBasicActionInfo();	

				if (skillID >= g_pActionInfoTable->GetMinResultActionInfo())
				{
					DEBUG_ADD_FORMAT("[Error] SkillType Error = %d", skillID);
					return;
				}
			}
			
			//int OrigskillID = skillID	;

			if((*g_pActionInfoTable)[skillID].IsUseActionStep() && pPacket->getGrade() > 0)
				skillID = (*g_pActionInfoTable)[skillID].GetActionStep( pPacket->getGrade() - 1);
			
			 // 2005.07.08 Sjheon Combo Add
			/*if((*g_pActionInfoTable)[skillID].GetComboAttackSkill())
			{	
				g_pPlayer->SetCombo(pPacket->getCombo()) ; 
				g_pPlayer->SetCurrentCombo(0) ; 
			}
			else
			{
				if((*g_pActionInfoTable)[skillID].IsUseActionStep() && pPacket->getGrade() > 0)
					skillID = (*g_pActionInfoTable)[skillID].GetActionStep( pPacket->getGrade() - 1);
			}*/	
			// 2005.07.08 Sjheon Combo End

				
			// 스킬에 따른 예외 처리.
			switch( skillID )
			{
			case SKILL_SHADOW_DANCING :
				SkillShadowDancing(g_pPlayer, pCreature, SKILL_SINGLE_BLOW);
				break;
				
			case SKILL_ILLENDUE :
				// 2004, 12, 15, sobeit modify start - 힐직이 인챈 라이트볼 쓰는게 버그.
				//SkillIllendue( g_pPlayer, pCreature, SKILL_LIGHT_BALL);
				SkillIllendue( g_pPlayer, pCreature, MAGIC_CAUSE_SERIOUS_WOUNDS);
				// 2004, 12, 15, sobeit modify end
				break;
//	        case SKILL_BAT_BREAKER :
	//			SkillIllendue( g_pPlayer, pCreature, SKILL_CLIENT_BAT_BREAKER);
			//	ExecuteActionInfoFromMainNode(SKILL_CLIENT_BAT_BREAKER,g_pPlayer->GetX(), g_pPlayer->GetY(), 0,g_pPlayer->GetDirection(),pCreature->GetID()	,	
			//		pCreature->GetX(), pCreature->GetY(), 0, 20 , NULL, false);
	//	       break;
			case SKILL_SPECTOR_INVERSE :
				// 2004, 12, 15, sobeit modify start - 힐직이 인챈 라이트볼 쓰는게 버그.
				//SkillIllendue( g_pPlayer, pCreature, SKILL_LIGHT_BALL);
				SkillSPECTORINVERSE( g_pPlayer, pCreature, SKILL_DUMMY_ACTION_MAGIC);
				// 2004, 12, 15, sobeit modify end
				break;
#if __CONTENTS(__ONIBLA)
			case SKILL_AGONY_CHILD_SHADOW_FOUR :
				SkillIllendue( pCreature, g_pPlayer, SKILL_ATTACK_MELEE);
				break;
			case SKILL_VILLAIN_LADY_GHOST_SHADOW :
				SkillGhostShadow(pCreature, g_pPlayer, MAGIC_RAPID_GLIDING, 8, 2);
				break;
#endif //__ONIBLA
			case SKILL_BLAZE_WALK :
				SkillBlazeWalk( dynamic_cast<MCreature*>(g_pPlayer), pCreature, SKILL_BLAZE_WALK_ATTACK , pPacket->getGrade() );
				break;
			case SKILL_BITE_OF_DEATH :
				pCreature->SetDrainCreatureID( g_pPlayer->GetID() );
				break;
			case SKILL_SET_AFIRE:
				g_pPlayer->AddEffectStatus(EFFECTSTATUS_SET_AFIRE_ATTACK, 20);
				break;
			case SKILL_ADV2_SUDDEN_BLAST:	
		//		g_pPlayer->AddEffectStatus(EFFECTSTATUS_SUDDEN_BLAST, 20);
				g_pPlayer->CreateAttachEffect(EFFECTSPRITETYPE_unknow_2342,10,0);
				break;
			case SKILL_ACCELERATE_CHARGE_1:
				g_pPlayer->CreateAttachEffect(EFFECTSPRITETYPE_unknow_2879,10,0);
				break;

			case SKILL_ACCELERATE_CHARGE_2:
				g_pPlayer->CreateAttachEffect(EFFECTSPRITETYPE_unknow_2892,10,0);
				break;
			case SKILL_ILLUSION_INVERSION:
				SkillIllendue( g_pPlayer, pCreature, SKILL_DUMMY_ACTION_MAGIC, 2);
				break;
			}
			
			MActionResult* pResult = new MActionResult;

			// 맞은 creature가 때린 player의 방향을 바라보기
			//pCreature->SetDirectionToPosition(g_pPlayer->GetX(), g_pPlayer->GetY());

			DWORD delayFrame = ConvertDurationToFrame( pPacket->getDuration() );

			// 결과 표현
			//pCreature->PacketSpecialActionResult( pPacket->getSkillType() + g_ActionInfoTable.GetMinResultActionInfo());
			pResult->Add( new MActionResultNodeActionInfo( 
											skillID,
											g_pPlayer->GetID(), 
											pCreature->GetID(), 
											pCreature->GetX(),
											pCreature->GetY(),
											delayFrame ) 
							);	

			//------------------------------------------------------
			//
			// skill에 결과가 있으면 적용 시킨다.
			//
			//------------------------------------------------------
			MActionResultNode* pActionResultNode = CreateActionResultNode(pCreature, skillID, pPacket->getGrade() );

			//------------------------------------------------------
			// NULL이 아니면 실행
			//------------------------------------------------------
			if (pActionResultNode!=NULL)
			{
				pResult->Add( pActionResultNode );
			}


			//------------------------------------------------------
			// EffectStatus가 있다면 붙인다.
			//------------------------------------------------------
			EFFECTSTATUS es = (*g_pActionInfoTable)[skillID].GetEffectStatus();
			
			if (es!=EFFECTSTATUS_NULL)
			{
				pResult->Add( new MActionResultNodeAddEffectStatus( pCreature->GetID(), 
																			es, 
																			delayFrame ) );
			}
			
			// [ TEST CODE ] 
			// 맞는 결과 damage를 표현해 준다.			
			/*
			MActionResult* pResult = new MActionResult;
			pResult->Add( new MActionResultNodeActionInfo( g_pPlayer->GetID(), g_pPlayer->GetID(), SKILL_ATTACK_MELEE ) );			

			g_pPlayer->PacketAddActionResult(pPacket->getCEffectID(), pResult);
			*/

			// Player의 현재 진행 중인 기술의 결과로 추가시킨다.
			g_pPlayer->PacketAddActionResult( pPacket->getCEffectID(), pResult );		
		}
		
	}
	

#endif

	__END_CATCH
}
