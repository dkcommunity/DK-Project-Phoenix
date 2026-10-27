//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectEatCorpse.h
// Written by  : excel96
// Description : 
// EatCorpse에 의해서 생성되는 산성 데미지 약화 이펙트이다.
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_EAT_CORPSE__
#define __EFFECT_EAT_CORPSE__

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectEatCorpse
//////////////////////////////////////////////////////////////////////////////

class EffectEatCorpse : public Effect 
{
public:
	EffectEatCorpse(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_EAT_CORPSE; }

	void	affect() throw(Error);
	void	affect(Creature* pCreature) throw(Error);

	void	unaffect() throw(Error);
	void	unaffect(Creature* pCreature) throw(Error);

	void	setRemainHP(HP_t hp) { m_RemainHP = hp; }
	HP_t	getRemainHP() { return m_RemainHP; }

	Turn_t	getDelay() const throw() { return m_Delay; }
	void	setDelay(Turn_t Delay) { m_Delay = Delay; }

	string	toString() const throw();

private:
	Turn_t	m_Delay;
	HP_t	m_RemainHP;
};

#endif // __EFFECT_EAT_CORPSE__
