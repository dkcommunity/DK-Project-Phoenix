//////////////////////////////////////////////////////////////////////////////
// Filename    : SquallyBarrier1.h 
// Written By  : ksym555
// Description : 질풍의 베리어 1
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_SQUALLY_BARRIER_1_HANDLER_H__
#define __SKILL_SQUALLY_BARRIER_1_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class SquallyBarrier1;
//////////////////////////////////////////////////////////////////////////////

class SquallyBarrier1 : public SkillHandler 
{
public:
	SquallyBarrier1() throw() {}
	~SquallyBarrier1() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "SquallyBarrier1"; }
	SkillType_t getSkillType() const throw() { return SKILL_SQUALLY_BARRIER1; }

	virtual SkillResultType execute(Ousters* pOusters,  OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern SquallyBarrier1 g_SquallyBarrier1;

#endif // __SKILL_SQUALLY_BARRIER_1_HANDLER_H__
