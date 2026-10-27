//////////////////////////////////////////////////////////////////////////////
// Filename    : IGNISINFERNUM.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_IGNIS_INFERNUM_HANDLER_H__
#define __SKILL_IGNIS_INFERNUM_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ADV2_LIGHTING_SHOCK;
//////////////////////////////////////////////////////////////////////////////



class IGNISINFERNUM : public SkillHandler 
{
public:
	IGNISINFERNUM() throw() {}
	~IGNISINFERNUM() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "IGNISINFERNUM"; }
	SkillType_t getSkillType() const throw() { return SKILL_IGNIS_INFERNUM; }

	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t ObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern IGNISINFERNUM g_IGNISINFERNUM;

#endif // __SKILL_IGNISINFERNUM_HANDLER_H__

