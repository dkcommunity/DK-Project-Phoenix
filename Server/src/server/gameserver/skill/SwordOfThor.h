//////////////////////////////////////////////////////////////////////////////
// Filename    : SwordOfThor.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_SWORD_OF_THOR_HANDLER_H__
#define __SKILL_SWORD_OF_THOR_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class SwordOfThor;
//////////////////////////////////////////////////////////////////////////////

class SwordOfThor : public SkillHandler 
{
public:
	SwordOfThor() throw() {}
	~SwordOfThor() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "SwordOfThor"; }
	SkillType_t getSkillType() const throw() { return SKILL_SWORD_OF_THOR; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern SwordOfThor g_SwordOfThor;

#endif // __SKILL_SWORD_OF_THOR_HANDLER_H__
