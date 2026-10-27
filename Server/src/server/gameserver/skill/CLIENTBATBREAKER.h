//////////////////////////////////////////////////////////////////////////////
// Filename    : BATBREAKER.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_CLIENT_BAT_BREAKER_HANDLER_H__
#define __SKILL_CLIENT_BAT_BREAKER_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class NooseOfWraith;
//////////////////////////////////////////////////////////////////////////////

class CLIENTBATBREAKER : public SkillHandler 
{
public:
	CLIENTBATBREAKER() throw();
	~CLIENTBATBREAKER() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "CLIENTBATBREAKER"; }
	SkillType_t getSkillType() const throw() { return SKILL_CLIENT_BAT_BREAKER; }

	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t ObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);


	void computeOutput(const SkillInput& input, SkillOutput& output);

};

// global variable declaration
extern CLIENTBATBREAKER g_CLIENTBATBREAKER;

#endif // __SKILL_NOOSE_OF_WRAITH_HANDLER_H__
