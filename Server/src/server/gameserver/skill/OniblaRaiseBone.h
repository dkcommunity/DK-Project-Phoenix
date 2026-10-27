//////////////////////////////////////////////////////////////////////////////
// Filename    : OniblaRaiseBone.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ONIBLA_RAISE_BONE_HANDLER_H__
#define __SKILL_ONIBLA_RAISE_BONE_HANDLER_H__

#include "SkillHandler.h"
#include "IceWave.h"

//////////////////////////////////////////////////////////////////////////////
// class OniblaRaiseBone;
//////////////////////////////////////////////////////////////////////////////

class OniblaRaiseBone: public IceWave 
{
public:
	OniblaRaiseBone() throw();
	virtual ~OniblaRaiseBone() throw();
	
public:
    string getSkillHandlerName() const throw() { return "OniblaRaiseBone"; }
	SkillType_t getSkillType() const throw() { return SKILL_ONIBLA_RAISE_BONE; }

	virtual void execute(Monster* pMonster, Creature *pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_ONIBLA_RAISE_BONE_HANDLER_H__
