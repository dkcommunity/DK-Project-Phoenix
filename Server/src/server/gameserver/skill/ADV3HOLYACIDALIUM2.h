//////////////////////////////////////////////////////////////////////////////
// Filename    : HOLYACIDALIUM1 .h 
// Written By  : bigheart
// Description : 20080401 InfinityThunderBolt °­È­ÆÇ
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_HOLY_ACIDALIUM_2_HANDLER_H__
#define __SKILL_HOLY_ACIDALIUM_2_HANDLER_H__


#include "SkillHandler.h"
#include "Creature.h"
//////////////////////////////////////////////////////////////////////////////
// class HOLYACIDALIUM2;
//////////////////////////////////////////////////////////////////////////////

class HOLYACIDALIUM2 : public SkillHandler 
{
public:
	HOLYACIDALIUM2() throw() {}
	~HOLYACIDALIUM2() throw() {}
	
public :
    string getSkillHandlerName() const throw() { return "HOLYACIDALIUM2"; }
	SkillType_t getSkillType() const throw() { return SKILL_HOLY_ACIDALIUM_2; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern HOLYACIDALIUM2 g_HOLYACIDALIUM2;

#endif
