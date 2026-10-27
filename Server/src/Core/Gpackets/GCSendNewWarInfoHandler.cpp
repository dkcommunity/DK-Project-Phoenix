////////////////////////////////////////////////////////////////////////////////
// Filename    : GCSendNewWarInfoHandler.cpp
// Written By  : ±èŒº¹Î
// Description :
////////////////////////////////////////////////////////////////////////////////

// include files
#include "GCSendNewWarInfo.h"

void GCSendNewWarInfoHandler::execute ( GCSendNewWarInfo * pPacket , Player * pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
	__BEGIN_DEBUG
	
#ifdef __GAME_CLIENT__
	//cout << pPacket->toString() << endl;
#endif

	__END_DEBUG
	__END_DEBUG_EX __END_CATCH
}
