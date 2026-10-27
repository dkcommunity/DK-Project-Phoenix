////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionGiveContributeReward.cpp
// Written By  : 
// Description :
// 
// History.
//
//  Date        Writer         Description
// ---------- ----------- ------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////////

#include "ActionGiveContributeReward.h"
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
#include "VariableManager.h"	//smallheart 080926

const int NeedContributeRank = 1;

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionGiveContributeReward::read(PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

 //	try
	//{
	//	m_Type = (ItemType_t)propertyBuffer.getPropertyInt("QuestItem");
	//}
	//catch (NoSuchElementException & nsee)
	//{
	//	throw Error(nsee.toString());
	//}

   __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// 액션을 실행한다.
////////////////////////////////////////////////////////////////////////////////
void ActionGiveContributeReward::execute(Creature * pCreature1 , Creature * pCreature2)
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

	//cout << "ActionGiveContributeReward" << ":" << m_Type;

	if(pPC->getContributeRank() != NeedContributeRank)
	{
		GCNPCResponse response;
		response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		return;
	}
	Item* pItem1 = NULL;

	list<OptionType_t> options;
		
	pItem1 = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_SKILL_BOOK, 19, options);
	Assert(pItem1 != NULL);
	if( pItem1 == NULL )
	{
		StringStream msg;
		msg << "ActionGiveContributeReward: "
			<< (int)Item::ITEM_CLASS_SKILL_BOOK << ", "
			<< (int)19;
		filelog("GiveContributeRewardBug.txt", "%s", msg.toString().c_str());

		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setMessage( g_pStringPool->getString( STRID_ITEM_CREATE_ERROR ) );
		pPlayer->sendPacket(&gcSystemMessage);

		GCNPCResponse response;
		response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
		pPlayer->sendPacket(&response);
		return;
	}

	TPOINT pt;
		
//	pItem1->setGrade( min(7, ItemGradeManager::Instance().getRandomBeadGrade()) );
	Inventory* pInventory = pPC->getInventory();
		
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
		
		if(pPC->isSlayer())
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
			Assert( pSlayer != NULL );

			SLAYER_RECORD prev;
			pSlayer->getSlayerRecord( prev );

			pPC->setContributePoint( 0 ); // 기여도 포인트를 0으로 

			//					pSlayer->initAllStat();
			pSlayer->sendModifyInfo(prev);
		}
		else if(pPC->isVampire())
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
			Assert( pVampire != NULL );

			VAMPIRE_RECORD prev;
			pVampire->getVampireRecord( prev );

			pPC->setContributePoint( 0 ); // 기여도 포인트를 0으로 

			//					pVampire->initAllStat();
			pVampire->sendModifyInfo( prev );
		}
		else if(pPC->isOusters())
		{
			Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
			Assert( pOusters != NULL );

			OUSTERS_RECORD prev;
			pOusters->getOustersRecord( prev );

			pPC->setContributePoint( 0 ); // 기여도 포인트를 0으로 

			//					pOusters->initAllStat();
			pOusters->sendModifyInfo( prev );
		}
		
		//smallheart - 080926 Blitz관련 기여도 보상 횟수 저장
		if ( g_pVariableManager->getVariable(SAVE_CONTRIBUTE_REWARDCOUNT))
		{
			Statement* pStmt = NULL;
			BEGIN_DB
			{
				pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
	
				Result* pResult = pStmt->executeQuery( "SELECT RewardCount FROM ContributeRewardCount WHERE OwnerID = '%s'",
						pPC->getName().c_str() );
				
				int RewardCount = 0;
				if(pResult->next())
				{
					RewardCount = pResult->getInt(1);
					RewardCount += 1;
					pStmt->executeQuery("UPDATE ContributeRewardCount SET RewardCount = %d WHERE OwnerID = '%s'",
							RewardCount, pPC->getName().c_str());
				}
				else
				{
					RewardCount = 1;
					int Race=0;
					if (pPC->isVampire()) Race = 1;
					else if(pPC->isOusters()) Race = 2;
					
					pStmt->executeQuery("INSERT INTO ContributeRewardCount (RewardCount, Race, OwnerID) VALUES (%d, %d, '%s')",
							RewardCount, Race, pPC->getName().c_str());
				}
	
				SAFE_DELETE(pStmt);
			}
			END_DB(pStmt)
		}

		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setMessage( g_pStringPool->getString( STRID_TRADE_SUCCESS ) );
		pPlayer->sendPacket(&gcSystemMessage);
			
	}
	else
	{
		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setMessage( g_pStringPool->getString( STRID_NOT_ENOUGH_INVENTORY_SPACE ) );
		pPlayer->sendPacket(&gcSystemMessage);
	}

	GCNPCResponse response;
	response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );

	pPlayer->sendPacket(&response);
	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionGiveContributeReward::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionGiveContributeReward("
	    << ")";
	return msg.toString();

	__END_CATCH
}
