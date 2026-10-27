//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectRingOfFlare.h
// Written by  : crazydog
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_ADV2CYCLINGWARROCK__
#define __EFFECT_ADV2CYCLINGWARROCK__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectRingOfFlare
//////////////////////////////////////////////////////////////////////////////

class EffectADV2CYCLINGWARROCK : public Effect 
{
public:
	EffectADV2CYCLINGWARROCK(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_ADV2CYCLINGWARROCK; }
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

#endif // __EFFECT_RING_OF_FLARE__
