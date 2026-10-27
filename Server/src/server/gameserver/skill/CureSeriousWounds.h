//////////////////////////////////////////////////////////////////////////////
// Filename    : CureSeriousWounds.h 
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_CURE_SERIOUS_WOUNDS_HANDLER_H__
#define __SKILL_CURE_SERIOUS_WOUNDS_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class CureSeriousWounds;
//////////////////////////////////////////////////////////////////////////////

class CureSeriousWounds: public SkillHandler 
{
public:
	CureSeriousWounds() throw() {}
	~CureSeriousWounds() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "CureSeriousWounds"; }
	SkillType_t getSkillType() const throw() { return SKILL_CURE_SERIOUS_WOUNDS; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);



	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern CureSeriousWounds g_CureSeriousWounds;

#endif // __SKILL_CURE_SERIOUS_WOUNDS_HANDLER_H__
