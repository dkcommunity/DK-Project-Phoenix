//////////////////////////////////////////////////////////////////////////////
// Filename    : Regeneration.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_REGENERATION_HANDLER_H__
#define __SKILL_REGENERATION_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class Regeneration;
//////////////////////////////////////////////////////////////////////////////

class Regeneration : public SkillHandler 
{
public:
	Regeneration() throw() {}
	~Regeneration() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "Regeneration"; }
	SkillType_t getSkillType() const throw() { return SKILL_REGENERATION; }

	virtual SkillResultType execute( Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID ) throw(Error);
	virtual SkillResultType execute( Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID ) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern Regeneration g_Regeneration;

#endif // __SKILL_REGENERATION_HANDLER_H__
