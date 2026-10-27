//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSkillToSelfHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGSkillToSelf.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "SkillHandlerManager.h"
	#include "ZoneUtil.h"
	#include "RelicUtil.h"

	#include "skill/Sniping.h"

	//#define __PROFILE_SKILLS__

	#ifdef __PROFILE_SKILLS__
		#include "Profile.h"
	#endif
#endif	// __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSkillToSelfHandler::execute (CGSkillToSelf* pPacket , Player* pPlayer)
	 throw (Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	try 
	{
		GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
		Assert(pGamePlayer != NULL);	// by sigi

		if (pGamePlayer->getPlayerStatus() != GPS_NORMAL) return;

		Creature* pCreature = pGamePlayer->getCreature();
		Assert(pCreature != NULL);	// by sigi

		Zone* pZone = pCreature->getZone();
		Assert(pZone != NULL);

		SkillType_t SkillType = pPacket->getSkillType();
	
		// 완전 안전지대라면 기술 사용 불가. by sigi. 2002.11.14
		ZoneLevel_t ZoneLevel = pZone->getZoneLevel(pCreature->getX(), pCreature->getY());
		if (ZoneLevel & COMPLETE_SAFE_ZONE)
		{
			executeSkillFailException(pCreature, SkillType);

			return;
		}
	
		if ( !pZone->NotifyUseSelfSkill(pCreature, SkillType) )
		{
             executeSkillFailException(pCreature, SkillType);
             
             return;
		}
		
		disableFlags( pCreature, pZone, SkillType);

		if (pCreature->isSlayer()) 
		{
			Slayer*    pSlayer    = dynamic_cast<Slayer*>(pCreature);
			SlayerSkillSlot* pSkillSlot = ((Slayer *)pCreature)->hasSkill(SkillType);
			bool       bSuccess   = true;

			if (pSkillSlot == NULL) 
			{
				if ( SkillType == SKILL_UN_TRANSFORM )
				{
					bSuccess = true;
				}
				else if ( ( SkillType < SKILL_HOLY_SWORD || SkillType > SKILL_PARTY_AURA )  && SkillType != SKILL_REQUEST_RESURRECT )
				{
					// 폭주 스킬인 경우 걍 쓸 수 있도록 한다.
					bSuccess = false;
				}
			}

			if ( bSuccess && !isAbleToUseSelfSkill(pSlayer, SkillType)) 
			{
				bSuccess = false;
			}
			
/*			if (pSlayer->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE))
			{
				g_Sniping.checkRevealRatio(pSlayer, 20, 10);
			} */

			if ( bSuccess && !pGamePlayer->verifySkillSpeed(pPacket->GetTimeStamp(), SkillType) )
   	       	{
   	           	bSuccess = false;
			}
			
			if (bSuccess) 
			{
				SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SkillType);
				Assert(pSkillHandler != NULL);
				
				SkillResultType skillResult;

				#ifdef __PROFILE_SKILLS__
					beginProfileEx(SkillTypes2String[SkillType]);
					skillResult = pSkillHandler->execute(pSlayer, pSkillSlot, pPacket->getCEffectID());
					endProfileEx(SkillTypes2String[SkillType]);
				#else
					skillResult = pSkillHandler->execute(pSlayer, pSkillSlot, pPacket->getCEffectID());
				#endif
					
				if ( skillResult != SKILL_RESULT_SUCCESS )
				{
					pGamePlayer->rollbackSkillSpeed(SkillType);	
				}
			 }
			 else
			 {
				 executeSkillFailException(pSlayer, SkillType);
			 }
		} 
		else if (pCreature->isVampire()) 
		{
			Vampire*          pVampire          = dynamic_cast<Vampire*>(pCreature);
			VampireSkillSlot* pVampireSkillSlot = ((Vampire *)pCreature)->hasSkill(SkillType);
			bool              bSuccess          = true;

			if ( (SkillType == SKILL_TRANSFORM_TO_BAT || SkillType == SKILL_TRANSFORM_TO_WOLF)  && 
				(!isAbleToUseSelfSkill(pVampire) || pVampire->isFlag( Effect::EFFECT_CLASS_HAS_FLAG ) || 
				pVampire->isFlag( Effect::EFFECT_CLASS_HAS_SWEEPER) )) //20071228
			{
				cout << "성물을 가진 상태에서는 변신할 수 없습니다" << endl;

				executeSkillFailException(pVampire, SkillType);
				
				return;
			}

			if (SkillType == SKILL_UN_INVISIBILITY && pVampire->isFlag(Effect::EFFECT_CLASS_INVISIBILITY))
			{
				Effect* pInvi = pVampire->findEffect( Effect::EFFECT_CLASS_INVISIBILITY );
				if ( pInvi ) pInvi->setDeadline(0);

				executeSkillFailException(pVampire, SkillType);

//				addVisibleCreature(pZone, pVampire, true);
				return;
			}
/*			if (SkillType == SKILL_HOWL && pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF)) 
			{
				SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SKILL_HOWL);
				Assert(pSkillHandler != NULL);
				pSkillHandler->execute(pVampire, pVampireSkillSlot, pPacket->getCEffectID());
				return;
			} */

/*            if (pVampire->isFlag(Effect::EFFECT_CLASS_EXTREME))
		    {
		   		EffectManager * pEffectManager = pVampire->getEffectManager();
		    	Assert( pEffectManager != NULL );
		    	Effect * pEffect = pEffectManager->findEffect( Effect::EFFECT_CLASS_EXTREME );
		    	if ( pEffect != NULL ) {
		    		pEffect->setDeadline(0);
		   	 	}
            } */

			if (pVampireSkillSlot == NULL) 
			{
				if ( SkillType == SKILL_UN_TRANSFORM || SkillType == SKILL_OPEN_CASKET )
				{
					bSuccess = true;
				}
				else if ( ( SkillType < SKILL_HOLY_SWORD || SkillType > SKILL_PARTY_AURA )  && SkillType != SKILL_REQUEST_RESURRECT )
				{
					// 폭주 스킬인 경우 걍 쓸 수 있도록 한다.
					
					bSuccess = false;
				}
			}

			if ( bSuccess && !isAbleToUseSelfSkill(pVampire, SkillType)) 
			{
				bSuccess = false;
			}

/*			if (pVampire->isFlag(Effect::EFFECT_CLASS_INVISIBILITY))
			{
				addVisibleCreature(pZone, pVampire, true);
			} */

			if ( bSuccess && !pGamePlayer->verifySkillSpeed(pPacket->GetTimeStamp(), SkillType) )
   	       	{
   	           	bSuccess = false;
			}
			
			if (bSuccess) 
			{
				SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SkillType);
				Assert(pSkillHandler != NULL);

				SkillResultType skillResult;
				
				#ifdef __PROFILE_SKILLS__
					beginProfileEx(SkillTypes2String[SkillType]);
					skillResult = pSkillHandler->execute(pVampire, pVampireSkillSlot, pPacket->getCEffectID());
					endProfileEx(SkillTypes2String[SkillType]);
				#else
					skillResult = pSkillHandler->execute(pVampire, pVampireSkillSlot, pPacket->getCEffectID());
				#endif
					
				if ( skillResult != SKILL_RESULT_SUCCESS )
				{
					pGamePlayer->rollbackSkillSpeed(SkillType);	
				}
			}
			else
			{
				executeSkillFailException(pVampire, SkillType);
			}
		} 
		else if (pCreature->isOusters()) 
		{
			Ousters*          pOusters          = dynamic_cast<Ousters*>(pCreature);
			OustersSkillSlot* pOustersSkillSlot = ((Ousters *)pCreature)->hasSkill(SkillType);
			bool              bSuccess          = true;

			if (pOusters->isFlag(Effect::EFFECT_CLASS_SQUALLY_BARRIER1))
		    {
		   		EffectManager * pEffectManager = pOusters->getEffectManager();
		    	Assert( pEffectManager != NULL );
		    	Effect * pEffect = pEffectManager->findEffect( Effect::EFFECT_CLASS_SQUALLY_BARRIER1 );
		    	if ( pEffect != NULL )
		    	{
		    		pEffect->unaffect();
		    		pOusters->removeFlag( Effect::EFFECT_CLASS_SQUALLY_BARRIER1 );
		    		pOusters->deleteEffect( Effect::EFFECT_CLASS_SQUALLY_BARRIER1 );
		    	}
		    }
			
			if (pOusters->isFlag(Effect::EFFECT_CLASS_SQUALLY_BARRIER2))
		    {
		   		EffectManager * pEffectManager = pOusters->getEffectManager();
		    	Assert( pEffectManager != NULL );
		    	Effect * pEffect = pEffectManager->findEffect( Effect::EFFECT_CLASS_SQUALLY_BARRIER2 );
		    	if ( pEffect != NULL )
		    	{
		    		pEffect->unaffect();
		    		pOusters->removeFlag( Effect::EFFECT_CLASS_SQUALLY_BARRIER2 );
		    		pOusters->deleteEffect( Effect::EFFECT_CLASS_SQUALLY_BARRIER2 );
		    	}
		    }

			if (pOustersSkillSlot == NULL) 
			{
				if ( SkillType == SKILL_UN_TRANSFORM )
				{
					bSuccess = true;
				}
				else if ( (SkillType < SKILL_HOLY_SWORD || SkillType > SKILL_PARTY_AURA) && SkillType != SKILL_REQUEST_RESURRECT )
				{
					// 폭주 스킬인 경우 걍 쓸 수 있도록 한다.
					
					bSuccess = false;
				}
			}

			if ( bSuccess && !isAbleToUseSelfSkill(pOusters, SkillType))
			{
				bSuccess = false;
			}
			
			if ( bSuccess && !pGamePlayer->verifySkillSpeed(pPacket->GetTimeStamp(), SkillType) )
   	       	{
   	           	bSuccess = false;
			}

			if (bSuccess) 
			{
				SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SkillType);
				Assert(pSkillHandler != NULL);
				
				SkillResultType skillResult;

				#ifdef __PROFILE_SKILLS__
					beginProfileEx(SkillTypes2String[SkillType]);
					skillResult = pSkillHandler->execute(pOusters, pOustersSkillSlot, pPacket->getCEffectID());
					endProfileEx(SkillTypes2String[SkillType]);
				#else
					skillResult = pSkillHandler->execute(pOusters, pOustersSkillSlot, pPacket->getCEffectID());
				#endif
					
				if ( skillResult != SKILL_RESULT_SUCCESS )
				{
					pGamePlayer->rollbackSkillSpeed(SkillType);	
				}
			}
			else
			{
				executeSkillFailException(pOusters, SkillType);
			}
		} 
	} 
	catch (Throwable & t) 
	{
		//cout << t.toString() << endl;
	}

#endif	// __GAME_SERVER__
		
	__END_DEBUG_EX __END_CATCH
}

