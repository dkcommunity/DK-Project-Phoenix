//////////////////////////////////////////////////////////////////////////////
// Filename    : AttackManticoret.cpp
// Written by  : rappi76 
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "AttackManticoret.h"
#include "SimpleMissileSkill.h"

void AttackManticoret::execute(Monster *pMonster, Creature *pEnemy)
	throw(Error)
{
	__BEGIN_TRY

//	cout << "AttackManticoret::execute()" << endl;

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

	param.addMask( 0,  0, 100);

	g_SimpleMissileSkill.execute(pMonster, pEnemy, param, result);

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
}

AttackManticoret g_AttackManticoret;
