//////////////////////////////////////////////////////////////////////////////
// Filename    : HEALPASS2.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_HEAL_PASS_2_HANDLER_H__
#define __SKILL_HEAL_PASS_2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class HEALPASS2;
//////////////////////////////////////////////////////////////////////////////

class HEALPASS2: public SkillHandler 
{
public:
	HEALPASS2() throw() {}
	~HEALPASS2() throw() {}

public:
    string getSkillHandlerName() const throw() { return "HEALPASS2"; }
	SkillType_t getSkillType() const throw() { return SKILL_HEAL_PASS_2; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern HEALPASS2 g_HEALPASS2;

#endif // __SKILL_HEALPASS2_HANDLER_H__
