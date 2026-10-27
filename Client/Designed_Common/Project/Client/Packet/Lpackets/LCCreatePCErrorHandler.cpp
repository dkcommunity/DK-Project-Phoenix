//----------------------------------------------------------------------
//
// Filename    : LCCreatePCErrorHandler.cpp
// Written By  : Reiot
// Description : 
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "LCCreatePCError.h"
#include "ClientDef.h"

extern void		PopupErrorMessage(ErrorID errorID);

//----------------------------------------------------------------------
// 
//----------------------------------------------------------------------
void LCCreatePCErrorHandler::execute ( LCCreatePCError * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__

	//std::cout << "Fail to create PC... (" << pPacket->toString() << ")" << std::endl;
	//throw ProtocolException("CreatePC Failed!");

	//UI_NewCharacterCreateFailed( 0 );

	PopupErrorMessage( (ErrorID)pPacket->getErrorID() );


	g_ModeNext = MODE_WAIT_SELECTPC;

#endif
		
	__END_CATCH
}
