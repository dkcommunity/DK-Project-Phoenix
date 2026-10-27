//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectPartyAuraToHit.h
// Written by  : bezz 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_PARTY_AURA_TOHIT__
#define __EFFECT_PARTY_AURA_TOHIT__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectPartyAuraToHit
//////////////////////////////////////////////////////////////////////////////

class EffectPartyAuraToHit : public Effect 
{
public:
	EffectPartyAuraToHit(Creature* pCreature) throw(Error);

public:
	// get/set EffectClass
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_PARTY_AURA_TOHIT; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();
};

#endif // __EFFECT_PARTY_AURA_TOHIT__

