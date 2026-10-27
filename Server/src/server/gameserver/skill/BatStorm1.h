//////////////////////////////////////////////////////////////////////////////
// Filename    : BatStorm1.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BAT_STORM1_HANDLER_H__
#define __SKILL_BAT_STORM1_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BatStorm1;
//////////////////////////////////////////////////////////////////////////////

class BatStorm1 : public SkillHandler 
{
public:
	BatStorm1() throw() {}
	~BatStorm1() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BatStorm1"; }
	SkillType_t getSkillType() const throw() { return SKILL_BAT_STORM1; }

	virtual SkillResultType execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y,  VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t,  VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	void execute(Monster*, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern BatStorm1 g_BatStorm1;

#endif // __SKILL_BAT_STORM1_HANDLER_H__
