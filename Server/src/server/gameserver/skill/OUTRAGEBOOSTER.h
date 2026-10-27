//////////////////////////////////////////////////////////////////////////////
// Filename    : Seduction.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_OUTRAGE_BOOSTER_HANDLER_H__
#define __SKILL_OUTRAGE_BOOSTER_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class OUTRAGEBOOSTER;
//////////////////////////////////////////////////////////////////////////////

class OUTRAGEBOOSTER : public SkillHandler 
{
public:
	OUTRAGEBOOSTER() throw() {}
	~OUTRAGEBOOSTER() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "OUTRAGEBOOSTER"; }
	SkillType_t getSkillType() const throw() { return SKILL_OUTRAGE_BOOSTER; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern OUTRAGEBOOSTER g_OUTRAGEBOOSTER;

#endif // __SKILL_SEDUCTION_HANDLER_H__
