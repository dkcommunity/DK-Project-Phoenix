//////////////////////////////////////////////////////////////////////////////
// Filename    : BeatHead.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "SPIRALMEGALITH.h"
#include "SimpleMissileSkill.h"
#include "RankBonus.h"

SkillResultType SPIRALMEGALITH::execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	SkillInput input(pOusters, pOustersSkillSlot);
	SkillOutput output;
	computeOutput(input, output);

	if (pOusters->hasRankGem(NewRankGem::SKILL_SPIRAL_MEGALITH_DAM ) )
	{
		NewRankGem* pRankGemMEGALITHDAM = pOusters->getRankGem( NewRankGem::SKILL_SPIRAL_MEGALITH_DAM );
		Assert( pRankGemMEGALITHDAM != NULL );

		output.Damage += pRankGemMEGALITHDAM->getGrade();
	}

	if (pOusters->hasRankGem(NewRankGem::SKILL_SPIRAL_MEGALITH_MCRI ) )
	{
		NewRankGem* pRankGemMEGALITHMCRI = pOusters->getRankGem( NewRankGem::SKILL_SPIRAL_MEGALITH_MCRI );
		Assert( pRankGemMEGALITHMCRI != NULL );

		bool isCri = HitRoll::isMagicCriticalHit(pOusters, 200);

		if(isCri)
		output.Damage += output.Damage*0.5;
	}

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

	param.Grade = 2;

	SIMPLE_SKILL_OUTPUT result;

	return g_SimpleMissileSkill.execute(pOusters, TargetObjectID, pOustersSkillSlot, param, result, CEffectID);

	__END_CATCH
}

SPIRALMEGALITH g_SPIRALMEGALITH;

