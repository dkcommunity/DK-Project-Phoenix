//////////////////////////////////////////////////////////////////////////////
// Filename    : CGBuyStoreItemHandler.cpp
// Written By  : 김성민
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGBuyStoreItem.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "Assert.h"
	#include "Store.h"
	#include "Inventory.h"
	#include "ItemUtil.h"
	#include "Zone.h"
	#include "PlayerCreature.h"
	#include "CreatureUtil.h"
	#include "PacketUtil.h"
	#include "DB.h"
	#include "VariableManager.h"
	#include "ContractOfBloodUtil.h"
	#include "item/SubInventory.h"
	#include "Gpackets/GCMyStoreInfo.h"
	#include "Gpackets/GCRemoveStoreItem.h"
	#include "Gpackets/GCShopSellOK.h"
	#include "Gpackets/GCModifyInformation.h"
	#include "Gpackets/GCCreateItem.h"
	#include "Gpackets/GCSystemMessage.h"
	#include "Gpackets/GCNoticeEvent.h"
	#include <cstdio>

	#include "DBExecuteManager.h"
	#include <sstream>

	#include "StringPool.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// 플레이어가 원하는 물건을 파는 NPC와 그 아이템이 있는지
// 확인하고, 일반 아이템과 모터 사이클 처리부분으로 분기한다.
//////////////////////////////////////////////////////////////////////////////
void CGBuyStoreItemHandler::execute (CGBuyStoreItem* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__
	
	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert(pGamePlayer != NULL);

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
	Assert( pPC != NULL );

	// 20070814 서버메시지 정리
	GCSystemMessage errorMsg;
	GCNoticeEvent errorNotice;

	if ( pPacket->getIndex() > MAX_ITEM_NUM )
	{
		filelog("Store.log", "[%s:%s] (%u) 잘못된 인덱스입니다.",
				pGamePlayer->getID().c_str(), pPC->getName().c_str(), pPacket->getIndex());
		return;
	}

	PlayerCreature* pStorePC = dynamic_cast<PlayerCreature*>(pPC->getZone()->getCreature(pPacket->getOwnerObjectID()));
	if ( pStorePC == NULL )
	{
		filelog("Store.log", "[%s:%s] (%u) 그런 유저가 없습니다.",
				pGamePlayer->getID().c_str(), pPC->getName().c_str(), pPacket->getOwnerObjectID());
//		errorMsg.setMessage("해당 판매자를 찾을 수 없습니다.");
		errorNotice.setCode(NOTICE_EVENT_CANNOT_FIND_STORE);
		pGamePlayer->sendPacket( &errorNotice );
		return;
	}

	if ( pStorePC->getRace() != pPC->getRace() )
	{
		filelog("Store.log", "[%s:%s] (%s) 다른 종족의 상점을 열려고 시도했습니다.",
				pGamePlayer->getID().c_str(), pPC->getName().c_str(), pStorePC->getName().c_str());
		return;
	}

	Store* pStore = pStorePC->getStore();
	if ( pStore == NULL || !pStore->isOpen() )
	{
		filelog("Store.log", "[%s:%s] (%s) 상점이 열려있지 않습니다..",
				pGamePlayer->getID().c_str(), pPC->getName().c_str(), pStorePC->getName().c_str());
//		errorMsg.setMessage("상점이 이미 닫혔습니다.");
		errorNotice.setCode(NOTICE_EVENT_STORE_CLOSED);
		pGamePlayer->sendPacket( &errorNotice );
		return;
	}

	StoreItem& storeItem = pStore->getStoreItem( pPacket->getIndex() );
	if ( !storeItem.isExists() )
	{
		filelog("Store.log", "[%s:%s] (%s:%u) 해당 위치에 아이템이 없습니다..",
				pGamePlayer->getID().c_str(), pPC->getName().c_str(), pStorePC->getName().c_str(), pPacket->getIndex());
//		errorMsg.setMessage("아이템이 이미 판매되었거나 판매자에 의해 철회되었습니다.");
		errorNotice.setCode(NOTICE_EVENT_ITEM_NOT_FOUND);
		pGamePlayer->sendPacket( &errorNotice );
		return;
	}

	Item* pItem = storeItem.getItem();
	Gold_t price = storeItem.getPrice();

	Assert( pItem != NULL );

	if ( pItem->getItemClass() == Item::ITEM_CLASS_SUB_INVENTORY )
	{
		if ( getSubInventoryNum( pPC ) >= 5 )
//				pSubInventoryItem->getInventory()->getItemNum() > 0 ||
//			hasItemWithItemClass( pPC, Item::ITEM_CLASS_SUB_INVENTORY ) )
		{
			errorNotice.setCode(NOTICE_EVENT_CANNOT_SELL );
			pGamePlayer->sendPacket( &errorNotice );
			return;
		}
	}

        pPC->loadDianJuan();

	if ( pPC->getDianJuan() < price )
	{
		filelog("Store.log", "[%s:%s] (%s:%u) (%u<%u) 돈이 모자랍니다.",
				pGamePlayer->getID().c_str(), pPC->getName().c_str(), pStorePC->getName().c_str(), pPacket->getIndex(), pPC->getDianJuan(), price);
//		errorMsg.setMessage("돈이 모자랍니다.");
		errorNotice.setCode(NOTICE_EVENT_NOT_ENOUGH_MONEY);
		pGamePlayer->sendPacket( &errorNotice );

		GCModifyInformation gcMI;
		gcMI.addLongData( MODIFY_DIANJUAN, pPC->getDianJuan() );
		pGamePlayer->sendPacket( &gcMI );

		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setMessage( g_pStringPool->c_str( STRID_NOT_ENOUGH_DianJuan ) );
		pGamePlayer->sendPacket(&gcSystemMessage);

		return;
	}

	if ( pStorePC->getDianJuan() > MAX_MONEY - price )
	{
		filelog("Store.log", "[%s:%s] (%s:%u) (%u,%u) 돈이 넘칩니다.",
				pGamePlayer->getID().c_str(), pPC->getName().c_str(), pStorePC->getName().c_str(), pPacket->getIndex(), pStorePC->getDianJuan(), price);
//		errorMsg.setMessage("판매자가 너무 많은 돈을 가지고 있어서 구입할 수 없습니다.");
		errorNotice.setCode(NOTICE_EVENT_TOO_MUCH_MONEY);
		pGamePlayer->sendPacket( &errorNotice );
		errorMsg.setMessage("너무 많은 돈을 가지고 있어서 물품을 판매할 수 없습니다.");
		pStorePC->getPlayer()->sendPacket(&errorMsg);
		pGamePlayer->sendPacket( &errorMsg );

		GCModifyInformation gcMI;
		gcMI.addLongData( MODIFY_DIANJUAN, pPC->getDianJuan() );
		pGamePlayer->sendPacket( &gcMI );
		return;
	}

	if ( pItem->isTimeLimitItem() || !canSellStore( pItem ) )
	{
		filelog("Store.log", "[%s:%s] (%s:%u) (%s) 팔 수 없는 아이템입니다.",
				pGamePlayer->getID().c_str(), pPC->getName().c_str(), pStorePC->getName().c_str(), pPacket->getIndex(), pItem->toString().c_str());
		return;
	}

	Inventory* pStoreInventory = pStorePC->getInventory();
	CoordInven_t storeX, storeY;

	Item* pStoreItem = pStoreInventory->findItemOID( pItem->getObjectID(), storeX, storeY );
	if ( pStoreItem != pItem )
	{
		filelog("Store.log", "[%s:%s] (%s:%u) (%p!=%p) 판매자가 아이템을 안갖고 있거나 이상한 아이템입니다.",
				pGamePlayer->getID().c_str(), pPC->getName().c_str(), pStorePC->getName().c_str(), pPacket->getIndex(), pStoreItem, pItem );
		errorMsg.setMessage("인벤토리에 판매할 아이템이 없습니다.");
		pStorePC->getPlayer()->sendPacket(&errorMsg);
		return;
	}

	Inventory* pInventory = pPC->getInventory();
	_TPOINT emptyPos;

	if ( !pInventory->getEmptySlot( pItem, emptyPos ) )
	{
		filelog("Store.log", "[%s:%s] (%s:%u) 살 사람 인벤토리에 자리가 없습니다.",
				pGamePlayer->getID().c_str(), pPC->getName().c_str(), pStorePC->getName().c_str(), pPacket->getIndex());
		errorNotice.setCode(NOTICE_EVENT_NO_INVENTORY_SPACE);
		pGamePlayer->sendPacket( &errorNotice );
		return;
	}

	if ( !pPC->decreaseMarketCashEx( price ) )
	{
		filelog("MarketCurrencyError.log", "[CGBuyStoreItemHandler] buyer MarketCash decrease failed buyer=%s seller=%s price=%u",
				pPC->getName().c_str(), pStorePC->getName().c_str(), (uint)price);
		errorNotice.setCode(NOTICE_EVENT_NO_ENOUGH_DIANJUAN);
		pGamePlayer->sendPacket( &errorNotice );

		GCModifyInformation gcMI;
		gcMI.addLongData( MODIFY_DIANJUAN, pPC->getDianJuan() );
		pGamePlayer->sendPacket( &gcMI );
		return;
	}

	if ( !pStorePC->increaseMarketCashEx( price ) )
	{
		filelog("MarketCurrencyError.log", "[CGBuyStoreItemHandler] seller MarketCash increase failed buyer=%s seller=%s price=%u",
				pPC->getName().c_str(), pStorePC->getName().c_str(), (uint)price);
		if ( !pPC->increaseMarketCashEx( price ) )
		{
			filelog("MarketCurrencyError.log", "[CGBuyStoreItemHandler] buyer MarketCash refund failed buyer=%s seller=%s price=%u",
					pPC->getName().c_str(), pStorePC->getName().c_str(), (uint)price);
		}
		errorMsg.setMessage("Market transaction failed.");
		pGamePlayer->sendPacket( &errorMsg );
		return;
	}

	Assert( pStore->removeStoreItem( pPacket->getIndex() ) == 0 );

	pStoreInventory->deleteItem( storeX, storeY );
	// 세금 띨만한곳 ~ 2006.09.21

	filelog("StoreBoughtDianJuan.log", "[%s:%u/%u] DianJuanSell.", pStorePC->getName().c_str(), pItem->getItemClass(), pItem->getItemID());

	GCShopSellOK gcSellOK;
	gcSellOK.setObjectID( pPC->getObjectID() );
	gcSellOK.setShopVersion( -1 );
	gcSellOK.setItemObjectID( pItem->getObjectID() );
	gcSellOK.setPrice( price );
	pStorePC->getPlayer()->sendPacket( &gcSellOK );

	Assert(pInventory->addItem( pItem, emptyPos ));

	if( checkContractOfBlood(pItem) )
	{
		pPC->initAllStatAndSend();
	}

	char pField[80];
	sprintf(pField, "OwnerID='%s', Storage=%d, StorageID=0, X=%d, Y=%d", pPC->getName().c_str(), STORAGE_INVENTORY, emptyPos.x, emptyPos.y);
	pItem->tinysave(pField);

	filelog("StoreBoughtDianJuan.log", "[%s:%u/%u] DianJuanBuy.", pPC->getName().c_str(), pItem->getItemClass(), pItem->getItemID());

	if ( pItem->isTraceItem() )
	{
		remainTraceLog( pItem, pStorePC->getName(), pPC->getName(), ITEM_LOG_TRADE, DETAIL_TRADE );
	}

	if ( price > g_pVariableManager->getMoneyTraceLogLimit() )
	{
		remainMoneyTraceLog( pPC->getName(), pStorePC->getName(), ITEM_LOG_TRADE, DETAIL_TRADE, price );
	}
	
	ostringstream os;
	
	os << "INSERT INTO TradeLog (Timeline, Name1, IP1, Name2, IP2, Content) VALUES ("
		<< "'" << VSDateTime::currentDateTime().toString().c_str()
		<< "', '" << pStorePC->getName().c_str()
		<< "', '" << pStorePC->getPlayer()->getSocket()->getHost().c_str()
		<< "', '" << pPC->getName().c_str()
		<< "', '" << pPC->getPlayer()->getSocket()->getHost().c_str()
		<< "', 'Store:[" << pStorePC->getName().c_str() 
			<< "(" << pStorePC->getPlayer()->getID().c_str() << ")]\n" << pItem->toString().c_str() 
			<< "\n----\nBuy:[" << pPC->getName().c_str() << "(" << pPC->getPlayer()->getID().c_str() 
			<< ")]\nGOLD:" << price << "\n')";
	filelog("DianJuan.log", "[Seller:%s/  IP:%s/  Buyer:%s/  IP:%s:/  ItemClass:%u/  ItemType:%u/  ItemID:%u/  Price:%u/  SellerDianJuanNow:%u/  BuyerDianJuanNow:%u]", 
								    pStorePC->getName().c_str(),
								    pStorePC->getPlayer()->getSocket()->getHost().c_str(),
								    pPC->getName().c_str(),
								    pPC->getPlayer()->getSocket()->getHost().c_str(),
								    pItem->getItemClass(),
								    pItem->getItemType(),
								    pItem->getItemID(),
								    price,
								    pStorePC->getDianJuan(),
								    pPC->getDianJuan());
	/*os << "INSERT INTO DianJuanLog (Timeline, Store, IP1, Buyer, IP2, Item,DianJuan,Change) VALUES ("
		<< "'" << VSDateTime::currentDateTime().toString().c_str()
		<< "', '" << pStorePC->getName().c_str()
		<< "', '" << pStorePC->getPlayer()->getSocket()->getHost().c_str()
		<< "', '" << pPC->getName().c_str()
		<< "', '" << pPC->getPlayer()->getSocket()->getHost().c_str()
		<< "', '"<< pItem->toString().c_str() 
		<< "', '"<< price
		<< "', 'Seller:[" << pStorePC->getName().c_str() 
		<< "(" << pStorePC->getPlayer()->getID().c_str() << ")]\n" << pStorePC->getDianJuan() 
		<< "\n----\nBuyer:[" << pPC->getName().c_str() << "(" << pPC->getPlayer()->getID().c_str() 
		<< ")]\nGOLD:" << pPC->getDianJuan() << "\n')";*/
	
	g_GameDBExecuteManager.RequestExecQuery(pPC->getObjectID(), os.str());	

	GCCreateItem gcCreateItem;
	makeGCCreateItem( &gcCreateItem, pItem, emptyPos.x, emptyPos.y );
	pGamePlayer->sendPacket( &gcCreateItem );

	GCModifyInformation gcMI;
	gcMI.addLongData( MODIFY_DIANJUAN, pPC->getDianJuan() );
	pGamePlayer->sendPacket( &gcMI );

	GCModifyInformation gcMISell;
	gcMISell.addLongData( MODIFY_DIANJUAN, pStorePC->getDianJuan() );
	pStorePC->getPlayer()->sendPacket( &gcMISell );

	GCRemoveStoreItem gcRemoveStoreItem;
	gcRemoveStoreItem.setOwnerObjectID( pStorePC->getObjectID() );
	gcRemoveStoreItem.setIndex( pPacket->getIndex() );
	pStorePC->getZone()->broadcastPacket( pStorePC->getX(), pStorePC->getY(), &gcRemoveStoreItem, pStorePC );

	GCMyStoreInfo gcInfo;
	gcInfo.setStoreInfo( &(pStore->getStoreInfo()) );
	pStorePC->getPlayer()->sendPacket( &gcInfo );

#endif

	__END_DEBUG_EX __END_CATCH
}


