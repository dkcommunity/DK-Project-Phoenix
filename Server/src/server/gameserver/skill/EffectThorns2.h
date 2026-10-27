//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectThorns.h
// Written by  : bigheart
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_THORNS2__
#define __EFFECT_THORNS2__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectThorns2
//////////////////////////////////////////////////////////////////////////////

class EffectThorns2 : public Effect 
{
public:
	EffectThorns2(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_THORNS2; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

public:
	ObjectID_t getCasterID() const { return m_CasterID; }
	void setCasterID( ObjectID_t casterID ) { m_CasterID = casterID; }

	Damage_t getDamage() const { return m_Damage; }
	void setDamage( Damage_t damage ) { m_Damage = damage; }
	
//	void setLevel(SkillLevel_t level);

	void setDelay(Turn_t delay) { m_Delay = delay; }
	Turn_t getDelay() const { return m_Delay; }

private:

	ObjectID_t m_CasterID;
	Turn_t m_Delay;
	Damage_t m_Damage;
//	EffectClass m_EffectClass;
};

#endif
