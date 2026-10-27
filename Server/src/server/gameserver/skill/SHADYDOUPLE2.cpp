#include "SHADYDOUPLE2.h"
#include "SimpleMissileSkill.h"
#include "RankBonus.h"
#include "SimpleTileMissileSkill.h"

SkillResultType SHADYDOUPLE2::execute(Vampire* pVampire, ObjectID_t TargetObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{

	SkillResultType skillResult;
	
	__BEGIN_TRY

	Zone* pZone = pVampire->getZone();
	Assert(pZone != NULL);
	
	Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
	if ( pTargetCreature == NULL )
	{
		executeSkillFailException(pVampire, getSkillType());
		
		return SKILL_RESULT_FAIL_INVALID_TARGET;
	}

	ZoneCoord_t X = pTargetCreature->getX();
	ZoneCoord_t Y = pTargetCreature->getY();

	SkillInput input(pVampire);
	SkillOutput output;
	computeOutput(input, output);

	if (pVampire->hasRankGem(NewRankGem::SKILL_SHADY_DOUPLE_DAM ) )
	{
		NewRankGem* pRankGemSHADYD = pVampire->getRankGem( NewRankGem::SKILL_SHADY_DOUPLE_DAM );
		Assert( pRankGemSHADYD != NULL );

		output.Damage += pRankGemSHADYD->getGrade();
	}

	if (pVampire->hasRankGem(NewRankGem::SKILL_SHADY_DOUPLE_MCRI ) )
	{
		NewRankGem* pRankGemSHADYMCRI = pVampire->getRankGem( NewRankGem::SKILL_SHADY_DOUPLE_MCRI );
		Assert( pRankGemSHADYMCRI != NULL );

		bool isCri = HitRoll::isMagicCriticalHit(pVampire, 200);
		if(isCri)
		output.Damage += output.Damage*0.5;
	}

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
	param.bAdd          = true;

	for ( int i=-2; i<=2; ++i )
	for ( int j=-2; j<=2; ++j )
	{
		param.addMask(i, j, 100);
	}

	SIMPLE_SKILL_OUTPUT result;

	skillResult = g_SimpleTileMissileSkill.execute(pVampire, X, Y, pVampireSkillSlot, param, result);


	__END_CATCH
	
	return skillResult;
}

SHADYDOUPLE2 g_SHADYDOUPLE2;
