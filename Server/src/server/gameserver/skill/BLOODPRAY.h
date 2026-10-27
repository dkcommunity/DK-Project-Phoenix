#ifndef __SKILL_BLOOD_PRAY_H__
#define __SKILL_BLOOD_PRAY_H__

#include "SkillHandler.h"

class BLOODPRAY: public SkillHandler 
{
public:
	BLOODPRAY() throw() {}
	~BLOODPRAY() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BLOODPRAY"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLOOD_PRAY; }

	void execute(Monster *pMonster, Creature *pEnemy) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern BLOODPRAY g_BLOODPRAY;

#endif
