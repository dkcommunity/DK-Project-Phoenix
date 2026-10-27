//////////////////////////////////////////////////////////////////////////////
// Filename    : BREATHOFDRYAD.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BREATH_OF_DRYAD_HANDLER_H__
#define __SKILL_BREATH_OF_DRYAD_HANDLER_H__

#include "SkillHandler.h"


//////////////////////////////////////////////////////////////////////////////
// class BREATHOFDRYAD;
//////////////////////////////////////////////////////////////////////////////

class BREATHOFDRYAD : public SkillHandler 
{
public:
	BREATHOFDRYAD() throw() {}
	~BREATHOFDRYAD() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "BREATHOFDRYAD"; }
	SkillType_t getSkillType() const throw() { return SKILL_BREATH_OF_DRYAD; }

	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern BREATHOFDRYAD g_BREATHOFDRYAD;

#endif // __SKILL_BREATHOFDRYAD_H__
