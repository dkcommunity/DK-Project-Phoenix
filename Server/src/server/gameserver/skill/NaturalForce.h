//////////////////////////////////////////////////////////////////////////////
// Filename    : NaturalForce.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_NATURAL_FORCE_HANDLER_H__
#define __SKILL_NATURAL_FORCE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class NaturalForce;
//////////////////////////////////////////////////////////////////////////////

class NaturalForce : public SkillHandler 
{
public:
	NaturalForce() throw() {}
	~NaturalForce() throw() {}

public:
    string getSkillHandlerName() const throw() { return "NaturalForce"; }
	SkillType_t getSkillType() const throw() { return SKILL_NATURAL_FORCE; }

	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput( const SkillInput& input, SkillOutput& output ) {}
};

// global variable declaration
extern NaturalForce g_NaturalForce;

#endif // __SKILL_NATURAL_FORCE_HANDLER_H__

