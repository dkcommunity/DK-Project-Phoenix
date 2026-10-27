//////////////////////////////////////////////////////////////////////////////
// Filename    : MagicShield2.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_MAGIC_SHIELD2_HANDLER_H__
#define __SKILL_MAGIC_SHIELD2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class MagicShield2;
//////////////////////////////////////////////////////////////////////////////

class MagicShield2 : public SkillHandler 
{
public:
	MagicShield2() throw() {}
	~MagicShield2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "MagicShield2"; }
	SkillType_t getSkillType() const throw() { return SKILL_MAGIC_SHIELD2; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern MagicShield2 g_MagicShield2;

#endif // __SKILL_MAGIC_SHIELD2_HANDLER_H__
