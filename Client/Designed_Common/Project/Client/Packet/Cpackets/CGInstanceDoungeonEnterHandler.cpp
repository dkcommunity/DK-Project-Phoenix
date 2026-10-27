//////////////////////////////////////////////////////////////////////////////
// Filename    : CGInstanceDoungeonEnterHandler.cpp
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "CGInstanceDoungeonEnter.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "ZoneUtil.h"
	#include "ZoneInfoManager.h"
	#include "EventTransport.h"

	#include "DynamicZoneScript.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#if __CONTENTS(__TIPOJYU_CASTLE)
void CGInstanceDoungeonEnterHandler::execute (CGInstanceDoungeonEnter* pPacket , Player* pPlayer)
	     
{
	__BEGIN_TRY
	__BEGIN_DEBUG

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	try 
	{
		GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
		Assert(pGamePlayer != NULL);

		if (pGamePlayer->getPlayerStatus() == GPS_NORMAL) 
		{
			Creature* pCreature = pGamePlayer->getCreature();
			Assert(pCreature != NULL);
			
			std::string strScriptName = "Tiffauges/Tiffauges2F_R";

			DynamicZoneScript *pDynamicZoneScript = CreateInstanceDoungeon(strScriptName.c_str());
			if ( pDynamicZoneScript == NULL )
			{
				return;
			}
			
			EventTransport *pEventTransport = dynamic_cast<EventTransport*>(pGamePlayer->getEvent(Event::EVENT_CLASS_TRANSPORT));
			if ( pEventTransport == NULL )
			{
				pEventTransport = new EventTransport(pGamePlayer);
			}
			
			ZoneInfo *pZoneInfo = g_pZoneInfoManager->getZoneInfo(pDynamicZoneScript->getZoneID());
			
			if ( pZoneInfo != NULL )
			{
				pEventTransport->setDeadline(0);
				pEventTransport->setZoneName(pZoneInfo->getFullName());
				pEventTransport->setTargetZone(
					pDynamicZoneScript->getZoneID(), 
					pDynamicZoneScript->GetStartPosX(), 
					pDynamicZoneScript->GetStartPosY()
				);

				pGamePlayer->addEvent(pEventTransport);
			}
			
			// 파티원 동시 입장 처리
		}
	} 
	catch (Throwable & t) 
	{ 
		//std::cout << t.toString(); 
	}

#endif

	__END_DEBUG
    __END_CATCH
}

#endif // __TIPOJYU_CASTLE
