//////////////////////////////////////////////////////////////////////////////
// Filename    : FireForce.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_FIRE_FORCE_HANDLER_H__
#define __SKILL_FIRE_FORCE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class FireForce;
//////////////////////////////////////////////////////////////////////////////

class FireForce : public SkillHandler 
{
public:
	FireForce() throw() {}
	~FireForce() throw() {}

public:
    string getSkillHandlerName() const throw() { return "FireForce"; }
	SkillType_t getSkillType() const throw() { return SKILL_FIRE_FORCE; }

	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput( const SkillInput& input, SkillOutput& output ) {}
};

// global variable declaration
extern FireForce g_FireForce;

#endif // __SKILL_FIRE_FORCE_HANDLER_H__

