//////////////////////////////////////////////////////////////////////////////
// Filename    : Incinerate.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_INCINERATE_HANDLER_H__
#define __SKILL_INCINERATE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class Incinerate;
//////////////////////////////////////////////////////////////////////////////

class Incinerate : public SkillHandler 
{
public:
	Incinerate() throw() {}
	~Incinerate() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "Incinerate"; }
	SkillType_t getSkillType() const throw() { return SKILL_INCINERATE; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t ObjectID,  OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern Incinerate g_Incinerate;

#endif // __SKILL_INCINERATE_HANDLER_H__
