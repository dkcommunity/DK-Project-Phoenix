//////////////////////////////////////////////////////////////////////////////
// Filename    : Brionac.h 
// Written By  : bigheart 
// Description : 20080410 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BRIONAC_HANDLER_H__
#define __SKILL_BRIONAC_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class Brionac;
//////////////////////////////////////////////////////////////////////////////

class Brionac: public SkillHandler 
{
public:
	Brionac() throw() {}
	~Brionac() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "Brionac"; }

	SkillType_t getSkillType() const throw() { return SKILL_BRIONAC; }

	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t targetObject, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern Brionac g_Brionac;

#endif // __SKILL_BRIONAC_HANDLER_H__
