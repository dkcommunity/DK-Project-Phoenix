//////////////////////////////////////////////////////////////////////////////
// Filename    : ZoneGroupThread.cc
// Written by  : reiot@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "ZoneGroupThread.h"
#include "ZonePlayerManager.h"
#include "LogClient.h"
#include "DB.h"
#include "VSDateTime.h"
#include "Properties.h"
#include "Profile.h"
#include "Timeval.h"
#include "Gpackets/GMServerInfo.h"

#include "VariableManager.h"

//#define __FULL_PROFILE__

#ifndef __FULL_PROFILE__
#undef beginProfileEx
#define beginProfileEx(name) ((void)0)
#undef endProfileEx
#define endProfileEx(name) ((void)0)
#endif


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

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
ZoneGroupThread::ZoneGroupThread (ZoneGroup* pZoneGroup) 
	throw ()
: m_pZoneGroup(pZoneGroup)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
ZoneGroupThread::~ZoneGroupThread () 
	throw ()
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ������ �޽����� �ֻ����� ���ǹǷ� __BEGIN_TRY�� __END_CATCH
// �� �� �ʿ䰡 ����. �� ��� ���ܸ� ��Ƽ� ó���ؾ� �Ѵٴ� �Ҹ�.
//////////////////////////////////////////////////////////////////////////////
void ZoneGroupThread::run () 
	throw ()
{
	__BEGIN_DEBUG

	string host     = g_pConfig->getProperty("DB_HOST");
	string db       = g_pConfig->getProperty("DB_DB");
	string user     = g_pConfig->getProperty("DB_USER");
	string password = g_pConfig->getProperty("DB_PASSWORD");
	uint port		= 0;
	if ( g_pConfig->hasKey("DB_PORT") )
		port = g_pConfig->getPropertyInt("DB_PORT");

	Connection* pConnection = new Connection(host, db, user, password, port);
	g_pDatabaseManager->addConnection((int)Thread::self(), pConnection);
	cout << "******************************************************" << endl;
	cout << " THREAD CONNECT DB " << endl;
	cout << "******************************************************" << endl;

	// ���� �α��ε�� �� DARKEDEN���� �����ϰ� �Ǿ��ִ�. ȥ������ ����!!!
	string dist_host     = g_pConfig->getProperty("UI_DB_HOST");
	string dist_db       = "DARKEDEN";
	string dist_user     = g_pConfig->getProperty("UI_DB_USER");
	string dist_password = g_pConfig->getProperty("UI_DB_PASSWORD");
	uint dist_port		= 0;
	if ( g_pConfig->hasKey("UI_DB_PORT") )
		dist_port = g_pConfig->getPropertyInt("UI_DB_PORT");

	Connection* pDistConnection = new Connection(dist_host, dist_db, dist_user, dist_password, dist_port);
	g_pDatabaseManager->addDistConnection(((int)Thread::self()), pDistConnection);
	cout << "******************************************************" << endl;
	cout << " THREAD CONNECT UIIRIBUTION DB " << endl;
	cout << " TID Number = " << (int)Thread::self()<< endl;
	cout << "******************************************************" << endl;

	/*
	// Login DB �� PCRoomDBInfo Table �о Connection �����
	Statement * pStmt = NULL;
	pStmt = pDistConnection->createStatement();
	Result * pResult = NULL;
	
	pResult = pStmt->executeQuery("SELECT ID, Host, DB, User, Password FROM PCRoomDBInfo");
	
	if (pResult->next())
	{
		WorldID_t ID = pResult->getInt(1);
		string host = pResult->getString(2);
		string db = pResult->getString(3);
		string user = pResult->getString(4);
		string password = pResult->getString(5);

		cout << "Connection: "
			 << "  ID=" << (int)ID
			 << ", HOST=" << host.c_str()
			 << ", DB=" << db.c_str()
			 << ", User=" << user.c_str() << endl;

		Connection * pConnection = new Connection(host, db, user, password);
		Assert(pConnection!=NULL);

		g_pDatabaseManager->addPCRoomConnection((int)(Thread::self()) , pConnection );
	}
	*/

	Timeval NextTime;
	getCurrentTime(NextTime);
	Timeval currentTime;

	NextTime.tv_sec += 2;

	Timeval dummyQueryTime;
	getCurrentTime(dummyQueryTime);

	try {

	while (g_bRunning) 
	{

//		beginProfileEx("ZGT_MAIN");
		try {
		beginProfileExNoTry("ZGT_MAIN");

		// 2026-05-12: Eski usleep(100) = 100 mikrosaniye = 10000 tick/sec
		// idealde 50-100 Hz olmali (player perception 20ms latency). 10000 Hz
		// CPU-bound spin yariyor. 5000 = 5ms (200 Hz) makul: oyun timing'i
		// korunur, CPU yuku ~50x azalir. NPCTalk/move responsiveness bozulmaz.
		usleep(5000);

		__ENTER_CRITICAL_SECTION((*m_pZoneGroup))

		beginProfileEx("ZG_PP");
		m_pZoneGroup->processPlayers(); // process all players in ZonePlayerManager;
		endProfileEx("ZG_PP");

		beginProfileEx("ZG_HEARTBEAT");
		m_pZoneGroup->heartbeat(); // process all npc, monster, ... in Zones
		endProfileEx("ZG_HEARTBEAT");

		__LEAVE_CRITICAL_SECTION((*m_pZoneGroup))

		getCurrentTime(currentTime);

//		endProfileEx("ZGT_MAIN");
		endProfileExNoCatch("ZGT_MAIN");
		} catch (Throwable&) {
		endProfileExNoCatch("ZGT_MAIN");
		throw;
		}

		if (dummyQueryTime < currentTime)
		{
			g_pDatabaseManager->executeDummyQuery( pConnection );
			g_pDatabaseManager->executeDummyQuery( pDistConnection );

			// 15�� ~ 20�� ���̿��� dummy query �ð��� �����Ѵ�.
		    // timeout�� ���� �ʰ� �ϱ� ���ؼ��̴�.
		    dummyQueryTime.tv_sec += (15+rand()%5) * 60;
		}


		if (NextTime < currentTime)
		{

			if (isProfileOutputEnabled())
			{
				GMServerInfo gmServerInfo;
				m_pZoneGroup->makeZoneUserInfo(gmServerInfo); 

				string profileOutputDir = getProfileOutputDir();
				(g_ProfileSampleManager.getProfileSampleSet())->outputProfileToFile("Profile", false, false,
															&gmServerInfo,
															profileOutputDir.c_str(), getProfileMaxBytes());
			}

			
			NextTime.tv_sec = currentTime.tv_sec + 10;
			NextTime.tv_usec = currentTime.tv_usec;

			// ���ϸ��� �������� �����͸� �ʱ�ȭ���ش�.
			// ���� �����ͺ��ٴ� �ð��뿡 ���� �ð��� �����ϱ� ���ؼ�...
			initProfileEx();
		}
	}

	} catch (Throwable& t){
		filelog("zoneGroupThreadError.log", "%s", t.toString().c_str());
		throw;
	}

	__END_DEBUG
}

//////////////////////////////////////////////////////////////////////////////
// get debug string
//////////////////////////////////////////////////////////////////////////////
string ZoneGroupThread::toString () const 
	throw ()
{
	StringStream msg;
	msg << "ZoneGroupThread("
			<< m_pZoneGroup->toString()
			<< ")";
	return msg.toString();
}
