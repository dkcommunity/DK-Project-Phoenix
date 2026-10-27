//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectTEMENOS.h
// Written by  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_TEMENOS__
#define __EFFECT_TEMENOS__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectTEMENOS
//////////////////////////////////////////////////////////////////////////////

class EffectTEMENOS : public Effect
{
public:
    EffectTEMENOS(Creature* pCreature) throw(Error);

public:
	EffectClass getEffectClass() const throw() { return EFFECT_CLASS_TEMENOS; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	int getDamageReduceRatio() const { return m_DamageReduceRatio; }
	void setDamageReduceRatio( int ratio ) { m_DamageReduceRatio = ratio; }

	string toString() const throw();

private:
	int		m_DamageReduceRatio;
};

#endif // __EFFECT_CAUSE_CRITICAL_WOUNDS__

