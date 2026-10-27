//////////////////////////////////////////////////////////////////////////////
// Filename    : MenegrothTrap1.cpp
// Written by  : rappi76 
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "MenegrothTrap1.h"
#include "SimpleMeleeSkill.h"
#include "SimpleTileMissileSkill.h"


void MenegrothTrap1::execute(Creature *pTargetCreature, int iMinDamage, int iMaxDamage)
	throw(Error)
{
	__BEGIN_TRY

		/*
	ZoneCoord_t X = pTargetCreature->getX();
	ZoneCoord_t Y = pTargetCreature->getY();

	SkillInput input(pTargetCreature);
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
	*/

	//g_SimpleTileMissileSkill.execute(pTargetCreature, X, Y, param, result);

	__END_CATCH
}

MenegrothTrap1 g_MenegrothTrap1;
