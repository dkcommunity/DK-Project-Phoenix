#include "BONESTRIKE.h"
#include "SimpleMissileSkill.h"

void BONESTRIKE::execute(Monster *pMonster, Creature *pEnemy)
	throw(Error)
{
	__BEGIN_TRY

	Zone* pZone = pMonster->getZone();
	Assert(pZone != NULL);

	if (pEnemy == NULL)
	{
		return;
	}

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
/*	int diff =2;
	for ( int i=-diff; i<=diff; ++i )
	{
		for ( int j=-diff; j<=diff; ++j )
		{
			param.addMask( i, j, 100 );
		}
	}*/
	param.addMask( 0,  0, 100);

	g_SimpleMissileSkill.execute(pMonster, pEnemy, param, result);


	__END_CATCH
}


BONESTRIKE g_BONESTRIKE;
