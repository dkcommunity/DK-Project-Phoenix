#include "SkillHandler.h"

#ifndef __SKILL_STRATEGIC_BOMBER_2_HANDLER_H__
#define __SKILL_STRATEGIC_BOMBER_2_HANDLER_H__

class STRATEGICBOMBER2 : public SkillHandler 
{
public:
	STRATEGICBOMBER2() throw() {}
	~STRATEGICBOMBER2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "STRATEGICBOMBER2"; }
	SkillType_t getSkillType() const throw() { return SKILL_STRATEGIC_BOMBER_2; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern STRATEGICBOMBER2 g_STRATEGICBOMBER2;

#endif 
