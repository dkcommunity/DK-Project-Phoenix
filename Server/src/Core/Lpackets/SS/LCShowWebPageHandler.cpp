//----------------------------------------------------------------------
//
// Filename    : LCShowWebPageHandler.cpp
// Written By  : Reiot
// Description : 
//
//----------------------------------------------------------------------

// include files
#include "LCShowWebPage.h"

#ifdef __GAME_CLIENT__
	#include "ClientPlayer.h"
	#include "Cpackets/CLGetPCList.h"
#endif

//----------------------------------------------------------------------
// 
//----------------------------------------------------------------------
void LCShowWebPageHandler::execute ( LCShowWebPage * pPacket , Player * pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_CLIENT__
#endif

	__END_DEBUG_EX __END_CATCH
}
