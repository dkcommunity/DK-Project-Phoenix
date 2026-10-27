//////////////////////////////////////////////////////////////////////////////
// Filename    : AttackPlumpyBoar.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ATTACK_PLUMPYBOAR_H__
#define __SKILL_ATTACK_PLUMPYBOAR_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class AttackPlumpyBoar;
//////////////////////////////////////////////////////////////////////////////

class AttackPlumpyBoar : public SkillHandler 
{
public:
	AttackPlumpyBoar() throw() {}
	~AttackPlumpyBoar() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "AttackPlumpyBoar"; }
	SkillType_t getSkillType() const throw() { return SKILL_ATTACK_PLUMPYBOAR; }

	void execute(Monster *pMonster, Creature *pEnemy) throw(Error);
	void execute(Monster *pMonster, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern AttackPlumpyBoar g_AttackPlumpyBoar;

#endif /* __SKILL_ATTACK_PLUMPYBOAR_H__ */
