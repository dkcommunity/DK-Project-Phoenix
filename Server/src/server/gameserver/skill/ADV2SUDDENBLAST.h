//////////////////////////////////////////////////////////////////////////////
// Filename    : SetAfire.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ADV2_SUDDEN_BLAST_HANDLER_H__
#define __SKILL_ADV2_SUDDEN_BLAST_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class SetAfire;
//////////////////////////////////////////////////////////////////////////////

class ADV2SUDDENBLAST : public SkillHandler 
{
public:
	ADV2SUDDENBLAST() throw() {}
	~ADV2SUDDENBLAST() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2SUDDENBLAST"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_SUDDEN_BLAST; }

	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ADV2SUDDENBLAST g_ADV2SUDDENBLAST;

#endif // __SKILL_SET_AFIRE_HANDLER_H__
