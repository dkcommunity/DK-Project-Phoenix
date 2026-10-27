//////////////////////////////////////////////////////////////////////////////
// Filename    : VillainLadySelfDestruction.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_VILLAIN_SELF_DESTRUCTION_SHADOW_HANDLER_H__
#define __SKILL_VILLAIN_SELF_DESTRUCTION_SHADOW_HANDLER_H__

#include "SkillHandler.h"
#include "SelfDestruction.h"

//////////////////////////////////////////////////////////////////////////////
// class VillainLadySelfDestruction;
//////////////////////////////////////////////////////////////////////////////

class VillainLadySelfDestruction: public SelfDestruction 
{
public:
	VillainLadySelfDestruction() throw();
	virtual ~VillainLadySelfDestruction() throw();
	
public:
    string getSkillHandlerName() const throw() { return "VillainLadySelfDestruction"; }
	SkillType_t getSkillType() const throw() { return SKILL_VILLAIN_LADY_SELF_DESTRUCTION; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_VILLAIN_LADY_GHOST_SHADOW_HANDLER_H__
