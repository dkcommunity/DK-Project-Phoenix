//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectRingOfFlare.h
// Written by  : crazydog
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_RING_OF_FLARE2__
#define __EFFECT_RING_OF_FLARE2__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectRingOfFlare2
//////////////////////////////////////////////////////////////////////////////

class EffectRingOfFlare2 : public Effect 
{
public:
	EffectRingOfFlare2(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_RING_OF_FLARE2; }
	EffectClass getSendEffectClass() const throw() { return m_SendEffectClass; }

	void affect() throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();

public:
	void setSendEffectClass(EffectClass eClass) throw() { m_SendEffectClass = eClass; }

	void setDamage( uint bonus ) { m_Damage = bonus; }
	uint getDamage() const { return m_Damage; }

private:
	EffectClass m_SendEffectClass;
	uint	m_Damage;
};

#endif
