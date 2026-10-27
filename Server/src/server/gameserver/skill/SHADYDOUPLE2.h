#ifndef __SKILL_SHADY_DOUPLE2_HANDLER_H__
#define __SKILL_SHADY_DOUPLE2_HANDLER_H__

#include "SkillHandler.h"


class SHADYDOUPLE2 : public SkillHandler 
{
public:
	SHADYDOUPLE2() throw() {}
	~SHADYDOUPLE2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "SHADYDOUPLE2"; }
	SkillType_t getSkillType() const throw() { return SKILL_SHADY_DOUPLE2; }

	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t ObjectID,  VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern SHADYDOUPLE2 g_SHADYDOUPLE2;

#endif
