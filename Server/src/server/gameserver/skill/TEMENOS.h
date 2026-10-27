//////////////////////////////////////////////////////////////////////////////
// Filename    : TEMENOS.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_TEMENOS_HANDLER_H__
#define __SKILL_TEMENOS_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class TEMENOS;
//////////////////////////////////////////////////////////////////////////////

class TEMENOS : public SkillHandler 
{
public:
	TEMENOS() throw() {}
	~TEMENOS() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "TEMENOS"; }
	SkillType_t getSkillType() const throw() { return SKILL_TEMENOS; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern TEMENOS g_TEMENOS;

#endif // __SKILL_TEMENOS_HANDLER_H__
