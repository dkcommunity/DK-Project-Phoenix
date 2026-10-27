//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodyShout2.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLOODY_SHOUT2_HANDLER_H__
#define __SKILL_BLOODY_SHOUT2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BloodyShout2;
//////////////////////////////////////////////////////////////////////////////

class BloodyShout2 : public SkillHandler 
{
public:
	BloodyShout2() throw() {}
	~BloodyShout2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BloodyShout2"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLOODY_SHOUT2; }

	virtual SkillResultType execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern BloodyShout2 g_BloodyShout2;

#endif // __SKILL_BLOODY_SHOUT_HANDLER_H__
