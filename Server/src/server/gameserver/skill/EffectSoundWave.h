//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSoundWave.h
// Written by  : crazydog
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_SOUND_WAVE__
#define __EFFECT_SOUND_WAVE__

#include "Effect.h"
#include "EffectLoader.h"
#include "Tile.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectSoundWave
//////////////////////////////////////////////////////////////////////////////

class EffectSoundWave : public Effect 
{
public:
	EffectSoundWave(Zone*, ZoneCoord_t, ZoneCoord_t) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_SOUND_WAVE; }

	void affect() throw(Error);
	void affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject ) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error) {};
	void unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject ) throw(Error);
	void unaffect(Item* pItem) throw(Error) {}

	string toString() const throw();

public:
	Damage_t getDamage() const throw(Error) { return m_Damage; }
	void setDamage(Damage_t d) throw(Error) { m_Damage = d;}

	void setDuration(Duration_t d) throw(Error) { m_Duration = d;}
	Duration_t getDuration() const throw(Error) { return m_Duration;}

	void setUserObjectID(ObjectID_t oid) throw() { m_UserObjectID = oid; }
	ObjectID_t getUserObjectID() const throw() { return m_UserObjectID; }

	void setTick(Turn_t Tick) throw() { m_Tick = Tick; }
	Turn_t getTick() throw() { return m_Tick; }

	bool affectCreature(Creature* pCreature, bool bAffectByMove) throw(Error);

private:
	Damage_t	m_Damage;
	Duration_t	m_Duration;
	ObjectID_t	m_UserObjectID;
	ZoneCoord_t	m_Step;
	Turn_t		m_Tick;
};

class EffectSoundWaveLoader : public EffectLoader
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_SOUND_WAVE; }
	virtual string getEffectClassName() const throw() { return "EffectSoundWave"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
	virtual void load(Zone* pZone) throw(Error);
};

extern EffectSoundWaveLoader* g_pEffectSoundWaveLoader;

#endif // __EFFECT_SOUND_WAVE__
