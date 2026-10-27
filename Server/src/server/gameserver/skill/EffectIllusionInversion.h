//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectIllusionInversion.h
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_ILLUSION_INVERSION__
#define __EFFECT_ILLUSION_INVERSION__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectIllusionInversion
//////////////////////////////////////////////////////////////////////////////

class EffectIllusionInversion : public Effect 
{
public:
	EffectIllusionInversion(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_ILLUSION_INVERSION; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

public:
	void setDamage(Damage_t Damage) { m_Damage = Damage; }
	Damage_t getDamage() const throw() { return m_Damage; }

	void setTick(Turn_t Tick) throw() { m_Tick = Tick; }
	Turn_t getTick() const throw() { return m_Tick; }

	void setUserObjectID(ObjectID_t oid) throw() { m_UserObjectID = oid; }
	ObjectID_t getUserObjectID() const throw() { return m_UserObjectID; }
	
	void setHeal(bool bHeal) { m_bHeal = bHeal; }
	bool getHeal() { return m_bHeal; }

	void setHealDamage(Damage_t healDamage) { m_HealDamage = healDamage; }
	Damage_t getHealDamage() { return m_HealDamage; }

private:
	bool	m_bHeal;
	Damage_t m_Damage;
	Damage_t m_HealDamage;
	Turn_t   m_Tick;
	ObjectID_t m_UserObjectID;
};

#endif // __EFFECT_ILLUSION_INVERSION__
