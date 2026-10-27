//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectHeterChakramBomb.h
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_HETER_CHAKRAM_BOMB__
#define __EFFECT_HETER_CHAKRAM_BOMB__

#include "Creature.h"
#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectHeterChakramBomb
//////////////////////////////////////////////////////////////////////////////

class EffectHeterChakramBomb : public Effect 
{
public:
	EffectHeterChakramBomb(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_HETER_CHAKRAM_BOMB; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

public:
	HP_t getDamage() const throw() { return m_Damage; }
	void setDamage(Damage_t Damage) throw() { m_Damage = Damage; }

	void setUserObjectID(ObjectID_t oid) throw() { m_UserObjectID = oid; }
	ObjectID_t getUserObjectID() const throw() { return m_UserObjectID; }

	void setTargetCreature(Creature::CreatureClass targetClass) throw() { m_TargetClass = targetClass; }
	Creature::CreatureClass getTargetCreature() throw() { return m_TargetClass; }

	void setTargetSpriteType(SpriteType_t spritetype) { m_SpriteType = spritetype; }

private:
	Damage_t	m_Damage;
	ObjectID_t 	m_UserObjectID;
	Creature::CreatureClass m_TargetClass;
	SpriteType_t m_SpriteType;
};

#endif // __EFFECT_HETER_CHAKRAM_BOMB__
