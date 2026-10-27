//////////////////////////////////////////////////////////////////////////////
// Filename    : GreatHeal2.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_GREAT_HEAL_2_HANDLER_H__
#define __SKILL_GREAT_HEAL_2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class GREATHEAL2;
//////////////////////////////////////////////////////////////////////////////

class GREATHEAL2: public SkillHandler 
{
public:
	GREATHEAL2() throw() {}
	~GREATHEAL2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "GREATHEAL2"; }
	SkillType_t getSkillType() const throw() { return SKILL_GREAT_HEAL_2; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern GREATHEAL2 g_GREATHEAL2;

#endif // __SKILL_GREATHEAL2_HANDLER_H__
