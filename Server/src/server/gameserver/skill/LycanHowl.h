//////////////////////////////////////////////////////////////////////////////
// Filename    : LycanHowl.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_Lycan_Howl_HANDLER_H__
#define __SKILL_Lycan_Howl_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class LycanHowl;
//////////////////////////////////////////////////////////////////////////////

class LycanHowl: public SkillHandler 
{
public:
	LycanHowl() throw();
	virtual ~LycanHowl() throw();
	
public:
    string getSkillHandlerName() const throw() { return "LycanHowl"; }
	SkillType_t getSkillType() const throw() { return SKILL_Lycan_Howl; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_Lycan_Howl_H__
