//////////////////////////////////////////////////////////////////////////////
// Filename    : TargetSummonMonsters.h 
// Written By  : Shinobi
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_TARGET_SUMMON_MONSTERS_HANDLER_H__
#define __SKILL_TARGET_SUMMON_MONSTERS_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class TargetSummonMonsters;
//////////////////////////////////////////////////////////////////////////////

class TargetSummonMonsters: public SkillHandler 
{
public:
	TargetSummonMonsters() throw();
	~TargetSummonMonsters() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "TargetSummonMonsters"; }
	SkillType_t getSkillType() const throw() { return SKILL_TARGET_SUMMON_MONSTERS; }

	void execute(Monster* pMonster) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern TargetSummonMonsters g_TargetSummonMonsters;

#endif // __SKILL_HIDE_HANDLER_H__
