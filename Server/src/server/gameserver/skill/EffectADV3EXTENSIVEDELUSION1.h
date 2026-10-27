#ifndef __EFFECT_ADV3EXTENSIVEDELUSION1__
#define __EFFECT_ADV3EXTENSIVEDELUSION1__

#include "Effect.h"

class EffectADV3EXTENSIVEDELUSION1 : public Effect 
{
public:
	EffectADV3EXTENSIVEDELUSION1(Creature* pCreature) throw(Error);

public:

    EffectClass getEffectClass() const throw() { return m_EffectClass; }
	void setEffectClass( EffectClass effectClass ) { m_EffectClass = effectClass; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();

private:
	EffectClass		m_EffectClass;

};

#endif

