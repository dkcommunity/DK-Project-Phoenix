//////////////////////////////////////////////////////////////////////////////
// Filename    : CLIENTBATBREAKER.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_CLIENT_SHADY_DOUPLE_HANDLER_H__
#define __SKILL_CLIENT_SHADY_DOUPLE_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class CLIENTSHADYDOUPLE;
//////////////////////////////////////////////////////////////////////////////

class CLIENTSHADYDOUPLE: public SkillHandler 
{
public:
	CLIENTSHADYDOUPLE() throw();
	virtual ~CLIENTSHADYDOUPLE() throw();
	
public:
    string getSkillHandlerName() const throw() { return "CLIENTSHADYDOUPLE"; }
	SkillType_t getSkillType() const throw() { return SKILL_CLIENT_SHADY_DOUPLE; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_CLIENTSHADYDOUPLE_H__
