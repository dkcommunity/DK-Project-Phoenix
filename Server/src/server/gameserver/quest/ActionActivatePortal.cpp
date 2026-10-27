////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionActivatePortal.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ActionActivatePortal.h"
#include "Zone.h"
#include "ZoneGroup.h"
#include "ZoneGroupManager.h"
#include "ZoneInfo.h"
#include "ZoneInfoManager.h"
#include "ZonePlayerManager.h"
#include "PCSlayerInfo2.h"
#include "PCVampireInfo2.h"
#include "Slayer.h"
#include "Vampire.h"
#include "WeatherManager.h"
#include "PaySystem.h"
#include "GamePlayer.h"
#include "IncomingPlayerManager.h"
#include "LogClient.h"
#include "PacketUtil.h"
#include "ZoneUtil.h"
#include "Properties.h"
#include "StringPool.h"

#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"

#include "GQuestManager.h"

#include "Gpackets/GCUpdateInfo.h"
#include "Gpackets/GCMoveOK.h"
#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCNPCResponse.h"
#include "WarSystem.h"
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void ActionActivatePortal::read (PropertyBuffer & pb)
{
    __BEGIN_TRY

	try 
	{
		m_ZoneID = pb.getPropertyInt("ZoneID");
		m_X      = pb.getPropertyInt("X");
		m_Y      = pb.getPropertyInt("Y");
	} 
	catch (NoSuchElementException & nsee)
	{
		throw Error(nsee.toString());
	}
	
    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// 액션을 실행한다.
////////////////////////////////////////////////////////////////////////////////
void ActionActivatePortal::execute (Creature * pNPC , Creature * pCreature)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	Assert(pCreature != NULL);
	Assert(pCreature->isPC());

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());
	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	//Zone* pZone = pCreature->getZone();

	bool bTransport = true;

	if (pGamePlayer == NULL || pPC == NULL)
	{
		filelog("transportCreatureError.log", "[ActionActivatePortal] invalid player state creature=%p gamePlayer=%p pc=%p target=[%d,(%d,%d)]", (void*)pCreature, (void*)pGamePlayer, (void*)pPC, (int)m_ZoneID, (int)m_X, (int)m_Y);
		return;
	}

	ZoneInfo* pZoneInfo = NULL;
	try
	{
		pZoneInfo = g_pZoneInfoManager->getZoneInfo( m_ZoneID );
	}
	catch (Throwable& t)
	{
		filelog("transportCreatureError.log", "[ActionActivatePortal] invalid target ZoneInfo player=%s target=[%d,(%d,%d)] error=%s", pCreature->getName().c_str(), (int)m_ZoneID, (int)m_X, (int)m_Y, t.toString().c_str());
		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setMessage( g_pStringPool->getString( STRID_CANNOT_ENTER ) );
		pGamePlayer->sendPacket( &gcSystemMessage );
		return;
	}

	if (pZoneInfo == NULL)
	{
		filelog("transportCreatureError.log", "[ActionActivatePortal] null target ZoneInfo player=%s target=[%d,(%d,%d)]", pCreature->getName().c_str(), (int)m_ZoneID, (int)m_X, (int)m_Y);
		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setMessage( g_pStringPool->getString( STRID_CANNOT_ENTER ) );
		pGamePlayer->sendPacket( &gcSystemMessage );
		return;
	}

