//////////////////////////////////////////////////////////////////////////////
// Filename    : NaturalPeace.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_NATURAL_PEACE_HANDLER_H__
#define __SKILL_NATURAL_PEACE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class NaturalPeace;
//////////////////////////////////////////////////////////////////////////////

class NaturalPeace : public SkillHandler 
{
public:
	NaturalPeace() throw() {}
	~NaturalPeace() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "NaturalPeace"; }
	SkillType_t getSkillType() const throw() { return SKILL_NATURAL_PEACE; }

	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output) {}
};

// global variable declaration
extern NaturalPeace g_NaturalPeace;

#endif // __SKILL_NATURAL_PEACE_HANDLER_H__

