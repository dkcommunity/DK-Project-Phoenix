#ifndef __SKILL_ADV2_INFINITY_BLAZE_HANDLER_H__
#define __SKILL_ADV2_INFINITY_BLAZE_HANDLER_H__

#include "SkillHandler.h"

class ADV2INFINITYBLAZE : public SkillHandler 
{
public:
	ADV2INFINITYBLAZE() throw() {}
	~ADV2INFINITYBLAZE() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2INFINITYBLAZE"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_INFINITY_BLAZE; }

	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t ObjectID,  OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};
extern ADV2INFINITYBLAZE g_ADV2INFINITYBLAZE;

#endif
