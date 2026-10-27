//////////////////////////////////////////////////////////////////////////////
// Filename    : RAGEBURST1.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_RAGE_BURST_1_HANDLER_H__
#define __SKILL_RAGE_BURST_1_HANDLER_H__

#include "SkillHandler.h"

class RAGEBURST1 : public SkillHandler 
{
public:
	RAGEBURST1() throw() {}
	~RAGEBURST1() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "RAGEBURST1"; }
	SkillType_t getSkillType() const throw() { return SKILL_RAGE_BURST_1; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t ObjectID,  OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern RAGEBURST1 g_RAGEBURST1;

#endif
