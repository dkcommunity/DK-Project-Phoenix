//////////////////////////////////////////////////////////////////////////////
// Filename    : MenegrothTrap.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_MENEGROTH_TRAP_1_H__
#define __SKILL_MENEGROTH_TRAP_1_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class MenegrothTrap;
//////////////////////////////////////////////////////////////////////////////

class MenegrothTrap1 : public SkillHandler 
{
public:
	MenegrothTrap1() throw() {}
	~MenegrothTrap1() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "MenegrothTrap1"; }
	SkillType_t getSkillType() const throw() { return SKILL_MENEGROTH_TRAP_1; }

	void execute(Creature *pTargetCreature, int iMinDamage, int iMaxDamage) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern MenegrothTrap1 g_MenegrothTrap1;

#endif /*  __SKILL_CABRACAM_ATTACK_H__ */
