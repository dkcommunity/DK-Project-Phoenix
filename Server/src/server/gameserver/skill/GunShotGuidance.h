//////////////////////////////////////////////////////////////////////////////
// Filename    : GunShotGuidance.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_GUN_SHOT_GUIDANCE_HANDLER_H__
#define __SKILL_GUN_SHOT_GUIDANCE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class GunShotGuidance;
//////////////////////////////////////////////////////////////////////////////

class GunShotGuidance : public SkillHandler 
{
public:
	GunShotGuidance() throw() {}
	~GunShotGuidance() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "GunShotGuidance"; }
	SkillType_t getSkillType() const throw() { return SKILL_GUN_SHOT_GUIDANCE; }

	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	void execute(Monster*, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern GunShotGuidance g_GunShotGuidance;

#endif // __SKILL_GUN_SHOT_GUIDANCE_HANDLER_H__
