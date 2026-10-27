//----------------------------------------------------------------------
//
// Filename    : GCFriendReqToDelHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCFriendReqToDel.h"
#include "VS_UI_Friend_System.h"

#include "DarkAssert.h"

#ifdef __GAME_SERVER__

#endif

#if __CONTENTS(__FRIEND_ADDITION)
void GCFriendReqToDelHandler::execute ( GCFriendReqToDel* pPacket, Player* pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__


#endif
		
	if (pPacket)
	{
		// 친구 대기자 창을 가지고 온다. 없을 경우 생성
		C_VS_UI_FRIEND_WAIT* pFriend = (C_VS_UI_FRIEND_WAIT*)gpC_window_manager->GetWindowbyName("FriendWaitList");
		if (!pFriend)
		{
			pFriend = new C_VS_UI_FRIEND_WAIT;
			pFriend->SetWindowName("FriendWaitList");
		}

		// 대기 중인 친구 목록에서 해당 친구를 삭제한다.
		int index = pFriend->GetListView()->GetItemIndexByName(pPacket->getPCName().c_str());
		if (index != -1)
		{
			pFriend->GetListView()->DeleteColumnItem(pPacket->getPCName().c_str());
		}
	}

	__END_CATCH
}
#endif //__FRIEND_ADDITION
