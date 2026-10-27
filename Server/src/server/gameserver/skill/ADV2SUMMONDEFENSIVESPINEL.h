#ifndef __SKILL_ADV2_SUMMON_DEFENSIVE_SPINEL_HANDLER_H__
#define __SKILL_ADV2_SUMMON_DEFENSIVE_SPINEL_HANDLER_H__

#include "SkillHandler.h"

class ADV2SUMMONDEFENSIVESPINEL : public SkillHandler 
{
public:
	ADV2SUMMONDEFENSIVESPINEL() throw() {}
	~ADV2SUMMONDEFENSIVESPINEL() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2SUMMONDEFENSIVESPINEL"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_SUMMON_DEFENSIVE_SPINEL; }

	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern ADV2SUMMONDEFENSIVESPINEL g_ADV2SUMMONDEFENSIVESPINEL;

#endif
