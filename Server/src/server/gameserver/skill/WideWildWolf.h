//////////////////////////////////////////////////////////////////////////////
// Filename    : WideWildWolf.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_Wide_Wild_Wolf_HANDLER_H__
#define __SKILL_Wide_Wild_Wolf_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class WideWildWolf;
//////////////////////////////////////////////////////////////////////////////

class WideWildWolf: public SkillHandler 
{
public:
	WideWildWolf() throw();
	virtual ~WideWildWolf() throw();
	
public:
    string getSkillHandlerName() const throw() { return "WideWildWolf"; }
	SkillType_t getSkillType() const throw() { return SKILL_Wide_Wild_Wolf; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_Wide_Wild_Wolf_H__
