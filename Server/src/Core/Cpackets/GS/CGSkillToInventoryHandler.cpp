//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSkillToInventoryHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGSkillToInventory.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "SkillHandlerManager.h"
	#include "ZoneUtil.h"

	#include "skill/Sniping.h"

#endif	// __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSkillToInventoryHandler::execute (CGSkillToInventory* pPacket , Player* pPlayer)
	 throw (Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert(pGamePlayer != NULL);	// by sigi

	if (pGamePlayer->getPlayerStatus() == GPS_NORMAL) 
	{
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
		
		if ( !pZone->NotifyUseInventorySkill(pCreature, SkillType) )
		{
             executeSkillFailException(pCreature, SkillType);
             
             return;
		}
		
		BYTE        X         = pPacket->getX();
		BYTE        Y         = pPacket->getY();
		BYTE        TX        = pPacket->getTargetX();
		BYTE        TY        = pPacket->getTargetY();

		disableFlags( pCreature, pZone, SkillType);
		
		if (pCreature->isSlayer()) 
		{
			Slayer*    pSlayer    = dynamic_cast<Slayer*>(pCreature);
			SlayerSkillSlot* pSkillSlot = pSlayer->hasSkill(SkillType);
			bool       bSuccess   = true;

			if (pSkillSlot == NULL) bSuccess = false;
			if ( SkillType == SKILL_INSTALL_MINE ) {
				bSuccess = true;
				TY = 0;
			} else {
				if (!isAbleToUseInventorySkill(pSlayer, X, Y, TX, TY)) bSuccess = false;
			}
/*			if (pSlayer->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE))
			{
				g_Sniping.checkRevealRatio(pSlayer, 20, 10);
			} */
						
			if (bSuccess)
			{
				SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SkillType);
				Assert(pSkillHandler != NULL);
   			 	pSkillHandler->execute(pSlayer, pPacket->getObjectID(), pPacket->getInventoryItemObjectID(), X, Y, TX, TY, pSkillSlot);
   			 }
             else
             {
                 executeSkillFailException(pSlayer, SkillType);
             }
		} 
		else if (pCreature->isVampire()) 
		{
			Vampire*          pVampire          = dynamic_cast<Vampire*>(pCreature);
			VampireSkillSlot* pVampireSkillSlot = pVampire->hasSkill(SkillType);
			bool              bSuccess          = true;
			
			if (pVampireSkillSlot == NULL) bSuccess = false;
			if (!isAbleToUseInventorySkill(pVampire, X, Y, TX, TY)) bSuccess = false;

/*			if (pVampire->isFlag(Effect::EFFECT_CLASS_INVISIBILITY))
			{
				addVisibleCreature(pZone, pVampire, true);
			}
		
	        if (pVampire->isFlag(Effect::EFFECT_CLASS_EXTREME))
	        {
		        EffectManager * pEffectManager = pVampire->getEffectManager();
	   		    Assert( pEffectManager != NULL );
	   		    Effect * pEffect = pEffectManager->findEffect( Effect::EFFECT_CLASS_EXTREME );
	   		    if ( pEffect != NULL ) {
		   		    pEffect->setDeadline(0);
		        }
			} */
			
			if (bSuccess) 
			{
				SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SkillType);
				Assert(pSkillHandler != NULL);
   			 	pSkillHandler->execute(pVampire, pPacket->getObjectID(), pPacket->getInventoryItemObjectID(), X, Y, TX, TY, pVampireSkillSlot);
   			 }
             else
             {
                 executeSkillFailException(pVampire, SkillType);
             }
		}
		else if (pCreature->isOusters()) 
		{
			Ousters*          pOusters          = dynamic_cast<Ousters*>(pCreature);
			OustersSkillSlot* pOustersSkillSlot = pOusters->hasSkill(SkillType);
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
			
			if (pOustersSkillSlot == NULL) bSuccess = false;
			if (!isAbleToUseInventorySkill(pOusters, X, Y, TX, TY)) bSuccess = false;
			
			if (bSuccess) 
			{
				SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SkillType);
				Assert(pSkillHandler != NULL);
   			 	pSkillHandler->execute(pOusters, pPacket->getObjectID(), pPacket->getInventoryItemObjectID(), X, Y, TX, TY, pOustersSkillSlot);
   			 }
             else
             {
                 executeSkillFailException(pOusters, SkillType);
             }
		}
	}

#endif	// __GAME_SERVER__
		
	__END_DEBUG_EX __END_CATCH
}

