//////////////////////////////////////////////////////////////////////////////
// Filename    : SATELLITEBOMB2.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_SATELLITE_BOMB_2_HANDLER_H__
#define __SKILL_SATELLITE_BOMB_2_HANDLER_H__

#include "SkillHandler.h"

//////////////////////////////////////////////////////////////////////////////
// class SATELLITEBOMB2;
//////////////////////////////////////////////////////////////////////////////

class SATELLITEBOMB2 : public SkillHandler 
{
public:
	SATELLITEBOMB2() throw() {}
	~SATELLITEBOMB2() throw() {}
	
public:
    string getSkillHandlerName() const throw() { return "SATELLITEBOMB2"; }
	SkillType_t getSkillType() const throw() { return SKILL_SATELLITE_BOMB_2; }

	virtual SkillResultType execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
	virtual SkillResultType execute(Slayer* pSlayer, ObjectID_t,  SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error);
//	void execute(Monster*, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void computeOutput(const SkillInput& input, SkillOutput& output);
};

// global variable declaration
extern SATELLITEBOMB2 g_SATELLITEBOMB2;

#endif // __SKILL_SATELLITE_BOMB_HANDLER_H__
