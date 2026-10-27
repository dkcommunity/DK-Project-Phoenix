//////////////////////////////////////////////////////////////////////////////
// Filename    : BatStorm2.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BAT_STORM2_HANDLER_H__
#define __SKILL_BAT_STORM2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BatStorm2;
//////////////////////////////////////////////////////////////////////////////

class BatStorm2 : public SkillHandler 
{
public:
	BatStorm2() throw() {}
	~BatStorm2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BatStorm2"; }
	SkillType_t getSkillType() const throw() { return SKILL_BAT_STORM2; }

	virtual SkillResultType execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y,  VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t,  VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	void execute(Monster*, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern BatStorm2 g_BatStorm2;

#endif // __SKILL_BAT_STORM2_HANDLER_H__
