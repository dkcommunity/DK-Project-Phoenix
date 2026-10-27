//////////////////////////////////////////////////////////////////////////////
// Filename    : DummyDrake.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_DUMMY_DRAKE_HANDLER_H__
#define __SKILL_DUMMY_DRAKE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class DummyDrake;
//////////////////////////////////////////////////////////////////////////////

class DummyDrake : public SkillHandler 
{
public:
	DummyDrake() throw() {}
	~DummyDrake() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "DummyDrake"; }
	SkillType_t getSkillType() const throw() { return SKILL_DUMMY_DRAKE; }

	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern DummyDrake g_DummyDrake;

#endif // __SKILL_DUMMY_DRAKE_HANDLER_H__
