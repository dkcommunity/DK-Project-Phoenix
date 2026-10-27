//////////////////////////////////////////////////////////////////////////////
// Filename    : CLSelectServerHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CLSelectServer.h"

#ifdef __LOGIN_SERVER__
	#include "Assert.h"
	#include "LoginPlayer.h"
	#include "DatabaseManager.h"
	#include "GameServerInfoManager.h"
	#include "DB.h"
	#include "GameServerGroupInfoManager.h"
	#include "GameWorldInfoManager.h"
	#include "OptionInfo.h"

	#include "UserInfoManager.h"

	#include "Lpackets/LCPCList.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// 클라이언트가 PC 의 리스트를 달라고 요청해오면, 로그인 서버는 DB로부터
// PC들의 정보를 로딩해서 LCPCList 패킷에 담아서 전송한다.
//////////////////////////////////////////////////////////////////////////////
void CLSelectServerHandler::execute (CLSelectServer* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__

//	cout << " /////////////////// CLSelectServerHandler::execute TEST BEGIN /////////////////// " << endl;

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);

	ServerGroupID_t CurrentServerGroupID = pPacket->getServerGroupID();

	WorldID_t WorldID = pLoginPlayer->getWorldID();

	//Assert ( WorldID <= g_pGameWorldInfoManager->getSize() );
	int MaxWorldID = g_pGameWorldInfoManager->getSize();
	if (WorldID>MaxWorldID)
	{
		WorldID = MaxWorldID;
	}

	//Assert ( CurrentServerGroupID <= g_pGameServerGroupInfoManager->getSize( WorldID ) );
	int MaxServerGroupID = g_pGameServerGroupInfoManager->getSize( WorldID );
	if  (CurrentServerGroupID>MaxServerGroupID)
	{
		CurrentServerGroupID = MaxServerGroupID;
	}


	// by sigi. 2003.1.7
	GameServerGroupInfo* pGameServerGroupInfo 
		= g_pGameServerGroupInfoManager->getGameServerGroupInfo( CurrentServerGroupID, WorldID );

	Assert(pGameServerGroupInfo!=NULL);
	if (pGameServerGroupInfo->getStat() == SERVER_DOWN)
	{
		filelog("errorLogin.txt", "Server Closed: %d", CurrentServerGroupID);
		throw DisconnectException("ServerClosed");
	}

/*	cout << " /////////////////////// CLSelectServerHandler::execute  RACE NUM TEST  BEGIN ////////////////////////" << endl;

    UserInfo * pUserInfo = g_pUserInfoManager->getUserInfo( CurrentServerGroupID, WorldID );
	vector<uint> RaceTmp = pUserInfo->getRaceNum();

	RaceTmp[0] = 50;
	RaceTmp[1] = 50;
	RaceTmp[2] = 50;

	cout << "getRaceNum(S) : " << RaceTmp[0] << endl;
	cout << "getRaceNum(B) : " << RaceTmp[1] << endl;
	cout << "getRaceNum(O) : " << RaceTmp[2] << endl;

	cout << " /////////////////////// CLSelectServerHandler::execute  RACE NUM TEST  END /////////////////////////" << endl;
*/
	pLoginPlayer->setServerGroupID(CurrentServerGroupID);

	//----------------------------------------------------------------------
	// 이제 LCPCList 패킷을 만들어 보내자
	//----------------------------------------------------------------------
	LCPCList lcPCList;
	pLoginPlayer->makePCList( lcPCList );

#ifdef __NETMARBLE_SERVER__
	// 넷마블 사용자 약관 동의 여부 확인
	lcPCList.setAgree( pLoginPlayer->isAgree() );
#endif

	pLoginPlayer->sendPacket(&lcPCList);
	pLoginPlayer->setPlayerStatus(LPS_PC_MANAGEMENT);

/*	try
	{
		pStmt    = g_pDatabaseManager->getConnection( "DARKEDEN" )->createStatement();	

		pStmt->executeQuery("UPDATE Player set CurrentServerGroupID = %d WHERE PlayerID = '%s'", (int)pPacket->getServerGroupID(), pLoginPlayer->getID().c_str());

		// 쿼리 결과 및 쿼리문 객체를 삭제한다.
		SAFE_DELETE(pStmt);
	}
	catch (SQLQueryException & sce) 
	{
		//cout << sce.toString() << endl;

		// 쿼리 결과 및 쿼리문 객체를 삭제한다.
		SAFE_DELETE(pStmt);

		throw DisconnectException(sce.toString());
	}*/

//	cout << " /////////////////// CLSelectServerHandler::execute TEST END //////////////////// " << endl;

#endif

	__END_DEBUG_EX __END_CATCH
}
