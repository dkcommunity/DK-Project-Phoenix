////////////////////////////////////////////////////////////////////////////////
// Filename    : MenegrothDoungeonManager.cpp
// Written By  : rappi76 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "Types.h"

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__

#include "Creature.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"

#include "StringPool.h"
#include "Player.h"
#include "MenegrothDoungeonManager.h"
#include "EffectMenegrothDoungeon.h"

#include "Gpackets/GCSystemMessage.h"


MenegrothDoungeonManager g_MenegrothDoungeonManager;

Level_t MenegrothDoungeonManager::getPlayerLevel(Creature *pCreature)
{
	Level_t playerLevel = 0;

    if (pCreature->isSlayer() )
    {
       	Slayer *pSlayer = dynamic_cast<Slayer *>(pCreature);

       	playerLevel = pSlayer->getHighestSkillDomainLevel();
	}
	else if ( pCreature->isVampire() )
	{
       	Vampire *pVampire = dynamic_cast<Vampire *>(pCreature);

       	playerLevel = pVampire->getLevel();

	}
   	else if ( pCreature->isOusters() )
    {
		Ousters *pOusters = dynamic_cast<Ousters *>(pCreature);

		playerLevel = pOusters->getLevel();
    }

	return playerLevel;
}

bool MenegrothDoungeonManager::isValidLevel(Creature *pCreature)
{
	if ( getFloorByLevel(pCreature) == 0 )
	{
		// 레벨에 해당하는 층이 없음
		return false;
	}

	return true;
}

void MenegrothDoungeonManager::getRequiredEntranceItems(Creature *pCreature, TRequiredItems& requiredItems)
{
	if ( !isValidLevel(pCreature))
	{
		return;
	}

	Level_t playerLevel = getPlayerLevel(pCreature);
	int iFloorToMove;

	iFloorToMove = getFloorByLevel(pCreature);

	RequiredItem item;

	switch(iFloorToMove)
	{
		case 1 :
			item.m_ItemClass = Item::ITEM_CLASS_COMMON_QUEST_ITEM;
			item.m_ItemType = 6;
			item.m_iItemCount = 5;

			requiredItems.push_back(item);
			
			break;
		case 2 :
			item.m_ItemClass = Item::ITEM_CLASS_COMMON_QUEST_ITEM;
			item.m_ItemType = 7;
			item.m_iItemCount = 5;
		
			requiredItems.push_back(item);

			break;
		case 3 :
			item.m_ItemClass = Item::ITEM_CLASS_COMMON_QUEST_ITEM;
			item.m_ItemType = 8;
			item.m_iItemCount = 5;
		
			requiredItems.push_back(item);

			break;
		case 4 :
			item.m_ItemClass = Item::ITEM_CLASS_COMMON_QUEST_ITEM;
			item.m_ItemType = 9;
			item.m_iItemCount = 5;
		
			requiredItems.push_back(item);

			break;
		case 5 : 
			item.m_ItemClass = Item::ITEM_CLASS_COMMON_QUEST_ITEM;
			item.m_ItemType = 10;
			item.m_iItemCount = 5;
		
			requiredItems.push_back(item);

			break;
		case 6 :
			item.m_ItemClass = Item::ITEM_CLASS_COMMON_QUEST_ITEM;
			item.m_ItemType = 11;
			item.m_iItemCount = 5;
		
			requiredItems.push_back(item);

			break;

		default :

			break;
    }
}

bool MenegrothDoungeonManager::hasRequiredEntranceItems(Creature *pCreature)
{	
	TRequiredItems requiredItems;

	getRequiredEntranceItems(pCreature, requiredItems);

	return hasItems(pCreature, requiredItems);
}

bool MenegrothDoungeonManager::hasItems(Creature *pCreature, TRequiredItems& requiredItems)
{
	PlayerCreature *pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory* pInventory = pPC->getInventory();
	if ( pInventory == NULL )
	{
		return false;
	}

	TRequiredItems::iterator iter;

	iter = requiredItems.begin();
	for( ; iter != requiredItems.end() ; ++iter)
	{
		RequiredItem item = (*iter);

		if ( !pInventory->hasEnoughNumItem(item.m_ItemClass, item.m_ItemType, item.m_iItemCount) )
		{
			return false;
		}
	}

	return true;
}

