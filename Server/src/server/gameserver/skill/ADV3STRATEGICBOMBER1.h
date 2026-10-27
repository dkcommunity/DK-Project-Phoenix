#include "SkillHandler.h"

#ifndef __SKILL_STRATEGIC_BOMBER_1_HANDLER_H__
#define __SKILL_STRATEGIC_BOMBER_1_HANDLER_H__

class STRATEGICBOMBER1 : public SkillHandler 
{
public:
	STRATEGICBOMBER1() throw() {}
	~STRATEGICBOMBER1() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "STRATEGICBOMBER1"; }
	SkillType_t getSkillType() const throw() { return SKILL_STRATEGIC_BOMBER_1; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern STRATEGICBOMBER1 g_STRATEGICBOMBER1;

#endif 
