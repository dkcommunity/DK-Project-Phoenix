//////////////////////////////////////////////////////////////////////////////
// Filename    : ExplosionRocketLauncher.h 
// Written By  : bigheart 
// Description : 20080407
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_EXPLOSION_ROCKET_LAUNCHER_HANDLER_H__
#define __SKILL_EXPLOSION_ROCKET_LAUNCHER_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ExplosionRocketLauncher;
//////////////////////////////////////////////////////////////////////////////

class ExplosionRocketLauncher: public SkillHandler 
{
public:
	ExplosionRocketLauncher() throw() {}
	~ExplosionRocketLauncher() throw() {}

public:
    string getSkillHandlerName() const throw() { return "ExplosionRocketLauncher"; }
	SkillType_t getSkillType() const throw() { return SKILL_EXPLOSION_ROCKET_LAUNCHER; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ExplosionRocketLauncher g_ExplosionRocketLauncher;

#endif // __SKILL_EXPLOSION_ROCKET_LAUNCHER_HANDLER_H__
