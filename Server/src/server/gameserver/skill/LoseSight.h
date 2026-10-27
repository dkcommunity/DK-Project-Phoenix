//////////////////////////////////////////////////////////////////////////////
// Filename    : LoseSight.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_LOSE_SIGHT_HANDLER_H__
#define __SKILL_LOSE_SIGHT_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class LoseSight;
//////////////////////////////////////////////////////////////////////////////

class LoseSight : public SkillHandler 
{
public:
	LoseSight() throw() {}
	~LoseSight() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "LoseSight"; }
	SkillType_t getSkillType() const throw() { return SKILL_LOSE_SIGHT; }

	virtual SkillResultType execute(Slayer* pSlayer,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output) {}
};

// global variable declaration
extern LoseSight g_LoseSight;

#endif // __SKILL_LOSE_SIGHT_HANDLER_H__

