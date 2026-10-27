//////////////////////////////////////////////////////////////////////////////
// Filename    : CRYSTALTHORN.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_CRYSTAL_THORN_HANDLER_2_H__
#define __SKILL_CRYSTAL_THORN_HANDLER_2_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class Destinies;
//////////////////////////////////////////////////////////////////////////////

class CRYSTALTHORN2: public SkillHandler 
{
public:
	CRYSTALTHORN2() throw() {}
	~CRYSTALTHORN2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "CRYSTALTHORN2"; }

	SkillType_t getSkillType() const throw() { return SKILL_CRYSTAL_THORN2; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern CRYSTALTHORN2 g_CRYSTALTHORN2;

#endif // __SKILL_CRYSTALTHORN_HANDLER_H__
