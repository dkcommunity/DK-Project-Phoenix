//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodyBall.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BAT_BREAKER_HANDLER_H__
#define __SKILL_BAT_BREAKER_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BATBREAKER;
//////////////////////////////////////////////////////////////////////////////

class BATBREAKER : public SkillHandler 
{
public:
	BATBREAKER() throw();
	~BATBREAKER() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BATBREAKER"; }
	SkillType_t getSkillType() const throw() { return SKILL_BAT_BREAKER; }

	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t ObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void execute(Monster* pMonster, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);

};

// global variable declaration
extern BATBREAKER g_BATBREAKER;

#endif // __SKILL_NOOSE_OF_WRAITH_HANDLER_H__
