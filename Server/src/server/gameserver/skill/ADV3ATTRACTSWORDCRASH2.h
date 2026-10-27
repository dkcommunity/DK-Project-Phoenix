//////////////////////////////////////////////////////////////////////////////
// Filename    : ATTRACTSWORDCRASH2.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ATTRACT_SWORD_CRASH_2_HANDLER_H__
#define __SKILL_ATTRACT_SWORD_CRASH_2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ATTRACTSWORDCRASH2;
//////////////////////////////////////////////////////////////////////////////

class ATTRACTSWORDCRASH2 : public SkillHandler 
{
public:
	ATTRACTSWORDCRASH2() throw() {}
	~ATTRACTSWORDCRASH2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ATTRACTSWORDCRASH2"; }

	SkillType_t getSkillType() const throw() { return SKILL_ATTRACT_SWORD_CRASH_2; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern ATTRACTSWORDCRASH2 g_ATTRACTSWORDCRASH2;

#endif
