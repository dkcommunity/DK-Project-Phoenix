//----------------------------------------------------------------------
//
// Filename    : GCFriendWaitListHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCFriendWaitList.h"
#include "VS_UI_Friend_System.h"
#include "DarkAssert.h"

#ifdef __GAME_SERVER__

#endif

#if __CONTENTS(__FRIEND_ADDITION)
void GCFriendWaitListHandler::execute ( GCFriendWaitList* pPacket, Player* pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__


#endif

	// 대기자 목록 저장
	if (pPacket)
	{
		// 친구 대기자 창을 가지고 온다. 없을 경우 생성
		C_VS_UI_FRIEND_WAIT* pFriend = (C_VS_UI_FRIEND_WAIT*)gpC_window_manager->GetWindowbyName("FriendWaitList");
		if (!pFriend)
		{
			pFriend = new C_VS_UI_FRIEND_WAIT;
			pFriend->SetWindowName("FriendWaitList");
		}

		// 대기 중인 친구를 목록에 저장한다.
		for(int i = 0; i < pPacket->GetFriendWaitListCount(); ++i)
		{
			FriendWaitInfo* pInfo = pPacket->GetFriendWaitInfo(i);
			if (pInfo)
			{
				int item_index = pFriend->GetListView()->GetItemSize();
				// 친구 이름
				pFriend->GetListView()->InsertColumnItem(pInfo->GetFriendName().c_str(),item_index,0);
			}
		}
	}
		
	__END_CATCH
}
#endif //__FRIEND_ADDITION
