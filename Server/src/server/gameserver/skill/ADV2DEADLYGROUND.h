//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2DEADLYGROUND.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ADV2_DEADLY_GROUND_HANDLER_H__
#define __SKILL_ADV2_DEADLY_GROUND_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ADV2DEADLYGROUND;
//////////////////////////////////////////////////////////////////////////////

class ADV2DEADLYGROUND : public SkillHandler 
{
public:
	ADV2DEADLYGROUND() throw() {}
	~ADV2DEADLYGROUND() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2DEADLYGROUND"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_DEADLY_GROUND; }

	virtual SkillResultType execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y,  VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t,  VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ADV2DEADLYGROUND g_ADV2DEADLYGROUND;

#endif // __SKILL_SUMMON_GORE_GLAND_HANDLER_H__
