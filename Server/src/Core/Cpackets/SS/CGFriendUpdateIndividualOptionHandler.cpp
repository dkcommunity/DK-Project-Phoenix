//////////////////////////////////////////////////////////////////////
//
// Filename    : CGFriendUpdateIndividualOptionHandler.cpp
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "CGFriendUpdateIndividualOption.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "Assert.h"
	#include "PlayerCreature.h"
	#include "SharedServerManager.h"

	#include "Gpackets/GSFriendUpdateIndividualOption.h"

#endif

void CGFriendUpdateIndividualOptionHandler::execute ( CGFriendUpdateIndividualOption * pPacket , Player * pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert( pGamePlayer != NULL );
    
    PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
    Assert( pPC != NULL );
	
    GSFriendUpdateIndividualOption gsFriendUpdateIndividualOption;
    
    gsFriendUpdateIndividualOption.SetSender(pPC->getName());
    gsFriendUpdateIndividualOption.SetFriendName(pPacket->GetFriendName());
    gsFriendUpdateIndividualOption.SetFriendIndividualOptionInfo(pPacket->GetFriendIndividualOptionInfo());
    
    g_pSharedServerManager->sendPacket( &gsFriendUpdateIndividualOption );

#endif
	
	__END_CATCH
}
