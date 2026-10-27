//////////////////////////////////////////////////////////////////////////////
// Filename    : AttackMassacre.h 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ATTACK_MASSACRE_H__
#define __SKILL_ATTACK_MASSACRE_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class AttackMassacre;
//////////////////////////////////////////////////////////////////////////////

class AttackMassacre : public SkillHandler 
{
public:
	AttackMassacre() throw() {}
	~AttackMassacre() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "AttackMassacre"; }
	SkillType_t getSkillType() const throw() { return SKILL_ATTACK_MASSACRE; }

	void execute(Monster *pMonster, Creature *pEnemy) throw(Error);
	void execute(Monster *pMonster, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern AttackMassacre g_AttackMassacre;

#endif /*  __SKILL_ATTACK_MASSACRE_H__ */
