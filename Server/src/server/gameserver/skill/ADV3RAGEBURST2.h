//////////////////////////////////////////////////////////////////////////////
// Filename    : RAGEBURST2.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_RAGE_BURST_2_HANDLER_H__
#define __SKILL_RAGE_BURST_2_HANDLER_H__

#include "SkillHandler.h"

class RAGEBURST2 : public SkillHandler 
{
public:
	RAGEBURST2() throw() {}
	~RAGEBURST2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "RAGEBURST2"; }
	SkillType_t getSkillType() const throw() { return SKILL_RAGE_BURST_2; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t ObjectID,  OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern RAGEBURST2 g_RAGEBURST2;

#endif
