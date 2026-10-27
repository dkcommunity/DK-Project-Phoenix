//////////////////////////////////////////////////////////////////////////////
// Filename    : RequestResurrect.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_REQUEST_RESURRECT_HANDLER_H__
#define __SKILL_REQUEST_RESURRECT_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class RequestResurrect;
//////////////////////////////////////////////////////////////////////////////

class RequestResurrect : public SkillHandler 
{
public:
	RequestResurrect() throw() {}
	~RequestResurrect() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "RequestResurrect"; }

	SkillType_t getSkillType() const throw() { return SKILL_REQUEST_RESURRECT; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSlayerSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern RequestResurrect g_RequestResurrect;

#endif // __SKILL_REQUEST_RESURRECT_HANDLER_H__
