//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectHydroConvergenceBomb.h
// Written by  : bezz
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_HYDRO_CONVERGENCE_BOMB__
#define __EFFECT_HYDRO_CONVERGENCE_BOMB__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectHydroConvergenceBomb
//////////////////////////////////////////////////////////////////////////////

class EffectHydroConvergenceBomb : public Effect 
{
public:
	EffectHydroConvergenceBomb(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_HYDRO_CONVERGENCE_BOMB; }

	void affect() throw(Error);
	void affect( Creature* pCreature ) throw(Error);
	
	void unaffect() throw(Error);

	string toString() const throw();

public:
	Damage_t getDamage() const throw() { return m_Damage; }
	void setDamage( Damage_t damage ) { m_Damage = damage; }

	ObjectID_t	getUserObjectID() const throw() { return m_UserObjectID; }
	void setUserObjectID(ObjectID_t oid) throw() { m_UserObjectID = oid; }
	
private:
	Damage_t	m_Damage;
	ObjectID_t	m_UserObjectID;
};

#endif
