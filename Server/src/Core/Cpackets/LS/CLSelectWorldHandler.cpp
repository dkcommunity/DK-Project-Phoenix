//////////////////////////////////////////////////////////////////////////////
// Filename    : CLSelectWorldHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CLSelectWorld.h"

#ifdef __LOGIN_SERVER__
	#include "Assert.h"
	#include "LoginPlayer.h"
	#include "DatabaseManager.h"
	#include "GameServerInfoManager.h"
	#include "GameWorldInfoManager.h"
	#include "DB.h"
	#include "PCSlayerInfo.h"
	#include "PCVampireInfo.h"
	#include "GameServerGroupInfoManager.h"
	#include "ServerGroupInfo.h"
	#include "UserInfo.h"
	#include "OptionInfo.h"
	#include "UserInfoManager.h"
	#include "Shape.h"
	#include "Lpackets/LCServerList.h"
	#include "Lpackets/LCShowWebPage.h"
	#include "Lpackets/LCSelectWorldError.h"

	#include "LoginPayTypeManager.h"

#endif

//////////////////////////////////////////////////////////////////////////////
// 월드 선택
//////////////////////////////////////////////////////////////////////////////
void CLSelectWorldHandler::execute (CLSelectWorld* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__

//	cout << " ////////////////////////// CLSelectWorldHandler::execute BEGIN //////////////////////////// " << endl;

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);
	//cout << "Start execute" << endl;

	LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);
	WorldID_t WorldID = pPacket->getWorldID();

	cout << pLoginPlayer->getID().c_str() << " - WorldID : " << (int)WorldID << endl;

	if ( !g_pLoginPayTypeManager->IsAllow(WorldID, pLoginPlayer))
	{
		LCSelectWorldError lcSelectWorldError;
		lcSelectWorldError.setErrorID(LCSelectWorldError::ERROR_NOT_PREMIUM_ACCOUNT);
		
		pLoginPlayer->sendPacket( &lcSelectWorldError );
				
		return;
	}
	
	//Assert( WorldID <= g_pGameWorldInfoManager->getSize() );
	// by sigi. 2002.12.20
	if (WorldID > g_pGameWorldInfoManager->getSize() )
	{
		filelog("errorLogin.txt", "WorldID Over[%d/%d]", (int)WorldID, (int)g_pGameWorldInfoManager->getSize());
		throw DisconnectException("WorldID over");
	}

	// close된 상태에서 못 들어오게 막기. by sigi. 2002.1.7
	GameWorldInfo* pGameWorldInfo = g_pGameWorldInfoManager->getGameWorldInfo(WorldID);
	if (pGameWorldInfo->getStatus()==WORLD_CLOSE)
	{
		filelog("errorLogin.txt", "WorldClosed[%d]", (int)WorldID);
		throw DisconnectException("WorldClosed");
	}

	// 트랜실(2) 빼기
	//if (WorldID==2) throw DisconnectException();

	pLoginPlayer->setWorldID( WorldID );

	Statement * pStmt = NULL;

