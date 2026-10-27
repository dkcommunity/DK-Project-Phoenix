//////////////////////////////////////////////////////////////////////
//
// Filename    : CGFriendResToAddHandler.cpp
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "CGFriendResToAdd.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "Assert.h"
	#include "PlayerCreature.h"

	#include "SharedServerManager.h"

	#include "Gpackets/GSFriendResToAdd.h"
#endif

void CGFriendResToAddHandler::execute ( CGFriendResToAdd * pPacket , Player * pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert( pGamePlayer != NULL );
    
    PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
    Assert( pPC != NULL );
    
	GSFriendResToAdd gsFriendResToAdd;
	gsFriendResToAdd.SetSender(pPC->getName());
	gsFriendResToAdd.SetFriendName(pPacket->GetPCName());
	gsFriendResToAdd.SetResponse(pPacket->GetResponse());
	
	g_pSharedServerManager->sendPacket( &gsFriendResToAdd );
	
#endif
	
	__END_CATCH
}
