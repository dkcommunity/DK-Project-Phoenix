//////////////////////////////////////////////////////////////////////////////
// Filename    : AttackCabracam.h 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ATTACK_CABRACAM_H__
#define __SKILL_ATTACK_CABRACAM_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class AttackCabracam;
//////////////////////////////////////////////////////////////////////////////

class AttackCabracam : public SkillHandler 
{
public:
	AttackCabracam() throw() {}
	~AttackCabracam() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "AttackCabracam"; }
	SkillType_t getSkillType() const throw() { return SKILL_ATTACK_CABRACAM; }

	void execute(Monster *pMonster, Creature *pEnemy) throw(Error);
	void execute(Monster *pMonster, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern AttackCabracam g_AttackCabracam;

#endif /*  __SKILL_ATTACK_CABRACAM_H__ */
