//////////////////////////////////////////////////////////////////////
//
// Filename    : LCSelectPCErrorHandler.cpp
// Written By  : Reiot
// Description : 
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "LCSelectPCError.h"
#include "UIDialog.h"
#include "ClientDef.h"
#include "MGameStringTable.h"
#include "UIFunction.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void LCSelectPCErrorHandler::execute ( LCSelectPCError * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
	DEBUG_ADD("[Packet] LCSelectPCErrorHandler");

#ifdef __GAME_CLIENT__

	//std::cout << "Access Denied... (" << pPacket->toString() << ")" << std::endl;
	//throw ProtocolException("SelectPC Failed!");

//	g_pUIDialog->PopupFreeMessageDlg( pPacket->getMessage().c_str() );
	switch(pPacket->getCode())
	{
		case SELECT_PC_CANNOT_PLAY:
			UI_RunAskGoBilingPage(0);
//			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_SELECT_PC_CANNOT_PLAY].GetString());
			Sleep(1000);
			break;

		case SELECT_PC_NOT_BILLING_CHECK:
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_SELECT_PC_NOT_BILLING_CHECK].GetString());
			Sleep(1000);
			break;

		case SELECT_PC_CANNOT_PLAY_BY_ATTR:
//			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_SELECT_PC_CANNOT_PLAY_BY_ATTR].GetString());
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_LOGIN_ERROR_NONPK].GetString());
			//UI_RunAskGoBilingPage(1);
			Sleep(1000);
			break;

		case SELECT_PC_DIDNOT_AGREE :
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[UI_STRING_MESSAGE_CAN_NOT_CONNECT_AGREEMENT].GetString());
			Sleep( 1000 );
			break;
		case SELECT_PC_CANNOT_PLAY_BY_AGE :
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[UI_STRING_MESSAGE_CAN_NOT_CONNECT_AGREEMENT].GetString());
			Sleep( 1000 );
			break;	
	}
	SetMode(MODE_WAIT_SELECTPC);

#endif
		
	__END_CATCH
}
