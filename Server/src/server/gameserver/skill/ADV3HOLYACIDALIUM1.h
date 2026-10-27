//////////////////////////////////////////////////////////////////////////////
// Filename    : HOLYACIDALIUM1 .h 
// Written By  : bigheart
// Description : 20080401 InfinityThunderBolt °­È­ÆÇ
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_HOLY_ACIDALIUM_1_HANDLER_H__
#define __SKILL_HOLY_ACIDALIUM_1_HANDLER_H__


#include "SkillHandler.h"
#include "Creature.h"
//////////////////////////////////////////////////////////////////////////////
// class HOLYACIDALIUM1;
//////////////////////////////////////////////////////////////////////////////

class HOLYACIDALIUM1 : public SkillHandler 
{
public:
	HOLYACIDALIUM1() throw() {}
	~HOLYACIDALIUM1() throw() {}
	
public :
    string getSkillHandlerName() const throw() { return "HOLYACIDALIUM1"; }
	SkillType_t getSkillType() const throw() { return SKILL_HOLY_ACIDALIUM_1; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern HOLYACIDALIUM1 g_HOLYACIDALIUM1;

#endif
