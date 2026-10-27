//////////////////////////////////////////////////////////////////////////////
// Filename    : DRACULEBLOODYCARPE.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_DRACULE_BLOODY_CARPE_HANDLER_H__
#define __SKILL_DRACULE_BLOODY_CARPE_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class DRACULEBLOODYCARPE;
//////////////////////////////////////////////////////////////////////////////

class DRACULEBLOODYCARPE: public SkillHandler 
{
public:
	DRACULEBLOODYCARPE() throw();
	virtual ~DRACULEBLOODYCARPE() throw();
	
public:
    string getSkillHandlerName() const throw() { return "DRACULEBLOODYCARPE"; }
	SkillType_t getSkillType() const throw() { return SKILL_DRACULE_BLOODY_CARPE; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_SmileofDeath_H__
