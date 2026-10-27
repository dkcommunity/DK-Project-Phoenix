//////////////////////////////////////////////////////////////////////////////
// Filename    : CGTradeAddItemHandler.cpp
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGTradeAddItem.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "Slayer.h"
	#include "Vampire.h"
	#include "Ousters.h"
	#include "Inventory.h"
	#include "Item.h"
	#include "PetItem.h"
	#include "AR.h"
	#include "SR.h"
	#include "SG.h"
	#include "SMG.h"
	#include "Belt.h"
	#include "OustersArmsband.h"
	#include "TradeManager.h"
	#include "ZoneUtil.h"
	#include "FlagSet.h"
	#include "RelicUtil.h"
	#include "ItemUtil.h"
	#include "Store.h"

	#include "Gpackets/GCTradeAddItem.h"
	#include "Gpackets/GCTradeError.h"
	#include "Gpackets/GCTradeVerify.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeAddItemHandler::execute (CGTradeAddItem* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	ObjectID_t  TargetOID   = pPacket->getTargetObjectID();
	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

	Creature* pPC = pGamePlayer->getCreature();
	Assert(pPC != NULL);

	Zone* pZone = pPC->getZone();
	Assert(pZone != NULL);

	TradeManager* pTradeManager = pZone->getTradeManager();
	Assert(pTradeManager != NULL);
	
	// 교환을 원하는 상대방을 존에서 찾아본다.
	Creature* pTargetPC = NULL;
	/*
	try 
	{ 
		pTargetPC = pZone->getCreature(TargetOID); 
	}
	catch (NoSuchElementException) 
	{ 
		pTargetPC = NULL; 
	}
	*/

	// NoSuch제거. by sigi. 2002.5.2
	pTargetPC = pZone->getCreature(TargetOID); 

	// 교환 상대가 없다면 에러다.
	if (pTargetPC == NULL)
	{
		pTradeManager->cancelTrade(pPC);
		executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_TARGET_NOT_EXIST);
		return;
	}

	// 교환 상대가 사람이 아니거나, 같은 종족이 아니라면 에러다.
	if (!pTargetPC->isPC() || !isSameRace(pTargetPC, pPC))
	{
		pTradeManager->cancelTrade(pPC);
		executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_RACE_DIFFER);
		return;
	}

    // 둘 다 안전 지대에 있는지 체크를 한다.
	if (!isInSafeZone(pPC) || !isInSafeZone(pTargetPC))
	{
		pTradeManager->cancelTrade(pPC);
		executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_NOT_SAFE);
		return;
	}

	// 오토바이를 타고 있다면 에러다.
	if (pPC->isSlayer() && pTargetPC->isSlayer())
	{
		Slayer* pSlayer1 = dynamic_cast<Slayer*>(pPC);
		Slayer* pSlayer2 = dynamic_cast<Slayer*>(pTargetPC);

		if (pSlayer1->hasRideMotorcycle() || pSlayer2->hasRideMotorcycle())
		{
			pTradeManager->cancelTrade(pPC);
			executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_MOTORCYCLE);
			return;
		}
	}

	if (pPC->isOusters() && pTargetPC->isOusters())
	{
		Ousters* pOusters1 = dynamic_cast<Ousters*>(pPC);
		Ousters* pOusters2 = dynamic_cast<Ousters*>(pTargetPC);

		if ( pOusters1->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH) 
			|| pOusters2->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH) )
		{
			pTradeManager->cancelTrade(pPC);
			executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_MOTORCYCLE);
			return;
		}
	}

	// 둘이서 교환을 하고 있는 상태가 아니라면 에러다.
	if (!pTradeManager->isTrading(pPC, pTargetPC))
	{
		pTradeManager->cancelTrade(pPC);
		executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_NOT_TRADING);
		return;
	}

	if (pPC->isSlayer()) executeSlayer(pPacket, pPlayer);
	else if (pPC->isVampire()) executeVampire(pPacket, pPlayer);
	else if (pPC->isOusters()) executeOusters(pPacket, pPlayer);
	else throw ProtocolException("CGTradeAddItemHandler::execute() : 알 수 없는 플레이어 크리쳐");

