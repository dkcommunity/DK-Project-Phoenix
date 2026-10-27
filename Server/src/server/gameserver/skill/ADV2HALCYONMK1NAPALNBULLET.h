#include "SkillHandler.h"

#ifndef __SKILL_ADV2_HALCYON_MK1_NAPALN_BULLET_HANDLER_H__
#define __SKILL_ADV2_HALCYON_MK1_NAPALN_BULLET_HANDLER_H__

class ADV2HALCYONMK1NAPALNBULLET : public SkillHandler 
{
public:
	ADV2HALCYONMK1NAPALNBULLET() throw() {}
	~ADV2HALCYONMK1NAPALNBULLET() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2HALCYONMK1NAPALNBULLET"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_HALCYON_MK1_NAPALN_BULLET; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern ADV2HALCYONMK1NAPALNBULLET g_ADV2HALCYONMK1NAPALNBULLET;

#endif 
