#ifndef __SKILL_REACTIVE_ARMOR2_HANDLER_H__
#define __SKILL_REACTIVE_ARMOR2_HANDLER_H__

#include "SkillHandler.h"

class ReactiveArmor2 : public SkillHandler 
{
public:
	ReactiveArmor2() throw() {}
	~ReactiveArmor2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ReactiveArmor2"; }
	SkillType_t getSkillType() const throw() { return SKILL_REACTIVE_ARMOR2; }

	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t ObjectID,  OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern ReactiveArmor2 g_ReactiveArmor2;

#endif
