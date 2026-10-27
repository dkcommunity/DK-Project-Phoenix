//////////////////////////////////////////////////////////////////////////////
// Filename    : MagicShield1.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_MAGIC_SHIELD1_HANDLER_H__
#define __SKILL_MAGIC_SHIELD1_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class MagicShield1;
//////////////////////////////////////////////////////////////////////////////

class MagicShield1 : public SkillHandler 
{
public:
	MagicShield1() throw() {}
	~MagicShield1() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "MagicShield1"; }
	SkillType_t getSkillType() const throw() { return SKILL_MAGIC_SHIELD1; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern MagicShield1 g_MagicShield1;

#endif // __SKILL_MAGIC_SHIELD_HANDLER_H__
