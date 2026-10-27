//////////////////////////////////////////////////////////////////////////////
// Filename    : SetAfire.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ACCELERATE_CHARGE_1_HANDLER_H__
#define __SKILL_ACCELERATE_CHARGE_1_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class SetAfire;
//////////////////////////////////////////////////////////////////////////////

class ACCELERATECHARGE1 : public SkillHandler 
{
public:
	ACCELERATECHARGE1() throw() {}
	~ACCELERATECHARGE1() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ACCELERATECHARGE1"; }
	SkillType_t getSkillType() const throw() { return SKILL_ACCELERATE_CHARGE_1; }

	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ACCELERATECHARGE1 g_ACCELERATECHARGE1;

#endif // __SKILL_SET_AFIRE_HANDLER_H__