#endif

	__END_DEBUG_EX __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeAddItemHandler::executeSlayer (CGTradeAddItem* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	// 상위 함수에서 에러를 검사했기 때문에,
	// 여기서는 포인터가 널인지를 검사하지 않는다.
	ObjectID_t   TargetOID   = pPacket->getTargetObjectID();
	ObjectID_t   ItemOID     = pPacket->getItemObjectID();
	GamePlayer*  pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*    pPC         = pGamePlayer->getCreature();
	Zone*        pZone       = pPC->getZone();
	PlayerCreature*    pTargetPC   = dynamic_cast<PlayerCreature*>(pZone->getCreature(TargetOID));

	// NoSuch제거. by sigi. 2002.5.2
	if (pTargetPC==NULL) return;

	Slayer*      pSender     = dynamic_cast<Slayer*>(pPC);

	TradeManager* pTradeManager = pZone->getTradeManager();
	Assert(pTradeManager != NULL);

	// 교환 대상에 추가할 아이템의 포인터를 얻어낸다.
	CoordInven_t X, Y;
	Inventory*   pInventory = pSender->getInventory();
	Item*        pItem      = pInventory->findItemOID(ItemOID, X, Y);

	// 추가할 아이템이 없다면 당연히 더 이상 처리가 불가능
	// 추가할 아이템이 Relic이면 추가할 수 없다.
	if (pItem == NULL
	|| !canTrade( pItem ) 
	|| pSender->getStore()->hasItem(pItem) 
//	|| ( pItem->getItemClass() == Item::ITEM_CLASS_SUB_INVENTORY && hasItemWithItemClass( pTargetPC, Item::ITEM_CLASS_SUB_INVENTORY ) )
	)
	{
		pTradeManager->cancelTrade(pPC);
		executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_ADD_ITEM);
		return;
	}

///*
//#ifdef __XMAS_EVENT_CODE__
	// 녹색 선물 상자인 경우에는, 상대방에게 적색 선물 상자가 없는지 검사한 후, 
	// 인증 패킷을 보내줘야 한다.
	if (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX)
	{
		PlayerCreature* pReceiver = dynamic_cast<PlayerCreature*>(pTargetPC);
		Item* pExtraSlotItem = pReceiver->getExtraInventorySlotItem();

		// 녹색 선물 상자
		if (pItem->getItemType() == 0)
		{
			// 교환 받을 사람이 이미 녹색 선물 상자를 받은 적이 있는지 체크한다. by sigi. 2002.12.16	
			FlagSet*   pFlagSet    = pReceiver->getFlagSet();
/*			if (pFlagSet->isOn(FLAGSET_RECEIVE_GREEN_GIFT_BOX)) 
			{
				// 적색 선물 상자는 교환 품목의 대상이 될 수 없다.
				GCTradeVerify gcTradeVerify;
				gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
				pPlayer->sendPacket(&gcTradeVerify);
				return;
			}
*/
			if (pFlagSet->isOn(FLAGSET_RECEIVE_GREEN_GIFT_BOX_2006_12))
			{
				// 적색 선물 상자는 교환 품목의 대상이 될 수 없다.
				GCTradeVerify gcTradeVerify;
				gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
				pPlayer->sendPacket(&gcTradeVerify);
				return;
			}

			Inventory* pTargetInventory = pReceiver->getInventory();
			if (pTargetInventory->hasRedGiftBox())
			{
				// 적색 선물 상자를 가지고 있다면 더할 수 없다. 리턴시킨다.
				GCTradeVerify gcTradeVerify;
				gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
				pPlayer->sendPacket(&gcTradeVerify);
				return;
			}
			else if (pExtraSlotItem != NULL)
			{
				if (pExtraSlotItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pExtraSlotItem->getItemType() == 1)
				{
					GCTradeVerify gcTradeVerify;
					gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
					pPlayer->sendPacket(&gcTradeVerify);
					return;
				}
			}
			else
			{
				// 적색 선물 상자를 가지고 있지 않다면, 걍 넘어간다.
				GCTradeVerify gcTradeVerify;
				gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_OK);
				pPlayer->sendPacket(&gcTradeVerify);
			}
		}
		// 빨간 선물 상자
		else if (pItem->getItemType() == 1)	
		{
			// 적색 선물 상자는 교환 품목의 대상이 될 수 없다.
			GCTradeVerify gcTradeVerify;
			gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
			pPlayer->sendPacket(&gcTradeVerify);
			return;
		}
	}
