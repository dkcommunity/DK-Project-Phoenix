////////////////////////////////////////////////////////////////////////////////
// Filename    : EventSurvivalZoneManager.h 
// Written By  : 쉭
// Description : 
////////////////////////////////////////////////////////////////////////////////


#include "EventSurvivalZoneManager.h"
#include "Assert.h"
#include "Zone.h"
#include "VariableManager.h"
#include "Timeval.h"
#include "Monster.h"
#include "MonsterAI.h"
#include "MonsterManager.h"
#include "PlayerCreature.h"
#include "Inventory.h"
#include "PCManager.h"
#include "Item.h"
#include "ItemUtil.h"
#include "ItemFactoryManager.h"
#include "PacketUtil.h"
#include "Player.h"
#include "MonsterCorpse.h"
#include "ZoneInfoManager.h"
#include "ZoneGroupManager.h"
#include "VSDateTime.h"
#include "StringPool.h"
#include "EventTransport.h"
#include "GamePlayer.h"
#include "MasterLairInfoManager.h"
#include "ItemGradeManager.h"
#include "EffectEdgeArch.h"

#include "Gpackets/GCNoticeEvent.h"
#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCDeleteObject.h"
#include "Gpackets/GCAddEffect.h"

#include <stdio.h>

const int SurvivalLiveMessageMinuteMax = 13;
const int SurvivalLiveMessageMinute[SurvivalLiveMessageMinuteMax] =
{ 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 5, 0 };

const int EdgearchMessageMinuteMax = 3;
const int EdgearchMessageMinute[EdgearchMessageMinuteMax] = 
{ 10, 5, 3 };

const int SurvivalEnterMessageMinuteMax = 2;
const int SurvivalEnterMessageMinute[SurvivalEnterMessageMinuteMax] =
{ 5, 1 };

enum effectEdgearchStep  
{
	WAITING_EFFECT,		// 0
	ADD_EFFECT,			// 1	
	ERASE_EFFECT,		// 2
	STAND_BY_EFFECT,	// 3
};
////////////////////////////////////////////////////////////////////////////////
//
// constructor
//
////////////////////////////////////////////////////////////////////////////////
EventSurvivalZoneManager::EventSurvivalZoneManager (Zone* pZone) 
	throw (Error)
{
	__BEGIN_TRY
		
	Assert(pZone != NULL);
	m_pZone = pZone;

	m_nMaxPassPlayer = 100;
	m_nPassPlayer = 0;

	m_State = STATE_CLOSE;

	Timeval currentTime;
	getCurrentTime(currentTime);

	m_StateTime.tv_sec = currentTime.tv_sec;
	m_StateTime.tv_usec = 0;

	m_SurvivalLiveMessageStep = SurvivalLiveMessageMinuteMax;
	//m_EdgearchMessageStep = EdgearchMessageMinuteMax;
	m_EdgearchMessageStep = WAITING_EFFECT;
	m_SurvivalEnterMessageStep = SurvivalEnterMessageMinuteMax;

	setStateClose();
	m_bStartEvent = false;

	// 2007 01 08
	m_bRace = false;
	m_Race = 0;
	// 기본 30분 후에 발동
	m_StandByEffectTime.tv_sec = currentTime.tv_sec + 30 * 60;
	m_StandByEffectTime.tv_usec = 0;
	m_StandByEffectStep = 0;

	m_Mutex.setName("EventSurvivalZoneManager");

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// destructor
//
////////////////////////////////////////////////////////////////////////////////
EventSurvivalZoneManager::~EventSurvivalZoneManager () 
	throw ()
{
	__BEGIN_TRY
	__END_CATCH
}
	
////////////////////////////////////////////////////////////////////////////////
//
// enterCreature ( Creature* )
//
////////////////////////////////////////////////////////////////////////////////
//
// Creature가 이 Zone(EventSurvivalZone)에 들어올 수 있는지 체크하고
// 들어올 수 있다면 들어왔다고 보고 체크해둔다.
//
// [조건]
//   - STATE_TANTACLE_COMBAT
//   - m_nPassPlayer < m_nMaxPassPlayer
//   - 봉인석을 가지고 있는경우
//   - 100 레벨 이상
//
////////////////////////////////////////////////////////////////////////////////
bool EventSurvivalZoneManager::enterPC(PlayerCreature* pPC )
	throw(Error)
{	
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)

	if (pPC->isDM() || pPC->isGOD())
	{
		m_Mutex.unlock();
			
		filelog("EventSurvivalZone.log", "enterPC - DM or GOD pass! - %s", pPC->getName().c_str() );
		return true;
	}

	if ( m_State != STATE_OPEN )
	{
		m_Mutex.unlock();
		return false;
	}

	// 인원 제한
	if (m_nPassPlayer >= m_nMaxPassPlayer )
	{
		m_Mutex.unlock();

		filelog("EventSurvivalZone.log", "enterPC - over limited pass. deny! - %s", pPC->getName().c_str() );
		return false;
	}

	if( m_bRace &&  m_Race != pPC->getRace() )
	{
		m_Mutex.unlock();

		filelog("EventSurvivalZone.log", "enterPC - not race. deny! - %s", pPC->getName().c_str() );
		return false;
	}

	// 들어올 수 있다고 판단된 경우
	++m_nPassPlayer;

	filelog("EventSurvivalZone.log", "enterPC - pass! - PassPlayerNum : %d", m_nPassPlayer );

	__LEAVE_CRITICAL_SECTION(m_Mutex)

	__END_CATCH

	return true;
}
////////////////////////////////////////////////////////////////////////////////
//
// leaveCreature ( Creature* )
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool EventSurvivalZoneManager::leaveCreature(Creature* pCreature)
	throw(Error)
{	
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)

	if (pCreature->isDM() || pCreature->isGOD())
	{
		m_Mutex.unlock();
		return true;
	}

	--m_nPassPlayer;

	filelog("EventSurvivalZone.log", "leavePC - pass! - PassPlayerNum : %d", m_nPassPlayer );

	__LEAVE_CRITICAL_SECTION(m_Mutex)

	__END_CATCH

	return true;
}


