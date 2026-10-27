//////////////////////////////////////////////////////////////////////////////
// Filename    : EarthForce.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_EARTH_FORCE_HANDLER_H__
#define __SKILL_EARTH_FORCE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class EarthForce;
//////////////////////////////////////////////////////////////////////////////

class EarthForce : public SkillHandler 
{
public:
	EarthForce() throw() {}
	~EarthForce() throw() {}

public:
    string getSkillHandlerName() const throw() { return "EarthForce"; }
	SkillType_t getSkillType() const throw() { return SKILL_EARTH_FORCE; }

	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput( const SkillInput& input, SkillOutput& output ) {}
};

// global variable declaration
extern EarthForce g_EarthForce;

#endif // __SKILL_EARTH_FORCE_HANDLER_H__

