//////////////////////////////////////////////////////////////////////
//
// Filename    : CGFriendUpdateGlobalOptionHandler.cpp
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "CGFriendUpdateGlobalOption.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "Assert.h"
	#include "PlayerCreature.h"
	#include "SharedServerManager.h"

	#include "Gpackets/GSFriendUpdateGlobalOption.h"
#endif

void CGFriendUpdateGlobalOptionHandler::execute ( CGFriendUpdateGlobalOption * pPacket , Player * pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert( pGamePlayer != NULL );
    
    PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
    Assert( pPC != NULL );

    GSFriendUpdateGlobalOption gsFriendUpdateGlobalOption;
    
    gsFriendUpdateGlobalOption.SetSender(pPC->getName());
    gsFriendUpdateGlobalOption.SetFriendGlobalOptionInfo(pPacket->GetFriendGlobalOptionInfo());
    
    g_pSharedServerManager->sendPacket( &gsFriendUpdateGlobalOption );
    

#endif
	
	__END_CATCH
}
