//////////////////////////////////////////////////////////////////////////////
// Filename    : CGInstanceDoungeonEnterHandler.cpp
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGInstanceDoungeonEnter.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "Zone.h"
	#include "ZoneUtil.h"
	#include "NPC.h"

	#include "DynamicZoneScript.h"
	#include "InstanceDoungeonManager.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGInstanceDoungeonEnterHandler::execute (CGInstanceDoungeonEnter* pPacket , Player* pPlayer)
	     throw (Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
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
			
			Zone *pZone = pCreature->getZone();
			Creature* pNPCBase = NULL;

			pNPCBase = pZone->getCreature(pPacket->getObjectID()); 
			if (pNPCBase == NULL || !pNPCBase->isNPC())
			{
				return;
			}

			NPC* pNPC = dynamic_cast<NPC*>(pNPCBase);
			
			if ( pNPC->getInstanceDoungeonScript().length() > 0 )
			{
				
				DynamicZoneScript *pDynamicZoneScript = NULL;
					
				pDynamicZoneScript = InstanceDoungeonManager::CreateInstanceDoungeon(
					pNPC->getInstanceDoungeonScript().c_str(), 
					pPacket->getSelectDoungeonIndex()
				);
				
				if ( pDynamicZoneScript == NULL )
				{
					return;
				}
				
				pDynamicZoneScript->TransportCreature(pCreature);
			}
		}
	} 
	catch (Throwable & t) 
	{ 
		//cout << t.toString(); 
	}

#endif

	__END_DEBUG
    __END_DEBUG_EX __END_CATCH
}
