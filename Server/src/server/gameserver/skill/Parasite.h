//////////////////////////////////////////////////////////////////////////////
// Filename    : Parasite.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_PARASITE_HANDLER_H__
#define __SKILL_PARASITE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class Parasite;
//////////////////////////////////////////////////////////////////////////////

class Parasite : public SkillHandler 
{
public:
	Parasite() throw() {}
	~Parasite() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "Parasite"; }
	SkillType_t getSkillType() const throw() { return SKILL_PARASITE; }

	void execute(Monster* pMonster, Creature* pEnemy) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern Parasite g_Parasite;

#endif // __SKILL_PARASITE_HANDLER_H__
