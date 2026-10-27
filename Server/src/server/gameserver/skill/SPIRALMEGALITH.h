//////////////////////////////////////////////////////////////////////////////
// Filename    : BeatHead.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_SPIRAL_MEGALITH_HANDLER_H__
#define __SKILL_SPIRAL_MEGALITH_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BeatHead;
//////////////////////////////////////////////////////////////////////////////

class SPIRALMEGALITH : public SkillHandler 
{
public:
	SPIRALMEGALITH() throw() {}
	~SPIRALMEGALITH() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "SPIRALMEGALITH"; }
	SkillType_t getSkillType() const throw() { return SKILL_SPIRAL_MEGALITH; }

	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t ObjectID,  OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern SPIRALMEGALITH g_SPIRALMEGALITH;

#endif // __SKILL_BEAT_HEAD_HANDLER_H__
