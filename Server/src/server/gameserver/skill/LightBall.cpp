//////////////////////////////////////////////////////////////////////////////
// Filename    : LightBall.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "LightBall.h"
#include "SimpleMissileSkill.h"
#include "RankBonus.h"

//////////////////////////////////////////////////////////////////////////////
// �����̾� ������Ʈ �ڵ鷯
//////////////////////////////////////////////////////////////////////////////
SkillResultType LightBall::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot * pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayer)" << endl;

	SkillInput input(pSlayer, pSkillSlot);
	SkillOutput output;
	computeOutput(input, output);

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_MAX;
	param.STRMultiplier = 1;
	param.DEXMultiplier = 1;
	param.INTMultiplier = 8;
	param.bMagicHitRoll = true;
	param.bMagicDamage  = false;
	param.bAdd          = false;

	SIMPLE_SKILL_OUTPUT result;

	// Holy Smashing �� �ִٸ� ������ ����
/*	if ( pSlayer->hasRankBonus( RankBonus::RANK_BONUS_HOLY_SMASHING ) )
	{
		RankBonus* pRankBonus = pSlayer->getRankBonus( RankBonus::RANK_BONUS_HOLY_SMASHING );
		Assert( pRankBonus != NULL );

		param.SkillDamage += pRankBonus->getPoint();
	}*/

	return g_SimpleMissileSkill.execute(pSlayer, TargetObjectID, pSkillSlot, param, result);

	__END_CATCH
}

LightBall g_LightBall;
