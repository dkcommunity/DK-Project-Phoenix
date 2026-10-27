//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodyShout1.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ADV2_THE_ROAR_OF_AWAKEN_WARRIOR_HANDLER_H__
#define __SKILL_ADV2_THE_ROAR_OF_AWAKEN_WARRIOR_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ADV2THEROAROFAWAKENWARRIOR;
//////////////////////////////////////////////////////////////////////////////

class ADV2THEROAROFAWAKENWARRIOR : public SkillHandler 
{
public:
	ADV2THEROAROFAWAKENWARRIOR() throw() {}
	~ADV2THEROAROFAWAKENWARRIOR() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2THEROAROFAWAKENWARRIOR"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_THE_ROAR_OF_AWAKEN_WARRIOR; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ADV2THEROAROFAWAKENWARRIOR g_ADV2THEROAROFAWAKENWARRIOR;

#endif // __SKILL_ADV2THEROAROFAWAKENWARRIOR_HANDLER_H__
