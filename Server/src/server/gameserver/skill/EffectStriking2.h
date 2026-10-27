//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectStriking2.h
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_STRIKING_2__
#define __EFFECT_STRIKING_2__

#include "Effect.h"
#include "EffectLoader.h"
#include "Item.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectStriking2
//////////////////////////////////////////////////////////////////////////////

class EffectStriking2 : public Effect 
{
public:
	EffectStriking2(Creature* pCreature) throw(Error);

public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_STRIKING_2; }

	void affect() throw(Error){}
	void affect(Creature* pCreature) throw(Error);
	void affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);
	void unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject) throw(Error);
    void unaffect() throw(Error);

	string toString() const throw();

public:
	Damage_t getDamageBonus(void) const { return m_DamageBonus; }
	void setDamageBonus(Damage_t bonus) { m_DamageBonus = bonus; }

	Item::ItemClass getItemClass() const { return m_ItemClass; }
	ItemID_t		getItemID() const { return m_ItemID; }
	void	setTargetItem( Item* pItem ) { m_ItemClass = pItem->getItemClass(); m_ItemID = pItem->getItemID(); }
	bool	isTargetItem( Item* pItem ) { return m_ItemClass == pItem->getItemClass() && m_ItemID == pItem->getItemID(); }

private :
	Damage_t   m_DamageBonus;
	Item::ItemClass	m_ItemClass;
	ItemID_t	m_ItemID;
};

#endif // __EFFECT_STRIKING_2__
