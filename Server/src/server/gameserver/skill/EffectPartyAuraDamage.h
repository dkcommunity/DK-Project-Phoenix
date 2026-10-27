//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectPartyAuraDamage.h
// Written by  : bezz 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_PARTY_AURA_DAMAGE__
#define __EFFECT_PARTY_AURA_DAMAGE__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectPartyAuraDamage
//////////////////////////////////////////////////////////////////////////////

class EffectPartyAuraDamage : public Effect 
{
public:
	EffectPartyAuraDamage(Creature* pCreature) throw(Error);

public:
	// get/set EffectClass
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_PARTY_AURA_DAMAGE; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();
};

#endif // __EFFECT_PARTY_AURA_DAMAGE__

