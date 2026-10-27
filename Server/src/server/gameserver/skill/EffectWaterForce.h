//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectWaterForce.h
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_WATER_FORCE__
#define __EFFECT_WATER_FORCE__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectWaterForce
//////////////////////////////////////////////////////////////////////////////

class EffectWaterForce : public Effect 
{
public:
	EffectWaterForce(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_WATER_FORCE; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	// unaffect method
	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);
	string toString() const throw();

public:
	void setDelay(Turn_t Delay) throw() { m_Delay = Delay; }
	Turn_t getDelay() const throw() { return m_Delay; }

private:
	Turn_t  m_Delay;

};

#endif // __EFFECT_WATER_FORCE__