#if defined(__PAY_SYSTEM_ZONE__) || defined(__PAY_SYSTEM_FREE_LIMIT__)
	try {
		// 유료존인데 유료사용자가 아니면...
//		if (pZoneInfo==NULL
//			|| pZoneInfo->isPayPlay() && !pGamePlayer->isPayPlaying())
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//          20080227 유료존 티켓 추가하면서 코드 수정도 함께 한다.
//          기존 isPayPlaying() 은 모든 기간제 사용자는 다 체크하는 거라 유료존 티켓에는 맞지 않는다.
//          어차피 기간제 사용자가 프리미엄 사용자가 되기 땜시 isPremiumPlay로 바꾸고 유료존 티켓체크를 추가한다. isPayZoneTicket()

        if( pZoneInfo != NULL
			&& (pZoneInfo->isPayPlay())
            && ( !pGamePlayer->isPremiumPlay() && !pGamePlayer->isPayZoneTicket() && !pGamePlayer->isFamilyFreePass() ))
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		{
//			string connectIP = pGamePlayer->getSocket()->getHost();

			// 유료 서비스 사용이 가능한가?
//			if (pGamePlayer->loginPayPlay(connectIP, pGamePlayer->getID()))
//			{
//				sendPayInfo(pGamePlayer);
//			}
//			else if ( !pGamePlayer->isFamilyFreePass() ) // 패밀리 프리 패스는 유료존으로 갈 수 있다.
//			{
				// 유료 서비스 사용 불가인 경우
				GCSystemMessage gcSystemMessage;

				if (g_pConfig->getPropertyInt("IsNetMarble")==0)
				{
					gcSystemMessage.setMessage( g_pStringPool->getString( STRID_CANNOT_ENTER_PAY_ZONE) );
				}
				else
				{
					gcSystemMessage.setMessage( g_pStringPool->getString( STRID_CANNOT_ENTER) );
				}

				pGamePlayer->sendPacket (&gcSystemMessage);

				bTransport = false;
//			}
		}
	} catch (Throwable& t) {
		filelog("transportCreatureError.log", "[ActionActivatePortal] pay-zone check failed player=%s target=[%d,(%d,%d)] error=%s", pCreature->getName().c_str(), (int)m_ZoneID, (int)m_X, (int)m_Y, t.toString().c_str());
		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setMessage( g_pStringPool->getString( STRID_CANNOT_ENTER ) );
		pGamePlayer->sendPacket (&gcSystemMessage);
		bTransport = false;
	}
#endif

	if (bTransport || (g_pWarSystem->hasActiveRaceWar() && pZoneInfo->isHolyLand()) )
	{
		// 2007 06 25 HellGarden Add
		if ( m_ZoneID == 1410 || pZoneInfo->isHellGardenTower() )
		{
			if ( pCreature->isFlag( Effect::EFFECT_CLASS_CAN_ENTER_GDR_LAIR ) && m_ZoneID == 1410 )
			{
				Effect* pEffect = pCreature->findEffect(Effect::EFFECT_CLASS_CAN_ENTER_GDR_LAIR);
				if ( pEffect != NULL ) pEffect->setDeadline(0);
			}

			if ( pCreature->isSlayer() )
			{
				Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
				Assert( pSlayer != NULL );

				// 오토바이를 타고 있으면 오토바이에서 내린다.
				if ( pSlayer->hasRideMotorcycle() )
				{
					pSlayer->getOffMotorcycle();
				}
			}

			if ( pCreature->isOusters() )
			{
				Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
				Assert( pOusters != NULL );

				// 실프 타고 있으면 내려준다
				if ( pOusters->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH) )
				{
					Effect* pEffect = pOusters->findEffect(Effect::EFFECT_CLASS_SUMMON_SYLPH);
					if ( pEffect != NULL ) pEffect->setDeadline(0);
				}
			}

			if ( pCreature->isVampire() )
			{
				Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
				Assert( pVampire != NULL );

				if ( pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT) )
				{
					addUntransformCreature( pVampire->getZone(), pVampire, true );
				}
			}
		}

		if ( pNPC != NULL )
		{
			pPC->getGQuestManager()->illegalWarp();
		}

		transportCreature(pCreature, m_ZoneID, m_X, m_Y, true);
	}
	else
	{
	    GCNPCResponse response;
	    response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
	    pGamePlayer->sendPacket(&response);
	}

	__END_DEBUG
	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionActivatePortal::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionActivatePortal("
	    << "ZoneID:" << (int)m_ZoneID
	    << ",X:"  << (int)m_X
	    << ",Y:"  << (int)m_Y
	    << ")";
	return msg.toString();

	__END_CATCH
}

