//////////////////////////////////////////////////////////////////////////////
// Filename    : PASSINGHEALK2.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_PASSING_HEAL_K2_HANDLER_H__
#define __SKILL_PASSING_HEAL_K2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class PASSINGHEALK2;
//////////////////////////////////////////////////////////////////////////////

class PASSINGHEALK2: public SkillHandler 
{
public:
	PASSINGHEALK2() throw() {}
	~PASSINGHEALK2() throw() {}

public:
    string getSkillHandlerName() const throw() { return "PASSINGHEALK2"; }
	SkillType_t getSkillType() const throw() { return SKILL_PASSING_HEAL_K2; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern PASSINGHEALK2 g_PASSINGHEALK2;

#endif // __SKILL_PASSING_HEAL_K2_HANDLER_H__
