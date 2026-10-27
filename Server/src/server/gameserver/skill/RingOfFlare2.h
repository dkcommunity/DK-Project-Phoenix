//////////////////////////////////////////////////////////////////////////////
// Filename    : RingOfFlare.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_RING_OF_FLARE2_HANDLER_H__
#define __SKILL_RING_OF_FLARE2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class RingOfFlare;
//////////////////////////////////////////////////////////////////////////////

class RingOfFlare2 : public SkillHandler 
{
public:
	RingOfFlare2() throw() {}
	~RingOfFlare2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "RingOfFlare2"; }
	SkillType_t getSkillType() const throw() { return SKILL_RING_OF_FLARE2; }

	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern RingOfFlare2 g_RingOfFlare2;

#endif // __SKILL_RING_OF_FLARE_HANDLER_H__
