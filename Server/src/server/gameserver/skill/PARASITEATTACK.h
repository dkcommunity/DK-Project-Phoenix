#ifndef __SKILL_PARASITE_ATTACK_H__
#define __SKILL_PARASITE_ATTACK_H__

#include "SkillHandler.h"

class PARASITEATTACK: public SkillHandler 
{
public:
	PARASITEATTACK() throw() {}
	~PARASITEATTACK() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "PARASITEATTACK"; }
	SkillType_t getSkillType() const throw() { return SKILL_PARASITE_ATTACK; }

	void execute(Monster *pMonster, Creature *pEnemy) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern PARASITEATTACK g_PARASITEATTACK;

#endif
