//----------------------------------------------------------------------
//
// Filename    : SGFriendSuggestToAddHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "SGFriendSuggestToAdd.h"
#include "Assert.h"

#ifdef __GAME_SERVER__
	#include "Creature.h"
	#include "Player.h"
	#include "PCFinder.h"

	#include "Gpackets/GCFriendSuggestToAdd.h"	
#endif

void SGFriendSuggestToAddHandler::execute ( SGFriendSuggestToAdd* pPacket, Player* pPlayer )
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
		
		GCFriendSuggestToAdd gcFriendSuggestToAdd;
				
		gcFriendSuggestToAdd.SetPCName(pPacket->GetFriendName());
				
		pPlayer->sendPacket(&gcFriendSuggestToAdd);
	}
	
	__LEAVE_CRITICAL_SECTION((*g_pPCFinder))
	
#endif
		
	__END_CATCH
}
