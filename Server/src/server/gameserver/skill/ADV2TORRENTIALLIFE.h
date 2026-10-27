//////////////////////////////////////////////////////////////////////////////
// Filename    : Whitsuntide.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ADV2_TORRENTIAL_LIFE_HANDLER_H__
#define __SKILL_ADV2_TORRENTIAL_LIFE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ADV2TORRENTIALLIFE;
//////////////////////////////////////////////////////////////////////////////

class ADV2TORRENTIALLIFE : public SkillHandler 
{
public:
	ADV2TORRENTIALLIFE() throw() {}
	~ADV2TORRENTIALLIFE() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2TORRENTIALLIFE"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_TORRENTIAL_LIFE; }

	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ADV2TORRENTIALLIFE g_ADV2TORRENTIALLIFE;

#endif // __SKILL_ADV2TORRENTIALLIFE_HANDLER_H__
