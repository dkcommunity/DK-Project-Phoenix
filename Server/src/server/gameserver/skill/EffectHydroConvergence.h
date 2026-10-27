//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectHydroConvergence.h
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_HYDRO_CONVERGENCE__
#define __EFFECT_HYDRO_CONVERGENCE__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectHydroConvergence
//////////////////////////////////////////////////////////////////////////////

class EffectHydroConvergence : public Effect 
{
public:
	EffectHydroConvergence(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_HYDRO_CONVERGENCE; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

public:
	Damage_t getDamage() const throw() { return m_Damage; }
	void setDamage(Damage_t Damage) throw() { m_Damage = Damage; }

	Damage_t getDotDamage() const throw() { return m_DotDamage; }
	void setDotDamage(Damage_t damage) throw() { m_DotDamage = damage; }

	void setUserObjectID(ObjectID_t oid) throw() { m_UserObjectID = oid; }
	ObjectID_t getUserObjectID() const throw() { return m_UserObjectID; }

	Damage_t getEffectDamage() const throw() {return m_EffectDamage; }
	void setEffectDamage(Damage_t damage) throw() { m_EffectDamage = damage; }

	Turn_t getTick() const throw() { return m_Tick; }
	void setTick(Turn_t tick) throw() { m_Tick = tick; }

private:
	Damage_t	m_Damage;
	Damage_t	m_DotDamage;
	Damage_t	m_EffectDamage;
	Turn_t		m_Tick;
	ObjectID_t 	m_UserObjectID;
};

#endif // __EFFECT_HYDRO_CONVERGENCE__
