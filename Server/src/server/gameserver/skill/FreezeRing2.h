//////////////////////////////////////////////////////////////////////////////
// Filename    : FreezeRing2.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_FREEZE_RING2_HANDLER_H__
#define __SKILL_FREEZE_RING2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class FreezeRing2;
//////////////////////////////////////////////////////////////////////////////

class FreezeRing2 : public SkillHandler 
{
public:
	FreezeRing2() throw() {}
	~FreezeRing2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "FreezeRing2"; }
	SkillType_t getSkillType() const throw() { return SKILL_FREEZE_RING2; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	
	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern FreezeRing2 g_FreezeRing2;

#endif // __SKILL_FREEZE_RING_HANDLER_H__