//#endif
//*/

	TradeInfo* pInfo1 = pTradeManager->getTradeInfo(pSender->getName());
	TradeInfo* pInfo2 = pTradeManager->getTradeInfo(pTargetPC->getName());

	// 20080926 멀티팩 5개까지만 거래가능
	if(pItem->getItemClass() == Item::ITEM_CLASS_SUB_INVENTORY && (pInfo1->getItemClassNum(pItem) + getSubInventoryNum( pTargetPC )) >= 5)
	{
		GCTradeVerify gcTradeVerify;
		gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
		pPlayer->sendPacket(&gcTradeVerify);
		return;
	}

	list<Item*>      tradeList1  = pInfo1->getItemList();

	if (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() > 1 && pItem->getItemType() < 6)
	{
		for (list<Item*>::iterator itr = tradeList1.begin(); itr != tradeList1.end(); itr++)
		{
			Item* pTradeItem = (*itr);
			if ( pTradeItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pTradeItem->getItemType() > 1 && pTradeItem->getItemType() < 6)	
			{
				GCTradeVerify gcTradeVerify;
				gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
				pPlayer->sendPacket(&gcTradeVerify);
				return;
			}
		}

		GCTradeVerify gcTradeVerify;
		gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_OK);
		pPlayer->sendPacket(&gcTradeVerify);
	}
	else if (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() >= 6)
	{
		GCTradeVerify gcTradeVerify;
		gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_OK);
		pPlayer->sendPacket(&gcTradeVerify);
	}


	// TradeManager에 교환 대상으로서 아이템을 더한다.
	//Assert(pInfo1->addItem(pItem));
	pInfo1->addItem(pItem);

	// 2006 10 09 ContractOfBlood
	//checkContractOfBlood(pPC, pItem, X, Y, pInventory->getContractOfBlood() );

	// 현재 OK를 누른 상태라면, 클라이언트에게 인증 패킷을 보내줘야 한다.
	if (pInfo1->getStatus() == TRADE_FINISH)
	{
        //cout << "CGTradeRemoveItem [" << pSender->getName() << "]의 상태가 TRADE_FINISH이므로, 인증 패킷을 보내준다." << endl;

		// 인증 패킷을 날려준다.
		GCTradeVerify gcTradeVerify;
		gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_WHEN_ACCEPT);
		pPlayer->sendPacket(&gcTradeVerify);
	}
    else
    {
		//cout << "CGTradeRemoveItem [" << pSender->getName() << "]의 상태가 TRADE_FINISH가 아니므로, 인증 패킷 날리지 않는다." << endl;
    }

	// 아이템을 더하거나 뺄 경우, 상태가 TRADE_FINISH라면 
	// TRADE_TRADING으로 바꿔줘야 한다.
	pInfo1->setStatus(TRADE_TRADING);
	pInfo2->setStatus(TRADE_TRADING);

	// 상대방에게 날려줄 아이템 정보를 구성한다.
	GCTradeAddItem gcTradeAddItem;
	makeGCTradeAddItemPacket(&gcTradeAddItem, pSender->getObjectID(), pItem, X, Y);

	// 상대방에게 교환할 아이템 정보를 날려준다.
	Player* pTargetPlayer = pTargetPC->getPlayer();
	pTargetPlayer->sendPacket(&gcTradeAddItem);


#endif

	__END_DEBUG_EX __END_CATCH
}




