//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectHalo.h
// Written by  : elca@ewestsoft.com
// Description : 己流付过 Halo狼 Effect甫 贸府秦林扁 困茄 努贰胶捞促.
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_HALO2__
#define __EFFECT_HALO2__

#include "Effect.h"
#include "EffectLoader.h"
#include "Creature.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectHalo
//////////////////////////////////////////////////////////////////////////////

class EffectHalo2 : public Effect 
{
public:
	EffectHalo2(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_HALO2; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

public:
	ObjectID_t getUserOID(void) const { return m_UserOID; }
	void setUserOID(ObjectID_t UserOID ) { m_UserOID = UserOID; }

	ObjectID_t getTargetOID(void) const { return m_TargetOID; }
	void setTargetOID(ObjectID_t TargetOID ) { m_TargetOID = TargetOID; }

	Damage_t getDamage(void) const { return m_Damage; }
	void setDamage(Damage_t Damage ) { m_Damage = Damage; }

private:
	ObjectID_t  m_UserOID;
	ObjectID_t  m_TargetOID;
	Damage_t	m_Damage;

};

#endif // __EFFECT_HALO__
