//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2CYCLINGWARROCK.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ADV2_CYCLING_WARROCK_HANDLER_H__
#define __SKILL_ADV2_CYCLING_WARROCK_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ADV2CYCLINGWARROCK;
//////////////////////////////////////////////////////////////////////////////

class ADV2CYCLINGWARROCK : public SkillHandler 
{
public:
	ADV2CYCLINGWARROCK() throw() {}
	~ADV2CYCLINGWARROCK() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2CYCLINGWARROCK"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_CYCLING_WARROCK; }

	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ADV2CYCLINGWARROCK g_ADV2CYCLINGWARROCK;

#endif // __SKILL_RING_OF_FLARE_HANDLER_H__