//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeAddItemHandler::executeVampire (CGTradeAddItem* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	// 상위 함수에서 에러를 검사했기 때문에,
	// 여기서는 포인터가 널인지를 검사하지 않는다.
	ObjectID_t   TargetOID   = pPacket->getTargetObjectID();
	ObjectID_t   ItemOID     = pPacket->getItemObjectID();
	GamePlayer*  pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*    pPC         = pGamePlayer->getCreature();
	Zone*        pZone       = pPC->getZone();
	PlayerCreature*    pTargetPC   = dynamic_cast<PlayerCreature*>(pZone->getCreature(TargetOID));

	// NoSuch제거. by sigi. 2002.5.2
	if (pTargetPC==NULL) return;

	Vampire*     pSender     = dynamic_cast<Vampire*>(pPC);

	TradeManager* pTradeManager = pZone->getTradeManager();
	Assert(pTradeManager != NULL);

	// 교환 대상에 추가할 아이템의 포인터를 얻어낸다.
	CoordInven_t X, Y;
	Inventory*   pInventory = pSender->getInventory();
	Item*        pItem      = pInventory->findItemOID(ItemOID, X, Y);

	// 추가할 아이템이 없다면 당연히 더 이상 처리가 불가능
	// Relic은 교환할 수 없다.
	if (pItem == NULL
	|| !canTrade( pItem )
	|| pSender->getStore()->hasItem(pItem)
//	|| ( pItem->getItemClass() == Item::ITEM_CLASS_SUB_INVENTORY && hasItemWithItemClass( pTargetPC, Item::ITEM_CLASS_SUB_INVENTORY ) )
	)
	{
		pTradeManager->cancelTrade(pPC);
		executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_ADD_ITEM);
		return;
	}


//
//#ifdef __XMAS_EVENT_CODE__
	// 녹색 선물 상자인 경우에는, 상대방에게 적색 선물 상자가 없는지 검사한 후, 
	// 인증 패킷을 보내줘야 한다.
	if (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX)
	{
		PlayerCreature* pReceiver = dynamic_cast<PlayerCreature*>(pTargetPC);
		Item* pExtraSlotItem = pReceiver->getExtraInventorySlotItem();

		if (pItem->getItemType() == 0)
		{
			FlagSet*   pFlagSet    = pReceiver->getFlagSet();
/*			if (pFlagSet->isOn(FLAGSET_RECEIVE_GREEN_GIFT_BOX))
			{
				// 적색 선물 상자는 교환 품목의 대상이 될 수 없다.
				GCTradeVerify gcTradeVerify;
				gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
				pPlayer->sendPacket(&gcTradeVerify);
				return;
			}
*/
			if (pFlagSet->isOn(FLAGSET_RECEIVE_GREEN_GIFT_BOX_2006_12))
			{
				// 적색 선물 상자는 교환 품목의 대상이 될 수 없다.
				GCTradeVerify gcTradeVerify;
				gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
				pPlayer->sendPacket(&gcTradeVerify);
				return;
			}

			Inventory* pTargetInventory = pReceiver->getInventory();
			if (pTargetInventory->hasRedGiftBox())
			{
				// 적색 선물 상자를 가지고 있다면 더할 수 없다. 리턴시킨다.
				GCTradeVerify gcTradeVerify;
				gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
				pPlayer->sendPacket(&gcTradeVerify);
				return;
			}
			else if (pExtraSlotItem != NULL)
			{
				if (pExtraSlotItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pExtraSlotItem->getItemType() == 1)
				{
					GCTradeVerify gcTradeVerify;
					gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
					pPlayer->sendPacket(&gcTradeVerify);
					return;
				}
			}
			else
			{
				// 적색 선물 상자를 가지고 있지 않다면, 걍 넘어간다.
				GCTradeVerify gcTradeVerify;
				gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_OK);
				pPlayer->sendPacket(&gcTradeVerify);
			}
		}
		else if (pItem->getItemType() == 1)
		{
			// 적색 선물 상자는 교환 품목이 될 수 없다.
			GCTradeVerify gcTradeVerify;
			gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
			pPlayer->sendPacket(&gcTradeVerify);
			return;
		}
	}
