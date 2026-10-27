//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectMagmaDetonation.h
// Written by  : ksym555
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_MAGMA_DETONATION__
#define __EFFECT_MAGMA_DETONATION__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectMagmaDetonation
//////////////////////////////////////////////////////////////////////////////

class EffectMagmaDetonation : public Effect 
{
public:
	EffectMagmaDetonation(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY, bool bPlayer = false) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_MAGMA_DETONATION1; }

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

//	string getCasterName(void) const { return m_CasterName; }
//	void setCasterName(const string & CasterName ) { m_CasterName = CasterName; }

	ObjectID_t getUserObjectID(void) const { return m_UserObjectID; }
	void setUserObjectID(ObjectID_t UserObjectID ) { m_UserObjectID = UserObjectID; }

	Turn_t getTick() { return m_Tick; }
	void setTick(Turn_t tick) { m_Tick = tick; }

private:
	int     m_Damage;
	Turn_t  m_Delay;
//	string  m_CasterName;
	ObjectID_t   m_UserObjectID;
	bool	m_bPlayer;			// 유저가 사용한 기술인가?
	Turn_t	m_Tick;
};

//////////////////////////////////////////////////////////////////////////////
// class EffectMagmaDetonationLoader
//////////////////////////////////////////////////////////////////////////////

class EffectMagmaDetonationLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_MAGMA_DETONATION1; }
	virtual string getEffectClassName() const throw() { return "EffectMagmaDetonation"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};

extern EffectMagmaDetonationLoader* g_pEffectMagmaDetonationLoader;

#endif // __EFFECT_MAGMA_DETONATION__
