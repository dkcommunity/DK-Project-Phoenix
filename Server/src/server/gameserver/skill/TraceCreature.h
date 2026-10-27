//////////////////////////////////////////////////////////////////////////////
// Filename    : TraceCreature.h 
// Written By  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_TRACE_CREATURE_HANDLER_H__
#define __SKILL_TRACE_CREATURE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class TraceCreature;
//////////////////////////////////////////////////////////////////////////////

class TraceCreature: public SkillHandler 
{
public:
	TraceCreature() throw();
	~TraceCreature() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "TraceCreature"; }
	SkillType_t getSkillType() const throw() { return SKILL_TRACE_CREATURE; }

	void execute(Monster* pMonster) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern TraceCreature g_TraceCreature;

#endif // __SKILL_TRACE_CREATURE_HANDLER_H__
