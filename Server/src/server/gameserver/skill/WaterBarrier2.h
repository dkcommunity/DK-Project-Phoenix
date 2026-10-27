#ifndef __SKILL_WATER_BARRIER2_HANDLER_H__
#define __SKILL_WATER_BARRIER2_HANDLER_H__

#include "SkillHandler.h"


class WaterBarrier2 : public SkillHandler 
{
public:
	WaterBarrier2() throw() {}
	~WaterBarrier2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "WaterBarrier2"; }
	SkillType_t getSkillType() const throw() { return SKILL_WATER_BARRIER2; }

	virtual SkillResultType execute(Ousters* pOusters,  OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern WaterBarrier2 g_WaterBarrier2;

#endif
