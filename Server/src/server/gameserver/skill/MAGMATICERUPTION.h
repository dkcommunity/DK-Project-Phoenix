//////////////////////////////////////////////////////////////////////////////
// Filename    : MagmaDetonation2.h 
// Written By  : ksym555
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_MAGMATIC_ERUPTION_HANDLER_H__
#define __SKILL_MAGMATIC_ERUPTION_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class MAGMATICERUPTION;
//////////////////////////////////////////////////////////////////////////////

class MAGMATICERUPTION : public SkillHandler 
{
public:
	MAGMATICERUPTION() throw() {}
	~MAGMATICERUPTION() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "MAGMATICERUPTION"; }
	SkillType_t getSkillType() const throw() { return SKILL_MAGMATIC_ERUPTION; }

	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	void execute(Monster*, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern MAGMATICERUPTION g_MAGMATICERUPTION;

#endif // __SKILL_MAGMA_DETONATION_2_HANDLER_H__
