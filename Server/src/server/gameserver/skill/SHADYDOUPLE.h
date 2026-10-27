//////////////////////////////////////////////////////////////////////////////
// Filename    : AcidBolt.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_SHADY_DOUPLE_HANDLER_H__
#define __SKILL_SHADY_DOUPLE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class AcidBolt;
//////////////////////////////////////////////////////////////////////////////

class SHADYDOUPLE : public SkillHandler 
{
public:
	SHADYDOUPLE() throw() {}
	~SHADYDOUPLE() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "SHADYDOUPLE"; }
	SkillType_t getSkillType() const throw() { return SKILL_SHADY_DOUPLE; }

	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t ObjectID,  VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	void execute(Monster* pMonster, Creature* pEnemy) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern SHADYDOUPLE g_SHADYDOUPLE;

#endif // __SKILL_ACID_BOLT_HANDLER_H__
