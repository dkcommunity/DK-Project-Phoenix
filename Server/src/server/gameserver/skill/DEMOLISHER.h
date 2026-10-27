//////////////////////////////////////////////////////////////////////////////
// Filename    : DEMOLISHER.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_DEMOLISHER_HANDLER_H__
#define __SKILL_DEMOLISHER_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class Destinies;
//////////////////////////////////////////////////////////////////////////////

class DEMOLISHER: public SkillHandler 
{
public:
	DEMOLISHER() throw() {}
	~DEMOLISHER() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "DEMOLISHER"; }

	SkillType_t getSkillType() const throw() { return SKILL_DEMOLISHER; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t ObjectID,  OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern DEMOLISHER g_DEMOLISHER;

#endif
