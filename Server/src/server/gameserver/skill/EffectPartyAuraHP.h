//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectPartyAuraHP.h
// Written by  : bezz 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_PARTY_AURA_HP__
#define __EFFECT_PARTY_AURA_HP__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectPartyAuraHP
//////////////////////////////////////////////////////////////////////////////

class EffectPartyAuraHP : public Effect 
{
public:
	EffectPartyAuraHP(Creature* pCreature) throw(Error);

public:
	// get/set EffectClass
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_PARTY_AURA_HP; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();
};

#endif // __EFFECT_PARTY_AURA_HP__

