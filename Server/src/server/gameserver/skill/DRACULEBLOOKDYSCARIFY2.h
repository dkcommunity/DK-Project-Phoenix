//////////////////////////////////////////////////////////////////////////////
// Filename    : DRACULEBLOOKDYSCARIFY2.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_DRACULE_BLOOKDY_SCARIFY_2_HANDLER_H__
#define __SKILL_DRACULE_BLOOKDY_SCARIFY_2_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class DRACULEBLOOKDYSCARIFY2;
//////////////////////////////////////////////////////////////////////////////

class DRACULEBLOOKDYSCARIFY2: public SkillHandler 
{
public:
	DRACULEBLOOKDYSCARIFY2() throw();
	virtual ~DRACULEBLOOKDYSCARIFY2() throw();
	
public:
    string getSkillHandlerName() const throw() { return "DRACULEBLOOKDYSCARIFY2"; }
	SkillType_t getSkillType() const throw() { return SKILL_DRACULE_BLOOKDY_SCARIFY_2; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_SmileofDeath_H__
