//////////////////////////////////////////////////////////////////////////////
// Filename    : MagmaDetonation2.h 
// Written By  : ksym555
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_MAGMA_DETONATION_2_HANDLER_H__
#define __SKILL_MAGMA_DETONATION_2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class MagmaDetonation2;
//////////////////////////////////////////////////////////////////////////////

class MagmaDetonation2 : public SkillHandler 
{
public:
	MagmaDetonation2() throw() {}
	~MagmaDetonation2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "MagmaDetonation2"; }
	SkillType_t getSkillType() const throw() { return SKILL_MAGMA_DETONATION_2; }

	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	void execute(Monster*, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern MagmaDetonation2 g_MagmaDetonation2;

#endif // __SKILL_MAGMA_DETONATION_2_HANDLER_H__
