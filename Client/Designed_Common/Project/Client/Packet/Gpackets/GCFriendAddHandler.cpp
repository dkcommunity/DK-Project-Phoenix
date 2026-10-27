//----------------------------------------------------------------------
//
// Filename    : GCFriendAddHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCFriendAdd.h"
#include "VS_UI_Friend_System.h"
#include "MZoneTable.h"

#include "DarkAssert.h"

#ifdef __GAME_SERVER__

#endif

#if __CONTENTS(__FRIEND_ADDITION)
void GCFriendAddHandler::execute ( GCFriendAdd* pPacket, Player* pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__


#endif

	if (pPacket)
	{
		// 친구 시스템 창을 가지고 온다. 없을 경우 생성
		C_VS_UI_FRIEND_SYSTEM* pFriend = (C_VS_UI_FRIEND_SYSTEM*)gpC_window_manager->GetWindowbyName("FriendSystem");
		if (!pFriend)
		{
			pFriend = new C_VS_UI_FRIEND_SYSTEM;
			pFriend->SetWindowName("FriendSystem");
		}

		// 친구 정보를 목록에 추가
		FriendSimpleInfo pInfo = pPacket->GetFriendSimpleInfo();
		int item_index = pFriend->GetListView()->GetItemSize();
		// 친구 이름
		pFriend->GetListView()->InsertColumnItem(pInfo.GetFriendName().c_str(),item_index,0);
		// 존 정보 저장
		std::string zone_id = "";
		if (g_pZoneTable->Get(pInfo.GetZoneID()) != NULL)
		{
			zone_id = g_pZoneTable->Get(pInfo.GetZoneID())->Name.GetString();
			if (!zone_id.empty())
				pFriend->GetListView()->InsertColumnItem(g_pZoneTable->Get(pInfo.GetZoneID())->Name,item_index,1);
			else
				pFriend->GetListView()->InsertColumnItem("Unknown",item_index,1);//by viva
		}
		else
			pFriend->GetListView()->InsertColumnItem("Unknown",item_index,1);//by viva
		// 접속 여부
		if (pInfo.GetConnect())
			pFriend->GetListView()->InsertColumnItem("ON",item_index,2);
		else
			pFriend->GetListView()->InsertColumnItem("OFF",item_index,2);

		// 서버 정보는 가지고만 있는다.
		pFriend->GetListView()->InsertColumnItem(pInfo.GetServerName().c_str(),item_index,3);

		// 친구가 추가 되었다고 확인 메시지를 보낸다.
		C_VS_UI_FRINEND_MESSAGE_BOX* pFriendPacket = (C_VS_UI_FRINEND_MESSAGE_BOX*)gpC_window_manager->GetWindowbyName("FriendAddOkPopupMessage");
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
		pFriendPacket = new C_VS_UI_FRINEND_MESSAGE_BOX("Successfully add to your friends list.");//by viva
		pFriendPacket->SetWindowName("FriendAddOkPopupMessage");
		pFriendPacket->Start();
	}
	
	__END_CATCH
}
#endif //__FRIEND_ADDITION
