//////////////////////////////////////////////////////////////////////////////
// Filename    : AttackManticoret.h 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ATTACK_MANTICORET_H__
#define __SKILL_ATTACK_MANTICORET_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class AttackManticoret;
//////////////////////////////////////////////////////////////////////////////

class AttackManticoret : public SkillHandler 
{
public:
	AttackManticoret() throw() {}
	~AttackManticoret() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "AttackManticoret"; }
	SkillType_t getSkillType() const throw() { return SKILL_ATTACK_MANTICORET; }

	void execute(Monster *pMonster, Creature *pEnemy) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern AttackManticoret g_AttackManticoret;

#endif /*  __SKILL_ATTACK_MANTICORET_H__ */
