//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodyWings.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLOODY_WINGS_HANDLER_H__
#define __SKILL_BLOODY_WINGS_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BloodyWings;
//////////////////////////////////////////////////////////////////////////////

class BloodyWings : public SkillHandler 
{
public:
	BloodyWings() throw() {}
	~BloodyWings() throw() {}

public:
    string getSkillHandlerName() const throw() { return "BloodyWings"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLOODY_WINGS; }

	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t,  VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y,  VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern BloodyWings g_BloodyWings;

#endif // __SKILL_BLOODY_WINGS_HANDLER_H__
