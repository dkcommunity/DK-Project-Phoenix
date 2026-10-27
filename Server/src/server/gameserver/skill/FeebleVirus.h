//////////////////////////////////////////////////////////////////////////////
// Filename    : FeebleVirus.h 
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_FEEBLE_VIRUS_HANDLER_H__
#define __SKILL_FEEBLE_VIRUS_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class FeebleVirus;
//////////////////////////////////////////////////////////////////////////////
class FeebleVirus: public SkillHandler 
{
public:
	FeebleVirus() throw() {}
	~FeebleVirus() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "FeebleVirus"; }
	SkillType_t getSkillType() const throw() { return SKILL_FEEBLE_VIRUS; }

	void execute(Monster* pMonster, Creature* pEnemy) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern FeebleVirus g_FeebleVirus;

#endif // __SKILL_FEEBLE_VIRUS_HANDLER_H__

