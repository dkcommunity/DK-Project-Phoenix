//////////////////////////////////////////////////////////////////////////////
// Filename    : OniSpanielThrowingAxe.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_SPANIEL_THROWING_AXE_HANDLER_H__
#define __SKILL_SPANIEL_THROWING_AXE_HANDLER_H__

#include "SkillHandler.h"
#include "ThrowingAxe.h"

//////////////////////////////////////////////////////////////////////////////
// class OniSpanielThrowingAxe;
//////////////////////////////////////////////////////////////////////////////

class OniSpanielThrowingAxe: public SkillHandler 
{
public:
	OniSpanielThrowingAxe() throw();
	virtual ~OniSpanielThrowingAxe() throw();
	
public:
    string getSkillHandlerName() const throw() { return "OniSpanielThrowingAxe"; }
	SkillType_t getSkillType() const throw() { return SKILL_ONI_SPANIEL_THROWING_AXE; }

	virtual void execute(Monster* pMonster, Creature* pEnemy) throw(Error);
	
	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

};

#endif // __SKILL_SPANIEL_THROWING_AXE_HANDLER_H__
