////////////////////////////////////////////////////////////////////////////////
// Filename    : MenegrothDoungeonManager.h 
// Written By  : rappi76 
// Description :
////////////////////////////////////////////////////////////////////////////////

#ifndef _MENEGROTH_DOUNGEONE_MANAGER_H_
#define _MENEGROTH_DOUNGEONE_MANAGER_H_

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__

#include "Creature.h"


typedef vector<ZONE_COORD> TTrapPosList;

class MenegrothDoungeonManager
{
public :
	class RequiredItem
	{
	public :
		RequiredItem()
		{
			m_ItemClass = Item::ITEM_CLASS_MAX;
			m_ItemType = 0;
			m_iItemCount = 0;
		}

		Item::ItemClass m_ItemClass;
		ItemType_t m_ItemType;
		int m_iItemCount;
	};

	typedef vector<MenegrothDoungeonManager::RequiredItem> TRequiredItems;

	bool hasItems(Creature *pCreature, TRequiredItems& requiredItems);
	void takeItems(Creature *pCreature, TRequiredItems& requiredItems);

	void getRequiredEntranceItems(Creature *pCreature, TRequiredItems& requiredItems);
	bool hasRequiredEntranceItems(Creature *pCreature);
	void takeRequiredEntranceItems(Creature *pCreature);

	void getRequiredCabracamHeart(Zone *pZone, TRequiredItems& requiredItems);
	bool hasRequiredCabracamHeart(Creature *pCreature);
	void takeRequiredCabracamHeart(Creature *pCreature);

	bool getAltarPosToMove(Creature *pCreature, ZoneID_t& ZoneIDToMove, ZoneCoord_t& XPosToMove, ZoneCoord_t& YPosToMove);
	bool getDoungeonSafePos(ZoneID_t ZoneID, ZoneCoord_t& XPos, ZoneCoord_t& YPos);
	bool getDoungeonZoneToMove(Creature *pCreature, ZoneID_t& ZoneIDToMove, ZoneCoord_t& XPosToMove, ZoneCoord_t& YPosToMove);

	int getFloor(ZoneID_t ZoneID);
	int getFloorByLevel(Creature *pCreature);
	bool getBossMonster(ZoneID_t ZoneID, MonsterType_t& MonsterType, ZoneCoord_t& XPos, ZoneCoord_t& YPos);
	bool isValidLevel(Creature *pCreature);
	bool isIncludedZone(ZoneID_t ZoneID);

	bool getRewardMenegrothVoucher(Creature *pCreature, Item::ItemClass& ItemClass, ItemType_t& ItemType);

	void getTrap1Pos(Zone *pZone, TTrapPosList& TrapPosList);
	void getTrap2Pos(Zone *pZone, TTrapPosList& TrapPosList);

	void getTrap1Damage(Zone *pZone, int& iMinDamage, int& iMaxDamage);
	void getTrap2Damage(Zone *pZone, int& iMinDamage, int& iMaxDamage);

	void sendRemainMenegrothTime(Creature *pCreature);

protected :
	Level_t getPlayerLevel(Creature *pCreature);

};

extern MenegrothDoungeonManager g_MenegrothDoungeonManager;


#endif /*  __MENEGROTH_DOUNGEON_SYSTEM__ */

#endif /* _MENEGROTH_DOUNGEONE_MANAGER_H_ */