//#endif
//

	TradeInfo* pInfo1 = pTradeManager->getTradeInfo(pSender->getName());
	TradeInfo* pInfo2 = pTradeManager->getTradeInfo(pTargetPC->getName());

	// 20080926 멀티팩 5개까지만 거래가능
	if(pItem->getItemClass() == Item::ITEM_CLASS_SUB_INVENTORY && (pInfo1->getItemClassNum(pItem) + getSubInventoryNum( pTargetPC )) >= 5)
	{
		GCTradeVerify gcTradeVerify;
		gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
		pPlayer->sendPacket(&gcTradeVerify);
		return;
	}
	
	list<Item*>      tradeList1  = pInfo1->getItemList();

	if (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() > 1 && pItem->getItemType() < 6)
	{
		for (list<Item*>::iterator itr = tradeList1.begin(); itr != tradeList1.end(); itr++)
		{
			Item* pTradeItem = (*itr);
			if ( pTradeItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pTradeItem->getItemType() > 1 && pTradeItem->getItemType() < 6)	
			{
				GCTradeVerify gcTradeVerify;
				gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
				pPlayer->sendPacket(&gcTradeVerify);
				return;
			}
		}

		GCTradeVerify gcTradeVerify;
		gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_OK);
		pPlayer->sendPacket(&gcTradeVerify);
	}
	else if (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() >= 6)
	{
		GCTradeVerify gcTradeVerify;
		gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_OK);
		pPlayer->sendPacket(&gcTradeVerify);
	}



	// TradeManager에 교환 대상으로서 아이템을 더한다.
	//Assert(pInfo1->addItem(pItem));
	pInfo1->addItem(pItem);

	// 2006 10 09 ContractOfBlood
	//checkContractOfBlood(pPC, pItem, X, Y, pInventory->getContractOfBlood() );

	// 현재 OK를 누른 상태라면, 클라이언트에게 인증 패킷을 보내줘야 한다.
	if (pInfo1->getStatus() == TRADE_FINISH)
	{
        //cout << "CGTradeRemoveItem [" << pSender->getName() << "]의 상태가 TRADE_FINISH이므로, 인증 패킷을 보내준다." << endl;

		// 인증 패킷을 날려준다.
		GCTradeVerify gcTradeVerify;
		gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_WHEN_ACCEPT);
		pPlayer->sendPacket(&gcTradeVerify);
	}
    else
    {
		//cout << "CGTradeRemoveItem [" << pSender->getName() << "]의 상태가 TRADE_FINISH가 아니므로, 인증 패킷 날리지 않는다." << endl;
    }

	// 아이템을 더하거나 뺄 경우, 상태가 TRADE_FINISH라면 
	// TRADE_TRADING으로 바꿔줘야 한다.
	pInfo1->setStatus(TRADE_TRADING);
	pInfo2->setStatus(TRADE_TRADING);

	// 상대방에게 날려줄 아이템 정보를 구성한다.
	GCTradeAddItem gcTradeAddItem;
	makeGCTradeAddItemPacket(&gcTradeAddItem, pSender->getObjectID(), pItem, X, Y);

	// 상대방에게 교환할 아이템 정보를 날려준다.
	Player* pTargetPlayer = pTargetPC->getPlayer();
	pTargetPlayer->sendPacket(&gcTradeAddItem);

