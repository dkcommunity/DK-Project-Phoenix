//////////////////////////////////////////////////////////////////////////////
// Filename    : FrozenGlacier.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_FROZEN_GLACIER_HANDLER_H__
#define __SKILL_FROZEN_GLACIER_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class FrozenGlacier;
//////////////////////////////////////////////////////////////////////////////

class FrozenGlacier : public SkillHandler 
{
public:
	FrozenGlacier() throw() {}
	~FrozenGlacier() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "FrozenGlacier"; }
	SkillType_t getSkillType() const throw() { return SKILL_FROZEN_GLACIER; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t ObjectID,  OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	void execute(Ousters* pOusters, Monster*, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern FrozenGlacier g_FrozenGlacier;

#endif // __SKILL_FROZEN_GLACIER_HANDLER_H__
