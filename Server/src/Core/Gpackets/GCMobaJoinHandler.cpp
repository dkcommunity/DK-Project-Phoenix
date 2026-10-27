//////////////////////////////////////////////////////////////////////////////
// Filename    : GCMobaJoinHandler.cpp
// Description : Server-side no-op handler for client packet class symmetry.
//////////////////////////////////////////////////////////////////////////////

#include "GCMobaJoin.h"

void GCMobaJoinHandler::execute(GCMobaJoin* pPacket, Player* pPlayer)
	throw(Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

	(void)pPacket;
	(void)pPlayer;

	__END_DEBUG_EX __END_CATCH
}
