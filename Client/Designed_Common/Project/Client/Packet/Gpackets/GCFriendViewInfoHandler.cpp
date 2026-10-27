//----------------------------------------------------------------------
//
// Filename    : GCFriendViewInfoHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCFriendViewInfo.h"
#include "VS_UI_Friend_System.h"
#include "MZoneTable.h"
#include "DarkAssert.h"

#ifdef __GAME_SERVER__

#endif

#if __CONTENTS(__FRIEND_ADDITION)
void GCFriendViewInfoHandler::execute ( GCFriendViewInfo* pPacket, Player* pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__


#endif

	if (pPacket)
	{
		// 친구 정보창을 가지고 온다. 없을 경우 생성
		C_VS_UI_FRIEND_INFO* pFriend = (C_VS_UI_FRIEND_INFO*)gpC_window_manager->GetWindowbyName("FriendInfo");
		if (!pFriend)
		{
			pFriend = new C_VS_UI_FRIEND_INFO;
			pFriend->SetWindowName("FriendInfo");
		}

		// 친구 정보를 저장한다.
		pFriend->SetUserName(pPacket->GetFriendDetailInfo().GetFriendName().c_str());
		pFriend->SetUserGrade((unsigned char)pPacket->GetFriendDetailInfo().GetRank());
		pFriend->SetUserLevel((unsigned char)pPacket->GetFriendDetailInfo().GetLevel());
		pFriend->SetUserAdvLevel((unsigned char)pPacket->GetFriendDetailInfo().GetAdvLevel());
		pFriend->SetUserDomain((unsigned char)pPacket->GetFriendDetailInfo().GetSkillDomainType());
		pFriend->SetUserSociety(pPacket->GetFriendDetailInfo().GetGuildName().c_str());
		pFriend->SetUserSocietyID(pPacket->GetFriendDetailInfo().GetGuildID());
		pFriend->SetUserPropensity(pPacket->GetFriendDetailInfo().GetAlignment());
		pFriend->SetUserContribute(pPacket->GetFriendDetailInfo().GetContributePoint());

		// 메모는 내용이 없으면 넣지 않는다.
		if (!pPacket->GetFriendIndividualOptionInfo().GetMemo().empty())
			pFriend->SetUserMemo(pPacket->GetFriendIndividualOptionInfo().GetMemo().c_str());

		// 존 정보 저장
		std::string zone_id = "";
		if (g_pZoneTable->Get(pPacket->GetFriendDetailInfo().GetZoneID()) != NULL)
		{
			zone_id = g_pZoneTable->Get(pPacket->GetFriendDetailInfo().GetZoneID())->Name;
			if (!zone_id.empty())
				pFriend->SetUserLocation(zone_id.c_str());
			else
				pFriend->SetUserLocation("Unknown");//알수 없음 by viva
		}
		else
			pFriend->SetUserLocation("Unknown");//알수 없음 by viva

		// 채널 정보는 친구 리스트에서 가지고 온다.
		// 이 형식은 그닥 좋지 않으므로 나중에 구조를 바꾸도록 한다.
		C_VS_UI_FRIEND_SYSTEM* pFriendSystem = (C_VS_UI_FRIEND_SYSTEM*)gpC_window_manager->GetWindowbyName("FriendSystem");
		if (pFriendSystem)
		{
			int index = pFriendSystem->GetListView()->GetItemIndexByName(pPacket->GetFriendDetailInfo().GetFriendName().c_str());
			if (index != -1 && index < pFriendSystem->GetListView()->GetItemSize())
			{
				std::string server = pFriendSystem->GetListView()->GetItemNameByIndex(index,3);
				pFriend->SetUserServer(server.c_str());
			}
		}

		pFriend->Start();
	}

	__END_CATCH
}
#endif //__FRIEND_ADDITION
