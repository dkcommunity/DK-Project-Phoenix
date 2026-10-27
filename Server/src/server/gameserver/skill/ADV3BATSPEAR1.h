//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV3BATSPEAR1.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BAT_SPEAR_1_HANDLER_H__
#define __SKILL_BAT_SPEAR_1_HANDLER_H__

#include "SkillHandler.h"


class BATSPEAR1 : public SkillHandler 
{
public:
	BATSPEAR1() throw() {}
	~BATSPEAR1() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BATSPEAR1"; }
	SkillType_t getSkillType() const throw() { return SKILL_BAT_SPEAR_1; }

	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t ObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern BATSPEAR1 g_BATSPEAR1;

#endif

