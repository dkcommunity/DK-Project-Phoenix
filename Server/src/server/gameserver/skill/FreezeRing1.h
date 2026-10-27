//////////////////////////////////////////////////////////////////////////////
// Filename    : FreezeRing1.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_FREEZE_RING1_HANDLER_H__
#define __SKILL_FREEZE_RING1_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class FreezeRing1;
//////////////////////////////////////////////////////////////////////////////

class FreezeRing1 : public SkillHandler 
{
public:
	FreezeRing1() throw() {}
	~FreezeRing1() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "FreezeRing1"; }
	SkillType_t getSkillType() const throw() { return SKILL_FREEZE_RING1; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern FreezeRing1 g_FreezeRing1;

#endif // __SKILL_FREEZE_RING_HANDLER_H__
