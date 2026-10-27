#include "HolyLandManager.h"
#include "PCManager.h"
#include "PlayerCreature.h"
#include "ShrineInfoManager.h"
#include "Zone.h"

HolyLandManager::HolyLandManager()
	throw ()
{
	__BEGIN_TRY
	__END_CATCH
}

HolyLandManager::~HolyLandManager()
	throw()
{
	__BEGIN_TRY

	clear();

	__END_CATCH
}

void HolyLandManager::addHolyLand( Zone* pZone )
	throw ( DuplicatedException, Error )
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION( m_Mutex )

	HashMapZoneItor itr = m_HolyLands.find( pZone->getZoneID() );

	if ( itr != m_HolyLands.end() )
	{
		throw DuplicatedException();
	}

	m_HolyLands[ pZone->getZoneID() ] = pZone;

	__LEAVE_CRITICAL_SECTION( m_Mutex )
	
	__END_CATCH
}

void HolyLandManager::broadcast( Packet* pPacket ) const
	throw(Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION( m_Mutex )

	HashMapZoneConstItor itr = m_HolyLands.begin();

	for( ; itr != m_HolyLands.end(); itr++ )
	{
		Zone* pZone = itr->second;
		Assert( pZone != NULL );

		pZone->broadcastPacket( pPacket );
	}

	__LEAVE_CRITICAL_SECTION( m_Mutex )

	__END_CATCH
}

/*void HolyLandManager::sendBloodBibleStatus() const 
	throw (Error)
{
	__BEGIN_TRY

	throw Error("Not Supported");

	__ENTER_CRITICAL_SECTION( m_Mutex )

	HashMapZoneConstItor itr = m_HolyLands.begin();

	for( ; itr != m_HolyLands.end(); itr++ )
	{
		Zone* pZone = itr->second;
		Assert( pZone != NULL );

		__ENTER_CRITICAL_SECTION((*pZone))

		const PCManager* pPCManager = pZone->getPCManager();
		const map< ObjectID_t, Creature* > & creatures = pPCManager->getCreatures();
		map< ObjectID_t, Creature* >::const_iterator itr;

		for (itr=creatures.begin(); itr!=creatures.end(); itr++)
		{
			Creature* pCreature = itr->second;
			Assert( pCreature->isPC() );

			PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
			g_pShrineInfoManager->sendBloodBibleStatus( pPC );
		}

		__LEAVE_CRITICAL_SECTION((*pZone))
	}

	__LEAVE_CRITICAL_SECTION( m_Mutex )

	__END_CATCH
}
*/
void HolyLandManager::fixTimeband( uint timeband )
	throw (Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION( m_Mutex )

	HashMapZoneConstItor itr = m_HolyLands.begin();

	for( ; itr != m_HolyLands.end(); itr++ )
	{
		Zone* pZone = itr->second;
		Assert( pZone != NULL );

		pZone->stopTime();
		pZone->setTimeband( timeband );
		pZone->resetDarkLightInfo();
	}

	__LEAVE_CRITICAL_SECTION( m_Mutex )

	__END_CATCH
}

void HolyLandManager::resumeTimeband()
	throw (Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION( m_Mutex )

	HashMapZoneConstItor itr = m_HolyLands.begin();

	for( ; itr != m_HolyLands.end(); itr++ )
	{
		Zone* pZone = itr->second;
		Assert( pZone != NULL );

		pZone->resumeTime();
		pZone->resetDarkLightInfo();
	}

	__LEAVE_CRITICAL_SECTION( m_Mutex )

	__END_CATCH
}


void    HolyLandManager::killAllMonsters() 
	throw (Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION( m_Mutex )

	HashMapZoneConstItor itr = m_HolyLands.begin();

	for( ; itr != m_HolyLands.end(); itr++ )
	{
		Zone* pZone = itr->second;
		Assert( pZone != NULL );

		pZone->killAllMonsters();
	}

	__LEAVE_CRITICAL_SECTION( m_Mutex )


	__END_CATCH
}

void	HolyLandManager::remainRaceWarPlayers()
	throw(Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION( m_Mutex )

	HashMapZoneConstItor itr = m_HolyLands.begin();

	for( ; itr != m_HolyLands.end(); itr++ )
	{
		Zone* pZone = itr->second;
		Assert( pZone != NULL );

		pZone->remainRaceWarPlayers();
	}

	__LEAVE_CRITICAL_SECTION( m_Mutex )

	__END_CATCH
}

void HolyLandManager::refreshHolyLandPlayers()
	throw (Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION( m_Mutex )

	HashMapZoneConstItor itr = m_HolyLands.begin();

	for( ; itr != m_HolyLands.end(); itr++ )
	{
		Zone* pZone = itr->second;
		Assert( pZone != NULL );

		pZone->setRefreshHolyLandPlayer( true );
	}

	__LEAVE_CRITICAL_SECTION( m_Mutex )

	__END_CATCH
}

#ifdef __CONTRIBUTION_SYSTEM__
void HolyLandManager::heartbeat() 
	throw(Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION( m_Mutex )

	VSDateTime dt(VSDateTime::currentDateTime());
	int lastSec = dt.secsTo(m_GiveContributePointTime);
	if( lastSec < 0 )
	{
		setGiveContributeTime(dt.addSecs(10 * 60));

		HashMapZoneConstItor itr = m_HolyLands.begin();

		for( ; itr != m_HolyLands.end(); itr++ )
		{
			Zone* pZone = itr->second;
			Assert( pZone != NULL );

			pZone->giveHolyWarContributePoint();
		}
	}

	__LEAVE_CRITICAL_SECTION( m_Mutex )

	__END_CATCH

}

void HolyLandManager::SendNewWarInfo() 
	throw(Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION( m_Mutex )

		HashMapZoneConstItor itr = m_HolyLands.begin();

		for( ; itr != m_HolyLands.end(); itr++ )
		{
			Zone* pZone = itr->second;
			Assert( pZone != NULL );

			pZone->SendNewWarInfo();
		}

	__LEAVE_CRITICAL_SECTION( m_Mutex )

	__END_CATCH

}
void HolyLandManager::setGiveContributeTime(const VSDateTime &dt) 
	throw(Error)
{
	m_GiveContributePointTime = dt; 
}
#endif

// global variable definition
HolyLandManager* g_pHolyLandManager = NULL;

