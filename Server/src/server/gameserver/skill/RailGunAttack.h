//////////////////////////////////////////////////////////////////////////////
// Filename    : RailGunAttack.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_Rail_Gun_Attack_HANDLER_H__
#define __SKILL_Rail_Gun_Attack_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class RailGunAttack;
//////////////////////////////////////////////////////////////////////////////

class RailGunAttack: public SkillHandler 
{
public:
	RailGunAttack() throw();
	virtual ~RailGunAttack() throw();
	
public:
    string getSkillHandlerName() const throw() { return "RailGunAttack"; }
	SkillType_t getSkillType() const throw() { return SKILL_Rail_Gun_Attack; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_Rail_Gun_Attack_H__
