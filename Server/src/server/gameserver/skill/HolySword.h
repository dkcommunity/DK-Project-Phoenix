//////////////////////////////////////////////////////////////////////////////
// Filename    : HolySword.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_HOLY_SWORD_HANDLER_H__
#define __SKILL_HOLY_SWORD_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class HolySword;
//////////////////////////////////////////////////////////////////////////////

class HolySword : public SkillHandler 
{
public:
	HolySword() throw() {}
	~HolySword() throw() {}

public:
    string getSkillHandlerName() const throw() { return "HolySword"; }
	SkillType_t getSkillType() const throw() { return SKILL_HOLY_SWORD; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput( const SkillInput& input, SkillOutput& output ) {}
};

// global variable declaration
extern HolySword g_HolySword;

#endif // __SKILL_HOLY_SWORD_HANDLER_H__

