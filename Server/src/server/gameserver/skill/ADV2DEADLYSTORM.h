#ifndef __SKILL_ADV2_DEADLY_STORM_HANDLER_H__
#define __SKILL_ADV2_DEADLY_STORM_HANDLER_H__

#include "SkillHandler.h"

class ADV2DEADLYSTORM : public SkillHandler 
{
public:
	ADV2DEADLYSTORM() throw() {}
	~ADV2DEADLYSTORM() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2DEADLYSTORM"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_DEADLY_STORM; }

	virtual SkillResultType execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y,  VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t,  VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};
extern ADV2DEADLYSTORM g_ADV2DEADLYSTORM;

#endif
