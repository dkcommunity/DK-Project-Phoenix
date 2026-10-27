//////////////////////////////////////////////////////////////////////////////
// Filename    : GLADIATOR.h 
// Written By  : Sequoia
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_GLADIATOR_HANDLER_H__
#define __SKILL_GLADIATOR_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class GLADIATOR;
//////////////////////////////////////////////////////////////////////////////

class GLADIATOR : public SkillHandler 
{
public:
	GLADIATOR() throw() {}
	~GLADIATOR() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "GLADIATOR"; }
	SkillType_t getSkillType() const throw() { return SKILL_GLADIATOR; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern GLADIATOR g_GLADIATOR;

#endif // __SKILL_GLADIATOR_HANDLER_H__