/*	BEGIN_DB
	{
		pStmt    = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		//cout << "PlayerID: " << pLoginPlayer->getID() << endl;
		pStmt->executeQuery("UPDATE Player set CurrentWorldID = %d WHERE PlayerID = '%s'", WorldID , pLoginPlayer->getID().c_str());

		SAFE_DELETE(pStmt); // by sigi
	}
	END_DB(pStmt)*/

	try 
	{
		int GroupNum = g_pGameServerGroupInfoManager->getSize(WorldID);

		//cout << "WorldNum : " << (int)GroupNum << endl;

		ServerGroupInfo* aServerGroupInfo[GroupNum];

		for(int i = 0 ; i < GroupNum; i++) 
		{
			ServerGroupInfo* pServerGroupInfo = new ServerGroupInfo();
			GameServerGroupInfo* pGameServerGroupInfo = g_pGameServerGroupInfoManager->getGameServerGroupInfo(i, WorldID );
			pServerGroupInfo->setGroupID(pGameServerGroupInfo->getGroupID());
			pServerGroupInfo->setGroupName(pGameServerGroupInfo->getGroupName());
			pServerGroupInfo->setNonPKServer(pGameServerGroupInfo->isNonPKServer());
			//pServerGroupInfo->setStat(SERVER_FREE);

			UserInfo * pUserInfo = g_pUserInfoManager->getUserInfo( pGameServerGroupInfo->getGroupID(), WorldID );
			//by Kim Sung Yong 2008. 02. 14
			GameServerInfo* pGameServerInfo = g_pGameServerInfoManager->getGameServerInfo( 1, pGameServerGroupInfo->getGroupID(), WorldID );  

		

			vector<uint> RaceTmp = pUserInfo->getRaceNum();
			   
//			RaceTmp[0] = 50;
//			RaceTmp[1] = 50;
//			RaceTmp[2] = 50;

//			cout << " aaaa : " << RaceTmp[0] << endl;
//			cout << " bbbb : " << RaceTmp[1] << endl;
//			cout << " cccc : " << RaceTmp[2] << endl;

			WORD SlayerNum = RaceTmp[0];
			WORD VampireNum = RaceTmp[1];
			WORD OustersNum = RaceTmp[2];

			pServerGroupInfo->setSlayerNum( SlayerNum );
			pServerGroupInfo->setVampireNum( VampireNum );
			pServerGroupInfo->setOustersNum( OustersNum );
//			cout << "getRaceNum(S) : " << RaceTmp[0] << endl;
//			cout << "getRaceNum(B) : " << RaceTmp[1] << endl;
//			cout << "getRaceNum(O) : " << RaceTmp[2] << endl;


/*
			WORD UserModify = 800;
#ifdef __CHINA_SERVER__
			WORD UserMax = 1800;
#else
			WORD UserMax = 1500;
#endif

			if (pUserInfo->getUserNum() < 100 + UserModify )
			{
				pServerGroupInfo->setStat(SERVER_FREE);
			}
			else if (pUserInfo->getUserNum() < 250 + UserModify )
			{
				pServerGroupInfo->setStat(SERVER_NORMAL);
			}
			else if (pUserInfo->getUserNum() < 400 + UserModify )
			{
				pServerGroupInfo->setStat(SERVER_BUSY);
			}
#ifdef __CHINA_SERVER__
			else if (pUserInfo->getUserNum() < 1000 + UserModify )
#else
			else if (pUserInfo->getUserNum() < 500 + UserModify )
#endif
			{
				pServerGroupInfo->setStat(SERVER_VERY_BUSY);
			}
			else //if (pUserInfo->getUserNum() >= 500 + UserModify )
			{
				pServerGroupInfo->setStat(SERVER_FULL);
			}
			//else
			{
			//pServerGroupInfo->setStat(SERVER_DOWN);
			}

			if (pUserInfo->getUserNum() >= UserMax)
			{
				pServerGroupInfo->setStat(SERVER_FULL);
			}

			if ( pGameServerGroupInfo->getStat() == SERVER_DOWN )
			{
				pServerGroupInfo->setStat( SERVER_DOWN );
			}

			aServerGroupInfo[i] = pServerGroupInfo;

			//cout << "AddServer : " << pServerGroupInfo->getGroupName() << endl;
		}
*/
#ifdef __CHINA_SERVER__
			WORD UserMax = 1800;
#else
			WORD UserMax = pGameServerInfo->getMaxUser();
			//cout << "kim sung yong !!!!!!!!!!!!!!!!!!!  >>>>>>>>>>> " << (int)UserMax << endl;
#endif
			//cout << "pUserInfo->getUserNum() =======  " << pUserInfo->getUserNum() << endl;
			//cout << "pUserInfo->getUserNum() * 100 / UserMax  =======  " << pUserInfo->getUserNum() * 100 / UserMax << endl;
			if ((pUserInfo->getUserNum() * 100 / UserMax ) <= 25 )
			{
				pServerGroupInfo->setStat(SERVER_FREE);
				//cout << "SERVER FREE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << endl;
			}
			else if ( pUserInfo->getUserNum() * 100 / UserMax > 25 && pUserInfo->getUserNum() * 100 / UserMax <= 50 )
			{
				pServerGroupInfo->setStat(SERVER_NORMAL);
				//cout << "SERVER_NORMAL !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << endl;
			}
				else if ( pUserInfo->getUserNum() * 100 / UserMax > 50 && pUserInfo->getUserNum() * 100 / UserMax <= 75 )
			{
				pServerGroupInfo->setStat(SERVER_BUSY);
				//cout << "SERVER_BUSY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << endl;
	  	    }
#ifdef __CHINA_SERVER__
//			else if (pUserInfo->getUserNum() < 1000 + UserModify )
#else
			else if ( pUserInfo->getUserNum() * 100 / UserMax > 75 && pUserInfo->getUserNum() * 100 / UserMax < 100 )
#endif
			{
				pServerGroupInfo->setStat(SERVER_VERY_BUSY);
				//cout << "SERVER_VERY_BUSY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << endl; 
			}
			else//if (pUserInfo->getUserNum() >= 500 + UserModify )
			{
				pServerGroupInfo->setStat(SERVER_FULL);
				//cout << "SERVER_FULL !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << endl;
			}
			//else
			{
				//pServerGroupInfo->setStat(SERVER_DOWN);
			}

			if (pUserInfo->getUserNum() >= UserMax)
			{
				pServerGroupInfo->setStat(SERVER_FULL);
			}

			if ( pGameServerGroupInfo->getStat() == SERVER_DOWN )
			{
				pServerGroupInfo->setStat( SERVER_DOWN );
			}

				aServerGroupInfo[i] = pServerGroupInfo;

				//cout << "AddServer : " << pServerGroupInfo->getGroupName() << endl;
		}


		LCServerList lcServerList;

		pStmt = NULL;
		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

			Result * pResult = pStmt->executeQuery("SELECT CurrentServerGroupID FROM Player where PlayerID='%s'" , pLoginPlayer->getID().c_str() );

			if( pResult->next() ) {
				lcServerList.setCurrentServerGroupID( pResult->getInt(1) );
			}

			SAFE_DELETE(pStmt);	// by sigi -_-
		}
		END_DB(pStmt)	// by sigi T_T

		bool bUnderFifthteen = false;

		// 2007 04 23 15세 미만은 논피케이 서버만 접속이 가능하게 한다.
