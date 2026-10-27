//////////////////////////////////////////////////////////////////////////////
// Filename    : BikeCrash.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ATTRACT_CIRCLE_HANDLER_H__
#define __SKILL_ATTRACT_CIRCLE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BikeCrash;
//////////////////////////////////////////////////////////////////////////////

class ATTRACTCIRCLE : public SkillHandler 
{
public:
	ATTRACTCIRCLE() throw() {}
	~ATTRACTCIRCLE() throw() {}

public :
    string getSkillHandlerName() const throw() { return "ATTRACTCIRCLE"; }
	SkillType_t getSkillType() const throw() { return SKILL_ATTRACT_CIRCLE; }

	virtual SkillResultType execute(Slayer* pSlayer,ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);

};

// global variable declaration
extern ATTRACTCIRCLE g_ATTRACTCIRCLE;

#endif // __SKILL_SHADOW_WALK_HANDLER_H__
