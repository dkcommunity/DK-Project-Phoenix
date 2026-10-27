//----------------------------------------------------------------------
//
// Filename    : LCShowWebPageHandler.cpp
// Written By  : Reiot
// Description : 
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "LCShowWebPage.h"
#include "UIDialog.h"
#include "VS_UI.h" 
#include "VS_UI_GameCommon.h"

#ifdef __GAME_CLIENT__
	#include "ClientPlayer.h"
	#include "Cpackets/CLGetPCList.h"
#endif

void g_StartShowWebPageMessage( int _x, int _y, char * szMsg, char * szUrl );

//----------------------------------------------------------------------
// 
//----------------------------------------------------------------------
void LCShowWebPageHandler::execute ( LCShowWebPage * pPacket , Player * pPlayer )
	 throw ( ProtocolException , Error )
{
	//__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_CLIENT__
		
	//char szBuf[1024];
	//sprintf(szBuf , pPacket->getMessage().c_str()) ; 
//	gC_vs_ui.RunPopupMessage(pPacket->getMessage().c_str(),
//		C_VS_UI_POPUP_MESSAGE::POPUP_SHOW_WEB_PAGE, pPacket->getURL().c_str()) ;
//	SetMode(MODE_WAIT_SERVER_LIST);

//	g_pUIDialog->PopupFreeMessageDlg( pPacket->getMessage().c_str() );

	g_StartShowWebPageMessage( -1, -1,
		const_cast<char *>( pPacket->getMessage().c_str() ),
		const_cast<char *>( pPacket->getURL().c_str() ) );
	
#endif

	//__END_DEBUG_EX __END_CATCH
}
