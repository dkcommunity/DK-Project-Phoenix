#ifndef __SKILL_ADV2_CHAIN_LIGHTNING_SWORD_HANDLER_H__
#define __SKILL_ADV2_CHAIN_LIGHTNING_SWORD_HANDLER_H__

#include "SkillHandler.h"

class ADV2CHAINLIGHTNINGSWORD : public SkillHandler 
{
public:
	ADV2CHAINLIGHTNINGSWORD() throw() {}
	~ADV2CHAINLIGHTNINGSWORD() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2CHAINLIGHTNINGSWORD"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_CHAIN_LIGHTNING_SWORD; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);


	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern ADV2CHAINLIGHTNINGSWORD g_ADV2CHAINLIGHTNINGSWORD;

#endif
