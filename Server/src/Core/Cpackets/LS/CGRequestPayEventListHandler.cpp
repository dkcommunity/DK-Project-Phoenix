//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestGuildListHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGRequestPayEventList.h"
#include "Creature.h"
#include "NPC.h"
#include "GamePlayer.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Guild.h"
#include "GoodsInfoManager.h"
#include "GuildInfo.h"
#include "Gpackets/GCWaitGuildList.h"
#include "Gpackets/GCPayEventList.h"
#include "Gpackets/GCModifyInformation.h"


#ifdef __GAME_SERVER__
	#include "SystemAvailabilitiesManager.h"
	#include "Assert.h"
	#include "StringPool.h"
	#include "DB.h"
	#include "ItemFactoryManager.h"
	#include "ItemInfoManager.h"
	#include "ItemUtil.h"
#endif	// __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGRequestPayEventListHandler::execute (CGRequestPayEventList* pPacket , Player* pPlayer)
	 throw (Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert( pGamePlayer != NULL );

	Creature* pCreature = pGamePlayer->getCreature();
	Assert( pCreature != NULL );

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Assert( pPC != NULL );

////////////////////////////////DB START//////////////////
        GCPayEventList PayEventList;
	Statement* pStmt    = NULL;
	Result*    pResult  = NULL;

	Statement* pStmt2    = NULL;
	Result*    pResult2  = NULL;
	
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
		pResult = pStmt->executeQuery("SELECT Money,ItemClass, ItemType, Grade, OptionType, Num, Limited+0, ImageID FROM PayItemInfo");
		while (pResult->next()) 
		{
			PayEventInfo* pGoodsInfo = new PayEventInfo();
			int i = 0;

			pGoodsInfo->objectID =  pResult->getInt(++i);//money
			pGoodsInfo->itemClass = (Item::ItemClass)(pResult->getInt(++i));//class
			pGoodsInfo->itemType = pResult->getInt(++i);//type
			pGoodsInfo->grade = pResult->getInt(++i);//grade
			string optionField = pResult->getString(++i);//option
			pGoodsInfo->num = pResult->getInt(++i);//num
			pGoodsInfo->timeLimit = pResult->getInt(++i) == 1; // time

			list<OptionType_t> optionTypes;
			setOptionTypeFromField( optionTypes, optionField );
			pGoodsInfo->optionType = optionTypes;

			pGoodsInfo->ImageID = pResult->getInt(++i);//ImageID

			PayEventList.addPayEventInfo(pGoodsInfo);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
/////////////////CLOSE DB/////////////////////////////
        
	int count20 	= 0;
	int count50 	= 0;
	int count100	= 0;
	int count200	= 0;
	int count500 	= 0;
	int count1000 	= 0;	
	int count2000 	= 0;
	int count5000	= 0;

	int Status20 	= 0;
	int Status50 	= 0;
	int Status100	= 0;
	int Status200	= 0;
	int Status500 	= 0;
	int Status1000 	= 0;	
	int Status2000 	= 0;
	int Status5000	= 0;

	BEGIN_DB
	{
		pStmt2 = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
		pResult2 = pStmt2->executeQuery("SELECT count20,count50, count100, count200, count500, count1000, count2000, count5000, status20, status50, status100, status200, status500, status1000, status2000, status5000 FROM PayLog WHERE PlayerID = '%s'", pGamePlayer->getID().c_str());
		while (pResult2->next()) 
		{
			int i = 0;
			count20		= pResult2->getInt(++i);
			count50 	= pResult2->getInt(++i);
			count100	= pResult2->getInt(++i);
			count200	= pResult2->getInt(++i);
			count500	= pResult2->getInt(++i);
			count1000	= pResult2->getInt(++i);
			count2000	= pResult2->getInt(++i);
			count5000	= pResult2->getInt(++i);

			Status20	= pResult2->getInt(++i);
			Status50 	= pResult2->getInt(++i);
			Status100	= pResult2->getInt(++i);
			Status200	= pResult2->getInt(++i);
			Status500	= pResult2->getInt(++i);
			Status1000	= pResult2->getInt(++i);
			Status2000	= pResult2->getInt(++i);
			Status5000	= pResult2->getInt(++i);
		}
		
		SAFE_DELETE(pStmt2);
	}
	END_DB(pStmt2)


	PayEventList.setCount20(count20);
	PayEventList.setCount50(count50);
	PayEventList.setCount100(count100);
	PayEventList.setCount200(count200);
	PayEventList.setCount500(count500);
	PayEventList.setCount1000(count1000);
	PayEventList.setCount2000(count2000);
	PayEventList.setCount5000(count5000);

	PayEventList.setStatus20(Status20);
	PayEventList.setStatus50(Status50);
	PayEventList.setStatus100(Status100);
	PayEventList.setStatus200(Status200);
	PayEventList.setStatus500(Status500);
	PayEventList.setStatus1000(Status1000);
	PayEventList.setStatus2000(Status2000);
	PayEventList.setStatus5000(Status5000);

	pPlayer->sendPacket( &PayEventList);




	
#endif	// __GAME_SERVER__
		
	__END_DEBUG_EX __END_CATCH
}
