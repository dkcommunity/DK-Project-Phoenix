//////////////////////////////////////////////////////////////////////////////
// Filename    : DACULETALONOFDDRACULE.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_DACULE_TALON_OF_DDRACULE_H__
#define __SKILL_DACULE_TALON_OF_DDRACULE_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class DACULETALONOFDDRACULE;
//////////////////////////////////////////////////////////////////////////////

class DACULETALONOFDDRACULE: public SkillHandler 
{
public:
	DACULETALONOFDDRACULE() throw();
	virtual ~DACULETALONOFDDRACULE() throw();
	
public:
    string getSkillHandlerName() const throw() { return "DACULETALONOFDDRACULE"; }
	SkillType_t getSkillType() const throw() { return SKILL_DACULE_TALON_OF_DDRACULE; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_DACULETALONOFDDRACULE_H__
