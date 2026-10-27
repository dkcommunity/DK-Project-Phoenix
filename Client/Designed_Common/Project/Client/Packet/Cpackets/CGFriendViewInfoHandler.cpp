//////////////////////////////////////////////////////////////////////
//
// Filename    : CGFriendViewInfoHandler.cpp
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "CGFriendViewInfo.h"
#include "VS_UI_GameCommon2.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "DarkAssert.h"
	#include "PlayerCreature.h"
	#include "PCFinder.h"

	#include "Gpackets/GCFriendViewInfo.h"

#endif

#if __CONTENTS(__FRIEND_ADDITION)
void CGFriendViewInfoHandler::execute ( CGFriendViewInfo * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert( pGamePlayer != NULL );
    
    PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(pGamePlayer->getCreature());
    Assert( pPC != NULL );
    
    GCFriendViewInfo gcFriendViewInfo;
    
    FriendDetailInfo info;
	
    info.SetFriendName("노과장님");
    info.SetRank(50);
    info.SetLevel(162);
    info.SetZoneID(12);
    info.SetGuildID(127);
    info.SetGuildName("기획길드");
    info.SetAlignment(5);
    info.SetContributePoint(5632);
    info.SetContributeRank(2);
    
    gcFriendViewInfo.SetFriendDetailInfo(info);
    gcFriendViewInfo.SetMemo("asldf asdlf asdflkjaskdjf");
	
	pGamePlayer->sendPacket(&gcFriendViewInfo);

#endif

	__END_CATCH
}
#endif //__FRIEND_ADDITION
