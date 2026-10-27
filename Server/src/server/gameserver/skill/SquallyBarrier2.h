//////////////////////////////////////////////////////////////////////////////
// Filename    : SquallyBarrier2.h 
// Written By  : ksym555
// Description : 질풍의 베리어 2
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_SQUALLY_BARRIER_2_HANDLER_H__
#define __SKILL_SQUALLY_BARRIER_2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class SquallyBarrier2;
//////////////////////////////////////////////////////////////////////////////

class SquallyBarrier2 : public SkillHandler 
{
public:
	SquallyBarrier2() throw() {}
	~SquallyBarrier2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "SquallyBarrier2"; }
	SkillType_t getSkillType() const throw() { return SKILL_SQUALLY_BARRIER2; }

	virtual SkillResultType execute(Ousters* pOusters,  OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern SquallyBarrier2 g_SquallyBarrier2;

#endif // __SKILL_SQUALLY_BARRIER_2_HANDLER_H__
