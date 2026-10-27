#ifndef __EFFECT_ADV2SOLARFLARE2__
#define __EFFECT_ADV2SOLARFLARE2__

#include "Effect.h"
#include "EffectLoader.h"

class EffectADV2SOLARFLARE2 : public Effect 
{
public:
	EffectADV2SOLARFLARE2(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY, bool bPlayer = false) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_ADV2SOLARFLARE2; }

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

class EffectADV2SOLARFLARE2Loader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_ADV2SOLARFLARE2; }
	virtual string getEffectClassName() const throw() { return "EffectADV2SOLARFLARE2"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};

extern EffectADV2SOLARFLARE2Loader* g_pEffectADV2SOLARFLARE2Loader;

#endif
