//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectShineSword.h
// Written by  : bezz
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_SHINE_SWORD__
#define __EFFECT_SHINE_SWORD__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectShineSword
//////////////////////////////////////////////////////////////////////////////

class EffectShineSword : public Effect 
{
public:
	EffectShineSword(Zone* pZone, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_SHINE_SWORD; }

	void affect() throw(Error);
	
	void unaffect() throw(Error);

	string toString() const throw();

public:
	Damage_t getDamage() const throw() { return m_Damage; }
	void setDamage( Damage_t damage ) { m_Damage = damage; }

	Turn_t	getTick() throw() { return m_Tick; }
	void setTick(Turn_t tick) throw() { m_Tick = tick; }

	ObjectID_t	getUserObjectID() const throw() { return m_UserObjectID; }
	void setUserObjectID(ObjectID_t oid) throw() { m_UserObjectID = oid; }
	
private:
	Coord_t		m_X;
	Coord_t		m_Y;
	Damage_t	m_Damage;
	Turn_t		m_Tick;
	ObjectID_t	m_UserObjectID;
};

#endif
