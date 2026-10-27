//////////////////////////////////////////////////////////////////////////////
// Filename    : MasterLairManager.h 
// Written by  : 쉭
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __FIELD_RAID_MANAGER_H__
#define __FIELD_RAID_MANAGER_H__

#include "MonsterCounter.h"
#include "Item.h"
#include "Timeval.h"
#include <map>
#include <vector>
#include "Mutex.h"

//////////////////////////////////////////////////////////////////////////////
// class OniblaLairManager
//////////////////////////////////////////////////////////////////////////////

class Zone;

class FieldRaidManager
{
public : 
	enum MasterLairEvent 
	{ 
		EVENT_WAITING_REGEN,      // 다시 리젠되길 기다린다. 
		EVENT_WAITING_PLAYER,     // 사람들이 들어오길 기다린다. 
		EVENT_MONSTER_WAR,      // 소환된 몬스터와 싸운다. 
		EVENT_MINION_COMBAT,      // 소환된 몬스터가 다 죽었는지 확인한다.
		EVENT_MAX
	}; 


public:
	FieldRaidManager(Zone* pZone) throw(Error);
	~FieldRaidManager() throw();

	MasterLairEvent getCurrentEvent() const { return m_Event; }

	bool heartbeat() throw (Error);

	void startEvent() throw (Error);
	void stopEvent() throw (Error);

	string toString() const throw(Error);

protected :
	void processEventWaitingRegen() throw (Error);
	void processEventWaitingPlayer() throw (Error);
	void processEventMonsterWar() throw (Error);
	void processEventMinionCombat() throw (Error);

	void activeEventWaitingPlayer() throw (Error);
	void activeEventMinionCombat() throw (Error);
	void activeEventWaitingRegen() throw (Error);

	void deleteAllMonsters() throw (Error);		// 모든 몬스터 삭제
	void giveKillingReward() throw (Error);		// 마스터 죽였을 때 받는 보상
private : 
	Zone*             m_pZone;  
	ObjectID_t        m_MasterID;           // 마스터는 총 3마리다.
	ZoneCoord_t       m_MasterX;
	ZoneCoord_t       m_MasterY;
	MasterLairEvent   m_Event;             // 현재의 이벤트 종류 
	Timeval           m_EventTime;         // 현재의 이벤트가 지속될 시간
	int               m_EventValue;		   // 이벤트와 관련된 값

	Timeval           m_RegenTime;         // 싹 정리하고 다시 시작하는 시간 

	mutable Mutex m_Mutex;
};

#endif
