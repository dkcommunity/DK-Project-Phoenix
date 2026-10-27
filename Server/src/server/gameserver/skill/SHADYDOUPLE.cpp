//////////////////////////////////////////////////////////////////////////////
// Filename    : AcidBolt.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "SHADYDOUPLE.h"
#include "SimpleMissileSkill.h"
#include "RankBonus.h"

//////////////////////////////////////////////////////////////////////////////
// �����̾� ������Ʈ �ڵ鷯
//////////////////////////////////////////////////////////////////////////////
SkillResultType SHADYDOUPLE::execute(Vampire* pVampire, ObjectID_t TargetObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(vampire)" << endl;

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
	param.bAdd          = false;

	SIMPLE_SKILL_OUTPUT result;

	// Knowledge of Acid �� �ִٸ� hit bonus 10
	int HitBonus = 0;
/*	if ( pVampire->hasRankBonus( RankBonus::RANK_BONUS_KNOWLEDGE_OF_ACID ) )
	{
		RankBonus* pRankBonus = pVampire->getRankBonus( RankBonus::RANK_BONUS_KNOWLEDGE_OF_ACID );
		Assert( pRankBonus != NULL );

		HitBonus = pRankBonus->getPoint();
	}*/

	return g_SimpleMissileSkill.execute(pVampire, TargetObjectID, pVampireSkillSlot, param, result, CEffectID, HitBonus);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ���� ������Ʈ �ڵ鷯
//////////////////////////////////////////////////////////////////////////////
void SHADYDOUPLE::execute(Monster* pMonster, Creature* pEnemy)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(monster)" << endl;

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

	g_SimpleMissileSkill.execute(pMonster, pEnemy, param, result);

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(monster)" << endl;

	__END_CATCH
}

SHADYDOUPLE g_SHADYDOUPLE;
