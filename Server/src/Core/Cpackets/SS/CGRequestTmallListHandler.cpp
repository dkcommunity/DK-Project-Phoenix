//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestGuildListHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGRequestTmallList.h"
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
#include "TmallGoodsInfoManager.h"
#include "Gpackets/GCTMallGoodsList.h"
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
void CGRequestTmallListHandler::execute (CGRequestTmallList* pPacket , Player* pPlayer)
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

	pPC->loadDianJuan();

	GCModifyInformation gcMI;
	gcMI.addLongData( MODIFY_DIANJUAN, pPC->getDianJuan() );
	pGamePlayer->sendPacket( &gcMI );

	pPC->loadJiFen();

	GCModifyInformation gcMIt;
	gcMIt.addLongData( MODIFY_JIFEN, pPC->getJiFen() );
	pGamePlayer->sendPacket( &gcMIt );
////////////////////////////////DB START//////////////////
        GCTMallGoodsList MallGoodsList;
	Statement* pStmt    = NULL;
	Result*    pResult  = NULL;
	
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
		pResult = pStmt->executeQuery("SELECT GoodsID,ItemClass, ItemType, Grade, OptionType, Num, Limited+0, Cash,Point FROM TmallListInfo WHERE (Race='%d' or Race='5') and Kind = '%d'",
		(int)pCreature->getRace(),
		(int)pPacket->getKind()
		);
		while (pResult->next()) 
		{
			TmallGoodsInfo* pGoodsInfo = new TmallGoodsInfo();
			int i = 0;

			pGoodsInfo->objectID =  pResult->getInt(++i);
			pGoodsInfo->itemClass = (Item::ItemClass)(pResult->getInt(++i));
			pGoodsInfo->itemType = pResult->getInt(++i);
			pGoodsInfo->grade = pResult->getInt(++i);
			string optionField = pResult->getString(++i);
			pGoodsInfo->num = pResult->getInt(++i);
			pGoodsInfo->timeLimit = pResult->getInt(++i) == 1; // enum( 'LIMITED'(1), 'UNLIMITED'(2) ) 

			list<OptionType_t> optionTypes;
			setOptionTypeFromField( optionTypes, optionField );
			pGoodsInfo->optionType = optionTypes;

			pGoodsInfo->Cash = pResult->getInt(++i);
			pGoodsInfo->Point = pResult->getInt(++i);

			MallGoodsList.addGoodsInfo(pGoodsInfo);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
/////////////////CLOSE DB/////////////////////////////
        
	//g_pGoodsInfoManager->MakeGoodsList(MallGoodsList,  race, Kind);

	pPlayer->sendPacket( &MallGoodsList);




	
#endif	// __GAME_SERVER__
		
	__END_DEBUG_EX __END_CATCH
}
