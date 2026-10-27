//////////////////////////////////////////////////////////////////////
//
// Filename    : GCShowWebMarketHandler.cpp
// Written By  : 
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "Client.h"
#include "VS_UI.h"
#include "GCShowWebMarket.h"
#include "UIFunction.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void GCShowWebMarketHandler::execute ( GCShowWebMarket * pPacket , Player * pPlayer )
	 
{
	//__BEGIN_TRY __BEGIN_DEBUG_EX
	__BEGIN_TRY

#ifdef __GAME_CLIENT__

	//std::cout << pPacket->toString() << std::endl;


	std::string PlayerID = pPacket->getPlayerID() ;

	// Name
	std::string Name = pPacket->getName();

	// World ID
	WorldID_t WorldID = pPacket->getWorldID();

	DWORD Key = pPacket->getKey() ; 


	char cResult[1024] ; 
	wsprintf(cResult , "http://gmk.darkeden.com/?userid=%s&name=%s&world=%d&key=%u" , PlayerID.c_str() , Name.c_str() , WorldID ,  Key ) ; 
	UI_Run_WebBrowser(cResult) ; 
	//gC_vs_ui.RunWebBrowser(g_hWnd,cResult , g_hInstance) ;

#endif
	__END_CATCH
	//__END_DEBUG_EX __END_CATCH
}
