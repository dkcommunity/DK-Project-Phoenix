//////////////////////////////////////////////////////////////////////////////
// Filename    : ALLYSTERWIND.h 
// Written By  : Evade xiu
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ALLYSTER_WIND_HANDLER_H__
#define __SKILL_ALLYSTER_WIND_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ALLYSTERWIND;
//////////////////////////////////////////////////////////////////////////////

class ALLYSTERWIND : public SkillHandler 
{
public:
	ALLYSTERWIND() throw() {}
	~ALLYSTERWIND() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ALLYSTERWIND"; }
	SkillType_t getSkillType() const throw() { return SKILL_ALLYSTER_WIND; }

	virtual SkillResultType execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ALLYSTERWIND g_ALLYSTERWIND;

#endif // __SKILL_ALLYSTER_WIND_HANDLER_H__
