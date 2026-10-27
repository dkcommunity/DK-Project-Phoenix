//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectFireBreath.h
// Written by  : elca@ewestsoft.com
// Description : 성직마법 FireBreath의 Effect를 처리해주기 위한 클래스이다.
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_FIRE_BREATH__
#define __EFFECT_FIRE_BREATH__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectFireBreath
//////////////////////////////////////////////////////////////////////////////

class EffectFireBreath : public Effect 
{
public:
	EffectFireBreath(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_FIRE_BREATH; }

	void affect() throw(Error);
	void affect(Creature*) throw(Error) {}

	void unaffect() throw(Error);
	void unaffect(Creature*) throw(Error) {}

	string toString() const throw();

public:
	Turn_t getTick() const { return m_Tick; }
	void setTick(Turn_t tick) { m_Tick = tick; }

	ObjectID_t getUserObjectID(void) const { return m_UserObjectID; }
	void setUserObjectID(ObjectID_t UserObjectID ) { m_UserObjectID = UserObjectID; }

private:
	Turn_t  	m_Tick;
	ObjectID_t	m_UserObjectID;
	bool		m_bFirst;
};

//////////////////////////////////////////////////////////////////////////////
// class EffectFireBreathLoader
//////////////////////////////////////////////////////////////////////////////

class EffectFireBreathLoader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_FIRE_BREATH; }
	virtual string getEffectClassName() const throw() { return "EffectFireBreath"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};

extern EffectFireBreathLoader* g_pEffectFireBreathLoader;

#endif // __EFFECT_FIRE_BREATH__
