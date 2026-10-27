#ifndef __SKILL_ADV2_WILD_WOLF_STRIKE_HANDLER_H__
#define __SKILL_ADV2_WILD_WOLF_STRIKE_HANDLER_H__

#include "SkillHandler.h"

class ADV2WILDWOLFSTRIKE : public SkillHandler 
{
public:
	ADV2WILDWOLFSTRIKE() throw() {}
	~ADV2WILDWOLFSTRIKE() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2WILDWOLFSTRIKE"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_WILD_WOLF_STRIKE; }

	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t targetObject, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern ADV2WILDWOLFSTRIKE g_ADV2WILDWOLFSTRIKE;

#endif
