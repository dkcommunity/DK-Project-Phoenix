//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectHeterChakram.h
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_HETER_CHAKRAM__
#define __EFFECT_HETER_CHAKRAM__

#include "Creature.h"
#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectHeterChakram
//////////////////////////////////////////////////////////////////////////////

class EffectHeterChakram : public Effect 
{
public:
	EffectHeterChakram(Creature* pCreature) throw(Error);
	virtual ~EffectHeterChakram() throw() {}
public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_HETER_CHAKRAM; }

	void affect() throw(Error);
	void affect(Creature* pCreature) throw(Error);

	void unaffect() throw(Error);
	void unaffect(Creature* pCreature) throw(Error);

	Creature*	serchTargetCreature( Zone* pZone, Coord_t X, Coord_t Y ) throw();

	string toString() const throw();

public:
	HP_t getDamage() const throw() { return m_Damage; }
	void setDamage(Damage_t Damage) throw() { m_Damage = Damage; }

	void setUserObjectID(ObjectID_t oid) throw() { m_UserObjectID = oid; }
	ObjectID_t getUserObjectID() const throw() { return m_UserObjectID; }

	BYTE getStep() const throw() { return m_Step; }
	void setStep(BYTE step) throw() { m_Step = step; }

	list<ObjectID_t> getAddEffectCreatureList() throw() { return m_ObjectList; }
	void setAddEffectCreatureList(list<ObjectID_t> objectList);

	void setTargetCreature(Creature::CreatureClass targetClass) throw() { m_TargetClass = targetClass; }
	Creature::CreatureClass getTargetCreature() throw() { return m_TargetClass; }

	void setTargetSpriteType(SpriteType_t spritetype) throw() { m_TargetSpriteType = spritetype; }

private:
	Creature::CreatureClass	m_TargetClass;
	SpriteType_t	m_TargetSpriteType;
	Damage_t	m_Damage;
	ObjectID_t 	m_UserObjectID;
	BYTE		m_Step;
	list<ObjectID_t> m_ObjectList;
};

#endif // __EFFECT_HETER_CHAKRAM__
