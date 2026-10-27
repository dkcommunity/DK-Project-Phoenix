//----------------------------------------------------------------------
//
// Filename    : SGFriendWaitListHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "SGFriendWaitList.h"
#include "Assert.h"

#ifdef __GAME_SERVER__
	#include "Creature.h"
	#include "PCFinder.h"
	#include "Player.h"

	#include "Gpackets/GCFriendWaitList.h"
#endif

void SGFriendWaitListHandler::execute ( SGFriendWaitList* pPacket, Player* pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	GCFriendWaitList gcFriendWaitList;
	
	for(int i = 0; i < pPacket->GetFriendWaitListCount(); ++i )
	{
		FriendWaitInfo *pInfo;
		
		pInfo = pPacket->GetFriendWaitInfo(i);
		
		gcFriendWaitList.AddFriendWaitInfo(*pInfo);	
	}
	
	__ENTER_CRITICAL_SECTION((*g_pPCFinder))

	Creature* pCreature = g_pPCFinder->getCreature_LOCKED( pPacket->GetReceiver() );
	
	if ( pCreature != NULL && pCreature->isPC() )
	{
		Player* pPlayer = pCreature->getPlayer();
		Assert( pPlayer != NULL );

		pPlayer->sendPacket(&gcFriendWaitList);
	}
	
	__LEAVE_CRITICAL_SECTION((*g_pPCFinder))
	
#endif
		
	__END_CATCH
}
