#ifndef __SKILL_LIGHTNING_SLASH_HANDLER_H__
#define __SKILL_LIGHTNING_SLASH_HANDLER_H__

#include "SkillHandler.h"

class LIGHTNINGSLASH: public SkillHandler 
{
public:
	LIGHTNINGSLASH() throw(){}
	~LIGHTNINGSLASH() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "LIGHTNINGSLASH"; }
	SkillType_t getSkillType() const throw() { return SKILL_LIGHTNING_SLASH; }

	void execute(Monster *pMonster, Creature *pEnemy) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern LIGHTNINGSLASH g_LIGHTNINGSLASH;

#endif
