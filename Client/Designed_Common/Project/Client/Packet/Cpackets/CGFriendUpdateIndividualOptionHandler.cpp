//////////////////////////////////////////////////////////////////////
//
// Filename    : CGFriendUpdateIndividualOptionHandler.cpp
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "CGFriendUpdateIndividualOption.h"
#include "VS_UI_GameCommon2.h"


#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "DarkAssert.h"
	#include "PlayerCreature.h"
	#include "SharedServerManager.h"

#endif

#if __CONTENTS(__FRIEND_ADDITION)
void CGFriendUpdateIndividualOptionHandler::execute ( CGFriendUpdateIndividualOption * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert( pGamePlayer != NULL );
    
    PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
    Assert( pPC != NULL );
	
    GSFriendUpdateIndividualOption gsFriendUpdateIndividualOption;
    
    gsFriendUpdateIndividualOption.SetSender(pPacket->getName());
    gsFriendUpdateIndividualOption.SetFriendName(pPacket->GetFriendName());
    gsFriendUpdateIndividualOption.SetFriendIndividualOptionInfo(pPacket->GetFriendIndividualOptionInfo());
    
    g_pSharedServerManager->sendPacket( &gsFriendUpdateIndividualOption );

#endif
	
	__END_CATCH
}
#endif //__FRIEND_ADDITION
