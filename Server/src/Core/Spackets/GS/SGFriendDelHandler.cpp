//----------------------------------------------------------------------
//
// Filename    : SGFriendDelHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "SGFriendDel.h"
#include "Assert.h"

#ifdef __GAME_SERVER__
	#include "Creature.h"
	#include "Player.h"
	#include "PCFinder.h"

	#include "Gpackets/GCFriendDel.h"
#endif

void SGFriendDelHandler::execute ( SGFriendDel* pPacket, Player* pPlayer )
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

		GCFriendDel gcFriendDel;
			    			
		gcFriendDel.SetPCName(pPacket->GetFriendName());
		
		pPlayer->sendPacket(&gcFriendDel);
	}
	
	__LEAVE_CRITICAL_SECTION((*g_pPCFinder))
		
#endif
		
	__END_CATCH
}
