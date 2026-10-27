//////////////////////////////////////////////////////////////////////////////
// Filename    : EVGENEIABELLUM.h
// Written by  : crazydog
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_EVGENEIABELLUM__
#define __EFFECT_EVGENEIABELLUM__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EVGENEIABELLUM
//////////////////////////////////////////////////////////////////////////////

class EffectEVGENEIABELLUM : public Effect 
{
public:
	EffectEVGENEIABELLUM(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_EVGENEIABELLUM; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();

//public:
//	void setBonus( uint bonus ) { m_Bonus = bonus; }
//	uint getBonus() const { return m_Bonus; }

//private:
//	uint	m_Bonus;
};

#endif // __EFFECT_EffectEVGENEIABELLUM__
