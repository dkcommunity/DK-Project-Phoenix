//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV3BATSPEAR2.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BAT_SPEAR_2_HANDLER_H__
#define __SKILL_BAT_SPEAR_2_HANDLER_H__

#include "SkillHandler.h"


class BATSPEAR2 : public SkillHandler 
{
public:
	BATSPEAR2() throw() {}
	~BATSPEAR2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BATSPEAR2"; }
	SkillType_t getSkillType() const throw() { return SKILL_BAT_SPEAR_2; }

	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t ObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern BATSPEAR2 g_BATSPEAR2;

#endif

