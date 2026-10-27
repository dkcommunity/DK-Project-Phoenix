//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodyShout1.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ADV2_TRUE_CLOLOR_OF_VAMP_LOAD_HANDLER_H__
#define __SKILL_ADV2_TRUE_CLOLOR_OF_VAMP_LOAD_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ADV2TRUECLOLOROFVAMPLOAD;
//////////////////////////////////////////////////////////////////////////////

class ADV2TRUECLOLOROFVAMPLOAD : public SkillHandler 
{
public:
	ADV2TRUECLOLOROFVAMPLOAD() throw() {}
	~ADV2TRUECLOLOROFVAMPLOAD() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2TRUECLOLOROFVAMPLOAD"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_TRUE_CLOLOR_OF_VAMP_LOAD; }

	virtual SkillResultType execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ADV2TRUECLOLOROFVAMPLOAD g_ADV2TRUECLOLOROFVAMPLOAD;

#endif // __SKILL_ADV2TRUECLOLOROFVAMPLOAD_HANDLER_H__
