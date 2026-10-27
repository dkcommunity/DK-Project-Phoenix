//////////////////////////////////////////////////////////////////////////////
// Filename    : Halo.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_HALO2_HANDLER_H__
#define __SKILL_HALO2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class Halo;
//////////////////////////////////////////////////////////////////////////////

class Halo2: public SkillHandler 
{
public:
	Halo2() throw() {}
	~Halo2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "Halo2"; }

	SkillType_t getSkillType() const throw() { return SKILL_HALO2; }

	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t targetObject, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern Halo2 g_Halo2;

#endif // __SKILL_HALO_HANDLER_H__
