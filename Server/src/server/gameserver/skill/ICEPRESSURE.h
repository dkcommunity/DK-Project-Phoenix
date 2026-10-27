//////////////////////////////////////////////////////////////////////////////
// Filename    : BLIZZARD.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ICEPRESSURE_H__
#define __SKILL_ICEPRESSURE_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BLIZZARD;
//////////////////////////////////////////////////////////////////////////////

class ICEPRESSURE : public SkillHandler 
{
public:
	ICEPRESSURE() throw() {}
	~ICEPRESSURE() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ICEPRESSURE"; }
	SkillType_t getSkillType() const throw() { return SKILL_ICE_PRESSURE; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ICEPRESSURE g_ICEPRESSURE;

#endif // __SKILL_SHARP_HAIL_HANDLER_H__
