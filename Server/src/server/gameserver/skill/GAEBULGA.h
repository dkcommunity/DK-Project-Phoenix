//////////////////////////////////////////////////////////////////////////////
// Filename    : GAEBULGA.h 
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_GAE_BULGA_HANDLER_H__
#define __SKILL_GAE_BULGA_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class GAEBULGA;
//////////////////////////////////////////////////////////////////////////////

class GAEBULGA : public SkillHandler 
{
public:
	GAEBULGA() throw() {}
	~GAEBULGA() throw() {}

public:
    string getSkillHandlerName() const throw() { return "GAEBULGA"; }
	SkillType_t getSkillType() const throw() { return SKILL_GAE_BULGA; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	void execute(Monster* pMonster, Creature* pEnemy) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern GAEBULGA g_GAEBULGA;

#endif // __SKILL_TRIDENT_HANDLER_H__
