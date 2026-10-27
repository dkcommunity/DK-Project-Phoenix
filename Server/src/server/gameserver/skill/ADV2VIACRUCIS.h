//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2VIACRUCIS.h 
// Written By  : bigheart
// Description : 20080401 InfinityThunderBolt °­È­ÆÇ
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ADV2_VIA_CRUCIS_HANDLER_H__
#define __SKILL_ADV2_VIA_CRUCIS_HANDLER_H__


#include "SkillHandler.h"
#include "Creature.h"
//////////////////////////////////////////////////////////////////////////////
// class ADV2VIACRUCIS;
//////////////////////////////////////////////////////////////////////////////

class ADV2VIACRUCIS : public SkillHandler 
{
public:
	ADV2VIACRUCIS() throw() {}
	~ADV2VIACRUCIS() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2VIACRUCIS"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_VIA_CRUCIS; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ADV2VIACRUCIS g_ADV2VIACRUCIS;

#endif // __SKILL_HEAVENLY_SPARK_HANDLER_H__
