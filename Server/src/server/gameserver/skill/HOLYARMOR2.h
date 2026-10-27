//////////////////////////////////////////////////////////////////////////////
// Filename    : HOLYARMOR2.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_HOLY_ARMOR_2_HANDLER_H__
#define __SKILL_HOLY_ARMOR_2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class HOLYARMOR2;
//////////////////////////////////////////////////////////////////////////////

class HOLYARMOR2 : public SkillHandler 
{
public:
	HOLYARMOR2() throw() {}
	~HOLYARMOR2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "HOLYARMOR2"; }
	SkillType_t getSkillType() const throw() { return SKILL_HOLY_ARMOR_2; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern HOLYARMOR2 g_HOLYARMOR2;

#endif // __SKILL_HOLY_ARMOR_HANDLER_2_H__
