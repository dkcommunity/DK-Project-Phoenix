//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2_LIGHTING_SHOCK2.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ADV2_LIGHTING_SHOCK2_HANDLER_H__
#define __SKILL_ADV2_LIGHTING_SHOCK2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ADV2_LIGHTING_SHOCK2;
//////////////////////////////////////////////////////////////////////////////



class ADV2LIGHTINGSHOCK2 : public SkillHandler 
{
public:
	ADV2LIGHTINGSHOCK2() throw() {}
	~ADV2LIGHTINGSHOCK2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2LIGHTINGSHOCK2"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_LIGHTING_SHOCK2; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ADV2LIGHTINGSHOCK2 g_ADV2LIGHTINGSHOCK2;

#endif // __SKILL_HEAVENLY_SPARK_HANDLER_H__

