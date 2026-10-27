//////////////////////////////////////////////////////////////////////////////
// Filename    : IllusionInversion.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_SPECTOR_INVERSE_HANDLER_H__
#define __SKILL_SPECTOR_INVERSE_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class IllusionInversion;
//////////////////////////////////////////////////////////////////////////////

class SPECTORINVERSE : public SkillHandler 
{
public:
	SPECTORINVERSE() throw() {}
	~SPECTORINVERSE() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "SPECTORINVERSE"; }

	SkillType_t getSkillType() const throw() { return SKILL_SPECTOR_INVERSE; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern SPECTORINVERSE g_SPECTORINVERSE;

#endif // __SKILL_ILLUSION_INVERSION_HANDLER_H__