#ifdef __METRO_SERVER__
		if( pLoginPlayer->isUnderFifthteen() )
		{
			bUnderFifthteen = true;
		}
#endif

/*		cout << " /////////////////////// CLSelectWorldHandler::execute  RACE NUM TEST  BEGIN ////////////////////////" << endl;
		 
		UserInfo * pUserInfo = g_pUserInfoManager->getUserInfo( CurrentServerGroupID, WorldID );
		vector<uint> RaceTmp = pUserInfo->getRaceNum();
		    
		RaceTmp[0] = 50;
	    RaceTmp[1] = 50;
	    RaceTmp[2] = 50;
			  
	    cout << "getRaceNum(S) : " << RaceTmp[0] << endl;
        cout << "getRaceNum(B) : " << RaceTmp[1] << endl;
		cout << "getRaceNum(O) : " << RaceTmp[2] << endl;
				  
		cout << " /////////////////////// CLSelectWorldHandler::execute  RACE NUM TEST  END /////////////////////////" << endl;

*/
		for(int k = 0; k < GroupNum; k++) 
		{
/*			if( bUnderFifthteen ) // 테섭은 전체 이용가다. 이건 본섭만 
			{
				if( aServerGroupInfo[k]->isNonPKServer() )
				{
					lcServerList.addListElement(aServerGroupInfo[k]);
				}
			}
//			else
			else*/ // 테섭은 여기까지 주석처리해야한다. 
			{
				lcServerList.addListElement(aServerGroupInfo[k]);
			}
		}

		pLoginPlayer->sendPacket(&lcServerList);
	
//		pLoginPlayer->setPlayerStatus(LPS_PC_MANAGEMENT);

	} 
	catch (Throwable & t) 
	{
		//cout << t.toString() << endl;
	}
	//cout << "End execute" << endl;

//	cout << " ////////////////////////// CLSelectWorldHandler::execute END //////////////////////////// " << endl;
#endif

	__END_DEBUG_EX __END_CATCH
}

