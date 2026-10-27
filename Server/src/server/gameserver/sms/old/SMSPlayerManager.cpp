//////////////////////////////////////////////////////////////////////
// Filename    : SMSPlayerManager.cpp
// Description :
//////////////////////////////////////////////////////////////////////

// include files
#include "SMSPlayerManager.h"
#include "SMSPlayer.h"
#include "CommonSMSPacket.h"
#include "Properties.h"
#include "LogClient.h"
#include "Assert.h"

#include <unistd.h>
//#include "DB.h"
#include "Timeval.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
SMSPlayerManager::SMSPlayerManager () 
	throw (Error)
{
	__BEGIN_TRY

	m_pSMSPlayer = NULL;

	m_Mutex.setName( "SMSPlayerManager" );

	m_bForceDisconnect = false;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
SMSPlayerManager::~SMSPlayerManager () 
	throw (Error)
{
	__BEGIN_TRY

	SAFE_DELETE(m_pSMSPlayer);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// stop thread
//////////////////////////////////////////////////////////////////////
void SMSPlayerManager::stop () 
	throw (Error)
{
	__BEGIN_TRY

	throw UnsupportedError();

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// main method
//////////////////////////////////////////////////////////////////////
void SMSPlayerManager::run () 
	throw ()
{
	__BEGIN_TRY
	try {

		Timeval dummyPingTime;
		getCurrentTime( dummyPingTime );

		Timeval disconnectCheckTime;
		getCurrentTime( disconnectCheckTime );

		const string& 	SMSServerIP 	= g_pConfig->getProperty( "SMSServerIP" );
		uint 			SMSServerPort 	= g_pConfig->getPropertyInt( "SMSServerPort" );

		bool bFirstConnection = true;

		while ( true )
		{
			usleep( 100 );

			// 연결되어 있지 않다면 연결을 시도한다.
			if ( m_pSMSPlayer == NULL )
			{
				Socket* pSocket = NULL;

				try
				{
					// create socket
					pSocket = new Socket( SMSServerIP, SMSServerPort );

					// connect
					pSocket->connect();

					// make nonblocking socket
					pSocket->setNonBlocking();

					// make no-linger socket
					pSocket->setLinger(0);

					__ENTER_CRITICAL_SECTION(m_Mutex)
					m_pSMSPlayer = new SMSPlayer( pSocket );
					__LEAVE_CRITICAL_SECTION(m_Mutex)

					pSocket = NULL;

					// 최초의 접속인 경우 Init packet을 보낸다.
					if (bFirstConnection)
					{
					#ifdef __GAME_SERVER__
						sendInit();
					#endif
						bFirstConnection = false;
					}

					cout << "connection to SMSServer established -  " 
						<< SMSServerIP.c_str() << ":" << SMSServerPort << endl;
					filelog(LOGFILE_SMS_PLAYER, "----- connection established(%s:%d) -----", SMSServerIP.c_str(), SMSServerPort);
				} 
				catch ( Throwable& t )
				{
					cout << "connect to SMSServer fail - "
						<< SMSServerIP.c_str() << ":" << SMSServerPort << endl;
					filelog(LOGFILE_SMS_PLAYER, "connection failed(%s:%d)", SMSServerIP.c_str(), SMSServerPort);

					try {
						SAFE_DELETE( pSocket );
					} catch (Throwable& t) {
						filelog(LOGFILE_SMS_PLAYER, "[0]%s", t.toString().c_str());
					}

					__ENTER_CRITICAL_SECTION(m_Mutex)
					
					try {
							SAFE_DELETE( m_pSMSPlayer ); 
					} catch (Throwable& t) {
						filelog(LOGFILE_SMS_PLAYER, "[1]%s", t.toString().c_str());
					}
					__LEAVE_CRITICAL_SECTION(m_Mutex)

					// 다음 접속시도시간
					usleep( 1000000 );	// 1초
				}
			}

			// 소켓이 연결되어 있다면 입출력을 처리한다.
			__ENTER_CRITICAL_SECTION(m_Mutex)

			if ( m_pSMSPlayer != NULL )
			{
				__BEGIN_TRY

				if ( m_pSMSPlayer->getSocket()->getSockError() )
				{
					try {
						SAFE_DELETE( m_pSMSPlayer );
					} catch ( Throwable& t )
					{
						filelog(LOGFILE_SMS_PLAYER, "[1]%s", t.toString().c_str());
					}
				}
				else
				{
					m_pSMSPlayer->processInput();
					m_pSMSPlayer->processOutput();
					m_pSMSPlayer->processCommand();
				}

				__END_CATCH
			}

			__LEAVE_CRITICAL_SECTION(m_Mutex)

			// dummy query
			Timeval currentTime;
			getCurrentTime(currentTime);

			if (dummyPingTime < currentTime)
			{
				// 연결이 끊어지지 않도록 SMS Server 에 Ping 을 보낸다

				// g_pDatabaseManager->executeDummyQuery( pDistConnection );

				// 1시간 ~ 1시간 30분 사이에서 dummy query 시간을 설정한다.
				// timeout이 되지 않게 하기 위해서이다.
				//dummyQueryTime.tv_sec += (60+rand()%30) * 60;

				// 20초 마다 보내달라는 요청이 있었지만 대략 오차 포함 18초로 한다
				dummyPingTime.tv_sec += 18;
			}

		}
	}
	catch (Throwable & t)
	{
		filelog("smsPlayerManager.log", "SMSPlayerManager::run() 4 : %s", t.toString().c_str());

		cerr << t.toString() << endl;
	}
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// send packet to sms server
//////////////////////////////////////////////////////////////////////
void SMSPlayerManager::sendPacket ( Packet* pPacket )
	throw (ProtocolException , Error)
{
	__BEGIN_SMS_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)
		
	if ( m_pSMSPlayer != NULL )
	{
		m_pSMSPlayer->sendPacket( pPacket );
	}

	__LEAVE_CRITICAL_SECTION(m_Mutex)
	
	__END_SMS_CATCH
}

//////////////////////////////////////////////////////////////////////
// 게임 서버가 처음 뜰 때 보낸다.
//////////////////////////////////////////////////////////////////////
void SMSPlayerManager::sendInit()
	throw( ProtocolException, Error )
{
	__BEGIN_SMS_TRY
	__ENTER_CRITICAL_SECTION(m_Mutex)
		
	if ( m_pSMSPlayer != NULL )
	{
		m_pSMSPlayer->sendSMSInit();
		
		// 바로 보내버린다.
		m_pSMSPlayer->processOutput();
	}

	__LEAVE_CRITICAL_SECTION(m_Mutex)
	__END_SMS_CATCH
}

// global variable definition
SMSPlayerManager* g_pSMSPlayerManager = NULL;

