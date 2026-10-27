#ifndef __SKILL_ADV2_SHINING_CHAKRAM_SPIN_HANDLER_H__
#define __SKILL_ADV2_SHINING_CHAKRAM_SPIN_HANDLER_H__

#include "SkillHandler.h"

class ADV2SHININGCHAKRAMSPIN : public SkillHandler 
{
public:
	ADV2SHININGCHAKRAMSPIN() throw() {}
	~ADV2SHININGCHAKRAMSPIN() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2SHININGCHAKRAMSPIN"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_SHINING_CHAKRAM_SPIN; }

	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern ADV2SHININGCHAKRAMSPIN g_ADV2SHININGCHAKRAMSPIN;

#endif

