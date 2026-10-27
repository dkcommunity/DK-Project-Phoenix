//////////////////////////////////////////////////////////////////////////////
// Filename    : VenomCircle.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_VENOM_CIRCLE_HANDLER_H__
#define __SKILL_VENOM_CIRCLE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class VenomCircle;
//////////////////////////////////////////////////////////////////////////////

class VenomCircle : public SkillHandler 
{
public:
	VenomCircle() throw() {}
	~VenomCircle() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "VenomCircle"; }
	SkillType_t getSkillType() const throw() { return SKILL_VENOM_CIRCLE; }

	virtual SkillResultType execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern VenomCircle g_VenomCircle;

#endif // __SKILL_CURSE_OF_BLOOD_HANDLER_H__
