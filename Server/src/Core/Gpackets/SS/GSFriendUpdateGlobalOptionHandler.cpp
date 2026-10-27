//----------------------------------------------------------------------
//
// Filename    : GSFriendUpdateGlobalOptionHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "GSFriendUpdateGlobalOption.h"
#include "Assert.h"

#ifdef __SHARED_SERVER__
	#include "GameServerManager.h"
	#include "FriendManager.h"

#endif

void GSFriendUpdateGlobalOptionHandler::execute ( GSFriendUpdateGlobalOption* pPacket, Player* pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __SHARED_SERVER__
	
	FriendListManager *pFriendListManager = g_pFriendManager->GetFriendListManager(pPacket->GetSender());
	if ( pFriendListManager != NULL )
	{
		FriendListManager::GlobalOption option;
		
		option.SetAllowToRequestAddition(pPacket->GetFriendGlobalOptionInfo().GetAllowToRequestAddition());
		
		pFriendListManager->UpdateGlobalOption(option);
	}
	
#endif
		
	__END_CATCH
}
