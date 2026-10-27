//////////////////////////////////////////////////////////////////////////////
// Filename    : SwordCombo.h 
// Written By  : bezz
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_SWORD_COMBO_HANDLER_H__
#define __SKILL_SWORD_COMBO_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class SwordCombo;
//////////////////////////////////////////////////////////////////////////////

class SwordCombo : public SkillHandler 
{
public:
	SwordCombo() throw() {}
	~SwordCombo() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "SwordCombo"; }
	SkillType_t getSkillType() const throw() { return SKILL_SWORD_COMBO; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern SwordCombo g_SwordCombo;

#endif // __SKILL_SWORD_COMBO_HANDLER_H__

