//----------------------------------------------------------------------
//
// Filename    : SGFriendUpdateConnectHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "SGFriendUpdateConnect.h"
#include "Assert.h"

#ifdef __GAME_SERVER__
	#include "Creature.h"
	#include "PCFinder.h"
	#include "Player.h"

	#include "Gpackets/GCFriendUpdateConnect.h"
#endif

void SGFriendUpdateConnectHandler::execute ( SGFriendUpdateConnect* pPacket, Player* pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	__ENTER_CRITICAL_SECTION((*g_pPCFinder))

	Creature* pCreature = g_pPCFinder->getCreature_LOCKED( pPacket->GetReceiver() );
	
	if ( pCreature != NULL && pCreature->isPC() )
	{
		Player* pPlayer = pCreature->getPlayer();
		Assert( pPlayer != NULL );

		GCFriendUpdateConnect gcFriendUpdateConnect;
		
		gcFriendUpdateConnect.SetPCName(pPacket->GetPCName());
		gcFriendUpdateConnect.SetConnect(pPacket->GetConnect());
		gcFriendUpdateConnect.SetServerName(pPacket->GetServerName());
		
		pPlayer->sendPacket(&gcFriendUpdateConnect);
	}
	
	__LEAVE_CRITICAL_SECTION((*g_pPCFinder))
	
#endif
		
	__END_CATCH
}
