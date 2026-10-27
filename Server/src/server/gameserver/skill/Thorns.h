//////////////////////////////////////////////////////////////////////////////
// Filename    : Thorns.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_THORNS_HANDLER_H__
#define __SKILL_THORNS_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class Thorns;
//////////////////////////////////////////////////////////////////////////////

class Thorns : public SkillHandler 
{
public:
	Thorns() throw() {}
	~Thorns() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "Thorns"; }
	SkillType_t getSkillType() const throw() { return SKILL_THORNS; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern Thorns g_Thorns;

#endif // __SKILL_THORNS_HANDLER_H__
