//////////////////////////////////////////////////////////////////////////////
// Filename    : SnakeCombo.h 
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_SNAKE_COMBO_HANDLER_H__
#define __SKILL_SNAKE_COMBO_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class SnakeCombo;
//////////////////////////////////////////////////////////////////////////////

class SnakeCombo : public SkillHandler 
{
public:
	SnakeCombo() throw() {}
	~SnakeCombo() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "SnakeCombo"; }
	SkillType_t getSkillType() const throw() { return SKILL_SNAKE_COMBO; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern SnakeCombo g_SnakeCombo;

#endif // __SKILL_SNAKE_COMBO_HANDLER_H__
