#ifndef __SKILL_ADV2_CONTRADICTO_HANDLER_H__
#define __SKILL_ADV2_CONTRADICTO_HANDLER_H__

#include "SkillHandler.h"

class ADV2CONTRADICTO : public SkillHandler 
{
public:
	ADV2CONTRADICTO() throw() {}
	~ADV2CONTRADICTO() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2CONTRADICTO"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_CONTRADICTO; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot * pSkillSlot, CEffectID_t CEffectID) throw(Error);
	void computeOutput(const SkillInput& input, SkillOutput& output);
};
extern ADV2CONTRADICTO g_ADV2CONTRADICTO;

#endif
