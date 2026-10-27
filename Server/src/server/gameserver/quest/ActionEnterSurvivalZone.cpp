////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionEnterSurvivalZone.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ActionEnterSurvivalZone.h"
#include "Zone.h"
#include "ZoneGroup.h"
#include "ZoneGroupManager.h"
#include "ZoneInfo.h"
#include "ZoneInfoManager.h"
#include "ZonePlayerManager.h"
#include "PCSlayerInfo2.h"
#include "PCVampireInfo2.h"
#include "PCOustersInfo2.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "WeatherManager.h"
#include "PaySystem.h"
#include "GamePlayer.h"
#include "IncomingPlayerManager.h"
#include "LogClient.h"
#include "PacketUtil.h"
#include "ZoneUtil.h"
#include "Properties.h"
#include "CastleInfoManager.h"
#include "StringStream.h"
#include "StringPool.h"
#include "EventSurvivalZoneManager.h"
#include "EffectHallucination.h"
#include "GQuestManager.h"

#include <stdio.h>

#include "Gpackets/GCUpdateInfo.h"
#include "Gpackets/GCMoveOK.h"
#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCModifyInformation.h"

#include "SystemAvailabilitiesManager.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void ActionEnterSurvivalZone::read (PropertyBuffer & pb)
{
    __BEGIN_TRY

	try 
	{
		m_ZoneID = pb.getPropertyInt("ZoneID");
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
void ActionEnterSurvivalZone::execute (Creature * pNPC , Creature * pCreature)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	Assert(pCreature != NULL);
	Assert(pCreature->isPC());

	bool bPayPlay = false;

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());
	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);

	Assert( pPC != NULL );

#if defined(__PAY_SYSTEM_ZONE__) || defined(__PAY_SYSTEM_FREE_LIMIT__)
//  if( pGamePlayer->isPayPlaying() || pGamePlayer->isFamilyFreePass() )
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//          20080227 유료존 티켓 추가하면서 코드 수정도 함께 한다.
//          기존 isPayPlaying() 은 모든 기간제 사용자는 다 체크하는 거라 유료존 티켓에는 맞지 않는다.
//          어차피 기간제 사용자가 프리미엄 사용자가 되기 땜시 isPremiumPlay로 바꾸고 유료존 티켓체크를 추가한다. isPayZoneTicket()
    if ( pGamePlayer->isPremiumPlay() || pGamePlayer->isPayZoneTicket() || pGamePlayer->isFamilyFreePass() )
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		bPayPlay = true;
	}
//	else
//	{
//		string connectIP = pGamePlayer->getSocket()->getHost();
//		if( pGamePlayer->loginPayPlay(connectIP, pGamePlayer->getID() ) )
//		{
//			sendPayInfo(pGamePlayer);
//			bPayPlay = true;
//		}
//	}
#else
	bPayPlay = true;
#endif

	GCNPCResponse	gcNPCResponse;
	Zone* pZone = getZoneByZoneID(m_ZoneID);
	EventSurvivalZoneManager* pEventSurvivalZoneManager = pZone->getEventSurvivalZoneManager();
	Assert( pEventSurvivalZoneManager != NULL );

	if( bPayPlay || pEventSurvivalZoneManager->getRaceSurvival() )
	{
//		Zone* pZone = getZoneByZoneID(m_ZoneID);

//		EventSurvivalZoneManager* pEventSurvivalZoneManager = pZone->getEventSurvivalZoneManager();
//		Assert( pEventSurvivalZoneManager != NULL );

		if( pEventSurvivalZoneManager->enterPC( pPC ) )
		{
			EffectHallucination* pEffectHallucination = new EffectHallucination(pCreature);
			pEffectHallucination->setDeadline(70*600);	// 1시간
			if( pPC->isSlayer() )
			{
				Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
				Assert( pSlayer != NULL );
				EffectManager*	pEffectManager = pSlayer->getEffectManager();
				Assert( pEffectManager != NULL );
				pEffectManager->addEffect(pEffectHallucination);
				pSlayer->setFlag(Effect::EFFECT_CLASS_HALLUCINATION);

				// 오토바이를 타고 있으면 오토바이에서 내린다.
				if( pSlayer->hasRideMotorcycle() )
				{
					pSlayer->getOffMotorcycle();
				}
			}
			else if( pPC->isVampire() )
			{
				Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
				Assert( pVampire != NULL );
				EffectManager*	pEffectManager = pVampire->getEffectManager();
				Assert( pEffectManager != NULL );
				pEffectManager->addEffect(pEffectHallucination);
				pVampire->setFlag(Effect::EFFECT_CLASS_HALLUCINATION);

				if( pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT) )
				{
					addUntransformCreature( pVampire->getZone(), pVampire, true );
				}
			}
			else if( pPC->isOusters() )
			{
				Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
				Assert( pOusters != NULL );
				EffectManager*	pEffectManager = pOusters->getEffectManager();
				Assert( pEffectManager != NULL );
				pEffectManager->addEffect(pEffectHallucination);
				pOusters->setFlag(Effect::EFFECT_CLASS_HALLUCINATION);

				// 실프를 소환 했으면 없애준다
				if( pOusters->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH) )
				{
					Effect* pEffect = pOusters->findEffect(Effect::EFFECT_CLASS_SUMMON_SYLPH);
					if( pEffect != NULL ) pEffect->setDeadline(0);
				}
			}
			transportCreature(pCreature, m_ZoneID, 75, 75, true);
		}
		else 
		{
			gcNPCResponse.setCode( NPC_RESPONSE_NOT_ENTER_SURVIVAL_ZONE );
			pGamePlayer->sendPacket( &gcNPCResponse );
		}
		return;
	}

	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setMessage( g_pStringPool->getString( STRID_CANNOT_ENTER_PAY_ZONE ) );
	pGamePlayer->sendPacket( &gcSystemMessage);

	__END_DEBUG
	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionEnterSurvivalZone::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionEnterSurvivalZone("
	    << "ZoneID:" << (int)m_ZoneID
	    << ")";
	return msg.toString();

	__END_CATCH
}

