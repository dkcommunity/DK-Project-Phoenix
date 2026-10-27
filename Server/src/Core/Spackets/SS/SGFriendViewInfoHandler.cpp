//----------------------------------------------------------------------
//
// Filename    : SGFriendViewInfoHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "SGFriendViewInfo.h"

#ifdef __GAME_SERVER__
	#include "Player.h"
	#include "PCFinder.h"
	#include "Assert.h"

	#include "Gpackets/GCFriendViewInfo.h"
#endif

void SGFriendViewInfoHandler::execute ( SGFriendViewInfo* pPacket, Player* pPlayer )
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
				
	    GCFriendViewInfo gcFriendViewInfo;
	    
	    gcFriendViewInfo.SetFriendDetailInfo(pPacket->GetFriendDetailInfo());
	    gcFriendViewInfo.SetFriendIndividualOptionInfo(pPacket->GetFriendIndividualOptionInfo());
		
	    pPlayer->sendPacket(&gcFriendViewInfo);
	}

	__LEAVE_CRITICAL_SECTION((*g_pPCFinder))
#endif
		
	__END_CATCH
}
