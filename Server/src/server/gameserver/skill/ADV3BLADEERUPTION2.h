//////////////////////////////////////////////////////////////////////////////
// Filename    : BLADEERUPTION2.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLADE_ERUPTION_2_HANDLER_H__
#define __SKILL_BLADE_ERUPTION_2_HANDLER_H__

#include "SkillHandler.h"

class BLADEERUPTION2 : public SkillHandler 
{
public:
	BLADEERUPTION2() throw() {}
	~BLADEERUPTION2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BLADEERUPTION2"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLADE_ERUPTION_2; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};
extern BLADEERUPTION2 g_BLADEERUPTION2;

#endif
