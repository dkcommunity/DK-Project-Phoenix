#ifndef __SKILL_DEVIL_SCREAM_H__
#define __SKILL_DEVIL_SCREAM_H__

#include "SkillHandler.h"

class DEVILSCREAM: public SkillHandler 
{
public:
	DEVILSCREAM() throw() {}
	~DEVILSCREAM() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "PARASITEATTACK"; }
	SkillType_t getSkillType() const throw() { return SKILL_DEVIL_SCREAM; }

	void execute(Monster *pMonster, Creature *pEnemy) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern DEVILSCREAM g_DEVILSCREAM;

#endif
