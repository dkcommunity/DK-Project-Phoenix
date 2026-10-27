#ifndef __SKILL_DEVIL_WAVE_H__
#define __SKILL_DEVIL_WAVE_H__

#include "SkillHandler.h"

class DEVILWAVE: public SkillHandler 
{
public:
	DEVILWAVE() throw() {}
	~DEVILWAVE() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "DEVILWAVE"; }
	SkillType_t getSkillType() const throw() { return SKILL_DEVIL_WAVE; }

	void execute(Monster *pMonster, Creature *pEnemy) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern DEVILWAVE g_DEVILWAVE;

#endif
