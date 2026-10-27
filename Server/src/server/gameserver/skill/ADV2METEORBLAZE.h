//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2METEORBLAZE.h 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_ADV2_METEOR_BLAZE_H__
#define __SKILL_ADV2_METEOR_BLAZE_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class ADV2METEORBLAZE;
//////////////////////////////////////////////////////////////////////////////

class ADV2METEORBLAZE : public SkillHandler 
{
public:
	ADV2METEORBLAZE() throw() {}
	~ADV2METEORBLAZE() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "ADV2METEORBLAZE"; }
	SkillType_t getSkillType() const throw() { return SKILL_ADV2_METEOR_BLAZE; }

	virtual SkillResultType execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Ousters* pOusters, ObjectID_t, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID) throw(Error);
	void execute(Monster*, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);
	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern ADV2METEORBLAZE g_ADV2METEORBLAZE;

#endif // __SKILL_SHARP_HAIL_HANDLER_H__
