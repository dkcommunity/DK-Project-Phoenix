//////////////////////////////////////////////////////////////////////////////
// Filename    : AURAEXPLOSION1 .h 
// Written By  : bigheart
// Description : 20080401 InfinityThunderBolt °­È­ÆÇ
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_AURA_EXPLOSION_1_HANDLER_H__
#define __SKILL_AURA_EXPLOSION_1_HANDLER_H__


#include "SkillHandler.h"
#include "Creature.h"
//////////////////////////////////////////////////////////////////////////////
// class AURAEXPLOSION1;
//////////////////////////////////////////////////////////////////////////////

class AURAEXPLOSION1 : public SkillHandler 
{
public:
	AURAEXPLOSION1() throw() {}
	~AURAEXPLOSION1() throw() {}
	
public :
    string getSkillHandlerName() const throw() { return "AURAEXPLOSION1"; }
	SkillType_t getSkillType() const throw() { return SKILL_AURA_EXPLOSION_1; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern AURAEXPLOSION1 g_AURAEXPLOSION1;

#endif
