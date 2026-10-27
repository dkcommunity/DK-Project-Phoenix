
//////////////////////////////////////////////////////////////////////
//
// Filename    : GCNotifyWinHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "GCNotifyWin.h"
#include "ClientDef.h"
#include "VS_UI.h"

//////////////////////////////////////////////////////////////////////
//
// 클라이언트에서 서버로부터 메시지를 받았을때 실행되는 메쏘드이다.
//
//////////////////////////////////////////////////////////////////////
void GCNotifyWinHandler::execute ( GCNotifyWin * pPacket , Player * pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY 
	
#ifdef __GAME_CLIENT__
	DWORD giftID = pPacket->getGiftID();
	gC_vs_ui.AddNotice( pPacket->getName().c_str(), giftID );
#endif

	__END_CATCH
}
