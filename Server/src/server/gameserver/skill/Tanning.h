//////////////////////////////////////////////////////////////////////////////
// Filename    : Tanning.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_TANNING_HANDLER_H__
#define __SKILL_TANNING_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class Tanning;
//////////////////////////////////////////////////////////////////////////////

class Tanning : public SkillHandler 
{
public:
	Tanning() throw() {}
	~Tanning() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "Tanning"; }
	SkillType_t getSkillType() const throw() { return SKILL_TANNING; }

	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y,  SlayerSkillSlot* pSlayerSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y,  VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y,  OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern Tanning g_Tanning;

#endif // __SKILL_TANNING_HANDLER_H__
