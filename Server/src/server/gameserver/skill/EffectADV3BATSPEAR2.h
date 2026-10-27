//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBATSPEAR2.h
// Written by  : ksym555
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_BATSPEAR2__
#define __EFFECT_BATSPEAR2__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectBATSPEAR2
//////////////////////////////////////////////////////////////////////////////

class EffectBATSPEAR2 : public Effect 
{
public:
	EffectBATSPEAR2(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY, bool bPlayer = false) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_BATSPEAR2; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

public:
	int getDamage(void) const { return m_Damage; }
	void setDamage(int damage) { m_Damage = damage; }
//	int geteffectDamage(void) const { return m_Damage2; }
//	void seteffectDamage(int damage2) { m_Damage2 = damage2; }

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
//	int     m_Damage2;
	Turn_t  m_Delay;
//	string  m_CasterName;
	ObjectID_t   m_UserObjectID;
	bool	m_bPlayer;			// À¯Àú°¡ »ç¿ëÇÑ ±âŒúÀÎ°¡?
	Turn_t	m_Tick;
};

//////////////////////////////////////////////////////////////////////////////
// class EffectBATSPEAR2Loader
//////////////////////////////////////////////////////////////////////////////

class EffectBATSPEAR2Loader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_BATSPEAR2; }
	virtual string getEffectClassName() const throw() { return "EffectBATSPEAR2"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};

extern EffectBATSPEAR2Loader* g_pEffectBATSPEAR2Loader;

#endif
