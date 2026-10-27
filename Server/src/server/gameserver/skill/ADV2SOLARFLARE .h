//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2SOLARFLARE .h 
// Written By  : bigheart
// Description : 20080401 InfinityThunderBolt °­È­ÆÇ
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ADV2_SOLAR_FLARE_HANDLER_H__
#define __SKILL_ADV2_SOLAR_FLARE_HANDLER_H__



#include "SkillHandler.h"
#include "Creature.h"
//////////////////////////////////////////////////////////////////////////////
// class ADV2SOLARFLARE;
//////////////////////////////////////////////////////////////////////////////

class ADV2SOLARFLARE : public SkillHandler 
{
public:
	ADV2SOLARFLARE() throw() {}
	~ADV2SOLARFLARE() throw() {}
	
public :
    string getSkillHandlerName() const throw() { return "ADV2SOLARFLARE"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_SOLAR_FLARE; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ADV2SOLARFLARE g_ADV2SOLARFLARE;
#endif // __SKILL_HEAVENLY_SPARK_HANDLER_H__
