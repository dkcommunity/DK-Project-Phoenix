//////////////////////////////////////////////////////////////////////////////
// Filename    : PULSEMAGNETICFIELD.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_PULSEMAGNETICFIELD_HANDLER_H__
#define __SKILL_PULSEMAGNETICFIELD_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class Darkness;
//////////////////////////////////////////////////////////////////////////////

class PULSEMAGNETICFIELD : public SkillHandler 
{
public:
	PULSEMAGNETICFIELD() throw() {}
	~PULSEMAGNETICFIELD() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "PULSEMAGNETICFIELD"; }
	SkillType_t getSkillType() const throw() { return SKILL_PULSEMAGNETICFIELD; }

	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern PULSEMAGNETICFIELD g_PULSEMAGNETICFIELD;

#endif
