#ifndef __SKILL_SNIPING2_HANDLER_H__
#define __SKILL_SNIPING2_HANDLER_H__

#include "SkillHandler.h"

class Sniping2: public SkillHandler 
{
public:
	Sniping2() throw() {}
	~Sniping2() throw() {}

public:
    string getSkillHandlerName() const throw() { return "Sniping2"; }
	SkillType_t getSkillType() const throw() { return SKILL_SNIPING2; }

	SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);

public:
	void checkRevealRatio2(Creature* pCreature, int base, int divisor);
};

extern Sniping2 g_Sniping2;

#endif
