//////////////////////////////////////////////////////////////////////////////
// Filename    : HeavenlySpark.h 
// Written By  : bigheart
// Description : 20080401 InfinityThunderBolt °­È­ÆÇ
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_HEAVENLY_SPARK_HANDLER_H__
#define __SKILL_HEAVENLY_SPARK_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class HeavenlySpark;
//////////////////////////////////////////////////////////////////////////////

class HeavenlySpark : public SkillHandler 
{
public:
	HeavenlySpark() throw() {}
	~HeavenlySpark() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "HeavenlySpark"; }
	SkillType_t getSkillType() const throw() { return SKILL_HEAVENLY_SPARK; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern HeavenlySpark g_HeavenlySpark;

#endif // __SKILL_HEAVENLY_SPARK_HANDLER_H__
