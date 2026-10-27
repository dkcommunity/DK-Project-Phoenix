//////////////////////////////////////////////////////////////////////////////
// Filename    : EventTransport.cpp
// Written by  : Reiot
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EventTransport.h"
#include "GamePlayer.h"
#include "Zone.h"
#include "PlayerStatus.h"
#include "ZoneGroup.h"
#include "ZonePlayerManager.h"
#include "PlayerStatus.h"
#include "ZoneUtil.h"
#include "PlayerCreature.h"
#include "RelicUtil.h"
#include "CreatureUtil.h"
#include "StringPool.h"
	
#include "Gpackets/GCSystemMessage.h"

#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////
// class EventTransport member methods
//////////////////////////////////////////////////////////////////////////////

EventTransport::EventTransport(GamePlayer* pGamePlayer) 
	throw()
: Event(pGamePlayer)
{
//	m_pTransportZone = NULL;
//	m_X = m_Y = 0;
}

EventTransport::~EventTransport() 
	throw()
{
}

//----------------------------------------------------------------------
// affect
//----------------------------------------------------------------------
void EventTransport::sendMessage()
	throw(Error)
{
	__BEGIN_TRY

	Assert(m_pGamePlayer != NULL);
	Creature * pCreature = m_pGamePlayer->getCreature();

	Timeval currentTime;
	getCurrentTime(currentTime);

	Turn_t       RemainTime = max(0, (int)(m_Deadline.tv_sec-currentTime.tv_sec));

//	StringStream msg;
//	msg << (int)RemainTime << "초 후에 " << m_ZoneName << "로 이동됩니다.";

    char msg[50];
    sprintf( msg, g_pStringPool->c_str( STRID_TRANSPORT_CREATURE ),
                    (int)RemainTime,
                    m_ZoneName.c_str() );

    string sMsg( msg );

	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setMessage(sMsg);
	pCreature->getPlayer()->sendPacket( &gcSystemMessage );

	//setNextTime( m_MessageTick );

	__END_CATCH
}


void EventTransport::activate () 
	throw (Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG


	Assert(m_pGamePlayer != NULL);

	Creature * pCreature = m_pGamePlayer->getCreature();

	Zone* pNewZone = validateTransportTarget("EventTransport::activate", pCreature, m_ZoneID, m_X, m_Y);
	if (pNewZone == NULL)
	{
		return;
	}

	Zone* pOldZone = pCreature->getZone();
	Assert(pOldZone != NULL);

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Assert(pPC!=NULL);

	if ( !pNewZone->NotifyBeforeEnterZone(pCreature) )
	{
		filelog("transportCreatureError.log", "[EventTransport::activate] NotifyBeforeEnterZone rejected player=%s current=[%d,(%d,%d)] target=[%d,(%d,%d)]", pCreature->getName().c_str(), (int)pOldZone->getZoneID(), (int)pCreature->getX(), (int)pCreature->getY(), (int)m_ZoneID, (int)m_X, (int)m_Y);
		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setMessage( g_pStringPool->getString( STRID_CANNOT_ENTER ) );
		pCreature->getPlayer()->sendPacket( &gcSystemMessage );
		return;
	}
	
	bool bSendPacket = true;
	dropRelicToZone( pPC, bSendPacket );
	dropFlagToZone( pPC, bSendPacket );
	dropSweeperToZone( pPC, bSendPacket );

	// Zone에서 제거한다.
	pOldZone->deleteCreature( pCreature, pCreature->getX(), pCreature->getY() );

	pOldZone->NotifyBeforeLeaveZone(pCreature);
	
	// 이동해야할 Zone을 설정한다.
	pCreature->setNewZone( pNewZone );
	pCreature->setNewXY( m_X, m_Y );
	
	try 
	{
		// 존그룹의 ZPM에서 플레이어를 삭제한다.
		pOldZone->getZoneGroup()->getZonePlayerManager()->deletePlayer(m_pGamePlayer->getSocket()->getSOCKET());

		// 여기서 설정해줘야지만 Save 이벤트가 IPM에서 동작하지 않는다.
		m_pGamePlayer->setPlayerStatus(GPS_WAITING_FOR_CG_READY);

		// IPM으로 플레이어를 옮긴다.
		//g_pIncomingPlayerManager->pushPlayer(m_pGamePlayer);
		pOldZone->getZoneGroup()->getZonePlayerManager()->pushOutPlayer(m_pGamePlayer);
		
		pOldZone->NotifyAfterLeaveZone(pCreature);

	} 
	catch (NoSuchElementException& t) 
	{
		filelog("eventTransport.txt", "%s", t.toString().c_str());
		cerr << "EventTransport::activate() : NoSuchElementException" << endl;
		throw Error("존에 플레이어가 존재하지 않습니다.");
	}


	__END_DEBUG
	__END_CATCH
}

string EventTransport::toString () const 
	throw ()
{
	StringStream msg;
	msg << "EventTransport("
		<< ")";
	return msg.toString();
}
