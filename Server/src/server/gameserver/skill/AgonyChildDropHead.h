//////////////////////////////////////////////////////////////////////////////
// Filename    : AgonyChildDropHead.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_AGONY_CHILD_DROP_HEAD_HANDLER_H__
#define __SKILL_AGONY_CHILD_DROP_HEAD_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class AgonyChildDropHead;
//////////////////////////////////////////////////////////////////////////////

class AgonyChildDropHead: public SkillHandler 
{
public:
	AgonyChildDropHead() throw();
	virtual ~AgonyChildDropHead() throw();
	
public:
    string getSkillHandlerName() const throw() { return "AgonyChildDropHead"; }
	virtual SkillType_t getSkillType() const throw() { return SKILL_AGONY_CHILD_DROP_HEAD; }

	virtual void execute(Monster *pMonster, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_AGONY_CHILD_DROP_HEAD_HANDLER_H__
