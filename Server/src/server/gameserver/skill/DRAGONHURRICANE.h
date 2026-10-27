//////////////////////////////////////////////////////////////////////////////
// Filename    : DRAGONHURRICANE.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_DRAGON_HURRICANE_HANDLER_H__
#define __SKILL_DRAGON_HURRICANE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class DRAGONHURRICANE;
//////////////////////////////////////////////////////////////////////////////

class DRAGONHURRICANE : public SkillHandler 
{
public:
	DRAGONHURRICANE() throw() {}
	~DRAGONHURRICANE() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "DRAGONHURRICANE"; }
	SkillType_t getSkillType() const throw() { return SKILL_DRAGON_HURRICANE; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern DRAGONHURRICANE g_DRAGONHURRICANE;

#endif // __SKILL_DRAGONHURRICANE_HANDLER_H__
