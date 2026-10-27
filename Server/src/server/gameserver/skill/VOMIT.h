#ifndef __SKILL_VOMIT_HANDLER_H__
#define __SKILL_VOMIT_HANDLER_H__

#include "SkillHandler.h"

class VOMIT : public SkillHandler 
{
public:
	VOMIT() throw() {}
	~VOMIT() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "VOMIT"; }
	SkillType_t getSkillType() const throw() { return SKILL_VOMIT; }

	void execute(Monster* pMonster, Creature* pEnemy) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern VOMIT g_VOMIT;

#endif
