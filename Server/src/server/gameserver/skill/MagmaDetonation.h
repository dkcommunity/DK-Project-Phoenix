//////////////////////////////////////////////////////////////////////////////
// Filename    : MagmaDetonation.h 
// Written By  : ksym555
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_MAGMA_DETONATION_HANDLER_H__
#define __SKILL_MAGMA_DETONATION_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class MagmaDetonation;
//////////////////////////////////////////////////////////////////////////////

class MagmaDetonation : public SkillHandler 
{
public:
	MagmaDetonation() throw() {}
	~MagmaDetonation() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "MagmaDetonation"; }
	SkillType_t getSkillType() const throw() { return SKILL_MAGMA_DETONATION_1; }

	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	void execute(Monster*, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern MagmaDetonation g_MagmaDetonation;

#endif // __SKILL_MAGMA_DETONATION_HANDLER_H__
