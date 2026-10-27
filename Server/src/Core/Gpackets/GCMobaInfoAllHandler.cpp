//////////////////////////////////////////////////////////////////////////////
// Filename    : GCMobaInfoAllHandler.cpp
// Description : Server-side no-op handler for client packet class symmetry.
//////////////////////////////////////////////////////////////////////////////

#include "GCMobaInfoAll.h"

void GCMobaInfoAllHandler::execute(GCMobaInfoAll* pPacket, Player* pPlayer)
	throw(Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

	(void)pPacket;
	(void)pPlayer;

	__END_DEBUG_EX __END_CATCH
}