void MenegrothDoungeonManager::takeItems(Creature *pCreature, TRequiredItems& requiredItems)
{
	PlayerCreature *pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory* pInventory = pPC->getInventory();
	if ( pInventory == NULL )
	{
		return;
	}

	TRequiredItems::iterator iter;

	iter = requiredItems.begin();
	for( ; iter != requiredItems.end() ; ++iter)
	{
		RequiredItem item = (*iter);

		pInventory->decreaseNumItem(item.m_ItemClass, item.m_ItemType, item.m_iItemCount, pCreature->getPlayer()); 
	}
}

void MenegrothDoungeonManager::takeRequiredEntranceItems(Creature *pCreature)
{
	TRequiredItems requiredItems;

	getRequiredEntranceItems(pCreature, requiredItems);
	takeItems(pCreature, requiredItems);
}

bool MenegrothDoungeonManager::getAltarPosToMove(Creature *pCreature, ZoneID_t& ZoneIDToMove, ZoneCoord_t& XPosToMove, ZoneCoord_t& YPosToMove)
{
	if ( pCreature->isSlayer() )
    {
        ZoneIDToMove = 12;
        XPosToMove = 47;
        YPosToMove = 182;
    }
	else if ( pCreature->isVampire() )
	{
		ZoneIDToMove = 22;
	    XPosToMove = 217;
	    YPosToMove = 230;
    }
    else if ( pCreature->isOusters() )
    {
        ZoneIDToMove = 81;
        XPosToMove = 191;
        YPosToMove = 111;
    }
	else
	{
		return false;
	}

	return true;
}

bool MenegrothDoungeonManager::getDoungeonSafePos(ZoneID_t ZoneID, ZoneCoord_t& XPos, ZoneCoord_t& YPos)
{
	switch(ZoneID)
	{
		case 1701 : XPos = 62, YPos = 19;	break;
		case 1702 : XPos = 23, YPos = 58;	break;
		case 1703 : XPos = 36, YPos = 36;	break;
		case 1704 : XPos = 58, YPos = 22;	break;
		case 1705 : XPos = 36, YPos = 36;	break;
		case 1706 : XPos = 59, YPos = 21;	break;

		case 1707 : XPos = 62, YPos = 19;	break;
		case 1708 : XPos = 23, YPos = 58;	break;
		case 1709 : XPos = 36, YPos = 36;	break;
		case 1710 : XPos = 58, YPos = 22;	break;
		case 1711 : XPos = 36, YPos = 36;	break;
		case 1712 : XPos = 59, YPos = 21;	break;

		case 1713 : XPos = 62, YPos = 19;	break;
		case 1714 : XPos = 23, YPos = 58;	break;
		case 1715 : XPos = 36, YPos = 36;	break;
		case 1716 : XPos = 58, YPos = 22;	break;
		case 1717 : XPos = 36, YPos = 36;	break;
		case 1718 : XPos = 59, YPos = 21;	break;

		default :
			return false;
	}

	return true;

}

