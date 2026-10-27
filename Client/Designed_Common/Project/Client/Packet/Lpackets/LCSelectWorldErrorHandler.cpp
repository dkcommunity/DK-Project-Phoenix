//----------------------------------------------------------------------
//
// Filename    : LCSelectWorldErrorHandler.cpp
// Written By  : rappi76
// Description : 
//
//----------------------------------------------------------------------

// include files
#include "client_PCH.h"
#include "LCSelectWorldError.h"

#include "ClientDef.h"
#include "DEServerInformation.h"
#include "DebugInfo.h"
#include "UIFunction.h"

#include "MGameStringTable.h"
#include "UIDialog.h"

extern UIDialog*			g_pUIDialog;
#if __CONTENTS(__LC_SELECT_WORLD_ERROR)

//----------------------------------------------------------------------
// 
//----------------------------------------------------------------------
void LCSelectWorldErrorHandler::execute ( LCSelectWorldError * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY

	SetMode( MODE_WAIT_SELECT_WORLD );

	g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_ERROR_BLITZ_CONNECT_SERVER_ERROR].GetString(), -1,-1,UI_DIALOG_TITLE_OK, true);

	__END_CATCH
}
#endif //__LC_SELECT_WORLD_ERROR
