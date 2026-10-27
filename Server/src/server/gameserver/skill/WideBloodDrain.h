//////////////////////////////////////////////////////////////////////////////
// Filename    : WideBloodDrain.h 
// Written By  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_WIDE_BLOOD_DRAIN_HANDLER_H__
#define __SKILL_WIDE_BLOOD_DRAIN_HANDLER_H__

#include "SkillHandler.h"

#define BLOODDRAIN_DURATION		(3*4*3600*10)	//3 ¿œ

//////////////////////////////////////////////////////////////////////
// class WideBloodDrain;
//////////////////////////////////////////////////////////////////////

class WideBloodDrain : public SkillHandler 
{
public:
	WideBloodDrain() throw() {}
	~WideBloodDrain() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "WideBloodDrain"; }
	SkillType_t getSkillType() const throw() { return SKILL_WIDE_BLOOD_DRAIN; }

	virtual SkillResultType execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	ObjectID_t executeSingleBloodDrain( Vampire* pVampire, Creature* pEnemy, ModifyInfo* pAttackerMI ) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output) {}
};

// global variable declaration
extern WideBloodDrain g_WideBloodDrain;

#endif // __SKILL_WIDE_BLOOD_DRAIN_HANDLER_H__
