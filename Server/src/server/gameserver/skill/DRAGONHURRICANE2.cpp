//////////////////////////////////////////////////////////////////////////////
// Filename    : DRAGONHURRICANE2.cpp
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "DRAGONHURRICANE2.h"
#include "SimpleMeleeSkill.h"
#include "EffectDRAGONHURRICANE2.h"

#include "Gpackets/GCAddEffectToTile.h"
#include "NewRankGem.h"

//////////////////////////////////////////////////////////////////////////////
// œœ·¹ÀÌŸî ¿ÀºêÁ§Æ® ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType DRAGONHURRICANE2::execute(Slayer * pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot * pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	SkillResultType skillResult;
	
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	SkillInput input(pSlayer, pSkillSlot);
	SkillOutput output;
	computeOutput(input, output);

	Damage_t Gemdamage = output.Damage;

	if (pSlayer->hasRankGem(NewRankGem::SKILL_DRAGON_HURRICANE_DAM ) )
	{
		NewRankGem* pRankGem = pSlayer->getRankGem( NewRankGem::SKILL_DRAGON_HURRICANE_DAM );
		Assert( pRankGem != NULL );

		Gemdamage += pRankGem->getGrade();
	}

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = Gemdamage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_BLADE;
	param.STRMultiplier = 8;
	param.DEXMultiplier = 1;
	param.INTMultiplier = 1;
	param.bMagicHitRoll = false;
	param.bMagicDamage  = false;
	param.bAdd          = false;

	SIMPLE_SKILL_OUTPUT result;

	skillResult = g_SimpleMeleeSkill.execute(pSlayer, TargetObjectID, pSkillSlot, param, result);

	if (result.bSuccess)
	{
		Zone* pZone = pSlayer->getZone();
		Assert(pZone!=NULL);

		Creature* pCreature = pZone->getCreature( TargetObjectID );

		if (pCreature!=NULL)
		{
			// žóœºÅÍÀÎ °æ¿ìžž delayžŠ Ãß°¡ÇÑŽÙ.
			// playerµéÀº client¿¡Œ­ Ã³ž®ÇÏ°Ô µÇŸîÀÖŽÙ.
			if (pCreature->isMonster())
			{
				Monster* pMonster = dynamic_cast<Monster*>(pCreature);

				// delayŒ³Á€ ( + 1ÃÊ )
				if (!pMonster->isMaster())
				{
					Timeval delay;
					delay.tv_sec  = 1;
					delay.tv_usec = 0;//500000;
					pMonster->addAccuDelay(delay);
				}
			}

			Tile& tile = pZone->getTile( pCreature->getX(), pCreature->getY() );

			if ( tile.canAddEffect() )
			{
				Duration_t effectDuration = output.Duration;

				if (pSlayer->hasRankGem(NewRankGem::SKILL_DRAGON_HURRICANE_DUR ) )
				{
					NewRankGem* pRankGem = pSlayer->getRankGem( NewRankGem::SKILL_DRAGON_HURRICANE_DUR );
					Assert( pRankGem != NULL );

					effectDuration += pRankGem->getGrade();
				}

				EffectDRAGONHURRICANE2* pEffect = new EffectDRAGONHURRICANE2( pZone, pCreature->getX(), pCreature->getY() );
				pEffect->setUserOID( pSlayer->getObjectID() );
				pEffect->setTargetOID( TargetObjectID );
				pEffect->setDamage( output.Range );
				pEffect->setChildDamage( output.Tick );
				pEffect->setDeadline( effectDuration );
				pEffect->setNextTime(10);
				pZone->registerObject( pEffect );
				tile.addEffect( pEffect );
				pZone->addEffect( pEffect );

				GCAddEffectToTile gcAE;
				gcAE.setEffectID( pEffect->getSendEffectClass() );
				gcAE.setObjectID( pEffect->getObjectID() );
				gcAE.setDuration( output.Duration );
				gcAE.setXY( pCreature->getX(), pCreature->getY() );
				pZone->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcAE );
			}
		}
	}
	
	__END_CATCH
	
	return skillResult;
}

DRAGONHURRICANE2 g_DRAGONHURRICANE2;
