//////////////////////////////////////////////////////////////////////////////
// Filename    : FROZENGROUND2.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_FROZEN_GROUND_2_H__
#define __SKILL_FROZEN_GROUND_2_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BLIZZARD;
//////////////////////////////////////////////////////////////////////////////

class FROZENGROUND2 : public SkillHandler 
{
public:
	FROZENGROUND2() throw() {}
	~FROZENGROUND2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "FROZENGROUND2"; }
	SkillType_t getSkillType() const throw() { return SKILL_FROZEN_GROUND_2; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern FROZENGROUND2 g_FROZENGROUND2;

#endif // __SKILL_SHARP_HAIL_HANDLER_H__
