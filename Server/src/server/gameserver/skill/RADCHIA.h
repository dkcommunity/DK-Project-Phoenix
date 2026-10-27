//////////////////////////////////////////////////////////////////////////////
// Filename    : IceHorizon.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_RADCHIA_HANDLER_H__
#define __SKILL_RADCHIA_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class IceHorizon;
//////////////////////////////////////////////////////////////////////////////

class RADCHIA : public SkillHandler 
{
public:
	RADCHIA() throw();
	~RADCHIA() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "RADCHIA"; }
	SkillType_t getSkillType() const throw() { return SKILL_RADCHIA; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);

};

// global variable declaration
extern RADCHIA g_RADCHIA;

#endif // __SKILL_ICE_HORIZON_HANDLER_H__
