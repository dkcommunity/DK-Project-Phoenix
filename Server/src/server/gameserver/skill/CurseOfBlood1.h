//////////////////////////////////////////////////////////////////////////////
// Filename    : CurseOfBlood1.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_CURSE_OF_BLOOD1_HANDLER_H__
#define __SKILL_CURSE_OF_BLOOD1_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class CurseOfBlood1;
//////////////////////////////////////////////////////////////////////////////

class CurseOfBlood1 : public SkillHandler 
{
public:
	CurseOfBlood1() throw() {}
	~CurseOfBlood1() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "CurseOfBlood1"; }
	SkillType_t getSkillType() const throw() { return SKILL_CURSE_OF_BLOOD1; }

	virtual SkillResultType execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern CurseOfBlood1 g_CurseOfBlood1;

#endif // __SKILL_CURSE_OF_BLOOD_HANDLER_H__
