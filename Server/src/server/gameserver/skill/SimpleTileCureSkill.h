//////////////////////////////////////////////////////////////////////////////
// Filename    : SimpleTileCureSkill.h
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SIMPLETILECURESKILL__
#define __SIMPLETILECURESKILL__

#include "SimpleSkill.h"

class SimpleTileCureSkill
{
public:
	SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, 
		const SIMPLE_SKILL_INPUT& param, SIMPLE_SKILL_OUTPUT& result,
		CEffectID_t CEffectID=0) throw (Error);
	/*
	void execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, 
		const SIMPLE_SKILL_INPUT& param,  SIMPLE_SKILL_OUTPUT& result,
		CEffectID_t CEffectID=0) throw (Error);
	void execute(Monster* pMonster,  
		const SIMPLE_SKILL_INPUT& param,  SIMPLE_SKILL_OUTPUT& result,
		CEffectID_t CEffectID=0) throw (Error);
	*/

	SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, 
		const SIMPLE_SKILL_INPUT& param, SIMPLE_SKILL_OUTPUT& result,
		CEffectID_t CEffectID=0) throw (Error);
	
	SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot,
		const SIMPLE_SKILL_INPUT& param, SIMPLE_SKILL_OUTPUT& result,
		CEffectID_t CEffectID=0) throw (Error);
	/*
	void execute(Vampire* pVampire, ObjectID_t ObjectID, VampireSkillSlot* pVampireSkillSlot, 
		const SIMPLE_SKILL_INPUT& param,  SIMPLE_SKILL_OUTPUT& result,
		CEffectID_t CEffectID=0) throw (Error);
	void execute(Monster* pMonster, Creature* pEnemy, 
		const SIMPLE_SKILL_INPUT& param,  SIMPLE_SKILL_OUTPUT& result,
		CEffectID_t CEffectID=0) throw (Error);
	*/
};

extern SimpleTileCureSkill g_SimpleTileCureSkill;

#endif
