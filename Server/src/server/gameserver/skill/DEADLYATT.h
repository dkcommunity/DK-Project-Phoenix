
//////////////////////////////////////////////////////////////////////////////
// Filename    : ChoppingFirewood.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_DEADLY_ATT_HANDLER_H__
#define __SKILL_DEADLY_ATT_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ChoppingFirewood;
//////////////////////////////////////////////////////////////////////////////

class DEADLYATT : public SkillHandler 
{
public:
	DEADLYATT() throw();
	~DEADLYATT() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "DEADLYATT"; }
	SkillType_t getSkillType() const throw() { return SKILL_DEADLY_ATT; }

	virtual void execute(Monster* pMonster, Creature *pEnemy) throw(Error);

	virtual void computeOutput(const SkillInput& input, SkillOutput& output);

private :
	POINT	m_pGoreGlandFireMask[9];
};
// global variable declaration
extern DEADLYATT g_DEADLYATT;

#endif // __SKILL_CHOPPING_FIREWOOD_HANDLER_H__
