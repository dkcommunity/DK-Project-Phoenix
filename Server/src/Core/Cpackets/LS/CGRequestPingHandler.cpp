//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestPingHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGRequestPing.h"
#include "Creature.h"
#include "NPC.h"
#include "GamePlayer.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"

#ifdef __GAME_SERVER__
	#include "SystemAvailabilitiesManager.h"
	#include "Assert.h"
	#include "StringPool.h"
	#include "DB.h"
	#include "ItemFactoryManager.h"
	#include "ItemInfoManager.h"
	#include "ItemUtil.h"
	#include "Gpackets/GCAddItemToItemVerify.h"
#endif	// __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGRequestPingHandler::execute (CGRequestPing* pPacket , Player* pPlayer)
	 throw (Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert( pGamePlayer != NULL );

	Creature* pCreature = pGamePlayer->getCreature();
	Assert( pCreature != NULL );

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Assert(pPC!=NULL);

	GCAddItemToItemVerify gcAddItemToItemVerify;
	gcAddItemToItemVerify.setCode( ADD_ITEM_TO_REQUEST_PING_OK );
	pGamePlayer->sendPacket( &gcAddItemToItemVerify );

	
#endif	// __GAME_SERVER__
		
	__END_DEBUG_EX __END_CATCH
}
