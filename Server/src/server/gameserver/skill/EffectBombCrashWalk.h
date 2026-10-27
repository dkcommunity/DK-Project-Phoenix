//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBombCrashWalk.h
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_BOMB_CRASH_WALK__
#define __EFFECT_BOMB_CRASH_WALK__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectBombCrashWalk
//////////////////////////////////////////////////////////////////////////////

class EffectBombCrashWalk : public Effect 
{
public:
	EffectBombCrashWalk(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_BOMB_CRASH_WALK; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

public:
	HP_t getDamage() const throw() { return m_Damage; }
	void setDamage(HP_t Damage) throw() { m_Damage = Damage; }

	void setTick(Turn_t Tick) throw() { m_Tick = Tick; }
	Turn_t getTick() const throw() { return m_Tick; }

	void setUserObjectID(ObjectID_t oid) throw() { m_UserObjectID = oid; }
	ObjectID_t getUserObjectID() const throw() { return m_UserObjectID; }

private:
	HP_t    m_Damage;
	Turn_t  m_Tick;
	ObjectID_t m_UserObjectID;
};

#endif // __EFFECT_BOMB_CRASH_WALK__
