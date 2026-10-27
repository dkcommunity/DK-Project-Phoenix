//////////////////////////////////////////////////////////////////////////////
// Filename    : AttackMassacre.cpp
// Written by  : rappi76 
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "AttackMassacre.h"
#include "SimpleMeleeSkill.h"
#include "SimpleTileMissileSkill.h"

void AttackMassacre::execute(Monster *pMonster, ZoneCoord_t X, ZoneCoord_t Y)
	throw(Error)
{
	__BEGIN_TRY

//	cout << "AttackMassacre::execute()" << endl;

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

	param.addMask( 0,  0, 100);
	param.addMask(-1, -1, 100);
	param.addMask( 0, -1, 100);
	param.addMask( 1, -1, 100);
	param.addMask(-1,  0, 100);
	param.addMask( 1,  0, 100);
	param.addMask(-1,  1, 100);
	param.addMask( 0,  1, 100);
	param.addMask( 1,  1, 100);

	g_SimpleTileMissileSkill.execute(pMonster, X, Y, param, result);

	__END_CATCH
}

void AttackMassacre::execute(Monster *pMonster, Creature *pEnemy)
	throw(Error)
{
	__BEGIN_TRY

//	cout << "AttackMassacre::execute()" << endl;

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;
	Zone* pZone = pMonster->getZone();
	Assert(pZone != NULL);

	// NoSuchÁ¦°Å. by sigi. 2002.5.2
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

AttackMassacre g_AttackMassacre;
