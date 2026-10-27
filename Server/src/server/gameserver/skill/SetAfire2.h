#ifndef __SKILL_SET_AFIRE2_HANDLER_H__
#define __SKILL_SET_AFIRE2_HANDLER_H__

#include "SkillHandler.h"

class SetAfire2: public SkillHandler 
{
public:
	SetAfire2() throw() {}
	~SetAfire2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "SetAfire2"; }

	SkillType_t getSkillType() const throw() { return SKILL_SET_AFIRE2; }

	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t targetObject, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern SetAfire2 g_SetAfire2;

#endif