#endif

	__END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeAddItemHandler::executeOusters (CGTradeAddItem* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	// 상위 함수에서 에러를 검사했기 때문에,
	// 여기서는 포인터가 널인지를 검사하지 않는다.
	ObjectID_t   TargetOID   = pPacket->getTargetObjectID();
	ObjectID_t   ItemOID     = pPacket->getItemObjectID();
	GamePlayer*  pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Creature*    pPC         = pGamePlayer->getCreature();
	Zone*        pZone       = pPC->getZone();
	PlayerCreature*    pTargetPC   = dynamic_cast<PlayerCreature*>(pZone->getCreature(TargetOID));

	// NoSuch제거. by sigi. 2002.5.2
	if (pTargetPC==NULL) return;

	Ousters*     pSender     = dynamic_cast<Ousters*>(pPC);

	TradeManager* pTradeManager = pZone->getTradeManager();
	Assert(pTradeManager != NULL);

	// 교환 대상에 추가할 아이템의 포인터를 얻어낸다.
	CoordInven_t X, Y;
	Inventory*   pInventory = pSender->getInventory();
	Item*        pItem      = pInventory->findItemOID(ItemOID, X, Y);

	// 추가할 아이템이 없다면 당연히 더 이상 처리가 불가능
	// Relic은 교환할 수 없다.
	if (pItem == NULL
	|| !canTrade( pItem )
	|| pSender->getStore()->hasItem(pItem)
//	|| ( pItem->getItemClass() == Item::ITEM_CLASS_SUB_INVENTORY && hasItemWithItemClass( pTargetPC, Item::ITEM_CLASS_SUB_INVENTORY ) )
    )
	{
		pTradeManager->cancelTrade(pPC);
		executeError(pPacket, pPlayer, GC_TRADE_ERROR_CODE_ADD_ITEM);
		return;
	}


//
//#ifdef __XMAS_EVENT_CODE__
	// 녹색 선물 상자인 경우에는, 상대방에게 적색 선물 상자가 없는지 검사한 후, 
	// 인증 패킷을 보내줘야 한다.
	if (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX)
	{
		PlayerCreature* pReceiver = dynamic_cast<PlayerCreature*>(pTargetPC);
		Item* pExtraSlotItem = pReceiver->getExtraInventorySlotItem();

		if (pItem->getItemType() == 0)
		{
			FlagSet*   pFlagSet    = pReceiver->getFlagSet();
/*			if (pFlagSet->isOn(FLAGSET_RECEIVE_GREEN_GIFT_BOX))
			{
				// 적색 선물 상자는 교환 품목의 대상이 될 수 없다.
				GCTradeVerify gcTradeVerify;
				gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
				pPlayer->sendPacket(&gcTradeVerify);
				return;
			}
*/
			if (pFlagSet->isOn(FLAGSET_RECEIVE_GREEN_GIFT_BOX_2006_12))
			{
				// 적색 선물 상자는 교환 품목의 대상이 될 수 없다.
				GCTradeVerify gcTradeVerify;
				gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
				pPlayer->sendPacket(&gcTradeVerify);
				return;
			}

			Inventory* pTargetInventory = pReceiver->getInventory();
			if (pTargetInventory->hasRedGiftBox())
			{
				// 적색 선물 상자를 가지고 있다면 더할 수 없다. 리턴시킨다.
				GCTradeVerify gcTradeVerify;
				gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
				pPlayer->sendPacket(&gcTradeVerify);
				return;
			}
			else if (pExtraSlotItem != NULL)
			{
				if (pExtraSlotItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pExtraSlotItem->getItemType() == 1)
				{
					GCTradeVerify gcTradeVerify;
					gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
					pPlayer->sendPacket(&gcTradeVerify);
					return;
				}
			}
			else
			{
				// 적색 선물 상자를 가지고 있지 않다면, 걍 넘어간다.
				GCTradeVerify gcTradeVerify;
				gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_OK);
				pPlayer->sendPacket(&gcTradeVerify);
			}
		}
		else if (pItem->getItemType() == 1)
		{
			// 적색 선물 상자는 교환 품목이 될 수 없다.
			GCTradeVerify gcTradeVerify;
			gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
			pPlayer->sendPacket(&gcTradeVerify);
			return;
		}
	}
