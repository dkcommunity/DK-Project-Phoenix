//////////////////////////////////////////////////////////////////////
//
// Filename    : ClientManager.cpp
// Written by  : reiot@ewestsoft.com
// Description : 로그인 서버용 클라이언트 매니저
//
//////////////////////////////////////////////////////////////////////

#include "DB.h"
#include "ClientManager.h"
#include "Assert.h"
#include "LoginPlayerManager.h"
#include "ReconnectLoginInfoManager.h"
#include "Profile.h"
#include "PacketProfile.h"
#include "Timeval.h"
#include "Properties.h"

#include "GameWorldInfoManager.h"
#include "GameServerGroupInfoManager.h"

#ifdef __THAILAND_SERVER__
	
	#include "TimeChecker.h"

#endif
#define _STLP_USE_STATIC_LIB 


namespace {

bool isProfileOutputEnabled()
{
	return g_pConfig!=NULL && g_pConfig->hasKey("ProfileOutput") && g_pConfig->getPropertyInt("ProfileOutput")!=0;
}

string getProfileOutputDir()
{
	if (g_pConfig!=NULL && g_pConfig->hasKey("ProfileOutputDir"))
		return g_pConfig->getProperty("ProfileOutputDir");

	return "log/profile";
}

uint getProfileMaxBytes()
{
	if (g_pConfig!=NULL && g_pConfig->hasKey("ProfileMaxBytes"))
		return static_cast<uint>(g_pConfig->getPropertyInt("ProfileMaxBytes"));

	return 10 * 1024 * 1024;
}

}

