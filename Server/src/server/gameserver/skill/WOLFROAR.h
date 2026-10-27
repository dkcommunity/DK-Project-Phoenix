//////////////////////////////////////////////////////////////////////////////
// Filename    : WOLFROAR1.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_WOLF_ROAR_HANDLER_H__
#define __SKILL_WOLF_ROAR_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class WOLF_ROAR;
//////////////////////////////////////////////////////////////////////////////

class WOLFROAR : public SkillHandler 
{
public:
	WOLFROAR() throw() {}
	~WOLFROAR() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "WOLFROAR"; }
	SkillType_t getSkillType() const throw() { return SKILL_WOLF_ROAR; }

	virtual SkillResultType execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern WOLFROAR g_WOLFROAR;

#endif // __SKILL_WOLFROAR_HANDLER_H__