////////////////////////////////////////////////////////////////////////////////
//
// heartbeat
// 
////////////////////////////////////////////////////////////////////////////////
bool EventSurvivalZoneManager::heartbeat() 
	throw (Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)

	switch (m_State)
	{
		case STATE_CLOSE:
			processCloseState();
		break;

		case STATE_OPEN:
			processOpenState();
		break;
		
		case STATE_START:
			processStartState();
		break;

		default :
			break;
	}; 

	__LEAVE_CRITICAL_SECTION(m_Mutex)

	__END_CATCH

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
// process Close State
//
////////////////////////////////////////////////////////////////////////////////
void EventSurvivalZoneManager::processCloseState()
	throw (Error)
{
	__BEGIN_TRY

	if( m_bStartEvent )
	{
		Timeval	currentTime;
		getCurrentTime( currentTime );

		m_State = STATE_OPEN;
		m_StateTime.tv_sec = currentTime.tv_sec + 10 * 60;		// 입장 가능 시간 10분

		// 서바이벌 존 오픈 메시지 발송
		if( m_bRace )
		{
			char msg[100];

			if( m_Race == 0 )
			{
				sprintf(msg, g_pStringPool->c_str(STRID_OPEN_SLAYER_TOP_SURVIVAL_ZONE) ); // 20070814
			}
			else if( m_Race == 1 )
			{
				sprintf(msg, g_pStringPool->c_str(STRID_OPEN_VAMPIRE_TOP_SURVIVAL_ZONE), szRace); // 20070814
			}
			else if( m_Race == 2 )
			{
				sprintf(msg, g_pStringPool->c_str(STRID_OPEN_OUSTERS_TOP_SURVIVAL_ZONE), szRace); // 20070814
			}

			GCSystemMessage	gcSystemMessage;
			gcSystemMessage.setType( SYSTEM_MESSAGE_INFO );
			gcSystemMessage.setMessage( msg );
			g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
			gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_CAN_ENTER_SURVIVAL_ZONE_TEN_MINUTE) ); // 20070814
			g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
		}
		else
		{
			GCSystemMessage	gcSystemMessage;
			gcSystemMessage.setType( SYSTEM_MESSAGE_INFO );
			gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_OPEN_SURVIVAL_ZONE) ); // 20070814
			g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
			gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_CAN_ENTER_SURVIVAL_ZONE_TEN_MINUTE) ); // 20070814
			g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
		}

		// 입장 하여 피케이 불가 지역으로 설정해 준다.
		m_pZone->setPKZone( false );
		fixTimeband( g_pVariableManager->getVariable( RACE_WAR_TIMEBAND ) );
		resetAllSafeZone();
	}

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// process Open State
//
////////////////////////////////////////////////////////////////////////////////
void EventSurvivalZoneManager::processOpenState()
	throw (Error)
{
	__BEGIN_TRY

	Timeval currentTime;
	getCurrentTime(currentTime);

	unsigned int openLimitTime	=  (m_StateTime.tv_sec - currentTime.tv_sec) / 60;

	if( SurvivalEnterMessageMinute[m_SurvivalEnterMessageStep] == openLimitTime )
	{
		char message[256];
		sprintf(message, g_pStringPool->c_str(STRID_REMAIN_TIME_CAN_ENTER_SURVIVAL_ZONE), openLimitTime); // 20070814
		GCSystemMessage	gcSystemMessage;
		gcSystemMessage.setType( SYSTEM_MESSAGE_INFO );
		gcSystemMessage.setMessage( message );
		g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
		++m_SurvivalEnterMessageStep;
	}

	if ( currentTime >= m_StateTime )
	{
		// 피케이 가능 지역으로 변경해 준다.
		m_pZone->setPKZone( true );
		// 서바이벌을 시작한다.
		m_State = STATE_START;
		m_StateTime.tv_sec = currentTime.tv_sec + 60 * 60;		// 1시간 동안 서바이벌을 진행한다.
		m_StandByEffectTime.tv_sec = currentTime.tv_sec + 30 * 60;		// 30분후 에지아크가 발동 하도록 기본 설정한다.

		GCSystemMessage	gcSystemMessage;
		gcSystemMessage.setType( SYSTEM_MESSAGE_INFO );
		gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_START_SURVIVAL) ); // 20070814 
		g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
		gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_CANNOT_ENTER_SURVIVAL_ZONE_NOW) ); // 20070814
		g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
		m_SurvivalLiveMessageStep= 0;
		m_EdgearchMessageStep = WAITING_EFFECT;
		m_bStartEvent = false;
		releaseAllSafeZone();
	}

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// Process Start State
//
////////////////////////////////////////////////////////////////////////////////
void EventSurvivalZoneManager::processStartState()
	throw (Error)
{
	__BEGIN_TRY

	Timeval	currentTime;
	getCurrentTime( currentTime );

	int effectRemain = ( (m_StateTime.tv_sec - currentTime.tv_sec ) - (m_StandByEffectTime.tv_sec - currentTime.tv_sec ) ) / 60;

	if( effectRemain <= 19 && m_EdgearchMessageStep != STAND_BY_EFFECT )
	{
		m_EdgearchMessageStep = STAND_BY_EFFECT;
	}

	// 2007 01 08 운영팀 요청으로 수동으로 발동 시킨다.
	if ( m_EdgearchMessageStep == ADD_EFFECT )
	{
		const PCManager* pPCManager = m_pZone->getPCManager();
		const map< ObjectID_t, Creature* > & pZoneCreature = pPCManager->getCreatures();
		map< ObjectID_t, Creature* >::const_iterator iter = pZoneCreature.begin();

		GCAddEffect gcAddEffect;
		for(; iter != pZoneCreature.end(); ++iter)
		{
			Creature* pTargetCreature = iter->second;
			EffectEdgeArch* pEffect = new EffectEdgeArch( pTargetCreature );
			pEffect->setTick(20);
			pEffect->setDeadline(18600);

			pTargetCreature->setFlag( Effect::EFFECT_CLASS_EDGE_ARCH );
			pTargetCreature->addEffect(pEffect);

			gcAddEffect.setObjectID( pTargetCreature->getObjectID() );
			gcAddEffect.setEffectID( Effect::EFFECT_CLASS_EDGE_ARCH );
			gcAddEffect.setDuration(18600);

			m_pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);
			pEffect->setNextTime(20);
		}
		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setType( SYSTEM_MESSAGE_INFO );
		gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_BIG_DAMAGE_FAR_CENTER_NOW)); // 20070814
		g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );

		m_EdgearchMessageStep = WAITING_EFFECT;
	}
	else if ( m_EdgearchMessageStep == ERASE_EFFECT )
	{
		const PCManager* pPCManager = m_pZone->getPCManager();
		const map< ObjectID_t, Creature* > & pZoneCreature = pPCManager->getCreatures();
		map< ObjectID_t, Creature* >::const_iterator iter = pZoneCreature.begin();

		GCAddEffect gcAddEffect;
		for(; iter != pZoneCreature.end(); ++iter)
		{
			Creature* pTargetCreature = iter->second;
			Effect* pEdgeArch = pTargetCreature->findEffect( Effect::EFFECT_CLASS_EDGE_ARCH );
			if( pEdgeArch != NULL )
			{
				pEdgeArch->setDeadline(0);
			}
		}
		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setType( SYSTEM_MESSAGE_INFO );
		gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_CLOSED_EDGE_ARC) ); // 20070814
		g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );

		m_StandByEffectStep = 0;
		m_StandByEffectTime.tv_sec = currentTime.tv_sec;
		m_EdgearchMessageStep = WAITING_EFFECT;
	}
	else if ( m_EdgearchMessageStep == STAND_BY_EFFECT )
	{
		int EdgearchRemainMinute = ( m_StandByEffectTime.tv_sec - currentTime.tv_sec ) / 60;

		if( EdgearchMessageMinuteMax <= m_StandByEffectStep && EdgearchRemainMinute <= 0 )
		{
			m_EdgearchMessageStep = ADD_EFFECT;
			return;
		}

		if( !(m_StandByEffectStep >= EdgearchMessageMinuteMax) && EdgearchRemainMinute < EdgearchMessageMinute[m_StandByEffectStep] )
		{
			++m_StandByEffectStep;
			return;
		}

		if( EdgearchRemainMinute != 0 && EdgearchRemainMinute <= EdgearchMessageMinute[m_StandByEffectStep] )
		{
			broadcastEdgearchMessage( EdgearchRemainMinute );
			++m_StandByEffectStep;
		}
	}
