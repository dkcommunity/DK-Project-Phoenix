#include "SkillHandler.h"
#include "Creature.h"
//////////////////////////////////////////////////////////////////////////////
// class MK1_Fire;
//////////////////////////////////////////////////////////////////////////////
#ifndef __SKILL_MK1_Fire_HANDLER_H__
#define __SKILL_MK1_Fire_HANDLER_H__

class MK1Fire : public SkillHandler 
{
public:
	MK1Fire() throw() {}
	~MK1Fire() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "MK1Fire"; }
	SkillType_t getSkillType() const throw() { return SKILL_MK1_Fire; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern MK1Fire g_MK1Fire;

#endif 
