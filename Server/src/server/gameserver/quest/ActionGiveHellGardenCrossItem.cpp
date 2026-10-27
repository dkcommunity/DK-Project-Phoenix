////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionGiveHellGardenCrossItem.cpp
// Written By  : 
// Description :
// 
// History.
//
//  Date        Writer         Description
// ---------- ----------- ------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////////

#include "ActionGiveHellGardenCrossItem.h"
#include "PlayerCreature.h"
#include "GamePlayer.h"
#include "Item.h"
#include "ItemUtil.h"
#include "Inventory.h"
#include "Zone.h"
#include "ItemFactoryManager.h"
#include "DB.h"
#include "Thread.h"
#include "Treasure.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "MonsterInfo.h"
#include "QuestItem.h"
#include "ItemUtil.h"
#include "StringPool.h"
#include "ItemMineInfo.h"
#include "PacketUtil.h"
#include "ItemGradeManager.h"
#include "ItemInfoManager.h"
#include "FlagSet.h"
#include <list>

#include "Gpackets/GCCreateItem.h"
#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCDeleteInventoryItem.h"

#include "SystemAvailabilitiesManager.h"

const int NeedCrossNum = 4;

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionGiveHellGardenCrossItem::read(PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

 	try
	{
		m_Type = (ItemType_t)propertyBuffer.getPropertyInt("QuestItem");
	}
	catch (NoSuchElementException & nsee)
	{
		throw Error(nsee.toString());
	}

   __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// 액션을 실행한다.
////////////////////////////////////////////////////////////////////////////////
void ActionGiveHellGardenCrossItem::execute(Creature * pCreature1 , Creature * pCreature2)
{
	__BEGIN_TRY

	Assert(pCreature1 != NULL);
	Assert(pCreature2 != NULL);
	Assert(pCreature1->isNPC());
	Assert(pCreature2->isPC());
	
	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);
	Assert(pPC != NULL);

	Player* pPlayer = pPC->getPlayer();
	Assert(pPlayer != NULL);

	GCNPCResponse okpkt;
	pPlayer->sendPacket(&okpkt);
	StringStream message;

	Zone* pZone = pPC->getZone();
	Assert( pZone != NULL );

	/*

	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setMessage("아직 지원되지 않는 기능입니다");
	pPlayer->sendPacket(&gcSystemMessage);
	*/


	//cout << "ActionGiveHellGardenCrossItem" << ":" << m_Type;

	Inventory* pInventory = pPC->getInventory();

	ItemType_t itemType = 0;
	MonsterType_t MonsterType = 0;
	Item* pItem1 = NULL;

	if( m_Type == 0 )
	{
		itemType = 11;
		MonsterType = 873;
	}
	else if( m_Type == 1 )
	{
		itemType = 12;
		MonsterType = 876;
	}
	else if( m_Type == 2 )
	{
		itemType = 13;
		MonsterType = 879;
	}
	else if( m_Type == 3 )
	{
		itemType = 14;
		MonsterType = 890;
	}
	else if( m_Type == 4 )
	{
		itemType = 15;
		MonsterType = 892;
	}
	else
	{
		GCNPCResponse response;
		response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		return;
	}

	ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo( Item::ITEM_CLASS_MOON_CARD, itemType );
	Assert( pItemInfo != NULL );

	if( pPC->getItemClassTypeNum( Item::Item::ITEM_CLASS_MOON_CARD, itemType ) >= NeedCrossNum )
	{
		const MonsterInfo* pMonsterInfo = g_pMonsterInfoManager->getMonsterInfo( MonsterType );
		TreasureList *pTreasureList = NULL;

		if( pCreature2->isSlayer() )
			pTreasureList = pMonsterInfo->getSlayerTreasureList();
		else if( pCreature2->isVampire() )
			pTreasureList = pMonsterInfo->getVampireTreasureList();
		else if( pCreature2->isOusters() )
			pTreasureList = pMonsterInfo->getOustersTreasureList();

		const list<Treasure*>& treasures = pTreasureList->getTreasures();

		list<Treasure*>::const_iterator itr = treasures.begin();

		ITEM_TEMPLATE it;

		for(; itr != treasures.end(); itr++)
		{
			Treasure* pTreasure = (*itr);

			it.ItemClass = Item::ITEM_CLASS_MAX;
			it.ItemType = 0;

//			it.NextOptionRatio = pItemInfo->getBonusRatio();
			if( pTreasure->getRandomItem(&it, true) )
			{
				pItem1 = g_pItemFactoryManager->createItem(it.ItemClass, it.ItemType, it.OptionType);
				Assert(pItem1 != NULL);
			}
		}

		if( pItem1 == NULL )
		{
			StringStream msg;
			msg << "ActionHellGardenItem: "
				<< (int)it.ItemClass << ", "
				<< (int)it.ItemType << ", "
				<< (int)it.bCreateOption << ", "
				<< getOptionTypeToString(it.OptionType);

			filelog("HellGardenItemBug.txt", "%s", msg.toString().c_str());

			GCSystemMessage gcSystemMessage;
			gcSystemMessage.setMessage( g_pStringPool->getString( STRID_ITEM_CREATE_ERROR ) );
			pPlayer->sendPacket(&gcSystemMessage);


			GCNPCResponse response;
			response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
			pPlayer->sendPacket(&response);
			return;
		}

		TPOINT pt;
		
		// 20080707	
		ItemInfo* pItemInfo1 = g_pItemInfoManager->getItemInfo(pItem1->getItemClass(), pItem1->getItemType());
		Assert( pItemInfo1 != NULL );
		if( !pInventory->getEmptySlot(pItemInfo1->getVolumeWidth(), pItemInfo1->getVolumeHeight(), pt) )
		{
			SAFE_DELETE(pItem1);
			list<OptionType_t> options;
			pItem1 = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_ETC, 14, options ); // 20080707 꽝아이템 노란사탕
			Assert( pItem1 != NULL );
		}

		pItem1->setGrade( min(7, ItemGradeManager::Instance().getRandomBeadGrade()) );
		pZone->registerObject(pItem1);


		if( pInventory->addItem(pItem1, pt) )
		{
			pItem1->create(pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y);

			GCCreateItem gcCreateItem;

			makeGCCreateItem( &gcCreateItem, pItem1, pt.x, pt.y );

			pPlayer->sendPacket(&gcCreateItem);

			if( pItem1 != NULL && pItem1->isTraceItem() )
			{
				remainTraceLog( pItem1, pCreature1->getName(), pCreature2->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
			}

			pPC->decreaseItemClassTypeNum( Item::ITEM_CLASS_MOON_CARD, itemType, NeedCrossNum );

			GCSystemMessage gcSystemMessage;
			gcSystemMessage.setMessage( g_pStringPool->getString( STRID_TRADE_SUCCESS ) );
			pPlayer->sendPacket(&gcSystemMessage);
			
			FlagSet* pFlagSet = pPC->getFlagSet();
			pFlagSet->turnOn( FLAGSET_HELLGARDEN_CLEAR_1 + m_Type );
			pPC->getFlagSet()->save(pPC->getName());
		}
		else
		{
			GCSystemMessage gcSystemMessage;
			gcSystemMessage.setMessage( g_pStringPool->getString( STRID_NOT_ENOUGH_INVENTORY_SPACE ) );
			pPlayer->sendPacket(&gcSystemMessage);
		}
	}
	else
	{
		GCNPCResponse response;
		response.setCode( NPC_RESPONSE_NOT_ENOUGH_ITEM );
		pPlayer->sendPacket(&response);
	}

	GCNPCResponse response;
	response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );

	pPlayer->sendPacket(&response);
	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionGiveHellGardenCrossItem::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionGiveHellGardenCrossItem("
	    << ")";
	return msg.toString();

	__END_CATCH
}
