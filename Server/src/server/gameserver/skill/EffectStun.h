//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectStun.h
// Written by  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_STUN__
#define __EFFECT_STUN__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectStun
//////////////////////////////////////////////////////////////////////////////

class EffectStun : public Effect 
{
public:
	EffectStun(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_STUN; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();
};

#endif // __EFFECT_STUN__
