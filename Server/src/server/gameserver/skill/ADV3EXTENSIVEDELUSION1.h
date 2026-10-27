//////////////////////////////////////////////////////////////////////////////
// Filename    : EXTENSIVEDELUSION1.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_EXTENSIVE_DELUSION_1_HANDLER_H__
#define __SKILL_EXTENSIVE_DELUSION_1_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class EXTENSIVEDELUSION1;
//////////////////////////////////////////////////////////////////////////////

class EXTENSIVEDELUSION1 : public SkillHandler 
{
public:
	EXTENSIVEDELUSION1() throw();
	~EXTENSIVEDELUSION1() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "EXTENSIVEDELUSION1"; }
	SkillType_t getSkillType() const throw() { return SKILL_EXTENSIVE_DELUSION_1; }

	virtual SkillResultType execute(Vampire* pVampire, ObjectID_t ObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID) throw(Error);

	void execute(Monster* pMonster, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);

};

extern EXTENSIVEDELUSION1 g_EXTENSIVEDELUSION1;

#endif
