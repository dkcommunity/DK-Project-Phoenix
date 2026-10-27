//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSatelliteBombFire.h
// Written by  : bezz
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_SATELLITE_BOMB_FIRE__
#define __EFFECT_SATELLITE_BOMB_FIRE__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectSatelliteBombFire
//////////////////////////////////////////////////////////////////////////////

class EffectSatelliteBombFire : public Effect 
{
public:
	EffectSatelliteBombFire(Creature* pCreature, Zone* pZone, ZoneCoord_t x, ZoneCoord_t y) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_SATELLITE_BOMB_FIRE; }

	void affect() throw(Error){}
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

public:
	void setSTR(Attr_t str) { m_STR = str; }
	Attr_t getSTR() { return m_STR; }

	void setSkillLevel( Attr_t level ) { m_SkillLevel = level; }
	Attr_t getSkillLevel()  { return m_SkillLevel; }

private:
	Attr_t	m_STR;
	Attr_t	m_SkillLevel;
};

#endif
