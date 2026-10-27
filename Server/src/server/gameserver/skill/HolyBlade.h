//////////////////////////////////////////////////////////////////////////////
// Filename    : HolyBlade.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_HOLY_BLADE_HANDLER_H__
#define __SKILL_HOLY_BLADE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class HolyBlade;
//////////////////////////////////////////////////////////////////////////////

class HolyBlade : public SkillHandler 
{
public:
	HolyBlade() throw() {}
	~HolyBlade() throw() {}

public:
    string getSkillHandlerName() const throw() { return "HolyBlade"; }
	SkillType_t getSkillType() const throw() { return SKILL_HOLY_BLADE; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput( const SkillInput& input, SkillOutput& output ) {}
};

// global variable declaration
extern HolyBlade g_HolyBlade;

#endif // __SKILL_HOLY_BLADE_HANDLER_H__

