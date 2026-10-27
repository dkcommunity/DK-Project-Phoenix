//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodyScarify.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLOODY_SCARIFY_HANDLER_H__
#define __SKILL_BLOODY_SCARIFY_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BloodyScarify;
//////////////////////////////////////////////////////////////////////////////

class BloodyScarify : public SkillHandler 
{
public:
	BloodyScarify() throw() {}
	~BloodyScarify() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BloodyScarify"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLOODY_SCARIFY; }

	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t ObjectID,  VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern BloodyScarify g_BloodyScarify;

#endif // __SKILL_BLOODY_SCARIFY_HANDLER_H__
