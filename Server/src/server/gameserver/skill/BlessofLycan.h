//////////////////////////////////////////////////////////////////////////////
// Filename    : BlessofLycan.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BlessofLycan_HANDLER_H__
#define __SKILL_BlessofLycan_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class BlessofLycan;
//////////////////////////////////////////////////////////////////////////////

class BlessofLycan: public SkillHandler 
{
public:
	BlessofLycan() throw();
	virtual ~BlessofLycan() throw();
	
public:
    string getSkillHandlerName() const throw() { return "BlessofLycan"; }
	SkillType_t getSkillType() const throw() { return SKILL_BlessofLycan; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_BlessofLycan_H__
