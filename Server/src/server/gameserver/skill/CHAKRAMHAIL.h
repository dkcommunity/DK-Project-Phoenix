//////////////////////////////////////////////////////////////////////////////
// Filename    : SharpHail.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_CHAKRAM_HAIL_HANDLER_H__
#define __SKILL_CHAKRAM_HAIL_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class SharpHail;
//////////////////////////////////////////////////////////////////////////////

class CHAKRAMHAIL : public SkillHandler 
{
public:
	CHAKRAMHAIL() throw() {}
	~CHAKRAMHAIL() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "CHAKRAMHAIL"; }
	SkillType_t getSkillType() const throw() { return SKILL_CHAKRAM_HAIL; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern CHAKRAMHAIL g_CHAKRAMHAIL;

#endif // __SKILL_SHARP_HAIL_HANDLER_H__
