//////////////////////////////////////////////////////////////////////////////
// Filename    : BikeCrash.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLAZE_WALK_2_HANDLER_H__
#define __SKILL_BLAZE_WALK_2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BikeCrash;
//////////////////////////////////////////////////////////////////////////////

class BLAZEWALK2 : public SkillHandler 
{
public:
	BLAZEWALK2() throw() {}
	~BLAZEWALK2() throw() {}

public :
    string getSkillHandlerName() const throw() { return "BLAZEWALK2"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLAZE_WALK_2; }

	virtual SkillResultType execute(Slayer* pSlayer,ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);

};

// global variable declaration
extern BLAZEWALK2 g_BLAZEWALK2;

#endif // __SKILL_SHADOW_WALK_HANDLER_H__
