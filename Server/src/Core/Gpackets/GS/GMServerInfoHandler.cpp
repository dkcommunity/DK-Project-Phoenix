//----------------------------------------------------------------------
//
// Filename    : GMServerInfoHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "GMServerInfo.h"
#include "Properties.h"

#include <vector>

#ifdef __LOGIN_SERVER__
	#include "UserInfoManager.h"
	#include "Assert.h"
#endif


//----------------------------------------------------------------------
// 
// GMServerInfoHander::execute()
// 
// 게임 서버가 로그인 서버로부터 GMServerInfo 패킷을 받게 되면,
// ReconnectLoginInfo를 새로 추가하게 된다.
// 
//----------------------------------------------------------------------
void GMServerInfoHandler::execute ( GMServerInfo * pPacket )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
	__BEGIN_DEBUG

#ifdef __LOGIN_SERVER__

//	cout << "////////////  GMServerInfoHandler::execute TEST BEGIN ////////////" << endl;

	ZONEUSERDATA rData;
	WorldID_t WorldID = pPacket->getWorldID();
	ServerGroupID_t ServerGroupID = pPacket->getServerID();
	BYTE MaxCount = pPacket->getZoneUserCount();

	UserInfo * pUserInfo = g_pUserInfoManager->getUserInfo( ServerGroupID, WorldID );

	pUserInfo->setUserNum(0);

	vector<uint> ZeroVector;
	ZeroVector.push_back(0);
	ZeroVector.push_back(0);
	ZeroVector.push_back(0);

  	pUserInfo->setRaceNum(ZeroVector);     

    Assert( pUserInfo != NULL );
    for( int count = 0; count < MaxCount; count++ ) 
	{
		pPacket->popZoneUserData( rData );
        pUserInfo->setUserNum( pUserInfo->getUserNum() + rData.UserNum );
		
		vector<uint> RaceBuf = pUserInfo->getRaceNum();
		vector<uint> RaceTmp = RaceBuf;
		RaceTmp[0] += rData.RaceNum[0];
		RaceTmp[1] += rData.RaceNum[1];
		RaceTmp[2] += rData.RaceNum[2];

//		cout << " RaceTmp[0] : " << RaceTmp[0] << "    RaceTmp[1] : " << RaceTmp[1] << "    RaceTmp[2] : " << RaceTmp[2] << endl;

		pUserInfo->setRaceNum( RaceTmp );

		RaceTmp.clear();
		RaceBuf.clear();
	}
	

//	cout << "////////////  GMServerInfoHandler::execute TEST END ////////////" << endl;

#endif


	__END_DEBUG
	__END_DEBUG_EX __END_CATCH
}
