//////////////////////////////////////////////////////////////////////////////
// Filename    : CastleDungeonMonsterManager.h 
// Written by  : bezz
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CASTLE_DUNGEON_MONSTER_MANAGER_H__
#define __CASTLE_DUNGEON_MONSTER_MANAGER_H__

#include "Types.h"
#include "Exception.h"

class CastleDungeonMonsterManager
{
public:
	static void checkAndRegenGuardianLeader( ZoneID_t zoneID ) throw (Error);
};

#endif
