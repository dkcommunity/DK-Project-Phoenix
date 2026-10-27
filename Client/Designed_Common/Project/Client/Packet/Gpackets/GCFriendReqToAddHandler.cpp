//----------------------------------------------------------------------
//
// Filename    : GCFriendReqToAddHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCFriendReqToAdd.h"
#include "VS_UI_Friend_System.h"
#include "types/FriendType.h"

#include "DarkAssert.h"

#ifdef __GAME_SERVER__

#endif

#if __CONTENTS(__FRIEND_ADDITION)
void GCFriendReqToAddHandler::execute ( GCFriendReqToAdd* pPacket, Player* pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__


#endif

	if (pPacket)
	{
		C_VS_UI_FRINEND_MESSAGE_BOX* pFriendPacket = (C_VS_UI_FRINEND_MESSAGE_BOX*)gpC_window_manager->GetWindowbyName("FriendRequestAddMessage");
		if (pFriendPacket)
		{
			if (pFriendPacket->Running())
			{
				pFriendPacket->Finish();
				DeleteNew(pFriendPacket);
			}
			else
			{
				DeleteNew(pFriendPacket);
			}
		}

		switch (pPacket->GetResultCode())
		{
		case FRIEND_REQ_TO_ADD_RESULT_CODE_SUCCESS:								// 추가 요청 성공
			pFriendPacket = new C_VS_UI_FRINEND_MESSAGE_BOX("Friend requests have been processed successfully.");
			pFriendPacket->SetWindowName("FriendRequestAddMessage");
			pFriendPacket->Start();
			break;
		case FRIEND_REQ_TO_ADD_RESULT_CODE_CAN_NOT_ADD_SELF:					// 자기 자신은 추가할 수 없음
			pFriendPacket = new C_VS_UI_FRINEND_MESSAGE_BOX("You can't add yourself as friend.");
			pFriendPacket->SetWindowName("FriendRequestAddMessage");
			pFriendPacket->Start();
			break;
		case FRIEND_REQ_TO_ADD_RESULT_CODE_EXCEED_MAX_LIST_COUNT_BY_SELF:		// 자신의 친구 최대 등록 개수 초과 
			break;
		case FRIEND_REQ_TO_ADD_RESULT_CODE_EXCEED_MAX_LIST_COUNT_BY_OTHER:		// 상대방의 친구 최대 등록 개수 초과
			pFriendPacket = new C_VS_UI_FRINEND_MESSAGE_BOX("This person can't add anymore friends.");
			pFriendPacket->SetWindowName("FriendRequestAddMessage");
			pFriendPacket->Start();
			break;
		case FRIEND_REQ_TO_ADD_RESULT_CODE_EXCEED_MAX_WAITLIST_COUNT_BY_OTHER:	// 상대방의 친구 최대 대기 목록 개수 초과
			pFriendPacket = new C_VS_UI_FRINEND_MESSAGE_BOX("The other side of the wait list is full.");
			pFriendPacket->SetWindowName("FriendRequestAddMessage");
			pFriendPacket->Start();
			break;
		case FRIEND_REQ_TO_ADD_RESULT_CODE_ALREADY_EXIST_FRIEND_IN_WAIT_LIST:	// 이미 대기목록에 존재함 
			pFriendPacket = new C_VS_UI_FRINEND_MESSAGE_BOX("Add a friend is already applied for a target.");
			pFriendPacket->SetWindowName("FriendRequestAddMessage");
			pFriendPacket->Start();
			break;
		case FRIEND_REQ_TO_ADD_RESULT_CODE_ALREADY_EXIST_FRIEND:				// 이미 추가된 친구
			pFriendPacket = new C_VS_UI_FRINEND_MESSAGE_BOX("The person is already in your friends list.");
			pFriendPacket->SetWindowName("FriendRequestAddMessage");
			pFriendPacket->Start();
			break;
		case FRIEND_REQ_TO_ADD_RESULT_CODE_NOT_EXIST_NAME:						// 존재하지 않는 이름임
			pFriendPacket = new C_VS_UI_FRINEND_MESSAGE_BOX("The name does not exist.");
			pFriendPacket->SetWindowName("FriendRequestAddMessage");
			pFriendPacket->Start();
			break;
		case FRIEND_REQ_TO_ADD_RESULT_CODE_DIFFERENT_RACE:						// 서로 다른 종족임
			pFriendPacket = new C_VS_UI_FRINEND_MESSAGE_BOX("Different race cannot be added as a friend.");
			pFriendPacket->SetWindowName("FriendRequestAddMessage");
			pFriendPacket->Start();
			break;
		case FRIEND_REQ_TO_ADD_RESULT_CODE_DENY_ADD_REQUEST:					// 친구 추가 요청을 거부함
			pFriendPacket = new C_VS_UI_FRINEND_MESSAGE_BOX("The friend request have been denied."); 
			pFriendPacket->SetWindowName("FriendRequestAddMessage");
			pFriendPacket->Start();
			break;
		}
	
		// 친구 대기자 창을 가지고 온다. 없을 경우 생성
		C_VS_UI_FRIEND_WAIT* pFriend = (C_VS_UI_FRIEND_WAIT*)gpC_window_manager->GetWindowbyName("FriendWaitList");
		if (!pFriend)
		{
			pFriend = new C_VS_UI_FRIEND_WAIT;
			pFriend->SetWindowName("FriendWaitList");
		}

		// 대기 중인 친구 목록에서 해당 친구를 삭제한다.
		int index = pFriend->GetListView()->GetItemIndexByName(pPacket->GetPCName().c_str());
		if (index != -1)
		{
			pFriend->GetListView()->DeleteColumnItem(pPacket->GetPCName().c_str());
		}
	}
	
	__END_CATCH
}
#endif //__FRIEND_ADDITION
