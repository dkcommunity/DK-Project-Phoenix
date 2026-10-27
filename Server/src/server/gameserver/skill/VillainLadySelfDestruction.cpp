//////////////////////////////////////////////////////////////////////////////
// Filename    : VillainLadySelfDestruction.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "VillainLadySelfDestruction.h"
#include "SimpleTileMissileSkill.h"

VillainLadySelfDestruction::VillainLadySelfDestruction() throw()
{
	
}

VillainLadySelfDestruction::~VillainLadySelfDestruction() throw()
{
	
}
	
void VillainLadySelfDestruction::execute(Monster* pMonster, Creature *pEnemy) throw(Error)
{
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

	int offset = 2;

	for ( int i=-offset; i<=offset; ++i )
	{
		for ( int j=-offset; j<=offset; ++j )
		{
			if ( !(i == 0 && j == 0) )
			{
				param.addMask( i, j, 50 );
			}
		}
	}
	
	param.addMask( 0, 0, 100 );

	g_SimpleTileMissileSkill.execute(pMonster, pEnemy->getX(), pEnemy->getY(), param, result );
}
