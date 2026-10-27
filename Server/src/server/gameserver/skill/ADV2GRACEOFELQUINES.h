#ifndef __SKILL_ADV2_GRACE_OF_ELQUINES_HANDLER_H__
#define __SKILL_ADV2_GRACE_OF_ELQUINES_HANDLER_H__

#include "SkillHandler.h"


class ADV2GRACEOFELQUINES : public SkillHandler 
{
public:
	ADV2GRACEOFELQUINES() throw() {}
	~ADV2GRACEOFELQUINES() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2GRACEOFELQUINES"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_GRACE_OF_ELQUINES; }

	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t ObjectID,  OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern ADV2GRACEOFELQUINES g_ADV2GRACEOFELQUINES;

#endif
