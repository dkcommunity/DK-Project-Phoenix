//////////////////////////////////////////////////////////////////////////////
// Filename    : TSARBLUEROSEDEVIL.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_TSAR_BLUE_ROSE_DEVIL_HANDLER_H__
#define __SKILL_TSAR_BLUE_ROSE_DEVIL_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class TSARBLUEROSEDEVIL;
//////////////////////////////////////////////////////////////////////////////

class TSARBLUEROSEDEVIL: public SkillHandler 
{
public:
	TSARBLUEROSEDEVIL() throw();
	virtual ~TSARBLUEROSEDEVIL() throw();
	
public:
    string getSkillHandlerName() const throw() { return "TSARBLUEROSEDEVIL"; }
	SkillType_t getSkillType() const throw() { return SKILL_TSAR_BLUE_ROSE_DEVIL; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_TSARBLUEROSEDEVIL_H__
