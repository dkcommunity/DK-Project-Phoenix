//////////////////////////////////////////////////////////////////////////////
// Filename    : EventShutdown.cpp
// Written by  : bezz
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EventShutdown.h"
#include "ZoneGroupManager.h"
#include "ZonePlayerManager.h"
#include "IncomingPlayerManager.h"
#include "BillingInfo.h"
#include "VariableManager.h"

#include "signal.h"

#ifdef __CONNECT_BILLING_SYSTEM__
	#include "billing/BillingPlayerManager.h"
#endif

#include "DB.h"
#include "DBExecuteManager.h"
//////////////////////////////////////////////////////////////////////////////
// class EventShutdown member methods
//////////////////////////////////////////////////////////////////////////////

EventShutdown::EventShutdown( GamePlayer* pGamePlayer )
	throw ()
	:Event(pGamePlayer)
{
}

void EventShutdown::activate () 
	throw (Error)
{
	__BEGIN_TRY

	cout << "Activate EventShutdown Event" << endl;
	
	cout << "Disconnect All Players" << endl;
	
	try
	{
		const map<ZoneGroupID_t, ZoneGroup*>& zoneGroups = g_pZoneGroupManager->getZoneGroups();
		map<ZoneGroupID_t, ZoneGroup*>::const_iterator itr = zoneGroups.begin();

		for ( ; itr != zoneGroups.end(); itr++ )
		{
			ZonePlayerManager* pZonePlayerManager = itr->second->getZonePlayerManager();

			pZonePlayerManager->clearPlayers();
		}

		g_pIncomingPlayerManager->clearPlayers();
	}
	catch ( Throwable& t )
	{
		// 무시
	}

#ifdef __CONNECT_BILLING_SYSTEM__
	// 모든 빌링 정보를 삭제한다.
	g_pBillingPlayerManager->sendPayInit();
#endif

	cout << "Terminate GameDBExecuteManager......." << endl;
	g_GameDBExecuteManager.Term();
	
	cout << "Waiting a second for Shutdown Processes" << endl;
	g_bRunning = false;
	
	// 2008.12.19 rappi76
	// LoginServerManager는 recvfrom()으로 인해 종료 되어 지지 않는다.
	// ZoneGroupThread는 Mutex가 unlock() 되어지지 않아 완전히 종료되어지지 않는다. 하나가 종료되면서 lock이 제대로 안 풀리는 듯 싶다.
	
	kill( getpid(), 9 );
	
	__END_CATCH
}

string EventShutdown::toString () const 
	throw ()
{
	StringStream msg;
	msg << "EventShutdown("
		<< ")";
	return msg.toString();
}
