//////////////////////////////////////////////////////////////////////////////
// Filename    : MIHAIBATSTREAM.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_MIHAI_BAT_STREAM_HANDLER_H__
#define __SKILL_MIHAI_BAT_STREAM_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class MIHAIBATSTREAM;
//////////////////////////////////////////////////////////////////////////////

class MIHAIBATSTREAM: public SkillHandler 
{
public:
	MIHAIBATSTREAM() throw();
	virtual ~MIHAIBATSTREAM() throw();
	
public:
    string getSkillHandlerName() const throw() { return "MIHAIBATSTREAM"; }
	SkillType_t getSkillType() const throw() { return SKILL_MIHAI_BAT_STREAM; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_MIHAIBATSTREAM_H__
