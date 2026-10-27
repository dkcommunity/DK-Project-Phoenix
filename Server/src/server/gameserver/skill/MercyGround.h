//////////////////////////////////////////////////////////////////////////////
// Filename    : MercyGround.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_MERCY_GROUND_HANDLER_H__
#define __SKILL_MERCY_GROUND_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class MercyGround;
//////////////////////////////////////////////////////////////////////////////

class MercyGround : public SkillHandler 
{
public:
	MercyGround() throw() {}
	~MercyGround() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "MercyGround"; }
	SkillType_t getSkillType() const throw() { return SKILL_MERCY_GROUND; }

	virtual SkillResultType execute( Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID ) throw(Error);
	virtual SkillResultType execute( Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID ) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern MercyGround g_MercyGround;

#endif // __SKILL_MERCY_GROUND_HANDLER_H__
