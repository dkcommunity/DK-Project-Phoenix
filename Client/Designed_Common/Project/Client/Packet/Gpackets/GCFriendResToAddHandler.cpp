//----------------------------------------------------------------------
//
// Filename    : GCFriendResToAddHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCFriendResToAdd.h"
#include "VS_UI_Friend_System.h"
#include "types/FriendType.h"

#include "DarkAssert.h"

#ifdef __GAME_SERVER__
	#include "Creature.h"
	#include "Player.h"
	#include "PCFinder.h"

	#include "Gpackets/GCFriendResToAdd.h"

#endif

#if __CONTENTS(__FRIEND_ADDITION)
void GCFriendResToAddHandler::execute ( GCFriendResToAdd* pPacket, Player* pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	
#endif

	if (pPacket)
	{
		C_VS_UI_FRINEND_MESSAGE_BOX* pFriendPacket = (C_VS_UI_FRINEND_MESSAGE_BOX*)gpC_window_manager->GetWindowbyName("FriendResultAddMessage");
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
		case FRIEND_RES_TO_ADD_RESULT_CODE_SUCCESS:												// 수락 요청 성공
			break;
		case FRIEND_RES_TO_ADD_RESULT_CODE_EXCEED_NOT_EXIST_IN_WAITLIST:						// 대기 목록에 존재하지 않음
			break;
		case FRIEND_RES_TO_ADD_RESULT_CODE_EXCEED_MAX_LIST_COUNT_BY_SELF:						// 자신의 친구 최대 등록 개수 초과 
			pFriendPacket = new C_VS_UI_FRINEND_MESSAGE_BOX("Waiting list is full.");//by viva
			pFriendPacket->SetWindowName("FriendResultAddMessage");
			pFriendPacket->Start();
			break;
		case FRIEND_RES_TO_ADD_RESULT_CODE_EXCEED_MAX_LIST_COUNT_BY_OTHER:						// 상대방의 친구 최대 등록 개수 초과
			pFriendPacket = new C_VS_UI_FRINEND_MESSAGE_BOX("The other side of the wait list is full.");//by viva
			pFriendPacket->SetWindowName("FriendResultAddMessage");
			pFriendPacket->Start();
			break;
		}
	}
		
	__END_CATCH
}
#endif //__FRIEND_ADDITION
