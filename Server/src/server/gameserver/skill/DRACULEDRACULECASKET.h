//////////////////////////////////////////////////////////////////////////////
// Filename    : DRACULEDRACULECASKET.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_DRACULE_DRACULE_CASKET_HANDLER_H__
#define __SKILL_DRACULE_DRACULE_CASKET_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class DRACULEDRACULECASKET;
//////////////////////////////////////////////////////////////////////////////

class DRACULEDRACULECASKET: public SkillHandler 
{
public:
	DRACULEDRACULECASKET() throw();
	virtual ~DRACULEDRACULECASKET() throw();
	
public:
    string getSkillHandlerName() const throw() { return "DRACULEDRACULECASKET"; }
	SkillType_t getSkillType() const throw() { return SKILL_DRACULE_DRACULE_CASKET; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_DRACULEDRACULECASKET_H__
