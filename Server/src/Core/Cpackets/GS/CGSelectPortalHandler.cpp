//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectPortalHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGSelectPortal.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "Vampire.h"
	#include "Slayer.h"
	#include "Portal.h"
	#include "Zone.h"
	#include "ZoneUtil.h"
#endif	// __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSelectPortalHandler::execute(CGSelectPortal* pPacket , Player* pPlayer)
	 throw (Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	try 
	{
		GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
		if (pGamePlayer == NULL)
		{
			filelog("transportCreatureError.log", "[CGSelectPortalHandler] invalid player packetTarget=%d", (int)pPacket->getZoneID());
			return;
		}

		if (pGamePlayer->getPlayerStatus() != GPS_NORMAL)
		{
			filelog("transportCreatureError.log", "[CGSelectPortalHandler] player status not GPS_NORMAL status=%d packetTarget=%d", (int)pGamePlayer->getPlayerStatus(), (int)pPacket->getZoneID());
			return;
		}

		Creature* pCreature = pGamePlayer->getCreature();
		if (pCreature == NULL)
		{
			filelog("transportCreatureError.log", "[CGSelectPortalHandler] null creature packetTarget=%d", (int)pPacket->getZoneID());
			return;
		}

		Zone*     pZone     = pCreature->getZone();
		if (pZone == NULL)
		{
			filelog("transportCreatureError.log", "[CGSelectPortalHandler] null current zone player=%s packetTarget=%d", pCreature->getName().c_str(), (int)pPacket->getZoneID());
			return;
		}

		if (!isValidZoneCoord(pZone, pCreature->getX(), pCreature->getY()))
		{
			filelog("transportCreatureError.log", "[CGSelectPortalHandler] invalid current coord player=%s current=[%d,(%d,%d)] packetTarget=%d", pCreature->getName().c_str(), (int)pZone->getZoneID(), (int)pCreature->getX(), (int)pCreature->getY(), (int)pPacket->getZoneID());
			return;
		}

		Tile & rTile = pZone->getTile(pCreature->getX() , pCreature->getY());

		// 만약 포탈이 있으면서, 크리처가 PC인 경우.. (몬스터와 NPC는 포탈 이동을 하지 않는다.)
		if (rTile.hasPortal()) 
		{
			Portal* pPortal = rTile.getPortal();

			if (pPortal->getPortalClass() == PORTAL_CLASS_MULTI) 
			{
				if (pCreature->isSlayer()) 
				{
					Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
					if (!pSlayer->hasRideMotorcycle() && (pPortal->getObjectType() == PORTAL_SLAYER)) 
					{
						pPortal->activate(pCreature, pPacket->getZoneID());
					}
				} 
				else 
				{
					if (pPortal->getObjectType() == PORTAL_NORMAL || pPortal->getObjectType() == PORTAL_SLAYER) 
					{
						pPortal->activate(pCreature , pPacket->getZoneID());
					}
				}
			}
		}
	} 
	catch (Throwable & t) 
	{ 
		filelog("transportCreatureError.log", "[CGSelectPortalHandler] exception packetTarget=%d error=%s", (int)pPacket->getZoneID(), t.toString().c_str());
		return;
	}

#endif	// __GAME_SERVER__
		
	__END_DEBUG_EX __END_CATCH
}
