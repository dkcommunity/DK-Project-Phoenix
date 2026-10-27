//////////////////////////////////////////////////////////////////////////////
// Filename    : AttackMassacre.h 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLOOD_PUNISH_H__
#define __SKILL_BLOOD_PUNISH_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class AttackMassacre;
//////////////////////////////////////////////////////////////////////////////

class BLOODPUNISH : public SkillHandler 
{
public:
	BLOODPUNISH() throw() {}
	~BLOODPUNISH() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BLOODPUNISH"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLOOD_PUNISH; }

	void execute(Monster *pMonster, Creature *pEnemy) throw(Error);
	void execute(Monster *pMonster, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern BLOODPUNISH g_BLOODPUNISH;

#endif /*  __SKILL_ATTACK_MASSACRE_H__ */
