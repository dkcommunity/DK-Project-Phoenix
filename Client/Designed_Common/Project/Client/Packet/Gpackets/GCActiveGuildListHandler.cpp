//////////////////////////////////////////////////////////////////////
//
// Filename    : GCActiveGuildListHandler.cpp
// Written By  : 
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCActiveGuildList.h"
#include "ClientDef.h"
#include "UIFunction.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void GCActiveGuildListHandler::execute ( GCActiveGuildList * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__
	//std::cout << pPacket->toString() << std::endl; 
	//------------------------------------------------------
	// 검증
	//------------------------------------------------------
	if ( g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_NPC_ASK )
	{
		g_pPlayer->SetWaitVerifyNULL();

		DEBUG_ADD("[Verified] NPC Ask Answer OK");
	}


	UI_ShowActiveGuildList(pPacket);
#endif

	__END_CATCH
}
