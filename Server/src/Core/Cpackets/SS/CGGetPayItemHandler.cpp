//////////////////////////////////////////////////////////////////////////////
// Filename    : CGBuyStoreItemHandler.cpp
// Written By  : ±èŒº¹Î
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGGetPayItem.h"

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

void CGGetPayItemHandler::execute (CGGetPayItem* pPacket , Player* pPlayer)
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

	BYTE RequestID =  pPacket->getMoneyID();
	int Count = 0;
	int Cash = 1;
	int uptime = 1;
	int num = 1;

/////////////////////////////////////////////////begin DB//////////////////////////////////////////////////////////////////////////
	Statement* pStmt    = NULL;
	Result*    pResult  = NULL;

	Statement* pStmt2    = NULL;

	Statement* pStmt3    = NULL;
	
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
		switch(RequestID)
		{
			case 0:
				pResult = pStmt->executeQuery("SELECT status20 FROM PayLog WHERE  PlayerID = '%s'", pGamePlayer->getID().c_str());
			break;

			case 1:
				pResult = pStmt->executeQuery("SELECT status50 FROM PayLog WHERE  PlayerID = '%s'", pGamePlayer->getID().c_str());
			break;

			case 2:
				pResult = pStmt->executeQuery("SELECT status100 FROM PayLog WHERE  PlayerID = '%s'", pGamePlayer->getID().c_str());
			break;

			case 3:
				pResult = pStmt->executeQuery("SELECT status200 FROM PayLog WHERE  PlayerID = '%s'", pGamePlayer->getID().c_str());
			break;

			case 4:
				pResult = pStmt->executeQuery("SELECT status500 FROM PayLog WHERE  PlayerID = '%s'", pGamePlayer->getID().c_str());
			break;

			case 5:
				pResult = pStmt->executeQuery("SELECT status1000 FROM PayLog WHERE  PlayerID = '%s'", pGamePlayer->getID().c_str());
			break;

			case 6:
				pResult = pStmt->executeQuery("SELECT status2000 FROM PayLog WHERE  PlayerID = '%s'", pGamePlayer->getID().c_str());
			break;

			case 7:
				pResult = pStmt->executeQuery("SELECT status5000 FROM PayLog WHERE  PlayerID = '%s'", pGamePlayer->getID().c_str());
			break;
		}
		if(pResult != NULL)
		{
			while (pResult->next()) 
			{

				Count =  pResult->getInt(1);
			}
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	if(Count > 0)
	{
		BEGIN_DB
		{
			pStmt2 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();		

			switch(RequestID)
			{
				case 0:
					pStmt2->executeQuery("UPDATE PayLog set status20=status20-%d,count20=count20+%d where PlayerID = '%s'",Cash,uptime, pGamePlayer->getID().c_str());
				break;

				case 1:
					pStmt2->executeQuery("UPDATE PayLog set status50=status50-%d,count50=count50+%d where PlayerID = '%s'",Cash,uptime, pGamePlayer->getID().c_str());
				break;

				case 2:
					pStmt2->executeQuery("UPDATE PayLog set status100=status100-%d,count100=count100+%d where PlayerID = '%s'",Cash,uptime, pGamePlayer->getID().c_str());
				break;

				case 3:
					pStmt2->executeQuery("UPDATE PayLog set status200=status200-%d,count200=count200+%d where PlayerID = '%s'",Cash,uptime, pGamePlayer->getID().c_str());
				break;

				case 4:
					pStmt2->executeQuery("UPDATE PayLog set status500=status500-%d,count500=count500+%d where PlayerID = '%s'",Cash,uptime, pGamePlayer->getID().c_str());
				break;

				case 5:
					pStmt2->executeQuery("UPDATE PayLog set status1000=status1000-%d,count1000=count1000+%d where PlayerID = '%s'",Cash,uptime, pGamePlayer->getID().c_str());
				break;

				case 6:
					pStmt2->executeQuery("UPDATE PayLog set status2000=status2000-%d,count2000=count2000+%d where PlayerID = '%s'",Cash,uptime, pGamePlayer->getID().c_str());
				break;

				case 7:
					pStmt2->executeQuery("UPDATE PayLog set status5000=status5000-%d,count5000=count5000+%d where PlayerID = '%s'",Cash,uptime, pGamePlayer->getID().c_str());
				break;
			}
			SAFE_DELETE(pStmt2);
		}

		END_DB(pStmt2)

		BEGIN_DB
		{
			pStmt3 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();		

			pStmt3->executeQuery("INSERT IGNORE INTO GoodsListObject "
						"SELECT '%s', '', %d, '%s', '%s', GoodsID, %d, 'NOT', now() FROM PayItemInfo "
						"WHERE Money = %d ",
						"PayEvent",
						g_pConfig->getPropertyInt("WorldID"),
						pGamePlayer->getID().c_str(),
						pPC->getName().c_str(),
						num,
						RequestID);		
			
			SAFE_DELETE(pStmt3);
		}

		END_DB(pStmt3)

		pPC->loadGoods();
		pPC->registerGoodsInventory( pPC->getZone()->getObjectRegistry() );
	}

	

#endif

	__END_DEBUG_EX __END_CATCH
}


