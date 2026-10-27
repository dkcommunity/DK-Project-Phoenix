//////////////////////////////////////////////////////////////////////////////
// Filename    : SoulChain.h 
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_SOUL_CHAIN_HANDLER_H__
#define __SKILL_SOUL_CHAIN_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class SoulChain;
//////////////////////////////////////////////////////////////////////////////

class SoulChain: public SkillHandler 
{
public:
	SoulChain() throw() {}
	~SoulChain() throw() {}
	
protected:
	bool isValidTargetZone(Zone *pTargetZone);

public:
    string getSkillHandlerName() const throw() { return "SoulChain"; }
	SkillType_t getSkillType() const throw() { return SKILL_SOUL_CHAIN; }

	virtual SkillResultType execute(Slayer* pSlayer, const string& targetName, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, const string& targetName, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, const string& targetName, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern SoulChain g_SoulChain;

#endif // __SKILL_SUMMON_CASKET_HANDLER_H__
