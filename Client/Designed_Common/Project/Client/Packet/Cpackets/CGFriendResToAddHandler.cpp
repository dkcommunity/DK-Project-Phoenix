//////////////////////////////////////////////////////////////////////
//
// Filename    : CGFriendResToAddHandler.cpp
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "CGFriendResToAdd.h"
#include "VS_UI_GameCommon2.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "DarkAssert.h"

	#include "Gpackets/GCFriendReqToDel.h"
	#include "Gpackets/GCSystemMessage.h"
#endif

#if __CONTENTS(__FRIEND_ADDITION)
void CGFriendResToAddHandler::execute ( CGFriendResToAdd * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert( pGamePlayer != NULL );
    
	
	/////////////////////////////////////////////////////////////////////////////
	GCSystemMessage	gcSystemMessage;
	
	gcSystemMessage.setType(SYSTEM_MESSAGE_NORMAL);
	gcSystemMessage.setMessage(pPacket->toString());
	pGamePlayer->sendPacket(&gcSystemMessage);

#endif
	
	__END_CATCH
}
#endif //__FRIEND_ADDITION