/*
	int EdgearchRemainMinute = ( m_StateTime.tv_sec - currentTime.tv_sec ) / 60;

	if( m_EdgearchMessageStep >= 0 && m_EdgearchMessageStep < EdgearchMessageMinuteMax)
	{
		if( EdgearchMessageMinute[m_EdgearchMessageStep] == EdgearchRemainMinute )
		{
			broadcastEdgearchMessage( EdgearchRemainMinute - 30 );
			++m_EdgearchMessageStep;
		}
		if ( m_EdgearchMessageStep == EdgearchMessageMinuteMax )
		{
			// 에지아크 이펙트 추가
			const PCManager* pPCManager = m_pZone->getPCManager();
			const map< ObjectID_t, Creature* > & pZoneCreature = pPCManager->getCreatures();
			map< ObjectID_t, Creature* >::const_iterator iter = pZoneCreature.begin();

			GCAddEffect gcAddEffect;
			for(; iter != pZoneCreature.end(); ++iter)
			{
				Creature* pTargetCreature = iter->second;
				EffectEdgeArch* pEffect = new EffectEdgeArch( pTargetCreature );
				pEffect->setTick(20);
				pEffect->setDeadline(18600);

				pTargetCreature->setFlag( Effect::EFFECT_CLASS_EDGE_ARCH );
				pTargetCreature->addEffect(pEffect);

				gcAddEffect.setObjectID( pTargetCreature->getObjectID() );
				gcAddEffect.setEffectID( Effect::EFFECT_CLASS_EDGE_ARCH );
				gcAddEffect.setDuration(18600);

				m_pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);
				pEffect->setNextTime(20);
			}
			++m_EdgearchMessageStep;
		}
	}
*/
/*	if( m_SurvivalLiveMessageStep >= 0 && m_SurvivalLiveMessageStep < SurvivalLiveMessageMinuteMax )
	{
		if( SurvivalLiveMessageMinute[m_SurvivalLiveMessageStep] == EdgearchRemainMinute )
		{
			broadcastSurvivalLiveMessage( m_nPassPlayer );
			++m_SurvivalLiveMessageStep;
		}
	}
*/
	if( currentTime >= m_StateTime )
	{
		// 피케이가 가능하지 않게 변경해준다.
		m_pZone->setPKZone( false );
		m_bStartEvent = false;
		m_State = STATE_CLOSE;
		m_StandByEffectStep = 0;
		resetAllSafeZone();
		resumeTimeband();

		m_bRace = false;

		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setType( SYSTEM_MESSAGE_INFO );
		gcSystemMessage.setMessage(g_pStringPool->c_str(STRID_END_SURVIVAL) ); // 20070814
		g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
	}

	__END_CATCH
}

