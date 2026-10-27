//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodyNail.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ADV2_CHAOS_SPLITTER_HANDLER_H__
#define __SKILL_ADV2_CHAOS_SPLITTER_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class BloodyNail;
//////////////////////////////////////////////////////////////////////////////

class ADV2CHAOSSPLITTER: public SkillHandler 
{
public:
	ADV2CHAOSSPLITTER() throw();
	~ADV2CHAOSSPLITTER() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2CHAOSSPLITTER"; }

	SkillType_t getSkillType() const throw() { return SKILL_ADV2_CHAOS_SPLITTER; }

	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t ObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	//void execute(Monster* pMonster, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);

protected:
	POINT m_pLethalClawMask[9];
};
extern ADV2CHAOSSPLITTER g_ADV2CHAOSSPLITTER;

#endif // __SKILL_BLOODY_NAIL_HANDLER_H__
