#ifndef __SKILL_GENESIS_ATTACK_H__
#define __SKILL_GENESIS_ATTACK_H__

#include "SkillHandler.h"

class GENESISATTACK: public SkillHandler 
{
public:
	GENESISATTACK() throw() {}
	~GENESISATTACK() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "GENESISATTACK"; }
	SkillType_t getSkillType() const throw() { return SKILL_GENESIS_ATTACK; }

	void execute(Monster *pMonster, Creature *pEnemy) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern GENESISATTACK g_GENESISATTACK;

#endif
