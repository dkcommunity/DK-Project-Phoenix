//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRemoveRankGemHandler.cpp
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGRemoveRankGem.h"

#ifdef __GAME_SERVER__
	#include <stdio.h>
	#include "GamePlayer.h"
	#include "PlayerCreature.h"
	#include "Inventory.h"
	#include "Item.h"
	#include "ItemInfoManager.h"
	#include "ItemFactoryManager.h"
	#include "OptionInfo.h"
	#include "ItemUtil.h"
	#include "CreatureUtil.h"
	#include "Slayer.h"
	#include "Vampire.h"
	#include "Ousters.h"
	#include "PacketUtil.h"
	#include "DB.h"
	#include "Zone.h"
	#include "RankGem.h"
	#include "Gpackets/GCAddItemToItemVerify.h"
	#include "Gpackets/GCDeleteInventoryItem.h"
	#include "Gpackets/GCModifyInformation.h"
	#include "Gpackets/GCSystemMessage.h"
	#include "Gpackets/GCNoticeEvent.h"
#endif
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGRemoveRankGemHandler::execute(CGRemoveRankGem* pPacket, Player* pPlayer)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__
	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert(pGamePlayer != NULL);

	Creature* pCreature = pGamePlayer->getCreature();
	Assert(pCreature != NULL);
	Assert(pCreature->isPC());

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Assert(pPC != NULL);

	GCSystemMessage errorMsg;
	GCNoticeEvent errorNotice;

	int GotSlotID = pPacket->getSlotID();
	Gold_t Cash = 0;

	pPC->loadDianJuan();
	if ( pPC->getDianJuan() < Cash )
	{
		errorNotice.setCode(NOTICE_EVENT_NO_ENOUGH_DIANJUAN);
		pGamePlayer->sendPacket( &errorNotice );

		GCModifyInformation gcMI;
		gcMI.addLongData( MODIFY_DIANJUAN, pPC->getDianJuan() );
		pGamePlayer->sendPacket( &gcMI );

		return;
	}

	if ( !pPC->decreaseMarketCashEx( Cash ) )
	{
		filelog("MarketCurrencyError.log", "[CGRemoveRankGemHandler] MarketCash decrease failed player=%s cash=%u",
				pPC->getName().c_str(), (uint)Cash);
		errorNotice.setCode(NOTICE_EVENT_NO_ENOUGH_DIANJUAN);
		pGamePlayer->sendPacket( &errorNotice );

		GCModifyInformation gcMI;
		gcMI.addLongData( MODIFY_DIANJUAN, pPC->getDianJuan() );
		pGamePlayer->sendPacket( &gcMI );
		return;
	}

	Statement* pStmt = NULL;
		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
			Result* pResult = pStmt->executeQuery("SELECT Race,Type,Grade FROM RankGemData WHERE Name='%s' and Slot='%d'",
									pPC->getName().c_str(),
									GotSlotID);
			while (pResult->next())
			{
				int i = 0;
				int Race =  pResult->getInt(++i);
				int Type =  pResult->getInt(++i);
				int Grade =  pResult->getInt(++i);
				if(Type != 999)
				{
					Statement* pStmt2 = NULL;
					Statement* pStmt3 = NULL;
					BEGIN_DB
					{
						pStmt2 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
						pStmt2->executeQuery("UPDATE RankGemData SET Type='999',Grade='999',OptionClass='999' WHERE Name='%s' and Race = '%d' and Slot='%d'",
						pPC->getName().c_str(),
						(int)pPC->getRace(),
						GotSlotID);
						SAFE_DELETE(pStmt2);
					}
					END_DB(pStmt2)

					BEGIN_DB
					{
						pStmt3 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
						pStmt3->executeQuery("INSERT INTO RankGemRemoveLog (Name, Race, Type, Grade, Slot, UseTime) VALUES ('%s','%d','%d', '%d', '%d',now())",
									pPC->getName().c_str(),
									(int)pPC->getRace(),
									Type,
									Grade,
									GotSlotID);
						SAFE_DELETE(pStmt3);
					}
					END_DB(pStmt3)
					pPC->clearRankGem((NewRankGem::RankGemType)Type);
					pPC->initAllStatAndSend();

					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_RANKGEM_REMOVE_OK );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );

					GCModifyInformation gcMI;
					gcMI.addLongData( MODIFY_DIANJUAN, pPC->getDianJuan() );
					pGamePlayer->sendPacket( &gcMI );
				}
			}

			SAFE_DELETE(pStmt);
		}
		END_DB(pStmt)


#endif

    __END_DEBUG_EX __END_CATCH
}
