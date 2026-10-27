//----------------------------------------------------------------------
//
// Filename    : GSFriendReqToDelHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "GSFriendReqToDel.h"
#include "Assert.h"

#ifdef __SHARED_SERVER__
	#include "GameServerManager.h"
	#include "FriendManager.h"

	#include "Spackets/SGFriendDel.h"

#endif

void GSFriendReqToDelHandler::execute ( GSFriendReqToDel* pPacket, Player* pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __SHARED_SERVER__
	
	FriendListManager *pFriendListManager = NULL;
				
	pFriendListManager = g_pFriendManager->GetFriendListManager(pPacket->GetSender());
	if ( pFriendListManager != NULL )
	{
		if ( pFriendListManager->IsExistFriend(pPacket->GetFriendName()) )
		{
			g_pFriendManager->DelFriendInDB(pPacket->GetSender(), pPacket->GetFriendName());
			g_pFriendManager->DelFriendInDB(pPacket->GetFriendName(), pPacket->GetSender());
			
			// 내 친구 목록에 친구 존재
			pFriendListManager->DelFriend(pPacket->GetFriendName());
			
			// 요청자에게 결과 전송
			SGFriendDel sgFriendDel;
			sgFriendDel.SetReceiver(pPacket->GetSender());
			sgFriendDel.SetFriendName(pPacket->GetFriendName());

			g_pGameServerManager->broadcast( &sgFriendDel, pPlayer );
			
			// 친구가 접속중인 경우 삭제를 알림
			Player *pFriendPlayer = g_pGameServerManager->GetPlayer(pPacket->GetFriendName());
			if ( pFriendPlayer != NULL )
			{
				SGFriendDel sgFriendDel;
				sgFriendDel.SetReceiver(pPacket->GetFriendName());
				sgFriendDel.SetFriendName(pPacket->GetSender());

				g_pGameServerManager->broadcast( &sgFriendDel, pFriendPlayer );
			}
		}
		else
		{
			// 내 친구 목록에 친구 존재하지 않음
			cout << "GSFriendReqToDel Name=" << pPacket->GetSender() << "는 " << pPacket->GetFriendName() << " 없음" << endl;
		}
	}

#endif
		
	__END_CATCH
}
