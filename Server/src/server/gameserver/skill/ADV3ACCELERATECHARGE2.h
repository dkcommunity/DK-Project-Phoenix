//////////////////////////////////////////////////////////////////////////////
// Filename    : SetAfire.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ACCELERATE_CHARGE_2_HANDLER_H__
#define __SKILL_ACCELERATE_CHARGE_2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class SetAfire;
//////////////////////////////////////////////////////////////////////////////

class ACCELERATECHARGE2 : public SkillHandler 
{
public:
	ACCELERATECHARGE2() throw() {}
	~ACCELERATECHARGE2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ACCELERATECHARGE2"; }
	SkillType_t getSkillType() const throw() { return SKILL_ACCELERATE_CHARGE_2; }

	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ACCELERATECHARGE2 g_ACCELERATECHARGE2;

#endif // __SKILL_SET_AFIRE_HANDLER_H__
