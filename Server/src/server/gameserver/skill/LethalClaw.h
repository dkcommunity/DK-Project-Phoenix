//////////////////////////////////////////////////////////////////////////////
// Filename    : LethalClaw.h 
// Written By  : bigheart 
// Description : 20080410 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_LETHAL_CLAW_HANDLER_H__
#define __SKILL_LETHAL_CLAW_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class LethalClaw;
//////////////////////////////////////////////////////////////////////////////

class LethalClaw : public SkillHandler 
{
public:
	LethalClaw() throw();
	~LethalClaw() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "LethalClaw"; }
	SkillType_t getSkillType() const throw() { return SKILL_LETHAL_CLAW; }

	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t ObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void execute(Monster* pMonster, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);

protected:
	POINT m_pLethalClawMask[9];
};

// global variable declaration
extern LethalClaw g_LethalClaw;

#endif // __SKILL_LETHALCLAW_HANDLER_H__
