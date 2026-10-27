//////////////////////////////////////////////////////////////////////////////
// Filename    : CRYSTALTHORN.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_CRYSTAL_THORN_HANDLER_H__
#define __SKILL_CRYSTAL_THORN_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class Destinies;
//////////////////////////////////////////////////////////////////////////////

class CRYSTALTHORN: public SkillHandler 
{
public:
	CRYSTALTHORN() throw() {}
	~CRYSTALTHORN() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "CRYSTALTHORN"; }

	SkillType_t getSkillType() const throw() { return SKILL_CRYSTAL_THORN; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern CRYSTALTHORN g_CRYSTALTHORN;

#endif // __SKILL_CRYSTALTHORN_HANDLER_H__
