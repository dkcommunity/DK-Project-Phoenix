//////////////////////////////////////////////////////////////////////////////
// Filename    : CastleDungeonMonsterManager.cpp
// Written by  : bezz
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CastleDungeonMonsterManager.h"
#include "ZoneUtil.h"
#include "Zone.h"
#include "MonsterManager.h"

void CastleDungeonMonsterManager::checkAndRegenGuardianLeader( ZoneID_t zoneID )
	throw (Error)
{
	__BEGIN_TRY

	Zone* pZone1 = getZoneByZoneID( zoneID );
	Assert( pZone1 != NULL );
	Assert( pZone1->isCastleZone() );

	if ( pZone1->getMonsterManager()->isAllEventMonsterNotExist() )
	{
		Zone* pZone2 = getZoneByZoneID( zoneID + 1 );
		Assert( pZone2 != NULL );
		Assert( pZone2->isCastleZone() );

		pZone2->getMonsterManager()->setAppearEventMonster( 814 );
	}

	__END_CATCH
}

