//////////////////////////////////////////////////////////////////////////////
// Filename    : DRAGONHURRICANE2.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_DRAGON_HURRICANE2_HANDLER_H__
#define __SKILL_DRAGON_HURRICANE2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class DRAGONHURRICANE2;
//////////////////////////////////////////////////////////////////////////////

class DRAGONHURRICANE2 : public SkillHandler 
{
public:
	DRAGONHURRICANE2() throw() {}
	~DRAGONHURRICANE2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "DRAGONHURRICANE2"; }
	SkillType_t getSkillType() const throw() { return SKILL_DRAGON_HURRICANE2; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern DRAGONHURRICANE2 g_DRAGONHURRICANE2;

#endif // __SKILL_DRAGONHURRICANE2_HANDLER_H__
