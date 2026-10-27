//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectDyingCreature.h
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_DYING_CREATURE__
#define __EFFECT_DYING_CREATURE__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectDyingCreature
//////////////////////////////////////////////////////////////////////////////

class EffectDyingCreature : public Effect 
{
public:
	EffectDyingCreature(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_DYING_CREATURE; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

public:

private:
};

#endif // __EFFECT_DYING_CREATURE__
