#ifndef __EFFECT_MK1FireAttack2__
#define __EFFECT_MK1FireAttack2__

#include "Effect.h"
#include "EffectLoader.h"

class EffectMK1Fire2 : public Effect 
{
public:
	EffectMK1Fire2(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY, bool bPlayer = false) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_MK1FireAttack2; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

public:
	int getDamage(void) const { return m_Damage; }
	void setDamage(int damage) { m_Damage = damage; }

	Turn_t getDelay() const { return m_Delay; }
	void setDelay(Turn_t Delay) { m_Delay = Delay; }

	ObjectID_t getUserObjectID(void) const { return m_UserObjectID; }
	void setUserObjectID(ObjectID_t UserObjectID ) { m_UserObjectID = UserObjectID; }

	Turn_t getTick() { return m_Tick; }
	void setTick(Turn_t tick) { m_Tick = tick; }

private:
	int     m_Damage;
	Turn_t  m_Delay;
	ObjectID_t   m_UserObjectID;
	bool	m_bPlayer;
	Turn_t	m_Tick;
};


class EffectMK1Fire2Loader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_MK1FireAttack2; }
	virtual string getEffectClassName() const throw() { return "EffectMK1Fire2"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};

extern EffectMK1Fire2Loader* g_pEffectMK1Fire2Loader;

#endif
