//----------------------------------------------------------------------
//
// Filename    : GSLogoutHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "GSLogout.h"
#include "Assert.h"

#ifdef __SHARED_SERVER__
	#include "GameServerManager.h"
	#include "FriendManager.h"

	#include "Spackets/SGFriendUpdateConnect.h"
#endif

//----------------------------------------------------------------------
// 
// GSLogoutHandler::execute()
// 
//----------------------------------------------------------------------
void GSLogoutHandler::execute ( GSLogout* pPacket, Player* pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __SHARED_SERVER__

	Assert( pPacket != NULL );
	
	g_pGameServerManager->RemovePlayer(pPacket->GetSender());
	
	FriendListManager *pFriendListManager = g_pFriendManager->GetFriendListManager(pPacket->GetSender());
	if ( pFriendListManager == NULL )
	{
		return;
	}
	
	TFriendList *pFriendList = &pFriendListManager->GetFriendFWList();
	
	if ( pFriendList != NULL )
	{
		TFriendList::iterator iter;
		string strFriendName;
		Player *pFriendPlayer;
		
		iter = pFriendList->begin();
		for(; iter != pFriendList->end() ; ++iter )
		{
			strFriendName = iter->second;
			
			pFriendPlayer = g_pGameServerManager->GetPlayer(strFriendName);
			if ( pFriendPlayer != NULL )
			{
				SGFriendUpdateConnect sgFriendUpdateConnect;
				
				sgFriendUpdateConnect.SetReceiver(strFriendName);
				sgFriendUpdateConnect.SetPCName(pPacket->GetSender());
				sgFriendUpdateConnect.SetConnect(false);
				sgFriendUpdateConnect.SetServerName("");
				
				g_pGameServerManager->broadcast( &sgFriendUpdateConnect, pFriendPlayer );
			}
		}
	}

	g_pFriendManager->RemoveFriendInfo(pPacket->GetSender());
	g_pFriendManager->UnloadFriendList(pPacket->GetSender());

#endif
		
	__END_DEBUG_EX __END_CATCH
}
