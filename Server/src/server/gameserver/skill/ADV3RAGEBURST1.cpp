//////////////////////////////////////////////////////////////////////////////
// Filename    : RAGEBURST1.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "ADV3RAGEBURST1.h"
#include "SimpleTileMissileSkill.h"
#include "RankBonus.h"
#include "EffectSimplePassive.h"
#include "Gpackets/GCAddEffect.h"

SkillResultType RAGEBURST1::execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
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

		if (pTargetCreature==NULL
			|| !canAttack( pOusters, pTargetCreature )
			)
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

SkillResultType RAGEBURST1::execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error)
{
	SkillResultType skillResult;
	__BEGIN_TRY

	SkillInput input(pOusters, pOustersSkillSlot);
	SkillOutput output;
	computeOutput(input, output);

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_OUSTERS_WRISTLET;
	param.STRMultiplier = 0;
	param.DEXMultiplier = 0;
	param.INTMultiplier = 0;
	param.bMagicHitRoll = true;
	param.bMagicDamage  = true;
	param.bAdd          = false;
	param.Grade			= 0;

	int diff =2;


	for ( int i=-diff; i<=diff; ++i )
	for ( int j=-diff; j<=diff; ++j )
	{
	   param.addMask( i, j, 100 );
	}

	SIMPLE_SKILL_OUTPUT result;

	skillResult = g_SimpleTileMissileSkill.execute(pOusters, X, Y, pOustersSkillSlot, param, result, CEffectID);

	if ( result.bSuccess )
	{
		list<Creature*>::iterator itr = result.targetCreatures.begin();
		list<Creature*>::iterator endItr = result.targetCreatures.end();

		for ( ; itr != endItr; ++itr )
		{
			Creature* pTargetCreature = *itr;
			Assert( pTargetCreature != NULL );

			EffectSimplePassive* pEffect = new EffectSimplePassive( pTargetCreature );
			pEffect->setEffectClass( Effect::EFFECT_CLASS_RAGEBURST1 );
			pEffect->setDeadline(40);
			pTargetCreature->setFlag( Effect::EFFECT_CLASS_RAGEBURST1 );
			pTargetCreature->addEffect( pEffect );

			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID( pTargetCreature->getObjectID() );
			gcAddEffect.setEffectID( pEffect->getEffectClass() );
			gcAddEffect.setDuration( 40 );
			pTargetCreature->getZone()->broadcastPacket( pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect );
		}
	}

	__END_CATCH
	
	return skillResult;
}

RAGEBURST1 g_RAGEBURST1;
