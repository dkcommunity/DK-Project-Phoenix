//////////////////////////////////////////////////////////////////////////////
// Filename    : RAGEOFBLOOD.h 
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_RAGE_OF_BLOOD_HANDLER_H__
#define __SKILL_RAGE_OF_BLOOD_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class RAGEOFBLOOD;
//////////////////////////////////////////////////////////////////////////////

class RAGEOFBLOOD : public SkillHandler 
{
public:
	RAGEOFBLOOD() throw() {}
	~RAGEOFBLOOD() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "RAGEOFBLOOD"; }
	SkillType_t getSkillType() const throw() { return SKILL_RAGE_OF_BLOOD; }

	virtual SkillResultType execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern RAGEOFBLOOD g_RAGEOFBLOOD;

#endif // __SKILL_SUMMON_CASKET_HANDLER_H__
