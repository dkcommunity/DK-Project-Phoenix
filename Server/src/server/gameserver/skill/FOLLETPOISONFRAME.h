//////////////////////////////////////////////////////////////////////////////
// Filename    : FOLLETPOISONFRAME.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_FOLLET_POISON_FRAME_HANDLER_H__
#define __SKILL_FOLLET_POISON_FRAME_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class FOLLETPOISONFRAME;
//////////////////////////////////////////////////////////////////////////////

class FOLLETPOISONFRAME: public SkillHandler 
{
public:
	FOLLETPOISONFRAME() throw();
	virtual ~FOLLETPOISONFRAME() throw();
	
public:
    string getSkillHandlerName() const throw() { return "FOLLETPOISONFRAME"; }
	SkillType_t getSkillType() const throw() { return SKILL_FOLLET_POISON_FRAME; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_FOLLETPOISONFRAME_H__
