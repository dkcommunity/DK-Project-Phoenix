///////////////////////////////////////////////////////////////////////////
// File : HeroItemManager.h
///////////////////////////////////////////////////////////////////////////

#ifndef __HERO_ITEM_MANAGER_H__
#define __HERO_ITEM_MANAGER_H__

// include files
#include "Types.h"
#include "Exception.h"
#include <vector>
#include "item/WarItem.h"

///////////////////////////////////////////////////////////////////////////
// class HeroItemManager
///////////////////////////////////////////////////////////////////////////
class HeroItemManager
{
public:
	typedef vector<WarItem*>			VectorWarItem;
	typedef vector<ZONE_COORD>			VectorZoneCoord;

public:
	// constructor & desctructor
	HeroItemManager();
	~HeroItemManager();

public:
	void init() throw(Error);
	void initDefaultPositions() throw(Error);
	void createAllHeroItems() throw(Error);

	void addAllHeroItemsToZone() throw(Error);

	void removeAllHeroItems() throw(Error);

	void warpToDefaultPosition( Creature* pCreature ) throw(Error);

private:
	VectorWarItem		m_SlayerHeroItems;
	VectorWarItem		m_VampireHeroItems;
	VectorWarItem		m_OustersHeroItems;
	VectorZoneCoord		m_SlayerDefaultPositions;
	VectorZoneCoord		m_VampireDefaultPositions;
	VectorZoneCoord		m_OustersDefaultPositions;
};

extern HeroItemManager* g_pHeroItemManager;

#endif

