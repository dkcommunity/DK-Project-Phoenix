//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSilence.h
// Written by  : 
// Description : Silence에 의한 방어력 하강 effect
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_SILENCE__
#define __EFFECT_SILENCE__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectSilence
//////////////////////////////////////////////////////////////////////////////

class EffectSilence : public Effect 
{
public:
	EffectSilence(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_SILENCE; }

	void affect() throw(Error) {}
	void affect(Creature* pCreature) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error); 

	string toString() const throw();

public:

private:
};

#endif // __EFFECT_SILENCE__
