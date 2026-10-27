//////////////////////////////////////////////////////////////////////////////
// Filename    : SmileofDeath.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_SmileofDeath_HANDLER_H__
#define __SKILL_SmileofDeath_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class SmileofDeath;
//////////////////////////////////////////////////////////////////////////////

class SmileofDeath: public SkillHandler 
{
public:
	SmileofDeath() throw();
	virtual ~SmileofDeath() throw();
	
public:
    string getSkillHandlerName() const throw() { return "SmileofDeath"; }
	SkillType_t getSkillType() const throw() { return SKILL_SmileofDeath; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_SmileofDeath_H__
