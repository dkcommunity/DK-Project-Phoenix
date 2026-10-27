//////////////////////////////////////////////////////////////////////////////
// Filename    : SimpleCureSkill.h
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SIMPLECURESKILL__
#define __SIMPLECURESKILL__

#include "SimpleSkill.h"

class SimpleCureSkill
{
public:
	bool CheckConditionHP(Slayer* pSlayer, int iLevel);
	bool RemoveBloodDrain(Slayer* pSlayer, int iLevel);
	
	SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSlayerSkillSlot, 
		const SIMPLE_SKILL_INPUT& param, SIMPLE_SKILL_OUTPUT& result,
		CEffectID_t CEffectID=0) throw (Error);

	SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSlayerSkillSlot, 
		const SIMPLE_SKILL_INPUT& param, SIMPLE_SKILL_OUTPUT& result,
		CEffectID_t CEffectID=0) throw (Error);

};

extern SimpleCureSkill g_SimpleCureSkill;

#endif
