//////////////////////////////////////////////////////////////////////////////
// Filename    : SoundWave.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_SOUND_WAVE_HANDLER_H__
#define __SKILL_SOUND_WAVE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class SoundWave;
//////////////////////////////////////////////////////////////////////////////

class SoundWave : public SkillHandler 
{
public:
	SoundWave() throw() {}
	~SoundWave() throw() {}

public:
    string getSkillHandlerName() const throw() { return "SoundWave"; }
	SkillType_t getSkillType() const throw() { return SKILL_SOUND_WAVE; }

	void execute(Monster* pMonster) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern SoundWave g_SoundWave;

#endif // __SKILL_SOUND_WAVE_HANDLER_H__
