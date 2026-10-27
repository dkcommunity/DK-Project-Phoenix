//////////////////////////////////////////////////////////////////////////////
// Filename    : BladeCombo.h 
// Written By  : bezz
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLADE_COMBO_HANDLER_H__
#define __SKILL_BLADE_COMBO_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BladeCombo;
//////////////////////////////////////////////////////////////////////////////

class BladeCombo : public SkillHandler 
{
public:
	BladeCombo() throw() {}
	~BladeCombo() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BladeCombo"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLADE_COMBO; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern BladeCombo g_BladeCombo;

#endif // __SKILL_BLADE_COMBO_HANDLER_H__

