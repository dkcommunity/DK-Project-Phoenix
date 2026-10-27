//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectPartyAuraResist.h
// Written by  : bezz 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_PARTY_AURA_RESIST__
#define __EFFECT_PARTY_AURA_RESIST__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectPartyAuraResist
//////////////////////////////////////////////////////////////////////////////

class EffectPartyAuraResist : public Effect 
{
public:
	EffectPartyAuraResist(Creature* pCreature) throw(Error);

public:
	// get/set EffectClass
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_PARTY_AURA_RESIST; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();
};

#endif // __EFFECT_PARTY_AURA_RESIST__

