//////////////////////////////////////////////////////////////////////////////
// Filename    : SharpShield.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_SHARP_SHIELD2_HANDLER_H__
#define __SKILL_SHARP_SHIELD2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class SharpShield2;
//////////////////////////////////////////////////////////////////////////////

class SharpShield2 : public SkillHandler 
{
public:
	SharpShield2() throw() {}
	~SharpShield2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "SharpShield2"; }
	SkillType_t getSkillType() const throw() { return SKILL_SHARP_SHIELD2; }

	SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern SharpShield2 g_SharpShield2;

#endif // __SKILL_SHARP_SHIELD_HANDLER_H__
