//////////////////////////////////////////////////////////////////////////////
// Filename    : BikeCrash.cpp
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "BikeCrash.h"
#include "SimpleMeleeSkill.h"
#include "SimpleTileMissileSkill.h"
#include "EffectSimplePassive.h"
#include "Gpackets/GCAddEffect.h"

#include <list>

SkillResultType BikeCrash::execute(Slayer* pSlayer, ObjectID_t targetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error)
{
	SkillResultType skillResult;
	
	__BEGIN_TRY

	Zone* pZone = pSlayer->getZone();
	Assert(pZone != NULL);
	
	Creature* pTargetCreature = pZone->getCreature(targetObjectID);
	if ( pTargetCreature == NULL )
	{
		executeSkillFailException(pSlayer, getSkillType());
		
		return SKILL_RESULT_FAIL_INVALID_TARGET;
	}

	ZoneCoord_t X = pTargetCreature->getX();
	ZoneCoord_t Y = pTargetCreature->getY();

	SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(getSkillType());
	bool bRangeCheck = verifyDistance(pSlayer, pTargetCreature, pSkillInfo->getRange());

	if ( !bRangeCheck || !pZone->moveFastPC( pSlayer, pSlayer->getX(), pSlayer->getY(), X, Y, getSkillType()) )
	{
		executeSkillFailNormal(pSlayer, getSkillType(), pTargetCreature);
		
		return SKILL_RESULT_FAIL_IS_NOT_IN_RANGE;
	}

	X = pSlayer->getX();
	Y = pSlayer->getY();

	SkillInput input(pSlayer, pSkillSlot);
	SkillOutput output;
	computeOutput(input, output);

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_SWORD;
	param.STRMultiplier = 8;
	param.DEXMultiplier = 1;
	param.INTMultiplier = 1;
	param.bMagicHitRoll = false;
	param.bMagicDamage  = false;
	param.bAdd          = true;
	param.bExpForTotalDamage = false;

	for ( int i=-2; i<=2; ++i )
	for ( int j=-2; j<=2; ++j )
	{
		param.addMask(i, j, 100);
	}

	SIMPLE_SKILL_OUTPUT result;

	skillResult = g_SimpleTileMissileSkill.execute(pSlayer, X, Y, pSkillSlot, param, result);

	if ( result.bSuccess )
	{
		list<Creature*>::iterator itr = result.targetCreatures.begin();
		list<Creature*>::iterator endItr = result.targetCreatures.end();

		for ( ; itr != endItr; ++itr )
		{
			Creature* pTargetCreature = *itr;
			Assert( pTargetCreature != NULL );

			int ratio = max( 20, min( 80, 100 - ( 20 + pTargetCreature->getLevel() - pSlayer->getLevel() ) ) );

			if ( rand() % 100 <= ratio )
			{
				// 이펙트를 생성해서 붙인다.
				EffectSimplePassive* pEffect = new EffectSimplePassive( pTargetCreature );
				pEffect->setEffectClass( Effect::EFFECT_CLASS_LOSE_SIGHT );
				pEffect->setDeadline(15);
				pTargetCreature->setFlag( Effect::EFFECT_CLASS_LOSE_SIGHT );
				pTargetCreature->addEffect( pEffect );

				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID( pTargetCreature->getObjectID() );
				gcAddEffect.setEffectID( pEffect->getEffectClass() );
				gcAddEffect.setDuration( 15 );
				pTargetCreature->getZone()->broadcastPacket( pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect );
			}
		}
	}

	__END_CATCH
	
	return skillResult;
}

BikeCrash g_BikeCrash;

