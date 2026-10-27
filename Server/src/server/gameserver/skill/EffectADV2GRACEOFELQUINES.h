#ifndef __EFFECT_ADV2GRACEOFELQUINES__
#define __EFFECT_ADV2GRACEOFELQUINES__

#include "Effect.h"
#include "EffectLoader.h"

class EffectADV2GRACEOFELQUINES : public Effect 
{
public:
	EffectADV2GRACEOFELQUINES(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_ADV2GRACEOFELQUINES; }

	void affect() throw(Error){}

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error);

	string toString() const throw();

public:
	void setBonus( uint bonus ) { m_Bonus = bonus; }
	uint getBonus() const { return m_Bonus; }

	void setAddbuff( uint addbuff ) { m_Addbuff = addbuff; }
	uint getAddbuff() const { return m_Addbuff; }

private:
	uint	m_Bonus;
	uint	m_Addbuff;
};

#endif
