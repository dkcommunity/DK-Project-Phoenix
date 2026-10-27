//////////////////////////////////////////////////////////////////////////////
// Filename    : MistOfSoul1.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_MIST_OF_SOUL1_HANDLER_H__
#define __SKILL_MIST_OF_SOUL1_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class MistOfSoul1;
//////////////////////////////////////////////////////////////////////////////

class MistOfSoul1 : public SkillHandler 
{
public:
	MistOfSoul1() throw() {}
	~MistOfSoul1() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "MistOfSoul1"; }
	SkillType_t getSkillType() const throw() { return SKILL_MIST_OF_SOUL1; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y,  OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t TargetObjectID,  OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern MistOfSoul1 g_MistOfSoul1;

#endif // __SKILL_MIST_OF_SOUL1_HANDLER_H__
