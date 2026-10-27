//----------------------------------------------------------------------
//
// Filename    : GCFriendUpdateZoneHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCFriendUpdateZone.h"
#include "VS_UI_Friend_System.h"
#include "MZoneTable.h"

#include "DarkAssert.h"

#ifdef __GAME_SERVER__

#endif

#if __CONTENTS(__FRIEND_ADDITION)
void GCFriendUpdateZoneHandler::execute ( GCFriendUpdateZone* pPacket, Player* pPlayer )
	 
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

		// 존 정보 저장
		int index = pFriend->GetListView()->GetItemIndexByName(pPacket->GetPCName().c_str());
		if (index != -1)
		{
			std::string zone_id = "";
			if (g_pZoneTable->Get(pPacket->GetZoneID()) != NULL)
			{
				zone_id = g_pZoneTable->Get(pPacket->GetZoneID())->Name;
				if (!zone_id.empty())
					pFriend->GetListView()->InsertColumnItem(zone_id.c_str(),index,1);
				else
					pFriend->GetListView()->InsertColumnItem("Unknown",index,1);//by viva
			}
			else
				pFriend->GetListView()->InsertColumnItem("Unknown",index,1); //by viva
		}
	}

	__END_CATCH
}
#endif //__FRIEND_ADDITION
