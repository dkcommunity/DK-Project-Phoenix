//////////////////////////////////////////////////////////////////////////////
// Filename    : BLADEERUPTION1.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLADE_ERUPTION_1_HANDLER_H__
#define __SKILL_BLADE_ERUPTION_1_HANDLER_H__

#include "SkillHandler.h"

class BLADEERUPTION1 : public SkillHandler 
{
public:
	BLADEERUPTION1() throw() {}
	~BLADEERUPTION1() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BLADEERUPTION1"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLADE_ERUPTION_1; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};
extern BLADEERUPTION1 g_BLADEERUPTION1;

#endif
