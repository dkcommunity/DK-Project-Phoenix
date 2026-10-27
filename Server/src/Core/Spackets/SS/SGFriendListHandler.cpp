//----------------------------------------------------------------------
//
// Filename    : SGFriendListHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "SGFriendList.h"
#include "Assert.h"

#ifdef __GAME_SERVER__
	#include "Creature.h"
	#include "PCFinder.h"
	#include "Player.h"

	#include "Gpackets/GCFriendList.h"
#endif

void SGFriendListHandler::execute ( SGFriendList* pPacket, Player* pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	GCFriendList gcFriendList;
	
	for(int i = 0; i < pPacket->GetFriendListCount(); ++i )
	{
		gcFriendList.AddFriendInfo(*pPacket->GetFriendSimpleInfo(i));	
	}
	gcFriendList.SetFriendGlobalOptionInfo(pPacket->GetFriendGlobalOptionInfo());
	
	__ENTER_CRITICAL_SECTION((*g_pPCFinder))

	Creature* pCreature = g_pPCFinder->getCreature_LOCKED( pPacket->GetReceiver() );
	
	if ( pCreature != NULL && pCreature->isPC() )
	{
		Player* pPlayer = pCreature->getPlayer();
		Assert( pPlayer != NULL );

		pPlayer->sendPacket(&gcFriendList);
	}
	
	__LEAVE_CRITICAL_SECTION((*g_pPCFinder))
	
	
	
#endif
		
	__END_CATCH
}
