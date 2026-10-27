//////////////////////////////////////////////////////////////////////////////
// Filename    : BladeStorm.h 
// Written By  : bigheart 
// Description : 20080403 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLADE_STORM_HANDLER_H__
#define __SKILL_BLADE_STORM_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BladeStorm;
//////////////////////////////////////////////////////////////////////////////

class BladeStorm : public SkillHandler 
{
public:
	BladeStorm() throw() {}
	~BladeStorm() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BladeStorm"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLADE_STORM; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t targetObjectID,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern BladeStorm g_BladeStorm;

#endif // __SKILL_BLADE_STORM_H__
