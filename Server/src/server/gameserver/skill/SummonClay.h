//////////////////////////////////////////////////////////////////////////////
// Filename    : SummonClay.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_SUMMON_CLAY_HANDLER_H__
#define __SKILL_SUMMON_CLAY_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class SummonClay;
//////////////////////////////////////////////////////////////////////////////

class SummonClay : public SkillHandler 
{
public:
	SummonClay() throw() {}
	~SummonClay() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "SummonClay"; }
	SkillType_t getSkillType() const throw() { return SKILL_SUMMON_CLAY; }

	virtual SkillResultType execute( Ousters* pOusters, ObjectID_t ObjectID, OustersSkillSlot* pSkillSlot, CEffectID_t CEffectID ) throw(Error);
	virtual SkillResultType execute( Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pSkillSlot, CEffectID_t CEffectID ) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern SummonClay g_SummonClay;

#endif // __SKILL_SUMMON_CLAY_HANDLER_H__
