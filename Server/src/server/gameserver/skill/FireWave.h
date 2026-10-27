//////////////////////////////////////////////////////////////////////////////
// Filename    : FireWave.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_FIRE_WAVE_HANDLER_H__
#define __SKILL_FIRE_WAVE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class FireWave;
//////////////////////////////////////////////////////////////////////////////

class FireWave : public SkillHandler 
{
public:
	FireWave() throw() {}
	~FireWave() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "FireWave"; }
	SkillType_t getSkillType() const throw() { return SKILL_FIRE_WAVE; }

	void execute(Monster* pMonster) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern FireWave g_FireWave;

#endif // __SKILL_FIRE_WAVE_HANDLER_H__
