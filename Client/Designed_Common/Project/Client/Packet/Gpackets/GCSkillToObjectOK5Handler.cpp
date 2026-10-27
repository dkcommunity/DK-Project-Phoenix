//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSkillToObjectOK5Handler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCSkillToObjectOK5.h"
#include "ClientDef.h"
#include "PacketFunction2.h"

// [새기술2]
#include "SkillDef.h"	
void	SkillShadowDancing(MCreature* pUserCreature, MCreature* pTargetCreature, int skillID);
extern void Add_RocketRuncher(MCreature* UserCreature, MCreature* TargetCreature);
extern void Add_XRL_RocketRuncher(MCreature* UserCreature, MCreature* TargetCreature);
extern void Add_Wild_Wolf(MCreature* UserCreature, MCreature* TargetCreature, bool IsEatCorpse);
extern void Add_FakeCreature_Wild_Wolf_Strike(MCreature* UserCreature, MCreature* TargetCreature);

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCSkillToObjectOK5Handler::execute ( GCSkillToObjectOK5 * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
		
#if __GAME_CLIENT__


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
		int skillID = pPacket->getSkillType();

		if( g_pActionInfoTable->GetSize() <= skillID )
		{
			DEBUG_ADD_FORMAT("[Error] Exceed SkillType %d",skillID);
			SendBugReport("[Error:GCSTOOK5H] Exceed SkillType %d", skillID );
			return;
		}


		MCreature* pUserCreature = g_pZone->GetCreature( pPacket->getObjectID() );
		MCreature* pTargetCreature = g_pZone->GetCreature( pPacket->getTargetObjectID() );		
//		if(pTargetCreature == NULL && skillID == SKILL_ABSORB_SOUL && pUserCreature != NULL && pUserCreature->IsOusters())
//		{
//			MItem *pItem = g_pZone->GetItem(pPacket->getTargetObjectID());
//			if(pItem != NULL)
//			{
//				pTargetCreature = ((MCorpse *)pItem)->GetCreature();
//			}
//		}
		// 2005, 1, 3, sobeit add start
		if(pPacket->getSkillType() == SKILL_WILD_WOLF && pUserCreature != NULL)
		{
			if(pTargetCreature != NULL)
				Add_Wild_Wolf(pUserCreature, pTargetCreature, pPacket->getGrade()?true:false);
			else
			{
				MItem* pItem = g_pZone->GetItem(pPacket->getTargetObjectID());
				if(pItem != NULL && pItem->GetItemClass() == ITEM_CLASS_CORPSE)
				{
					// release build mismatch korumasi
					MCorpse* pCorpse = dynamic_cast<MCorpse*>(pItem);
					if (pCorpse != NULL)
					{
						pTargetCreature = pCorpse->GetCreature();
						if(pTargetCreature != NULL)
						{
							Add_Wild_Wolf(pUserCreature, pTargetCreature, pPacket->getGrade()?true:false);
						}
					}
				}
			}
		}
		// 2005, 1, 3, sobeit add end
	
		// Creature에게 Damage 입힘
		if (pUserCreature != NULL && pTargetCreature != NULL)
		{
			pUserCreature->SetActionStep(pPacket->getActionStep());
			
			// 2004, 11, 13, sobeit add start 
			if(skillID == SKILL_PLASMA_ROCKET_LAUNCHER)
			{
				Add_RocketRuncher(pUserCreature, pTargetCreature);
			}
			// 2004, 11, 13, sobeit add end
#if __CONTENTS(__SECOND_RARE_SKILL)
			else if(skillID == SKILL_XRL)
			{
				Add_XRL_RocketRuncher(pUserCreature, pTargetCreature);
			}
#endif //__SECOND_RARE_SKILL		
			else if(skillID == SKILL_ADV2_WILD_WOLF_STRIKE)
			{
				Add_FakeCreature_Wild_Wolf_Strike(pUserCreature,pTargetCreature);
			}
			if ((*g_pActionInfoTable)[skillID].IsAffectCurrentWeaponAction())
			{
				skillID = pUserCreature->GetBasicActionInfo();

				if (skillID >= g_pActionInfoTable->GetMinResultActionInfo())
				{
					DEBUG_ADD_FORMAT("[Error] SkillType Error = %d", skillID);
					return;
				}
			}
			if( (*g_pActionInfoTable)[skillID].IsUseActionStep() && pPacket->getGrade() > 0)
				skillID = (*g_pActionInfoTable)[skillID].GetActionStep(pPacket->getGrade() - 1);

			// [새기술2]
			// ShadowDancing인지 체크하고 임시로(-_-;) DoubleImpacet를 쓴다.
			switch( skillID )
			{
			case SKILL_SHADOW_DANCING :
				SkillShadowDancing(pUserCreature, pTargetCreature, SKILL_SINGLE_BLOW);
				break;

			case SKILL_ILLENDUE :
				// 2004, 12, 15, sobeit modify start - 힐직이 인챈 라이트볼 쓰는게 버그.
				//SkillIllendue( pUserCreature, pTargetCreature, SKILL_LIGHT_BALL );
				SkillIllendue( pUserCreature, pTargetCreature, MAGIC_CAUSE_SERIOUS_WOUNDS );
				// 2004, 12, 15, sobeit modify end
				break;
	  //      case SKILL_BAT_BREAKER :
	//		    SkillIllendue( pUserCreature, pTargetCreature, SKILL_CLIENT_BAT_BREAKER);
	//	       break;
			case SKILL_SPECTOR_INVERSE :
				// 2004, 12, 15, sobeit modify start - 힐직이 인챈 라이트볼 쓰는게 버그.
				//SkillIllendue( pUserCreature, pTargetCreature, SKILL_LIGHT_BALL );
				SkillSPECTORINVERSE( pUserCreature, pTargetCreature,SKILL_DUMMY_ACTION_MAGIC);
				// 2004, 12, 15, sobeit modify end
				break;
#if __CONTENTS(__ONIBLA)
			case SKILL_AGONY_CHILD_SHADOW_FOUR :
				SkillIllendue(pUserCreature, pTargetCreature, SKILL_ATTACK_MELEE);
				break;
			case SKILL_VILLAIN_LADY_GHOST_SHADOW :
				SkillGhostShadow(pUserCreature, pTargetCreature, MAGIC_RAPID_GLIDING, 8, 2);
				break;
#endif //__ONIBLA
			case SKILL_BLAZE_WALK :
				SkillBlazeWalk( pUserCreature, pTargetCreature, SKILL_BLAZE_WALK_ATTACK, pPacket->getGrade()  );
				break;
			case SKILL_SET_AFIRE:
				pUserCreature->AddEffectStatus(EFFECTSTATUS_SET_AFIRE_ATTACK, 20);
				break;
			case SKILL_ADV2_SUDDEN_BLAST:
				//pUserCreature->AddEffectStatus(EFFECTSTATUS_SUDDEN_BLAST, 20);
				pUserCreature->CreateAttachEffect(EFFECTSPRITETYPE_unknow_2342,10,0);
				break;
			case SKILL_ACCELERATE_CHARGE_1:
				pUserCreature->CreateAttachEffect(EFFECTSPRITETYPE_unknow_2879,10,0);
				break;
			case SKILL_ACCELERATE_CHARGE_2:
				pUserCreature->CreateAttachEffect(EFFECTSPRITETYPE_unknow_2892,10,0);
				break;
			case SKILL_ILLUSION_INVERSION:
				SkillIllendue( pUserCreature, pTargetCreature, SKILL_DUMMY_ACTION_MAGIC, 2);
				break;
			}

			// [ TEST CODE ]
			//
			// 결과를 생성&저장해서 보내야 한다.
			//
			// 누군가(target)가 누군가가 사용한 SKill을 맞은 경우..
			// [ TEST CODE ]
			MActionResult* pResult = new MActionResult;

			DWORD delayFrame = ConvertDurationToFrame( pPacket->getDuration() );

			pResult->Add( new MActionResultNodeActionInfo( 
										skillID, 
										pPacket->getObjectID(), 
										pPacket->getTargetObjectID(), 
										pTargetCreature->GetX(),
										pTargetCreature->GetY(),
										delayFrame ) );

			//------------------------------------------------------
			//
			// skill에 결과가 있으면 적용 시킨다.
			//
			//------------------------------------------------------
			MActionResultNode* pActionResultNode = CreateActionResultNode(pTargetCreature, skillID, pPacket->getGrade() );

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
				pResult->Add( new MActionResultNodeAddEffectStatus( pTargetCreature->GetID(),
																		es, 
																		delayFrame ) );
			}


			// 서로 바라보기
			pUserCreature->SetDirectionToPosition(pTargetCreature->GetX(), pTargetCreature->GetY());
			//pTargetCreature->SetDirectionToPosition(pUserCreature->GetX(), pUserCreature->GetY());

			//Duration_t	m_Duration;
			pUserCreature->PacketSpecialActionToOther(
								skillID, /*pPacket->getSkillType(), */
								pPacket->getTargetObjectID(),
								pResult			// 결과
			);		

			if( skillID == SKILL_BITE_OF_DEATH )
				pTargetCreature->SetDrainCreatureID( pPacket->getObjectID() );
		}
	}	

#endif

	__END_CATCH
}
