//----------------------------------------------------------------------
//
// Filename    : GSFriendReqToAddHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "GSFriendReqToAdd.h"
#include "Assert.h"

#ifdef __SHARED_SERVER__
	#include "types/FriendType.h"
	#include "GameServerManager.h"
	#include "FriendManager.h"

	#include "Spackets/SGFriendReqToAdd.h"
	#include "Spackets/SGFriendSuggestToAdd.h"

#endif

void GSFriendReqToAddHandler::execute ( GSFriendReqToAdd* pPacket, Player* pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __SHARED_SERVER__
	
	if ( pPacket->GetSender().compare(pPacket->GetFriendName()) == 0 )
	{
		// 자기 자신은 추가할 수 없음
		
		SGFriendReqToAdd sgFriendReqToAdd;
		sgFriendReqToAdd.SetReceiver(pPacket->GetSender());
		sgFriendReqToAdd.SetFriendName(pPacket->GetFriendName());
		sgFriendReqToAdd.SetResultCode(FRIEND_REQ_TO_ADD_RESULT_CODE_CAN_NOT_ADD_SELF);		
	
		g_pGameServerManager->broadcast( &sgFriendReqToAdd, pPlayer );
	
		return;
	}

	if ( !g_pFriendManager->IsExistName(pPacket->GetFriendName()) )
	{
		// 존재하지 않는 이름임
		
		SGFriendReqToAdd sgFriendReqToAdd;
		sgFriendReqToAdd.SetReceiver(pPacket->GetSender());
		sgFriendReqToAdd.SetFriendName(pPacket->GetFriendName());
		sgFriendReqToAdd.SetResultCode(FRIEND_REQ_TO_ADD_RESULT_CODE_NOT_EXIST_NAME);

		g_pGameServerManager->broadcast( &sgFriendReqToAdd, pPlayer );
		
		return;
	}

	Race_t RaceOfMine;
	Race_t RaceOfOther;
	
	RaceOfMine = g_pFriendManager->GetRace(pPacket->GetSender());
	RaceOfOther = g_pFriendManager->GetRace(pPacket->GetFriendName());
	
	if ( RaceOfMine != RACE_MAX && RaceOfMine != RaceOfOther )
	{
		// 서로 다른 종족임
		
		SGFriendReqToAdd sgFriendReqToAdd;
		sgFriendReqToAdd.SetReceiver(pPacket->GetSender());
		sgFriendReqToAdd.SetFriendName(pPacket->GetFriendName());
		sgFriendReqToAdd.SetResultCode(FRIEND_REQ_TO_ADD_RESULT_CODE_DIFFERENT_RACE);

		g_pGameServerManager->broadcast( &sgFriendReqToAdd, pPlayer );
		
		return;
	}

	if ( g_pFriendManager->IsExistFriendInDB(pPacket->GetSender(), pPacket->GetFriendName()) )
	{
		// 이미 추가된 친구
		
		SGFriendReqToAdd sgFriendReqToAdd;
		sgFriendReqToAdd.SetReceiver(pPacket->GetSender());
		sgFriendReqToAdd.SetFriendName(pPacket->GetFriendName());
		sgFriendReqToAdd.SetResultCode(FRIEND_REQ_TO_ADD_RESULT_CODE_ALREADY_EXIST_FRIEND);

		g_pGameServerManager->broadcast( &sgFriendReqToAdd, pPlayer );
		
		return;
	}
	
	FriendListManager *pFriendListManager = g_pFriendManager->GetFriendListManager(pPacket->GetSender());
		
	if ( pFriendListManager != NULL && (pFriendListManager->GetFriendFWList().size() > FRIEND_MAX_LIST_COUNT) ) 
	{
		// 자신의 친구 최대 등록 개수 초과 
		
		SGFriendReqToAdd sgFriendReqToAdd;
		sgFriendReqToAdd.SetReceiver(pPacket->GetSender());
		sgFriendReqToAdd.SetFriendName(pPacket->GetFriendName());
		sgFriendReqToAdd.SetResultCode(FRIEND_REQ_TO_ADD_RESULT_CODE_EXCEED_MAX_LIST_COUNT_BY_SELF);

		g_pGameServerManager->broadcast( &sgFriendReqToAdd, pPlayer );
			
		return;
	}
		
	if ( g_pFriendManager->IsExistFriendToWaitListInDB(pPacket->GetSender(), pPacket->GetFriendName()) )
	{
		// 이미 대기목록에 존재함
		
		SGFriendReqToAdd sgFriendReqToAdd;
		sgFriendReqToAdd.SetReceiver(pPacket->GetSender());
		sgFriendReqToAdd.SetFriendName(pPacket->GetFriendName());
		sgFriendReqToAdd.SetResultCode(FRIEND_REQ_TO_ADD_RESULT_CODE_ALREADY_EXIST_FRIEND_IN_WAIT_LIST);

		g_pGameServerManager->broadcast( &sgFriendReqToAdd, pPlayer );
		
		return;
	}
	
	if ( g_pFriendManager->IsExistFriendToWaitListInDB(pPacket->GetFriendName(), pPacket->GetSender()) )
	{
		// 내가 등록하려는 친구가 나의 대기목록에 존재함
		// 에러 코드는 수정할 필요가 있음
		
		SGFriendReqToAdd sgFriendReqToAdd;
		sgFriendReqToAdd.SetReceiver(pPacket->GetSender());
		sgFriendReqToAdd.SetFriendName(pPacket->GetFriendName());
		sgFriendReqToAdd.SetResultCode(FRIEND_REQ_TO_ADD_RESULT_CODE_ALREADY_EXIST_FRIEND_IN_WAIT_LIST);

		g_pGameServerManager->broadcast( &sgFriendReqToAdd, pPlayer );
		
		return;
	}

	if ( g_pFriendManager->GetFriendListCountInDB(pPacket->GetFriendName()) >= FRIEND_MAX_LIST_COUNT ) 
	{
		// 상대방의 친구 최대 등록 개수 초과 
		
		SGFriendReqToAdd sgFriendReqToAdd;
		sgFriendReqToAdd.SetReceiver(pPacket->GetSender());
		sgFriendReqToAdd.SetFriendName(pPacket->GetFriendName());
		sgFriendReqToAdd.SetResultCode(FRIEND_REQ_TO_ADD_RESULT_CODE_EXCEED_MAX_LIST_COUNT_BY_OTHER);

		g_pGameServerManager->broadcast( &sgFriendReqToAdd, pPlayer );
			
		return;
	}
	
	if ( g_pFriendManager->GetFriendWaitListCountInDB(pPacket->GetFriendName()) >= FRIEND_MAX_WAIT_LIST_COUNT )
	{
		// 상대방의 대기 목록 최대 개수 초과
		SGFriendReqToAdd sgFriendReqToAdd;
		sgFriendReqToAdd.SetReceiver(pPacket->GetSender());
		sgFriendReqToAdd.SetFriendName(pPacket->GetFriendName());
		sgFriendReqToAdd.SetResultCode(FRIEND_REQ_TO_ADD_RESULT_CODE_EXCEED_MAX_WAITLIST_COUNT_BY_OTHER);

		g_pGameServerManager->broadcast( &sgFriendReqToAdd, pPlayer );
		
		return;		
	}

	FriendListManager::GlobalOption option;
	option = g_pFriendManager->GetGlobalOption(pPacket->GetFriendName());
	
	if ( !option.GetAllowToRequestAddition() )
	{
		// 친구 추가 요청을 거부함
		
		SGFriendReqToAdd sgFriendReqToAdd;
		sgFriendReqToAdd.SetReceiver(pPacket->GetSender());
		sgFriendReqToAdd.SetFriendName(pPacket->GetFriendName());
		sgFriendReqToAdd.SetResultCode(FRIEND_REQ_TO_ADD_RESULT_CODE_DENY_ADD_REQUEST);		
	
		g_pGameServerManager->broadcast( &sgFriendReqToAdd, pPlayer );
	
		return;
	}
	
    g_pFriendManager->AddFriendToWaitListInDB(pPacket->GetSender(), pPacket->GetFriendName());
    
	// 상대방의 대기목록에 추가
	FriendListManager *pFriendListManagerByOther = g_pFriendManager->GetFriendListManager(pPacket->GetFriendName());
	if ( pFriendListManagerByOther != NULL )
	{
		pFriendListManagerByOther->AddFriendWaitList(pPacket->GetSender());
	}
	
	// 요청자에게 결과 전송
	SGFriendReqToAdd sgFriendReqToAdd;
	sgFriendReqToAdd.SetReceiver(pPacket->GetSender());
	sgFriendReqToAdd.SetFriendName(pPacket->GetFriendName());
	sgFriendReqToAdd.SetResultCode(FRIEND_REQ_TO_ADD_RESULT_CODE_SUCCESS);
	
	g_pGameServerManager->broadcast( &sgFriendReqToAdd, pPlayer );
	
	Player* pRequestedPlayer;
		
	pRequestedPlayer = g_pGameServerManager->GetPlayer(pPacket->GetFriendName());
	
	if ( pRequestedPlayer != NULL )
	{
		// 요청받는자에게 추가수락여부 전달
		SGFriendSuggestToAdd sgFriendSuggestToAdd;
		sgFriendSuggestToAdd.SetReceiver(pPacket->GetFriendName());
		sgFriendSuggestToAdd.SetFriendName(pPacket->GetSender());
		
		g_pGameServerManager->broadcast( &sgFriendSuggestToAdd, pRequestedPlayer );
	}

#endif
		
	__END_CATCH
}
