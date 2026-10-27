//////////////////////////////////////////////////////////////////////
//
// Filename    : CGFriendReqToDelHandler.cpp
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "CGFriendReqToDel.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "Assert.h"
	#include "PlayerCreature.h"
	#include "SharedServerManager.h"

	#include "Gpackets/GSFriendReqToDel.h"
#endif

void CGFriendReqToDelHandler::execute ( CGFriendReqToDel * pPacket , Player * pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__
	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert( pGamePlayer != NULL );
    
    PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
	Assert( pPC != NULL );
    
    GSFriendReqToDel gsFriendReqToDel;
    gsFriendReqToDel.SetSender(pPC->getName());
    gsFriendReqToDel.SetFriendName(pPacket->getPCName());
	
	g_pSharedServerManager->sendPacket( &gsFriendReqToDel );
    	
#endif
	
	__END_CATCH
}
