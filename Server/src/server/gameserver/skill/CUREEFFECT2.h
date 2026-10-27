//////////////////////////////////////////////////////////////////////////////
// Filename    : CUREEFFECT2.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_CURE_EFFECT_2_HANDLER_H__
#define __SKILL_CURE_EFFECT_2_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class CUREEFFECT2;
//////////////////////////////////////////////////////////////////////////////

class CUREEFFECT2: public SkillHandler 
{
public:
	CUREEFFECT2() throw();
	virtual ~CUREEFFECT2() throw();
	
public:
    string getSkillHandlerName() const throw() { return "CUREEFFECT2"; }
	SkillType_t getSkillType() const throw() { return SKILL_CURE_EFFECT_2; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_CUREEFFECT2_H__
