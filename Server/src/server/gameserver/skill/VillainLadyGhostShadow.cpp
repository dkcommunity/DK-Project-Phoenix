//////////////////////////////////////////////////////////////////////////////
// Filename    : VillainLadyGhostShadow.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "VillainLadyGhostShadow.h"
#include "SimpleMissileSkill.h"

VillainLadyGhostShadow::VillainLadyGhostShadow() throw()
{
	
}

VillainLadyGhostShadow::~VillainLadyGhostShadow() throw()
{
	
}
	
void VillainLadyGhostShadow::execute(Monster *pMonster, Creature *pEnemy) throw(Error)
{
	__BEGIN_TRY

	Zone* pZone = pMonster->getZone();
	Assert( pZone != NULL );

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
	param.bAdd = false;

	SIMPLE_SKILL_OUTPUT result;
	
	param.addMask( 0, 0, 100 );

	g_SimpleMissileSkill.execute(pMonster, pEnemy, param, result);
	
	__END_CATCH
}
