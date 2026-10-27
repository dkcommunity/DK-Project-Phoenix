//////////////////////////////////////////////////////////////////////////////
// Filename    : Thorns2.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_THORNS2_HANDLER_H__
#define __SKILL_THORNS2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class Thorns2;
//////////////////////////////////////////////////////////////////////////////

class Thorns2 : public SkillHandler 
{
public:
	Thorns2() throw() {}
	~Thorns2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "Thorns2"; }
	SkillType_t getSkillType() const throw() { return SKILL_THORNS2; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern Thorns2 g_Thorns2;

#endif // __SKILL_THORNS_HANDLER_H__
