//////////////////////////////////////////////////////////////////////////////
// Filename    : VigorDrop.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_VIGOR_DROP_HANDLER_H__
#define __SKILL_VIGOR_DROP_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class VigorDrop;
//////////////////////////////////////////////////////////////////////////////

class VigorDrop : public SkillHandler 
{
public:
	VigorDrop() throw() {}
	~VigorDrop() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "VigorDrop"; }
	SkillType_t getSkillType() const throw() { return SKILL_VIGOR_DROP; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void execute(Monster* pMonster, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern VigorDrop g_VigorDrop;

#endif // __SKILL_VIGOR_DROP_HANDLER_H__
