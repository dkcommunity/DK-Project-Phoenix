////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionActivateMazeEnter.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ActionActivateMazeEnter.h"
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
#include "EffectRefiniumTicket.h"

#include "Gpackets/GCUpdateInfo.h"
#include "Gpackets/GCMoveOK.h"
#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCNPCResponse.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void ActionActivateMazeEnter::read (PropertyBuffer & pb)
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
void ActionActivateMazeEnter::execute (Creature * pNPC , Creature * pCreature)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	Assert(pCreature != NULL);
	Assert(pCreature->isPC());

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());
	//Zone* pZone = pCreature->getZone();

	bool bTransport = true;

#if defined(__PAY_SYSTEM_ZONE__) || defined(__PAY_SYSTEM_FREE_LIMIT__)
	try {

		ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo( m_ZoneID );

		// 유료존인데 유료사용자가 아니면...
//		if (pZoneInfo==NULL
//			|| pZoneInfo->isPayPlay() && !pGamePlayer->isPayPlaying())
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//          20080227 유료존 티켓 추가하면서 코드 수정도 함께 한다.
//          기존 isPayPlaying() 은 모든 기간제 사용자는 다 체크하는 거라 유료존 티켓에는 맞지 않는다.
//          어차피 기간제 사용자가 프리미엄 사용자가 되기 땜시 isPremiumPlay로 바꾸고 유료존 티켓체크를 추가한다. isPayZoneTicket()
        if( (pZoneInfo != NULL)
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
	} catch (NoSuchElementException&) {
	}
#endif

	if (bTransport)
	{
		EffectRefiniumTicket* pEffect = new EffectRefiniumTicket( pCreature );
		pEffect->setExit( rand()%6 );

		pCreature->setFlag( pEffect->getEffectClass() );
		pCreature->addEffect(pEffect);
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
string ActionActivateMazeEnter::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionActivateMazeEnter("
	    << "ZoneID:" << (int)m_ZoneID
	    << ",X:"  << (int)m_X
	    << ",Y:"  << (int)m_Y
	    << ")";
	return msg.toString();

	__END_CATCH
}

