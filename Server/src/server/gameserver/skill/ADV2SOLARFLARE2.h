//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2SOLARFLARE .h 
// Written By  : bigheart
// Description : 20080401 InfinityThunderBolt °­È­ÆÇ
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ADV2_SOLAR_FLARE2_HANDLER_H__
#define __SKILL_ADV2_SOLAR_FLARE2_HANDLER_H__


#include "SkillHandler.h"
#include "Creature.h"
//////////////////////////////////////////////////////////////////////////////
// class RottenApple;
//////////////////////////////////////////////////////////////////////////////

class ADV2SOLARFLARE2 : public SkillHandler 
{
public:
	ADV2SOLARFLARE2() throw() {}
	~ADV2SOLARFLARE2() throw() {}
	
public :
    string getSkillHandlerName() const throw() { return "ADV2SOLARFLARE2"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_SOLAR_FLARE2; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ADV2SOLARFLARE2 g_ADV2SOLARFLARE2;

#endif // __SKILL_ROTTEN_APPLE_HANDLER_H__
