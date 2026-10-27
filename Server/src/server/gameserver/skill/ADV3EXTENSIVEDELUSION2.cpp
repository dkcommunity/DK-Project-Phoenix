#include "ADV3EXTENSIVEDELUSION2.h"
#include "SimpleTileMissileSkill.h"
#include "RankBonus.h"
#include "EffectSimplePassive.h"
#include "Gpackets/GCAddEffect.h"

EXTENSIVEDELUSION2::EXTENSIVEDELUSION2()
	throw()
{
	__BEGIN_TRY

	__END_CATCH
}

SkillResultType EXTENSIVEDELUSION2::execute(Vampire* pVampire, ObjectID_t TargetObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
    throw(Error)
{
    __BEGIN_TRY

	Assert(pVampire != NULL);
	Assert(pVampireSkillSlot != NULL);

    try
    {
		Zone* pZone = pVampire->getZone();
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);

		if (pTargetCreature==NULL)
		{
			executeSkillFailException(pVampire, getSkillType());

			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		return execute(pVampire, pTargetCreature->getX(), pTargetCreature->getY(), pVampireSkillSlot, CEffectID);
    } 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
    }

    __END_CATCH

    return SKILL_RESULT_FAIL_UNKNOWN;
}

SkillResultType EXTENSIVEDELUSION2::execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	SkillResultType skillResult;
	__BEGIN_TRY

	SkillInput input(pVampire);
	SkillOutput output;
	computeOutput(input, output);

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_MAX;
	param.STRMultiplier = 0;
	param.DEXMultiplier = 0;
	param.INTMultiplier = 0;
	param.bMagicHitRoll = true;
	param.bMagicDamage  = true;
	param.bAdd          = false;

	for ( int i=-2; i<=2; ++i )
	for ( int j=-2; j<=2; ++j )
	{
		param.addMask(i, j, 100);
	}

	SIMPLE_SKILL_OUTPUT result;


	skillResult = g_SimpleTileMissileSkill.execute(pVampire, X, Y, pVampireSkillSlot, param, result);

	if ( result.bSuccess )
	{
		list<Creature*>::iterator itr = result.targetCreatures.begin();
		list<Creature*>::iterator endItr = result.targetCreatures.end();

		EffectSimplePassive* pEffectU = new EffectSimplePassive( pVampire );
		pEffectU->setEffectClass( Effect::EFFECT_CLASS_EXTENSIVEDELUSION2_USER );
		pEffectU->setDeadline(100);
		pVampire->setFlag( Effect::EFFECT_CLASS_EXTENSIVEDELUSION2_USER );
		pVampire->addEffect( pEffectU );

		GCAddEffect gcAddEffect;
		gcAddEffect.setObjectID( pVampire->getObjectID() );
		gcAddEffect.setEffectID( pEffectU->getEffectClass() );
		gcAddEffect.setDuration( 100 );
		pVampire->getZone()->broadcastPacket( pVampire->getX(), pVampire->getY(), &gcAddEffect );


		for ( ; itr != endItr; ++itr )
		{
			Creature* pTargetCreature = *itr;
			Assert( pTargetCreature != NULL );

			EffectSimplePassive* pEffect = new EffectSimplePassive( pTargetCreature );
			pEffect->setEffectClass( Effect::EFFECT_CLASS_EXTENSIVEDELUSION2 );
			pEffect->setDeadline(100);
			pTargetCreature->setFlag( Effect::EFFECT_CLASS_EXTENSIVEDELUSION2 );
			pTargetCreature->addEffect( pEffect );

			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID( pTargetCreature->getObjectID() );
			gcAddEffect.setEffectID( pEffect->getEffectClass() );
			gcAddEffect.setDuration( 100 );
			pTargetCreature->getZone()->broadcastPacket( pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect );
		}
	}
	__END_CATCH
	
	return skillResult;
}

void EXTENSIVEDELUSION2::execute(Monster* pMonster, ZoneCoord_t X, ZoneCoord_t Y)
	throw(Error)
{
	__BEGIN_TRY

	SkillInput input(pMonster);
	SkillOutput output;
	computeOutput(input, output);

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_MAX;
	param.STRMultiplier = 0;
	param.DEXMultiplier = 0;
	param.INTMultiplier = 0;
	param.bMagicHitRoll = true;
	param.bMagicDamage  = true;
	param.bAdd          = false;

	SIMPLE_SKILL_OUTPUT result;

	for ( int i=-2; i<=2; ++i )
	for ( int j=-2; j<=2; ++j )
	{
		param.addMask(i, j, 100);
	}


	g_SimpleTileMissileSkill.execute(pMonster, X, Y, param, result);

	__END_CATCH
}

EXTENSIVEDELUSION2 g_EXTENSIVEDELUSION2;
