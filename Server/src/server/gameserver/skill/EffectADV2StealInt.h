#ifndef __EFFECT_ADV2StealInt__
#define __EFFECT_ADV2StealInt__

#include "Effect.h"
#include "EffectLoader.h"

class EffectADV2StealInt : public Effect 
{
public:
	EffectADV2StealInt(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_ADV2StealInt; }

	void affect() throw(Error){}

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

public:
	void setReflection( int ref ) { m_Reflection = ref; }
	int getReflection() const { return m_Reflection; }

private:
	int m_Reflection;
};

#endif
