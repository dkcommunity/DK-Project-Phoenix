///////////////////////////////////////////////////////////////////////////
// File : LevelWarItemManager.h
///////////////////////////////////////////////////////////////////////////

#ifndef __HERO_ITEM_MANAGER_H__
#define __HERO_ITEM_MANAGER_H__

// include files
#include "Types.h"
#include "Exception.h"
#include <vector>
#include "item/WarItem.h"

class LevelWarManager;
///////////////////////////////////////////////////////////////////////////
// class LevelWarItemManager
///////////////////////////////////////////////////////////////////////////
class LevelWarItemManager
{
public:
	typedef vector<WarItem*>			VectorWarItem;
	typedef vector<ZONE_COORD>			VectorZoneCoord;

public:
	// constructor & desctructor
	LevelWarItemManager();
	~LevelWarItemManager();

public:
	void init(int level, LevelWarManager* pLevelWarManager) throw(Error);
	void initDefaultPositions() throw(Error);
	void createAllHeroItems() throw(Error);

	void addAllHeroItemsToZone() throw(Error);

	void removeAllHeroItems() throw(Error);

	void warpToDefaultPosition( Creature* pCreature ) throw(Error);

private:
	LevelWarManager*	m_pLeveWarManager;
	int					m_Level;
	VectorWarItem		m_SlayerHeroItems;
	VectorWarItem		m_VampireHeroItems;
	VectorWarItem		m_OustersHeroItems;
	VectorZoneCoord		m_SlayerDefaultPositions;
	VectorZoneCoord		m_VampireDefaultPositions;
	VectorZoneCoord		m_OustersDefaultPositions;
};

//extern LevelWarItemManager* g_pLevelWarItemManager;

#endif

