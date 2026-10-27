//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectHeavenGround.h
// Written by  : bezz
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_GLORYGROUND__
#define __EFFECT_GLORYGROUND__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectHeavenGround
//////////////////////////////////////////////////////////////////////////////

class EffectGLORYGROUND : public Effect 
{
public:
	EffectGLORYGROUND(Zone* pZone, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_GLORYGROUND; }

	void affect() throw(Error);
	void affect(Zone* pZone, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

	void unaffect() throw(Error);

	string toString() const throw();

public:
	void setTick(Turn_t tick) throw() { m_Tick = tick; }
	Turn_t	getTick() const throw() { return m_Tick; }

	void setUserObjectID(ObjectID_t oid) throw() { m_UserObjectID = oid; }
	ObjectID_t getUserObjectID() const throw() { return m_UserObjectID; }

	void setDamage(Damage_t damage) throw() { m_Damage = damage; }
	Damage_t getDamage() throw() { return m_Damage; }

private:
	Damage_t	m_Damage;
	Turn_t		m_Tick;
	ObjectID_t	m_UserObjectID;
};

#endif
