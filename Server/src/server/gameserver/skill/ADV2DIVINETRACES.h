#ifndef __SKILL_ADV2_DIVINE_TRACES_HANDLER_H__
#define __SKILL_ADV2_DIVINE_TRACES_HANDLER_H__

#include "SkillHandler.h"

class ADV2DIVINETRACES: public SkillHandler 
{
public:
	ADV2DIVINETRACES() throw() {}
	~ADV2DIVINETRACES() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2DIVINETRACES"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_DIVINE_TRACES; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern ADV2DIVINETRACES g_ADV2DIVINETRACES;

#endif
