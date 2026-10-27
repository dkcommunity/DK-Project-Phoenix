//////////////////////////////////////////////////////////////////////////////
// Filename    : FirePiercing.h 
// Written By  : FLAME_SPIKE
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_FLAME_SPIKE_HANDLER_H__
#define __SKILL_FLAME_SPIKE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class FLAME_SPIKE;
//////////////////////////////////////////////////////////////////////////////

class FLAMESPIKE : public SkillHandler 
{
public:
	FLAMESPIKE() throw() {}
	~FLAMESPIKE() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "FLAMESPIKE"; }
	SkillType_t getSkillType() const throw() { return SKILL_FLAME_SPIKE; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern FLAMESPIKE g_FLAMESPIKE;

#endif // __SKILL_FIRE_PIERCING_HANDLER_H__
