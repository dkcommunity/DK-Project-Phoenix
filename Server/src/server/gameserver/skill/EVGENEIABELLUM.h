//////////////////////////////////////////////////////////////////////////////
// Filename    : Evade.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_EVGENEIA_BELLUM_HANDLER_H__
#define __SKILL_EVGENEIA_BELLUM_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class EVGENEIA_BELLUM;
//////////////////////////////////////////////////////////////////////////////

class EVGENEIABELLUM : public SkillHandler 
{
public:
	EVGENEIABELLUM() throw() {}
	~EVGENEIABELLUM() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "EVGENEIABELLUM"; }
	SkillType_t getSkillType() const throw() { return SKILL_EVGENEIA_BELLUM; }

	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern EVGENEIABELLUM g_EVGENEIABELLUM;

#endif // __SKILL_EVGENEIA_BELLUM_HANDLER_H__
