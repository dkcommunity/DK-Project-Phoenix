//////////////////////////////////////////////////////////////////////////////
// Filename    : FLAMESIGHT.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_FLAME_SIGHT_HANDLER_H__
#define __SKILL_FLAME_SIGHT_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class FLAMESIGHT;
//////////////////////////////////////////////////////////////////////////////

class FLAMESIGHT : public SkillHandler 
{
public:
	FLAMESIGHT() throw() {}
	~FLAMESIGHT() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "FLAMESIGHT"; }
	SkillType_t getSkillType() const throw() { return SKILL_FLAME_SIGHT; }

	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	//virtual SkillResultType execute(Ousters* pOusters, ObjectID_t ObjectID,  OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern FLAMESIGHT g_FLAMESIGHT;

#endif // __SKILL_FLAME_SIGHT_HANDLER_H__
