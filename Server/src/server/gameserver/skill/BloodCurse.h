//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodCurse.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLOOD_CURSE_HANDLER_H__
#define __SKILL_BLOOD_CURSE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BloodCurse;
//////////////////////////////////////////////////////////////////////////////

class BloodCurse : public SkillHandler 
{
public:
	BloodCurse() throw() {}
	~BloodCurse() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BloodCurse"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLOOD_CURSE; }

	virtual SkillResultType execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);


	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern BloodCurse g_BloodCurse;

#endif // __SKILL_BLOOD_CURSE_HANDLER_H__
