//--------------------------------------------------------------------------------
//
// Filename    : LCRegisterPlayerErrorHandler.cpp
// Written By  : Reiot
// Description : 
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "LCRegisterPlayerError.h"
#include "ClientDef.h"
#include "UIFunction.h"

extern bool		g_PhoenixRegistrationAlreadyExists;

extern BOOL		g_bNeedUpdate;

//--------------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------------
void LCRegisterPlayerErrorHandler::execute ( LCRegisterPlayerError * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__

	// message
	DEBUG_ADD_FORMAT("[ RegisterPlayerError ] %d", (int)pPacket->getErrorID() );
	
	
	//throw DisconnectException("RegisterPlayer Failed!");
	//g_UIDialog.PopupFreeMessageDlg( pPacket->getMessage().c_str() );
	if (!g_bNeedUpdate)
	{
		g_PhoenixRegistrationAlreadyExists = pPacket->getErrorID() == ALREADY_REGISTER_ID;
		UI_UserRegistrationFailedMessage();
		// NewIDCreate.spk owns this flow's visible result; a generic popup covers frames 6/8.
	}

	// 다시 UI를 기다리는 상태로 바꾼다.
	g_ModeNext = MODE_NEWUSER;

#endif
		
	__END_CATCH
}
