//////////////////////////////////////////////////////////////////////////////
// Filename    : MenegrothTrap.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_MENEGROTH_TRAP_2_H__
#define __SKILL_MENEGROTH_TRAP_2_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class MenegrothTrap;
//////////////////////////////////////////////////////////////////////////////

class MenegrothTrap2 : public SkillHandler 
{
public:
	MenegrothTrap2() throw() {}
	~MenegrothTrap2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "MenegrothTrap2"; }
	SkillType_t getSkillType() const throw() { return SKILL_MENEGROTH_TRAP_2; }

	void execute(Creature *pTargetCreature, int iMinDamage, int iMaxDamage) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern MenegrothTrap2 g_MenegrothTrap2;

#endif /*  __SKILL_CABRACAM_ATTACK_H__ */
