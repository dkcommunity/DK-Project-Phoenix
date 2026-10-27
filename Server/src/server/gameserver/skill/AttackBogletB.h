//////////////////////////////////////////////////////////////////////////////
// Filename    : AttackBogletB.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ATTACK_BOGLETB_H__
#define __SKILL_ATTACK_BOGLETB_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class AttackBogletB;
//////////////////////////////////////////////////////////////////////////////

class AttackBogletB : public SkillHandler 
{
public:
	AttackBogletB() throw() {}
	~AttackBogletB() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "AttackBogletB"; }
	SkillType_t getSkillType() const throw() { return SKILL_ATTACK_BOGLETB; }

	void execute(Monster *pMonster, Creature *pEnemy) throw(Error);
	void execute(Monster *pMonster, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern AttackBogletB g_AttackBogletB;

#endif /*  __SKILL_ATTACK_BOGLETB_H__ */
