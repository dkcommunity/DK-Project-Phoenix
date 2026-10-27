//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectPartyAuraLuck.h
// Written by  : bezz 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_PARTY_AURA_LUCK__
#define __EFFECT_PARTY_AURA_LUCK__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectPartyAuraLuck
//////////////////////////////////////////////////////////////////////////////

class EffectPartyAuraLuck : public Effect 
{
public:
	EffectPartyAuraLuck(Creature* pCreature) throw(Error);

public:
	// get/set EffectClass
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_PARTY_AURA_LUCK; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();
};

#endif // __EFFECT_PARTY_AURA_LUCK__

