//////////////////////////////////////////////////////////////////////////////
// Filename    : FuryShock.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_FuryShock_HANDLER_H__
#define __SKILL_FuryShock_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class FuryShock;
//////////////////////////////////////////////////////////////////////////////

class FuryShock: public SkillHandler 
{
public:
	FuryShock() throw();
	virtual ~FuryShock() throw();
	
public:
    string getSkillHandlerName() const throw() { return "FuryShock"; }
	SkillType_t getSkillType() const throw() { return SKILL_FuryShock; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_FuryShock_H__
