//////////////////////////////////////////////////////////////////////////////
// Filename    : PotentialExplosion2.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_POTENTIAL_EXPLOSION2_HANDLER_H__
#define __SKILL_POTENTIAL_EXPLOSION2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class PotentialExplosion2;
//////////////////////////////////////////////////////////////////////////////

class PotentialExplosion2 : public SkillHandler 
{
public:
	PotentialExplosion2() throw() {}
	~PotentialExplosion2() throw() {}

public:
    string getSkillHandlerName() const throw() { return "PotentialExplosion2"; }
	SkillType_t getSkillType() const throw() { return SKILL_POTENTIAL_EXPLOSION2; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern PotentialExplosion2 g_PotentialExplosion2;

#endif // __SKILL_POTENTIAL_EXPLOSION2_HANDLER_H__
