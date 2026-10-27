//////////////////////////////////////////////////////////////////////
//
// Filename    : CGFriendViewInfoHandler.cpp
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "CGFriendViewInfo.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "Assert.h"
	#include "PlayerCreature.h"
	#include "SharedServerManager.h"

	#include "Gpackets/GSFriendViewInfo.h"

#endif

void CGFriendViewInfoHandler::execute ( CGFriendViewInfo * pPacket , Player * pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert( pGamePlayer != NULL );
    
    PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
    Assert( pPC != NULL );
	
	GSFriendViewInfo gsFriendViewInfo;
	
	gsFriendViewInfo.SetSender(pPC->getName());
	gsFriendViewInfo.SetFriendName(pPacket->GetPCName());
	g_pSharedServerManager->sendPacket( &gsFriendViewInfo );

#endif
	
	__END_CATCH
}
