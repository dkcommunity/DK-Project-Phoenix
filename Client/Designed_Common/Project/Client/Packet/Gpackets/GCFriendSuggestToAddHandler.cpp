//----------------------------------------------------------------------
//
// Filename    : GCFriendSuggestToAddHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCFriendSuggestToAdd.h"
#include "VS_UI_Friend_System.h"

#include "DarkAssert.h"

#ifdef __GAME_SERVER__

#endif

#if __CONTENTS(__FRIEND_ADDITION)
unsigned char message_box_count = 0;

void GCFriendSuggestToAddHandler::execute ( GCFriendSuggestToAdd* pPacket, Player* pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__


#endif

	if (pPacket)
	{
		// 친구 추가로 인한 상대방 확인 메시지
		char window_name[128] = {0};
		sprintf(window_name,"FriendResName-%d",message_box_count);
		++message_box_count;
		C_VS_UI_FRINEND_CONFIRM* pFriend = (C_VS_UI_FRINEND_CONFIRM*)gpC_window_manager->GetWindowbyName(window_name);
		if (pFriend)
		{
			if (pFriend->Running())
			{
				pFriend->Finish();
				DeleteNew(pFriend);
			}
			else
			{
				DeleteNew(pFriend);
			}
		}

		std::string message;
		message = "Do you want to accept friend requests from  \'";
		message.append(pPacket->GetPCName().c_str());
		message.append("\' ?");

		pFriend = new C_VS_UI_FRINEND_CONFIRM(pPacket->GetPCName().c_str(),message.c_str());
		pFriend->SetWindowName(window_name);
		pFriend->Start();
	}
		
	__END_CATCH
}
#endif //__FRIEND_ADDITION
