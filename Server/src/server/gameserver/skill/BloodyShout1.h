//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodyShout1.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLOODY_SHOUT1_HANDLER_H__
#define __SKILL_BLOODY_SHOUT1_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BloodyShout1;
//////////////////////////////////////////////////////////////////////////////

class BloodyShout1 : public SkillHandler 
{
public:
	BloodyShout1() throw() {}
	~BloodyShout1() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BloodyShout1"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLOODY_SHOUT1; }

	virtual SkillResultType execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern BloodyShout1 g_BloodyShout1;

#endif // __SKILL_BLOODY_SHOUT_HANDLER_H__
