//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectPartyAuraDefense.h
// Written by  : bezz 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_PARTY_AURA_DEFENSE__
#define __EFFECT_PARTY_AURA_DEFENSE__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectPartyAuraDefense
//////////////////////////////////////////////////////////////////////////////

class EffectPartyAuraDefense : public Effect 
{
public:
	EffectPartyAuraDefense(Creature* pCreature) throw(Error);

public:
	// get/set EffectClass
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_PARTY_AURA_DEFENSE; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();
};

#endif // __EFFECT_PARTY_AURA_DEFENSE__

