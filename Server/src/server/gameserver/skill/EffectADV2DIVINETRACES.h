#ifndef __EFFECT_ADV2DIVINETRACES__
#define __EFFECT_ADV2DIVINETRACES__

#include "Effect.h"

class EffectADV2DIVINETRACES : public Effect 
{
public:
	EffectADV2DIVINETRACES(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_ADV2DIVINETRACES; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error); 

	string toString() const throw();

public:
	int getHealPoint() throw() { return m_HealPoint;}
	void setHealPoint(HP_t HealPoint) throw(Error) { m_HealPoint = HealPoint; }

	int getTick() const { return m_Tick; }
	void setTick( int tick ) { m_Tick = tick; }

private :
	int m_Tick;
	HP_t m_HealPoint;
};

#endif
