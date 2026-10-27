//////////////////////////////////////////////////////////////////////
//
// Filename    : LCLoginErrorHandler.cpp
// Written By  : Reiot
// Description : 
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "LCLoginError.h"
#include "UIDialog.h"
#include "MGameStringTable.h"
#include "ClientDef.h"

extern void		PopupErrorMessage(ErrorID errorID);

extern BOOL		g_bNeedUpdate;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void LCLoginErrorHandler::execute ( LCLoginError * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__
	//20081224 크리스마스 이브 입니다. 반복수행 패킷 공격 방어 해봅시다.
	Sleep(2000);

	//std::cout << "Access Denied... (" << pPacket->toString() << ")" << std::endl;
	//throw ProtocolException("Login Failed!");
	// Debug Message

	if (!g_bNeedUpdate)
	{
		// 재입력
		//SetMode( MODE_MAINMENU );
		g_ModeNext = MODE_LOGIN_WRONG;

		// ETC_ERROR는 비정상적인 계정이라고 출력해야 한다.
//		if ((ErrorID)pPacket->getErrorID()==ETC_ERROR)
//		{
//			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_ERROR_LOGIN_DENY].GetString());
//		}
//		else if ((ErrorID)pPacket->getErrorID() == IP_DENYED )
//		{
//			g_pUIDialog->PopupFreeMessageDlg("You can't play this. Play your country version, Please.");
//		}
//		else
		{
			//g_UIDialog.PopupFreeMessageDlg( pPacket->getMessage().c_str() );
			PopupErrorMessage( (ErrorID)pPacket->getErrorID() );
		}
	}

#endif
		
	__END_CATCH
}
