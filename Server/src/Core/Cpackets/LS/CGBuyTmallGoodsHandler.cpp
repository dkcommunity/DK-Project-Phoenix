//////////////////////////////////////////////////////////////////////////////
// Filename    : CGBuyStoreItemHandler.cpp
// Written By  : ±èŒº¹Î
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGBuyTmallGoods.h"

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
	#include "Properties.h"
#endif

void CGBuyTmallGoodsHandler::execute (CGBuyTmallGoods* pPacket , Player* pPlayer)
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

	GCSystemMessage errorMsg;
	GCNoticeEvent errorNotice;

	Gold_t Cash = 0;
	Gold_t Point = 0;
	int Kind = -1;
	bool foundGoods = false;
	int num =1;
/////////////////////////////////////////////////begin DB//////////////////////////////////////////////////////////////////////////
	Statement* pStmt    = NULL;
	Result*    pResult  = NULL;
	
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
		pResult = pStmt->executeQuery(
		"SELECT t.Cash,t.Point,t.Kind FROM TmallListInfo t "
		"INNER JOIN GoodsListInfo g ON g.GoodsID = t.GoodsID AND g.Kind <> 'SET' "
		"WHERE t.GoodsID = '%d'",
		(int)pPacket->getGoodsID()
		);
		while (pResult->next()) 
		{
			int i = 0;

			Cash =  pResult->getInt(++i);
			Point = pResult->getInt(++i);
			Kind = pResult->getInt(++i);
			foundGoods = true;
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
//////////////////////////////////////////////////////////////end db/////////////////////////////////////////////////////////////////////////////
	if (!foundGoods || Kind != (int)pPacket->getKind())
	{
		errorNotice.setCode(NOTICE_EVENT_ITEM_NOT_FOUND);
		pGamePlayer->sendPacket( &errorNotice );
		return;
	}

	// Kind yalnizca sekme dogrulamasi icin; odeme Cash/Point kolonlarindan belirlenir.
	enum TmallPaymentType
	{
		TMALL_PAY_FREE = 0,
		TMALL_PAY_CASH,
		TMALL_PAY_POINT
	};

	TmallPaymentType paymentType = TMALL_PAY_FREE;
	Gold_t chargeCash = 0;
	Gold_t chargePoint = 0;
	const char* buyId = "TMALLBUYFree";
	const char* logType = "FreeBuy";

	if (Cash <= 0 && Point <= 0)
	{
		paymentType = TMALL_PAY_FREE;
	}
	else if (Cash > 0 && Point <= 0)
	{
		pPC->loadDianJuan();
		if (pPC->getDianJuan() < Cash)
		{
			errorNotice.setCode(NOTICE_EVENT_NO_ENOUGH_DIANJUAN);
			pGamePlayer->sendPacket(&errorNotice);

			GCModifyInformation gcMI;
			gcMI.addLongData(MODIFY_DIANJUAN, pPC->getDianJuan());
			pGamePlayer->sendPacket(&gcMI);
			return;
		}

		paymentType = TMALL_PAY_CASH;
		chargeCash = Cash;
		buyId = "TMALLBUYDianJuan";
		logType = "CashBuy";
	}
	else if (Point > 0 && Cash <= 0)
	{
		pPC->loadJiFen();
		if (pPC->getJiFen() < Point)
		{
			errorNotice.setCode(NOTICE_EVENT_NO_ENOUGH_JIFEN);
			pGamePlayer->sendPacket(&errorNotice);

			GCModifyInformation gcMI;
			gcMI.addLongData(MODIFY_JIFEN, pPC->getJiFen());
			pGamePlayer->sendPacket(&gcMI);
			return;
		}

		paymentType = TMALL_PAY_POINT;
		chargePoint = Point;
		buyId = "TMALLBUYJiFen";
		logType = "PointBuy";
	}
	else
	{
		// Cash > 0 ve Point > 0: once Point, yetmezse Cash fallback.
		pPC->loadDianJuan();
		pPC->loadJiFen();

		if (pPC->getJiFen() >= Point)
		{
			paymentType = TMALL_PAY_POINT;
			chargePoint = Point;
			buyId = "TMALLBUYJiFen";
			logType = "PointBuy";
		}
		else if (pPC->getDianJuan() >= Cash)
		{
			paymentType = TMALL_PAY_CASH;
			chargeCash = Cash;
			buyId = "TMALLBUYDianJuan";
			logType = "CashBuy";
		}
		else
		{
			errorNotice.setCode(NOTICE_EVENT_NO_ENOUGH_JIFEN);
			pGamePlayer->sendPacket(&errorNotice);

			GCModifyInformation gcMI;
			gcMI.addLongData(MODIFY_JIFEN, pPC->getJiFen());
			pGamePlayer->sendPacket(&gcMI);
			return;
		}
	}

	if (paymentType == TMALL_PAY_CASH)
	{
		if (!pPC->decreaseMarketCashEx(chargeCash))
		{
			filelog("MarketCurrencyError.log", "[CGBuyTmallGoodsHandler] MarketCash decrease failed player=%s goods=%u price=%u", pGamePlayer->getID().c_str(), (uint)pPacket->getGoodsID(), (uint)chargeCash);
			errorNotice.setCode(NOTICE_EVENT_ITEM_NOT_FOUND);
			pGamePlayer->sendPacket(&errorNotice);
			return;
		}
	}
	else if (paymentType == TMALL_PAY_POINT)
	{
		if (!pPC->decreaseMarketPointEx(chargePoint))
		{
			filelog("MarketCurrencyError.log", "[CGBuyTmallGoodsHandler] MarketPoint decrease failed player=%s goods=%u price=%u", pGamePlayer->getID().c_str(), (uint)pPacket->getGoodsID(), (uint)chargePoint);
			errorNotice.setCode(NOTICE_EVENT_ITEM_NOT_FOUND);
			pGamePlayer->sendPacket(&errorNotice);
			return;
		}
	}

	Statement* pBuyStmt = NULL;
	Statement* pBuyStmt2 = NULL;
	Statement* pBuyStmt3 = NULL;
	Statement* pBuyStmt4 = NULL;

	BEGIN_DB
	{
		pBuyStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();

		pBuyStmt->executeQuery(
			"INSERT INTO GoodsListObject "
			"(BuyID, World, PlayerID, Name, GoodsID, Num, Status, `Count`) "
			"VALUES ('%s', %d, '%s', '%s', %d, %d, 'NOT', 0)",
			buyId,
			g_pConfig->getPropertyInt("WorldID"),
			pGamePlayer->getID().c_str(),
			pPC->getName().c_str(),
			pPacket->getGoodsID(),
			num
		);
		SAFE_DELETE(pBuyStmt);
	}

	END_DB(pBuyStmt)

	BEGIN_DB
	{
		pBuyStmt2 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();

		pBuyStmt2->executeQuery("UPDATE TmallListInfo set TotalNum=TotalNum+1 where GoodsID ='%d'", pPacket->getGoodsID());
		SAFE_DELETE(pBuyStmt2);
	}

	END_DB(pBuyStmt2)

	if (chargeCash > 0)
	{
		BEGIN_DB
		{
			pBuyStmt3 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();

			pBuyStmt3->executeQuery("UPDATE TmallListInfo set TotalCash=TotalCash+%d where GoodsID ='%d'", chargeCash, pPacket->getGoodsID());
			SAFE_DELETE(pBuyStmt3);
		}

		END_DB(pBuyStmt3)
	}

	if (chargePoint > 0)
	{
		BEGIN_DB
		{
			pBuyStmt3 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();

			pBuyStmt3->executeQuery("UPDATE TmallListInfo set TotalPoint=TotalPoint+%d where GoodsID ='%d'", chargePoint, pPacket->getGoodsID());
			SAFE_DELETE(pBuyStmt3);
		}

		END_DB(pBuyStmt3)
	}

	BEGIN_DB
	{
		pBuyStmt4 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();

		if (paymentType == TMALL_PAY_POINT)
		{
			pBuyStmt4->executeQuery("INSERT IGNORE INTO TmallListInfolog VALUES ('',%d, '%s', '%s', '', '', %d, %d, '%s','%s', now())",
				pPacket->getGoodsID(),
				pGamePlayer->getID().c_str(),
				pPC->getName().c_str(),
				pPC->getJiFen(),
				chargePoint,
				pPC->getPlayer()->getSocket()->getHost().c_str(),
				logType
			);
		}
		else if (paymentType == TMALL_PAY_CASH)
		{
			pBuyStmt4->executeQuery("INSERT IGNORE INTO TmallListInfolog VALUES ('',%d, '%s', '%s', %d, %d,'','', '%s','%s', now())",
				pPacket->getGoodsID(),
				pGamePlayer->getID().c_str(),
				pPC->getName().c_str(),
				pPC->getDianJuan(),
				chargeCash,
				pPC->getPlayer()->getSocket()->getHost().c_str(),
				logType
			);
		}
		else
		{
			pBuyStmt4->executeQuery("INSERT IGNORE INTO TmallListInfolog VALUES ('',%d, '%s', '%s', 0, 0, 0, 0, '%s','%s', now())",
				pPacket->getGoodsID(),
				pGamePlayer->getID().c_str(),
				pPC->getName().c_str(),
				pPC->getPlayer()->getSocket()->getHost().c_str(),
				logType
			);
		}

		SAFE_DELETE(pBuyStmt4);
	}

	END_DB(pBuyStmt4)

	errorNotice.setCode(NOTICE_EVENT_TMALL_BUY_OKY);
	pGamePlayer->sendPacket(&errorNotice);

	pPC->loadGoods();
	pPC->registerGoodsInventory(pPC->getZone()->getObjectRegistry());

	if (paymentType == TMALL_PAY_CASH)
	{
		GCModifyInformation gcMI;
		gcMI.addLongData(MODIFY_DIANJUAN, pPC->getDianJuan());
		pGamePlayer->sendPacket(&gcMI);
	}
	else if (paymentType == TMALL_PAY_POINT)
	{
		GCModifyInformation gcMI;
		gcMI.addLongData(MODIFY_JIFEN, pPC->getJiFen());
		pGamePlayer->sendPacket(&gcMI);
	}

#endif

	__END_DEBUG_EX __END_CATCH
}

