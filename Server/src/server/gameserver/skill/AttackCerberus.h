//////////////////////////////////////////////////////////////////////////////
// Filename    : AttackCerberus.h 
// Written By  : rappi76 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ATTACK_CERBERUS_H__
#define __SKILL_ATTACK_CERBERUS_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class AttackCerberus;
//////////////////////////////////////////////////////////////////////////////

class AttackCerberus : public SkillHandler 
{
public:
	AttackCerberus() throw() {}
	~AttackCerberus() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "AttackCerberus"; }
	SkillType_t getSkillType() const throw() { return SKILL_ATTACK_CERBERUS; }

	void execute(Monster *pMonster, Creature *pEnemy) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern AttackCerberus g_AttackCerberus;

#endif /*  __SKILL_ATTACK_CERBERUS_H__ */
