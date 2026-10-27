#include "SkillHandler.h"
#include "Creature.h"
//////////////////////////////////////////////////////////////////////////////
// class MK1_Fire2;
//////////////////////////////////////////////////////////////////////////////
#ifndef __SKILL_MK1_Fire2_HANDLER_H__
#define __SKILL_MK1_Fire2_HANDLER_H__

class MK1Fire2 : public SkillHandler 
{
public:
	MK1Fire2() throw() {}
	~MK1Fire2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "MK1Fire2"; }
	SkillType_t getSkillType() const throw() { return SKILL_MK1_Fire2; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern MK1Fire2 g_MK1Fire2;

#endif 
