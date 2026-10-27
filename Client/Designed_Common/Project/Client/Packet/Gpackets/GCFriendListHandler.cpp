//----------------------------------------------------------------------
//
// Filename    : GCFriendListHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCFriendList.h"
#include "VS_UI_Friend_System.h"
#include "MZoneTable.h"

#include "DarkAssert.h"

#ifdef __GAME_SERVER__

#endif

#if __CONTENTS(__FRIEND_ADDITION)
void GCFriendListHandler::execute ( GCFriendList* pPacket, Player* pPlayer )
	 
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

		// 리스트에 넣기전 현재 리스트를 모두 지운다.
		pFriend->GetListView()->DeleteAllItem(); 

		// 친구 정보를 목록에 추가
		for(int i = 0; i < pPacket->GetFriendListCount(); ++i)
		{
			FriendSimpleInfo* pInfo = pPacket->GetFriendInfo(i);
			if (pInfo)
			{
				int item_index = pFriend->GetListView()->GetItemSize();
				// 친구 이름
				pFriend->GetListView()->InsertColumnItem(pInfo->GetFriendName().c_str(),item_index,0);
				// 존 정보 저장
				std::string zone_id = "";
				if (g_pZoneTable->Get(pInfo->GetZoneID()) != NULL)
				{
					zone_id = g_pZoneTable->Get(pInfo->GetZoneID())->Name;
					if (!zone_id.empty())
						pFriend->GetListView()->InsertColumnItem(zone_id.c_str(),item_index,1);
					else
						pFriend->GetListView()->InsertColumnItem("Unknown",item_index,1); //by viva
				}
				else
					pFriend->GetListView()->InsertColumnItem("Unknown",item_index,1); //by viva
				// 접속 여부
				if (pInfo->GetConnect())
					pFriend->GetListView()->InsertColumnItem("ON",item_index,2);
				else
					pFriend->GetListView()->InsertColumnItem("OFF",item_index,2);

				// 서버 정보는 가지고만 있는다.
				pFriend->GetListView()->InsertColumnItem(pInfo->GetServerName().c_str(),item_index,3);
			}
		}

		// 옵션 정보 설정
		pFriend->SetCheckConfirm(pPacket->GetFriendGlobalOptionInfo().GetOptionToDenyAddRequest()^1);
	}
		
	__END_CATCH
}
#endif //__FRIEND_ADDITION
