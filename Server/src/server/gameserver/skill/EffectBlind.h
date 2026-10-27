//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBlind.h
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_BLIND__
#define __EFFECT_BLIND__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectBlind
//////////////////////////////////////////////////////////////////////////////

class EffectBlind : public Effect 
{
public:
	EffectBlind(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_BLIND; }

	void affect() throw(Error) {}
	void affect(Creature* pCreature) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error); 

	string toString() const throw();

private:
};

#endif // __EFFECT_BLIND__
