#include "BLOODPUNISH.h"
#include "SimpleMeleeSkill.h"
#include "SimpleTileMissileSkill.h"

void BLOODPUNISH::execute(Monster *pMonster, ZoneCoord_t X, ZoneCoord_t Y)
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
	param.bMagicHitRoll = true;
	param.bMagicDamage  = true;
	param.bAdd          = false;

	SIMPLE_SKILL_OUTPUT result;


	int diff =2;
	for ( int i=-diff; i<=diff; ++i )
	{
		for ( int j=-diff; j<=diff; ++j )
		{
			param.addMask( i, j, 100 );
		}
	}

	g_SimpleTileMissileSkill.execute(pMonster, X, Y, param, result);

	__END_CATCH
}

void BLOODPUNISH::execute(Monster *pMonster, Creature *pEnemy)
	throw(Error)
{
	__BEGIN_TRY

	Zone* pZone = pMonster->getZone();
	Assert(pZone != NULL);

	if (pEnemy == NULL)
	{
		return;
	}

	ZoneCoord_t X = pEnemy->getX();
	ZoneCoord_t Y = pEnemy->getY();

	execute(pMonster, X, Y);


	__END_CATCH
}

BLOODPUNISH g_BLOODPUNISH;
