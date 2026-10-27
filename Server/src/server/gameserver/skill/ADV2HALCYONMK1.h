//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2HALCYONMK1.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ADV2_HALCYON_MK1_HANDLER_H__
#define __SKILL_ADV2_HALCYON_MK1_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class InstallTurret;
//////////////////////////////////////////////////////////////////////////////

class ADV2HALCYONMK1 : public SkillHandler 
{
public:
	ADV2HALCYONMK1() throw() {}
	~ADV2HALCYONMK1() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2HALCYONMK1"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_HALCYON_MK1; }

	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ADV2HALCYONMK1 g_ADV2HALCYONMK1;

#endif // __SKILL_INSTALL_TURRET_HANDLER_H__
