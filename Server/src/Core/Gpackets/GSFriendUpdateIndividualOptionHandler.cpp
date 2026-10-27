//----------------------------------------------------------------------
//
// Filename    : GSFriendUpdateIndividualOptionHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "GSFriendUpdateIndividualOption.h"
#include "Assert.h"

#ifdef __SHARED_SERVER__
	#include "GameServerManager.h"
	#include "FriendManager.h"

#endif

void GSFriendUpdateIndividualOptionHandler::execute ( GSFriendUpdateIndividualOption* pPacket, Player* pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __SHARED_SERVER__

	FriendListManager *pFriendListManager = g_pFriendManager->GetFriendListManager(pPacket->GetSender());
	if ( pFriendListManager != NULL )
	{
		FriendListManager::IndividualOption option;
		
		option.SetMemo(pPacket->GetFriendIndividualOptionInfo().GetMemo());
		
		pFriendListManager->UpdateIndividualOption(pPacket->GetFriendName(), option);
	}

#endif
		
	__END_CATCH
}
