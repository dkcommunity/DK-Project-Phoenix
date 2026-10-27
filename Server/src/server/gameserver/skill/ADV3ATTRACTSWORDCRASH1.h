//////////////////////////////////////////////////////////////////////////////
// Filename    : ATTRACTSWORDCRASH1.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ATTRACT_SWORD_CRASH_1_HANDLER_H__
#define __SKILL_ATTRACT_SWORD_CRASH_1_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ATTRACTSWORDCRASH1;
//////////////////////////////////////////////////////////////////////////////

class ATTRACTSWORDCRASH1 : public SkillHandler 
{
public:
	ATTRACTSWORDCRASH1() throw() {}
	~ATTRACTSWORDCRASH1() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ATTRACTSWORDCRASH1"; }

	SkillType_t getSkillType() const throw() { return SKILL_ATTRACT_SWORD_CRASH_1; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern ATTRACTSWORDCRASH1 g_ATTRACTSWORDCRASH1;

#endif
