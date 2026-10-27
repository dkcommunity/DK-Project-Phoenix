//////////////////////////////////////////////////////////////////////////////
// Filename    : SatelliteBomb.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_SATELLITE_BOMB_HANDLER_H__
#define __SKILL_SATELLITE_BOMB_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class SatelliteBomb;
//////////////////////////////////////////////////////////////////////////////

class SatelliteBomb : public SkillHandler 
{
public:
	SatelliteBomb() throw() {}
	~SatelliteBomb() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "SatelliteBomb"; }
	SkillType_t getSkillType() const throw() { return SKILL_SATELLITE_BOMB; }

	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
//	void execute(Monster*, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern SatelliteBomb g_SatelliteBomb;

#endif // __SKILL_SATELLITE_BOMB_HANDLER_H__
