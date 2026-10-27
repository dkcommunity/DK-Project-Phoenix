//////////////////////////////////////////////////////////////////////////////
// Filename    : AgonyChildShadowFour.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "AgonyChildShadowFour.h"
#include "SimpleMissileSkill.h"

AgonyChildShadowFour::AgonyChildShadowFour() throw() 
{
	
}

AgonyChildShadowFour::~AgonyChildShadowFour() throw() 
{
	
}

void AgonyChildShadowFour::execute(Monster* pMonster, Creature* pEnemy) throw(Error)
{
	SkillInput input(pMonster);
	SkillOutput output;
	computeOutput(input, output);

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.bMagicHitRoll = false;
	param.bMagicDamage  = false;
	param.bAdd          = false;

	SIMPLE_SKILL_OUTPUT result;

	int offset = 2;

	for ( int i=-offset; i<=offset; ++i )
	{
		for ( int j=-offset; j<=offset; ++j )
		{
			param.addMask( i, j, 100 );
		}
	}

	
	g_SimpleMissileSkill.execute(pMonster, pEnemy, param, result);

}