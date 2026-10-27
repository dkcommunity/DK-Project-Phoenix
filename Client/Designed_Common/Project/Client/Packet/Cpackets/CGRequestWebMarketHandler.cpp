//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestWebMarketHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGRequestWebMarket.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "PlayerCreature.h"
	#include "DB.h"
	#include "Properties.h"

	#include "Gpackets/GCShowWebMarket.h"
#endif	// __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGRequestWebMarketHandler::execute (CGRequestWebMarket* pPacket , Player* pPlayer)
	 
{
	//__BEGIN_TRY __BEGIN_DEBUG_EX
	__BEGIN_TRY
		
#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert(pGamePlayer != NULL);

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
	Assert( pPC != NULL );

	switch ( pPacket->getCode() )
	{
		case CGRequestWebMarket::REQUEST_WEB_MARKET:
			{
				// 키 생성
				DWORD key = rand() << ( time(0) % 10 ) + rand() >> ( time(0) % 10 );

				std::cout << "Key: " << key << std::endl;

				// 키 저장
				Statement* pStmt = NULL;
				Result* pResult = NULL;
				
				BEGIN_DB
				{
					pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
					pResult = pStmt->executeQuery( "UPDATE WebMarketKey SET Key = %u WHERE PlayerID = '%s'", key, pGamePlayer->getID().c_str() );

					if ( pStmt->getAffectedRowCount() == 0 )
					{
						pStmt->executeQuery( "INSERT INTO WebMarketKey VALUES( '%s', %u )", pGamePlayer->getID().c_str(), key );
					}
				}
				END_DB( pStmt )

				// 월드ID 불러오기
				static int WorldID = g_pConfig->getPropertyInt("Dimension") * 3 + g_pConfig->getPropertyInt("WorldID");

				// 패킷 생성
				GCShowWebMarket gcShowWebMarket;
				gcShowWebMarket.setPlayerID( pGamePlayer->getID() );
				gcShowWebMarket.setName( pPC->getName() );
				gcShowWebMarket.setWorldID( WorldID );
				gcShowWebMarket.setKey( key );

				// 패킷 보내기
				pGamePlayer->sendPacket( &gcShowWebMarket );
			}
			break;

		case CGRequestWebMarket::REQUEST_BUY_COMPLETE:
			{
				// GoodsObject 를 새로 로딩한다
				pPC->loadGoods();
			}
			break;

		default:
			break;
	}

#endif	// __GAME_SERVER__
	__END_CATCH	
	//__END_DEBUG_EX __END_CATCH
}

