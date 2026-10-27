//////////////////////////////////////////////////////////////////////////////
// Filename    : HolyArmor.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_DEPEND_SOUL_HANDLER_H__
#define __SKILL_DEPEND_SOUL_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class HolyArmor;
//////////////////////////////////////////////////////////////////////////////

class DEPENDSOUL : public SkillHandler 
{
public:
	DEPENDSOUL() throw() {}
	~DEPENDSOUL() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "DEPENDSOUL"; }
	SkillType_t getSkillType() const throw() { return SKILL_DEPEND_SOUL; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern DEPENDSOUL g_DEPENDSOUL;

#endif // __SKILL_HOLY_ARMOR_HANDLER_H__
