#ifndef __SKILL_HALF_MOON_KICK_H__
#define __SKILL_HALF_MOON_KICK_H__

#include "SkillHandler.h"

class HALFMOONKICK : public SkillHandler 
{
public:
	HALFMOONKICK() throw() {}
	~HALFMOONKICK() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "HALFMOONKICK"; }
	SkillType_t getSkillType() const throw() { return SKILL_HALF_MOON_KICK; }

	void execute(Monster *pMonster, Creature *pEnemy) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern HALFMOONKICK g_HALFMOONKICK;

#endif
