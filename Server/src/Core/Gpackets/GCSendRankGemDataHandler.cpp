////////////////////////////////////////////////////////////////////////////////
// Filename    : GCSendRankGemDataHandler.cpp
// Written By  : rallser
// Description :
////////////////////////////////////////////////////////////////////////////////

// include files
#include "GCSendRankGemData.h"

void GCSendRankGemDataHandler::execute ( GCSendRankGemData * pPacket , Player * pPlayer )
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
