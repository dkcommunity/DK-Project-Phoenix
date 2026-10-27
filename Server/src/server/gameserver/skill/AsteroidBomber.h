//////////////////////////////////////////////////////////////////////////////
// Filename    : AsteroidBomber.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ASTEROID_BOMBER_HANDLER_H__
#define __SKILL_ASTEROID_BOMBER_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class AsteroidBomber;
//////////////////////////////////////////////////////////////////////////////

class AsteroidBomber : public SkillHandler 
{
public:
	AsteroidBomber() throw() {}
	~AsteroidBomber() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "AsteroidBomber"; }
	SkillType_t getSkillType() const throw() { return SKILL_ASTEROID_BOMBER; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t ObjectID,  OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
	void computeOutputByCreature(PlayerCreature* pPC, SkillOutput& output);
};

// global variable declaration
extern AsteroidBomber g_AsteroidBomber;

#endif // __SKILL_ASTEROID_BOMBER_HANDLER_H__
