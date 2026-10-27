//----------------------------------------------------------------------
//
// Filename    : GSFriendResToAddHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "GSFriendResToAdd.h"
#include "Assert.h"

#ifdef __SHARED_SERVER__
	#include "types/FriendType.h"
	#include "GameServerManager.h"
	#include "FriendManager.h"

	#include "Spackets/SGFriendAdd.h"
	#include "Spackets/SGFriendResToAdd.h"

#endif

void GSFriendResToAddHandler::execute ( GSFriendResToAdd* pPacket, Player* pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __SHARED_SERVER__

	// 대기 목록에 있는지 확인
	FriendListManager *pFriendListManager = g_pFriendManager->GetFriendListManager(pPacket->GetSender());
	
	if ( pFriendListManager == NULL)
	{
		return;
	}
	
	if ( !pFriendListManager->IsExistFriendWaitList(pPacket->GetFriendName()) )
	{
		// 대기 목록에 존재하지 않음
		
		SGFriendResToAdd sgFriendResToAdd;
		
		sgFriendResToAdd.SetReceiver(pPacket->GetSender());
		sgFriendResToAdd.SetFriendName(pPacket->GetFriendName());
		sgFriendResToAdd.SetResultCode(FRIEND_RES_TO_ADD_RESULT_CODE_EXCEED_NOT_EXIST_IN_WAITLIST);
		
		g_pGameServerManager->broadcast( &sgFriendResToAdd, pPlayer );
		
		return;
	}
	
	if ( pFriendListManager->GetFriendFWList().size() >= FRIEND_MAX_LIST_COUNT )
	{
		// 나의  친구 최대 목록 개수 초과
		
		SGFriendResToAdd sgFriendResToAdd;
				
		sgFriendResToAdd.SetReceiver(pPacket->GetSender());
		sgFriendResToAdd.SetFriendName(pPacket->GetFriendName());
		sgFriendResToAdd.SetResultCode(FRIEND_RES_TO_ADD_RESULT_CODE_EXCEED_MAX_LIST_COUNT_BY_SELF);
		
		g_pGameServerManager->broadcast( &sgFriendResToAdd, pPlayer );
		
		return;
	}
	
	if ( g_pFriendManager->GetFriendListCountInDB(pPacket->GetFriendName()) >= FRIEND_MAX_LIST_COUNT ) 
	{
		// 상대방의 친구 최대 등록 개수 초과 
		
		SGFriendResToAdd sgFriendResToAdd;
		
		sgFriendResToAdd.SetReceiver(pPacket->GetSender());
		sgFriendResToAdd.SetFriendName(pPacket->GetFriendName());
		sgFriendResToAdd.SetResultCode(FRIEND_RES_TO_ADD_RESULT_CODE_EXCEED_MAX_LIST_COUNT_BY_OTHER);

		g_pGameServerManager->broadcast( &sgFriendResToAdd, pPlayer );
			
		return;
	}
	
	pFriendListManager->DelFriendWaitList(pPacket->GetFriendName());
	
	// Sender는 요청을 받은 캐릭터 임
	g_pFriendManager->DelFriendToWaitListInDB(pPacket->GetFriendName(), pPacket->GetSender());	
	g_pFriendManager->DelFriendToWaitListInDB(pPacket->GetSender(), pPacket->GetFriendName());
	
	switch(pPacket->GetResponse())
	{
		case FRIEND_ADD_RESPONSE_CODE_ACCEPT :	// 친구 추가를 수락한 경우
			{
				g_pFriendManager->AddFriendInDB(pPacket->GetSender(), pPacket->GetFriendName());
				g_pFriendManager->AddFriendInDB(pPacket->GetFriendName(), pPacket->GetSender());
				
				Player* pSenderPlayer;
				Player* pFriendPlayer;
				
				pSenderPlayer = g_pGameServerManager->GetPlayer(pPacket->GetSender());
				if ( pSenderPlayer != NULL )
				{
					FriendListManager *pFriendListManager = NULL;
					
					pFriendListManager = g_pFriendManager->GetFriendListManager(pPacket->GetSender());
					if ( pFriendListManager != NULL )
					{
						pFriendListManager->AddFriend(pPacket->GetFriendName());
					}

					FriendInfo *pFriendInfo;
					pFriendInfo = g_pFriendManager->GetFriendInfo(pPacket->GetFriendName());
					
					// 수락한 캐릭터에게 친구추가 알림
					SGFriendAdd sgFriendAdd;
					FriendSimpleInfo info;
					
					info.SetFriendName(pPacket->GetFriendName());
					
					info.SetConnect(pFriendInfo->IsConnect());
					info.SetServerName(pFriendInfo->GetServerName());
					info.SetZoneID(pFriendInfo->GetZoneID());
					
					sgFriendAdd.SetReceiver(pPacket->GetSender());
					sgFriendAdd.SetFriendName(pPacket->GetFriendName());
					sgFriendAdd.SetFriendSimpleInfo(info);
					
					g_pGameServerManager->broadcast( &sgFriendAdd, pSenderPlayer );
				}
				
				pFriendPlayer = g_pGameServerManager->GetPlayer(pPacket->GetFriendName()); 
				if ( pFriendPlayer != NULL )
				{
					// 추가하려는 친구가 접속중이면
					
					FriendListManager *pFriendListManager = NULL;
					
					pFriendListManager = g_pFriendManager->GetFriendListManager(pPacket->GetFriendName());
					
					if ( pFriendListManager != NULL )
					{
						pFriendListManager->AddFriend(pPacket->GetSender());
					}
					
					FriendInfo *pFriendInfo;
					pFriendInfo = g_pFriendManager->GetFriendInfo(pPacket->GetSender());
										
					// 추가 요청했던 캐릭 터에게 친구추가 알림
					SGFriendAdd sgFriendAdd;
					FriendSimpleInfo info;
					
					info.SetFriendName(pPacket->GetSender());
					info.SetConnect(pFriendInfo->IsConnect());
					info.SetServerName(pFriendInfo->GetServerName());
					info.SetZoneID(pFriendInfo->GetZoneID());
					
					sgFriendAdd.SetReceiver(pPacket->GetFriendName());
					sgFriendAdd.SetFriendName(pPacket->GetSender());
					sgFriendAdd.SetFriendSimpleInfo(info);
					
					g_pGameServerManager->broadcast( &sgFriendAdd, pFriendPlayer );
				}
			}
			break;
		case FRIEND_ADD_RESPONSE_CODE_REJECT :	// 친구 추가를 거부한 경우
		default :
			break;
	}
	
	SGFriendResToAdd sgFriendResToAdd;
			
	sgFriendResToAdd.SetReceiver(pPacket->GetSender());
	sgFriendResToAdd.SetFriendName(pPacket->GetFriendName());
	sgFriendResToAdd.SetResultCode(FRIEND_RES_TO_ADD_RESULT_CODE_SUCCESS);
	
	g_pGameServerManager->broadcast( &sgFriendResToAdd, pPlayer );

#endif
		
	__END_CATCH
}
