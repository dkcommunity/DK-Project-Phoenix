//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2_LIGHTING_SHOCK.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ADV2_LIGHTING_SHOCK_HANDLER_H__
#define __SKILL_ADV2_LIGHTING_SHOCK_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ADV2_LIGHTING_SHOCK;
//////////////////////////////////////////////////////////////////////////////



class ADV2LIGHTINGSHOCK : public SkillHandler 
{
public:
	ADV2LIGHTINGSHOCK() throw() {}
	~ADV2LIGHTINGSHOCK() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2LIGHTINGSHOCK"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_LIGHTING_SHOCK; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ADV2LIGHTINGSHOCK g_ADV2LIGHTINGSHOCK;

#endif // __SKILL_HEAVENLY_SPARK_HANDLER_H__

