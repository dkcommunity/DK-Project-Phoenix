//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2_THE_RAGE_OF_GUARDIAN_SPRIT.h 
// Written By  : Evade xiu
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ADV2_THE_RAGE_OF_GUARDIAN_SPRIT_HANDLER_H__
#define __SKILL_ADV2_THE_RAGE_OF_GUARDIAN_SPRIT_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ADV2_THE_RAGE_OF_GUARDIAN_SPRIT;
//////////////////////////////////////////////////////////////////////////////

class ADV2THERAGEOFGUARDIANSPRIT : public SkillHandler 
{
public:
	ADV2THERAGEOFGUARDIANSPRIT() throw() {}
	~ADV2THERAGEOFGUARDIANSPRIT() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2THERAGEOFGUARDIANSPRIT"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_THE_RAGE_OF_GUARDIAN_SPRIT; }

	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ADV2THERAGEOFGUARDIANSPRIT g_ADV2THERAGEOFGUARDIANSPRIT;

#endif // __SKILL_ADV2THERAGEOFGUARDIANSPRIT_HANDLER_H__