//////////////////////////////////////////////////////////////////////
//
// constructor
//
// 하위 매니저 객체를 생성하는 곳이다.
//
//////////////////////////////////////////////////////////////////////
ClientManager::ClientManager () 
	throw ( Error )
{
	__BEGIN_TRY

	// 로그인 플레이어 매니저를 생성한다.
	g_pLoginPlayerManager = new LoginPlayerManager();

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// destructor
//
// 하위 매니저 객체를 삭제하는 곳이다.
//
//////////////////////////////////////////////////////////////////////
ClientManager::~ClientManager () 
	throw ( Error )
{
	__BEGIN_TRY

	// 로그인 플레이어 매니저를 삭제한다.
	if ( g_pLoginPlayerManager != NULL ) {
		delete g_pLoginPlayerManager;
		g_pLoginPlayerManager = NULL;	
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// 하위 매니저 객체를 초기화하고, 자신을 초기화한다.
//
//////////////////////////////////////////////////////////////////////
void ClientManager::init ()
	throw ( Error )
{
	__BEGIN_TRY

	g_pLoginPlayerManager->init();

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// 서비스를 시작한다.
//
//////////////////////////////////////////////////////////////////////
void ClientManager::start ()
	throw ( Error )
{
	__BEGIN_TRY

	run();		// 바로 run() 메쏘드를 호출한다. ^^;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// 자신의 서비스를 중단한 후, 하위 매니저 객체들의 서비스를 중단시킨다.
//
//////////////////////////////////////////////////////////////////////
void ClientManager::stop ()
	throw ( Error )
{
	__BEGIN_TRY

	throw UnsupportedError("stopping manager not supported.");

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// 클라이언트 매니저의 메인 루프이다.
//////////////////////////////////////////////////////////////////////
void ClientManager::run ()
	throw ( Error )
{
	__BEGIN_TRY

	Assert( g_pLoginPlayerManager != NULL );

	Timeval NextTime;
	getCurrentTime(NextTime);

	// GameWorldInfo, GameServerInfo 새로 로드하는 시간
	Timeval ReloadNextTime = NextTime;
	// GameWorldInfo, GameServerInfo 새로 로드하는 시간 간격, 분 단위
	int ReloadGap = g_pConfig->getPropertyInt("ServerInfoReloadTime") * 60;
	ReloadNextTime.tv_sec += ReloadGap;

	NextTime.tv_sec += 10;

	Timeval dummyQueryTime;
	getCurrentTime(dummyQueryTime);

	while ( true ) 
	{
		usleep(100);

		beginProfileEx("LS_MAIN");

		beginProfileEx("LPM_SELECT");
		g_pLoginPlayerManager->select();
		endProfileEx("LPM_SELECT");

		beginProfileEx("LPM_EXCEPTION");
		g_pLoginPlayerManager->processExceptions();
		endProfileEx("LPM_EXCEPTION");

		beginProfileEx("LPM_INPUT");
		g_pLoginPlayerManager->processInputs();
		endProfileEx("LPM_INPUT");

		beginProfileEx("LPM_COMMAND");
		g_pLoginPlayerManager->processCommands();
		endProfileEx("LPM_COMMAND");

		beginProfileEx("LPM_OUTPUT");
		g_pLoginPlayerManager->processOutputs();
		endProfileEx("LPM_OUTPUT");

		beginProfileEx("LPM_HEARTBEAT");
		g_pReconnectLoginInfoManager->heartbeat();
		endProfileEx("LPM_HEARTBEAT");

		endProfileEx("LS_MAIN");

		Timeval currentTime;
		getCurrentTime(currentTime);

		if (NextTime < currentTime)
		{
			if (isProfileOutputEnabled())
			{
				string profileOutputDir = getProfileOutputDir();
				(g_ProfileSampleManager.getProfileSampleSet())->outputProfileToFile("Profile", false, false, NULL,
														 profileOutputDir.c_str(), getProfileMaxBytes());
			}

			NextTime.tv_sec = currentTime.tv_sec + 10;
			NextTime.tv_usec = currentTime.tv_usec;

			// 매턴마다 프로파일 데이터를 초기화해준다.
			// 누적 데이터보다는 시간대에 따른 시간을 측정하기 위해서...
			initProfileEx();

			//g_PacketProfileManager.outputResultToFile("PacketProfile.txt");
			g_PacketProfileManager.init();
		}

		if ( ReloadNextTime < currentTime )
		{
			if ( g_pGameWorldInfoManager != NULL )
			{
				g_pGameWorldInfoManager->load();
			}

			if ( g_pGameServerGroupInfoManager != NULL )
			{
				g_pGameServerGroupInfoManager->load();
			}

			ReloadNextTime.tv_sec += ReloadGap;
		}

		// DB connection 이 timeout 으로 끊어지지 않게 가끔씩 의미없는 쿼리함해준다.
		// by bezz. 2003.04.21
		if ( dummyQueryTime < currentTime )
		{
			dbDummyQuery();

			dummyQueryTime.tv_sec += (60+rand()%30) * 60;
		}

#ifdef __THAILAND_SERVER__
	
		g_pTimeChecker->heartbeat();
#endif
		
	}

	__END_CATCH
}

// DB 더미 쿼리. DB 타임아웃 방지를 위한 Dummy query
void ClientManager::dbDummyQuery()
	throw ( Error )
{
	__BEGIN_TRY

	Statement* pStmt = NULL;

	// World Game DB Connection
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection( "DARKEDEN" )->createStatement();
		Result* pResult = pStmt->executeQuery("SELECT WorldID FROM WorldDBInfo WHERE WorldID <> 0");

		while ( pResult->next() )
		{
			WorldID_t worldID = pResult->getInt(1);

			g_pDatabaseManager->executeDummyQuery( g_pDatabaseManager->getConnection( worldID ) );

			cout << "dummyQuery : " << (int)worldID << endl;
		}

		SAFE_DELETE( pStmt );
	}
	END_DB( pStmt )

	// User Info Connection
	g_pDatabaseManager->executeDummyQuery( g_pDatabaseManager->getUserInfoConnection() );

	__END_CATCH
}

// global variable definition
ClientManager * g_pClientManager = NULL;
