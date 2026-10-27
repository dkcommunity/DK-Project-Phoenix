#ifndef __SKILL_ADV2_ADVENT_BLADE_HANDLER_H__
#define __SKILL_ADV2_ADVENT_BLADE_HANDLER_H__

#include "SkillHandler.h"

class ADV2ADVENTBLADE: public SkillHandler 
{
public:
	ADV2ADVENTBLADE() throw() {}
	~ADV2ADVENTBLADE() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2ADVENTBLADE"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_ADVENT_BLADE; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern ADV2ADVENTBLADE g_ADV2ADVENTBLADE;

#endif
