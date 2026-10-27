//////////////////////////////////////////////////////////////////////////////
// Filename    : WaterForce.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_WATER_FORCE_HANDLER_H__
#define __SKILL_WATER_FORCE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class WaterForce;
//////////////////////////////////////////////////////////////////////////////

class WaterForce : public SkillHandler 
{
public:
	WaterForce() throw() {}
	~WaterForce() throw() {}

public:
    string getSkillHandlerName() const throw() { return "WaterForce"; }
	SkillType_t getSkillType() const throw() { return SKILL_WATER_FORCE; }

	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput( const SkillInput& input, SkillOutput& output ) {}
};

// global variable declaration
extern WaterForce g_WaterForce;

#endif // __SKILL_WATER_FORCE_HANDLER_H__

