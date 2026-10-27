#ifndef __SKILL_LAY_WASTE_HANDLER_H__
#define __SKILL_LAY_WASTE_HANDLER_H__

#include "SkillHandler.h"

class LAYWASTE : public SkillHandler 
{
public:
	LAYWASTE() throw() {}
	~LAYWASTE() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "LAYWASTE"; }
	SkillType_t getSkillType() const throw() { return SKILL_LAY_WASTE; }

	void execute(Monster* pMonster, Creature* pEnemy) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern LAYWASTE g_LAYWASTE;

#endif
