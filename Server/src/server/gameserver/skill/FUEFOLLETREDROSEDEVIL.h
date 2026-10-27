//////////////////////////////////////////////////////////////////////////////
// Filename    : FUEFOLLETREDROSEDEVIL.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_FUE_FOLLET_RED_ROSE_DEVIL_HANDLER_H__
#define __SKILL_FUE_FOLLET_RED_ROSE_DEVIL_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class FUEFOLLETREDROSEDEVIL;
//////////////////////////////////////////////////////////////////////////////

class FUEFOLLETREDROSEDEVIL: public SkillHandler 
{
public:
	FUEFOLLETREDROSEDEVIL() throw();
	virtual ~FUEFOLLETREDROSEDEVIL() throw();
	
public:
    string getSkillHandlerName() const throw() { return "FUEFOLLETREDROSEDEVIL"; }
	SkillType_t getSkillType() const throw() { return SKILL_FUE_FOLLET_RED_ROSE_DEVIL; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __FUEFOLLETREDROSEDEVIL_H__
