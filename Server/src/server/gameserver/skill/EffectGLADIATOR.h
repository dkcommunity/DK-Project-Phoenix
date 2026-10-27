//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectGLADIATOR.h
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_GLADIATOR__
#define __EFFECT_GLADIATOR__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectBerserker
//////////////////////////////////////////////////////////////////////////////

class EffectGLADIATOR : public Effect 
{
public:
	EffectGLADIATOR(Creature* pCreature) throw(Error);

public:
	void affect() throw(Error) {}
	void affect(Creature* pCreature) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error); 

    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_GLADIATOR; }

	string toString() const throw();

public:
	int getDefensePenalty(void) const { return m_DefensePenalty; }
	void setDefensePenalty(int penalty) { m_DefensePenalty = penalty; }

	int getDamagePenalty(void) const { return m_DamagePenalty; }
	void setDamagePenalty(int penalty) { m_DamagePenalty = penalty; }

	int getToHitBonus(void) const { return m_ToHitBonus; }
	void setToHitBonus(int bonus) { m_ToHitBonus = bonus; }
	
//	int getDamageBonus(void) const { return m_DamageBonus; }
//	void setDamageBonus(int bonus) { m_DamageBonus = bonus; }

private :
	int m_DefensePenalty;
	int m_DamagePenalty;
	int m_ToHitBonus;
//	int m_DamageBonus;
};

#endif // __EFFECT_GLADIATOR__
