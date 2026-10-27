//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSimplePassive.h
// Written by  : bezz 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_SIMPLE_PASSIVE__
#define __EFFECT_SIMPLE_PASSIVE__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectSimplePassive
//////////////////////////////////////////////////////////////////////////////

class EffectSimplePassive : public Effect 
{
public:
	EffectSimplePassive(Creature* pCreature) throw(Error);

public:
	// get/set EffectClass
    EffectClass getEffectClass() const throw() { return m_EffectClass; }
	void setEffectClass( EffectClass effectClass ) { m_EffectClass = effectClass; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();

private:
	EffectClass		m_EffectClass;

};

#endif // __EFFECT_SIMPLE_PASSIVE__

