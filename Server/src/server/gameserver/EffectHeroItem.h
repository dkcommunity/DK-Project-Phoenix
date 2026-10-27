//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectHeroItem.h
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EFFECT_HERO_ITEM__
#define __EFFECT_HERO_ITEM__

#include "EffectHasRelic.h"

//////////////////////////////////////////////////////////////////////////////
// class EffectHeroItem
//////////////////////////////////////////////////////////////////////////////

class EffectHeroItem : public EffectHasRelic 
{
public:
	EffectHeroItem(Creature* pCreature) throw(Error);
	EffectHeroItem(Item* pItem) throw(Error);

public:
	EffectClass getEffectClass() const throw() { return EFFECT_CLASS_HERO_ITEM; }

	void affect(Creature* pCreature) throw(Error);
	void affect(Item* pItem) throw(Error);

	void unaffect(Creature* pCreature) throw(Error);

	string toString() const throw();

	// get/set ItemID
	ItemID_t getItemID() const { return m_ItemID; }
	void setItemID( ItemID_t itemID ) { m_ItemID = itemID; }

private:
	// HeroItem 아이템의 ID. GlobalPostionLoader 를 위한 멤버
	ItemID_t	m_ItemID;
};

#endif 
