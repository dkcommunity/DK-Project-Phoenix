//////////////////////////////////////////////////////////////////////////////
// Filename    : VampireCombo.h 
// Written By  : bezz
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_VAMPIRE_COMBO_HANDLER_H__
#define __SKILL_VAMPIRE_COMBO_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class VampireCombo;
//////////////////////////////////////////////////////////////////////////////

class VampireCombo : public SkillHandler 
{
public:
	VampireCombo() throw() {}
	~VampireCombo() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "VampireCombo"; }
	SkillType_t getSkillType() const throw() { return SKILL_VAMPIRE_COMBO; }

	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t ObjectID, VampireSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern VampireCombo g_VampireCombo;

#endif // __SKILL_VAMPIRE_COMBO_HANDLER_H__

