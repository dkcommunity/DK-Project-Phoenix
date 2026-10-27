//////////////////////////////////////////////////////////////////////////////
// Filename    : DRACULEBLOOKDYSCARIFY2.cpp
// Written by  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "DRACULEBLOOKDYSCARIFY2.h"
#include "SimpleMissileSkill.h"

DRACULEBLOOKDYSCARIFY2::DRACULEBLOOKDYSCARIFY2() throw()
{
	
}

DRACULEBLOOKDYSCARIFY2::~DRACULEBLOOKDYSCARIFY2() throw()
{
	
}
	
void DRACULEBLOOKDYSCARIFY2::execute(Monster* pMonster, Creature* pEnemy)
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
	param.bMagicHitRoll = false;
	param.bMagicDamage  = false;
	param.bAdd          = false;

	SIMPLE_SKILL_OUTPUT result;

	g_SimpleMissileSkill.execute(pMonster, pEnemy, param, result);

	__END_CATCH
}
