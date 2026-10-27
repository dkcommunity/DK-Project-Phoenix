//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestRankGemDataHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGRequestRankGemData.h"
#include "Creature.h"
#include "NPC.h"
#include "GamePlayer.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Gpackets/GCSendRankGemData.h"

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
void CGRequestRankGemDataHandler::execute (CGRequestRankGemData* pPacket , Player* pPlayer)
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
	Assert(pPC!=NULL);
////////////////////////////////DB START//////////////////
        GCSendRankGemData SendRankGemData;
	Statement* pStmt    = NULL;
	Result*    pResult  = NULL;
	
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
		pResult = pStmt->executeQuery("SELECT Slot,Type,Grade FROM RankGemData WHERE Name='%s' and Race = '%d'",
		pPC->getName().c_str(),
		(int)pPC->getRace()
		);
		while (pResult->next())
		{
			RankGemData* pRankGemData = new RankGemData();
			int i = 0;

			pRankGemData->Slot =  pResult->getInt(++i);
			pRankGemData->Type =  pResult->getInt(++i);
			pRankGemData->Grade = pResult->getInt(++i);

			SendRankGemData.addRankGemData(pRankGemData);
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
/////////////////CLOSE DB/////////////////////////////

	pPlayer->sendPacket( &SendRankGemData);




	
#endif	// __GAME_SERVER__
		
	__END_DEBUG_EX __END_CATCH
}
