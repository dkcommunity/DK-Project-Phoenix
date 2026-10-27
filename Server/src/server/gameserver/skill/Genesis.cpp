//////////////////////////////////////////////////////////////////////////////
// Filename    : Genesis.cpp
// Written by  : bigheart
// Description : 20080407
//////////////////////////////////////////////////////////////////////////////

#include "Genesis.h"
#include "SimpleMeleeSkill.h"
#include "SimpleTileMissileSkill.h"
#include "RankBonus.h"
#include "NewRankGem.h"

//////////////////////////////////////////////////////////////////////////////
// �����̾� ������Ʈ �ڵ鷯
//////////////////////////////////////////////////////////////////////////////
SkillResultType Genesis::execute(Slayer* pSlayer, ObjectID_t targetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error)
{
	__BEGIN_TRY

	Zone* pZone = pSlayer->getZone();
	Assert(pZone != NULL);
	
	Creature* pTargetCreature = pZone->getCreature(targetObjectID);
	//Assert(pTargetCreature != NULL);

	// NoSuch����. by sigi. 2002.5.2
	if (pTargetCreature==NULL)
	{
		executeSkillFailException(pSlayer, getSkillType());
		
		return SKILL_RESULT_FAIL_INVALID_TARGET;
	}

	return execute( pSlayer, pTargetCreature->getX(), pTargetCreature->getY(), pSkillSlot, CEffectID );

	__END_CATCH
}
	
SkillResultType Genesis::execute(Slayer * pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot * pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{

	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayer)" << endl;
	try {
		
	SkillInput input(pSlayer, pSkillSlot);
	SkillOutput output;
	computeOutput(input, output);

	if (pSlayer->hasRankGem(NewRankGem::SKILL_GENESIS_DAM ) )
	{
		NewRankGem* pRankGemGENESISD = pSlayer->getRankGem( NewRankGem::SKILL_GENESIS_DAM );
		Assert( pRankGemGENESISD != NULL );

		output.Damage += pRankGemGENESISD->getGrade();
	}

	if (pSlayer->hasRankGem(NewRankGem::SKILL_GENESIS_MCRI ) )
	{
		NewRankGem* pRankGemGENESISDMCRI = pSlayer->getRankGem( NewRankGem::SKILL_GENESIS_MCRI );
		Assert( pRankGemGENESISDMCRI != NULL );

		bool isCri = HitRoll::isMagicCriticalHit(pSlayer, 200);
		if(isCri)
		output.Damage += output.Damage*0.5;
	}

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_MAX;
	param.STRMultiplier = 1;
	param.DEXMultiplier = 1;
	param.INTMultiplier = 8;
	param.bMagicHitRoll = true;
	param.bMagicDamage  = true;
	param.bAdd          = false;

	int diff =2;

	if (pSlayer->hasRankGem(NewRankGem::SKILL_GENESIS_RANGE ) )
	{
		NewRankGem* pRankGemGENESISRANGE = pSlayer->getRankGem( NewRankGem::SKILL_GENESIS_RANGE );
		Assert( pRankGemGENESISRANGE != NULL );

		diff=3;
	}

	for ( int i=-diff; i<=diff; ++i )
	for ( int j=-diff; j<=diff; ++j )
	{
		param.addMask(i, j, 100);
	}


	SIMPLE_SKILL_OUTPUT result;


	return g_SimpleTileMissileSkill.execute(pSlayer, X, Y, pSkillSlot, param, result);

	} catch ( Throwable & t ) {
		executeSkillFailException(pSlayer, getSkillType());
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

Genesis g_Genesis;


