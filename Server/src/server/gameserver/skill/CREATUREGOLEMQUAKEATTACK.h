#ifndef __SKILL_CREATURE_GOLEM_QUAKE_ATTACK_HANDLER_H__
#define __SKILL_CREATURE_GOLEM_QUAKE_ATTACK_HANDLER_H__

#include "SkillHandler.h"


class CREATUREGOLEMQUAKEATTACK : public SkillHandler 
{
public:
	CREATUREGOLEMQUAKEATTACK() throw() {}
	~CREATUREGOLEMQUAKEATTACK() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "CREATUREGOLEMQUAKEATTACK"; }
	SkillType_t getSkillType() const throw() { return SKILL_CREATURE_GOLEM_QUAKE_ATTACK; }

	void execute(Monster*, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};


extern CREATUREGOLEMQUAKEATTACK g_CREATUREGOLEMQUAKEATTACK;

#endif
