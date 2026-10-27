//////////////////////////////////////////////////////////////////////////////
// Filename    : VICIOUSGUIDANCE.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_VICIOUS_GUIDANCE_HANDLER_H__
#define __SKILL_VICIOUS_GUIDANCE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class VICIOUSGUIDANCE;
//////////////////////////////////////////////////////////////////////////////

class VICIOUSGUIDANCE: public SkillHandler 
{
public:
	VICIOUSGUIDANCE() throw() {}
	~VICIOUSGUIDANCE() throw() {}

public:
    string getSkillHandlerName() const throw() { return "VICIOUSGUIDANCE"; }
	SkillType_t getSkillType() const throw() { return SKILL_VICIOUS_GUIDANCE; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern VICIOUSGUIDANCE g_VICIOUSGUIDANCE;

#endif // __SKILL_VICIOUSGUIDANCE_HANDLER_H__