bool MenegrothDoungeonManager::getDoungeonZoneToMove(Creature *pCreature, ZoneID_t& ZoneIDToMove, ZoneCoord_t& XPosToMove, ZoneCoord_t& YPosToMove)
{
	if ( !isValidLevel(pCreature) )
	{
		return false;
	}

	int iFloorToMove;

	iFloorToMove = getFloorByLevel(pCreature);

	if ( pCreature->isSlayer() )
	{
		switch(iFloorToMove)
		{
			case 1 : ZoneIDToMove = 1701; 	return getDoungeonSafePos(ZoneIDToMove, XPosToMove, YPosToMove);
			case 2 : ZoneIDToMove = 1702; 	return getDoungeonSafePos(ZoneIDToMove, XPosToMove, YPosToMove);
			case 3 : ZoneIDToMove = 1703; 	return getDoungeonSafePos(ZoneIDToMove, XPosToMove, YPosToMove);
			case 4 : ZoneIDToMove = 1704; 	return getDoungeonSafePos(ZoneIDToMove, XPosToMove, YPosToMove);
			case 5 : ZoneIDToMove = 1705; 	return getDoungeonSafePos(ZoneIDToMove, XPosToMove, YPosToMove);
			case 6 : ZoneIDToMove = 1706; 	return getDoungeonSafePos(ZoneIDToMove, XPosToMove, YPosToMove);
			default :
				return false;
		}
	}
	else if ( pCreature->isVampire() )
	{
		switch(iFloorToMove)
		{
			case 1 : ZoneIDToMove = 1707; 	return getDoungeonSafePos(ZoneIDToMove, XPosToMove, YPosToMove); 
			case 2 : ZoneIDToMove = 1708; 	return getDoungeonSafePos(ZoneIDToMove, XPosToMove, YPosToMove);
			case 3 : ZoneIDToMove = 1709; 	return getDoungeonSafePos(ZoneIDToMove, XPosToMove, YPosToMove);
			case 4 : ZoneIDToMove = 1710; 	return getDoungeonSafePos(ZoneIDToMove, XPosToMove, YPosToMove);
			case 5 : ZoneIDToMove = 1711; 	return getDoungeonSafePos(ZoneIDToMove, XPosToMove, YPosToMove);
			case 6 : ZoneIDToMove = 1712; 	return getDoungeonSafePos(ZoneIDToMove, XPosToMove, YPosToMove);
			default :
				return false;
		}				
	}
	else if ( pCreature->isOusters() )
	{
		switch(iFloorToMove)
		{
			case 1 : ZoneIDToMove = 1713; 	return getDoungeonSafePos(ZoneIDToMove, XPosToMove, YPosToMove);
			case 2 : ZoneIDToMove = 1714; 	return getDoungeonSafePos(ZoneIDToMove, XPosToMove, YPosToMove);
			case 3 : ZoneIDToMove = 1715; 	return getDoungeonSafePos(ZoneIDToMove, XPosToMove, YPosToMove);
			case 4 : ZoneIDToMove = 1716; 	return getDoungeonSafePos(ZoneIDToMove, XPosToMove, YPosToMove);
			case 5 : ZoneIDToMove = 1717; 	return getDoungeonSafePos(ZoneIDToMove, XPosToMove, YPosToMove);
			case 6 : ZoneIDToMove = 1718; 	return getDoungeonSafePos(ZoneIDToMove, XPosToMove, YPosToMove);
			default :
				return false;
		}				
	}

	return true;
}

int MenegrothDoungeonManager::getFloorByLevel(Creature *pCreature)
{

	Level_t playerLevel = getPlayerLevel(pCreature);

    if ( playerLevel >= 21 && playerLevel <= 40)
	{
		return 1;
	}
	else if ( playerLevel >= 41 && playerLevel <= 60)
	{
		return 2;
	}
	else if ( playerLevel >= 61 && playerLevel <= 80)
	{
		return 3;
	}
	else if ( playerLevel >= 81 && playerLevel <= 100)
	{
		return 4;
	}
	else if ( playerLevel >= 101 && playerLevel <= 120)
	{
		return 5;
	}
	else if ( playerLevel >= 121 )
	{
		return 6;
	}

	return 0;	// 해당하는 층이 없음

}

int MenegrothDoungeonManager::getFloor(ZoneID_t ZoneID)
{
	switch(ZoneID)
	{
		// 1F
		case 1701 :
		case 1707 :
		case 1713 :
			return 1;
		// 2F
		case 1702 :
		case 1708 :
		case 1714 :
			return 2;
		// 3F
		case 1703 :
		case 1709 :
		case 1715 :
			return 3;
		// 4F
		case 1704 :
		case 1710 :
		case 1716 :
			return 4;
		// 5F
		case 1705 :
		case 1711 :
		case 1717 :
			return 5;
		// 6F
		case 1706 :
		case 1712 :
		case 1718 :
			return 6;
		default :
			break;
	}

	return 0;
}

bool MenegrothDoungeonManager::getBossMonster(ZoneID_t ZoneID, MonsterType_t& MonsterType, ZoneCoord_t& XPos, ZoneCoord_t& YPos)
{
	switch(getFloor(ZoneID))
	{
		// 1F Boss Monster
		case 1 :
			MonsterType = 1021;
			XPos = 64;
			YPos = 63;
			break;
		// 2F Boss Monster
		case 2 :
			MonsterType = 1022;
			XPos = 46;
			YPos = 81;
			break;
		// 3F Boss Monster
		case 3 :
			MonsterType = 1023;
			XPos = 63;
			YPos = 64;
			break;
		// 4F Boss Monster
		case 4 :
			MonsterType = 1024;
			XPos = 82;
			YPos = 45;
			break;
		// 5F Boss Monster
		case 5 :
			MonsterType = 1025;
			XPos = 64;
			YPos = 63;
			break;
		// 6F Boss Monster
		case 6 :
			MonsterType = 1026;
			XPos = 64;
			YPos = 99;
			break;
		default :
			return false;
	}

	return true;
}

