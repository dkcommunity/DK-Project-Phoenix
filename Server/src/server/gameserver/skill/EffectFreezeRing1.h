//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectFreezeRing1.h
// Written by  : bezz
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_FREEZE_RING1__
#define __EFFECT_FREEZE_RING1__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectFreezeRing1
//////////////////////////////////////////////////////////////////////////////

class EffectFreezeRing1 : public Effect 
{
public:
	EffectFreezeRing1(Creature* pCreature, ZoneCoord_t X, ZoneCoord_t Y) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_FREEZE_RING1; }

	void affect() throw(Error);
	
	void unaffect() throw(Error);

	string toString() const throw();

public:
	Damage_t getDamage() const throw() { return m_Damage; }
	void setDamage( Damage_t damage ) { m_Damage = damage; }
	Duration_t getDuration() const throw() { return m_Duration; }
	void setDuration( Duration_t duration ) { m_Duration = duration; }

	ObjectID_t getUserObjectID(void) const { return m_UserObjectID; }
	void setUserObjectID(ObjectID_t UserObjectID ) { m_UserObjectID = UserObjectID; }

private:
	Coord_t		m_X;
	Coord_t		m_Y;
	Damage_t	m_Damage;
	Creature*	m_pCreature;
	Duration_t	m_Duration;
	ObjectID_t   m_UserObjectID;
};

#endif
