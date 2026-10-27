//////////////////////////////////////////////////////////////////////
//
// Filename    : CGFriendUpdateGlobalOptionHandler.cpp
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "CGFriendUpdateGlobalOption.h"
#include "VS_UI_GameCommon2.h"


#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "DarkAssert.h"
	#include "PlayerCreature.h"
	#include "SharedServerManager.h"

#endif

#if __CONTENTS(__FRIEND_ADDITION)
void CGFriendUpdateGlobalOptionHandler::execute ( CGFriendUpdateGlobalOption * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert( pGamePlayer != NULL );
    
    PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
    Assert( pPC != NULL );
	
//	GSFriendReqToAdd gsFriendReqToAdd;
//	gsFriendReqToAdd.SetRequestName(pPC->getName());
//	gsFriendReqToAdd.SetRequestedName(pPacket->getPCName());
//	
//	g_pSharedServerManager->sendPacket( &gsFriendReqToAdd );
    
    std::cout << pPacket->toString() << std::endl;

#endif
	
	__END_CATCH
}
#endif //__FRIEND_ADDITION
