//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSharpShield2.h
// Written by  : crazydog
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_SHARP_SHIELD2__
#define __EFFECT_SHARP_SHIELD2__

#include "Effect.h"
#include "EffectLoader.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectSharpShield2
//////////////////////////////////////////////////////////////////////////////

class EffectSharpShield2 : public Effect 
{
public:
	EffectSharpShield2(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_SHARP_SHIELD2; }

	void affect() throw(Error){}
	void affect(Creature* pCreature) throw(Error);
	void affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject) throw(Error);
	void unaffect(Item* pItem) throw(Error) {}
	void unaffect() throw(Error);

	string toString() const throw();

public:
	Damage_t getDamage() const throw() { return m_Damage; }
	void setDamage(Damage_t Damage) throw() { m_Damage = Damage; }


private:
	Damage_t 		m_Damage;				// 때린놈한테 주는 데미지
};

//////////////////////////////////////////////////////////////////////////////
// class EffectSharpShieldLoader
//////////////////////////////////////////////////////////////////////////////

class EffectSharpShield2Loader : public EffectLoader 
{
public:
	virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_SHARP_SHIELD2; }
	virtual string getEffectClassName() const throw() { return "EffectSharpShield2"; }

public:
	virtual void load(Creature* pCreature) throw(Error) {}
};


#endif
