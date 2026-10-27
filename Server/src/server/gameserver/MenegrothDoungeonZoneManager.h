////////////////////////////////////////////////////////////////////////////////
// Filename    : MenegrothDoungeonZoneManager.h 
// Written By  : rappi76 
// Description :
////////////////////////////////////////////////////////////////////////////////

#ifndef _MENEGROTH_DOUNGEONE_ZONE_MANAGER_H_
#define _MENEGROTH_DOUNGEONE_ZONE_MANAGER_H_

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__

#include "Creature.h"
#include "Timeval.h"
#include "MenegrothDoungeonManager.h"

typedef	vector<Timeval> TTrapInstallTimeList;

class MenegrothDoungeonZoneManager
{
public :
	MenegrothDoungeonZoneManager(Zone *pZone);

	bool BeReadyToSummonCabracam();
	bool IsAlreadySummonCabracam();
	void SummonCabracam();

	void ResetCabracamHeartToAltar();
	void AddCabracamHeartToAltar(ObjectID_t ObjectID);
	int GetCabracamHeartCountToAltar();
	void giveRewardItemToAllPCInZone();

	void InstallTrap1(ZoneCoord_t trapX, ZoneCoord_t trapY);
	void InstallTrap2(ZoneCoord_t trapX, ZoneCoord_t trapY);

	void heartbeat();

protected :
	Zone *m_pZone;
	Timeval m_nextCabracamRegenTime;

	ObjectID_t m_CabracamObjectID;

	bool m_bReadyToSummon;
	bool m_bSummonCabracam;
	list<ObjectID_t> m_AltarToInsertCabracamHeartList;

	MonsterType_t m_summonMonsterType;
	ZoneCoord_t m_summonX,m_summonY;

	TTrapPosList m_Trap1PosList;
	TTrapPosList m_Trap2PosList;
	TTrapInstallTimeList m_Trap2InstallTimeList;

	Timeval m_PrevTrapCheckTime;

	void calcNextCabracamRegenTime();

};

#endif /* __MENEGROTH_DOUNGEON_SYSTEM__ */

#endif /* _MENEGROTH_DOUNGEONE_ZONE_MANAGER_H_ */
