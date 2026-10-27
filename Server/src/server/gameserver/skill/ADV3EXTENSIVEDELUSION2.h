//////////////////////////////////////////////////////////////////////////////
// Filename    : EXTENSIVEDELUSION2.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_EXTENSIVE_DELUSION_2_HANDLER_H__
#define __SKILL_EXTENSIVE_DELUSION_2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class EXTENSIVEDELUSION2;
//////////////////////////////////////////////////////////////////////////////

class EXTENSIVEDELUSION2 : public SkillHandler 
{
public:
	EXTENSIVEDELUSION2() throw();
	~EXTENSIVEDELUSION2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "EXTENSIVEDELUSION2"; }
	SkillType_t getSkillType() const throw() { return SKILL_EXTENSIVE_DELUSION_2; }

	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t ObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void execute(Monster* pMonster, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);

};

extern EXTENSIVEDELUSION2 g_EXTENSIVEDELUSION2;

#endif
