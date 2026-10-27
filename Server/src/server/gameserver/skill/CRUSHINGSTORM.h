//////////////////////////////////////////////////////////////////////////////
// Filename    : CRUSHINGSTORM.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_CRUSHING_STORM_HANDLER_H__
#define __SKILL_CRUSHING_STORM_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class SwordOfThor;
//////////////////////////////////////////////////////////////////////////////

class CRUSHINGSTORM : public SkillHandler 
{
public:
	CRUSHINGSTORM() throw() {}
	~CRUSHINGSTORM() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "CRUSHINGSTORM"; }
	SkillType_t getSkillType() const throw() { return SKILL_CRUSHING_STORM; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern CRUSHINGSTORM g_CRUSHINGSTORM;

#endif // __SKILL_CRUSHING_STORM_HANDLER_H__
