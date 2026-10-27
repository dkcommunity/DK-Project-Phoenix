//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectEternity.h
// Written by  : 
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_DEPENDSOUL__
#define __EFFECT_DEPENDSOUL__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectEternity
//////////////////////////////////////////////////////////////////////////////

class EffectDEPENDSOUL : public Effect 
{
public:
	EffectDEPENDSOUL(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_DEPENDSOUL; }

	void affect() throw(Error) {}
	void affect(Creature* pCreature) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error); 

	string toString() const throw();

};

#endif // __EFFECT_ETERNITY__
