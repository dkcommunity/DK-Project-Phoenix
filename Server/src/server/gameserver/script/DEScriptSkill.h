#ifndef _DESCRIPTSKILL_H_
#define _DESCRIPTSKILL_H_

#include "DEScript.h"

#include "skill/SkillHandler.h"
#include "skill/SimpleSkill.h"

class DEScriptSkill : public DEScript
{
public :
	DEScriptSkill();
	virtual ~DEScriptSkill();
	
	void AddMask(lua_tinker::table SimpleSkillInputInfo, int x, int y, int penalty);
	
	void NotifyInitialize();
	void NotifyComputOutput(const SkillInput& input, SkillOutput& output);
	void NotifyExecuteSkill(SIMPLE_SKILL_INPUT& simpleSkillInput);
	
protected :
	
	
};

#endif /* _DESCRIPTSKILL_H_ */
