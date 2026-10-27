//////////////////////////////////////////////////////////////////////////////
// Filename    : HeavenlySpark.cpp
// Written by  : bigheart
// Description : 20080401
//////////////////////////////////////////////////////////////////////////////

#include "HeavenlySpark.h"
#include "SimpleMeleeSkill.h"
#include "SimpleTileMissileSkill.h"
#include "ZoneUtil.h"
#include "Gpackets/GCAddEffectToTile.h"
#include <list>
#include "NewRankGem.h"

SkillResultType HeavenlySpark::execute(Slayer* pSlayer, ObjectID_t targetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error)
{
	__BEGIN_TRY

	Zone* pZone = pSlayer->getZone();
	Assert(pZone != NULL);
	
	Creature* pTargetCreature = pZone->getCreature(targetObjectID);
	//Assert(pTargetCreature != NULL);

	// NoSuchÁ¦°Å. by sigi. 2002.5.2
	if (pTargetCreature==NULL)
	{
		executeSkillFailException(pSlayer, getSkillType());
		
		return SKILL_RESULT_FAIL_INVALID_TARGET;
	}

	return execute( pSlayer, pTargetCreature->getX(), pTargetCreature->getY(), pSkillSlot, CEffectID );

	__END_CATCH
}

SkillResultType HeavenlySpark::execute(Slayer * pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot * pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

//	cout << "(x,y)=" << X << "," << Y << endl;

	Zone* pZone = pSlayer->getZone();
	Assert(pZone!=NULL);

	SkillInput input(pSlayer, pSkillSlot);
	SkillOutput output;
	computeOutput(input, output);

	Damage_t Gemdamage = output.Damage;

	if (pSlayer->hasRankGem(NewRankGem::SKILL_HEAVENLY_SPARK_DAM ) )
	{
		NewRankGem* pRankGemD = pSlayer->getRankGem( NewRankGem::SKILL_HEAVENLY_SPARK_DAM );
		Assert( pRankGemD != NULL );

		Gemdamage += pRankGemD->getGrade();
	}

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = Gemdamage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_MAX;
	param.STRMultiplier = 8;
	param.DEXMultiplier = 1;
	param.INTMultiplier = 1;
	param.bMagicHitRoll = false;
	param.bMagicDamage  = false;
	param.bAdd          = true;
	param.bExpForTotalDamage = false;

	int diff = 1;
	if (pSlayer->hasRankGem(NewRankGem::SKILL_HEAVENLY_SPARK_RANGE ) )
	{
		NewRankGem* pRankGemR = pSlayer->getRankGem( NewRankGem::SKILL_HEAVENLY_SPARK_RANGE );
		Assert( pRankGemR != NULL );

		diff=2;
	}
	for ( int i=-diff; i<=diff; ++i )
	for ( int j=-diff; j<=diff; ++j )
	{
		param.addMask(i, j, 100);
	}

	SIMPLE_SKILL_OUTPUT result;


	return g_SimpleTileMissileSkill.execute(pSlayer, X, Y, pSkillSlot, param, result);

	__END_CATCH
}

HeavenlySpark g_HeavenlySpark;

