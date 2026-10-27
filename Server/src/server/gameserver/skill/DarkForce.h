//////////////////////////////////////////////////////////////////////////////
// Filename    : DarkForce.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_DARK_FORCE_HANDLER_H__
#define __SKILL_DARK_FORCE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class DarkForce;
//////////////////////////////////////////////////////////////////////////////

class DarkForce : public SkillHandler 
{
public:
	DarkForce() throw() {}
	~DarkForce() throw() {}

public:
    string getSkillHandlerName() const throw() { return "DarkForce"; }
	SkillType_t getSkillType() const throw() { return SKILL_DARK_FORCE; }

	virtual SkillResultType execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput( const SkillInput& input, SkillOutput& output ) {}
};

// global variable declaration
extern DarkForce g_DarkForce;

#endif // __SKILL_DARK_FORCE_HANDLER_H__

