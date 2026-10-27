//////////////////////////////////////////////////////////////////////////////
// Filename    : Brandish.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BRANDISH_HANDLER_H__
#define __SKILL_BRANDISH_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class Brandish;
//////////////////////////////////////////////////////////////////////////////

class Brandish : public SkillHandler 
{
public:
	Brandish() throw() {}
	~Brandish() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "Brandish"; }
	SkillType_t getSkillType() const throw() { return SKILL_BRANDISH; }

//	void execute(Vampire* pVampire, ObjectID_t ObjectID, VampireSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	void execute(Monster* pMonster) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern Brandish g_Brandish;

#endif // __SKILL_BRANDISH_HANDLER_H__
