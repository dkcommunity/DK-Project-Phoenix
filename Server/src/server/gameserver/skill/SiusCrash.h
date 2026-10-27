//////////////////////////////////////////////////////////////////////////////
// Filename    : SiusCrash.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_Sius_Crash_HANDLER_H__
#define __SKILL_Sius_Crash_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class SiusCrash;
//////////////////////////////////////////////////////////////////////////////

class SiusCrash: public SkillHandler 
{
public:
	SiusCrash() throw(){}
	virtual ~SiusCrash() throw(){}
	
public:
    string getSkillHandlerName() const throw() { return "SiusCrash"; }
	SkillType_t getSkillType() const throw() { return SKILL_Sius_Crash; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_Sius_Crash_H__
