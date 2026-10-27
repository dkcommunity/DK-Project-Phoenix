//////////////////////////////////////////////////////////////////////
//
// Filename    : LCRegisterPlayerOKHandler.cpp
// Written By  : Reiot
// Description : 
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "LCRegisterPlayerOK.h"
#include "UserInformation.h"
#include "ClientDef.h"
#include "UIFunction.h"
#include "login_socket_trace.h"

extern BOOL g_bNeedUpdate;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void LCRegisterPlayerOKHandler::execute ( LCRegisterPlayerOK * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__

	//std::cout << "플레이어 등록에 성공했습니당.. 추카.. " << std::endl;

	//throw DisconnectException("플레이어 등록에 성공했으니깐, 접속을 종료하고 다시 해랑..");
	// Debug Message

	if (!g_bNeedUpdate)
	{
		//------------------------------------------------------------
		// Basari sonucunu goster; pencere kisa sure sonra login ekranina doner.
		//------------------------------------------------------------
		UI_UserRegistrationSucceededMessage();
		// ID를 기억시켜둔다.
		const size_t userIDLength = g_pUserInformation->UserID.GetLength();
		const char* userID = g_pUserInformation->UserID.GetString();
		PhoenixRegistrationTrace("CLIENT REG_OK backup idLen=%u userIDPtr=%p",
			(unsigned int)userIDLength,
			(const void*)userID);
		if( userID != NULL && userIDLength > 0 && userIDLength < 13 )
			UI_BackupLoginID( g_pUserInformation->UserID );
		else
			UI_BackupLoginID( "DarkEden" );
		//------------------------------------------------------------
		// PC list'e hemen atlama; NewID sonuc penceresi gorunur kalmali.
		//------------------------------------------------------------
		g_ModeNext = MODE_NEWUSER;

		//------------------------------------------------------------
		// Gore Level을 바꾼다.
		//------------------------------------------------------------
		bool bGoreLevel = pPacket->isAdult() && !g_pUserOption->UseTeenVersion;

		SetGoreLevel( bGoreLevel);	
		
	}

#endif

	__END_CATCH
}
