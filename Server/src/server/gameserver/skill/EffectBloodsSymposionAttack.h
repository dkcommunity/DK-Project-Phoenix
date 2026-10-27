//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBloodsSymposionAttack.h
// Written by  : 
// Description : BloodsSymposionAttack에 의한 방어력 하강 effect
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_BLOODS_SYMPOSION_ATTACK__
#define __EFFECT_BLOODS_SYMPOSION_ATTACK__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectBloodsSymposionAttack
//////////////////////////////////////////////////////////////////////////////

class EffectBloodsSymposionAttack : public Effect 
{
public:
	EffectBloodsSymposionAttack(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_BLOODS_SYMPOSION_ATTACK; }

	void affect() throw(Error) {}
	void affect(Creature* pCreature) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect() throw(Error); 

	string toString() const throw();

public:
	int getDefensePenalty(void) const { return m_DefensePenalty; }
	void setDefensePenalty(int penalty) { m_DefensePenalty = penalty; }

	int getProtectionPenalty(void) const { return m_ProtectionPenalty; }
	void setProtectionPenalty(int penalty) { m_ProtectionPenalty = penalty; }

	int getHPPenalty(void) const { return m_HPPenalty; }
	void setHPPenalty(int penalty) { m_HPPenalty = penalty; }

private:
	int m_DefensePenalty;
	int m_ProtectionPenalty;
	int m_HPPenalty;
};

#endif // __EFFECT_BLOODS_SYMPOSION_ATTACK__
