//////////////////////////////////////////////////////////////////////////////
// Filename    : GoreGlandFire.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_DEADLY_ATT_HANDLER_H__
#define __SKILL_DEADLY_ATT_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class DEADLYATT;
//////////////////////////////////////////////////////////////////////////////

class DEADLYATT : public SkillHandler 
{
public:
	DEADLYATT() throw() {}
	~DEADLYATT() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "DEADLYATT"; }
	SkillType_t getSkillType() const throw() { return SKILL_DEADLY_ATT; }

	virtual SkillResultType execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void execute(Monster* pMonster, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);

};

// global variable declaration
extern DEADLYATT g_DEADLYATT;

#endif // __SKILL_GORE_GLAND_FIRE_HANDLER_H__
