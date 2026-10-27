//----------------------------------------------------------------------
//
// Filename    : SGFriendResToAddHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "SGFriendResToAdd.h"
#include "Assert.h"

#ifdef __GAME_SERVER__
	#include "Creature.h"
	#include "Player.h"
	#include "PCFinder.h"

	#include "Gpackets/GCFriendResToAdd.h"

#endif

void SGFriendResToAddHandler::execute ( SGFriendResToAdd* pPacket, Player* pPlayer )
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
				
		GCFriendResToAdd gcFriendResToAdd;
		
		gcFriendResToAdd.SetFriendName( pPacket->GetFriendName());
		gcFriendResToAdd.SetResultCode( pPacket->GetResultCode());
		
		pPlayer->sendPacket(&gcFriendResToAdd);

	}
	
	__LEAVE_CRITICAL_SECTION((*g_pPCFinder))
	
#endif
		
	__END_CATCH
}