void EventSurvivalZoneManager::broadcastEdgearchMessage( int minute )
	throw (Error)
{
	__BEGIN_TRY
	char msg[256];
/*	if( minute == 0 )
	{
		sprintf( msg, "지금부터 경기장 중앙에서 멀어질수록 큰 데미지를 입습니다.");
	}
	else  */
	sprintf( msg, g_pStringPool->c_str(STRID_AFTER_MINUTE_BIG_DAMAGE_FAR_CENTER_EDGE_ARC), minute); // 20070814

	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setType( SYSTEM_MESSAGE_INFO );
	gcSystemMessage.setMessage( msg );
	g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
	__END_CATCH
}

void EventSurvivalZoneManager::broadcastSurvivalLiveMessage( int pc )
	throw (Error)
{
	__BEGIN_TRY
	char msg[256];
	sprintf( msg, g_pStringPool->c_str(STRID_LIVE_PLAYER_NOW), pc); // 20070814 

	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setType( SYSTEM_MESSAGE_INFO );
	gcSystemMessage.setMessage( msg );
	g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// kick out all PC
////////////////////////////////////////////////////////////////////////////////
void EventSurvivalZoneManager::kickOutAllPC()
	throw (Error)
{
	__BEGIN_TRY

	m_pZone->getPCManager()->transportAllCreatures( 1129, 63, 63, defaultRaceValue, 10 );

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// set State Close
////////////////////////////////////////////////////////////////////////////////
void EventSurvivalZoneManager::setStateClose()
	throw (Error)
{
	__BEGIN_TRY

	__END_CATCH
}

string EventSurvivalZoneManager::toString() const 
	throw(Error)
{
	StringStream msg;

	switch (m_State)
	{
		case STATE_CLOSE:
			msg << "STATE_CLOSE, ";
		break;

		case STATE_OPEN:
			msg << "STATE_OPEN, ";
		break;

		case STATE_START:
			msg << "STATE_START, ";
		break;

		default :
		break;
	}

	Timeval currentTime;
	getCurrentTime(currentTime);

	int timeGap = m_StateTime.tv_sec - currentTime.tv_sec;

	msg << timeGap << " sec remain, " << (int)m_pZone->getPCManager()->getSize() << " players";

	return msg.toString();
}

void EventSurvivalZoneManager::fixTimeband( uint timeband )
	throw (Error)
{
	__BEGIN_TRY
	m_pZone->stopTime();
	m_pZone->setTimeband( timeband );
	m_pZone->resetDarkLightInfo();
	__END_CATCH
}

void EventSurvivalZoneManager::resumeTimeband()
	throw (Error) 
{
	__BEGIN_TRY
	m_pZone->resumeTime();
	m_pZone->resetDarkLightInfo();
	__END_CATCH

}

void EventSurvivalZoneManager::releaseAllSafeZone()
	throw (Error)
{
	__BEGIN_TRY
	m_pZone->releaseSafeZone();
	__END_CATCH
}

void EventSurvivalZoneManager::resetAllSafeZone()
	throw (Error)
{
	__BEGIN_TRY
	m_pZone->resetSafeZone();
	__END_CATCH
}

void EventSurvivalZoneManager::startEvent()
	throw(Error)
{
	m_State = STATE_CLOSE;
	m_bStartEvent = true;

}
