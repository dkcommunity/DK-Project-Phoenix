//////////////////////////////////////////////////////////////////////////////
// Filename    : PartyAura.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_PARTY_AURA_HANDLER_H__
#define __SKILL_PARTY_AURA_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class PartyAura;
//////////////////////////////////////////////////////////////////////////////

class PartyAura : public SkillHandler 
{
public:
	PartyAura() throw() {}
	~PartyAura() throw() {}

public:
    string getSkillHandlerName() const throw() { return "PartyAura"; }
	SkillType_t getSkillType() const throw() { return SKILL_PARTY_AURA; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSlayerSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput( const SkillInput& input, SkillOutput& output ) {}
};

// global variable declaration
extern PartyAura g_PartyAura;

#endif // __SKILL_PARTY_AURA_HANDLER_H__

