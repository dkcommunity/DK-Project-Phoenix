//////////////////////////////////////////////////////////////////////////////
// Filename    : AttackCabracam.cpp
// Written by  : rappi76 
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "AttackCabracam.h"
#include "SimpleMeleeSkill.h"
#include "SimpleTileMissileSkill.h"

void AttackCabracam::execute(Monster *pMonster, ZoneCoord_t X, ZoneCoord_t Y)
	throw(Error)
{
	__BEGIN_TRY

//	cout << "AttackCabracam::execute()" << endl;

	SkillInput input(pMonster);
	SkillOutput output;
	computeOutput(input, output);

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.bMagicHitRoll = false;
	param.bMagicDamage  = false;
	param.bAdd          = true;

	SIMPLE_SKILL_OUTPUT result;

	// Attack Range 5x5
	for(int y=-2;y<=2;++y)
	{
		for(int x=-2;x<=2;++x)
		{
			param.addMask( x,  y, 100);
		}
	}

	g_SimpleTileMissileSkill.execute(pMonster, X, Y, param, result);

	__END_CATCH
}

void AttackCabracam::execute(Monster *pMonster, Creature *pEnemy)
	throw(Error)
{
	__BEGIN_TRY

//	cout << "AttackCabracam::execute()" << endl;

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;
	Zone* pZone = pMonster->getZone();
	Assert(pZone != NULL);

	if (pEnemy == NULL)
	{
		return;
	}

	ZoneCoord_t X = pEnemy->getX();
	ZoneCoord_t Y = pEnemy->getY();

	execute(pMonster, X, Y);

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
}

AttackCabracam g_AttackCabracam;
