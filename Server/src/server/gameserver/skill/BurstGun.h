//////////////////////////////////////////////////////////////////////////////
// Filename    : BurstGun.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BURST_GUN_HANDLER_H__
#define __SKILL_BURST_GUN_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BurstGun;
//////////////////////////////////////////////////////////////////////////////

class BurstGun : public SkillHandler 
{
public:
	BurstGun() throw() {}
	~BurstGun() throw() {}

public:
    string getSkillHandlerName() const throw() { return "BurstGun"; }
	SkillType_t getSkillType() const throw() { return SKILL_BURST_GUN; }

	SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput( const SkillInput& input, SkillOutput& output ) {}
};

// global variable declaration
extern BurstGun g_BurstGun;

#endif // __SKILL_BURST_GUN_HANDLER_H__

