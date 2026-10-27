//----------------------------------------------------------------------
//
// Filename    : GCFriendDelHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCFriendDel.h"
#include "VS_UI_Friend_System.h"

#include "DarkAssert.h"

#ifdef __GAME_SERVER__

#endif

#if __CONTENTS(__FRIEND_ADDITION)
void GCFriendDelHandler::execute ( GCFriendDel* pPacket, Player* pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__


#endif
		
	// 친구 시스템 창을 가지고 온다. 없을 경우 생성
	C_VS_UI_FRIEND_SYSTEM* pFriend = (C_VS_UI_FRIEND_SYSTEM*)gpC_window_manager->GetWindowbyName("FriendSystem");
	if (!pFriend)
	{
		pFriend = new C_VS_UI_FRIEND_SYSTEM;
		pFriend->SetWindowName("FriendSystem");
	}
	
	// 친구 지우기
	if (!pPacket->GetPCName().empty())
	{
		pFriend->GetListView()->DeleteColumnItem(pPacket->GetPCName().c_str());
	}

	__END_CATCH
}
#endif //__FRIEND_ADDITION
