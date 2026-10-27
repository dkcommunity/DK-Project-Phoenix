//////////////////////////////////////////////////////////////////////////////
// Filename    : HolyMace.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_HOLY_MACE_HANDLER_H__
#define __SKILL_HOLY_MACE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class HolyMace;
//////////////////////////////////////////////////////////////////////////////

class HolyMace : public SkillHandler 
{
public:
	HolyMace() throw() {}
	~HolyMace() throw() {}

public:
    string getSkillHandlerName() const throw() { return "HolyMace"; }
	SkillType_t getSkillType() const throw() { return SKILL_HOLY_MACE; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput( const SkillInput& input, SkillOutput& output ) {}
};

// global variable declaration
extern HolyMace g_HolyMace;

#endif // __SKILL_HOLY_MACE_HANDLER_H__

