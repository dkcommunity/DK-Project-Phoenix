//////////////////////////////////////////////////////////////////////////////
// Filename    : EaglesEye1.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_EAGLES_EYE1_HANDLER_H__
#define __SKILL_EAGLES_EYE1_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class EaglesEye1;
//////////////////////////////////////////////////////////////////////////////

class EaglesEye1 : public SkillHandler 
{
public:
	EaglesEye1() throw() {}
	~EaglesEye1() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "EaglesEye1"; }
	SkillType_t getSkillType() const throw() { return SKILL_EAGLES_EYE1; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern EaglesEye1 g_EaglesEye1;

#endif // __SKILL_EAGLES_EYE_HANDLER_H__
