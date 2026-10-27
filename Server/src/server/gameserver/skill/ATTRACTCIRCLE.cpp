//////////////////////////////////////////////////////////////////////////////
// Filename    : BikeCrash.cpp
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "ATTRACTCIRCLE.h"
#include "SimpleMeleeSkill.h"
#include "SimpleTileMissileSkill.h"
#include "EffectSimplePassive.h"
#include "Gpackets/GCAddEffect.h"

#include <list>
#include "NewRankGem.h"
SkillResultType ATTRACTCIRCLE::execute(Slayer* pSlayer, ObjectID_t targetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error)
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

	Damage_t Gemdamage = output.Damage;

	if (pSlayer->hasRankGem(NewRankGem::SKILL_ATTRACT_CIRCLE_DAM ) )
	{
		NewRankGem* pRankGemATTRACTD = pSlayer->getRankGem( NewRankGem::SKILL_ATTRACT_CIRCLE_DAM );
		Assert( pRankGemATTRACTD != NULL );

		Gemdamage += pRankGemATTRACTD->getGrade();
	}

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = Gemdamage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_SWORD;
	param.STRMultiplier = 8;
	param.DEXMultiplier = 1;
	param.INTMultiplier = 1;
	param.bMagicHitRoll = false;
	param.bMagicDamage  = false;
	if (pSlayer->hasRankGem(NewRankGem::SKILL_ATTRACT_CIRCLE_CRI ) )
	{
		NewRankGem* pRankGemATTRACTC = pSlayer->getRankGem( NewRankGem::SKILL_ATTRACT_CIRCLE_CRI );
		Assert( pRankGemATTRACTC != NULL );

		param.bAdd          = true;
	}
	else
	{
	param.bAdd          = false;
	}
	param.bExpForTotalDamage = false;

	int diff = 2;
	if (pSlayer->hasRankGem(NewRankGem::SKILL_ATTRACT_CIRCLE_RANGE ) )
	{
		NewRankGem* pRankGemATTRACTR = pSlayer->getRankGem( NewRankGem::SKILL_ATTRACT_CIRCLE_RANGE );
		Assert( pRankGemATTRACTR != NULL );

		diff=3;
	}

	for ( int i=-diff; i<=diff; ++i )
	for ( int j=-diff; j<=diff; ++j )
	{
		param.addMask(i, j, 100);
	}

	SIMPLE_SKILL_OUTPUT result;

	skillResult = g_SimpleTileMissileSkill.execute(pSlayer, X, Y, pSkillSlot, param, result);
      /*  if ( result.bSuccess )
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
				// ÀÌÆåÆ®žŠ »ýŒºÇØŒ­ ºÙÀÎŽÙ.
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
	}*/

	__END_CATCH
	
	return skillResult;
}

ATTRACTCIRCLE g_ATTRACTCIRCLE;


