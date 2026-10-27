//////////////////////////////////////////////////////////////////////////////
// Filename    : HolyCross.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_HOLY_CROSS_HANDLER_H__
#define __SKILL_HOLY_CROSS_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class HolyCross;
//////////////////////////////////////////////////////////////////////////////

class HolyCross : public SkillHandler 
{
public:
	HolyCross() throw() {}
	~HolyCross() throw() {}

public:
    string getSkillHandlerName() const throw() { return "HolyCross"; }
	SkillType_t getSkillType() const throw() { return SKILL_HOLY_CROSS; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput( const SkillInput& input, SkillOutput& output ) {}
};

// global variable declaration
extern HolyCross g_HolyCross;

#endif // __SKILL_HOLY_CROSS_HANDLER_H__

