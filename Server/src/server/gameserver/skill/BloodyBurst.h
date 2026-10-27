//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodyBurst.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLOODY_BURST_HANDLER_H__
#define __SKILL_BLOODY_BURST_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BloodyBurst;
//////////////////////////////////////////////////////////////////////////////

class BloodyBurst : public SkillHandler 
{
public:
	BloodyBurst() throw() {}
	~BloodyBurst() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BloodyBurst"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLOODY_BURST; }

	void execute(Monster* pMonster) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern BloodyBurst g_BloodyBurst;

#endif // __SKILL_BLOODY_BURST_HANDLER_H__
