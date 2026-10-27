//////////////////////////////////////////////////////////////////////////////
// Filename    : MistOfSoul2.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_MIST_OF_SOUL2_HANDLER_H__
#define __SKILL_MIST_OF_SOUL2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class MistOfSoul2;
//////////////////////////////////////////////////////////////////////////////

class MistOfSoul2 : public SkillHandler 
{
public:
	MistOfSoul2() throw() {}
	~MistOfSoul2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "MistOfSoul2"; }
	SkillType_t getSkillType() const throw() { return SKILL_MIST_OF_SOUL2; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y,  OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t TargetObjectID,  OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern MistOfSoul2 g_MistOfSoul2;

#endif // __SKILL_MIST_OF_SOUL2_HANDLER_H__
