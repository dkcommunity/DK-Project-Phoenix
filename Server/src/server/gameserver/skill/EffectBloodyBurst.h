//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBloodyBurst.h
// Written by  : crazydog
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_BLOODY_BURST__
#define __EFFECT_BLOODY_BURST__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectBloodyBurst
//////////////////////////////////////////////////////////////////////////////

class EffectBloodyBurst : public Effect 
{
public:
	EffectBloodyBurst(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_BLOODY_BURST; }
    EffectClass getSendEffectClass() const throw() { return EFFECT_CLASS_BLOODY_BURST; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();

private:
};

#endif // __EFFECT_BLOODY_BURST__
