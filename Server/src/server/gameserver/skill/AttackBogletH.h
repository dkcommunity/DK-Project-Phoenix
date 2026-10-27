//////////////////////////////////////////////////////////////////////////////
// Filename    : AttackBogletH.h 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ATTACK_BOGLETH_H__
#define __SKILL_ATTACK_BOGLETH_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class AttackBogletH;
//////////////////////////////////////////////////////////////////////////////

class AttackBogletH : public SkillHandler 
{
public:
	AttackBogletH() throw() {}
	~AttackBogletH() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "AttackBogletH"; }
	SkillType_t getSkillType() const throw() { return SKILL_ATTACK_BOGLETH; }

	void execute(Monster *pMonster, Creature *pEnemy) throw(Error);
	void execute(Monster *pMonster, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern AttackBogletH g_AttackBogletH;

#endif /*  __SKILL_ATTACK_BOGLETH_H__ */
