//////////////////////////////////////////////////////////////////////////////
// Filename    : VillainLadyGhostShadow.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_VILLAIN_LADY_GHOST_SHADOW_HANDLER_H__
#define __SKILL_VILLAIN_LADY_GHOST_SHADOW_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class VillainLadyGhostShadow;
//////////////////////////////////////////////////////////////////////////////

class VillainLadyGhostShadow: public SkillHandler 
{
public:
	VillainLadyGhostShadow() throw();
	virtual ~VillainLadyGhostShadow() throw();
	
public:
    string getSkillHandlerName() const throw() { return "VillainLadyGhostShadow"; }
	SkillType_t getSkillType() const throw() { return SKILL_VILLAIN_LADY_GHOST_SHADOW; }

	virtual void execute(Monster *pMonster, Creature *pEnemy) throw(Error);
	
	void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_VILLAIN_LADY_GHOST_SHADOW_HANDLER_H__
