//----------------------------------------------------------------------
//
// Filename    : LCVersionCheckErrorHandler.cpp
// Written By  : Reiot
// Description : 
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "LCVersionCheckError.h"
#include "ClientDef.h"

extern BOOL		g_bNeedUpdate;

//----------------------------------------------------------------------
// 
//----------------------------------------------------------------------
void LCVersionCheckErrorHandler::execute ( LCVersionCheckError * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__

	
	// client를 update해야한다.
	g_bNeedUpdate = TRUE;
	g_ModeNext = MODE_QUIT;

#endif
		
	__END_CATCH
}
