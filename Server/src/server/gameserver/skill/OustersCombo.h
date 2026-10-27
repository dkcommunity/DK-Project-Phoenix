//////////////////////////////////////////////////////////////////////////////
// Filename    : OustersCombo.h 
// Written By  : bezz
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_OUSTERS_COMBO_HANDLER_H__
#define __SKILL_OUSTERS_COMBO_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class OustersCombo;
//////////////////////////////////////////////////////////////////////////////

class OustersCombo : public SkillHandler 
{
public:
	OustersCombo() throw() {}
	~OustersCombo() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "OustersCombo"; }
	SkillType_t getSkillType() const throw() { return SKILL_OUSTERS_COMBO; }

	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t ObjectID, OustersSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern OustersCombo g_OustersCombo;

#endif // __SKILL_OUSTERS_COMBO_HANDLER_H__

