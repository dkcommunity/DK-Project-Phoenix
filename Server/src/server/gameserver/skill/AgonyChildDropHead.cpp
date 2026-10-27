//////////////////////////////////////////////////////////////////////////////
// Filename    : AgonyChildDropHead.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "AgonyChildDropHead.h"

#include "SimpleTileMeleeSkill.h"
#include "MonsterAI.h"

AgonyChildDropHead::AgonyChildDropHead() throw()
{
	
}

AgonyChildDropHead::~AgonyChildDropHead() throw() 
{
	
}

void AgonyChildDropHead::execute(Monster *pMonster, ZoneCoord_t X, ZoneCoord_t Y) throw(Error)
{
	SkillInput input(pMonster);
	SkillOutput output;
	computeOutput(input, output);

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.bMagicHitRoll = true;
	param.bMagicDamage  = true;
	param.bAdd          = false;

	SIMPLE_SKILL_OUTPUT result;

	int offset = 1;

	for ( int i=-offset; i<=offset; ++i )
	{
		for ( int j=-offset; j<=offset; ++j )
		{
			param.addMask( i, j, 100 );
		}
	}

	g_SimpleTileMeleeSkill.execute(
		pMonster, 
		X, Y,
		param, result, 
		0, 
		false
	);
	
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
					pBrain->useSkill(pAffectedCreature, SKILL_DOOM, 33);
				}
			}
		}
	}
}
