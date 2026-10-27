//////////////////////////////////////////////////////////////////////////////
// Filename    : FROZENGROUND1.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_FROZEN_GROUND_1_H__
#define __SKILL_FROZEN_GROUND_1_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BLIZZARD;
//////////////////////////////////////////////////////////////////////////////

class FROZENGROUND1 : public SkillHandler 
{
public:
	FROZENGROUND1() throw() {}
	~FROZENGROUND1() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "FROZENGROUND1"; }
	SkillType_t getSkillType() const throw() { return SKILL_FROZEN_GROUND_1; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern FROZENGROUND1 g_FROZENGROUND1;

#endif // __SKILL_SHARP_HAIL_HANDLER_H__
