//////////////////////////////////////////////////////////////////////////////
// Filename    : BLIZZARD.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLIZZARD_2_H__
#define __SKILL_BLIZZARD_2_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BLIZZARD;
//////////////////////////////////////////////////////////////////////////////

class BLIZZARD2 : public SkillHandler 
{
public:
	BLIZZARD2() throw() {}
	~BLIZZARD2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BLIZZARD2"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLIZZARD2; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern BLIZZARD2 g_BLIZZARD2;

#endif // __SKILL_SHARP_HAIL_HANDLER_H__
