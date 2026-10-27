#ifndef __SKILL_BONE_STRIKE_H__
#define __SKILL_BONE_STRIKE_H__

#include "SkillHandler.h"

class BONESTRIKE: public SkillHandler 
{
public:
	BONESTRIKE() throw() {}
	~BONESTRIKE() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BONESTRIKE"; }
	SkillType_t getSkillType() const throw() { return SKILL_BONE_STRIKE; }

	void execute(Monster *pMonster, Creature *pEnemy) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern BONESTRIKE g_BONESTRIKE;

#endif
