//////////////////////////////////////////////////////////////////////////////
// Filename    : ThunderStorm.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_THUNDER_STORM_HANDLER_H__
#define __SKILL_THUNDER_STORM_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ThunderStorm;
//////////////////////////////////////////////////////////////////////////////

class ThunderStorm : public SkillHandler 
{
public:
	ThunderStorm() throw() {}
	~ThunderStorm() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ThunderStorm"; }
	SkillType_t getSkillType() const throw() { return SKILL_THUNDER_STORM; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ThunderStorm g_ThunderStorm;

#endif // __SKILL_THUNDER_STORM_HANDLER_H__
