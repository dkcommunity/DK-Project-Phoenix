//////////////////////////////////////////////////////////////////////////////
// Filename    : IntimateGrail2.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_INTIMATE_GRAIL_2_HANDLER_H__
#define __SKILL_INTIMATE_GRAIL_2_HANDLER_H__

#include "SkillHandler.h"
#include "Creature.h"
//////////////////////////////////////////////////////////////////////////////
// class IntimateGrail2;
//////////////////////////////////////////////////////////////////////////////

class INTIMATEGRAIL2 : public SkillHandler 
{
public:
	INTIMATEGRAIL2() throw() {}
	~INTIMATEGRAIL2() throw() {}
	
public :
    string getSkillHandlerName() const throw() { return "INTIMATEGRAIL2"; }
	SkillType_t getSkillType() const throw() { return SKILL_INTIMATE_GRAIL_2; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern INTIMATEGRAIL2 g_INTIMATEGRAIL2;

#endif // __SKILL_INTIMATE_GRAIL_HANDLER_2_H__
