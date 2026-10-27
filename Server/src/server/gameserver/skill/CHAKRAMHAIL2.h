//////////////////////////////////////////////////////////////////////////////
// Filename    : SharpHail.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_CHAKRAM_HAIL2_HANDLER_H__
#define __SKILL_CHAKRAM_HAIL2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class SharpHail2;
//////////////////////////////////////////////////////////////////////////////

class CHAKRAMHAIL2 : public SkillHandler 
{
public:
	CHAKRAMHAIL2() throw() {}
	~CHAKRAMHAIL2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "CHAKRAMHAIL2"; }
	SkillType_t getSkillType() const throw() { return SKILL_CHAKRAM_HAIL2; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern CHAKRAMHAIL2 g_CHAKRAMHAIL2;

#endif // __SKILL_SHARP_HAIL2_HANDLER_H__
