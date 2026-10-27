#ifndef __SKILL_METEORIC_FALL_1_HANDLER_H__
#define __SKILL_METEORIC_FALL_1_HANDLER_H__

#include "SkillHandler.h"

class METEORICFALL1: public SkillHandler 
{
public:
	METEORICFALL1() throw() {}
	~METEORICFALL1() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "METEORICFALL1"; }

	SkillType_t getSkillType() const throw() { return SKILL_METEORIC_FALL_1; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern METEORICFALL1 g_METEORICFALL1;

#endif
