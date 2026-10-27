//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodsSymposionAttack.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLOODS_SYMPOSION_ATTACK_HANDLER_H__
#define __SKILL_BLOODS_SYMPOSION_ATTACK_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BloodsSymposionAttack;
//////////////////////////////////////////////////////////////////////////////

class BloodsSymposionAttack : public SkillHandler 
{
public:
	BloodsSymposionAttack() throw() {}
	~BloodsSymposionAttack() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BloodsSymposionAttack"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLOODS_SYMPOSION_ATTACK; }

	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t ObjectID,  VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern BloodsSymposionAttack g_BloodsSymposionAttack;

#endif // __SKILL_BLOODS_SYMPOSION_ATTACK_HANDLER_H__
