//////////////////////////////////////////////////////////////////////////////
// Filename    : SimpleMissileSkill.h
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SIMPLEMISSILESKILL__
#define __SIMPLEMISSILESKILL__

#include "SimpleSkill.h"

class SimpleMissileSkill
{
public:
	SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, 
		const SIMPLE_SKILL_INPUT& param, SIMPLE_SKILL_OUTPUT& result,
		CEffectID_t CEffectID=0) throw (Error);
	SkillResultType execute(Vampire* pVampire, ObjectID_t ObjectID, VampireSkillSlot* pVampireSkillSlot, 
		const SIMPLE_SKILL_INPUT& param, SIMPLE_SKILL_OUTPUT& result,
		CEffectID_t CEffectID=0, int HitBonus=0) throw (Error);
	SkillResultType execute(Ousters* pOusters, ObjectID_t ObjectID, OustersSkillSlot* pOustersSkillSlot, 
		const SIMPLE_SKILL_INPUT& param, SIMPLE_SKILL_OUTPUT& result,
		CEffectID_t CEffectID=0, int HitBonus=0) throw (Error);
	void execute(Monster* pMonster, Creature* pEnemy, 
		const SIMPLE_SKILL_INPUT& param, SIMPLE_SKILL_OUTPUT& result,
		CEffectID_t CEffectID=0) throw (Error);
};

extern SimpleMissileSkill g_SimpleMissileSkill;

#endif
