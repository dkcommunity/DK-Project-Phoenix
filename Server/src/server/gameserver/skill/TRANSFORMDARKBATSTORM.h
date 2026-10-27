//////////////////////////////////////////////////////////////////////////////
// Filename    : TRANSFORMDARKBATSTORM.h 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_TRANSFORM_DARK_BAT_STORM_HANDLER_H__
#define __SKILL_TRANSFORM_DARK_BAT_STORM_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class TRANSFORMDARKBATSTORM;
//////////////////////////////////////////////////////////////////////////////

class TRANSFORMDARKBATSTORM : public SkillHandler 
{
public:
	TRANSFORMDARKBATSTORM() throw();
	~TRANSFORMDARKBATSTORM() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "TRANSFORMDARKBATSTORM"; }
	SkillType_t getSkillType() const throw() { return SKILL_TRANSFORM_DARK_BAT_STORM; }

	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t ObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void execute(Monster* pMonster, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);

};

// global variable declaration
extern TRANSFORMDARKBATSTORM g_TRANSFORMDARKBATSTORM;

#endif //by rallser
