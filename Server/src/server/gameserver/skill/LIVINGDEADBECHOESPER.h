//////////////////////////////////////////////////////////////////////////////
// Filename    : LIVINGDEADBECHOESPER.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_LIVINGDEAD_B_ECHO_ESPER_HANDLER_H__
#define __SKILL_LIVINGDEAD_B_ECHO_ESPER_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class LIVINGDEADBECHOESPER;
//////////////////////////////////////////////////////////////////////////////

class LIVINGDEADBECHOESPER: public SkillHandler 
{
public:
	LIVINGDEADBECHOESPER() throw();
	virtual ~LIVINGDEADBECHOESPER() throw();
	
public:
    string getSkillHandlerName() const throw() { return "LIVINGDEADBECHOESPER"; }
	SkillType_t getSkillType() const throw() { return SKILL_LIVINGDEAD_B_ECHO_ESPER; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_LIVINGDEADBECHOESPER_H__
