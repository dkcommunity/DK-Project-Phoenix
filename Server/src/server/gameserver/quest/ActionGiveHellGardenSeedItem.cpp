////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionGiveHellGardenSeedItem.cpp
// Written By  : 
// Description :
// 
// History.
//
//  Date        Writer         Description
// ---------- ----------- ------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////////

#include "ActionGiveHellGardenSeedItem.h"
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

const int NeedSeedNum = 90; // 50개에서 90 개로 수정 20070703 by bigheart

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionGiveHellGardenSeedItem::read(PropertyBuffer & propertyBuffer)
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
void ActionGiveHellGardenSeedItem::execute(Creature * pCreature1 , Creature * pCreature2)
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

	FlagSet* pFlagSet = pPC->getFlagSet();

	/*

	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setMessage("아직 지원되지 않는 기능입니다");
	pPlayer->sendPacket(&gcSystemMessage);
	*/


	//cout << "ActionGiveHellGardenSeedItem" << ":" << m_Type;

	Inventory* pInventory = pPC->getInventory();

	ItemClass_t itemClass = 0;
	ItemType_t itemType = 0;
	ItemType_t needItemType = 0;
	MonsterType_t MonsterType = 0;
	Item* pItem1 = NULL;
	bool bGetItem = true;

	list<OptionType_t> options;
	string itemOption;

	if( m_Type == 0 )
	{
		itemClass = Item::ITEM_CLASS_CONTRACT_OF_BLOOD;
		itemType = 1;
		itemOption = "ATTR+2";
		needItemType = 6;
		if( pFlagSet->isOn(FLAGSET_HELLGARDEN_SEED_1) )
			bGetItem = false;
	}
	else if( m_Type == 1 )
	{
		itemClass = Item::ITEM_CLASS_CONTRACT_OF_BLOOD;
		itemType = 2;
		itemOption = "ATTR+2,RES+3";
		needItemType = 7;
		if( pFlagSet->isOn(FLAGSET_HELLGARDEN_SEED_2) )
			bGetItem = false;
	}
	else if( m_Type == 2 )
	{
		itemClass = Item::ITEM_CLASS_CONTRACT_OF_BLOOD;
		itemType = 3;
		itemOption = "ATTR+3,RES+3";
		needItemType = 8;
		if( pFlagSet->isOn(FLAGSET_HELLGARDEN_SEED_3) )
			bGetItem = false;
	}
	else if( m_Type == 3 )
	{
		if( pPC->isSlayer() )
		{
			itemClass = Item::ITEM_CLASS_RING;
		}
		else if( pPC->isVampire() )
		{
			itemClass = Item::ITEM_CLASS_VAMPIRE_RING;
		}
		else if( pPC->isOusters() )
		{
			itemClass = Item::ITEM_CLASS_OUSTERS_RING;
		}

		itemType = 14;
		needItemType = 9;
		if( pFlagSet->isOn(FLAGSET_HELLGARDEN_SEED_4) )
			bGetItem = false;
	}
	else if( m_Type == 4 )
	{
		if( pPC->isSlayer() )
		{
			itemClass = Item::ITEM_CLASS_NECKLACE;
			itemType = 14;
		}
		else if( pPC->isVampire() )
		{
			itemClass = Item::ITEM_CLASS_VAMPIRE_NECKLACE;
			itemType = 14;
		}
		else if( pPC->isOusters() )
		{
			itemClass = Item::ITEM_CLASS_OUSTERS_PENDENT;
			itemType = 14;
		}
		needItemType = 10;
		if( pFlagSet->isOn(FLAGSET_HELLGARDEN_SEED_5) )
			bGetItem = false;
	}

	if( !bGetItem )
	{
		GCNPCResponse response;
		response.setCode(NPC_RESPONSE_REWARD_FAIL);
		pPlayer->sendPacket(&response);
		return;
	}

	ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo( (Item::ItemClass)itemClass, itemType );
	Assert( pItemInfo != NULL );

	if( pPC->getItemClassTypeNum( Item::Item::ITEM_CLASS_MOON_CARD, needItemType ) >= NeedSeedNum )
	{
		if( itemOption != "" )
		{
			makeOptionList( itemOption, options );
		}

		pItem1 = g_pItemFactoryManager->createItem( pItemInfo->getItemClass(), pItemInfo->getItemType(), options );
		Assert( pItem1 != NULL );

		if( pItem1 == NULL )
		{
			StringStream msg;
			msg << "ActionHellGardenItem: "
				<< pItemInfo->getItemClass() << ", "
				<< pItemInfo->getItemType() << ", "
				<< itemOption.c_str();

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

			pPC->decreaseItemClassTypeNum( Item::ITEM_CLASS_MOON_CARD, needItemType, NeedSeedNum );

			GCSystemMessage gcSystemMessage;
			gcSystemMessage.setMessage( g_pStringPool->getString( STRID_TRADE_SUCCESS ) );
			pPlayer->sendPacket(&gcSystemMessage);

			pFlagSet->turnOn( FLAGSET_HELLGARDEN_SEED_1 + m_Type );
			pFlagSet->save( pPC->getName() );
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
string ActionGiveHellGardenSeedItem::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionGiveHellGardenSeedItem("
	    << ")";
	return msg.toString();

	__END_CATCH
}