void MenegrothDoungeonManager::getRequiredCabracamHeart(Zone *pZone, TRequiredItems& requiredItems)
{
	ZoneID_t ZoneID;

	ZoneID = pZone->getZoneID();

	RequiredItem item;

	item.m_ItemClass = Item::ITEM_CLASS_COMMON_QUEST_ITEM;
	item.m_iItemCount = 1;

	switch(getFloor(ZoneID))
	{
		case 1 :	// 1F
			item.m_ItemType = 12;
			break;
		case 2 :	// 2F
			item.m_ItemType = 13;
			break;
		case 3 :	// 3F
			item.m_ItemType = 14;
			break;
		case 4 :	// 4F
			item.m_ItemType = 15;
			break;
		case 5 :	// 5F
			item.m_ItemType = 16;
			break;
		case 6 :	// 6F
			item.m_ItemType = 17;
			break;

		default :
			return;
	}

	requiredItems.push_back(item);

	return;

}

bool MenegrothDoungeonManager::hasRequiredCabracamHeart(Creature *pCreature)
{
	PlayerCreature *pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Inventory* pInventory = pPC->getInventory();
	if ( pInventory == NULL )
	{
		return false;
	}

	Zone *pZone;

	pZone = pPC->getZone();

	TRequiredItems requiredItems;
	TRequiredItems::iterator iter;

	getRequiredCabracamHeart(pZone, requiredItems);

	iter = requiredItems.begin();
	for( ; iter != requiredItems.end() ; ++iter)
	{
		RequiredItem item = (*iter);

		if ( !pInventory->hasEnoughNumItem(item.m_ItemClass, item.m_ItemType, item.m_iItemCount) )
		{
			return false;
		}
	}

	return true;

}

void MenegrothDoungeonManager::takeRequiredCabracamHeart(Creature *pCreature)
{
	TRequiredItems requiredItems;

	getRequiredCabracamHeart(pCreature->getZone(), requiredItems);
	takeItems(pCreature, requiredItems);
}

bool MenegrothDoungeonManager::isIncludedZone(ZoneID_t ZoneID)
{
	if ( ZoneID >= 1701 && ZoneID <= 1718 )
	{
		return true;
	}

	return false;
}

bool MenegrothDoungeonManager::getRewardMenegrothVoucher(Creature *pCreature, Item::ItemClass& ItemClass, ItemType_t& ItemType)
{
	Zone *pZone = pCreature->getZone();
	if ( pZone == NULL )
	{
		return false;
	}

	ItemClass = Item::ITEM_CLASS_COMMON_QUEST_ITEM;

	if ( pCreature->isSlayer() )
	{
		switch(getFloor(pZone->getZoneID()))
		{
			case 1 :
				ItemType = 18;
				break;
			case 2 :
				ItemType = 21;
				break;
			case 3 :
				ItemType = 24;
				break;
			case 4 :
			case 5 :
			case 6 :
				ItemType = 27;
				break;
			default :
				return false;
		}

		return true;
	}
	else if ( pCreature->isVampire() )
	{
		switch(getFloor(pZone->getZoneID()))
		{
			case 1 :
				ItemType = 19;
				break;
			case 2 :
				ItemType = 22;
				break;
			case 3 :
				ItemType = 25;
				break;
			case 4 :
			case 5 :
			case 6 :
				ItemType = 28;
				break;
			default :
				return false;
		}

		return true;
	}
	else if ( pCreature->isOusters() )
	{
		switch(getFloor(pZone->getZoneID()))
		{
			case 1 :
				ItemType = 20;
				break;
			case 2 :
				ItemType = 23;
				break;
			case 3 :
				ItemType = 26;
				break;
			case 5 :
			case 4 :
			case 6 :
				ItemType = 29;
				break;
			default :
				return false;
		}

		return true;
	}

	return false;

}

