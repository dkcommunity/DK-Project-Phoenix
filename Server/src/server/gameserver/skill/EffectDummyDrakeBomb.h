//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectDummyDrakeBomb.h
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_DUMMY_DRAKE_BOMB__
#define __EFFECT_DUMMY_DRAKE_BOMB__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectDummyDrakeBomb
//////////////////////////////////////////////////////////////////////////////

class EffectDummyDrakeBomb : public Effect 
{
public:
	EffectDummyDrakeBomb(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_DUMMY_DRAKE_BOMB; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

public:
	HP_t getDamage() const throw() { return m_Damage; }
	void setDamage(Damage_t Damage) throw() { m_Damage = Damage; }

	void setUserObjectID(ObjectID_t oid) throw() { m_UserObjectID = oid; }
	ObjectID_t getUserObjectID() const throw() { return m_UserObjectID; }

private:
	Damage_t	m_Damage;
	ObjectID_t 	m_UserObjectID;
};

#endif // __EFFECT_DUMMY_DRAKE_BOMB__
