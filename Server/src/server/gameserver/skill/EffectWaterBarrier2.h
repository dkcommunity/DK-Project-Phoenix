#ifndef __EFFECT_WATER_BARRIER2__
#define __EFFECT_WATER_BARRIER2__

#include "Effect.h"
#include "EffectLoader.h"

class EffectWaterBarrier2 : public Effect 
{
public:
	EffectWaterBarrier2(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_WATER_BARRIER2; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();

public:
	void setBonus( uint bonus ) { m_Bonus = bonus; }
	uint getBonus() const { return m_Bonus; }

private:
	uint	m_Bonus;
};

#endif