void MenegrothDoungeonManager::getTrap1Damage(Zone *pZone, int& iMinDamage, int& iMaxDamage)
{

	switch(getFloor(pZone->getZoneID()))
	{
		case 1 :
			iMinDamage = 25;
			iMaxDamage = 50;
			break;
		case 2 :
			iMinDamage = 35;
			iMaxDamage = 70;
			break;
		case 3 :
			iMinDamage = 45;
			iMaxDamage = 90;
			break;
		case 4 :
			iMinDamage = 55;
			iMaxDamage = 110;
			break;
		case 5 :
			iMinDamage = 65;
			iMaxDamage = 130;
			break;
		case 6 :
			iMinDamage = 75;
			iMaxDamage = 150;
			break;

	}
}
void MenegrothDoungeonManager::getTrap2Damage(Zone *pZone, int& iMinDamage, int& iMaxDamage)
{

	switch(getFloor(pZone->getZoneID()))
	{
		case 1 :
			iMinDamage = 40;
			iMaxDamage = 70;
			break;
		case 2 :
			iMinDamage = 50;
			iMaxDamage = 90;
			break;
		case 3 :
			iMinDamage = 60;
			iMaxDamage = 110;
			break;
		case 4 :
			iMinDamage = 70;
			iMaxDamage = 130;
			break;
		case 5 :
			iMinDamage = 80;
			iMaxDamage = 150;
			break;
		case 6 :
			iMinDamage = 90;
			iMaxDamage = 170;
			break;
	}
}

void MenegrothDoungeonManager::getTrap1Pos(Zone *pZone, TTrapPosList& TrapPosList)
{
	// 종족별 동일 층은 동일 맵을 사용하므로 위치가 층별로 같다.

	Statement* pStmt;
	Result* pResult;

	BEGIN_DB
	{
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery("SELECT TrapX, TrapY FROM MenegrothDoungeonTrapInfo WHERE TrapType='1' AND ZoneID=%d", pZone->getZoneID());

		ZONE_COORD TrapPos;
		ZoneCoord_t TrapX, TrapY;

		while(pResult->next())
		{
			TrapX = pResult->getInt(1);
			TrapY = pResult->getInt(2);

			TrapPos.set(pZone->getZoneID(), TrapX, TrapY);
			TrapPosList.push_back(TrapPos);
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
}

void MenegrothDoungeonManager::getTrap2Pos(Zone *pZone, TTrapPosList& TrapPosList)
{
	// 종족별 동일 층은 동일 맵을 사용하므로 위치가 층별로 같다.

	Statement* pStmt;
	Result* pResult;

	BEGIN_DB
	{
		pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery("SELECT TrapX, TrapY FROM MenegrothDoungeonTrapInfo WHERE TrapType='2' AND ZoneID=%d", pZone->getZoneID());

		ZONE_COORD TrapPos;
		ZoneCoord_t TrapX, TrapY;

		while(pResult->next())
		{
			TrapX = pResult->getInt(1);
			TrapY = pResult->getInt(2);

			TrapPos.set(pZone->getZoneID(), TrapX, TrapY);
			TrapPosList.push_back(TrapPos);
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
}

void MenegrothDoungeonManager::sendRemainMenegrothTime(Creature *pCreature)
{
	if ( !pCreature->isFlag(Effect::EFFECT_CLASS_MENEGROTH_DOUNGEON) )
	{
		return;
	}

	EffectMenegrothDoungeon *pEffect = dynamic_cast<EffectMenegrothDoungeon *>(pCreature->findEffect(Effect::EFFECT_CLASS_MENEGROTH_DOUNGEON));

	if ( pEffect == NULL )
	{
		return;
	}

	Timeval currentTime;
	getCurrentTime(currentTime);
	Timeval remainTime = timediff( pEffect->getDeadline(), currentTime );
	Turn_t remainTurn = remainTime.tv_sec * 10 + remainTime.tv_usec / 100000;

	char message[512];
	sprintf(message, g_pStringPool->c_str(STRID_MENEGROTH_DOUNGEON_LIMITED_TIMER), remainTurn);

	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setType( SYSTEM_MESSAGE_TIMER );
	gcSystemMessage.setMessage(message);

	pCreature->getPlayer()->sendPacket(&gcSystemMessage);

}

#endif /* __MENEGROTH_DOUNGEON_SYSTEM__ */
