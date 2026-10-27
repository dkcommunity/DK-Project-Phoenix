//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectHolyCross.h
// Written by  : elca@ewestsoft.com
// Description : 성직마법 HolyCross의 Effect를 처리해주기 위한 클래스이다.
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_HOLY_CROSS__
#define __EFFECT_HOLY_CROSS__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectHolyCross
//////////////////////////////////////////////////////////////////////////////

class EffectHolyCross : public Effect 
{
public:
	EffectHolyCross(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_HOLY_CROSS; }

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

#endif // __EFFECT_HOLY_CROSS__

