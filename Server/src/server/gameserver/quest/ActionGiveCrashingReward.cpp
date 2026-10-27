////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionGiveCrashingReward.cpp
// Written By  : 
// Description :
// 
// History.
//
//  Date        Writer         Description
// ---------- ----------- ------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////////

#include "ActionGiveCrashingReward.h"
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


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionGiveCrashingReward::read(PropertyBuffer & propertyBuffer)
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
// �׼��� �����Ѵ�.
////////////////////////////////////////////////////////////////////////////////
void ActionGiveCrashingReward::execute(Creature * pCreature1 , Creature * pCreature2)
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

	Inventory* pInventory = pPC->getInventory();
	Assert( pInventory != NULL );

	/*

	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setMessage("���� �������� �ʴ� ����Դϴ�");
	pPlayer->sendPacket(&gcSystemMessage);
	*/

	//cout << "ActionGiveCrashingReward" << ":" << m_Type;


    Statement* pStmt = NULL;
    Result*    pResult = NULL;
	int	IndexID = 0;
	int RewardClass = -1;
	int RewardItemType = -1;
	list<OptionType_t> RewardOptionType;

    // 2026-05-13 UNHANDLED EXCEPTION FIX: ItemList tablosu DARKEDEN DB'sinde yok
    // (CTF event reward sistemi, bu Phoenix kurulumunda schema'da olmayabilir).
    // BEGIN_DB/END_DB makrosu SQLQueryException'i const char* olarak rethrow ediyor
    // (memory: project_db_exception_const_char_rethrow.md). Ust katman catch(Throwable&)
    // bunu yakalayamaz -> worker thread terminate -> UNHANDLED EXCEPTION -> server crash.
    //
    // Surgical defansif sarmal: outer try ile hem const char* hem Throwable yakala,
    // graceful skip yap. Player'a sade mesaj gonder, server hayatta kalsin.
    try {

    BEGIN_DB
    {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        pResult = pStmt->executeQuery("SELECT IndexID, ClassNum, ItemType, OptionType from ItemList where OwnerID = '%s' and GiveFlag=0", pPC->getName().c_str());

        int rowCount = pResult->getRowCount();
        if(rowCount <= 0)
        {
			
			GCSystemMessage gcSystemMessage;
			gcSystemMessage.setMessage("���� ������ �����ϴ�.");
			pPlayer->sendPacket(&gcSystemMessage);

			GCNPCResponse response;
			response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
			pPlayer->sendPacket(&response);
			return;
        }

       	if(pResult->next())
		{
            int IndexID = pResult->getInt(1);
            Item::ItemClass itemClass = (Item::ItemClass)pResult->getInt(2);
            int itemType = pResult->getInt(3);
            string option = pResult->getString(4);
			_TPOINT pt;
			
			std::vector<std::string> Params;
			int iIndex;
			while( true )
			{
				iIndex = option.find(" ");
				if( iIndex <= 0)
				{
					Params.push_back(option);
					break;
				}
				Params.push_back(option.substr(0, iIndex));
				option = option.substr(iIndex + 1);
			}
			
			if(Params[0].compare("NONE") != 0)
			{
				std::vector<std::string>::const_iterator itr;
				for (itr=Params.begin(); itr!=Params.end(); itr++)
				{
					RewardOptionType.push_back(atoi((*itr).c_str()));
				}
			}

			ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo( itemClass, itemType );
			if( !pInventory->getEmptySlot(pItemInfo->getVolumeWidth(), pItemInfo->getVolumeHeight(), pt) )
			{
				GCNPCResponse response;
				response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
				response.setParameter( NOT_ENOUGH_INVENTORY_SPACE );
				pPlayer->sendPacket( &response );
				return;
			}
		
			Item* pItem = g_pItemFactoryManager->createItem( pItemInfo->getItemClass(), pItemInfo->getItemType(), RewardOptionType );
			Assert( pItem != NULL );

			pZone->registerObject( pItem );
		
			if( pInventory->addItem( pItem, pt ) )
			{
				pItem->setGrade( 5 );
				pItem->create( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );
				
		GCCreateItem gcCreateItem;
		makeGCCreateItem(&gcCreateItem, pItem, pt.x, pt.y);

		pPlayer->sendPacket( &gcCreateItem );

			GCSystemMessage gcSystemMessage;
			gcSystemMessage.setMessage("������ ���� �Ǿ����ϴ�.");
			pPlayer->sendPacket(&gcSystemMessage);
			GCNPCResponse response;
			response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
			pPlayer->sendPacket(&response);
				
        		pResult = pStmt->executeQuery("UPDATE ItemList SET GiveFlag = 1 WHERE IndexID = %d AND OwnerID = '%s'", IndexID, pPC->getName().c_str());
	        }
		}
        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt);

    } // outer try
    catch (const char* msg) {
        // END_DB makrosu SQLQueryException'i const char* olarak rethrow eder.
        // Yakala, graceful skip.
        filelog("ActionGiveCrashingReward.log", "DB exception caught: %s", msg ? msg : "(null)");
        GCSystemMessage gcSystemMessage;
        gcSystemMessage.setMessage("Reward system unavailable (DB error)");
        pPlayer->sendPacket(&gcSystemMessage);
        GCNPCResponse response;
        response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
        pPlayer->sendPacket(&response);
    }
    catch (Throwable& t) {
        filelog("ActionGiveCrashingReward.log", "Throwable caught: %s", t.toString().c_str());
        GCSystemMessage gcSystemMessage;
        gcSystemMessage.setMessage("Reward system error");
        pPlayer->sendPacket(&gcSystemMessage);
        GCNPCResponse response;
        response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
        pPlayer->sendPacket(&response);
    }
    catch (...) {
        // Defansif: bilinmeyen exception'lar da yakalanmali, server hayatta kalsin
        filelog("ActionGiveCrashingReward.log", "Unknown exception caught");
        GCNPCResponse response;
        response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
        pPlayer->sendPacket(&response);
    }

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionGiveCrashingReward::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionGiveCrashingReward("
	    << ")";
	return msg.toString();

	__END_CATCH
}
