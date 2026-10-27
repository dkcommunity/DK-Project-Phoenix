//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestWebMarket2Handler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGRequestWebMarket2.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "PlayerCreature.h"
	#include "Zone.h"
	#include "DB.h"
	#include "Properties.h"
	#include "GoodsInventory.h"
	#include "VariableManager.h"

	#include "Gpackets/GCShowWebMarket.h"
	#include "Gpackets/GCGoodsList.h"
#endif	// __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGRequestWebMarket2Handler::execute (CGRequestWebMarket2* pPacket , Player* pPlayer)
	 throw (Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert(pGamePlayer != NULL);

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
	Assert( pPC != NULL );

	switch ( pPacket->getCode() )
	{
		/*case CGRequestWebMarket2::REQUEST_WEB_MARKET:
			{
				// 키 생성
				DWORD key = rand() << ( time(0) % 10 ) + rand() >> ( time(0) % 10 );

				// 키 저장
				Statement* pStmt = NULL;
				
				BEGIN_DB
				{
					pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
					pStmt->executeQuery( "REPLACE INTO WebMarketKey VALUES( '%s', %u, 0 )", pGamePlayer->getID().c_str(), key );

					SAFE_DELETE( pStmt );
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
			break;*/

		case CGRequestWebMarket2::REQUEST_BUY_COMPLETE:
			{
				// 키 저장
				Statement* pStmt = NULL;
				
				BEGIN_DB
				{
					pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
					pStmt->executeQuery( "UPDATE WebMarketKey SET Refresh = 0 WHERE PlayerID = '%s'", pGamePlayer->getID().c_str() );

					if ( pStmt->getAffectedRowCount() != 0 )
					{
						// 적용된 필드가 있다는 말을 Refresh 가 0이 아닌 값 즉 GoodsListObject 를 새로 로딩할 필요가 있다는 말이다.
						// WebServer 에서 게임내 마켓 결재시 이 필드의 Refresh 값을 1로 바꾼다.
						// GoodsListObject 를 새로 로딩한다
						pPC->loadGoods();
						pPC->registerGoodsInventory( pPC->getZone()->getObjectRegistry() );
					}

					SAFE_DELETE( pStmt );
				}
				END_DB( pStmt )
			}
			break;

		case CGRequestWebMarket2::REQUEST_GOODS_LIST:
			{
				if ( g_pVariableManager->getVariable( CAN_BUY_SHOP ) == 0 )
				{
					return;
				}

				GoodsInventory::ListItem& iList = pPC->getGoodsInventory()->getGoods();

				GoodsInventory::ListItemItr itr = iList.begin();
				GoodsInventory::ListItemItr endItr = iList.end();

				int count = 0;
				GCGoodsList gcGoodsList;

				for ( ; itr != endItr; ++itr )
				{
					if ( count++ >= MAX_GOODS_LIST ) break;

					Item* pItem = (*itr).m_pItem;

					GoodsInfo* pGI = new GoodsInfo;
					pGI->objectID = pItem->getObjectID();
					pGI->itemClass = (BYTE)pItem->getItemClass();
					pGI->itemType = pItem->getItemType();
					pGI->grade = pItem->getGrade();
					pGI->optionType = pItem->getOptionTypeList();
					pGI->num = pItem->getNum();

					if ( pItem->isTimeLimitItem() )
					{
						if ( pItem->getHour() == 0 ) pGI->timeLimit = 1;
						else pGI->timeLimit = pItem->getHour() * 3600;
					}
					else
					{
						pGI->timeLimit = 0;
					}

					gcGoodsList.addGoodsInfo( pGI );
				}

				pGamePlayer->sendPacket( &gcGoodsList );
			}

			break;

		default:
			break;
	}

#endif	// __GAME_SERVER__
		
	__END_DEBUG_EX __END_CATCH
}

