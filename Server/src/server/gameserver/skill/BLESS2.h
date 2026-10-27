//////////////////////////////////////////////////////////////////////////////
// Filename    : Bless2.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLESS_2_HANDLER_H__
#define __SKILL_BLESS_2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class Bless2;
//////////////////////////////////////////////////////////////////////////////

class BLESS2 : public SkillHandler 
{
public:
	BLESS2() throw() {}
	~BLESS2() throw() {}
	
public :
    string getSkillHandlerName() const throw() { return "BLESS2"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLESS_2; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern BLESS2 g_BLESS2;

#endif // __SKILL_BLESS_2_HANDLER_H__
