//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBleeding.h
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_BLEEDING__
#define __EFFECT_BLEEDING__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectBleeding
//////////////////////////////////////////////////////////////////////////////

class EffectBleeding : public Effect 
{
public:
	EffectBleeding(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_BLEEDING; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);
	void affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);
	void unaffect(Item* pItem) throw(Error) {}
	void unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject) throw(Error);

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

#endif // __EFFECT_BLEEDING__
