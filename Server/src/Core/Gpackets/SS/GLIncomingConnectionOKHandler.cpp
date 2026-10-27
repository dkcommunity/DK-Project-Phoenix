//--------------------------------------------------------------------------------
//
// Filename    : GLIncomingConnectionOKHandler.cpp
// Written By  : Reiot
// Description :
//
//--------------------------------------------------------------------------------

// include files
#include "GLIncomingConnectionOK.h"

#ifdef __LOGIN_SERVER__

	#include "LoginPlayer.h"
	#include "LoginPlayerManager.h"
	#include "Assert.h"

	#include "Lpackets/LCReconnect.h"

	#include "DB.h"	//by viva

#endif

//--------------------------------------------------------------------------------
// 
// GLIncomingConnectionOKHander::execute()
// 
// 게임 서버로부터 GLIncomingConnectionOK 패킷이 날아오면, 로그인 서버는 이 허가가 
// 어느 플레이어에 대한 허가인지 찾아내야 한다. 그 후, 이 플레이어에게 LCReconnect
// 패킷을 던져줘야 한다.
// 
//--------------------------------------------------------------------------------
void GLIncomingConnectionOKHandler::execute ( GLIncomingConnectionOK * pPacket )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__

	try {	

		// 플레이어 아이디를 사용해서 플레이어 객체에 접근한다.
		__ENTER_CRITICAL_SECTION((*g_pLoginPlayerManager))

		LoginPlayer * pLoginPlayer = g_pLoginPlayerManager->getPlayer_NOLOCKED( pPacket->getPlayerID() );

		cout<<"------------->login oky"<<endl;
		if ( pLoginPlayer->getPlayerStatus() == LPS_AFTER_SENDING_LG_INCOMING_CONNECTION ) 
		{
	        // 클라이언트에게 게임 서버로 재접속하라고 알려준다.
			LCReconnect lcReconnect;
			lcReconnect.setGameServerPort( pPacket->getTCPPort() );
			lcReconnect.setKey( pPacket->getKey() );

			//by viva
			Statement* pStmt = NULL;
			Result* pResult;
			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

			pResult = pStmt->executeQuery("SELECT ProxyIP,GamePort FROM ProxyIP WHERE ProxyIP='%s' and GamePort='%d'", pPacket->getClientIP().c_str(), pPacket->getTCPPort());
			if(pResult->next())
				lcReconnect.setGameServerIP( pPacket->getClientIP() );
			else
				lcReconnect.setGameServerIP( pPacket->getHost() );

			pResult = pStmt->executeQuery("SELECT ProxyIP,GamePort FROM ProxyIP WHERE ProxyIP='%s' and GamePort='%d'", pPacket->getHost().c_str(), pPacket->getTCPPort());
			if(pResult->next())
				lcReconnect.setGameServerIP( pPacket->getHost() );
			SAFE_DELETE(pStmt);
			//end

			// LCReconnect 패킷을 전송한다.
			
			pLoginPlayer->sendPacket( &lcReconnect );
cout<<"------------->sendPacket Reconnect oky"<<endl;

			//cout << "GLIncomingConnectionOKHandler Send to Client Required Reconnect ServerIP : " << pPacket->getHost() << endl;
		}
		else
		{
			//cout << "Invalid Player Status.. must be AFTER_SENDING_LG_INCOMING_CONNECTION" << endl;
		}

		// 연결을 종료한다.
		//pLoginPlayer->disconnect( UNDISCONNECTED );
		pLoginPlayer->disconnect_nolog( UNDISCONNECTED );

		// LPM에서 삭제한다.
		g_pLoginPlayerManager->deletePlayer_NOLOCKED( pLoginPlayer->getSocket()->getSOCKET() );

		// LoginPlayer 객체를 삭제한다.
		SAFE_DELETE(pLoginPlayer);

		__LEAVE_CRITICAL_SECTION((*g_pLoginPlayerManager))


	} catch ( NoSuchElementException & nsee ) {
		//cout << "Player not exist or already disconnected" << endl;
	}

#endif
		
	__END_DEBUG_EX __END_CATCH
}
