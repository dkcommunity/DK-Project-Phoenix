//////////////////////////////////////////////////////////////////////////////
// Filename    : BLOODYSKULL2.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLOODY_SKULL_2_HANDLER_H__
#define __SKILL_BLOODY_SKULL_2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BLOODYSKULL;
//////////////////////////////////////////////////////////////////////////////

class BLOODYSKULL2 : public SkillHandler 
{
public:
	BLOODYSKULL2() throw() {}
	~BLOODYSKULL2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BLOODYSKULL2"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLOODY_SKULL_2; }

	virtual SkillResultType execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y,  VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t,  VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	void execute(Monster*, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global BLOODYSKULL declaration
extern BLOODYSKULL2 g_BLOODYSKULL2;

#endif // __SKILL_BLOODY_SKULL_HANDLER_H__
