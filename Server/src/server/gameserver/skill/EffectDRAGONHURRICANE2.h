//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectDRAGONHURRICANE2.h
// Written by  : elca@ewestsoft.com
// Description : ŒºÁ÷ž¶¹ý DragonTornadoÀÇ EffectžŠ Ã³ž®ÇØÁÖ±â À§ÇÑ Å¬·¡œºÀÌŽÙ.
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_DRAGON_HURRICANE2__
#define __EFFECT_DRAGON_HURRICANE2__

#include "Effect.h"
#include "EffectLoader.h"
#include "Creature.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectDragonTornado
//////////////////////////////////////////////////////////////////////////////

class EffectDRAGONHURRICANE2 : public Effect 
{
public:
	EffectDRAGONHURRICANE2(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_DRAGON_HURRICANE2; }
    EffectClass getSendEffectClass() const throw() { return (m_bCanSplit)?EFFECT_CLASS_DRAGON_HURRICANE2:EFFECT_CLASS_DRAGON_HURRICANE2; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

public:
	ObjectID_t getUserOID(void) const { return m_UserOID; }
	void setUserOID(ObjectID_t UserOID) { m_UserOID = UserOID; }

	ObjectID_t getTargetOID(void) const { return m_TargetOID; }
	void setTargetOID(ObjectID_t TargetOID) { m_TargetOID = TargetOID; }

	Damage_t	getDamage() const { return m_Damage; }
	void		setDamage(Damage_t damage) { m_Damage = damage; }

	Damage_t	getChildDamage() const { return m_ChildDamage; }
	void		setChildDamage(Damage_t damage) { m_ChildDamage = damage; }

	bool		canSplit() const { return m_bCanSplit; }
	void		setSplit(bool split) { m_bCanSplit = split; }

private:
	ObjectID_t  m_UserOID;
	ObjectID_t	m_TargetOID;
	Damage_t	m_Damage;
	Damage_t	m_ChildDamage;
	bool		m_bCanSplit;
};

#endif // __EFFECT_DRAGON_DRAGONHURRICANE2__
