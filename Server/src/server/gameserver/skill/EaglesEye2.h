//////////////////////////////////////////////////////////////////////////////
// Filename    : EaglesEye2.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_EAGLES_EYE2_HANDLER_H__
#define __SKILL_EAGLES_EYE2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class EaglesEye2;
//////////////////////////////////////////////////////////////////////////////

class EaglesEye2 : public SkillHandler 
{
public:
	EaglesEye2() throw() {}
	~EaglesEye2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "EaglesEye2"; }
	SkillType_t getSkillType() const throw() { return SKILL_EAGLES_EYE2; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern EaglesEye2 g_EaglesEye2;

#endif // __SKILL_EAGLES_EYE_HANDLER_H__
