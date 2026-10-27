//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUseItemFromInventoryHandler.cpp
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGUseRankGemFromInventory.h"

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
#endif
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGUseRankGemFromInventoryHandler::execute(CGUseRankGemFromInventory* pPacket, Player* pPlayer)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	try 
	{
	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert(pGamePlayer != NULL);

	Creature* pCreature = pGamePlayer->getCreature();
	Assert(pCreature != NULL);
	Assert(pCreature->isPC());

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Assert(pPC != NULL);

	ObjectID_t      objectID    = pPacket->getObjectID();

	Item*   pMouseItem = pPC->getExtraInventorySlotItem();

	if (pMouseItem==NULL)
	{
		GCAddItemToItemVerify gcAddItemToItemVerify;
		gcAddItemToItemVerify.setCode( ADD_ITEM_TO_RANKGEM_ERROR );
		pGamePlayer->sendPacket( &gcAddItemToItemVerify );
		return;
	}

	ObjectID_t ItemObjectID = pMouseItem->getObjectID();

	if(ItemObjectID!=objectID)
	{
		GCAddItemToItemVerify gcAddItemToItemVerify;
		gcAddItemToItemVerify.setCode( ADD_ITEM_TO_RANKGEM_ERROR );
		pGamePlayer->sendPacket( &gcAddItemToItemVerify );
		return;
	}

	RankGemInfo* pGemInfo = dynamic_cast<RankGemInfo*>(g_pItemInfoManager->getItemInfo(pMouseItem->getItemClass(), pMouseItem->getItemType()));
	Assert(pGemInfo != NULL);


	int GotSlotID = pPacket->getSlotID();
	int GemNeedA = 0;
	int GemNeedB = 0;
	int SlotNeedA = 0;
	int SlotNeedB = 0;
	int needrace = 0;
	int myRace =0;
	int Type =0;
	int Grade =0;
	Statement* pStmt1 = NULL;
////////////////////////////////////////////////////////get iteminfo //////////////////////////////////////////
		BEGIN_DB
		{
			pStmt1 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
			Result* pResult1 = pStmt1->executeQuery("SELECT Race,NeedA,NeedB FROM RankGemInfo WHERE ItemType='%d'",
			(int)pMouseItem->getItemType());
			while (pResult1->next())
			{
				int i = 0;
				needrace =  pResult1->getInt(++i);
				GemNeedA =  pResult1->getInt(++i);
				GemNeedB =  pResult1->getInt(++i);
			}

			SAFE_DELETE(pStmt1);
		}
		END_DB(pStmt1)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		Statement* pStmt2 = NULL;
/////////////////////////////////////////////////////get slotinfo/////////////////////////////////////////////////
		BEGIN_DB
		{
			pStmt2 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
			Result* pResult2 = pStmt2->executeQuery("SELECT NeedA,NeedB FROM RankGemSlotInfo WHERE SlotID='%d'",
			GotSlotID);
			while (pResult2->next())
			{
				int i = 0;
				SlotNeedA =  pResult2->getInt(++i);
				SlotNeedB =  pResult2->getInt(++i);
			}

			SAFE_DELETE(pStmt2);
		}
		END_DB(pStmt2)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		if((GemNeedA == SlotNeedA) && (GemNeedB == SlotNeedB))//check gem and slot
			{
////////////////////////check slot open /////////////////////////////////////////////////////////////

				Statement* pStmt3 = NULL;
				BEGIN_DB
				{
					pStmt3 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
					Result* pResult3 = pStmt3->executeQuery("SELECT Race,Type,Grade FROM RankGemData WHERE Name='%s' and Slot='%d'",
									pPC->getName().c_str(),
									GotSlotID);
					while (pResult3->next())
					{
						int i = 0;
						myRace =  pResult3->getInt(++i);
						Type =  pResult3->getInt(++i);
						Grade =  pResult3->getInt(++i);
						if((needrace==7 || needrace == myRace) && Type == 999 && Grade == 999)
							{
									Statement* pStmt4 = NULL;
									Statement* pStmt5 = NULL;
									BEGIN_DB
									{
										pStmt4 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
										pStmt4->executeQuery("UPDATE RankGemData SET Type='%d',Grade='%d',OptionClass='%d' WHERE Name='%s' and Race = '%d' and Slot='%d'",
										(int)pMouseItem->getItemType(),
										(int)pMouseItem->getGrade(),
										(int)pGemInfo->getOptionClass(),
										pPC->getName().c_str(),
										(int)pPC->getRace(),
										GotSlotID);
										SAFE_DELETE(pStmt4);
									}
									END_DB(pStmt4)

									BEGIN_DB
									{
										pStmt5 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
										pStmt5->executeQuery("INSERT INTO RankGemUseLog (Name, Race, Type, Grade, Slot, GemObjectID, UseTime) VALUES ('%s','%d','%d', '%d', '%d','%d',now())",
													pPC->getName().c_str(),
													(int)pPC->getRace(),
													(int)pMouseItem->getItemType(),
													(int)pMouseItem->getGrade(),
													GotSlotID,
													ItemObjectID);
										SAFE_DELETE(pStmt5);
									}
									END_DB(pStmt5)

									GCAddItemToItemVerify gcAddItemToItemVerify;
									gcAddItemToItemVerify.setCode( ADD_ITEM_TO_RANKGEM_OK );
									pGamePlayer->sendPacket( &gcAddItemToItemVerify );

									pPC->deleteItemFromExtraInventorySlot();
									pMouseItem->destroy();
									SAFE_DELETE( pMouseItem );
									pPC->loadRankGem();
									pPC->initAllStatAndSend();

							}
						else
							{
										GCAddItemToItemVerify gcAddItemToItemVerify;
										gcAddItemToItemVerify.setCode( ADD_ITEM_TO_RANKGEM_ERROR );
										pGamePlayer->sendPacket( &gcAddItemToItemVerify );
										return;
							}
					}
					SAFE_DELETE(pStmt3);
				}
				END_DB(pStmt3)
			}
			else
				{
										GCAddItemToItemVerify gcAddItemToItemVerify;
										gcAddItemToItemVerify.setCode( ADD_ITEM_TO_RANKGEM_ERROR );
										pGamePlayer->sendPacket( &gcAddItemToItemVerify );
										return;
				}
	}
	catch (Throwable & t) 
	{ 
		cout << t.toString();
	}

#endif

    __END_DEBUG_EX __END_CATCH
}
