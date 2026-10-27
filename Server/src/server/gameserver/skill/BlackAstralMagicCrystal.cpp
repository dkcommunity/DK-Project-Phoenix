//////////////////////////////////////////////////////////////////////////////
// Filename    : BlackAstralMagicCrystal.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "BlackAstralMagicCrystal.h"
#include "SimpleTileMissileSkill.h"
#include "MonsterAI.h"

BlackAstralMagicCrystal::BlackAstralMagicCrystal() throw()
{
	
}

BlackAstralMagicCrystal::~BlackAstralMagicCrystal() throw()
{
	
}

void BlackAstralMagicCrystal::execute(Monster* pMonster, Creature *pEnemy) throw(Error)
{
	__BEGIN_TRY

	Zone* pZone = pMonster->getZone();
	Assert( pZone != NULL );

	ZoneCoord_t targetX, targetY;
	
	targetX = pEnemy->getX();
	targetY = pEnemy->getY();
	
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
			param.addMask( i, j, 100 );
		}
	}

	g_SimpleTileMissileSkill.execute(pMonster, targetX, targetY, param, result);

	if(result.bSuccess)
	{
		SIMPLE_SKILL_OUTPUT::TTargetCreatureList::iterator iter;
		Creature *pAffectedCreature;
		
		iter = result.targetCreatures.begin();
		for( ; iter != result.targetCreatures.end() ; ++iter )
		{
			pAffectedCreature = (*iter);
			
			if ( pAffectedCreature != NULL )
			{
				MonsterAI *pBrain = pMonster->getBrain();
				if ( pBrain != NULL )
				{
					pBrain->useSkill(pAffectedCreature, SKILL_PARALYZE, 33);
				}
			}
		}
	}
	

	__END_CATCH
}
