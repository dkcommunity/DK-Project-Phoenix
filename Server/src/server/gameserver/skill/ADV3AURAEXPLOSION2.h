//////////////////////////////////////////////////////////////////////////////
// Filename    : AURAEXPLOSION2 .h 
// Written By  : bigheart
// Description : 20080401 InfinityThunderBolt °­È­ÆÇ
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_AURA_EXPLOSION_2_HANDLER_H__
#define __SKILL_AURA_EXPLOSION_2_HANDLER_H__


#include "SkillHandler.h"
#include "Creature.h"
//////////////////////////////////////////////////////////////////////////////
// class AURAEXPLOSION2;
//////////////////////////////////////////////////////////////////////////////

class AURAEXPLOSION2 : public SkillHandler 
{
public:
	AURAEXPLOSION2() throw() {}
	~AURAEXPLOSION2() throw() {}
	
public :
    string getSkillHandlerName() const throw() { return "AURAEXPLOSION2"; }
	SkillType_t getSkillType() const throw() { return SKILL_AURA_EXPLOSION_2; }

	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

extern AURAEXPLOSION2 g_AURAEXPLOSION2;

#endif
