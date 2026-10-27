//////////////////////////////////////////////////////////////////////////////
// Filename    : Striking2.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_STRIKING_2_HANDLER_H__
#define __SKILL_STRIKING_2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class Striking2;
//////////////////////////////////////////////////////////////////////////////

class STRIKING2 : public SkillHandler 
{
public:
	STRIKING2() throw() {}
	~STRIKING2() throw() {}

public:
    string getSkillHandlerName() const throw() { return "STRIKING2"; }
	SkillType_t getSkillType() const throw() { return SKILL_STRIKING_2; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern STRIKING2 g_STRIKING2;

#endif // __SKILL_STRIKING_HANDLER_2_H__
