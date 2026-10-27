//////////////////////////////////////////////////////////////////////////////
// Filename    : Untransform.h 
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_UN_TRANSFORM_HANDLER_H__
#define __SKILL_UN_TRANSFORM_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class Untransform;
//////////////////////////////////////////////////////////////////////////////

class Untransform: public SkillHandler 
{
public:
	Untransform() throw() {}
	~Untransform() throw() {}

public:
    string getSkillHandlerName() const throw() { return "Untransform"; }
	SkillType_t getSkillType() const throw() { return SKILL_UN_TRANSFORM; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	void execute(Monster* pMonster) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output) {}
};

// global variable declaration
extern Untransform g_Untransform;

#endif // __SKILL_UN_TRANSFORM_HANDLER_H__
