//////////////////////////////////////////////////////////////////////////////
// Filename    : Bless.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BLESS_HANDLER_H__
#define __SKILL_BLESS_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class Bless;
//////////////////////////////////////////////////////////////////////////////

class Bless : public SkillHandler 
{
public:
	Bless() throw() {}
	~Bless() throw() {}
	
public :
    string getSkillHandlerName() const throw() { return "Bless"; }
	SkillType_t getSkillType() const throw() { return SKILL_BLESS; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern Bless g_Bless;

#endif // __SKILL_BLESS_HANDLER_H__
