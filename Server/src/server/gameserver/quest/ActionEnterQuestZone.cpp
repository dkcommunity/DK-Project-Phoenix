////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionEnterQuestZone.cpp
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ActionEnterQuestZone.h"
#include "Properties.h"
#include "StringPool.h"
#include "Zone.h"
#include "ZoneInfo.h"
#include "ZoneInfoManager.h"
#include "ZoneUtil.h"
#include "ZoneGroup.h"
#include "ZoneGroupManager.h"
#include "PacketUtil.h"
#include "GamePlayer.h"
#include "PlayerCreature.h"
#include "DynamicZoneAlterOfBlood.h"
#include "DynamicZoneGroup.h"
#include "DynamicZoneManager.h"
#include "DynamicZoneInfo.h"
#include "GQuestManager.h"

#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCSystemMessage.h"


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void ActionEnterQuestZone::read (PropertyBuffer & pb)
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
void ActionEnterQuestZone::execute (Creature * pNPC , Creature * pCreature)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	Assert(pCreature != NULL);
	Assert(pCreature->isPC());

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());
	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);

	Assert( pPC != NULL );

	bool bTransport = true;
#if defined(__PAY_SYSTEM_ZONE__) || defined(__PAY_SYSTEM_FREE_LIMIT__)
	try {

		ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo( m_ZoneID );

		// 유료존인데 유료사용자가 아니면...
//		if (pZoneInfo==NULL
//			|| pZoneInfo->isPayPlay() && !(pGamePlayer->isPayPlaying() || pGamePlayer->isFamilyFreePass()))
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
//			else
//			{
				// 유료 서비스 사용 불가인 경우
				GCSystemMessage gcSystemMessage;

				if (g_pConfig->getPropertyInt("IsNetMarble")==0)
				{
					gcSystemMessage.setMessage( g_pStringPool->getString( STRID_CANNOT_ENTER_PAY_ZONE ) );
				}
				else
				{
					gcSystemMessage.setMessage( g_pStringPool->getString( STRID_CANNOT_ENTER ) );
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
		// Dynamic 존인지 확인.
		int targetDynamicZoneType = g_pDynamicZoneInfoManager->getDynamicZoneTypeByZoneID( m_ZoneID );

		if ( targetDynamicZoneType != DYNAMIC_ZONE_MAX )
		{
			// Dynamic 존일 경우
			DynamicZoneGroup* pDynamicZoneGroup = g_pDynamicZoneManager->getDynamicZoneGroup( targetDynamicZoneType );
			Assert( pDynamicZoneGroup != NULL );

			DynamicZone* pDynamicZone = pDynamicZoneGroup->getAvailableDynamicZone();
			Assert( pDynamicZone != NULL );

			transportCreature(pCreature, pDynamicZone->getZoneID(), m_X, m_Y, true);

			if ( targetDynamicZoneType == DYNAMIC_ZONE_ALTER_OF_BLOOD )
			{
				DynamicZoneAlterOfBlood* pAlterOfBlood = dynamic_cast<DynamicZoneAlterOfBlood*>(pDynamicZone);
				Assert( pAlterOfBlood != NULL );
				pAlterOfBlood->setRace( pPC->getRace() );
			}
		}
		else
		{
			// Dynamic 존이 아닐 경우
			transportCreature( pCreature, m_ZoneID, m_X, m_Y, true );
		}
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
string ActionEnterQuestZone::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionEnterQuestZone("
	    << "ZoneID:" << (int)m_ZoneID
	    << ",X:"  << (int)m_X
	    << ",Y:"  << (int)m_Y
	    << ")";
	return msg.toString();

	__END_CATCH
}

