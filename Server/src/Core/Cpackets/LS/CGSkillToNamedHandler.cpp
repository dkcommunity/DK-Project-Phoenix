//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSkillToNamedHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGSkillToNamed.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "SkillHandlerManager.h"
	#include "ZoneUtil.h"

	#include "skill/Sniping.h"

#endif	// __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSkillToNamedHandler::execute (CGSkillToNamed* pPacket , Player* pPlayer)
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

		if ( !pZone->NotifyUseNamedSkill(pCreature, SkillType) )
		{
             executeSkillFailException(pCreature, SkillType);
             
             return;
		}
		
		disableFlags( pCreature, pZone, SkillType);
		
		if (pCreature->isSlayer()) 
		{
			Slayer*    pSlayer    = dynamic_cast<Slayer*>(pCreature);
			SlayerSkillSlot* pSkillSlot = ((Slayer *)pCreature)->hasSkill(SkillType);
			bool bSuccess   = true;

			if (pSkillSlot == NULL) 
			{
				bSuccess = false;
			}
			
			if ( bSuccess && !isAbleToUseSelfSkill(pSlayer, SkillType)) 
			{
				bSuccess = false;
			}
			
			if ( bSuccess && !pGamePlayer->verifySkillSpeed(pPacket->GetTimeStamp(), SkillType) )
   	       	{
   	           	bSuccess = false;
			}

/*			if (pSlayer->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE))
			{
				g_Sniping.checkRevealRatio(pSlayer, 20, 10);
			} */

			if (bSuccess) 
			{
				SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SkillType);
				Assert(pSkillHandler != NULL);
				
				SkillResultType skillResult;
				
				skillResult = pSkillHandler->execute(pSlayer, pPacket->getTargetName(), pSkillSlot, pPacket->getCEffectID());
				
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
			bool bSuccess   = true;
			
			//cout << "SkillType:" << (int)SkillType << endl;

/*			if (pVampire->isFlag(Effect::EFFECT_CLASS_EXTREME))
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
				bSuccess = false;
			}
			
			if ( bSuccess && !isAbleToUseSelfSkill(pVampire, SkillType)) 
			{
				bSuccess = false;
			}

			if ( bSuccess && !pGamePlayer->verifySkillSpeed(pPacket->GetTimeStamp(), SkillType) )
   	       	{
   	           	bSuccess = false;
			}

/*			if (pVampire->isFlag(Effect::EFFECT_CLASS_INVISIBILITY))
			{
				addVisibleCreature(pZone, pVampire, true);
			} */

			if (bSuccess) 
			{
				SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SkillType);
				Assert(pSkillHandler != NULL);
				
				SkillResultType skillResult;
				
				skillResult = pSkillHandler->execute(pVampire, pPacket->getTargetName(), pVampireSkillSlot, pPacket->getCEffectID());
				
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
			bool bSuccess   = true;
			
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
				bSuccess = false;
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
				
				skillResult = pSkillHandler->execute(pOusters, pPacket->getTargetName(), pOustersSkillSlot, pPacket->getCEffectID());
				
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

