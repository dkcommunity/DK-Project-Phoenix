//////////////////////////////////////////////////////////////////////////////
// Filename    : Brionac.cpp
// Written by  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Brionac.h"
#include "SimpleTileMissileSkill.h"
#include "RankBonus.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCModifyInformation.h"
#include "Player.h"
#include "EffectBrionac.h"

SkillResultType Brionac::execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pOusters != NULL);
	Assert(pOustersSkillSlot != NULL);

    try
    {
		Zone* pZone = pOusters->getZone();
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//Assert(pTargetCreature != NULL);

		// NoSuchÁ¦°Å. by sigi. 2002.5.2
		if (pTargetCreature==NULL)
		{
			executeSkillFailException(pOusters, getSkillType());

			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		return execute(pOusters, pTargetCreature->getX(), pTargetCreature->getY(), pOustersSkillSlot, CEffectID);
    } 
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType());
    }

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

SkillResultType Brionac::execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	SkillResultType skillResult;
	
	__BEGIN_TRY

	Zone* pZone = pOusters->getZone();
	Assert( pZone != NULL );

	SkillInput input(pOusters, pOustersSkillSlot);
	SkillOutput output;
	computeOutput(input, output);

	Item* pWeapon = pOusters->getWearItem( Ousters::WEAR_RIGHTHAND );
	if ( pWeapon == NULL )
	{
		executeSkillFailException( pOusters, getSkillType() );
		
		return SKILL_RESULT_FAIL_INVALID_ITEM;
	}

	if (pOusters->hasRankGem(NewRankGem::SKILL_BRIONAC_DAM ) )
	{
		NewRankGem* pRankGemBRIONACD = pOusters->getRankGem( NewRankGem::SKILL_BRIONAC_DAM );
		Assert( pRankGemBRIONACD != NULL );

		output.Damage += pRankGemBRIONACD->getGrade();
	}

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;

	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_OUSTERS_CHAKRAM;
	param.STRMultiplier = 0;
	param.DEXMultiplier = 0;
	param.INTMultiplier = 0;
	param.bMagicHitRoll = false;
	param.bMagicDamage  = false;
	param.bAdd = true;

	SIMPLE_SKILL_OUTPUT result;

	int offset = 1;
	param.Grade=4;

	for ( int i=-offset; i<=offset; ++i )
	for ( int j=-offset; j<=offset; ++j )
		param.addMask( i, j, 100 );

	skillResult = g_SimpleTileMissileSkill.execute(pOusters, X, Y, pOustersSkillSlot, param, result, CEffectID );

	list<Creature*>::iterator itr = result.targetCreatures.begin();
	for ( ; itr != result.targetCreatures.end() ; ++itr )
	{
		Creature* pTargetCreature = *itr;
		if ( pTargetCreature->getX() == X && pTargetCreature->getY() == Y )
		{
			GCModifyInformation gcMI, gcAttackerMI;
			Damage_t damage = computeElementalCombatSkill( pOusters, pTargetCreature, gcAttackerMI );
			if ( damage != 0 )
			{
				::setDamage( pTargetCreature, damage, pOusters, SKILL_BRIONAC, &gcMI, &gcAttackerMI );
				if ( pTargetCreature->isPC() ) pTargetCreature->getPlayer()->sendPacket( &gcMI );

				if (pTargetCreature->isDead())
				{
					int exp = computeCreatureExp(pTargetCreature, 70, pOusters);
					shareOustersExp(pOusters, exp, gcAttackerMI);
				}

				pOusters->getPlayer()->sendPacket( &gcAttackerMI );
			}
		}

		if ( !pTargetCreature->isFlag( Effect::EFFECT_CLASS_BRIONAC ) && result.bSuccess )
		{
			int targetLevel = pTargetCreature->getLevel();
			int ratio = 0;

			ratio = max(20, min(80, (int)( pOusters->getLevel() + 60.0 ) - targetLevel ) );
			
			if ( rand() % 100 < ratio )
			{

				EffectBrionac* pEffect = new EffectBrionac( pTargetCreature );
				Assert( pEffect != NULL );

				pEffect->setDamage( 15 ); // 
				pEffect->setNextTime(20);
				pEffect->setCasterID( pOusters->getObjectID() );
				pEffect->setDeadline( output.Duration );

				if ( pTargetCreature->getX() == X && pTargetCreature->getY() == Y )
					pEffect->setSteal(true);

				pTargetCreature->setFlag( Effect::EFFECT_CLASS_BRIONAC );
				pTargetCreature->addEffect( pEffect );

				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID( pTargetCreature->getObjectID() );
				gcAddEffect.setEffectID( pEffect->getSendEffectClass() );
				gcAddEffect.setDuration( output.Duration );

				pZone->broadcastPacket( pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect );
			}
		}
	}

	__END_CATCH
	
	return skillResult;
}

Brionac g_Brionac;