//#endif
//

	TradeInfo* pInfo1 = pTradeManager->getTradeInfo(pSender->getName());
	TradeInfo* pInfo2 = pTradeManager->getTradeInfo(pTargetPC->getName());

	// 20080926 멀티팩 5개까지만 거래가능
	if(pItem->getItemClass() == Item::ITEM_CLASS_SUB_INVENTORY && (pInfo1->getItemClassNum(pItem) + getSubInventoryNum( pTargetPC )) >= 5)
	{
		GCTradeVerify gcTradeVerify;
		gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
		pPlayer->sendPacket(&gcTradeVerify);
		return;
	}

	list<Item*>      tradeList1  = pInfo1->getItemList();

	if (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() > 1 && pItem->getItemType() < 6)
	{
		for (list<Item*>::iterator itr = tradeList1.begin(); itr != tradeList1.end(); itr++)
		{
			Item* pTradeItem = (*itr);
			if ( pTradeItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pTradeItem->getItemType() > 1 && pTradeItem->getItemType() < 6)	
			{
				GCTradeVerify gcTradeVerify;
				gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL);
				pPlayer->sendPacket(&gcTradeVerify);
				return;
			}
		}

		GCTradeVerify gcTradeVerify;
		gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_OK);
		pPlayer->sendPacket(&gcTradeVerify);
	}
	else if (pItem->getItemClass() == Item::ITEM_CLASS_EVENT_GIFT_BOX && pItem->getItemType() >= 6)
	{
		GCTradeVerify gcTradeVerify;
		gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_OK);
		pPlayer->sendPacket(&gcTradeVerify);
	}


	// TradeManager에 교환 대상으로서 아이템을 더한다.
	//Assert(pInfo1->addItem(pItem));
	pInfo1->addItem(pItem);

	// 2006 10 09 ContractOfblood
	//checkContractOfBlood(pPC, pItem, X, Y, pInventory->getContractOfBlood() );

	// 현재 OK를 누른 상태라면, 클라이언트에게 인증 패킷을 보내줘야 한다.
	if (pInfo1->getStatus() == TRADE_FINISH)
	{
        //cout << "CGTradeRemoveItem [" << pSender->getName() << "]의 상태가 TRADE_FINISH이므로, 인증 패킷을 보내준다." << endl;

		// 인증 패킷을 날려준다.
		GCTradeVerify gcTradeVerify;
		gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_ADD_ITEM_WHEN_ACCEPT);
		pPlayer->sendPacket(&gcTradeVerify);
	}
    else
    {
		//cout << "CGTradeRemoveItem [" << pSender->getName() << "]의 상태가 TRADE_FINISH가 아니므로, 인증 패킷 날리지 않는다." << endl;
    }

	// 아이템을 더하거나 뺄 경우, 상태가 TRADE_FINISH라면 
	// TRADE_TRADING으로 바꿔줘야 한다.
	pInfo1->setStatus(TRADE_TRADING);
	pInfo2->setStatus(TRADE_TRADING);

	// 상대방에게 날려줄 아이템 정보를 구성한다.
	GCTradeAddItem gcTradeAddItem;
	makeGCTradeAddItemPacket(&gcTradeAddItem, pSender->getObjectID(), pItem, X, Y);

	// 상대방에게 교환할 아이템 정보를 날려준다.
	Player* pTargetPlayer = pTargetPC->getPlayer();
	pTargetPlayer->sendPacket(&gcTradeAddItem);

#endif

	__END_DEBUG_EX __END_CATCH
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeAddItemHandler::makeGCTradeAddItemPacket (GCTradeAddItem* pPacket, ObjectID_t Sender, Item* pItem, CoordInven_t X, CoordInven_t Y) 
	throw ()
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__
	
	// 상대방에게 날려줄 아이템 정보를 구성한다.
	pPacket->setTargetObjectID(Sender);
	pPacket->setX(X);
	pPacket->setY(Y);
	
	pItem->makePCItemInfo(*pPacket->GetPCItemInfo());

#endif

	__END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTradeAddItemHandler::executeError(CGTradeAddItem* pPacket, Player* pPlayer, BYTE ErrorCode)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__
	
	GCTradeError gcTradeError;
	gcTradeError.setTargetObjectID(pPacket->getTargetObjectID());
	gcTradeError.setCode(ErrorCode);
	pPlayer->sendPacket(&gcTradeError);

#endif

	__END_DEBUG_EX __END_CATCH
}

