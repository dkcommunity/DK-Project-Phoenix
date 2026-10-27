//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSquallyBarrier1.h
// Written by  : ksym555
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_SQUALLY_BARRIER_1__
#define __EFFECT_SQUALLY_BARRIER_1__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectSquallyBarrier1
//////////////////////////////////////////////////////////////////////////////

class EffectSquallyBarrier1 : public Effect 
{
public:
	EffectSquallyBarrier1(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_SQUALLY_BARRIER1; }

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

#endif // __EFFECT_SQUALLY_BARRIER_1__
