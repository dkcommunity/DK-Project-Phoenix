//////////////////////////////////////////////////////////////////////////////
// Filename    : AgonyChildShadowFour.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_AGONY_CHILD_SHADOW_FOUR_HANDLER_H__
#define __SKILL_AGONY_CHILD_SHADOW_FOUR_HANDLER_H__

#include "SkillHandler.h"
#include "Illendue.h"

//////////////////////////////////////////////////////////////////////////////
// class AgonyChildShadowFour;
//////////////////////////////////////////////////////////////////////////////

class AgonyChildShadowFour: public Illendue 
{
public:
	AgonyChildShadowFour() throw();
	virtual ~AgonyChildShadowFour() throw();
	
public:
    string getSkillHandlerName() const throw() { return "AgonyChildShadowFour"; }
	SkillType_t getSkillType() const throw() { return SKILL_AGONY_CHILD_SHADOW_FOUR; }
	
	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_AGONY_CHILD_SHADOW_FOUR_HANDLER_H__
