//----------------------------------------------------------------------
//
// Filename    : SGFriendReqToAddHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "SGFriendReqToAdd.h"
#include "Assert.h"

#ifdef __GAME_SERVER__
	#include "Creature.h"
	#include "Player.h"
	#include "PCFinder.h"

	#include "Gpackets/GCFriendReqToAdd.h"
#endif

void SGFriendReqToAddHandler::execute ( SGFriendReqToAdd* pPacket, Player* pPlayer )
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

		GCFriendReqToAdd gcFriendReqToAdd;
			    			
		gcFriendReqToAdd.SetPCName(pPacket->GetFriendName());
		gcFriendReqToAdd.SetResultCode(pPacket->GetResultCode());
		
		pPlayer->sendPacket(&gcFriendReqToAdd);
		
	}
	
	__LEAVE_CRITICAL_SECTION((*g_pPCFinder))
		
#endif
		
	__END_CATCH
}
