//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodDrain2.h 
// Written By  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLOOD_DRAIN2_HANDLER_H__
#define __SKILL_BLOOD_DRAIN2_HANDLER_H__

#include "SkillHandler.h"

#define BLOODDRAIN_DURATION		(3*4*3600*10)	//3 ¿œ

//////////////////////////////////////////////////////////////////////
// class BloodDrain2;
//////////////////////////////////////////////////////////////////////

class BloodDrain2 : public SkillHandler 
{
public:
	BloodDrain2() throw() {}
	~BloodDrain2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BloodDrain2"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLOOD_DRAIN2; }

	virtual SkillResultType execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	ObjectID_t executeSingleBloodDrain( Vampire* pVampire, Creature* pEnemy, ModifyInfo* pAttackerMI ) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output) {}
};

// global variable declaration
extern BloodDrain2 g_BloodDrain2;

#endif // __SKILL_BLOOD_DRAIN2_HANDLER_H__
