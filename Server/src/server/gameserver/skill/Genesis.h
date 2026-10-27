//////////////////////////////////////////////////////////////////////////////
// Filename    : Genesis.h 
// Written By  : bigheart 
// Description : 20080407
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_GENESIS_HANDLER_H__
#define __SKILL_GENESIS_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class Genesis;
//////////////////////////////////////////////////////////////////////////////

class Genesis : public SkillHandler 
{
public:
	Genesis() throw() {}
	~Genesis() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "Genesis"; }

	SkillType_t getSkillType() const throw() { return SKILL_GENESIS; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern Genesis g_Genesis;

#endif // __SKILL_GENESIS_HANDLER_H__
