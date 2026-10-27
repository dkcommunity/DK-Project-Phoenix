//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2METEORBLAZE.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ADV2_METEOR_BLAZE2_H__
#define __SKILL_ADV2_METEOR_BLAZE2_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ADV2METEORBLAZE2;
//////////////////////////////////////////////////////////////////////////////

class ADV2METEORBLAZE2 : public SkillHandler 
{
public:
	ADV2METEORBLAZE2() throw() {}
	~ADV2METEORBLAZE2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2METEORBLAZE2"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_METEOR_BLAZE2; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	void execute(Monster* pMonster, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);
	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ADV2METEORBLAZE2 g_ADV2METEORBLAZE2;

#endif // __SKILL_SHARP_HAIL_HANDLER_H__
