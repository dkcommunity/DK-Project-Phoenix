//////////////////////////////////////////////////////////////////////
//
// Filename    : CGFriendReqToAddHandler.cpp
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "CGFriendReqToAdd.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "Assert.h"
	#include "PlayerCreature.h"
	#include "SharedServerManager.h"
	
	#include "Gpackets/GSFriendReqToAdd.h"

#endif

void CGFriendReqToAddHandler::execute ( CGFriendReqToAdd * pPacket , Player * pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert( pGamePlayer != NULL );
    
    PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
    Assert( pPC != NULL );
	
	GSFriendReqToAdd gsFriendReqToAdd;
	gsFriendReqToAdd.SetSender(pPC->getName());
	gsFriendReqToAdd.SetFriendName(pPacket->getPCName());
	
	g_pSharedServerManager->sendPacket( &gsFriendReqToAdd );

#endif
	
	__END_CATCH
}
