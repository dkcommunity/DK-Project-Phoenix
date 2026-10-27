#ifndef __SKILL_METEORIC_FALL_2_HANDLER_H__
#define __SKILL_METEORIC_FALL_2_HANDLER_H__

#include "SkillHandler.h"

class METEORICFALL2: public SkillHandler 
{
public:
	METEORICFALL2() throw() {}
	~METEORICFALL2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "METEORICFALL2"; }

	SkillType_t getSkillType() const throw() { return SKILL_METEORIC_FALL_2; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	void execute(Monster*, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);
	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern METEORICFALL2 g_METEORICFALL2;

#endif
