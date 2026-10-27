//////////////////////////////////////////////////////////////////////////////
// Filename    : BLIZZARD.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLIZZARD_H__
#define __SKILL_BLIZZARD_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BLIZZARD;
//////////////////////////////////////////////////////////////////////////////

class BLIZZARD : public SkillHandler 
{
public:
	BLIZZARD() throw() {}
	~BLIZZARD() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BLIZZARD"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLIZZARD; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern BLIZZARD g_BLIZZARD;

#endif // __SKILL_SHARP_HAIL_HANDLER_H__
