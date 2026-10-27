//////////////////////////////////////////////////////////////////////////////
// Filename    : ENRAGECONQUROR.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ENRAGE_CONQUROR_HANDLER_H__
#define __SKILL_ENRAGE_CONQUROR_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ENRAGECONQUROR;
//////////////////////////////////////////////////////////////////////////////

class ENRAGECONQUROR : public SkillHandler 
{
public:
	ENRAGECONQUROR() throw() {}
	~ENRAGECONQUROR() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ENRAGECONQUROR"; }
	SkillType_t getSkillType() const throw() { return SKILL_ENRAGE_CONQUROR; }

	//virtual SkillResultType execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
        virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ENRAGECONQUROR g_ENRAGECONQUROR;

#endif // __SKILL_CURSE_OF_BLOOD_HANDLER_H__
